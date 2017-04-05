/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdMultiTapi.c
*
* DESCRIPTION:
*       MultiTapi implementation
*
*
* DEPENDENCIES:
*       gtOs.h, gtOsEx.h
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <appDemo/multiTapi/appDemoMultiTapi.h>
#include <cmdShell/shell/cmdMultiTapi.h>
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/os/cmdStream.h>

/* definition for Multi-TAPI unit switching */
#define CMD_SWITCHING_BASE_SOCKET_PORT  8000

/* parameter for command execution on all the system units */
#define ALL_UNITS 255

/***** Variables *******************************************************/

/* indicator whether units to IP mapping database has been initiazlized*/
static GT_BOOL isLinkedListInitialized;

/* Unit is connected/not connected to Galtis */
GT_BOOL terminalUnit;

/* ports for Galtis command switching, must by initialized */
/* the value may be overridden                             */
extern GT_U16 cmdSwitchingServerPort;

/* Unit to IP Mapping linked list */
STRUCT_LL_HEAD *cmdLlHead;

/* client sockets for Galtis commands switching */
cmdStreamPTR cmdSocketId[MAX_NUM_UNITS];

/* gtInitSystem has been run */
extern GT_BOOL systemInitialized;

/* Galtis commands are directed to this unit */
GT_U32 cmdActiveUnit = DEFAULT_UNIT;


#ifndef DUNE_FE_IMAGE
/* ports for messages exchanging */
extern GT_U16 appDemoMsgExchangingServerPort;
#endif

/***** Public Functions ************************************************/

static GT_STATUS createMultiTapiClient
(
    IN GT_IPADDR      serverIpAddr,
    IN GT_U8          unitNum
);

CMD_STATUS sendCommandOnSocket
(
    GT_U8 *inputBufferI,
    GT_U8 *fieldBufferI,
    GT_U32 unit,
    GT_U8 **outputBuffer
);

/*******************************************************************************
* syncUnitsDatabase
*
* DESCRIPTION:
*       Synchronize the Unit to IP mapping of all the units.
*
* INPUTS:
*       serverIpAddr - server IP address.
*       unitNum      - unit number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success,
*       GT_NO_RESOURCE     - failed to allocate memory for socket,
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
static GT_STATUS syncUnitsDatabase
(
    IN GT_IPADDR      serverIpAddr,
    IN GT_U32         unitNum
)
{
    GT_U8     *msg;            /* holds the message to send */
    GT_U8     i;               /* loop index */
    GT_IPADDR ipAddr;          /* IP address to send in the message */
    GT_BOOL failed = GT_FALSE; /* connection with server failed */
    GT_U8 *outputBuffer = NULL; /* Galtis output */

    /* Don't synchronize if commands are not executed directly on the unit */
    if (terminalUnit == GT_FALSE)
    {
        return GT_OK;
    }

    msg = (GT_U8*)cmdOsMalloc (CMD_MAX_BUFFER);
    if (msg == NULL)
    {
        return GT_NO_RESOURCE;
    }
    /* Send new IP address to the other units */
    for (i = 0; i < MAX_NUM_UNITS; i++)
    {
        if ((i == DEFAULT_UNIT) || (i == unitNum) ||
            (cmdGetIpForUnit (i, &ipAddr) == GT_NOT_FOUND))
            continue;
        /* prepare message for sending */
        cmdOsSprintf (msg, "cmdSetIpForUnit %d, \"%08x\"\r\n", unitNum,
                   cmdOsHtonl(serverIpAddr.u32Ip));
        if (sendCommandOnSocket (msg, "\0", i, &outputBuffer) != CMD_OK)
        {
            failed = GT_TRUE; /*connection with the other unit failed */
        }
    }
    /* Send units database to the new unit */
    if (unitNum != DEFAULT_UNIT)
    {
        for (i = 0; i < MAX_NUM_UNITS; i++)
        {
            if (cmdGetIpForUnit (i, &ipAddr) == GT_NOT_FOUND)
                continue;
            /* prepare message for sending */
            cmdOsSprintf (msg, "cmdSetIpForUnit %d, \"%08x\"\r\n", i,
                       cmdOsHtonl(ipAddr.u32Ip));
            if (sendCommandOnSocket (msg, "\0", unitNum, &outputBuffer)
                != CMD_OK)
            {
                failed = GT_TRUE; /*connection with the other unit failed */
            }
        }
    }
    cmdOsFree (msg);

    if (failed)
    {
        return GT_FAIL;
    }
    return GT_OK;
}

/*******************************************************************************
* createMultiTapiClient
*
* DESCRIPTION:
*       Create TCP socket as client and connect to the new unit server.
*       Synchronize units DB with all the servers.
*
* INPUTS:
*       serverIpAddr - server IP address.
*       unitNum      - unit number.
*
* OUTPUTS:
*       socketId - socket id.
*
* RETURNS:
*       GT_OK              - on success,
*       GT_NO_RESOURCE     - failed to allocate memory for socket,
*       GT_FAIL            - on error.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
static GT_STATUS createMultiTapiClient
(
    IN GT_IPADDR  serverIpAddr,   /* server IP address */
    IN GT_U8      unitNum         /* new unit number */
)
{
    struct sockaddr_in  *remoteSockAddr;
    GT_SIZE_T           remoteSockAddrLen;
    GT_8                dottedIpAddr[16];
    GT_STATUS           retVal;
    GT_U32              serverPort;
    GT_SOCKET_FD        socket;

    /* Don't create client if commands are not executed directly on the unit */
    if (terminalUnit == GT_FALSE)
    {
        return GT_OK;
    }

    /* create TCP socket */
    if ((socket = cmdOsSocketTcpCreate(GT_SOCKET_DEFAULT_SIZE)) < 0)
    {
        return GT_FAIL;
    }
    cmdOsInetNtoa (cmdOsHtonl(serverIpAddr.u32Ip), dottedIpAddr);
    /* for WM socket port is different */
#ifdef ASIC_SIMULATION
    serverPort = CMD_SWITCHING_BASE_SOCKET_PORT + ((unitNum > 0) ? unitNum : 0);
#else
    serverPort = cmdSwitchingServerPort;
#endif
    /* Build TCP/IP address to be used across all other functions */
    if ((retVal = cmdOsSocketCreateAddr (dottedIpAddr, serverPort,
                  (GT_VOID**)&remoteSockAddr, &remoteSockAddrLen)) != GT_OK)
    {
        return retVal;
    }
    /* set socket to blocking mode */
    if ((retVal = cmdOsSocketSetBlock (socket)) != GT_OK)
    {
        return retVal;
    }
    /* connect to the other unit */
    if ((retVal = cmdOsSocketConnect (socket, remoteSockAddr,
                                   remoteSockAddrLen)) != GT_OK)
    {
        return retVal;
    }

    cmdSocketId[unitNum] = cmdStreamCreateSocket(socket);

    if (cmdSocketId[unitNum] == NULL)
    {
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* compUnitNum
*
* DESCRIPTION:
*       Compare two units LL data nodes by their unit number.
*
* INPUTS:
*       node1  - pointer to first unit data node
*       node2  - pointer to second unit data node
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_LL_EQUAL    - if data is equal,
*       GT_LL_GREATER  - if second node should be before first one in LL,
*       GT_LL_SMALLER  - if second node should be after first one in LL
*
* COMMENTS:
*
*******************************************************************************/
static GT_LL_COMP_RES compUnitNum
(
    IN  GT_UNIT_IP_INFO_STC* node1,
    IN  GT_UNIT_IP_INFO_STC* node2
)
{
    if((node1 == NULL) && (node2 == NULL))
        return GT_LL_EQUAL;

    if((node1 == NULL) && (node2 != NULL))
        return GT_LL_SMALLER;

    if((node1 != NULL) && (node2 == NULL))
        return GT_LL_GREATER;

    if(node1->unitNum == node2->unitNum)
        return GT_LL_EQUAL;

    if (node1->unitNum < node2->unitNum)
        return GT_LL_SMALLER;

    return GT_LL_GREATER;
}

#ifndef ASIC_SIMULATION
/*******************************************************************************
* compIpAddr
*
* DESCRIPTION:
*       Compare two units LL data nodes by their IP addr.
*
* INPUTS:
*       node1  - pointer to first unit data node
*       node2  - pointer to second unit data node
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_LL_EQUAL    - if data is equal,
*       GT_LL_GREATER  - if second node should be before first one in LL,
*       GT_LL_SMALLER  - if second node should be after first one in LL
*
* COMMENTS:
*
*******************************************************************************/
static GT_LL_COMP_RES compIpAddr
(
    IN  GT_UNIT_IP_INFO_STC* node1,
    IN  GT_UNIT_IP_INFO_STC* node2
)
{
    if((node1 == NULL) && (node2 == NULL))
        return GT_LL_EQUAL;

    if((node1 == NULL) && (node2 != NULL))
        return GT_LL_SMALLER;

    if((node1 != NULL) && (node2 == NULL))
        return GT_LL_GREATER;

    if((node1->ipAddr.u32Ip) == (node2->ipAddr.u32Ip))
        return GT_LL_EQUAL;

    if ((node1->ipAddr.u32Ip) < (node2->ipAddr.u32Ip))
        return GT_LL_SMALLER;

    return GT_LL_GREATER;
}
#endif /* !ASIC_SIMULATION */

/*******************************************************************************
* cmdGetIpForUnit
*
* DESCRIPTION:
*       Get the IP address of a specified unit.
*
* INPUTS:
*       unitNum  - Number of unit.
*
* OUTPUTS:
*       ipAddr   - The IP address of the unit.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_NOT_FOUND       - IP address not found for the unit.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
GT_STATUS cmdGetIpForUnit
(
    IN  GT_U8    unitNum,
    OUT GT_IPADDR *ipAddr
)
{
    GT_UNIT_IP_INFO_STC* pResultData;    /* pointer to result data structure */
    GT_UNIT_IP_INFO_STC searchData;      /* structure to hold the search data */

    /* database is empty */
    if (cmdLlHead == NULL)
    {
        cmdOsMemSet (ipAddr, 0, sizeof(ipAddr));
        return GT_NOT_FOUND;
    }

    searchData.unitNum = unitNum;
    pResultData = llSearch(cmdLlHead, &searchData);

    /* unit was not found */
    if (pResultData == NULL)
    {
        cmdOsMemSet (ipAddr, 0, sizeof(ipAddr));
        return GT_NOT_FOUND;
    }

    *ipAddr = pResultData->ipAddr;
    return GT_OK;
}

/*******************************************************************************
* removeUnit
*
* DESCRIPTION:
*       Removes unit from the unit to IP mapping database.
*
* INPUTS:
*       unitNum - Unit number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
static void removeUnit
(
    GT_U8 unitNum
)
{
    GT_UNIT_IP_INFO_STC data;      /* structure to hold data to remove */

    data.unitNum = unitNum;
    cmdOsFree (llDelete (cmdLlHead, &data));
}

/*******************************************************************************
* internalSetIpForUnit
*
* DESCRIPTION:
*       Set the IP address of a specified unit.
*
* INPUTS:
*       unitNum  - Number of unit.
*       ipAddr   - The IP address of the unit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_ALREADY_EXIST   - unit or IP address already exist.
*       GT_SET_ERROR       - unable to set IP address because system has
*                            been already initialized.
*
* COMMENTS:
*      It's recommended to use this API only before all the system units are
*      initialized.
*
*******************************************************************************/
static GT_STATUS internalSetIpForUnit
(
    IN  GT_U8     unitNum,
    IN  GT_IPADDR ipAddr
)
{
    GT_UNIT_IP_INFO_STC *unitData;    /* structure to hold unit data */
    GT_STATUS retVal;                 /* returned value */

    /* If linked list was not initialized, do it now */
    if (!isLinkedListInitialized)
    {
        cmdLlHead = llInit((GT_INTFUNCPTR)compUnitNum, GT_FALSE);
        isLinkedListInitialized = GT_TRUE;
    }

    /* gtInitSystem has already been executed */

    if (systemInitialized)
    {
        return GT_SET_ERROR;
    }


    /* Fill unitData parameters */
    unitData = (GT_UNIT_IP_INFO_STC*)cmdOsMalloc (sizeof (GT_UNIT_IP_INFO_STC));
    unitData->unitNum = unitNum;
    unitData->ipAddr = ipAddr;

/* In WM allow 2 units with the same IP address */
#ifndef ASIC_SIMULATION
    /* Check if the IP address already exists in the database */
    cmdLlHead->compareFunc = (GT_INTFUNCPTR)compIpAddr;
    if (llSearch (cmdLlHead, unitData) != NULL)
    {
        cmdLlHead->compareFunc = (GT_INTFUNCPTR)compUnitNum;
        return GT_ALREADY_EXIST;
    }
    /* Restore compare function, such that the unit will be the search key */
    cmdLlHead->compareFunc = (GT_INTFUNCPTR)compUnitNum;
#endif

    /* Add new node to list */
    if ((retVal = llAdd (cmdLlHead, unitData)) != GT_OK)
    {
        return retVal;
    }
    return GT_OK;
}

/*******************************************************************************
* cmdSetIpForUnit
*
* DESCRIPTION:
*       Set the IP address of a specified unit.
*
* INPUTS:
*       unitNum  - Number of unit.
*       ipAddr   - The IP address of the unit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_ALREADY_EXIST   - unit or IP address already exist.
*       GT_BAD_PARAM       - illegal unit number.
*       GT_SET_ERROR       - unable to set IP address because system has
*                            been already initialized.
*       GT_BAD_STATE       - command is not executed on valid unit.
*       GT_ABORTED         - operation aborted due to failure in connecting to
*                            the new unit.
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*      It's recommended to use this API only before all the system units are
*      initialized.
*
*******************************************************************************/
GT_STATUS cmdSetIpForUnit
(
    IN  GT_U8     unitNum,
    IN  GT_IPADDR ipAddr
)
{
    GT_STATUS retVal;       /* returned value */

    if (cmdActiveUnit != DEFAULT_UNIT)
    {
        return GT_BAD_STATE;
    }
    if (unitNum >= MAX_NUM_UNITS)
    {
        return GT_BAD_PARAM;
    }
    if ((retVal = internalSetIpForUnit (unitNum, ipAddr)) != GT_OK)
        return retVal;
    if (unitNum != DEFAULT_UNIT)   /* Don't connect unit to itself */
    {
        if ((retVal = createMultiTapiClient (ipAddr, unitNum)) != GT_OK)
        {
            removeUnit (unitNum);
            return GT_ABORTED;
        }
    }

    if ((retVal = syncUnitsDatabase (ipAddr, unitNum)) != GT_OK)
        return retVal;

    return GT_OK;
}

/*******************************************************************************
* cmdIsMultiTapiSystem
*
* DESCRIPTION:
*       Get information on whether the system has more than one TAPI.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE              - System is multi-TAPI.
*       GT_FALSE             - System is single-TAPI.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
GT_BOOL cmdIsMultiTapiSystem
(
    GT_VOID
)
{
    if ((cmdLlHead != NULL) && (cmdLlHead->numOfNodes > 1))
    {
        return GT_TRUE;
    }
    return GT_FALSE;
}

/*******************************************************************************
* cmdGetUnitsNumber
*
* DESCRIPTION:
*       Get the number of units in the system.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Number of units in the system.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
GT_U8 cmdGetUnitsNumber
(
    GT_VOID
)
{
    if (cmdLlHead == NULL)
    {
        return 1;
    }
    return (GT_U8)cmdLlHead->numOfNodes;
}

/*******************************************************************************
* cmdIsUnitExist
*
* DESCRIPTION:
*       Check if a specific unit exists in the database.
*
* INPUTS:
*       unit - Unit number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - The unit exists.
*       GT_FALSE  - The unit doesn't exist.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
GT_BOOL cmdIsUnitExist
(
    IN GT_U8 unit
)
{
    GT_UNIT_IP_INFO_STC searchData;      /* structure to hold search data */

    /* list is empty */
    if (cmdLlHead == NULL)
    {
        return GT_FALSE;
    }

    searchData.unitNum = unit;
    /* unit was not found */
    if (llSearch (cmdLlHead, &searchData) == NULL)
    {
        return GT_FALSE;
    }

    return GT_TRUE;
}

/*******************************************************************************
* sendCommandOnSocket
*
* DESCRIPTION:
*       Send the command to remote unit and get its status
*
* INPUTS:
*       inputBufferI  - Pointer to string with the input command
*       fieldBufferI  - Pointer to string with the command arguments
*       unit          - the unit that should accept the data
*
* OUTPUTS:
*       outputBuffer  - The command output
*
* RETURNS:
*      CMD_OK              - no error
*      CMD_AGENT_ERROR     - internal error in galtis agent or connection failed
*      CMD_SYNTAX_ERROR    - generic syntax error
*      CMD_ARG_UNDERFLOW   - not enough arguments
*      CMD_ARG_OVERFLOW    - too many arguments
*      CMD_FIELD_UNDERFLOW - not enough fields
*      CMD_FIELD_OVERFLOW  - too many fields
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS sendCommandOnSocket
(
    GT_U8 *inputBufferI,
    GT_U8 *fieldBufferI,
    GT_U32 unit,
    GT_U8 **outputBuffer
)
{
    CMD_STATUS status;
    GT_U32 len = CMD_MAX_BUFFER;
    cmdStreamPTR stream;

    static GT_U8 *outBuf = NULL; /* Galtis output */

    if (outBuf == NULL)
    {
       outBuf  = (GT_U8*) cmdOsStaticMalloc(CMD_MAX_BUFFER); /* Galtis output */
    }

    stream = cmdSocketId[unit];
    if (!stream)
    {
        return GT_FAIL;
    }

    outBuf[0] = 0;
    *outputBuffer = outBuf;

    /* send the command to remote unit */
    if (stream->write(stream, inputBufferI,
                           cmdOsStrlen (inputBufferI)) < 0)
    {
        return CMD_AGENT_ERROR;
    }

    /* If the command has fields as input, send them */
    if (fieldBufferI[0] != 0)
    {
        /* Receive nothing  */
        len = CMD_MAX_BUFFER;
        if (stream->readLine(stream, *outputBuffer, len) < 0)
        {
            return CMD_AGENT_ERROR;
        }
        /* Send the command fields */
        if (stream->write(stream, fieldBufferI,
                            cmdOsStrlen (fieldBufferI)) < 0)
        {
            return CMD_AGENT_ERROR;
        }
    }

    /* Receive command status */
    len = CMD_MAX_BUFFER;
    cmdOsMemSet (*outputBuffer, 0, CMD_MAX_BUFFER);
    if (stream->readLine(stream, *outputBuffer, len) < 0)
    {
        return CMD_AGENT_ERROR;
    }
    status = cmdOsStrTo32 (*outputBuffer);

    /* Send nothing */
    if (stream->write(stream, "", 1) < 0)
    {
        return CMD_AGENT_ERROR;
    }

    /* receive output */
    len = CMD_MAX_BUFFER;
    cmdOsMemSet (*outputBuffer, 0, CMD_MAX_BUFFER);
    if (stream->readLine(stream, *outputBuffer, len) < 0)
    {
        cmdOsStrCpy (*outputBuffer, "@@@");
        cmdOsSprintf (*outputBuffer+3, "%x", GT_ABORTED);
        cmdOsStrCat (*outputBuffer, "!!!###");
    }
    return status;
}

/*******************************************************************************
* cmdSwitchUnit
*
* DESCRIPTION:
*       Implementation of the switching parser - switch the active unit.
*
* INPUTS:
*       selectedUnit - selected unit to work with.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_NOT_FOUND       - the unit doesn't exist.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
GT_STATUS cmdSwitchUnit
(
    IN GT_U8 selectedUnit
)
{
    if ((cmdGetUnitsNumber() == 1) && (selectedUnit == DEFAULT_UNIT))
    {
        return GT_OK;
    }
    if ((cmdIsUnitExist (selectedUnit) == GT_FALSE) &&
        (selectedUnit != ALL_UNITS))
    {
        return GT_NOT_FOUND;
    }

    cmdActiveUnit = selectedUnit;
    return GT_OK;
}

