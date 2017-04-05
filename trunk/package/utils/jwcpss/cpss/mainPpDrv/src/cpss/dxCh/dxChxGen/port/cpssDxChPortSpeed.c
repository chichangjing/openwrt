/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortSpeed.c
*
* DESCRIPTION:
*       CPSS implementation for Port speed configuration.
*
*       The following APIs can run now "preliminary stage" and "post stage"
*       callbacks, if the bind with port MAC object pointer engaged:
*        - cpssDxChPortSpeedSet;
*        - cpssDxChPortSpeedGet;
*
* FILE REVISION NUMBER:
*       $Revision: 62 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLion2.h>

#include <mvHwsPortInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <private/mvPortModeElements.h>

/* TM glue */
#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGlue.h>

/* array defining serdes speed used in given interface mode for given port data speed */
/* APPLICABLE DEVICES:  DxChXcat and above. */
extern CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesFrequency[CPSS_PORT_INTERFACE_MODE_NA_E][CPSS_PORT_SPEED_NA_E];

extern PRV_CPSS_LION2_INT_CAUSE_ENT lion2PortUnitHglSumIndex
(
    IN GT_PHYSICAL_PORT_NUM localPort
);

/* array for translation of given interface mode and serdes speed of flex link to data speed of port */
/* APPLICABLE DEVICES:  Xcat, Lion, Xcat2. */
static CPSS_PORT_SPEED_ENT ifModeSerdesToPortSpeed[CPSS_PORT_INTERFACE_MODE_NA_E][CPSS_DXCH_PORT_SERDES_SPEED_NA_E] =
{                                               /* 1.25G                  3.125G                      3.75G                       6.25G                         5G                          4.25G                       2.5G                  5.156G  */
/* CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E  */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_MII_E           */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_SGMII_E         */  {CPSS_PORT_SPEED_1000_E , CPSS_PORT_SPEED_2500_E    , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_XGMII_E         */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_10000_E   , CPSS_PORT_SPEED_12000_E   , CPSS_PORT_SPEED_20000_E   , CPSS_PORT_SPEED_16000_E   , CPSS_PORT_SPEED_13600_E   , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_MGMII_E         */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,   */  {CPSS_PORT_SPEED_1000_E , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_GMII_E,         */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_MII_PHY_E,      */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_QX_E,           */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_2500_E    , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_5000_E    , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_HX_E,           */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_5000_E    , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_10000_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_RXAUI_E,        */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_10000_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_100BASE_FX_E,   */  {CPSS_PORT_SPEED_1000_E , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_QSGMII_E,       */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_1000_E    , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    },
/* CPSS_PORT_INTERFACE_MODE_XLG_E,          */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E      , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_40000_E },
/* CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E   */  {CPSS_PORT_SPEED_NA_E   , CPSS_PORT_SPEED_10000_E   , CPSS_PORT_SPEED_12000_E   , CPSS_PORT_SPEED_20000_E   , CPSS_PORT_SPEED_16000_E   , CPSS_PORT_SPEED_13600_E   , CPSS_PORT_SPEED_NA_E, CPSS_PORT_SPEED_NA_E    }
};

/* matrix [serdes speed][serdes ref clock] = reg value */
static GT_U32 speedRegValue[4][3] =
{{0x205C,0x200C,0   }, /* 1.25G */
 {0     ,0x245E,0x2421}, /* 3.125G */
 {0     ,0x245D,0x2420}, /* 6.25G */
 {0x255C,0x2435,0x2410}  /* 3.75G */
};

static GT_STATUS cheetah1_2_XgPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

static GT_STATUS cheetah3_XgPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

static GT_STATUS cheetah1_3_GePortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

static GT_STATUS xCatSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

static GT_STATUS lionSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

static GT_STATUS lion2SpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

GT_STATUS bcat2SpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

/* Array bellow is table of pointers to ifMode configuration functions per DXCH ASIC family per interface mode
 * if mode not supported NULL pointer provided
 */
PRV_CPSS_DXCH_PORT_SPEED_SET_FUN portSpeedSetFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1][PRV_CPSS_XG_PORT_OPTIONS_MAX_E]=
{                                   /* PRV_CPSS_XG_PORT_XG_ONLY_E  PRV_CPSS_XG_PORT_HX_QX_ONLY_E PRV_CPSS_XG_PORT_XG_HX_QX_E PRV_CPSS_GE_PORT_GE_ONLY_E PRV_CPSS_XG_PORT_XLG_SGMII_E  PRV_CPSS_XG_PORT_CG_SGMII_E */
/* CPSS_PP_FAMILY_CHEETAH_E     */  {  cheetah1_2_XgPortSpeedSet,  cheetah1_2_XgPortSpeedSet,    cheetah1_2_XgPortSpeedSet,  cheetah1_3_GePortSpeedSet, NULL,                         NULL         },
/* CPSS_PP_FAMILY_CHEETAH2_E    */  {  cheetah1_2_XgPortSpeedSet,  NULL,                         NULL,                       cheetah1_3_GePortSpeedSet, NULL,                         NULL         },
/* CPSS_PP_FAMILY_CHEETAH3_E    */  {  cheetah3_XgPortSpeedSet,    NULL,                         NULL,                       cheetah1_3_GePortSpeedSet, NULL,                         NULL         },
/* CPSS_PP_FAMILY_DXCH_XCAT_E   */  {  NULL,                       NULL,                         xCatSpeedSet,               xCatSpeedSet,              NULL,                         NULL         },
/* CPSS_PP_FAMILY_DXCH_XCAT3_E  */  {  NULL,                       NULL,                         NULL,/*xCatSpeedSet,*/      bcat2SpeedSet,             bcat2SpeedSet,                NULL         },
/* CPSS_PP_FAMILY_DXCH_LION_E   */  {  NULL,                       NULL,                         NULL,                       lionSpeedSet,              lionSpeedSet,                 NULL         },
/* CPSS_PP_FAMILY_DXCH_XCAT2_E  */  {  NULL,                       NULL,                         xCatSpeedSet,               xCatSpeedSet,              NULL,                         NULL         },
/* CPSS_PP_FAMILY_DXCH_LION2_E  */  {  NULL,                       NULL,                         NULL,                       lion2SpeedSet,/*for CPU*/  NULL,                         lion2SpeedSet},
/* CPSS_PP_FAMILY_DXCH_LION3_E  */  {  NULL,                       NULL,                         NULL,                       lion2SpeedSet,             NULL,                         lion2SpeedSet},
/* CPSS_PP_FAMILY_DXCH_BOBCAT2_E*/  {  NULL,                       NULL,                         NULL,                       bcat2SpeedSet,             NULL,                         bcat2SpeedSet},
/* CPSS_PP_FAMILY_DXCH_BOBCAT3_E*/  {  NULL,                       NULL,                         NULL,                       bcat2SpeedSet,             NULL,                         bcat2SpeedSet},
/* CPSS_PP_FAMILY_DXCH_ALDRIN_E */  {  NULL,                       NULL,                         NULL,                       bcat2SpeedSet,             NULL,                         bcat2SpeedSet}
};

static GT_STATUS cheetah1_3_PortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
);

static GT_STATUS xcatPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
);

static GT_STATUS lionPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
);

/* array of pointers to port interface mode get functions per DXCH ASIC family */
PRV_CPSS_DXCH_PORT_SPEED_GET_FUN portSpeedGetFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1]=
{
/* CPSS_PP_FAMILY_CHEETAH_E     */  cheetah1_3_PortSpeedGet,
/* CPSS_PP_FAMILY_CHEETAH2_E    */  cheetah1_3_PortSpeedGet,
/* CPSS_PP_FAMILY_CHEETAH3_E    */  cheetah1_3_PortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_XCAT_E   */  xcatPortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_XCAT3_E  */  lionPortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_LION_E   */  lionPortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_XCAT2_E  */  xcatPortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_LION2_E  */  lionPortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_LION3_E  */  lionPortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_BOBCAT2_E*/  lionPortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_BOBCAT3_E*/  lionPortSpeedGet,
/* CPSS_PP_FAMILY_DXCH_ALDRIN_E */  lionPortSpeedGet
};

/*******************************************************************************
* prvCpssDxChPortSpeedForCutThroughWaCalc
*
* DESCRIPTION:
*       Calculate port speed HW value and field offsets in RX and TX DMAs
*       for Cut Throw WA - packet from slow to fast port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum        - device number
*       localPortNum  - local port number
*       speed         - port speed
*
* OUTPUTS:
*       hwSpeedPtr    - pointer to 2-bit HW speed value.
*       rxRegAddrPtr  - pointer to address of relevant RX DMA register.
*       txRegAddrPtr  - pointer to address of relevant TX DMA register.
*       rxRegOffPtr   - pointer to bit offset of field in relevant RX DMA register.
*       txRegOffPtr   - pointer to bit offset of field in relevant TX DMA register.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSpeedForCutThroughWaCalc
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM localPortNum,
    IN  CPSS_PORT_SPEED_ENT  speed,
    OUT GT_U32               *hwSpeedPtr,
    OUT GT_U32               *rxRegAddrPtr,
    OUT GT_U32               *txRegAddrPtr,
    OUT GT_U32               *rxRegOffPtr,
    OUT GT_U32               *txRegOffPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_LION_E  );
    if (PRV_CPSS_DXCH_LION2_B1_AND_ABOVE_CHECK_MAC(devNum) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }
    CPSS_NULL_PTR_CHECK_MAC(hwSpeedPtr);
    CPSS_NULL_PTR_CHECK_MAC(rxRegOffPtr);
    CPSS_NULL_PTR_CHECK_MAC(txRegOffPtr);
    if (localPortNum >= 12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (speed)
    {
        /* three speed GIGE */
        case CPSS_PORT_SPEED_10_E:
        case CPSS_PORT_SPEED_100_E:
        case CPSS_PORT_SPEED_1000_E:
            *hwSpeedPtr = 0;
            break;
        case CPSS_PORT_SPEED_10000_E:
            *hwSpeedPtr = 1;
            break;
        case CPSS_PORT_SPEED_20000_E:
            *hwSpeedPtr = 2;
            break;
        case CPSS_PORT_SPEED_40000_E:
            *hwSpeedPtr = 3;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* RXDMA DFX metal fix register */
    *rxRegAddrPtr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.rxdmaDfxMetalFix;

    /* TXDMA Buffer memory clear type register */
    *txRegAddrPtr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaBufMemClearType;

    *rxRegOffPtr  = (2 * localPortNum) + 2;
    *txRegOffPtr  = (2 * localPortNum);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMacSerdesSpeedSet
*
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*                                  (For DxCh3, not XG ports only.)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMacSerdesSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;   /* return code */
    PRV_CPSS_DXCH_PORT_SPEED_SET_FUN speedSetFuncPtr; /* ptr to speed set function per devFamily
                                                            per port options */
    GT_PHYSICAL_PORT_NUM    portMacNum; /* MAC to which mapped current physical port */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacNum);

    if((!PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
       && (CPSS_CPU_PORT_NUM_CNS == portNum))
    {
        speedSetFuncPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->devObj.portPtr->
                        setPortSpeed[PRV_CPSS_GE_PORT_GE_ONLY_E];
    }
    else
    {
        speedSetFuncPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->devObj.portPtr->
                        setPortSpeed[PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum)];
    }

    if(speedSetFuncPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* configure requested speed on port */
    rc = speedSetFuncPtr(devNum, portNum, speed);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*                                  (For DxCh3, not XG ports only.)
*
* COMMENTS:
*       1. If the port is enabled then the function disables the port before
*          the operation and re-enables it at the end.
*       2. For Flex-Link ports the interface mode should be
*          configured before port's speed, see cpssDxChPortInterfaceModeSet.
*       3.This API also checks all SERDES per port initialization. If serdes was
*         not initialized, proper init will be done
*       4.Added callback bind option used to run PHYMAC configuration functions
*         Callbacks can be run before switch MAC configuration and after
*         First callback can set doPpMacConfig TRUE or FALSE. If the return value
*         is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    CPSS_PORT_SPEED_ENT  origSpeed = speed;     /* speed to configure on port */
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;        /* port callback pointer */
    GT_STATUS rc = GT_OK;                       /* return code */
    GT_BOOL doPpMacConfig = GT_TRUE;            /* run switch mac code indicator */
    GT_U32               portGroupId;           /* Port Group Id */
    GT_U32               localPort;             /* Local Port Num */
    GT_U32               hwSpeed;               /* Cut Through WA HW port speed value */
    GT_U32               rxRegAddr;             /* Cut Through WA RX DMA register address */
    GT_U32               txRegAddr;             /* Cut Through WA TX DMA register address */
    GT_U32               rxRegOff;              /* Cut Through WA bit offset in RX DMA register */
    GT_U32               txRegOff;              /* Cut Through WA bit offset in TX DMA register */
    GT_U32               portMacNum;

    if((systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E)
        && (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.hitlessWriteMethodEnable))
    {/* do only if we during recovery */
        CPSS_PORT_SPEED_ENT   currentSpeed;

        rc = prvCpssDxChPortSpeedGet(devNum, portNum, &currentSpeed);
        if(rc != GT_OK)
        {
            return rc;
        }
        if(speed == currentSpeed)
        {
            return GT_OK;
        }
    }

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,
                                                                 portMacNum);

    /* Get PHY MAC object ptr
       if it is CPU port or the pointer is NULL (no bind) - return NULL */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* run callback*/
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacSpeedSetFunc(devNum,portNum,
                                  origSpeed,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&origSpeed);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* doPpMacConfig value defined by callback; if it is TRUE -
       run prvCpssDxChPortSpeedSet that means do all required
       confiruration actions for switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortMacSerdesSpeedSet(devNum,portNum,origSpeed);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* if required the post stage callback can run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacSpeedSetFunc(devNum,portNum,
                                       origSpeed,CPSS_MACDRV_STAGE_POST_E,
                                       &doPpMacConfig,&origSpeed);
        if(rc!=GT_OK)
            {
            return rc;
        }
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_LION2_CUT_THROUGH_SLOW_TO_FAST_WA_E.
            enabled == GT_TRUE)
    {
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

        rc = prvCpssDxChPortSpeedForCutThroughWaCalc(
            devNum, localPort, speed,
            &hwSpeed, &rxRegAddr, &txRegAddr, &rxRegOff, &txRegOff);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* RX DMA register */
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, rxRegAddr, rxRegOff, 2 /*fieldLength*/, hwSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* TX DMA register */
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, txRegAddr, txRegOff, 2 /*fieldLength*/, hwSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*                                  (For DxCh3, not XG ports only.)
*
* COMMENTS:
*       1. If the port is enabled then the function disables the port before
*          the operation and re-enables it at the end.
*       2. For Flex-Link ports the interface mode should be
*          configured before port's speed, see cpssDxChPortInterfaceModeSet.
*       3.This API also checks all SERDES per port initialization. If serdes was
*         not initialized, proper init will be done
*       4.Added callback bind option used to run PHYMAC configuration functions
*         Callbacks can be run before switch MAC configuration and after
*         First callback can set doPpMacConfig TRUE or FALSE. If the return value
*         is TRUE the switch MAC will be configured
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_PHYSICAL_PORT_NUM portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,
                                                                 portMacNum);

    if(PRV_CPSS_PORT_NOT_EXISTS_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portMacNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* check input parameter */
    if (speed < CPSS_PORT_SPEED_10_E || speed >= CPSS_PORT_SPEED_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChPortSpeedSet(devNum, portNum, speed);
}

/*******************************************************************************
* cpssDxChPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*                                  (For DxCh3, not XG ports only.)
*
* COMMENTS:
*       1. If the port is enabled then the function disables the port before
*          the operation and re-enables it at the end.
*       2. For Flex-Link ports the interface mode should be
*          configured before port's speed, see cpssDxChPortInterfaceModeSet.
*       3.This API also checks all SERDES per port initialization. If serdes was
*         not initialized, proper init will be done
*       4.Added callback bind option used to run PHYMAC configuration functions
*         Callbacks can be run before switch MAC configuration and after
*         First callback can set doPpMacConfig TRUE or FALSE. If the return value
*         is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSpeedSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, speed));

    rc = internal_cpssDxChPortSpeedSet(devNum, portNum, speed);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, speed));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* geMacUnitSpeedSet
*
* DESCRIPTION:
*       Configure GE MAC unit of specific device and port to required speed
*
* INPUTS:
*       devNum - physical device number
*       portNum - physical port number
*       speed  - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS geMacUnitSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;               /* return code */
    GT_U32    portState;        /* current port state (enable/disable) */
    GT_U32    gmiiOffset;       /* gmii speed bit offset */
    GT_U32    miiOffset;        /* mii speed bit offset */
    GT_U32    portSpeedRegAddr; /* address of GE port speed register */
    GT_U32    portGroupId;/*the port group Id - support multi-port-groups device */

    GT_U32 portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* disable port if we need */
    rc = prvCpssDxChPortStateDisableSet(devNum,portNum,&portState);
    if(rc != GT_OK)
        return rc;

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&portSpeedRegAddr);

    gmiiOffset = 6;
    miiOffset  = 5;

    switch (speed)
    {
        case CPSS_PORT_SPEED_10_E:
            if(prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, portSpeedRegAddr, gmiiOffset, 1, 0) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, portSpeedRegAddr, miiOffset, 1, 0);
            if(rc != GT_OK)
                return rc;
            break;

        case CPSS_PORT_SPEED_100_E:
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, portSpeedRegAddr, gmiiOffset, 1, 0) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, portSpeedRegAddr, miiOffset, 1, 1);
            if(rc != GT_OK)
                return rc;
            break;

        case CPSS_PORT_SPEED_1000_E:
        case CPSS_PORT_SPEED_2500_E:
            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_GE_E)
            {
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, portSpeedRegAddr, gmiiOffset, 1, 1);
                if(rc != GT_OK)
                    return rc;

                if((PRV_CPSS_PP_MAC(devNum)->devFamily > CPSS_PP_FAMILY_CHEETAH3_E)
                    && ((CPSS_PORT_INTERFACE_MODE_1000BASE_X_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portMacNum))
                        || (CPSS_PORT_SPEED_2500_E == speed)))
                {
                    GT_BOOL linkDownStatus;

                    rc = prvCpssDxChPortForceLinkDownEnable(devNum,portNum,&linkDownStatus);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    /* configure 1000BaseX Port type, although the
                        interface mode called SGMII from historic reasons */
                    rc = prvCpssDxChPortGePortTypeSet(devNum, portNum,
                                                    CPSS_PORT_INTERFACE_MODE_1000BASE_X_E);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    rc = prvCpssDxChPortInbandAutonegMode(devNum,portNum, CPSS_PORT_INTERFACE_MODE_1000BASE_X_E);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    /* usually it's application's decision enable/disable inband auto-neg.,
                        but to make 1000BaseX and 2.5G establish link it must be enabled */
                    rc = cpssDxChPortInbandAutoNegEnableSet(devNum,portNum,GT_TRUE);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    rc = prvCpssDxChPortForceLinkDownDisable(devNum,portNum,linkDownStatus);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }

                break;
            }
            /* else
                continue toward failure */

        default:
            /* enable port if we need */
            rc = prvCpssDxChPortStateEnableSet(devNum,portNum,portState);
            if(rc != GT_OK)
                return rc;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG); /* GT_BAD_PARAM; */
    }

    /* enable port if we need */
    rc = prvCpssDxChPortStateEnableSet(devNum,portNum,portState);

    return rc;
}

/*******************************************************************************
* xcatGeSpecificConfig
*
* DESCRIPTION:
*       Configure GE MAC unit of XCAT/XCAT2 ports
*
* INPUTS:
*       devNum - physical device number
*       portNum - physical port number
*       speed  - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS xcatGeSpecificConfig
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;       /* return code */
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode; /* current interface mode of port */
    GT_U32 regAddr;     /* register address */
    GT_U32 dpClkOffset; /* DP Clock configuration bit offset */
    GT_U32  data;       /* data to write to register */

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) != GT_TRUE)
    {/* Network Ports case */

        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            && (PRV_CPSS_PP_MAC(devNum)->revision == 0)
            && (speed == CPSS_PORT_SPEED_2500_E))
        {/* NP's of xcat A0 don't support 2.5G */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            if(PRV_CPSS_PP_MAC(devNum)->revision < 3)
            {/* NP's of xcat2 till rev B0 don't support 2.5G */
                if(speed == CPSS_PORT_SPEED_2500_E)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            else
            {/* because in xcat2 this configuration is per port we do it
                in runtime and not in init */
                /* 1 - dpClk, 0 - coreClk */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.buffMemMppmConfig;
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,(portNum/4), 1,
                        (speed == CPSS_PORT_SPEED_2500_E) ? 1 : 0);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* 1 - syncByPass, 0 - DoNotBypas */
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,6+(portNum/4), 1,
                        (speed == CPSS_PORT_SPEED_2500_E) ? 0 : 1);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if(portNum%4 == 0)
                {/* enable/disable fix */
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                            perPortRegs[portNum].serdesCnfg;
                    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, 13, 3,
                            (speed == CPSS_PORT_SPEED_2500_E) ? 0x7 : 0x0);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
        }

        if((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) || (ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E))
        {
            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                PRV_CPSS_DXCH_XCAT_RM_METAL_FIX_SGMII25G_DPCLK_SEL_WA_E))
            {
                if(PRV_CPSS_DXCH_XCAT_B1_ONLY_CHECK_MAC(devNum))
                {
                    dpClkOffset = 26 + (portNum/4);
                    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.metalFix,
                            dpClkOffset, 1,
                            (speed == CPSS_PORT_SPEED_2500_E) ? 1 : 0);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
        } /* end of if(ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) */

    } /* end of network ports case */
    else
    {
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_RM_METAL_FIX_SGMII25G_DPCLK_SEL_WA_E))
        {/* enable dp_clk and sync_fifo bypass disable */
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portNum,PRV_CPSS_PORT_GE_E,&regAddr);
                data = (speed == CPSS_PORT_SPEED_2500_E) ? 1 : 0;
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        regAddr, 2, 1, data) != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                }
                data = (speed == CPSS_PORT_SPEED_2500_E) ? 0 : 1;
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        regAddr, 3, 1, data) != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                }
            }
            else if(PRV_CPSS_DXCH_XCAT_B1_ONLY_CHECK_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.metalFix;
                dpClkOffset = 22 + (portNum-24);
                data = (speed == CPSS_PORT_SPEED_2500_E) ? 1 : 0;
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        regAddr, dpClkOffset, 1, data);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            /* SGMII or HGS mode */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
            data = (speed == CPSS_PORT_SPEED_2500_E) ? 1 : 0;
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, portNum + 2, 1, data) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }
    }/* end of stack ports case */

    if((CPSS_PORT_SPEED_1000_E == speed) && (portNum != CPSS_CPU_PORT_NUM_CNS))
    {
        rc = prvCpssDxChPortInbandAutonegMode(devNum, portNum, ifMode);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* xCatSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified xcat/xcat2 device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS xCatSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;   /* return code */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                        | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E
                                        | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E 
                                        | CPSS_BOBCAT3_E);

    /* speed parameter validity check */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        switch(speed)
        {
            case CPSS_PORT_SPEED_20000_E:
            case CPSS_PORT_SPEED_40000_E:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            default:
                break;
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {/* xcat2 supports just GE speeds 10M-2.5G */
        switch(speed)
        {
            case CPSS_PORT_SPEED_10_E:
            case CPSS_PORT_SPEED_100_E:
            case CPSS_PORT_SPEED_1000_E:
            case CPSS_PORT_SPEED_2500_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    /* for ports supporting GE and above need to configure serdes speed
     * specific registers
     */
    if((PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_GE_E)
        && (portNum != CPSS_CPU_PORT_NUM_CNS))
    {
        rc = serdesSpeedSet(devNum,portNum,speed);
        if(rc != GT_OK)
            return rc;
    }

    /* configure GE MAC unit for FE and GE speeds */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) <= PRV_CPSS_PORT_GE_E)
    {
        rc = geMacUnitSpeedSet(devNum,portNum,speed);
        if(rc != GT_OK)
            return rc;
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_GE_E)
    {
        rc = xcatGeSpecificConfig(devNum,portNum,speed);
        if(rc != GT_OK)
            return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* lionSerdesSpeedSet
*
* DESCRIPTION:
*       Configure serdes registers uniqiue for specific frequency
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - physical device number
*       portNum - physical port number
*       speed  - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lionSerdesSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;   /* return code */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;    /* interface mode */
    GT_U32 startSerdes = 0;     /* first serdes */
    GT_U32 serdesesNum = 0;     /* serdes quantity */
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM localPort;  /* local port - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM globalPort; /* global port number */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed = CPSS_DXCH_PORT_SERDES_SPEED_NA_E;
                                                        /* serdes frequency to configure */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeedPrev;    /* old serdes frequency */
    GT_U32 regAddr; /* register address */
    GT_U32 regValue;    /* register value */
    GT_U32 fieldOffset; /* offset of configured field in register */
    GT_U32 index;   /* iterator */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                        | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                                                                | CPSS_XCAT3_E | CPSS_XCAT2_E
                                        | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    serdesSpeed = serdesFrequency[ifMode][speed];
    if(serdesSpeed == CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum,portNum,ifMode,&startSerdes,&serdesesNum)) != GT_OK)
        return rc;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* if neighbor port defined as special port need check if serdes frequency defined on
     * his serdeses equal to required for even port */
    if((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (serdesesNum == 2))
    {/* number of serdeses less then needed for this mode i.e. neighbor is special port */
        rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes+2,&serdesSpeedPrev);
        if (rc != GT_OK)
        {
            return rc;
        }
        if(serdesSpeedPrev != serdesSpeed)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* if special mode required and port is odd, check that even port configuration compatible to required */
    if((ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E) && ((localPort%2) != 0))
    {
        globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,localPort-1);
        if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,globalPort) == CPSS_PORT_INTERFACE_MODE_XGMII_E)
        {
            rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes-2,&serdesSpeedPrev);
            if (rc != GT_OK)
            {
                return rc;
            }
            if(serdesSpeedPrev != serdesSpeed)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            else
                return GT_OK; /* needed speed already configured */
        }
    }

    rc = serdesSpeedSet(devNum,portNum,speed);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* configure GE MAC unit for FE and GE speeds */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_GE_E)
    {
        rc = geMacUnitSpeedSet(devNum,portNum,speed);
        if(rc != GT_OK)
            return rc;
    }

    switch(speed)
    {
        case     CPSS_PORT_SPEED_10_E:
        case     CPSS_PORT_SPEED_100_E:
        case     CPSS_PORT_SPEED_1000_E:
        case     CPSS_PORT_SPEED_2500_E:
        case     CPSS_PORT_SPEED_5000_E:
        case     CPSS_PORT_SPEED_10000_E:
            regValue = 0;
            break;

        case     CPSS_PORT_SPEED_16000_E:
        case     CPSS_PORT_SPEED_20000_E:
            regValue = 0x15;
            break;

        case     CPSS_PORT_SPEED_40000_E:
            regValue = 0x17;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    index = (localPort < 6) ? 0 : 1;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaThresholdOverrideEn[index];
    fieldOffset = (localPort%6)*5;
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,fieldOffset,5,regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_XLG_WS_SUPPORT_WA_E))
        return GT_OK;

    /* Metal_fix[31] - Enable Strict Priority to port10 (40Gbps)
        Metal_fix[30] - Port0 is 20 Gpbs
        Metal_fix[29] - Port2 is 20 Gpbs
        Metal_fix[28] - Port4 is 20 Gpbs
        Metal_fix[27] - Port6 is 20 Gpbs
        Metal_fix[26] - Port8 is 20 Gpbs
        Metal_fix[25] - Port10 is 20 Gpbs
        Metal_fix[24] - Enable fix
        NOTE: - When one of the bits Metal_fix[31:25] is SET, these 2 other configurations must be set as well:
        1. Metal_fix[24]
        2. Scheduler/Scheduler Configuration/Scheduler Configuration/DeficitEnable
    */
    if(localPort%2 != 0)
        return GT_OK;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.global.debug.metalFix;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,24,8,&regValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    if((speed == CPSS_PORT_SPEED_20000_E) || (speed == CPSS_PORT_SPEED_16000_E))
    {
        U32_SET_FIELD_MAC(regValue,6-(localPort/2),1,1);
        /* disable metal fix for 40G */
        if(localPort == 10)
            U32_SET_FIELD_MAC(regValue,7,1,0);

        /* enable metal fix */
        U32_SET_FIELD_MAC(regValue,0,1,1);
    }
    else if(speed == CPSS_PORT_SPEED_40000_E)
    {
        /* disable metal fix for 20G on ports 8 and 10 */
        U32_SET_FIELD_MAC(regValue,1,2,0);

        /* enable metal fix for 40G */
        U32_SET_FIELD_MAC(regValue,7,1,1);

        /* enable metal fix */
        U32_SET_FIELD_MAC(regValue,0,1,1);
    }
    else
    {
        U32_SET_FIELD_MAC(regValue,6-(localPort/2),1,0);
        if(localPort == 10)
            U32_SET_FIELD_MAC(regValue,7,1,0);

        /* if just enable metal fix bit left - disable metal fix */
        if(1 == regValue)
            U32_SET_FIELD_MAC(regValue,0,1,0);
    }

    /* write full configuration at once to avoid state where metal fix enabled
        and no port requiring it set */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,24,8,regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* lionSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified xcat/xcat2 device.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lionSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;   /* return code */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                        | CPSS_CH2_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E
                                        | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    switch(speed)
    {
        case CPSS_PORT_SPEED_12000_E:
        case CPSS_PORT_SPEED_13600_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        default:
            break;
    }

    if(CPSS_CPU_PORT_NUM_CNS == portNum)
    {
        return geMacUnitSpeedSet(devNum,portNum,speed);
    }

    rc = lionSerdesSpeedSet(devNum,portNum,speed);
    if(rc != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLion2PortTypeSet
*
* DESCRIPTION:
*       Sets port type (mostly means which mac unit used) on a specified port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*       speed     - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion2PortTypeSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS   rc; /* return code */
    GT_BOOL     noExtendedMac = GT_FALSE;/* if device has just 3 mini-GOP's */
    GT_PHYSICAL_PORT_NUM localPort;   /* number of port in port group */

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if((9 == localPort) || (11 == localPort))
    {
        switch(PRV_CPSS_PP_MAC(devNum)->devType)
        {
            case CPSS_LION_2_3_THREE_MINI_GOPS_DEVICES_CASES_MAC:
                noExtendedMac = GT_TRUE;
                break;
            case CPSS_LION_2_3_FOUR_MINI_GOPS_DEVICES_CASES_MAC:
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    switch(ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_10_E:
                case CPSS_PORT_SPEED_100_E:
                case CPSS_PORT_SPEED_1000_E:
                case CPSS_PORT_SPEED_2500_E:
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_GE_E;
            break;

        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
            if((speed != CPSS_PORT_SPEED_10000_E) && (speed != CPSS_PORT_SPEED_20000_E)
                && (speed != CPSS_PORT_SPEED_12000_E) && (speed != CPSS_PORT_SPEED_16000_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XLG_E;
            break;

        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            if(speed != CPSS_PORT_SPEED_1000_E)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_GE_E;
            break;

        case CPSS_PORT_INTERFACE_MODE_HX_E:
            if(speed != CPSS_PORT_SPEED_10000_E)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XLG_E;
            break;

        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            if(speed != CPSS_PORT_SPEED_10000_E)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XLG_E;
            break;

        case CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E:
        case CPSS_PORT_INTERFACE_MODE_KR_E:
        case CPSS_PORT_INTERFACE_MODE_SR_LR_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_10000_E:
                    break;
                case CPSS_PORT_SPEED_20000_E:
                    if(noExtendedMac)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    }
                break;
                case CPSS_PORT_SPEED_40000_E:
                    if(noExtendedMac)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    }
                    break;
                case CPSS_PORT_SPEED_100G_E:
                    if(CPSS_PORT_INTERFACE_MODE_KR_E != ifMode)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    }
                break;
                default:
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }
            }

            if(CPSS_PORT_SPEED_10000_E == speed)
            {
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XG_E;
            }
            else
            {
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XLG_E;
            }
            break;

        case CPSS_PORT_INTERFACE_MODE_HGL_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_15000_E:
                case CPSS_PORT_SPEED_16000_E:
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType =
                        PRV_CPSS_PORT_HGL_E;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    /* 40G is XLHGL supported by XLG MAC */
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XLG_E;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_PORT_INTERFACE_MODE_CHGL_12_E:
            if(speed != CPSS_PORT_SPEED_100G_E)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            break;
        case CPSS_PORT_INTERFACE_MODE_XHGS_E:
        case CPSS_PORT_INTERFACE_MODE_XHGS_SR_E:
            switch (speed)
            {
                case CPSS_PORT_SPEED_11800_E:
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XG_E;
                break;
                case CPSS_PORT_SPEED_47200_E:
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XLG_E;
                break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_NA_E:
            /* when port is powered down must set it to XG not XLG(!) to allow
                neighbour ports be configured. NA - used only for loops,
                NOT_EXIST - means port physically not exists */
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = PRV_CPSS_PORT_XG_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    switch(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType)
    {
        case PRV_CPSS_PORT_GE_E:
            rc = prvCpssDxChPortPeriodicFlowControlIntervalSelectionSet(devNum,
                                portNum, CPSS_DXCH_PORT_PERIODIC_FC_TYPE_GIG_E);
            if (rc != GT_OK)
            {
                return rc;
            }
            break;
        case PRV_CPSS_PORT_XG_E:
        case PRV_CPSS_PORT_XLG_E:
        case PRV_CPSS_PORT_HGL_E:
            rc = prvCpssDxChPortPeriodicFlowControlIntervalSelectionSet(devNum,
                                portNum, CPSS_DXCH_PORT_PERIODIC_FC_TYPE_XG_E);
            if (rc != GT_OK)
            {
                return rc;
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}


GT_STATUS bcat2MacGESpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;                        /* return code */
    GT_PHYSICAL_PORT_NUM portMacNum;    /* MAC to which port mapped */
    GT_U32    portSpeedRegAddr; /* address of GE port speed register */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,  portMacNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portMacNum) != PRV_CPSS_PORT_GE_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum, portMacNum, &portSpeedRegAddr);

    switch (speed)
    {
        case CPSS_PORT_SPEED_10_E:
            rc = prvCpssDrvHwPpSetRegField(devNum, portSpeedRegAddr, 5, 2, 0);
            break;

        case CPSS_PORT_SPEED_100_E:
            rc = prvCpssDrvHwPpSetRegField(devNum, portSpeedRegAddr, 5, 2, 1);
            break;

        case CPSS_PORT_SPEED_1000_E:
            rc = prvCpssDrvHwPpSetRegField(devNum, portSpeedRegAddr, 6, 1, 1);
            break;

        default:
            rc = GT_BAD_PARAM;
            break;
    }

    if(GT_OK == rc)
    {
        /* save new port speed in DB */
        PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum) = speed;
    }
    return rc;
}

/*******************************************************************************
* bcat2SpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified bcat2 device.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bcat2SpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc, rc1;                        /* return code */
    GT_PHYSICAL_PORT_NUM portMacNum;    /* MAC to which port mapped */
    GT_U32    portState;        /* current port state (enable/disable) */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,  portMacNum);

    if((!PRV_CPSS_SIP_5_CHECK_MAC(devNum)) &&
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portMacNum) != PRV_CPSS_PORT_GE_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* disable port if we need */
    rc = prvCpssDxChPortStateDisableSet(devNum, portNum, &portState);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = bcat2MacGESpeedSet(devNum,portNum,speed);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* enable port if we need */
    rc1 = prvCpssDxChPortStateEnableSet(devNum, portNum, portState);
    if(rc1 != GT_OK)
    {
        return rc1;
    }

    return rc;
}


/*******************************************************************************
* lion2SpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified xcat/xcat2 device.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion2SpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;                        /* return code */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode; /* current i/f mode of port */
    GT_U32               portGroup; /* number of local core */
    GT_PHYSICAL_PORT_NUM localPort;   /* number of port in port group */
    PRV_CPSS_PORT_TYPE_ENT originalPortType; /* keep original port type to restore on failure */
    MV_HWS_PORT_STANDARD portMode;  /* port interface in hwServices format */
    GT_U32               macNum,     /* number of MAC used by port for given ifMode */
                         pcsNum,     /* number of PCS used by port for given ifMode */
                         sdVecSize;  /* size of serdes redundancy array */
    GT_U32               *sdVectorPtr; /* serdes redundancy array */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                        | CPSS_CH2_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E
                                        | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(CPSS_CPU_PORT_NUM_CNS == portNum)
    {
        return geMacUnitSpeedSet(devNum,portNum,speed);
    }

    if(CPSS_NULL_PORT_NUM_CNS == portNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortPizzaArbiterIfCheckSupport(devNum,portNum,speed);
    if (GT_OK != rc)
    {
        return rc;
    }


    /* save original port type to restore it if new configuration fails */
    originalPortType = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType;

    ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum);
    rc = prvCpssDxChLion2PortTypeSet(devNum,portNum,ifMode,speed);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    portGroup = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rc = hwsPortsParamsCfgGet(devNum, portGroup,
                              localPort, portMode, &macNum, &pcsNum,
                              &sdVecSize, &sdVectorPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(NA_NUM == macNum)
    {
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = originalPortType;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((macNum%2 != 0) && (sdVecSize > 1))
    {/* 1 serdes modes already checked */
        GT_U32 localPortTmp;
        GT_U32 macNumTmp;
        GT_U32 pcsNumTmp;
        GT_U32 sdVecSizeTmp;
        GT_U32 *sdVectorTmpPtr;
        GT_PHYSICAL_PORT_NUM firstInCouple;

        firstInCouple = portNum-1;
        /* if even neighbor port not configured check not needed */
        if((PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,firstInCouple) !=
                                            CPSS_PORT_INTERFACE_MODE_NA_E)
           && (PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,firstInCouple) !=
                                                        CPSS_PORT_SPEED_NA_E))
        {
            rc = prvCpssLion2CpssIfModeToHwsTranslate(
                        PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,firstInCouple),
                        PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,firstInCouple),
                        &portMode);
            if(rc != GT_OK)
                return rc;

            localPortTmp = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,
                                                            firstInCouple);
            if(localPortTmp >= 12)
            {/* to prevent coverity warning OVERRUN_STATIC */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            rc = hwsPortsParamsCfgGet(devNum, portGroup,
                                        localPortTmp, portMode, &macNumTmp,
                                        &pcsNumTmp, &sdVecSizeTmp, &sdVectorTmpPtr);
            if(rc != GT_OK)
            {
                return rc;
            }
            if(NA_NUM == macNumTmp)
            {/* i.e. current port can't support required ifMode */
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = originalPortType;
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(sdVecSizeTmp >= 2)
            {/* i.e. current port's serdeses occupied by its couple even port */
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = originalPortType;
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
    }

    if((macNum%4 != 0) && (sdVecSize > 1))
    {/* 1 serdes modes already checked */
        GT_U32 localPortTmp;
        GT_U32 macNumTmp;
        GT_U32 pcsNumTmp;
        GT_U32 sdVecSizeTmp;
        GT_U32 *sdVectorTmpPtr;
        GT_PHYSICAL_PORT_NUM firstInQuadruplet;

        firstInQuadruplet = (macNum != 14) ? (portNum&0xFFFFFFFC) :
                                            (portNum-2) /* i.e. local port 9 */;
        /* if first port of quadruplet not configured check not needed */
        if((PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,firstInQuadruplet) !=
                                            CPSS_PORT_INTERFACE_MODE_NA_E)
           && (PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,firstInQuadruplet) !=
                                                        CPSS_PORT_SPEED_NA_E))
        {
            rc = prvCpssLion2CpssIfModeToHwsTranslate(
                        PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,firstInQuadruplet),
                        PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,firstInQuadruplet),
                        &portMode);
            if(rc != GT_OK)
                return rc;

            localPortTmp = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,
                                                            firstInQuadruplet);
            if(localPortTmp >= 12)
            {/* to prevent coverity warning OVERRUN_STATIC */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            rc = hwsPortsParamsCfgGet(devNum, portGroup,
                                      localPortTmp, portMode, &macNumTmp, &pcsNumTmp,
                                      &sdVecSizeTmp, &sdVectorTmpPtr);
            if(rc != GT_OK)
            {
                return rc;
            }
            if(NA_NUM == macNumTmp)
            {/* i.e. current port can't support required ifMode */
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = originalPortType;
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
            {
                GT_U32 sliceNumUsed; /* number od pizza slices used by neighbor port */

                rc = cpssDxChPortPizzaArbiterIfPortStateGet(devNum, portGroup,
                                                            localPortTmp, &sliceNumUsed);
                if(rc != GT_OK)
                {
                    return rc;
                }
                if(sliceNumUsed > 2)
                {
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = originalPortType;
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
            else if(sdVecSizeTmp >= 4)
            {/* i.e. current port's serdeses occupied by first port in its quadruplet */
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = originalPortType;
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
    }

    /* update addresses of mac registers accordingly to used MAC GE/XLG/etc. */
    rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, portNum, ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if((GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_LION2_HGL_MAC_INT_WA_E))
       && (PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum)->ppEventsCfg[portGroup].intVecNum !=
           CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS))
    {/* mask the HGL MAC interrupts if the portInterface != HGL*/
        GT_32  intKey;

        (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

        /* unmask - i.e. event enable.                */
        /* prvCpssDrvEventsMask may return not GT_OK  */
        /* if interrupt was not binded by application */
        /* or interrupts are not used.                */
        /* Need to ignore this fail.                  */
        (GT_VOID)prvCpssDrvEventsMask(devNum,
            lion2PortUnitHglSumIndex(localPort),
            ((ifMode != CPSS_PORT_INTERFACE_MODE_HGL_E)
             || (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType !=
                        PRV_CPSS_PORT_HGL_E)));
        (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    }

    /* save new port speed in DB */
    PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portNum) = speed;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3PortSerdesSet
*
* DESCRIPTION:
*       Sets SERDES speed configurations for specified port on specified device.
*       The ports that are members of same SERDES group MUST work with same speed.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PortSerdesSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_U32                regAddr;       /* register address */
    GT_U32                regValue;      /* register value */
    CPSS_PORT_SPEED_ENT   existSpeed;    /* existed speed */
    GT_BOOL               oldPowerUp;    /* serdes lanes old power up state */
    GT_U32                serdesGroup;   /* SERDES group */
    GT_PHYSICAL_PORT_NUM  checkPortNum;  /* checkd port in serdes group */
    GT_BOOL         serdesSpeed_2500_En; /* GT_TRUE  - serdes speed is 2.5 gig.
                                            GT_FALSE - other serdes speed */
    GT_U32                i;    /* counter */
    GT_STATUS rc;               /* return code */
    GT_U32  portSerdesGroup;  /* port group number */

    rc = cpssDxChPortSpeedGet(devNum, portNum, &existSpeed);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Set SERDES when the port speed was changed from 2.5 gig or to 2.5 gig. */
    if((existSpeed == speed) ||
      ((speed != CPSS_PORT_SPEED_2500_E)&& (existSpeed != CPSS_PORT_SPEED_2500_E)))
    {
        return GT_OK;
    }

    serdesGroup = portNum>>2;
    for(i = 0; i < 4; i++)
    {
        checkPortNum = i + (serdesGroup<<2);

        /* check speed setting for all group members those already
           "SERDES Powered UP" except of the port for setting */
        if(checkPortNum == portNum)
        {
            continue;
        }
       /* Get power up or down state to port and serdes */
        rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum,
                                                  checkPortNum,
                                                  &oldPowerUp);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(oldPowerUp == GT_TRUE)
        {
            /* Get SERDES speed mode (2.5 gig / 1 gig ) */
            rc = prvCpssDxCh3SerdesSpeedModeGet(devNum, checkPortNum,
                                                &serdesSpeed_2500_En);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Speed of the group member is different from speed for setting */
            /* Members of same SERDES group MUST work with same speed.*/

            if(serdesSpeed_2500_En == GT_TRUE)
            {
                if(speed != CPSS_PORT_SPEED_2500_E)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
            else
            {
                if(speed == CPSS_PORT_SPEED_2500_E)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
        }
    }

    /* Configure serdes lanes power Down */
    rc = cpssDxChPortSerdesGroupGet(devNum, portNum, &portSerdesGroup);
    if(rc != GT_OK)
    {
        return rc;
    }
    rc = prvCpssDxCh3PortGroupSerdesPowerStatusSet(devNum, portSerdesGroup,
                                                   GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* NP ref clock configurations */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesSpeed1[0];
    regValue = (speed == CPSS_PORT_SPEED_2500_E) ? PRV_SERDES_SPEED_2500_REG_VAL_CNS :
                                                    PRV_SERDES_SPEED_1000_REG_VAL_CNS;
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                            regAddr, regValue) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* Setting electrical parameters */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesSpeed2[0];
    regValue = (speed == CPSS_PORT_SPEED_2500_E) ? 0x000001F7 : 0x00000036;
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                            regAddr, regValue) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesReservConf[0];
    regValue = (speed == CPSS_PORT_SPEED_2500_E) ? 0x0000F0F0 : 0x0000F1F0;
    /* bit 12 is for metal fix enabling */
    if (prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               regAddr, 0xFFFFEFFF, regValue) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* NP SERDES electrical configurations */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesIvrefConf1[0];
    regValue = (speed == CPSS_PORT_SPEED_2500_E) ? 0x0000575A : 0x00005555;
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                            regAddr, regValue) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesIvrefConf2[0];
    regValue = (speed == CPSS_PORT_SPEED_2500_E) ? 0x00000003 : 0x00000001;
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                            regAddr, regValue) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesMiscConf[0];
    regValue = (speed == CPSS_PORT_SPEED_2500_E) ? 0x00000008 : 0x00000009;
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                            regAddr, regValue) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cheetah1_3_GePortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified GE port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetah1_3_GePortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;           /* return code */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode; /* current port interface mode */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_XCAT_E | CPSS_XCAT3_E
                                                                                        | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E
                                                                                        | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if((CPSS_PORT_SPEED_2500_E == speed) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH3_E))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
    if((CPSS_PORT_INTERFACE_MODE_1000BASE_X_E == ifMode) &&
        (speed <= CPSS_PORT_SPEED_100_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* Set SERDES */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if(portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            rc = prvCpssDxCh3PortSerdesSet(devNum, portNum, speed);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    rc = geMacUnitSpeedSet(devNum,portNum,speed);

    return rc;
}

/*******************************************************************************
* cheetah3_XgPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified XG port on specified DxCh3 device.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetah3_XgPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_BOOL oldPowerUp;             /* XAUI serdes lanes old power up state */
    GT_U32  serdesSpeedRegAddr;     /* Serdes Speed register address */
    GT_U32  fieldValue;             /* register field value */
    GT_U32  lane;                   /* iterator */
    GT_STATUS rc;                   /* return status */
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;  /* current interface mode of port */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;    /* serdes frequency */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                                                                    | CPSS_CH2_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                                                                                | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E
                                                                                                | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    serdesSpeed = serdesFrequency[ifMode][speed];
    if(serdesSpeed == CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    switch (speed)
    {
        case CPSS_PORT_SPEED_16000_E:
        case CPSS_PORT_SPEED_10000_E:
            /* DxCh3 XG ports < 24 support 10G only */
            if ((speed == CPSS_PORT_SPEED_16000_E) && (portNum < 24))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            if(speed == CPSS_PORT_SPEED_10000_E)
            {
                /* the speed for ports < 24 cannot be changed*/
                if(portNum < 24)
                {
                    return GT_OK;
                }

                fieldValue = 0x8;
            }
            else
            {
                fieldValue = 0xE;
            }

            /* Configure XAUI serdes lanes power Down */
            rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum, portNum, &oldPowerUp);
            if(rc != GT_OK)
            {
                return rc;
            }

            rc = PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->setSerdesPowerStatus(
                                                            devNum, portNum,
                                                            CPSS_PORT_DIRECTION_BOTH_E,
                                                            0xF, GT_FALSE);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Configure XAUI speeds */

            for(lane = 0; lane < 4; lane++)
            {
                serdesSpeedRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                          macRegs.perPortRegs[portNum].serdesSpeed1[lane];
                rc = prvCpssDrvHwPpSetRegField(devNum, serdesSpeedRegAddr,
                                                 2, 7, fieldValue);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            if(oldPowerUp == GT_TRUE)
            {
               return PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->setSerdesPowerStatus(
                                                            devNum, portNum,
                                                            CPSS_PORT_DIRECTION_BOTH_E,
                                                            0xF, GT_TRUE);
            }

            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cheetah1_2_XgPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified XG port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cheetah1_2_XgPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;   /* return code */
    GT_U32 regAddr; /* register address */
    GT_U32 regValue;/* register value */
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;  /* current interface mode of port */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed; /* serdes speed required for
                                                        given port speed */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                                                                                | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E
                                                                                                | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    serdesSpeed = serdesFrequency[ifMode][speed];
    if(serdesSpeed == CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    switch (speed)
    {
        case CPSS_PORT_SPEED_2500_E:
        case CPSS_PORT_SPEED_5000_E:
        break;

        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_12000_E:

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                smiRegs.smi10GePhyConfig0[portNum];

            regValue = (speed == CPSS_PORT_SPEED_10000_E) ? 0 : 1;

            /* Reset the XAUI PHY */
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 0, 1, 0) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            /* Set HyperG.Stack port speed */
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 6, 2, regValue) !=
                GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            /* Set the XAUI PHY to 1 */
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 0, 1, 1) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_STATUS rc;   /* return code */
    PRV_CPSS_DXCH_PORT_SPEED_GET_FUN speedGetFuncPtr; /* pointer to
            per device_family function returning current port speed */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(speedPtr);

    if((speedGetFuncPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->devObj.portPtr->
                                                getPortSpeed) == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    rc = speedGetFuncPtr(devNum, portNum, speedPtr);

    return rc;
}

/*******************************************************************************
* cheetah1_3_PortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - speed not appropriate for interface mode
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS cheetah1_3_PortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_U32    value;                /* register value */
    GT_U32    regAddr;              /* register address */
    GT_BOOL   serdesSpeed_2500_En;  /* GT_TRUE  - serdes speed is 2.5 gig.
                                       GT_FALSE - other serdes speed */
    GT_STATUS rc;                   /* return code */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode; /* interface mode */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                                    CPSS_XCAT2_E | CPSS_LION2_E
                                                                                                        | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if((CPSS_CPU_PORT_NUM_CNS == portNum) ||
       (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E))
    {
        PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,&regAddr);

        /* read GMII Speed */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 1, 1, &value) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

        /* Port speed 1 gig or 2.5 gig according to the value. */
        if (value == 1)
        {
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            {
                if(portNum == CPSS_CPU_PORT_NUM_CNS)
                {
                    *speedPtr = CPSS_PORT_SPEED_1000_E;
                    return GT_OK;
                }

                /* Get SERDES speed mode (2.5 gig / 1 gig ) */
                rc = prvCpssDxCh3SerdesSpeedModeGet(devNum, portNum,
                                                    &serdesSpeed_2500_En);
                if(rc != GT_OK)
                {
                    return rc;
                }

                *speedPtr = (serdesSpeed_2500_En == GT_TRUE) ? CPSS_PORT_SPEED_2500_E
                                                                : CPSS_PORT_SPEED_1000_E;
            }
            else /* CH2 */
            {
                *speedPtr = CPSS_PORT_SPEED_1000_E;
            }
        }
        else
        {
            /* read MII Speed */
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 2, 1, &value) != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

            *speedPtr = (value == 1) ? CPSS_PORT_SPEED_100_E : CPSS_PORT_SPEED_10_E;
        }
    }
    else
    {
        rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
        }

        if((ifMode == CPSS_PORT_INTERFACE_MODE_QX_E) ||
            (ifMode == CPSS_PORT_INTERFACE_MODE_HX_E))
        {
            if( (25 != portNum) && (26 != portNum) )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
                hxPortConfig0[portNum-25];
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 5, 1, &value) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            *speedPtr = (value == 0) ? CPSS_PORT_SPEED_2500_E : CPSS_PORT_SPEED_5000_E;
        }
        else
        {/* 10 Gb interface - can be either 10 or 12 Gb */
            if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
            {
                /* DxCh3 XG ports < 24 support 10G only */
                if(portNum < 24)
                {
                    *speedPtr = CPSS_PORT_SPEED_10000_E;
                    return GT_OK;
                }

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                          macRegs.perPortRegs[portNum].serdesSpeed1[0];

                rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        regAddr, 2, 7, &value);
                if(rc != GT_OK)
                {
                    return rc;
                }

                switch(value)
                {
                    case 0x8:
                        *speedPtr = CPSS_PORT_SPEED_10000_E;
                        break;
                    case 0xE:
                        *speedPtr = CPSS_PORT_SPEED_16000_E;
                        break;
                    default:
                        *speedPtr = CPSS_PORT_SPEED_NA_E;
                        break;
                }
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GePhyConfig0[portNum];
                if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        regAddr, 6, 2, &value) != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                }
                if (value == 0)
                {
                    *speedPtr = CPSS_PORT_SPEED_10000_E;
                }
                else if (value == 1)
                {
                    *speedPtr = CPSS_PORT_SPEED_12000_E;
                }
                else /* 2 and 3 are reserved values */
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* flexLinkSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified flex link on specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - speed not appropriate for interface mode
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS flexLinkSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_STATUS   rc; /* return code */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;    /* interface mode */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;    /* current serdes frequency */
    GT_U32      serdes; /* serdes number */
    GT_U32      serdesNum;  /* serdes quantity */
    GT_U32      refClockId; /* reference clock */
    GT_U32      regValue;   /* register value */
    GT_U32      portGroupId;    /* port group Id for multi-port-group support */
    GT_U32      regAddr;    /* register address */
    GT_U32      portMacNum; /* MAC number connected to port */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                                        | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,
                                                                 portMacNum);

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* NA interface */
    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        *speedPtr = CPSS_PORT_SPEED_NA_E;
        return GT_OK;
    }

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                             portMacNum);

    if((CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        || (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        || (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum)))
    {
        *speedPtr = PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum);
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        if((rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode, &serdes,
                                                    &serdesNum)) != GT_OK)
        {
            return rc;
        }

        rc = prvGetLpSerdesSpeed(devNum,portGroupId,serdes,&serdesSpeed);
        if (rc != GT_OK)
        {
            if ((GT_NOT_SUPPORTED == rc) || (GT_NOT_INITIALIZED == rc))
            {/* if port/serdes doesn't support speed parameter don't fail the function */
             /* or serdes was not initialized */
                *speedPtr = CPSS_PORT_SPEED_NA_E;
                return GT_OK;
            }
            else
                return rc;
        }
        *speedPtr = ifModeSerdesToPortSpeed[ifMode][serdesSpeed];
    }
    else /* xCat A0 */
    {
        switch (PRV_CPSS_DXCH_PP_MAC(devNum)->port.serdesRefClock)
        {
            case CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_25_SINGLE_ENDED_E:
                refClockId = 0;
                break;
            case CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E:
            case CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E:
            case CPSS_DXCH_PP_SERDES_REF_CLOCK_INTERNAL_125_E:
                refClockId = 1;
                break;
            case CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_SINGLE_ENDED_E:
            case CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E:
                refClockId = 2;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        /* Get SERDES speed configuration */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesSpeed1[0];
        if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        if (regValue == speedRegValue[0][refClockId])
        {
            serdesSpeed = CPSS_DXCH_PORT_SERDES_SPEED_1_25_E;
        }
        else if (regValue == speedRegValue[1][refClockId])
        {
            serdesSpeed = CPSS_DXCH_PORT_SERDES_SPEED_3_125_E;
        }
        else if (regValue == speedRegValue[2][refClockId])
        {
            serdesSpeed = CPSS_DXCH_PORT_SERDES_SPEED_6_25_E;
        }
        else if (regValue == speedRegValue[3][refClockId])
        {
            serdesSpeed = CPSS_DXCH_PORT_SERDES_SPEED_3_75_E;
        }
        else
        {
            serdesSpeed = CPSS_DXCH_PORT_SERDES_SPEED_NA_E;
            *speedPtr = CPSS_PORT_SPEED_NA_E;
            return GT_OK;
        }

        *speedPtr = ifModeSerdesToPortSpeed[ifMode][serdesSpeed];
    }

    if(*speedPtr <= CPSS_PORT_SPEED_1000_E)
    {/* for all types of ASIC's for FE/GE ports get speed from HW for case
        auto-negotiation is enabled */
        PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum, portMacNum, &regAddr);
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 1,
                                                2, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        if ((regValue & 0x1) == 0)
        {
            *speedPtr = ((regValue >> 1) == 1) ? CPSS_PORT_SPEED_100_E:
                CPSS_PORT_SPEED_10_E;
        }
        else
        {
            *speedPtr = CPSS_PORT_SPEED_1000_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* xcatPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified flex link on specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - speed not appropriate for interface mode
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS xcatPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_STATUS   rc;   /* return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* register value */
    GT_U32      startSerdes;/* number of first serdes used by port */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT  serdesSpeed;   /* speed of serdes */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                        | CPSS_CH2_E | CPSS_LION_E | CPSS_LION2_E
                                                                                | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(CPSS_CPU_PORT_NUM_CNS == portNum)
    {
        *speedPtr = CPSS_PORT_SPEED_1000_E;
        return GT_OK;
    }

    if(PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        rc = flexLinkSpeedGet(devNum,portNum,speedPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,&regAddr);

        /* read GMII Speed */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 1, 1, &value) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

        if (value == 1)
        {
            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
            {/* MAC of FE port of xcat can be configured to 1G speed, but there won't be
                traffic because lack of synchronization with lower layer */
                *speedPtr = CPSS_PORT_SPEED_1000_E;
                return GT_OK;
            }

            startSerdes = portNum/4;
            rc = prvGetLpSerdesSpeed(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,startSerdes,&serdesSpeed);
            if (GT_NOT_INITIALIZED == rc)
            {
                /* or serdes was not initialized */
                *speedPtr = CPSS_PORT_SPEED_NA_E;
                return GT_OK;
            }
            else if (rc != GT_OK)
            {
                return rc;
            }
            /* can't determine serdes speed */
            if (serdesSpeed == CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            else
            {
                switch(serdesSpeed)
                {
                    case CPSS_DXCH_PORT_SERDES_SPEED_1_25_E: /* sgmii */
                    case CPSS_DXCH_PORT_SERDES_SPEED_5_E:    /* qsgmii */
                        *speedPtr = CPSS_PORT_SPEED_1000_E;
                    break;

                    case CPSS_DXCH_PORT_SERDES_SPEED_3_125_E: /* sgmii 2.5G */
                        *speedPtr = CPSS_PORT_SPEED_2500_E;
                    break;

                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }
            }
        }
        else
        {
            /* read MII Speed */
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 2, 1, &value) != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

            *speedPtr = (value == 1) ? CPSS_PORT_SPEED_100_E : CPSS_PORT_SPEED_10_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* lionPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified flex link on specified device.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - speed not appropriate for interface mode
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lionPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                        | CPSS_CH2_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    if(!PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        if(CPSS_CPU_PORT_NUM_CNS == portNum)
        {
            *speedPtr = CPSS_PORT_SPEED_1000_E;
            return GT_OK;
        }
    }

    return flexLinkSpeedGet(devNum, portNum, speedPtr);
}

/*******************************************************************************
* internal_cpssDxChPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        1.This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*       2.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    CPSS_MACDRV_OBJ_STC *portMacObjPtr; /* port callback pointer */
    GT_STATUS rc = GT_OK;               /* return code */
    GT_BOOL doPpMacConfig = GT_TRUE;    /* do switch mac code indicator */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(speedPtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_NOT_EXISTS_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* run MACPHY callback */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacSpeedGetFunc(devNum,portMacNum,
                                  speedPtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,speedPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* if callback set doPpMacConfig as TRUE - run prvCpssDxChPortSpeedGet - switch MAC configuration code */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortSpeedGet(devNum,portNum,speedPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* run "post stage" callback */
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacSpeedGetFunc(devNum,portMacNum,
                                  speedPtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,speedPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        1.This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*       2.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSpeedGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, speedPtr));

    rc = internal_cpssDxChPortSpeedGet(devNum, portNum, speedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, speedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChLionPortSpeedHwGet
*
* DESCRIPTION:
*       Gets from HW speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to configured port speed (in case of GE when auto-neg.
*                  enabled real speed could differ from configured)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - speed not appropriate for interface mode
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionPortSpeedHwGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_STATUS   rc; /* return code */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;    /* interface mode */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;    /* current serdes frequency */
    GT_U32      serdes; /* serdes number */
    GT_U32      serdesNum;  /* serdes quantity */
    GT_U32      regValue;   /* register value */
    GT_U32      portGroupId;    /* port group Id for multi-port-group support */
    GT_U32      regAddr;    /* register address */
    GT_U32      i;          /* iterator */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum);
    if(CPSS_PORT_INTERFACE_MODE_NA_E == ifMode)
    {/* attempt to enable call of this function before ifMode defined */
        rc = prvCpssDxChPortInterfaceModeHwGet(devNum,portNum,&ifMode);
        if(rc != GT_OK)
            return GT_OK;
    }

    if(CPSS_PORT_INTERFACE_MODE_NA_E == ifMode)
    {/* i.e. MAC in reset, data speed could not be found */
        return GT_OK;
    }

    if((ifMode != CPSS_PORT_INTERFACE_MODE_QSGMII_E) &&
       (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) != PRV_CPSS_PORT_FE_E))
    {
        if ((rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode, &serdes, 
                                                    &serdesNum)) != GT_OK)
        {
            return rc;
        }
    
        /* check if all serdes in port are initialized */
        for (i = serdes; i < serdes + serdesNum;i++)
        {
            rc = prvCpssDxChLpCheckSerdesInitStatus(devNum, portGroupId, i);
            if((rc != GT_OK) && (rc != GT_NOT_INITIALIZED))
            {
                return rc;
            }
            if (rc == GT_NOT_INITIALIZED)
            {/* SERDES was not initialized */
                return GT_OK;
            }
        }
    
        rc = prvGetLpSerdesSpeed(devNum,portGroupId,serdes,&serdesSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
    
        if((CPSS_DXCH_PORT_SERDES_SPEED_3_125_E == serdesSpeed) &&
                    (CPSS_PORT_INTERFACE_MODE_1000BASE_X_E == ifMode))
        {/* SGMII 2.5G in HW implemented as 1000BaseX */
            *speedPtr = CPSS_PORT_SPEED_2500_E;
            return GT_OK;
        }
        
        *speedPtr = ifModeSerdesToPortSpeed[ifMode][serdesSpeed];
    }
    else
    {/* if QSGMII or FE */
        *speedPtr = CPSS_PORT_SPEED_1000_E;
    }

    if(CPSS_PORT_SPEED_1000_E == *speedPtr)
    {/* read configured speed - not status */
        PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum, portNum, &regAddr);
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 5, 
                                                                    2, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (((regValue & 0x2) == 0) ||
            (PRV_CPSS_PORT_FE_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum)))
        {
            *speedPtr = ((regValue & 0x1) == 1) ? CPSS_PORT_SPEED_100_E:
                CPSS_PORT_SPEED_10_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLion2PortSpeedHwGet
*
* DESCRIPTION:
*       Gets from HW speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - speed not appropriate for interface mode
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion2PortSpeedHwGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */
    GT_STATUS   rc;     /* return code */
    MV_HWS_PORT_STANDARD    portMode = NON_SUP_MODE; /* port mode in HWS format */
    GT_U32  portMacMap;
    GT_U32 localPort;   /* number of port in local core */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacMap);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacMap);

    if(PRV_CPSS_GE_PORT_GE_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacMap))        
    {
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortInterfaceGet(devNum[%d], portGroup[%d], phyPortNum[%d], *portModePtr)", devNum, portGroupId, portMacMap&0xFFFFFFFC);
        rc = mvHwsPortInterfaceGet(devNum, portGroupId, portMacMap&0xFFFFFFFC, &portMode);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
        if (QSGMII == portMode)
        {
            goto speedGet;
        }
    }

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortInterfaceGet(devNum[%d], portGroup[%d], phyPortNum[%d], *portModePtr)", devNum, portGroupId, localPort);
    rc = mvHwsPortInterfaceGet(devNum, portGroupId, localPort, &portMode);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
    }
    
speedGet:
    switch(portMode)
    {
        case _100Base_FX:
            *speedPtr = CPSS_PORT_SPEED_100_E;
            break;

        case SGMII:
        case QSGMII:
            {/* HWS doesn't support 10/100M configuration */
                GT_U32 regAddr;
                GT_U32 value;

                /* read configured speed - not status */
                PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum, portMacMap, &regAddr);
                rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 5, 
                                                                            2, &value);
                if (rc != GT_OK)
                {
                    return rc;
                }

                if ((value & 0x2) == 0)
                {
                    *speedPtr = ((value & 0x1) == 1) ? CPSS_PORT_SPEED_100_E:
                        CPSS_PORT_SPEED_10_E;
                }
                else
                {
                    *speedPtr = CPSS_PORT_SPEED_1000_E;
                }
            }

            break;

        case SGMII2_5:
            *speedPtr = CPSS_PORT_SPEED_2500_E;
            break;

        case _1000Base_X:
            *speedPtr = CPSS_PORT_SPEED_1000_E;
            break;

        case _10GBase_KX4:
        case _10GBase_KX2:
        case _10GBase_KR:
        case RXAUI:
        case _10GBase_SR_LR:
        case INTLKN_4Lanes_3_125G:
            *speedPtr = CPSS_PORT_SPEED_10000_E;
            break;

        case _20GBase_KX4:
        case _20GBase_KR:
        case _20GBase_SR_LR:
        case INTLKN_8Lanes_3_125G:
        case INTLKN_4Lanes_6_25G:
            *speedPtr = CPSS_PORT_SPEED_20000_E;
            break;

        case _40GBase_KR:
        case XLHGL_KR4:
        case _40GBase_SR_LR:
        case INTLKN_8Lanes_6_25G:
            *speedPtr = CPSS_PORT_SPEED_40000_E;
            break;

        case _100GBase_KR10:
        case _100GBase_SR10:
        case CHGL:
        case INTLKN_12Lanes_6_25G:
        case INTLKN_12Lanes_10_3125G:
        case INTLKN_12Lanes_12_5G:
        case INTLKN_16Lanes_6_25G:
        case INTLKN_16Lanes_10_3125G:
        case INTLKN_16Lanes_12_5G:
        case INTLKN_16Lanes_3_125G:
        case INTLKN_24Lanes_6_25G:
        case INTLKN_24Lanes_3_125G:
            *speedPtr = CPSS_PORT_SPEED_100G_E;
            break;

        case _12_5GBase_KR:
            *speedPtr = CPSS_PORT_SPEED_12000_E;
            break;

        case HGL:
            *speedPtr = CPSS_PORT_SPEED_15000_E;
            break;

        case HGL16G:
            *speedPtr = CPSS_PORT_SPEED_16000_E;
            break;

        default:
            *speedPtr = CPSS_PORT_SPEED_NA_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortSpeedHwGet
*
* DESCRIPTION:
*       Gets from HW speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        Lion; xCat; Lion2; xCat2; Bobcat2; xCat3; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - speed not appropriate for interface mode
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSpeedHwGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      regValue;   /* register value */
    GT_U32      portGroupId;    /* port group Id for multi-port-group support */
    GT_U32      regAddr;    /* register address */
    GT_U32      portMacMap; /* number of mac mapped to this physical port */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                        | CPSS_CH2_E);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,
                                                                 portMacMap);
    CPSS_NULL_PTR_CHECK_MAC(speedPtr);

    *speedPtr = CPSS_PORT_SPEED_NA_E;

    if(CPSS_CPU_PORT_NUM_CNS == portMacMap)
    {
        PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacMap,&regAddr);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacMap);

        /* read GMII Speed */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                                regAddr, 6, 1, &regValue) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

        if (regValue == 1)
        {
            *speedPtr = CPSS_PORT_SPEED_1000_E;
        }
        else
        {
            /* read MII Speed */
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                        regAddr, 5, 1, &regValue) != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

            *speedPtr = (regValue == 1) ? CPSS_PORT_SPEED_100_E :
                                                    CPSS_PORT_SPEED_10_E;
        }

        return GT_OK;
    }

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_LION_E:
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            rc = prvCpssDxChLionPortSpeedHwGet(devNum,portNum,speedPtr);
            break;

        case CPSS_PP_FAMILY_DXCH_LION2_E:
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            rc = prvCpssDxChLion2PortSpeedHwGet(devNum,portNum,speedPtr);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return rc;
}

