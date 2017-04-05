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
* gtAppDemoXbarConfig.h
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
#ifndef __gtAppDemoXbarConfig
#define __gtAppDemoXbarConfig

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <gtExtDrv/drivers/gtHwIfDrv.h>
#include <cpssXbar/generic/xbarHwInit/gtCoreXbarHwInit.h>

#include <appDemo/os/appOs.h>


#define MAX_NUM_OF_HGLINKS 4


/*
 * Typedef: struct APP_DEMO_XBAR_CONFIG
 *
 * Description: Holds XBAR configuration parameters for initialization.
 *
 * Fields:
 *      xbarInitDone- Indicates whether the corePpHwPhase1Init() function was
 *                    already called.
 *      devNum      - The device's device number.
 *      oldDevNum   - The old device number that was assigned to the device in
 *                    phase1.
 *      unitNum     - The unit number on which this device resides.
 *      xbarDevice  - The device type of crossbar.
 *      intConfig   - Xbar device Interrupt definition.
 *      valid       - If this entry holds valid data.
 *      phase1Params- Phase1 Crossbar initialization parameters.
 *
 */
typedef struct
{
    GT_BOOL                     xbarInitDone;
    GT_U8                       devNum;
    GT_U8                       oldDevNum;
    GT_U8                       unitNum;
    GT_XBAR_DEVICE              xbarDevice;
    GT_BOOL                     valid;
    GT_XBAR_PHASE1_INIT_PARAMS  phase1Params;
}APP_DEMO_XBAR_CONFIG;

/****************************************************************************/
/* GT_XBAR_SYS_CFG_PHASE1 struct default                                    */
/****************************************************************************/
#define GT_XBAR_SYS_CFG_DEF                                         \
{                                                                   \
    11,                                 /* eventsTaskPrio       */  \
    4096,                               /* eventsTaskStackSize  */  \
    GT_FALSE,                           /* eventRequestDriven   */  \
}

/****************************************************************/
/* XBar Phase1 initialization parameters default values.        */
/****************************************************************/
#define GT_XBAR_DEF_DEV_TYPE  GT_XBAR_98FX910A

#define GT_XBAR_FOX_PHASE1_DEF_PARAMS                           \
{                                                               \
    0,                                  /* devNum           */  \
    GT_XBAR_FOX_FAMILY,                 /* XBAR family      */  \
    GT_MGMT_VIA_PP,                     /* mgmtIf           */  \
    0x0,                                /* i2cBaseAddr      */  \
    0,                                  /* hgParamsListLen  */  \
    NULL,                               /* hgParamsList     */  \
    {                                   /* intConfig        */  \
        GT_INT_OTHER,                   /* intType          */  \
        0,                              /* intVecNum        */  \
        0                               /* intMask          */  \
    },                                                          \
}

#define GT_XBAR_CAP_PHASE1_DEF_PARAMS                           \
{                                                               \
    0,                                  /* devNum           */  \
    GT_XBAR_CAP_FAMILY,                 /* XBAR family      */  \
    GT_MGMT_I2C,                        /* mgmtIf           */  \
    0x0,                                /* i2cBaseAddr      */  \
    0,                                  /* hgParamsListLen  */  \
    NULL,                               /* hgParamsList     */  \
    {                                   /* intConfig        */  \
        GT_INT_OTHER,                   /* intType          */  \
        0,                              /* intVecNum        */  \
        0                               /* intMask          */  \
    },                                                          \
}

/*******************************************************************************
* Core Xbar Hw Init related functions
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
);


/*******************************************************************************
* appDemoGetXbarPhase1InitParams
*
* DESCRIPTION:
*       Returns Xbar init params.
*
* INPUTS:
*       devNum     - The device number to get the params for.
*       xbarFamily - Xbar family type FOX or CAP. 
*
* OUTPUTS:
*       xbarInitParams - The xbar init params struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS appDemoGetXbarPhase1InitParams
(
    IN  GT_U8                       devNum,
    IN  GT_XBAR_DEV_FAMILY          xbarFamily,
    OUT GT_XBAR_PHASE1_INIT_PARAMS  *xbarInitParams
);


/*******************************************************************************
* appDemoSetXbarPhase1InitParams
*
* DESCRIPTION:
*       Performs Xbar initialization.
*
* INPUTS:
*       xbarInitParams - The xbar init params struct.
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
*       None.
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS appDemoSetXbarPhase1InitParams
(
    IN  GT_XBAR_PHASE1_INIT_PARAMS  *xbarInitParams
);


/*******************************************************************************
* appDemoSetXbarPhase2InitParams
*
* DESCRIPTION:
*       Performs Xbar initialization.
*
* INPUTS:
*       xbarInitParams - The xbar init params struct.
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
*       None.
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS appDemoSetXbarPhase2InitParams
(
    IN  GT_XBAR_PHASE2_INIT_PARAMS  *xbarInitParams
);

/*******************************************************************************
* appDemoFaXbarConfig
*
* DESCRIPTION: Initialize the XBARS and FAs for the demo. This function will
*              configure the routing tables to the XBARs. Also it will configure
*              the VOQs of the FAs
*
* INPUTS: 
*           xbarDevNum  : device number of FA/XBAR to configure
*           inOutFaPort : In and out FA port - leads to targetDev
*           targetDev   : remote Device Id
* OUTPUTS:
*           None.
*
* RETURNS :
*           GT_OK        - on success,
*           GT_FAIL      - Operation failed.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoFaXbarConfig
(
    IN GT_U8 xbarDevNum,
    IN GT_U8 inOutFaPort,
    IN GT_U8 targetDev
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtAppDemoXbarConfig */


