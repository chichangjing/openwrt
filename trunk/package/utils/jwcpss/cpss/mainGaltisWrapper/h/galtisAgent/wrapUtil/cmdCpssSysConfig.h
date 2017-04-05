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
* cmdSysConfig.h
*
* DESCRIPTION:
*       System configuration and initialization control.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 25 $
*
*******************************************************************************/
#ifndef __cmdSysConfigh
#define __cmdSysConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssTypes.h>

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/cmdDb/cmdBase.h>

/*******************************************************************************
 * susconf related definitions
 ******************************************************************************/
/* Max number of chars in string fields in CMD_SYSCONF_INFO */
#define MAX_SYSCONF_STR_LEN 30

#define TAPI_TYPE_ID    12      /* For father Type  */
#define TAPI_ID         0       /* For id           */
#define TAPI_TYPE       0x101   /* For deviceId     */

#define UNIT_TYPE_ID    13      /* For father Type  */
#define UNIT_TYPE       0x102   /* For deviceId     */
#define UNIT_SON_TYPE   0       /* For sonType      */


#define PP_TYPE_ID      14      /* For father type  */
#define PP_SON_TYPE     0       /* For sonType      */


#define PHY_SON_TYPE    22      /* For sonType      */

#define IS_MARVELL_PHY(phyId)                                   \
        ((phyId >> 10) == 0x5043)

#define GET_PHY_ID(phyId)                                       \
        (phyId >> 4)

/*
 * Typedef: struct CMD_SYSCONF_INFO
 *
 * Description: Holds information regarding all elements in system for the
 *              sysconf usage.
 *
 * Fields:
 *      id          - The id of the device.
 *      revision    - The device revision.
 *      subRev      - Device's sub-revision.
 *      deviceId    - Device type.
 *      fatherId    - Id field of the device's father.
 *      fatherType  - Type of the father.
 *      sonIndex    - Which son is this.
 *      sonType     - Type of this device.
 *
 */
typedef struct
{
    GT_U32                  id;
    GT_U8                   revision[MAX_SYSCONF_STR_LEN];
    GT_U8                   subRev[MAX_SYSCONF_STR_LEN];
    GT_U32                  deviceId;
    GT_U32                  fatherId;
    GT_U32                  fatherType;
    GT_U32                  sonIndex;
    GT_U32                  sonType;
}CMD_SYSCONF_INFO;


/*******************************************************************************
* mngInitSystem
*
* DESCRIPTION:
*       Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
* GalTis:
*       Command - mngInitSystem
*
* Toolkit:
*       Interface - <prestera/tapi/sysConfig/commands.api>
*
*******************************************************************************/
GT_STATUS mngInitSystem(void);


/*******************************************************************************
* mngGetSysFirstElement
*
* DESCRIPTION:
*       Returns the first element of the sysconf.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
* Galtis:
*       Command - mngGetSysFirstElement
*
* Toolkit:
*       Interface - <prestera/tapi/sysConfig/commands.api>
*
*******************************************************************************/
GT_STATUS mngGetSysFirstElement(void);

/*******************************************************************************
* mngGetSysNextElement
*
* DESCRIPTION:
*       Returns the next element of the sysconf.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
* Galtis:
*       Command - mngGetSysNextElement
*
* Toolkit:
*       Interface - <prestera/tapi/sysConfig/commands.api>
*
*******************************************************************************/
GT_STATUS mngGetSysNextElement(void);


/*******************************************************************************
* sys
*
* DESCRIPTION:
*       Prints the sysconf elements.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
* Galtis:
*       Command - sys
*
* Toolkit:
*       Interface - <prestera/tapi/sysConfig/commands.api>
*
*******************************************************************************/
GT_STATUS sys(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdSysConfigh */

