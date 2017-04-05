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
* gtOsVersion.c
*
* DESCRIPTION:
*       Operating System wrapper. Version facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*******************************************************************************/


/************* Includes *******************************************************/

#include <gtOs/gtOsVersion.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsStr.h>

#define GT_OS_VERSTRING   "v1.2a3"

/************* Functions ******************************************************/
/*******************************************************************************
* osVersion
*
* DESCRIPTION:
*       Returns OS Layer wrapper version number.
*
* INPUTS:
*       verString - pointer to buffer for return string
*
* OUTPUTS:
*       verString - string with version number
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on NULL pointer
*
* COMMENTS:
*       User of this function should allocate at less 10 bytes for version
*       string.
*
*******************************************************************************/
GT_STATUS osVersion
(
    IN GT_OS_VERSION * verString
)
{
    if (verString == NULL)
        return GT_BAD_PTR;

    osMemCpy(&(verString->version[0]),GT_OS_VERSTRING,
             osStrlen(GT_OS_VERSTRING) + 1);

    return GT_OK;
}
