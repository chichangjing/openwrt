/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssCrc.h
*
* DESCRIPTION:
*       Includes definitions for CRC calculation function.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __prvCpssCrch
#define __prvCpssCrch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

 
/********* Include files ******************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*******************************************************************************
* prvCpssCalcCrc32
*
* DESCRIPTION:
*       Calculate CRC 32 bit for input data.
*
* INPUTS:
*       crc     - CRC start value.
*       buffer  - pointer to the buffer.
*       byteNum - number of bytes in the buffer.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       32 bit CRC.
*
* COMMENTS:
*   For calculation a new CRC the value of CRC should be 0xffffffff.
*
*******************************************************************************/
GT_U32 prvCpssCalcCrc32
(
    IN GT_U32 crc,
    IN GT_U8  *buffer,
    IN GT_U32 byteNum
);

/*******************************************************************************
* prvCpssCalcCrc16
*
* DESCRIPTION:
*       Calculate CRC 16 bit for input data.
*
* INPUTS:
*       crc     - CRC start value.
*       buffer  - pointer to the buffer.
*       byteNum - number of bytes in the buffer.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       32 bit CRC.
*
* COMMENTS:
*   For calculation a new CRC the value of CRC should be 0xffff.
*
*******************************************************************************/
GT_U16 prvCpssCalcCrc16
(
    IN GT_U16 crc,
    IN GT_U8  *buffer,
    IN GT_U32 byteNum
);

/*******************************************************************************
* prvCpssCalcCrc8
*
* DESCRIPTION:
*       Calculate CRC 8 bit for input data.
*
* INPUTS:
*       crc     - CRC start value.
*       buffer  - pointer to the buffer.
*       byteNum - number of bytes in the buffer.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       32 bit CRC.
*
* COMMENTS:
*   For calculation a new CRC the value of CRC should be 0xff.
*
*******************************************************************************/
GT_U8 prvCpssCalcCrc8
(
    IN GT_U8  crc,
    IN GT_U8  *buffer,
    IN GT_U32 byteNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssCrch */

