/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* main.c
*
* DESCRIPTION:
*       Initialization functions for the simulation on Unix-like OS
*
*
* FILE REVISION NUMBER:
*       $Revision: 25 $
*
*
*******************************************************************************/
#include <ctype.h>
#define __USE_BSD
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef CMD_LUA_CLI
#include <termios.h>
#endif
#include <unistd.h>
#include <errno.h>

#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsInit.h>
#include <gtOs/gtOsSharedMemory.h>
#include <gtStack/gtOsSocket.h>
#define CMDOS_NO_CMDCOMMON
#include <cmdShell/os/cmdOs.h>

#include <asicSimulation/SInit/sinit.h>
#include <os/simTypesBind.h>

#define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
#include <os/simOsBindOwn.h>

/* commandLine should belong to simulation (simulation can be used by another client withot this var) */
extern char commandLine[1280];
char **cmdArgv;
char **cmdEnv;

/* IntLockmutex should belong to simulation (simulation can be used without Enabler)! */
extern GT_MUTEX IntLockmutex;

extern int osSprintf(char * buffer, const char* format, ...);

#define BIND_FUNC(funcName)        \
                        simOsBindInfo.funcName = funcName

#define BIND_LEVEL_FUNC(level,funcName) \
            simOsBindInfo.level.funcName = funcName

SIM_OS_FUNC_BIND_STC simOsBindInfo;

static void simulationBindOsFuncs(void)
{
        /* reset all fields of simOsBindInfo */
        osMemSet(&simOsBindInfo,0,sizeof(simOsBindInfo));

        /* let the simulation OS bind it's own functions to the simulation */
        simOsFuncBindOwnSimOs();

        BIND_FUNC(osWrapperOpen);

        BIND_LEVEL_FUNC(sockets,osSocketTcpCreate);
        BIND_LEVEL_FUNC(sockets,osSocketUdpCreate);
        BIND_LEVEL_FUNC(sockets,osSocketTcpDestroy);
        BIND_LEVEL_FUNC(sockets,osSocketUdpDestroy);
        BIND_LEVEL_FUNC(sockets,osSocketCreateAddr);
        BIND_LEVEL_FUNC(sockets,osSocketDestroyAddr);
        BIND_LEVEL_FUNC(sockets,osSocketBind);
        BIND_LEVEL_FUNC(sockets,osSocketListen);
        BIND_LEVEL_FUNC(sockets,osSocketAccept);
        BIND_LEVEL_FUNC(sockets,osSocketConnect);
        BIND_LEVEL_FUNC(sockets,osSocketSetNonBlock);
        BIND_LEVEL_FUNC(sockets,osSocketSetBlock);
        BIND_LEVEL_FUNC(sockets,osSocketSend);
        BIND_LEVEL_FUNC(sockets,osSocketSendTo);
        BIND_LEVEL_FUNC(sockets,osSocketRecv);
        BIND_LEVEL_FUNC(sockets,osSocketRecvFrom);
        BIND_LEVEL_FUNC(sockets,osSocketSetSocketNoLinger);
        BIND_LEVEL_FUNC(sockets,osSocketExtractIpAddrFromSocketAddr);
        BIND_LEVEL_FUNC(sockets,osSocketGetSocketAddrSize);
        BIND_LEVEL_FUNC(sockets,osSocketShutDown);

        BIND_LEVEL_FUNC(sockets,osSelectCreateSet);
        BIND_LEVEL_FUNC(sockets,osSelectEraseSet);
        BIND_LEVEL_FUNC(sockets,osSelectZeroSet);
        BIND_LEVEL_FUNC(sockets,osSelectAddFdToSet);
        BIND_LEVEL_FUNC(sockets,osSelectClearFdFromSet);
        BIND_LEVEL_FUNC(sockets,osSelectIsFdSet);
        BIND_LEVEL_FUNC(sockets,osSelectCopySet);
        BIND_LEVEL_FUNC(sockets,osSelect);
        BIND_LEVEL_FUNC(sockets,osSocketGetSocketFdSetSize);

        /* this needed for binding the OS of simulation with our OS functions */
        simOsFuncBind(&simOsBindInfo);

        return;
}

void simulationStart (void)
{
    extern int userAppInit(void);

    char   semName[50];

    /* Must be called before use any of OS functions. */
    osWrapperOpen(NULL);
    /* create semaphores */

    osSprintf(semName, "IntLockmutex");

    /* create 'reentrant' mutex ! */
    if (osMutexCreate(semName, &IntLockmutex) != 0)
        exit (0);

    simulationBindOsFuncs();

#ifndef RTOS_ON_SIM
    /* call simulation init */
    simulationLibInit();
#endif
    /* init CMD */
    userAppInit();
}

extern GT_BOOL cmdStreamGrabSystemOutput;
int main(int argc, char * argv[], char *envp[])
{

    int i, len=0;
#ifdef CMD_LUA_CLI
    struct termios tc_saved;
#endif

#ifdef SHARED_MEMORY
    if(shrMemInitSharedLibrary_FirstClient() != GT_OK)
        return -1;
#endif

    cmdStreamGrabSystemOutput = GT_FALSE;
#ifdef CMD_LUA_CLI
    cmdDefaultShellType = CMD_STARTUP_SHELL_LUACLI_E;
    /* set cmdOsConfigFilePath to current directory */
    {
        char tmppath[4096];
        if (getcwd(tmppath, sizeof(tmppath)) == 0)
        {
            fprintf(stderr, "getcwd() : %s\n", strerror(errno));
        }
        strcat(tmppath, "/");
        cmdOsConfigFilePath = strdup(tmppath);
    }
#endif /* CMD_LUA_CLI */

    cmdArgv = calloc(argc+1, sizeof(char*));
    if (cmdArgv == NULL)
    {
        fprintf(stderr, "calloc(%d, %d) failed: %s\n",
                argc+1, sizeof(char*), strerror(errno));
        return 1;
    }
    cmdEnv = envp;
    for (i=0; i<argc; i++)
    {
        len += strlen(argv[i]);
        cmdArgv[i] = argv[i];

        if ((unsigned)len < sizeof(commandLine)-2) /* leave place for ' ' and '\0' */
        {
            strcat(commandLine,argv[i]);

            if (i<argc-1)
            {
                strcat(commandLine," ");
                len++;
            }
        }

        /* no need to define environment variable PSEUDO to cause simulation
         * print output on local console
         */
        /* new key "-tty" causes all input/output be on local console */
#if 0
        /* just ignore this option, always act as if it set */
        if (i > 0 && !strcmp(argv[i], "-tty"))
        {
        }
#endif

        /* new key equale to '-stdout comPort' */
        if (i > 0 && !strcmp(argv[i], "-grabstd"))
        {
            cmdStreamGrabSystemOutput = GT_TRUE;
        }

        /* support "-stdout" key like in window's simulation */
        if (i > 0 && (i+1 < argc) && !strcmp(argv[i], "-stdout"))
        {
            if (strcmp(argv[i+1], "comPort") == 0 ||
                    strcmp(argv[i+1], "comport") == 0)
            {
                cmdStreamGrabSystemOutput = GT_TRUE;
            }
        }

#ifdef CMD_LUA_CLI
        /* support "-cmdshell" key to set cmdShell starttup shell */
        if (strcmp("-cmdshell", argv[i]) == 0) {
            cmdDefaultShellType = CMD_STARTUP_SHELL_CMDSHELL_E;
            continue;
        }
        /* support "-config" key to enter the name of the configuration file */
        if (strcmp("-config", argv[i]) == 0) {
            i++;
            cmdOsConfigFileName = strdup(argv[i]);
            continue;
        }
#endif /* CMD_LUA_CLI */

    }
    /* all input/output be on local console */
    setenv("PSEUDO", "/dev/tty", 1);

    cmdArgv[argc] = 0;
    /* extend cmdArgv[0] to full path if it is relative */
    if (cmdArgv[0][0] != '/')
    {
        char argv0[4096];
        if (getcwd(argv0, sizeof(argv0)) == 0)
        {
            fprintf(stderr, "getcwd() : %s\n", strerror(errno));
        }
        strcat(argv0, "/");
        strcat(argv0, cmdArgv[0]);
        cmdArgv[0] = strdup(argv0);
    }

#ifdef CMD_LUA_CLI
    tcgetattr(0, &tc_saved);
#endif
    if (osStartEngine(argc, (const char**)argv, "appDemoSim", simulationStart) != GT_OK)
        return 1;
#ifdef CMD_LUA_CLI
    tcsetattr(0, TCSANOW, &tc_saved);
    printf("tty restored\n");
#endif

    return 0;
}

