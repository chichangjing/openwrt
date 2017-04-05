/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapAsicSim.h
*
* DESCRIPTION:
*       This file implements wrapper functions for the 'Hardware' library
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#ifndef __WrapAsicSimh
#define __WrapAsicSimh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cmdShell/common/cmdCommon.h>


/*******************************************************************************
* cmdLibInitAsicSim
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitAsicSim();

/*******************************************************************************
* scibWriteRegister (general command)
*
* DESCRIPTION:
*     Write data to register in the simulation
*
* INPUTS:
*     [0] GT_32 deviceNum -
*          PP device number to write to.
*
*     [1] GT_U32 regAddr -
*          Register address to write to.
*
*     [2] GT_U32 regData -
*          Data to be written to register.
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrScibWriteRegister
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

/*******************************************************************************
* scibReadRegister (general command)
*
* DESCRIPTION:
*     Reads the unmasked bits of a register.
*
* INPUTS:
*     [0] GT_U32 lDevNum -
*          PP device number to read from.
*
*     [1] GT_U32 lRegAddr -
*          Register address to read from.
*
*
* OUTPUTS:
*     [0] GT_U32 lDataPtr -
*          Data read from register.
*
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrScibReadRegister
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

/*******************************************************************************
* scibWriteMemory (general command)
*
* DESCRIPTION:
*     Write data to memory in the simulation. Up to 8 words may be written
*
* INPUTS:
*     [0] GT_32 deviceNum -
*          PP device number to write to.
*
*     [1] GT_U32 memAddr -
*          Memory address to write to.
*
*     [2] GT_U32 memSize -
*          Size of data to be written to memory.
*
*     [3] GT_U32 memDataWord0  -
           Data to be written to register words0
*     [4] GT_U32 memDataWord1  -
           Data to be written to register words1
*     [5] GT_U32 memDataWord2  -
           Data to be written to register words2
*     [6] GT_U32 memDataWord3  -
           Data to be written to register words3
*     [7] GT_U32 memDataWord4  -
           Data to be written to register words4
*     [8] GT_U32 memDataWord5  -
           Data to be written to register words5
*     [9] GT_U32 memDataWord6  -
           Data to be written to register words6
*    [10] GT_U32 memDataWord7  -
           Data to be written to register words7
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrScibWriteMemory
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

/*******************************************************************************
* wrScibReadMemory (general command)
*
* DESCRIPTION:
*     Read data from memory in the simulation. Up to 8 words may be read.
*
* INPUTS:
*     [0] GT_U32 lDevNum -
*          PP device number to read from.
*
*     [1] GT_U32 lRegAddr -
*          Register address to read from.
*
*     [2] GT_U32 lMemSize -
*          Size of data to be written to memory.
*
* OUTPUTS:
*     [3] GT_U32 memDataWord0  -
           Data to be written to register words0
*     [4] GT_U32 memDataWord1  -
           Data to be written to register words1
*     [5] GT_U32 memDataWord2  -
           Data to be written to register words2
*     [6] GT_U32 memDataWord3  -
           Data to be written to register words3
*     [7] GT_U32 memDataWord4  -
           Data to be written to register words4
*     [8] GT_U32 memDataWord5  -
           Data to be written to register words5
*     [9] GT_U32 memDataWord6  -
           Data to be written to register words6
*    [10] GT_U32 memDataWord7  -
*
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrScibReadMemory
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);


CMD_STATUS wrIndirectTblMemoryTest
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

CMD_STATUS wrSearchMemoryTest
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

/*******************************************************************************
* wrSkernelPolicerConformanceLevelForce
*
* DESCRIPTION:
*    force the conformance level for the packets entering the policer
*       (traffic cond)
*
*
* INPUTS:
*     [0] GT_DP_LEVEL    conformance level - green/yellow/red
*
*
* OUTPUTS:

* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrSkernelPolicerConformanceLevelForce
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

/*******************************************************************************
* wrCpssAsicSimRtosOnSimRemoteConnectionParametersSet
*
* DESCRIPTION:
*    set RTOS on simulation parameters , this emulate the INI file of simulation
*
*
* INPUTS:
*     inArgs[0] - simulation IP - the "server side" (Note: currently the PP side is
*               the "server side" , it will be changed ... , and then this
*               parameter will be meaningless)
*               format is "10.6.150.92"
*     inArgs[1] - tcp_port_synchronic - the TCP port of the "synchronic connection"
*     inArgs[2] - tcp_port_asynchronous - the TCP port of the "asynchronous connection"
*
* OUTPUTS:
*       none
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssAsicSimRtosOnSimRemoteConnectionParametersSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

/*******************************************************************************
* wrCpssAsicSimRtosOnSimIniFileEmulateFill
*
* DESCRIPTION:
*    set RTOS on simulation parameters , this emulate the INI file of simulation
*
*
* INPUTS:
*     inArgs[0] - chapter name (string)
*     inArgs[1] - value name (string)
*     inArgs[2] - value (string)
*
* OUTPUTS:
*       none
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssAsicSimRtosOnSimIniFileEmulateFill
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WrapAsicSimh */


