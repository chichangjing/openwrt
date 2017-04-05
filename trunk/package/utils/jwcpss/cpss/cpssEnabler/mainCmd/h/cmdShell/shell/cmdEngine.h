/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* cmdEngine.h
*
* DESCRIPTION:
*       commander database engine api header
*
*
* DEPENDENCIES:
*       gtOs.h
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#ifndef __cmdEngine_h__
#define __cmdEngine_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Include files ***************************************************/

#include <cmdShell/shell/cmdSystem.h>

/*******************************************************************************
* cmdEventFields
*
* DESCRIPTION:
*       parses input buffer, and tells whether command has fields as input
*
* INPUTS:
*       inBuffer    - null terminated string holding command buffer
*
* RETURNS:
*       GT_TRUE     - fields need to be read
*       GT_FALSE    - command has no fields as input
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_BOOL cmdEventFields
(
    IN GT_CHAR *inBuffer
);

/*******************************************************************************
* cmdEventRun
*
* DESCRIPTION:
*       command interpreter; parses and executes single command stored
*       in null terminated string.
*
* INPUTS:
*       inBuffer      - null terminated string holding command buffer
*       inFieldBuffer - null terminated string holding field values
*
* OUTPUTS:
*       outBuffer   - pointer to null terminated string holding output
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS cmdEventRun
(
    IN  GT_CHAR *inBuffer,
    IN  GT_CHAR *inFieldBuffer,
    OUT GT_CHAR **outBuffer
);
/*******************************************************************************
* cmdEngineInit
*
* DESCRIPTION:
*       initializes engine. Spawn new task for command execution.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on fialure
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdEngineInit(GT_VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdEngine_H__ */

