/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpGenDump.h
*
* DESCRIPTION:
*       PP Registers and PP memory dump function headers
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssDrvPpGenDump
#define __cpssDrvPpGenDump

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/config/private/prvCpssConfigTypes.h>

/*
 * typedef: enum CPSS_MEMORY_DUMP_TYPE_ENT
 *
 * Description: This enumeration defines format of memory dump
 *
 * Fields:
 *      CPSS_MEMORY_DUMP_BYTE_E  - dump string will contain 16 byte  elements (8  bits each)
 *      CPSS_MEMORY_DUMP_SHORT_E - dump string will contain  8 short elements (16 bits each)
 *      CPSS_MEMORY_DUMP_WORD_E  - dump string will contain  4 word  elements (32 bits each)
 */
typedef enum
{
    CPSS_MEMORY_DUMP_BYTE_E,
    CPSS_MEMORY_DUMP_SHORT_E,
    CPSS_MEMORY_DUMP_WORD_E
}CPSS_MEMORY_DUMP_TYPE_ENT;


/*******************************************************************************
* cpssPpDumpRegisters
*
* DESCRIPTION:
*       This routine print dump of PP registers.
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (all Registers dumped successfully)
*       GT_FAIL         - on failure. (at least one register read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpDumpRegisters
(
    IN GT_U8                    devNum
);

/*******************************************************************************
* cpssPpDumpMemory
*
* DESCRIPTION:
*       This routine print dump of PP memory.
*
* INPUTS:
*       devNum      - PP's device number .
*       startAddr   - dump start address (will be aligned to 16 bytes)
*       dumpType    - dump line format (16 bytes / 8 shorts / or 4 words)
*       dumpLength  - dump length in bytes
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (memory dumped successfully)
*       GT_FAIL         - on failure. (memory read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*       4 lower bits of startAddr will be zeroed (ignored) for alignment
*
*******************************************************************************/
GT_STATUS cpssPpDumpMemory
(
    IN GT_U8                        devNum,
    IN GT_U32                       startAddr,  
    IN CPSS_MEMORY_DUMP_TYPE_ENT    dumpType,
    IN GT_U32                       dumpLength
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDrvPpGenDump */


