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
* gtAppDemoXbarConfig.c
*
* DESCRIPTION:
*       Crossbar configuration and initialization control.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/

#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoXbarConfig.h>

/*******************************************************************************
* internal definitions
*******************************************************************************/
/*******************************************************************************
* appDemoHgGetDefInitParams
*
* DESCRIPTION:
*       Retrieve the initialization parameters
*
* INPUTS:
*     xbarDevType - The crossbar device type
*     hgParamsLen - The number of elements in hgParams[] array.  This should
*                   be the maximum HyperG.link port on the device.
*
* OUTPUTS:
*     hgParamsLen - List length of initialization parameters.
*     hgParams  -   The HyperG.Link fabric port initialization paramters
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       The HyperG.Link fabric ports are initialized by the
*       crossbar initialiation API: coreXbarHwPhase1Init()
*
* GalTis:
*       Command - none.
*
*******************************************************************************/
GT_STATUS appDemoHgGetDefInitParams
(
    IN      GT_XBAR_DEVICE       xbarDevType,
    INOUT   GT_U8                *hgParamsLen,
    OUT     GT_HG_INIT_PARAMS    hgParams[]
)
{
  return GT_OK;
}


