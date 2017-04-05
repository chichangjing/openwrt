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
* gtOsVersion.h
*
* DESCRIPTION:
*       Operating System wrapper. Random facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __gtOsVersionh
#define __gtOsVersionh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

#define OS_VERSION_MAX_LEN 30
/*
 * Typedef: struct GT_VERSION
 *
 * Description: This struct holds the package version.
 *
 * Fields:
 *      version - string array holding the version.
 *
 */
typedef struct
{
    GT_U8   version[OS_VERSION_MAX_LEN];
}GT_OS_VERSION;

/************* Functions ******************************************************/
/*******************************************************************************
* osVersion
*
* DESCRIPTION:
*       Returns OS Layer wrapper version number.
*
* INPUTS:
*       verString - pointer to buffer struct with buffer for string
*
* OUTPUTS:
*       verString - string with version number
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osVersion
(
    IN GT_OS_VERSION * verString
);


#ifdef __cplusplus
}
#endif

#endif  /* __gtOsVersionh */
/* Do Not Add Anything Below This Line */

