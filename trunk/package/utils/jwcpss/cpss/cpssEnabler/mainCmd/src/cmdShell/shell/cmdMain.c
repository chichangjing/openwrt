/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdMain.c
*
* DESCRIPTION:
*       command shell entry point implementation
*
*
* DEPENDENCIES:
*       gtOs.h, gtOsEx.h
*
* FILE REVISION NUMBER:
*       $Revision: 31 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/common/cmdExtServices.h>

#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/shell/cmdMain.h>
#include <cmdShell/shell/cmdEngine.h>
#include <cmdShell/shell/cmdConsole.h>
#include <cmdShell/shell/cmdSystem.h>
#include <cmdShell/shell/cmdParser.h>
#include <cmdShell/shell/userInitCmdDb.h>
#include <cmdShell/os/cmdStreamImpl.h>
#include <cmdShell/FS/cmdFS.h>


#define MAX_NUM_UNITS 128
#define MSG_EXCHANGING_BASE_SOCKET_PORT 6000


#ifdef LINUX_SIM
    #include <string.h>
#endif

/* definition for Multi-TAPI unit switching */
#define CMD_SWITCHING_BASE_SOCKET_PORT  8000

/***** Variables *******************************************************/
static GT_32 globalInit = 0;

extern IOStreamPTR defaultStdio;

/* ports for Galtis command switching, must by initialized */
/* the value may be overridden                             */
GT_U16 cmdSwitchingServerPort = CMD_SWITCHING_BASE_SOCKET_PORT;

/* Unit to IP Mapping linked list */
/*STRUCT_LL_HEAD *cmdLlHead;*/

/* cpssInitSystem has been run */
extern GT_BOOL systemInitialized;

/* Unit is connected/not connected to Galtis */
#if (defined PSS_PRODUCT) || (defined CPSS_PRODUCT)
extern 
#else
static
#endif 
GT_BOOL terminalUnit;

/* for task thread safe */
GT_SEM parserMutex = 0;
GT_SEM serverMutex = 0;

GT_U32 taskSerial = 0;

/* Stdio sem -  only one stream can have the stdio */
GT_MUTEX  gStdioMtx;

/***** Public Functions ************************************************/

GT_STATUS tcpKeepAliveTask(
        IOStreamPTR stream
)
{
    char buf[100];
    int k;
    while (stream->connected(stream))
    {
        if ((k = stream->read(stream, buf, sizeof(buf))) > 0)
            stream->write(stream, buf, k);
    }
    stream->destroy(stream);
    return GT_OK;
}

/* extern */
GT_STATUS cmdEventLoop
(
    IOStreamPTR IOStream
);

GT_STATUS tcpServerInstance(
        IOStreamPTR stream
)
{
    /* start keepalive here */
    cmdEventLoop(stream);
    stream->destroy(stream);
    return GT_OK;
}

#ifdef CMD_LUA_CLI
GT_STATUS luaCLI_EventLoop
(
    IOStreamPTR IOStream
);
GT_STATUS luaCLI_tcpInstance(
        IOStreamPTR stream
)
{
    /* add telnet protocol layer */
    IOStreamPTR telnetStream;
    telnetStream = IOStreamCreateTelnet(stream);
    if (telnetStream == NULL)
    {
        stream->destroy(stream);
        return GT_FAIL;
    }

    luaCLI_EventLoop(telnetStream);
    telnetStream->destroy(telnetStream);
    return GT_OK;
}
GT_STATUS luaCLI_LoadConfigurationFile(
  IN  IOStreamPTR IOStream,
  OUT GT_VOID_PTR *luaInstancePtrPtr
);

#endif /* CMD_LUA_CLI */
#ifdef  CMD_FS
GT_STATUS cmdFileTransferServiceStart(void);
GT_STATUS cmdTFTPDstart(void);
#endif
#ifdef _WIN32
IOStreamPTR cmdStreamCreateWin32Console(void);
#endif

#ifdef CMD_INCLUDE_TCPIP
static const char* tcpServices[] = {
    "luaCLI",
    "cmdShell",
    NULL
};
#endif

/*******************************************************************************
* cmdInit
*
* DESCRIPTION:
*       commander initialization (entry point); this function spawns
*       configured streaming threads and immediately returns to caller.
*
* INPUTS:
*       devNum  - The device number of the serial port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_NO_RESOURCE     - failed to allocate memory for socket
*       GT_FAIL            - on error
*
* COMMENTS:
*
* For a new pemanent connection (like serial one) add the following: 
*   IOStreamPTR stream; 
*   ... 
*   stream = cmdStreamCreate....(); 
*   cmdStreamStartEventLoopHandler(...); 
*   
*   To accept tcp connections on second port: 
*       cmdStreamSocketServiceCreate(...);
*       cmdStreamSocketServiceStart(...);
*
*******************************************************************************/
GT_STATUS cmdInit
(
    IN  GT_U32  devNum
)
{
#ifdef CMD_INCLUDE_SERIAL
    GT_TASK taskSerial = 0;
#endif
    
    GT_STATUS retVal;

#ifndef PSS_PRODUCT
    /* initialize external services (can't call directly to mainOs functions)*/
    if(GT_OK != cmdInitExtServices())
    {
        return GT_FAIL;
    }
#endif

    #ifndef ASIC_SIMULATION
        cmdSwitchingServerPort = CMD_SWITCHING_BASE_SOCKET_PORT;
    #endif /*ASIC_SIMULATION*/

#if (defined PSS_PRODUCT) || (defined CPSS_PRODUCT)
    if (osWrapperOpen(NULL) != GT_OK)
    {   /* os wrapper failed to initialize */
        cmdOsPrintf("osWrapper initialization failure!\n");
        return GT_FAIL;
    }
#endif

    if (cmdStreamSocketInit() != GT_OK)
    {   /* os wrapper failed to initialize */
        cmdOsPrintf("cmdStreamSocketInit() initialization failure!\n");
        return GT_FAIL;
    }
#ifdef CMD_INCLUDE_SERIAL
    if (cmdStreamSerialInit() != GT_OK)
    {   /* os wrapper failed to initialize */
        cmdOsPrintf("cmdStreamSocketInit() initialization failure!\n");
        return GT_FAIL;
    }
#endif

    /* was before - osMutexCreate("parser", &parserMutex) */
    if (cmdParseInit(NULL) == GT_FAIL ||
        cmdOsMutexCreate("server", &serverMutex) != GT_OK)
    {
        cmdOsPrintf("failed to create mutex descriptors!\n");
        return GT_FAIL;
    }

    /* check if we already running */
    if (globalInit++)
        return GT_FAIL;
#ifdef IMPL_GALTIS
    /* initialize commander database */
    if (cmdCpssInitDatabase() != GT_OK)
    {
        cmdOsPrintf("failed to initialize command database!\n");
        return GT_FAIL;
    }
#endif      
    /* add test app level commands to commander database */
    if (cmdUserInitDatabase() != GT_OK)
    {
        cmdOsPrintf("failed to initialize command database!\n");
        return GT_FAIL;
    }
    if (cmdEngineInit() != GT_OK)
    {
            cmdOsPrintf("failed to initialize command engine!\n");
       return GT_FAIL;
    }
    terminalUnit = GT_TRUE;
    cmdStreamSocketServiceCreate(
        "CMD_Switching",
        "127.0.0.1", cmdSwitchingServerPort,
        tcpServerInstance,
        GT_FALSE/*multipleInstances*/,
        GT_FALSE/*cmdOsConsole*/);
    if ((retVal = cmdStreamSocketServiceStart("CMD_Switching")) == GT_OK)
    {
        /* do nothing */
    }
    else
    {
        /* If connection is requested, then commands are not executed directly
           on this unit */
        terminalUnit = GT_FALSE;
    }

    /* Allocate stdio semaphore */
    if (cmdOsMutexCreate("cmdStdioMtx", &gStdioMtx) != GT_OK)
    {
        cmdOsPrintf("failed to initialize stdio semaphore!\n");
        return GT_FAIL;
    }


#ifdef CMD_INCLUDE_SERIAL
    defaultStdio = cmdStreamCreateSerial(devNum);
    cmdTakeGlobalStdio(defaultStdio);
    cmdReleaseGlobalStdio();
#ifndef PSS_PRODUCT
    /* force cmdPrintf as cmdOsPrintf */
    cmdOsPrintf = cmdPrintf;
#endif /* PSS_PRODUCT */
#else
    defaultStdio = cmdStreamCreateNULL();
#endif /* CMD_INCLUDE_SERIAL */
#ifdef  _WIN32
    if (devNum == 0)
    {
        /* if appDemo started with -serial 0 use console */
        if (defaultStdio != NULL)
        {
            defaultStdio->destroy(defaultStdio);
        }
        defaultStdio = cmdStreamCreateWin32Console();
        cmdTakeGlobalStdio(defaultStdio);
        cmdReleaseGlobalStdio();
    }
#endif

    if (defaultStdio == NULL)
    {
        defaultStdio = IOStreamCreateNULL();
    }
    cmdStreamRedirectStdout(defaultStdio);

#if 0
#ifdef _WIN32
    {
        extern GT_U32 simulationInitReady;

        while(simulationInitReady == 0)
        {
            /* wait for simulation to be in operational state */
            cmdOsTimerWkAfter(500);
        }
    }
#endif /*_WIN32*/
#endif


#ifdef CMD_INCLUDE_TCPIP
#ifdef  CMD_LUA_CLI

    cmdStreamSocketServiceCreate(
            "luaCLI",
            NULL, 12345/*port*/,
            luaCLI_tcpInstance,
            GT_TRUE/*multipleInstances*/,
            GT_TRUE/*isConsole*/);
    /* initialize LUA CLI */
    if (cmdStreamSocketServiceStart("luaCLI") != GT_OK)
    {
        cmdOsPrintf("commander: failed to create LUA CLI listening task\n");
    }
#endif
#ifdef  CMD_FS
    cmdFSinit();
    cmdFileTransferServiceStart();
    cmdTFTPDstart();

#endif
    cmdStreamSocketServiceCreate(
            "cmdShell",
            NULL, CMD_SERVER_PORT,
            tcpServerInstance,
            GT_TRUE/*multipleInstances*/,
            GT_TRUE/*isConsole*/);
    if (cmdStreamSocketServiceStart("cmdShell") != GT_OK)
    {
        cmdOsPrintf("commander: failed to create tcpip server thread\n");
    }
    cmdStreamSocketServiceCreate(
            "TCPKEEPALIVE",
            NULL, CMD_KA_SERVER_PORT,
            tcpKeepAliveTask,
            GT_TRUE/*multipleInstances*/,
            GT_FALSE/*isConsole*/);
    if (cmdStreamSocketServiceStart("TCPKEEPALIVE") != GT_OK)
    {
        cmdOsPrintf("commander: failed to create tcpip keepalive task thread\n");
    }
#endif /* CMD_INCLUDE_TCPIP */

    {
        char services[80] = {0};
#ifdef CMD_INCLUDE_TCPIP
        int i;
        for (i = 0; tcpServices[i]; i++)
        {
            if (cmdStreamSocketServiceIsRunning(tcpServices[i]) == GT_TRUE)
            {
                cmdOsStrCat(services, " ");
                cmdOsStrCat(services, tcpServices[i]);
            }
        }
#endif
        if (taskSerial)
        {
            cmdOsStrCat(services, " SERIAL");
        }
        if (!services[0])
        {
            cmdOsStrCpy(services, " NONE");
        }
        cmdOsPrintf("commander: threads are running:%s\n\n", services);
    }

#if 0
    /* initialize remote ip detection thread */
    if (cmdRemoteIpDetectionServerCreate() != GT_OK)
    {
        cmdOsPrintf("commander: fail to create remoteip detection server thread\n");
    }
#endif

#ifdef CMD_LUA_CLI
    luaCLI_LoadConfigurationFile(
#ifdef CMD_INCLUDE_SERIAL
            defaultStdio
#else
            NULL
#endif
            , &(defaultStdio->customPtr));

#endif

#ifdef CMD_INCLUDE_SERIAL
    defaultStdio->flags |= IO_STREAM_FLAG_C_FIRSTSHELL;
    /* initialize serial server thread */
    if (cmdStreamStartEventLoopHandler(
                "SERIAL",
                cmdEventLoop,
                defaultStdio,
                GT_TRUE/*isConsole*/,
                &taskSerial) != GT_OK)
    {
        cmdOsPrintf("commander: failed to spawn serial thread\n");
        taskSerial = 0;
    }
#endif /* CMD_INCLUDE_SERIAL */


    return GT_OK;
}
