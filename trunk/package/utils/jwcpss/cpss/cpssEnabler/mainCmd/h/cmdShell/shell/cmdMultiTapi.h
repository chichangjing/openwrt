/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdMultiTapi.h
*
* DESCRIPTION:
*       MultiTapi definitions
*
*
* DEPENDENCIES:
*       gtOs.h, gtOsEx.h
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/
#ifndef __cmdMultiTapih
#define __cmdMultiTapih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>
#if (!defined  CPSS_PRODUCT) && (!defined PSS_PRODUCT)
#  include <cpss/generic/cpssTypes.h>/* this include for cpss only */
#else
#  include <prestera/common/gtLinkedList.h>
#  include <prestera/os/gtTypes.h>
#endif

/* default unit for Galtis commands execution */
#define DEFAULT_UNIT 0

/*
 * Typedef: struct GT_UNIT_IP_INFO_STC
 *
 * Description: Stores IP address of a unit.
 *
 * Fields:
 *      unitNum - unit number
 *      ipAddr  - unit IP address
 *
 */
typedef struct
{
    GT_U8 unitNum;
    GT_IPADDR ipAddr;
}GT_UNIT_IP_INFO_STC;

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
);

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
);

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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdMultiTapih */
