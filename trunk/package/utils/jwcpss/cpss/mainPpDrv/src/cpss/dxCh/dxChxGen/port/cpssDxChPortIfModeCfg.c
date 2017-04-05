/********************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortIfModeCfg.c
*
* DESCRIPTION:
*       CPSS implementation for Port interface mode configuration.
*
*
* FILE REVISION NUMBER:
*       $Revision: 116 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#define CPSS_LOG_IN_MODULE_ENABLE

#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBobKResource.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgResource.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortInterlaken.h>

#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>


#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLion2.h>
/* TM glue */
#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGlue.h>


#include <mvHwsPortInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <private/mvPortModeElements.h>
#include <mac/mvHwsMacIf.h>
#include <pcs/mvHwsPcsIf.h>



/* array defining serdes speed used in given interface mode for given port data speed */
/* APPLICABLE DEVICES:  DxChXcat and above. */
extern CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesFrequency[CPSS_PORT_INTERFACE_MODE_NA_E][CPSS_PORT_SPEED_NA_E];

extern GT_VOID prvCpssDxChPortIlknPrMemInit
(
    GT_VOID
);

/* array defining possible interfaces/ports modes configuration options */
/* APPLICABLE DEVICES:  DxCh */
GT_BOOL supportedPortsModes[PRV_CPSS_XG_PORT_OPTIONS_MAX_E][CPSS_PORT_INTERFACE_MODE_NA_E] =
{                                  /* R_10BIT  R_GMII    MII       SGMII     XGMII     MGMII   1000BASE_X    GMII    MII_PHY    QX          HX      RXAUI   100BASE_FX  QSGMII       XLG   LOCAL_XGMII  KR      HGL       CHGL_12    ILK      SR_LR */
/* PRV_CPSS_XG_PORT_XG_ONLY_E */    {GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE },
/* PRV_CPSS_XG_PORT_HX_QX_ONLY_E */ {GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE },
/* PRV_CPSS_XG_PORT_XG_HX_QX_E */   {GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_FALSE, GT_TRUE , GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_TRUE , GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE },
/* PRV_CPSS_GE_PORT_GE_ONLY_E */    {GT_FALSE, GT_TRUE , GT_TRUE , GT_TRUE , GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE },
/* PRV_CPSS_XG_PORT_XLG_SGMII_E */  {GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_FALSE, GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE },
/* PRV_CPSS_XG_PORT_CG_SGMII_E */   {GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_FALSE, GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_FALSE, GT_FALSE, GT_FALSE, GT_TRUE , GT_TRUE , GT_TRUE , GT_TRUE , GT_FALSE, GT_TRUE  }
};


typedef struct prvIfModeSupportedPortsModes_STC
{
    PRV_CPSS_PORT_TYPE_OPTIONS_ENT  portType;
    CPSS_PORT_INTERFACE_MODE_ENT   *supportedIfList;
}prvIfModeSupportedPortsModes_STC;



CPSS_PORT_INTERFACE_MODE_ENT supportedPortsModes_XG_PORT_XG_ONLY[] =
{
    CPSS_PORT_INTERFACE_MODE_XGMII_E,
    CPSS_PORT_INTERFACE_MODE_NA_E
};

CPSS_PORT_INTERFACE_MODE_ENT supportedPortsModes_XG_PORT_HX_QX_ONLY[] =
{
    CPSS_PORT_INTERFACE_MODE_SGMII_E,
    CPSS_PORT_INTERFACE_MODE_QX_E,
    CPSS_PORT_INTERFACE_MODE_HX_E,
    CPSS_PORT_INTERFACE_MODE_NA_E
};

CPSS_PORT_INTERFACE_MODE_ENT supportedPortsModes_XG_PORT_XG_HX_QX[] =
{
    CPSS_PORT_INTERFACE_MODE_SGMII_E,
    CPSS_PORT_INTERFACE_MODE_XGMII_E,
    CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,
    CPSS_PORT_INTERFACE_MODE_QX_E,
    CPSS_PORT_INTERFACE_MODE_HX_E,
    CPSS_PORT_INTERFACE_MODE_RXAUI_E,
    CPSS_PORT_INTERFACE_MODE_100BASE_FX_E,
    CPSS_PORT_INTERFACE_MODE_NA_E
};

CPSS_PORT_INTERFACE_MODE_ENT supportedPortsModes_GE_PORT_GE_ONLY[] =
{
    CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E,
    CPSS_PORT_INTERFACE_MODE_MII_E,
    CPSS_PORT_INTERFACE_MODE_SGMII_E,

    CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,
    CPSS_PORT_INTERFACE_MODE_GMII_E,
    CPSS_PORT_INTERFACE_MODE_MII_PHY_E,

    CPSS_PORT_INTERFACE_MODE_100BASE_FX_E,
    CPSS_PORT_INTERFACE_MODE_QSGMII_E,
    CPSS_PORT_INTERFACE_MODE_NA_E
};

CPSS_PORT_INTERFACE_MODE_ENT supportedPortsModes_XG_PORT_XLG_SGMII[] =  /* Lion */
{
    CPSS_PORT_INTERFACE_MODE_SGMII_E,
    CPSS_PORT_INTERFACE_MODE_XGMII_E,

    CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,
    CPSS_PORT_INTERFACE_MODE_HX_E,
    CPSS_PORT_INTERFACE_MODE_RXAUI_E,

    CPSS_PORT_INTERFACE_MODE_XLG_E,
    CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E,
    CPSS_PORT_INTERFACE_MODE_NA_E
};

CPSS_PORT_INTERFACE_MODE_ENT supportedPortsModes_XG_PORT_CG_SGMII[] = /* lion2 and above , flex link */
{
    CPSS_PORT_INTERFACE_MODE_SGMII_E,
    CPSS_PORT_INTERFACE_MODE_XGMII_E,

    CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,
    CPSS_PORT_INTERFACE_MODE_HX_E,
    CPSS_PORT_INTERFACE_MODE_RXAUI_E,

    CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E,
    CPSS_PORT_INTERFACE_MODE_KR_E,
    CPSS_PORT_INTERFACE_MODE_HGL_E,
    CPSS_PORT_INTERFACE_MODE_CHGL_12_E,
    CPSS_PORT_INTERFACE_MODE_SR_LR_E,
    CPSS_PORT_INTERFACE_MODE_XHGS_E,
    CPSS_PORT_INTERFACE_MODE_XHGS_SR_E,
    CPSS_PORT_INTERFACE_MODE_NA_E
};



prvIfModeSupportedPortsModes_STC supportedPortsModesList[] =
{
    {  PRV_CPSS_XG_PORT_XG_ONLY_E,     &supportedPortsModes_XG_PORT_XG_ONLY[0]        }
   ,{  PRV_CPSS_XG_PORT_HX_QX_ONLY_E,  &supportedPortsModes_XG_PORT_HX_QX_ONLY[0]     }
   ,{  PRV_CPSS_XG_PORT_XG_HX_QX_E,    &supportedPortsModes_XG_PORT_XG_HX_QX[0]       }
   ,{  PRV_CPSS_GE_PORT_GE_ONLY_E,     &supportedPortsModes_GE_PORT_GE_ONLY[0]        }
   ,{  PRV_CPSS_XG_PORT_XLG_SGMII_E,   &supportedPortsModes_XG_PORT_XLG_SGMII[0]      }
   ,{  PRV_CPSS_XG_PORT_CG_SGMII_E,    &supportedPortsModes_XG_PORT_CG_SGMII[0]       }
   ,{  PRV_CPSS_XG_PORT_OPTIONS_MAX_E, (CPSS_PORT_INTERFACE_MODE_ENT *)NULL           }
};


CPSS_PORT_INTERFACE_MODE_ENT * prvCpssDxChPortTypeSupportedModeListGet
(
    PRV_CPSS_PORT_TYPE_OPTIONS_ENT portType
)
{
    prvIfModeSupportedPortsModes_STC *listPtr;
    GT_U32 i;

    listPtr = &supportedPortsModesList[0];
    for (i = 0 ; listPtr->supportedIfList != NULL; i++,listPtr++)
    {
        if (listPtr->portType == portType)
        {
            return listPtr->supportedIfList;
        }
    }
    return (CPSS_PORT_INTERFACE_MODE_ENT *)NULL;
}


GT_BOOL prvCpssDxChPortTypeSupportedModeCheck
(
    IN PRV_CPSS_PORT_TYPE_OPTIONS_ENT portType,
    IN CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    CPSS_PORT_INTERFACE_MODE_ENT * listPtr;
    GT_U32 i;

    listPtr = prvCpssDxChPortTypeSupportedModeListGet(portType);
    if (listPtr == NULL)
    {
        return GT_FALSE;
    }
    for (i = 0 ; *listPtr != CPSS_PORT_INTERFACE_MODE_NA_E ; i++, listPtr++)
    {
        if (*listPtr == ifMode)
        {
            return GT_TRUE;
        }
    }
    return GT_FALSE;
}

GT_STATUS prvCpssDxChPortSupportedModeCheck
(
    IN  GT_U8                         devNum,
    IN  GT_PHYSICAL_PORT_NUM          portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT  ifMode,
    OUT GT_BOOL                      *isSupportedPtr
)
{
    PRV_CPSS_PORT_TYPE_OPTIONS_ENT portType;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    /* check if interface mode supported by given port */
    portType = PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum);

    /* resOld = supportedPortsModes[PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum)][ifMode]; */
    *isSupportedPtr = prvCpssDxChPortTypeSupportedModeCheck(portType,ifMode);
    return GT_OK;
}



static GT_STATUS cheetah1_3_GePortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

static GT_STATUS cheetah1_3_XgPortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

static GT_STATUS xCatGePortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

static GT_STATUS xCatXgPortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

static GT_STATUS lionFlexLinkIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

static GT_STATUS xCatFePortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

static GT_STATUS lion2FlexLinkIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

static GT_STATUS prvCpssDxChPortXcat3ModeSpeedSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  GT_BOOL                         powerUp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
);

static GT_STATUS prvCpssDxChPortCaelumModeSpeedSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  GT_BOOL                         powerUp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
);

static GT_STATUS cpuPortIfModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
);

static GT_STATUS lionIfModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
);

static GT_STATUS xcatPortIfModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
);

static GT_STATUS cheetah1_3_PortIfModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
);

static GT_STATUS prvCpssDxChPortInterfaceSpeedSupportLion2PlusGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          portMacNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    OUT GT_BOOL                         *supportedPtr
);


/* Array bellow is table of pointers to ifMode configuration functions per DXCH ASIC family per interface mode
 * if mode not supported NULL pointer provided
 */
#if 0
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN portIfModeFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1][CPSS_PORT_INTERFACE_MODE_NA_E]=
    {                                   /* REDUCED_10BIT_E  REDUCED_GMII_E              MII_E                           SGMII                           XGMII                         MGMII       1000BASE_X_E                    GMII                        MII_PHY_E                  QX                             HX                              RXAUI           100BASE_FX_E         QSGMII_E                XLG                    LOCAL_XGMII              KR                      HGL                     CHGL_12                  ILK   SR_LR                   XHGS*/
    /* CPSS_PP_FAMILY_CHEETAH_E     */  {      NULL,       cheetah1_3_GePortIfModeCfg,  cheetah1_3_GePortIfModeCfg, cheetah1_3_GePortIfModeCfg,      cheetah1_3_XgPortIfModeCfg,    NULL,   cheetah1_3_GePortIfModeCfg,     cheetah1_3_GePortIfModeCfg, cheetah1_3_GePortIfModeCfg, NULL,                       NULL,                       NULL,                   NULL,                   NULL,                   NULL,                   NULL,                    NULL,                   NULL,                    NULL,                   NULL, NULL,                   NULL                   },
    /* CPSS_PP_FAMILY_CHEETAH2_E    */  {      NULL,       cheetah1_3_GePortIfModeCfg,  cheetah1_3_GePortIfModeCfg, cheetah1_3_GePortIfModeCfg,      cheetah1_3_XgPortIfModeCfg,    NULL,   cheetah1_3_GePortIfModeCfg,     cheetah1_3_GePortIfModeCfg, cheetah1_3_GePortIfModeCfg, cheetah1_3_XgPortIfModeCfg, cheetah1_3_XgPortIfModeCfg, NULL,                   NULL,                   NULL,                   NULL,                   NULL,                    NULL,                   NULL,                    NULL,                   NULL, NULL,                   NULL                   },
    /* CPSS_PP_FAMILY_CHEETAH3_E    */  {      NULL,       NULL,                        cheetah1_3_GePortIfModeCfg, cheetah1_3_GePortIfModeCfg,      cheetah1_3_XgPortIfModeCfg,    NULL,   cheetah1_3_GePortIfModeCfg,     cheetah1_3_GePortIfModeCfg, NULL,                       NULL,                       NULL,                       NULL,                   NULL,                   NULL,                   NULL,                   NULL,                    NULL,                   NULL,                    NULL,                   NULL, NULL,                   NULL                   },
    /* CPSS_PP_FAMILY_DXCH_XCAT_E   */  {      NULL,       NULL,                        xCatFePortIfModeCfg,        xCatGePortIfModeCfg,             xCatXgPortIfModeCfg,           NULL,   xCatGePortIfModeCfg,            NULL,                       NULL,                       xCatXgPortIfModeCfg,        xCatXgPortIfModeCfg,        xCatXgPortIfModeCfg,    xCatGePortIfModeCfg,    xCatGePortIfModeCfg,    NULL,                   NULL,                    NULL,                   NULL,                    NULL,                   NULL, NULL,                   NULL                   },
    /* CPSS_PP_FAMILY_DXCH_XCAT3_E  */  {      NULL,       NULL,                        NULL,                       NULL,                            NULL,                          NULL,   NULL,                           NULL,                       NULL,                       NULL,                       NULL,                       NULL,                   NULL,                   NULL,                   NULL,                   NULL,                    NULL,                   NULL,                    NULL,                   NULL, NULL,                   NULL                   },
    /* CPSS_PP_FAMILY_DXCH_LION_E   */  {      NULL,       NULL,                        NULL,                       lionFlexLinkIfModeCfg,           lionFlexLinkIfModeCfg,         NULL,   lionFlexLinkIfModeCfg,          NULL,                       NULL,                       NULL,                       lionFlexLinkIfModeCfg,      lionFlexLinkIfModeCfg,  NULL,                   NULL,                   lionFlexLinkIfModeCfg,  lionFlexLinkIfModeCfg,   NULL,                   NULL,                    NULL,                   NULL, NULL,                   NULL                   },
    /* CPSS_PP_FAMILY_DXCH_XCAT2_E  */  {      NULL,       NULL,                        xCatFePortIfModeCfg,        xCatGePortIfModeCfg,             NULL,                          NULL,   xCatGePortIfModeCfg,            NULL,                       NULL,                       NULL,                       NULL,                       NULL,                   xCatGePortIfModeCfg,    xCatGePortIfModeCfg,    NULL,                   NULL,                    NULL,                   NULL,                    NULL,                   NULL, NULL,                   NULL                   },
    /* CPSS_PP_FAMILY_DXCH_LION2_E  */  {      NULL,       NULL,                        NULL,                       lion2FlexLinkIfModeCfg,          lion2FlexLinkIfModeCfg,        NULL,   lion2FlexLinkIfModeCfg,         NULL,                       NULL,                       NULL,                       lion2FlexLinkIfModeCfg,     lion2FlexLinkIfModeCfg, lion2FlexLinkIfModeCfg, NULL,                   NULL,                   lion2FlexLinkIfModeCfg,  lion2FlexLinkIfModeCfg, lion2FlexLinkIfModeCfg,  NULL,                   NULL, lion2FlexLinkIfModeCfg, lion2FlexLinkIfModeCfg },
    /* CPSS_PP_FAMILY_DXCH_LION3_E  */  {      NULL,       NULL,                        NULL,                       lion2FlexLinkIfModeCfg,          lion2FlexLinkIfModeCfg,        NULL,   lion2FlexLinkIfModeCfg,         NULL,                       NULL,                       NULL,                       lion2FlexLinkIfModeCfg,     lion2FlexLinkIfModeCfg, NULL,                   NULL,                   NULL,                   lion2FlexLinkIfModeCfg,  lion2FlexLinkIfModeCfg, lion2FlexLinkIfModeCfg,  lion2FlexLinkIfModeCfg, NULL, lion2FlexLinkIfModeCfg, lion2FlexLinkIfModeCfg },
    /* CPSS_PP_FAMILY_DXCH_BOBCAT2_E  */{      NULL,       NULL,                        NULL,                       NULL,                            NULL,                          NULL,   NULL,                           NULL,                       NULL,                       NULL,                       NULL,                       NULL,                   NULL,                   NULL,                   NULL,                   NULL,                    NULL,                   NULL,                    NULL,                   NULL,                         NULL                   }
    };

    /* array of pointers to port interface mode get functions per DXCH ASIC family */
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_GET_FUN portIfModeGetFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1]=
    {
    /* CPSS_PP_FAMILY_CHEETAH_E     */  cheetah1_3_PortIfModeGet,
    /* CPSS_PP_FAMILY_CHEETAH2_E    */  cheetah1_3_PortIfModeGet,
    /* CPSS_PP_FAMILY_CHEETAH3_E    */  cheetah1_3_PortIfModeGet,
    /* CPSS_PP_FAMILY_DXCH_XCAT_E   */  xcatPortIfModeGet,
    /* CPSS_PP_FAMILY_DXCH_XCAT3_E  */  lionIfModeGet,/*xcatPortIfModeGet,*/
    /* CPSS_PP_FAMILY_DXCH_LION_E   */  lionIfModeGet,
    /* CPSS_PP_FAMILY_DXCH_XCAT2_E  */  xcatPortIfModeGet,
    /* CPSS_PP_FAMILY_DXCH_LION2_E  */  lionIfModeGet,
    /* CPSS_PP_FAMILY_DXCH_LION3_E  */  lionIfModeGet,
    /* CPSS_PP_FAMILY_DXCH_BOBCAT2_E*/  lionIfModeGet
    };

#endif

typedef struct prvCpssIfModeSetFun_STC
{
    CPSS_PORT_INTERFACE_MODE_ENT                ifMode;
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN   fun;
}prvCpssIfModeSetFun_STC;

typedef struct prvCpssDev_IfMode_Fun_STC
{
    CPSS_PP_FAMILY_TYPE_ENT                   familyType;
    prvCpssIfModeSetFun_STC                  *ifModeSetFuncListPtr;
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_GET_FUN ifModeGetFunc;
}prvCpssDev_IfMode_Fun_STC;



prvCpssIfModeSetFun_STC prvCpssIfModeFun_Cheetah_List[] =
{
     { CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E, cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_MII_E,          cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_SGMII_E,        cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XGMII_E,        cheetah1_3_XgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,   cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_GMII_E,         cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_MII_PHY_E,      cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                       }
};

prvCpssIfModeSetFun_STC prvCpssIfModeFun_Cheetah2_List[] =
{
     { CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E, cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_MII_E,          cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_SGMII_E,        cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XGMII_E,        cheetah1_3_XgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,   cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_GMII_E,         cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_MII_PHY_E,      cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_QX_E,           cheetah1_3_XgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_HX_E,           cheetah1_3_XgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                   }
};

prvCpssIfModeSetFun_STC prvCpssIfModeFun_Cheetah3_List[] =
{
     { CPSS_PORT_INTERFACE_MODE_MII_E,          cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_SGMII_E,        cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XGMII_E,        cheetah1_3_XgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,   cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_GMII_E,         cheetah1_3_GePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                       }
};

prvCpssIfModeSetFun_STC prvCpssIfModeFun_Xcat_List[] =
{
     { CPSS_PORT_INTERFACE_MODE_MII_E,          xCatFePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_SGMII_E,        xCatGePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XGMII_E,        xCatXgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,   xCatGePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_QX_E,           xCatXgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_HX_E,           xCatXgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_RXAUI_E,        xCatXgPortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_100BASE_FX_E,   xCatGePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_QSGMII_E,       xCatGePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                }
};

prvCpssIfModeSetFun_STC prvCpssIfModeFun_Xcat2_List[] =
{
     { CPSS_PORT_INTERFACE_MODE_MII_E,          xCatFePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_SGMII_E,        xCatGePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,   xCatGePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_100BASE_FX_E,   xCatGePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_QSGMII_E,       xCatGePortIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                }
};

prvCpssIfModeSetFun_STC prvCpssIfModeFun_Lion_List[] =
{
     { CPSS_PORT_INTERFACE_MODE_SGMII_E,        lionFlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XGMII_E,        lionFlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,   lionFlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_HX_E,           lionFlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_RXAUI_E,        lionFlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XLG_E,          lionFlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E,  lionFlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                }
};


prvCpssIfModeSetFun_STC prvCpssIfModeFun_Lion2_List[] =
{
     { CPSS_PORT_INTERFACE_MODE_SGMII_E,        lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XGMII_E,        lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,   lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_HX_E,           lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_RXAUI_E,        lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_100BASE_FX_E,   lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E,  lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_KR_E,           lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_HGL_E,          lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_SR_LR_E,        lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XHGS_E,         lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_XHGS_SR_E,      lion2FlexLinkIfModeCfg }
    ,{ CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                   }
};


prvCpssIfModeSetFun_STC prvCpssIfModeFun_Bobcat2_List[] =
{
    { CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                   }
};


prvCpssIfModeSetFun_STC prvCpssIfModeFun_xcat3_List[] =
{
    { CPSS_PORT_INTERFACE_MODE_NA_E,           NULL                   }
};


prvCpssDev_IfMode_Fun_STC prvCpssIfModeSetFunList[] =
{
     {  CPSS_PP_FAMILY_CHEETAH_E,       &prvCpssIfModeFun_Cheetah_List[0] , cheetah1_3_PortIfModeGet  }
    ,{  CPSS_PP_FAMILY_CHEETAH2_E,      &prvCpssIfModeFun_Cheetah2_List[0], cheetah1_3_PortIfModeGet  }
    ,{  CPSS_PP_FAMILY_CHEETAH3_E,      &prvCpssIfModeFun_Cheetah3_List[0], cheetah1_3_PortIfModeGet  }
    ,{  CPSS_PP_FAMILY_DXCH_XCAT_E,     &prvCpssIfModeFun_Xcat_List[0]    , xcatPortIfModeGet         }
    ,{  CPSS_PP_FAMILY_DXCH_XCAT2_E,    &prvCpssIfModeFun_Xcat2_List[0]   , xcatPortIfModeGet         }
    ,{  CPSS_PP_FAMILY_DXCH_LION_E,     &prvCpssIfModeFun_Lion_List[0]    , lionIfModeGet             }
    ,{  CPSS_PP_FAMILY_DXCH_LION2_E,    &prvCpssIfModeFun_Lion2_List[0]   , lionIfModeGet             }
    ,{  CPSS_PP_FAMILY_DXCH_BOBCAT2_E,  &prvCpssIfModeFun_Bobcat2_List[0] , lionIfModeGet             }
    ,{  CPSS_PP_FAMILY_DXCH_XCAT3_E,    &prvCpssIfModeFun_xcat3_List[0]   , lionIfModeGet             }
    ,{  CPSS_PP_FAMILY_DXCH_BOBCAT3_E,  &prvCpssIfModeFun_Bobcat2_List[0] , lionIfModeGet             }
    ,{  CPSS_PP_FAMILY_DXCH_ALDRIN_E,   &prvCpssIfModeFun_Bobcat2_List[0] , lionIfModeGet             }
    ,{  CPSS_MAX_FAMILY,                NULL                              , NULL                      }
};


prvCpssDev_IfMode_Fun_STC * prvCpssDxChFillPortIfFunctionsFind
(
    CPSS_PP_FAMILY_TYPE_ENT devFamily
)
{
    GT_U32 devIdx;
    for (devIdx = 0; prvCpssIfModeSetFunList[devIdx].familyType != CPSS_MAX_FAMILY; devIdx++)
    {
        if (prvCpssIfModeSetFunList[devIdx].familyType == devFamily)
        {
            return &prvCpssIfModeSetFunList[devIdx];
        }
    }
    return (prvCpssDev_IfMode_Fun_STC*)NULL;
}


PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN prvCpssDxChFillPortIfFunctionsSetFind
(
    IN CPSS_PP_FAMILY_TYPE_ENT     devFamily,
    IN CPSS_PORT_INTERFACE_MODE_ENT ifMode
)
{
    prvCpssDev_IfMode_Fun_STC * ifFunsPtr;
    prvCpssIfModeSetFun_STC  *setFunlistPtr;
    GT_U32 ifIdx;

    ifFunsPtr = prvCpssDxChFillPortIfFunctionsFind(devFamily);
    if (ifFunsPtr == NULL)
    {
        return (PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN)NULL;
    }
    setFunlistPtr = ifFunsPtr->ifModeSetFuncListPtr;
    for (ifIdx = 0; setFunlistPtr->ifMode != CPSS_PORT_INTERFACE_MODE_NA_E; ifIdx++,setFunlistPtr++)
    {
        if (setFunlistPtr->ifMode == ifMode)
        {
            return setFunlistPtr->fun;
        }
    }

    return (PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN)NULL;
}

GT_STATUS prvCpssDxChPortIfFunctionsObjInit
(
    INOUT PRV_CPSS_DXCH_PORT_OBJ_STC *objPtr,
    IN    CPSS_PP_FAMILY_TYPE_ENT     devFamily
)
{
    GT_U32 ifIdx;
    prvCpssIfModeSetFun_STC   *setFunlistPtr;
    prvCpssDev_IfMode_Fun_STC *ifFunsPtr;

    /*-------------------------------------------------*/
    /* search for setPortInterfaceMode() function list */
    /*-------------------------------------------------*/

    ifFunsPtr = prvCpssDxChFillPortIfFunctionsFind(devFamily);
    if (ifFunsPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    /*-------------------------------------------------*/
    /*  fill setPortInterfaceMode() functions          */
    /*-------------------------------------------------*/
    for (ifIdx = 0; ifIdx < sizeof(objPtr->setPortInterfaceMode)/sizeof(objPtr->setPortInterfaceMode[0]);  ifIdx++)
    {
        objPtr->setPortInterfaceMode[ifIdx] = (PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN)NULL;
    }
    setFunlistPtr = ifFunsPtr->ifModeSetFuncListPtr;
    for (ifIdx = 0; setFunlistPtr->ifMode != CPSS_PORT_INTERFACE_MODE_NA_E; ifIdx++,setFunlistPtr++)
    {
        objPtr->setPortInterfaceMode[setFunlistPtr->ifMode] = setFunlistPtr->fun;
    }


    objPtr->getPortInterfaceMode = ifFunsPtr->ifModeGetFunc;

    return GT_OK;
}




PRV_CPSS_LION2_INT_CAUSE_ENT lion2PortUnitHglSumIndex
(
    IN GT_PHYSICAL_PORT_NUM localPort
)
{
    switch(localPort)
    {
        case 0:
            return PRV_CPSS_LION2_PORT0_UNITS_SUM_HGL_SUMMARY_E;
        case 1:
            return PRV_CPSS_LION2_PORT1_UNITS_SUM_HGL_SUMMARY_E;
        case 2:
            return PRV_CPSS_LION2_PORT2_UNITS_SUM_HGL_SUMMARY_E;
        case 3:
            return PRV_CPSS_LION2_PORT3_UNITS_SUM_HGL_SUMMARY_E;
        case 4:
            return PRV_CPSS_LION2_PORT4_UNITS_SUM_HGL_SUMMARY_E;
        case 5:
            return PRV_CPSS_LION2_PORT5_UNITS_SUM_HGL_SUMMARY_E;
        case 6:
            return PRV_CPSS_LION2_PORT6_UNITS_SUM_HGL_SUMMARY_E;
        case 7:
            return PRV_CPSS_LION2_PORT7_UNITS_SUM_HGL_SUMMARY_E;
        case 8:
            return PRV_CPSS_LION2_PORT8_UNITS_SUM_HGL_SUMMARY_E;
        case 9:
            return PRV_CPSS_LION2_PORT9_UNITS_SUM_HGL_SUMMARY_E;
        case 10:
            return PRV_CPSS_LION2_PORT10_UNITS_SUM_HGL_SUMMARY_E;
        case 11:
            return PRV_CPSS_LION2_PORT11_UNITS_SUM_HGL_SUMMARY_E;
        case 12:
            return PRV_CPSS_LION2_PORT12_UNITS_SUM_HGL_SUMMARY_E;
        case 14:
            return PRV_CPSS_LION2_PORT14_UNITS_SUM_HGL_SUMMARY_E;
        default:
            return PRV_CPSS_LION2_LAST_INT_E;
    }
}

/*******************************************************************************
* prvCpssDxChPortSerdesLoopbackActivate
*
* DESCRIPTION:
*       Set internal loopback enable and
*       Set power down/up of Tx and Rx on Serdeses.
*       Not Valid for port speed 10G and 20G.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat; xCat2.
*
* INPUTS:
*       devNum      - system device number
*       portNum     - physical port number
*       speed       - port data speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failed
*       GT_NOT_SUPPORTED         - not supported
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSerdesLoopbackActivate
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_SPEED_ENT      speed
)
{
    GT_STATUS                    rc;

    rc = prvCpssDxChPortLion2InternalLoopbackEnableSet(devNum, portNum, GT_TRUE);
    if (rc != GT_OK)
    {
         return rc;
    }
    if(CPSS_PORT_SPEED_1000_E == speed)
    {
        rc = prvCpssDxChPortSerdesPartialPowerDownSet(devNum, portNum, GT_TRUE, GT_TRUE);
        if (rc != GT_OK)
        {
             return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortSerdesPartialPowerDownSet
*
* DESCRIPTION:
*       Set power down/up of Tx and Rx on Serdeses.
*       Not Valid for port speed 10G and 20G
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat; xCat2.
*
* INPUTS:
*       devNum      - system device number
*       portNum     - physical port number
*       powerDownRx - Status of Serdes Rx (TRUE - power down, FALSE - power up).
*       powerDownTx - Status of Serdes Tx (TRUE - power down, FALSE - power up).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on failed.
*       GT_NOT_SUPPORTED            - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSerdesPartialPowerDownSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  powerDownRx,
    IN GT_BOOL                  powerDownTx
)
{
    GT_STATUS                    rc;
    GT_UOPT                      portGroup;
    MV_HWS_PORT_STANDARD         portMode;/* interface modes of port which use this configuration in Black Box format */
    CPSS_PORT_SPEED_ENT          speed; /* current speed of port */
    GT_UOPT                      phyPortNum; /* number of port in local core */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode; /* port interface mode */
    GT_U32                       firstInQuadruplet;  /* mac number of first port in quadruplet */
    GT_U32                       maxGEonly = 24;     /* maximal GE only ports for device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT_E | CPSS_XCAT2_E);
    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    speed = PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portNum);
    phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
    ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum);
    rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
    if (rc != GT_OK)
    {
        return rc;
    }
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        maxGEonly = 48;
    }
    if ((PRV_CPSS_GE_PORT_GE_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, phyPortNum))
        && (phyPortNum < maxGEonly))
    {/* provide to HWS first port in quadruplet and it will configure
                all other ports if needed */
         firstInQuadruplet = phyPortNum & 0xFFFFFFFC;
    }
    else
    {
        firstInQuadruplet = phyPortNum;
    }
    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortPartialPowerDown(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], powerDownRx[%d], powerDownTx[%d])", devNum, portGroup, firstInQuadruplet, portMode, powerDownRx, powerDownTx);
    rc = mvHwsPortPartialPowerDown(devNum, portGroup, firstInQuadruplet, portMode, powerDownRx, powerDownTx);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortTypeSet
*
* DESCRIPTION:
*       Sets port type (mostly means which mac unit used) on a specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        Lion2.
*
*
* INPUTS:
*       devNum  - physical device number
*       macNum  - port MAC number
*       ifMode  - Interface mode
*       speed   - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvCpssDxChPortTypeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          macNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    if((CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        cpssOsPrintf("prvCpssDxChPortTypeSet: Not applicable device");
        return;
    }

    switch (ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_1000_E:
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[macNum].portType = PRV_CPSS_PORT_GE_E;
                    break;
                case CPSS_PORT_SPEED_10000_E:
                case CPSS_PORT_SPEED_20000_E:
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[macNum].portType = PRV_CPSS_PORT_XG_E;
                    break;
                default:
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[macNum].portType = PRV_CPSS_PORT_XG_E;
                    break;
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
        case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
        case CPSS_PORT_INTERFACE_MODE_100BASE_FX_E:
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[macNum].portType = PRV_CPSS_PORT_GE_E;
            break;
        case CPSS_PORT_INTERFACE_MODE_QX_E:
        case CPSS_PORT_INTERFACE_MODE_HX_E:
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[macNum].portType = PRV_CPSS_PORT_XG_E;
            break;
        case CPSS_PORT_INTERFACE_MODE_XLG_E:
        case CPSS_PORT_INTERFACE_MODE_KR_E:
        case CPSS_PORT_INTERFACE_MODE_SR_LR_E:
        case CPSS_PORT_INTERFACE_MODE_HGL_E: /* pay attention this definition for
        HGL relevant for Bobcat2 XLHGL_KR4 mode, in other cases could be required
        PRV_CPSS_PORT_HGL_E */
        case CPSS_PORT_INTERFACE_MODE_XHGS_E: /* If with Serdes   of 12.1875Ghz */
        case CPSS_PORT_INTERFACE_MODE_XHGS_SR_E:
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[macNum].portType = PRV_CPSS_PORT_XLG_E;
            break;
        case CPSS_PORT_INTERFACE_MODE_ILKN4_E:
        case CPSS_PORT_INTERFACE_MODE_ILKN8_E:
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[macNum].portType = PRV_CPSS_PORT_ILKN_E;
            break;
        case CPSS_PORT_INTERFACE_MODE_NA_E:
            /* leave CPSS init default value, see hwPpPhase1Part1 */
            break;
        default:
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[macNum].portType = PRV_CPSS_PORT_FE_E;
            break;
    }

    return;
}

/*******************************************************************************
* internal_cpssDxChPortInterfaceModeSet
*
* DESCRIPTION:
*       Sets Interface mode on a specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        Depends on capability of the specific device. The port's speed
*        should be set after configuring Interface Mode.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInterfaceModeSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_STATUS rc;   /* return code */
    GT_U32 portMacNum;      /* MAC number */


    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN ifModeSetFuncPtr; /* pointer to [devFamily][ifMode] specific function */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_XCAT3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_NOT_EXISTS_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if((ifMode >= CPSS_PORT_INTERFACE_MODE_NA_E) || (ifMode < CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    ifModeSetFuncPtr = PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->setPortInterfaceMode[ifMode];
    if(ifModeSetFuncPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(portNum != CPSS_CPU_PORT_NUM_CNS)
    {
        GT_BOOL resNew;
        /* check if interface mode supported by given port */

        /*
        //resOld = supportedPortsModes[PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum)][ifMode];
        */

        rc = prvCpssDxChPortSupportedModeCheck(devNum,portNum,ifMode,/*OUT*/&resNew);
        if (rc != GT_OK)
        {
            return GT_OK;
        }

        if (resNew == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    if((systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E)
        && (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.hitlessWriteMethodEnable))
    {/* do only if we during recovery */
        CPSS_PORT_INTERFACE_MODE_ENT   currentIfMode;

        rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &currentIfMode);
        if(rc != GT_OK)
        {
            return rc;
        }
        if(ifMode == currentIfMode)
        {
            return GT_OK;
        }
    }

    /* configure requested interface mode on port */
    if((rc = ifModeSetFuncPtr(devNum, portNum, ifMode)) != GT_OK)
        return rc;

    /* cpu port has not entry in phyPortInfoArray - skip SW port info DB update */
    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        return GT_OK;

    /* update current port type - used to define which mac unit currently in use by port
    */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        prvCpssDxChPortTypeSet(devNum,portNum,ifMode,CPSS_PORT_SPEED_NA_E);
    }


    /* save new interface mode in DB */
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum) = ifMode;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInterfaceModeSet
*
* DESCRIPTION:
*       Sets Interface mode on a specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        Depends on capability of the specific device. The port's speed
*        should be set after configuring Interface Mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterfaceModeSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInterfaceModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ifMode));

    rc = internal_cpssDxChPortInterfaceModeSet(devNum, portNum, ifMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ifMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cheetah1_3_GePortIfModeCfg
*
* DESCRIPTION:
*       Sets GE Interface mode on a specified port of devices from DXCH
*           family CH3 and older.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS cheetah1_3_GePortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32     regAddr;             /* register address */
    GT_U32     macCnrl2RegAddr;     /* register address */
    GT_U32     value;               /* value to write into the register */
    GT_U32     useIntClkForEn = 0;  /* Must be set for CPU port according to ifMode */
    GT_U32     fieldLength;         /* The number of bits to be written to register.*/
    GT_BOOL    linkDownStatus;      /* Port link down status */
    GT_STATUS  rc;                  /* return code     */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        switch(ifMode)
        {
            case CPSS_PORT_INTERFACE_MODE_MII_E:
                value = 0;
                useIntClkForEn = 0;
                break;
            case CPSS_PORT_INTERFACE_MODE_GMII_E:
                value = 1;
                useIntClkForEn = 0;
                break;
            case CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E:
                if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                value = 2;
                useIntClkForEn = 1;
                break;
            case CPSS_PORT_INTERFACE_MODE_MII_PHY_E:
                if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                value = 3;
                useIntClkForEn = 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
        fieldLength = 2;
    }
    else
    {
        /* unlike new flex links cheetah's XG_HX_QX ports don't support GE modes */
        if(PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum) == PRV_CPSS_XG_PORT_XG_HX_QX_E)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        switch(ifMode)
        {
            case CPSS_PORT_INTERFACE_MODE_SGMII_E:
                value = 0;
                break;
            case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
                value = 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,
                    PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
        fieldLength = 1;
    }

    rc = prvCpssDxChPortForceLinkDownEnable(devNum,portNum,&linkDownStatus);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,
            PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&macCnrl2RegAddr);
        /* Must be set for CPU port according to ifMode */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                macCnrl2RegAddr, 4, 1,
                                                useIntClkForEn);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Set interface mode */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, 1, fieldLength, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) ||
        (ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E))
    {
        rc = prvCpssDxChPortInbandAutonegMode(devNum,portNum,ifMode);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChPortForceLinkDownDisable(devNum,portNum,linkDownStatus);

    return rc;
}


/*******************************************************************************
* cheetah1_3_GePortIfModeCfg
*
* DESCRIPTION:
*       Sets XGMII Interface mode on a specified port of devices from DXCH
*           family CH3 and older.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS cheetah1_3_XgPortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 regValue;    /* register value */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if((ifMode == CPSS_PORT_INTERFACE_MODE_QX_E) || (ifMode == CPSS_PORT_INTERFACE_MODE_HX_E))
    {
        if(PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum) == PRV_CPSS_XG_PORT_XG_ONLY_E)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        if((25 != portNum) && (26 != portNum))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        /* change interface to HX */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hxPortGlobalConfig;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 15, 1, 0)
            != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
            hxPortConfig0[portNum-25];
        regValue = (ifMode == CPSS_PORT_INTERFACE_MODE_QX_E) ? 0 : 1;
        /* Reset the HX port PCS */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 11, 1, 1)
            != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        /* Set QX or HX port mode */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 5, 1,
                                                regValue) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        /* Set the HX port PCS to 0 */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 11, 1, 0)
            != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else if(ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E)
    {
        if (PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum) == PRV_CPSS_XG_PORT_HX_QX_ONLY_E)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        if (PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum) == PRV_CPSS_XG_PORT_XG_ONLY_E)
            return GT_OK; /* it's already configured - there is just no other option */

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hxPortGlobalConfig;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 15, 1, 1)
            != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXcat100BaseFxConfig
*
* DESCRIPTION:
*       This function configures 100_Base_FX mode on given GE port
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
*       enable   - GT_TRUE  - enable 100BaseFX
*                  GT_FALSE - disable 100BaseFX
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXcat100BaseFxConfig
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      portState;  /* current port state */
    GT_STATUS   rc;         /* return code */

    rc = prvCpssDxChPortStateDisableSet(devNum,portNum,&portState);
    if(rc != GT_OK)
        return rc;

    if(GT_TRUE == enable)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                    perPortRegs[portNum].Config100Fx;
        /* 100FX PCS Sigdet polarity */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr,
                                               13, 1, 1) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        /* desired 100-FX PHY address in MDC/MDIO transaction */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr,
                                               8, 5, portNum+4) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        /* 100FX PCS use sigdet */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr,
                                               5, 1, 1) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                    perPortRegs[portNum].autoNegCtrl;
        /* AnDuplexEn => 0x0, SetFullDuplex => 0x1, AnFcEn => 0x0 */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr,
                                               11, 3, 2) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        /* PauseAdv => 0x0, SetFcEn => 0x0, AnSpeedEn => 0x0, SetGMIISpeed => 0x0,
            SetMIISpeed => 0x1 */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr,
                                               5, 5, 1) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        /* WA for "Rx errors on 100BASE-FX traffic using fiber transceivers
            directly connected to FlexLink uplink ports." */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT2_100FX_WA_E))
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portNum,PRV_CPSS_PORT_GE_E,&regAddr);
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 4, 1, 1);
            if(rc != GT_OK)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 8+(portNum-24), 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* PcsEn field in Port MAC Control Register2*/
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,
                    PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr,
                                               3, 1, 0) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        /* 100FX enable field in 100FX Configuration Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                    perPortRegs[portNum].Config100Fx;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr,
                                               7, 1, 1) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else
    {/* disable 100FX */

        /* WA for "Rx errors on 100BASE-FX traffic using fiber transceivers
            directly connected to FlexLink uplink ports." */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT2_100FX_WA_E))
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portNum,PRV_CPSS_PORT_GE_E,&regAddr);
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 4, 1, 0);
            if(rc != GT_OK)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, 8+(portNum-24), 1, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                    perPortRegs[portNum].Config100Fx;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 7, 1, 0) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        /* PcsEn field in Port MAC Control Register2*/
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,
                PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 3, 1, 1) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }

    rc = prvCpssDxChPortStateEnableSet(devNum,portNum,portState);
    if(rc != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortGePortTypeSet
*
* DESCRIPTION:
*       Set port type and inband auto-neg. mode of GE MAC of port
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number (CPU port not supported)
*       ifMode      - port interface mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, ifMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGePortTypeSet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN CPSS_PORT_INTERFACE_MODE_ENT ifMode
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      value;  /* value to set in register */
    GT_U32      regAddr; /* register address */
    GT_BOOL     linkDownStatus; /* current port force link down state */
    GT_U32      portGroupId;    /* port group ID - for multi-port-group devices */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                        | CPSS_CH2_E | CPSS_CH3_E
                                        | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* Set interface mode */
    switch(ifMode)
    {
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[unterminated_case] */
        case CPSS_PORT_INTERFACE_MODE_QSGMII_E: /*No break*/
            if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            CPSS_COVERITY_NON_ISSUE_BOOKMARK
            /* coverity[fallthrough] */
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            value = 0;
            break;
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortForceLinkDownEnable(devNum,portNum,&linkDownStatus);
    if(rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portNum, PRV_CPSS_PORT_GE_E,
                                        &regAddr);
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(
                                                                devNum, portNum);
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 1, 1,
                                            value);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortForceLinkDownDisable(devNum,portNum,linkDownStatus);

    return rc;
}

/*******************************************************************************
* xCatGePortIfModeCfg
*
* DESCRIPTION:
*       Sets GE Interface mode on a specified port of devices from DXCH
*           XCAT and XCAT2 families.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS xCatGePortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32 regAddr;     /* register address */
    GT_STATUS rc;       /* return code */
    GT_BOOL linkDownStatus; /* current force link down status */
    GT_U32      portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E |  CPSS_XCAT3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if((portNum == CPSS_CPU_PORT_NUM_CNS) ||
        (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChPortForceLinkDownEnable(devNum,portNum,&linkDownStatus);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((rc = prvCpssDxChPortMacResetStateSet(devNum, portNum, GT_TRUE)) != GT_OK)
        return rc;

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        /* configure SGMII mode (not HGS) */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, portNum + 2, 1, 0) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_PP_MAC(devNum)->revision > 2)
        {/* define XAUI mediaInterface for xCat revision 3 and above */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, portNum, 1, 0) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }

        /* Set GigMAC in MacModeSelect - not relevant for XCAT2 */
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            /* MAC Mode is selected in XG MAC.
                This field is not applicable for 40G ports and for 1G
                it has some other purpose */
            PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum, portNum, PRV_CPSS_PORT_XG_E, &regAddr);
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 13, 3, 0) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }

            /* update current port type - used to define which mac unit currently in use by port */
            prvCpssDxChPortTypeSet(devNum,portNum,ifMode,CPSS_PORT_SPEED_NA_E);

            /* update addresses of mac registers accordingly to used MAC 1G/XG/XLG */
            rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, portNum, ifMode);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        rc = prvCpssDxChXcat100BaseFxConfig(devNum, portNum,
                            (ifMode == CPSS_PORT_INTERFACE_MODE_100BASE_FX_E) ?
                                GT_TRUE : GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }

    }/* end of flex link */
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;
        if((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) || (ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E))
        {
            /* Enable SGMII mode in Network Port */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr,15+portNum/4,1,1);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* for SGMII 1G doesn't matter if network_ports_dp_sync_fifo_bypass enabled or disabled */
        }
        else if(ifMode == CPSS_PORT_INTERFACE_MODE_QSGMII_E)
        {
            /* clear network_ports_sgmii_en for port */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr,15+portNum/4,1,0);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* state of network_ports_dp_sync_fifo_bypass doesn't matter for 1G mode */
        }

        HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,10);
    }

    /* reset again may be port switched to new mac unit */
    if((rc = prvCpssDxChPortMacResetStateSet(devNum, portNum, GT_TRUE)) != GT_OK)
        return rc;

    if(ifMode != CPSS_PORT_INTERFACE_MODE_100BASE_FX_E)
    {
        rc = prvCpssDxChPortGePortTypeSet(devNum,portNum,ifMode);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChPortForceLinkDownDisable(devNum,portNum,linkDownStatus);

    return rc;
}

/*******************************************************************************
* xCatXgPortIfModeCfg
*
* DESCRIPTION:
*       Sets Interface mode above GE on a specified port of devices from DXCH
*           XCAT and XCAT2 families.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS xCatXgPortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* register value */
    GT_U32 resetState;  /* current PCS reset state */
    GT_STATUS rc;       /* return code */
    GT_BOOL linkDownStatus; /* current force link down status */
    GT_PHYSICAL_PORT_NUM localPort;  /* port number in port group */
    GT_U32      portGroupId; /*the port group Id - support multi-port-groups device */

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if((portNum == CPSS_CPU_PORT_NUM_CNS) ||
        (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChPortForceLinkDownEnable(devNum,portNum,&linkDownStatus);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((rc = prvCpssDxChPortMacResetStateSet(devNum, portNum, GT_TRUE)) != GT_OK)
        return rc;

    /* configure HGS mode (not SGMII) */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr,
                                            portNum + 2, 1, 1) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* update current port type - used to define which mac unit currently in use by port */
    prvCpssDxChPortTypeSet(devNum,portNum,ifMode,CPSS_PORT_SPEED_NA_E);

    /* reset xpcs here to be sure port already in XG mode */
    if((rc = prvCpssDxChPortXgPcsResetStateSet(devNum, portNum, GT_TRUE)) != GT_OK)
        return rc;

    /* update addresses of mac registers accordingly to used MAC 1G/XG/XLG */
    rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, portNum, ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* reset again may be port switched to new mac unit */
    if((rc = prvCpssDxChPortMacResetStateSet(devNum, portNum, GT_TRUE)) != GT_OK)
        return rc;

    switch (ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_QX_E:
            value = 0;
            break;
        case CPSS_PORT_INTERFACE_MODE_HX_E:
            value = 1;
            break;
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            if(PRV_CPSS_PP_MAC(devNum)->revision <= 2)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            /* no break */
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
            /* RXAUI and XAUI both use 4 lanes, but RXAUI uses 2 SerDes while XAUI uses 4 */
            value = 2;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_PP_MAC(devNum)->revision > 2)
    {/* define mediaInterface for xCat revision 3 and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, localPort, 1,
                                                (ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E) ? 1 : 0)
                                                != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }

    /* Get LaneActive in Global Configuration0 register*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].xgGlobalConfReg0;

    /* LaneActive may be changed only when <PCSReset> is HIGH.*/
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, 0, 1, &resetState) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    if(resetState == 0)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 0, 1, 1) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,20);
    }

    /* set LaneActive */
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, 5, 2, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }
    HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,20);

    /* restore PSC Reset state */
    if(resetState == 0)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 0, 1, 0) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,20);
    }

    /* Set MAC type in MacModeSelect */
    /* MAC Mode is selected in XG MAC.
        This field is not applicable for 40G ports and for 1G
        it has some other purpose */
    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, 13, 3, 1) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,20);

    rc = prvCpssDxChPortForceLinkDownDisable(devNum,portNum,linkDownStatus);

    return rc;
}

/*******************************************************************************
* xCatFePortIfModeCfg
*
* DESCRIPTION:
*       Sets FE Interface mode on a specified port of devices from DXCH
*           XCAT and XCAT2 families.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        Cannot change type of FE port, so just check ifMode.
*
*        From architectural point of view would be better to use portOptions
*        enum to define FE port, but for now use of portType easier for
*        implementation
*
*******************************************************************************/
static GT_STATUS xCatFePortIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    /* Note: cpu port is GE */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) != PRV_CPSS_PORT_FE_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(ifMode != CPSS_PORT_INTERFACE_MODE_MII_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* lionFlexLinkPcsActiveLanesNumSet
*
* DESCRIPTION:
*       Sets active PCS lanes number for port of Lion
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS lionFlexLinkPcsActiveLanesNumSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32 value;       /* new reg field value */
    GT_U32 regAddr;     /* reg address */
    GT_U32 resetState;  /* current PCS reset state */
    GT_U32 portGroupId; /* port group ID */

    /* convert the 'Physical port' to portGroupId -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    switch (ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_HX_E:
            value = 1;
            break;
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
        case CPSS_PORT_INTERFACE_MODE_XLG_E:
        case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
            /* RXAUI and XAUI both use 4 lanes, but RXAUI uses 2 SerDes while XAUI uses 4 */
            value = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Get LaneActive in Global Configuration0 register*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        macRegs.perPortRegs[portNum].xgGlobalConfReg0;

    /* This field may be changed only when PCSReset_ is asserted */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &resetState) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    if(resetState == 0)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,20);
    }

    /* set LaneActive */
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 5, 2, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }
    HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,20);

    /* restore PSC Reset state */
    if(resetState == 0)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,20);
    }

    return GT_OK;
}

/*******************************************************************************
* lionFlexLinkIfModeCfgGe
*
* DESCRIPTION:
*       Sets GE Interface mode on a specified port of Lion
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS lionFlexLinkIfModeCfgGe
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* register address */
    GT_U32      portGroupId;    /* port group ID - for multi-port-group devices */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* PcsEn field in Port MAC Control Register2*/
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,
                                    PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                           3, 1, 1) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortGePortTypeSet(devNum,portNum,ifMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* lionFlexLinkIfModeCfg
*
* DESCRIPTION:
*       Sets Interface mode on a specified port of devices from DXCH
*           Lion family.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS lionFlexLinkIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      value;          /* value to write into the register */
    GT_STATUS   rc;             /* return code     */
    GT_U32      portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM localPort;      /* number of port in port group */
    CPSS_PORT_INTERFACE_MODE_ENT ifModePrv; /* old ports interface mode */
    GT_U32      startSerdes;/* number of first serdes used by port */
    GT_U32      numOfSerdesLanes; /* number of serdes lanes occupied by port */
    GT_PHYSICAL_PORT_NUM globalPort;     /* calculated port number in device */
    GT_U32      i;              /* loop counter */
    GT_BOOL     dontResetSerdes = GT_FALSE; /* don't reset serdeses they are already configured as required */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E |
                                                CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* can't define XLG mode if one of ports which serdeses will be occupied by XLG port is special */
    if(ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E)
    {
        if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum,portNum,ifMode,
                                                            &startSerdes,&numOfSerdesLanes)) != GT_OK)
            return rc;

        for(i = startSerdes/2; i*2 < startSerdes+numOfSerdesLanes; i++)
        {
            globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,i);
            if((globalPort != portNum)
                && (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,globalPort) == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
    }

    if(ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E)
    {
        if((localPort == 8) || (localPort == 9) || (localPort == 11))
        {/* can't define special mode on ports which serdeses occupied by XLG port */
            globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,10);
            if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,globalPort) == CPSS_PORT_INTERFACE_MODE_XLG_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }

        if(localPort%2 != 0)
        {/* if neighbor even port occupies more then 2 serdes then special mode could be defined only
          * if neighbor is XGMII */
            if((rc = prvCpssDxChPortIfModeSerdesNumGet(devNum,portNum-1,
                                                    PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1),
                                                    &startSerdes,&numOfSerdesLanes)) != GT_OK)
                return rc;
            if((numOfSerdesLanes > 2)
                && (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1) != CPSS_PORT_INTERFACE_MODE_XGMII_E))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifModePrv);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* may be scenario when serdeses of odd port used for XGMII by even port,
     * now even port changed mode to lets say RXAUI and we want to change mode
     * of odd port too, lets fake here old port's mode to some mode using just
     * own serdeses of current port.
     */
    if(ifModePrv == CPSS_PORT_INTERFACE_MODE_NA_E)
        ifModePrv = CPSS_PORT_INTERFACE_MODE_RXAUI_E;

    /* for Lion reset mac, xpcs, serdes before changing port mode and speed,
     * so unreset will be in serdesPowerStatusSet */
    if((rc = prvCpssDxChPortIfModeSerdesNumGet(devNum,portNum,ifModePrv, /* reset here accordingly to old ifMode */
                                            &startSerdes,&numOfSerdesLanes)) != GT_OK)
        return rc;

    for(i = startSerdes/2; i*2 < startSerdes+numOfSerdesLanes; i++)
    {
        globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,i);

        if((rc = prvCpssDxChPortXgPcsResetStateSet(devNum, globalPort, GT_TRUE)) != GT_OK)
            return rc;

        if((rc = prvCpssDxChPortMacResetStateSet(devNum, globalPort, GT_TRUE)) != GT_OK)
            return rc;

        if(globalPort != portNum)
        {
            /* invalidate current interface mode in DB */
            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, globalPort) = CPSS_PORT_INTERFACE_MODE_NA_E;
        }
    }

    if((ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E) && (localPort%2 != 0))
    {
        if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1) == CPSS_PORT_INTERFACE_MODE_XGMII_E)
            dontResetSerdes = GT_TRUE;
    }

    if(dontResetSerdes != GT_TRUE)
    {
        if((rc = prvCpssDxChPortSerdesResetStateSet(devNum, portNum, startSerdes, numOfSerdesLanes,
                                                    GT_TRUE)) != GT_OK)
            return rc;
    }

    /* update current port type - used to define which mac unit currently in use by port */
    prvCpssDxChPortTypeSet(devNum,portNum,ifMode,CPSS_PORT_SPEED_NA_E);

    /* update addresses of mac registers accordingly to used MAC 1G/XG/XLG */
    rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, portNum, ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum,portNum,ifMode,&startSerdes,
                                                       &numOfSerdesLanes)) != GT_OK)
        return rc;

    /* RXAUI -> other
        1.  Change MAC Mode to 1G (this makes sure that MAC uses 1 SERDES only and
                                                    0x38C will not kill adjacent port);
        2.  Change Media interface to "non-RXAUI" (clear the relevant bit in 0x38C);

       other -> RXAUI
        1.  Change Media interface to RXAUI(set relevant bits in 0x38C)
            This makes sure that adjacent port is NOT getting affected since we are
             still in 1G MAC mode and port uses 1 serdes.
        2.  Change MAC Mode to 10G. Is safe now since 0x38C is configured to RXAUI already.
    */
    if(ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, startSerdes,
                                                2, 0x3) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }

    /* MAC Mode is selected in XG MAC.
        This field is not applicable for 40G ports and for 1G
        it has some other purpose */
    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,PRV_CPSS_PORT_XG_E,&regAddr);
    switch (ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            value = 0;
            break;
        case CPSS_PORT_INTERFACE_MODE_HX_E:
        case CPSS_PORT_INTERFACE_MODE_XGMII_E: /* both 10G and 20G */
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
        case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
            value = 1;
            break;
        case CPSS_PORT_INTERFACE_MODE_XLG_E:
            value = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 13, 3, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
    if(localPort%2 != 0)
    {/* if we configure odd port while his less significant even neighbour
        port not configured we must change media of even port to RXAUI
        otherwise won't be link on odd port, although odd port requires XAUI
        media on his serdeses */
        if(CPSS_PORT_INTERFACE_MODE_NA_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1))
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                   startSerdes-2, 2, 0x3) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }
    }

    if(ifMode != CPSS_PORT_INTERFACE_MODE_RXAUI_E)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, startSerdes,
                                                numOfSerdesLanes, 0) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }

    HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,20);

    if(GT_TRUE == dontResetSerdes)
    {/* other configurations done in scope of even neighbor port */

        if((rc = prvCpssDxChPortXgPcsResetStateSet(devNum, portNum, GT_FALSE)) != GT_OK)
            return rc;

        if((rc = prvCpssDxChPortMacResetStateSet(devNum, portNum, GT_FALSE)) != GT_OK)
            return rc;

        return GT_OK;
    }

   /* Call serdes reset again, because in new mode could be used more SerDes's then in old one
        and type of MAC may change */

    for(i = startSerdes/2; i*2 < startSerdes+numOfSerdesLanes; i++)
    {
        globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,i);

        if((rc = prvCpssDxChPortXgPcsResetStateSet(devNum, globalPort, GT_TRUE)) != GT_OK)
            return rc;

        if((rc = prvCpssDxChPortMacResetStateSet(devNum, globalPort, GT_TRUE)) != GT_OK)
            return rc;

        if(globalPort != portNum)
        {
            /* invalidate current interface mode in DB */
            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, globalPort) = CPSS_PORT_INTERFACE_MODE_NA_E;
        }
    }

    /* run this reset after interface mode configuration, because it defines number of serdeses
        to reset accordingly to defined mode */
    if((rc = prvCpssDxChPortSerdesResetStateSet(devNum,portNum,startSerdes,numOfSerdesLanes,GT_TRUE)) != GT_OK)
        return rc;

    HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,10);

    switch (ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            return lionFlexLinkIfModeCfgGe(devNum, portNum, ifMode);

        case CPSS_PORT_INTERFACE_MODE_HX_E:
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
        case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
        case CPSS_PORT_INTERFACE_MODE_XLG_E:
            return lionFlexLinkPcsActiveLanesNumSet(devNum, portNum, ifMode);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* lion2FlexLinkIfModeCfg
*
* DESCRIPTION:
*       Sets Interface mode on a specified port of devices from DXCH
*           Lion2 family.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_SUPPORTED  - on not supported interface or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS lion2FlexLinkIfModeCfg
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_STATUS            rc;        /* return code */
    MV_HWS_PORT_STANDARD portMode;  /* port i/f mode and speed translated to BlackBox enum */
    GT_U32               macNum,     /* number of MAC used by port for given ifMode */
                         pcsNum,     /* number of PCS used by port for given ifMode */
                         sdVecSize;  /* size of serdes redundancy array */
    GT_U32               *sdVectorPtr; /* serdes redundancy array */
    GT_PHYSICAL_PORT_NUM localPort;   /* number of port in port group */
    GT_BOOL              notEnoughInfo = GT_FALSE; /* not enough info to define validity */
    GT_U32               portGroup; /* number of local core */
    GT_U32               sliceNumUsed; /* number of pizza slices used by neighbour
                                            port */
    CPSS_PORT_INTERFACE_MODE_ENT    ifModeOld;
    CPSS_PORT_SPEED_ENT speedOld;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
    if(localPort >= 12)
    {/* to prevent coverity warning OVERRUN_STATIC */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    switch(ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            /* no need to check SGMII2_5 it's same like SGMII */
            portMode = SGMII;
            break;
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
            /* other speeds similar to 10G */
            portMode = _10GBase_KX4;
            break;
        case CPSS_PORT_INTERFACE_MODE_KR_E:
            /* check here just basic KR support, validity for KR20/40/100
                will be tested in speedSet */
            portMode = _10GBase_KR;
            if((9 == localPort) || (11 == localPort))
            {/* local ports 9 and 11 can use regular and extended MAC
                till we not sure which one we can't check validity */
                notEnoughInfo = GT_TRUE;
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_HGL_E:
            if((9 == localPort) || (11 == localPort))
            {/* if core clock of device less then 480 it doesn't
                support more then 10G speed on ports 9 and 11 */
                if(PRV_CPSS_PP_MAC(devNum)->coreClock < 480)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            portMode = HGL; /* could be XLHGL, but here doesn't matter */
            break;
        case CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E:
        case CPSS_PORT_INTERFACE_MODE_SR_LR_E:
            /* check here just basic SR support, validity for SR20/40
                will be tested in speedSet */
            portMode = _10GBase_SR_LR;
            if((9 == localPort) || (11 == localPort))
            {/* local ports 9 and 11 can use regular and extended MAC
                till we not sure which one we can't check validity */
                notEnoughInfo = GT_TRUE;
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_XHGS_E:
        case CPSS_PORT_INTERFACE_MODE_XHGS_SR_E:
            if (PRV_CPSS_PP_MAC(devNum)->revision == 0) /* LION2 version 0 and clock less than 360 MHz are not supported */
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            if(PRV_CPSS_PP_MAC(devNum)->coreClock < 480)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            portMode = (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode) ?
                                                       _12GBaseR : _12GBase_SR;
            if((9 == localPort) || (11 == localPort))
            {/* local ports 9 and 11 can use regular and extended MAC
                till we not sure which one we can't check validity */
                notEnoughInfo = GT_TRUE;
            }

            break;
        default:
            rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode,
                                /* in these cases just one option for speed */
                                CPSS_PORT_SPEED_NA_E,
                                &portMode);
            if(rc != GT_OK)
            {
                return rc;
            }
            break;
    }

    portGroup = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rc = hwsPortsParamsCfgGet(devNum, portGroup,
                              localPort, portMode, &macNum, &pcsNum,
                              &sdVecSize, &sdVectorPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(NA_NUM == macNum)
    {/* i.e. current port can't support required ifMode */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(notEnoughInfo && (macNum > 11))
    {/* if configured local port 9 or 11 and it uses extended MAC
        for 1 serdes mode too, then we have enough info to check validity */
        notEnoughInfo = GT_FALSE;
    }

    switch(PRV_CPSS_PP_MAC(devNum)->devType)
    {
        case CPSS_LION_2_3_THREE_MINI_GOPS_DEVICES_CASES_MAC:
            if(macNum > 11)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

            break;

        default:
            break;
    }

    if((macNum%2 != 0) && (macNum < 12) && (!notEnoughInfo))
    {
        if(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
        {
            rc = cpssDxChPortPizzaArbiterIfPortStateGet(devNum, portGroup,
                                                        localPort-1, &sliceNumUsed);
            if(rc != GT_OK)
            {
                return rc;
            }
            if(sliceNumUsed > 1)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
        else if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum-1) >= PRV_CPSS_PORT_XLG_E)
        {/* i.e. odd port's serdeses occupied by neighbor even port */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    if((macNum%4 != 0) && (!notEnoughInfo))
    {
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
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
            {
                rc = cpssDxChPortPizzaArbiterIfPortStateGet(devNum, portGroup,
                                                            localPortTmp, &sliceNumUsed);
                if(rc != GT_OK)
                {
                    return rc;
                }
                if(sliceNumUsed > 2)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
            else if(sdVecSizeTmp >= 4)
            {/* i.e. current port's serdeses occupied by first port in its quadruplet */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
    }

    speedOld = PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portNum);
    ifModeOld = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum);
    if((!notEnoughInfo) && (ifModeOld != CPSS_PORT_INTERFACE_MODE_NA_E)
                                        && (speedOld != CPSS_PORT_SPEED_NA_E))
    {/* if we sure new mode is valid - reset port accordingly to old mode */
        rc = prvCpssLion2CpssIfModeToHwsTranslate(ifModeOld, speedOld, &portMode);
        if(rc != GT_OK)
        {
            return rc;
        }
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortReset(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], action[%d])", devNum, portGroup, localPort, portMode, PORT_POWER_DOWN);
        rc = mvHwsPortReset(devNum, portGroup, localPort, portMode,
                            PORT_POWER_DOWN);
        if (rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
    }

    if((GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_LION2_HGL_MAC_INT_WA_E))
       && (PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum)->ppEventsCfg[portGroup].intVecNum !=
           CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS))
    {/* mask the HGL MAC interrupts if the portInterface != HGL*/
        /* convert the 'Physical port' to portGroupId -- supporting multi-port-groups device */
        GT_32  intKey;

        (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

        /* unmask - i.e. event enable.                */
        /* prvCpssDrvEventsMask may return not GT_OK  */
        /* if interrupt was not binded by application */
        /* or interrupts are not used.                */
        /* Need to ignore this fail.                  */
        (GT_VOID)prvCpssDrvEventsMask(devNum,
            lion2PortUnitHglSumIndex(localPort),
            (ifMode != CPSS_PORT_INTERFACE_MODE_HGL_E));
        (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortInterfaceModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
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
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInterfaceModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_GET_FUN ifModeGetFuncPtr; /* pointer to
            per device_family function returning current port interface mode */
    GT_U32  portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(ifModePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_NOT_EXISTS_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if((ifModeGetFuncPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->devObj.portPtr->
                                                getPortInterfaceMode) == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    return ifModeGetFuncPtr(devNum, portNum, ifModePtr);
}

/*******************************************************************************
* cpssDxChPortInterfaceModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
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
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterfaceModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInterfaceModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ifModePtr));

    rc = internal_cpssDxChPortInterfaceModeGet(devNum, portNum, ifModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ifModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* lionIfModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lionIfModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_STATUS   rc;
    GT_U32  portMacNum;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,portMacNum);
    devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        if(portNum == CPSS_CPU_PORT_NUM_CNS-1)
        {
CPSS_TBD_BOOKMARK_BOBCAT2
        }
    }

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        return cpuPortIfModeGet(devNum,portNum,ifModePtr);

    *ifModePtr = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portMacNum);

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum,
                                                            &portMapShadowPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E == portMapShadowPtr->portMap.mappingType)
        {
            if((CPSS_PORT_INTERFACE_MODE_ILKN4_E != *ifModePtr) &&
                (CPSS_PORT_INTERFACE_MODE_ILKN8_E != *ifModePtr))
            {/* interlaken channel mapped to physical MAC that can serve not
                interlaken, but it's wrong to relate to channel anything
                else then ILKN mode */
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
            }
        }
    }

    if(CPSS_PP_FAMILY_DXCH_LION_E == devFamily)
    {
        GT_PHYSICAL_PORT_NUM localPort;   /* number of port in port group */

        /* in available Lion boxes port 10 capable for 40G support
         * it's implemented by taking over serdeses of ports 8,9,10,11
         * so when port 10 configured to XLG mode ports 8,9,11 not available
         */
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        if (localPort == 8 || localPort == 9 || localPort == 11)
        {
            GT_U32 portGroupId; /* port group ID */
            GT_PHYSICAL_PORT_NUM tempPort; /* global number of port 10 in given port group */

            portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
            tempPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,10);
            if (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,tempPort) == CPSS_PORT_INTERFACE_MODE_XLG_E)
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
                return GT_OK;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* xcatPortIfModeHwGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port from HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (except CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS xcatPortIfModeHwGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* register value */

    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, portNum + 2, 1, &value) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        if (value == 0) /* GE mac unit used */
        {
            /* get 100FX enable from 100FX Configuration Register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                    perPortRegs[portNum].Config100Fx;
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 7, 1, &value) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }

            if (value == 1)
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_100BASE_FX_E;
            }
            else
            {
                /* get PortType from Port MAC Control Register0*/
                PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,
                                PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
                if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        regAddr, 1, 1, &value) != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                }

                *ifModePtr = (value == 0) ? CPSS_PORT_INTERFACE_MODE_SGMII_E :
                                                CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
            }
        }
        else /* if XG mac unit used */
        {
            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_GE_E)
            {/* if GE MAC used it's SGMII 2.5G */
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_SGMII_E;
                return GT_OK;
            }

            /* Get LaneActive in Global Configuration0 register*/
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].xgGlobalConfReg0;
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 5, 2, &value) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }

            switch (value)
            {
                case 0:
                    *ifModePtr = CPSS_PORT_INTERFACE_MODE_QX_E;
                    break;
                case 1:
                    *ifModePtr = CPSS_PORT_INTERFACE_MODE_HX_E;
                     break;
                case 2:
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;
                    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                            regAddr, portNum, 1, &value) != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                    }

                     *ifModePtr = (value == 0) ? CPSS_PORT_INTERFACE_MODE_XGMII_E :
                                                        CPSS_PORT_INTERFACE_MODE_RXAUI_E;
                     break;
                default:
                     CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }

        return GT_OK;
    }/* end of flex link case */

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
    {
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_MII_E;
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portNum,
                                    PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum), &regAddr);
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, 1, 1, &value) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    if (value == 1)
    {
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;
        if(prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 15, 6, &value) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

        if(value & (1<<(portNum/4)))
        {
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_SGMII_E;
        }
        else
        {
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_QSGMII_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* xcatPortIfModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS xcatPortIfModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        return cpuPortIfModeGet(devNum,portNum,ifModePtr);

    *ifModePtr = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum);
    if(*ifModePtr != CPSS_PORT_INTERFACE_MODE_NA_E)
        return GT_OK;

    return xcatPortIfModeHwGet(devNum, portNum, ifModePtr);
}

/*******************************************************************************
* cheetah1_3_PortIfModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Lion; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS cheetah1_3_PortIfModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_U32      regAddr;/* register address */
    GT_U32      regValue;/* register value */
    GT_STATUS   rc;     /* return code */

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
        return cpuPortIfModeGet(devNum,portNum,ifModePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hxPortGlobalConfig;
        if((PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum) == PRV_CPSS_XG_PORT_XG_ONLY_E)
            || (PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr))
        {
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_XGMII_E;
            return GT_OK;
        }

        /* at this point the port can be either XG or HX/QX - read port25Mode field
           to get the mode */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 15, 1, &regValue);
        if (rc != GT_OK)
        {
            return GT_FALSE;
        }

        if(regValue)
        {
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_XGMII_E;
        }
        else
        {
            if( (25 != portNum) && (26 != portNum) )
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
                hxPortConfig0[portNum-25];
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 5, 1, &regValue);
            if (rc != GT_OK)
            {
                return GT_FALSE;
            }
            *ifModePtr = (regValue) ? CPSS_PORT_INTERFACE_MODE_HX_E : CPSS_PORT_INTERFACE_MODE_QX_E;
        }

        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portNum,
                                        PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum), &regAddr);
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, 1, 1, &regValue) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *ifModePtr = (regValue == 1) ? CPSS_PORT_INTERFACE_MODE_1000BASE_X_E :
                                        CPSS_PORT_INTERFACE_MODE_SGMII_E;

    return GT_OK;
}

/*******************************************************************************
* cpuPortIfModeGet
*
* DESCRIPTION:
*       Gets Interface mode of CPU port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
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
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS cpuPortIfModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_U32 value;   /* register value */
    GT_STATUS rc;   /* return code */

    if(portNum != CPSS_CPU_PORT_NUM_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E == portMapShadowPtr->portMap.mappingType)
        {/* return dummy ifMode to support legacy behavior */
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_MII_E;
            return GT_OK;
        }
        else
        {
            return prvCpssDxChPortLion2InterfaceModeHwGet(devNum,portNum,ifModePtr);
        }
    }

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg,
            1, 2, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    switch(value)
    {
        case 0:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_MII_E;
            break;
        case 1:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_GMII_E;
            break;
        case 2:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E;
            break;
        case 3:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_MII_PHY_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortForceLinkDownEnable
*
* DESCRIPTION:
*       Enable Force link down on a specified port on specified device and
*           read current force link down state of it.
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
*       portNum  - physical port number, CPU port number
*
* OUTPUTS:
*       linkDownStatusPtr - (ptr to) current force link down state of port:
*                                   GT_TRUE  - enable force link down on port,
*                                   GT_FALSE - disable force link down on port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortForceLinkDownEnable
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL *linkDownStatusPtr
)
{
    GT_STATUS rc; /* return code */

    /* Get force link down status */
    if((rc = cpssDxChPortForceLinkDownEnableGet(devNum, portNum, linkDownStatusPtr)) != GT_OK)
        return rc;

    if(*linkDownStatusPtr == GT_FALSE)
    {/* Set force link down */
        if((rc = cpssDxChPortForceLinkDownEnableSet(devNum,portNum,GT_TRUE)) != GT_OK)
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortForceLinkDownDisable
*
* DESCRIPTION:
*       Disable Force link down on a specified port on specified device if
*       it was previously disabled.
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
*       portNum  - physical port number, CPU port number
*       linkDownStatus - previous force link down state:
*                                   GT_TRUE  - enable force link down on port,
*                                   GT_FALSE - disable force link down on port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortForceLinkDownDisable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL linkDownStatus
)
{
    GT_STATUS rc;   /* return code */

    if(linkDownStatus == GT_FALSE)
    {
        if((rc = cpssDxChPortForceLinkDownEnableSet(devNum,portNum,GT_FALSE)) != GT_OK)
            return rc;
    }

    return GT_OK;
}

#define PRV_CPSS_DXCH_PORT_BOBCAT2_MINIMAL_IPG_CNS  2
#define PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_CONSTANT_CNS 20
#define PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_DFB_CNS      16
#define PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_SPARE_CNS    100
#define PRV_CPSS_DXCH_PORT_BOBCAT2_DESC_FIFO_SIZE_CNS   206

#define PRV_CPSS_DXCH_PORT_BOBCAT2_MPPM_MIN_LATENCY_CNS         32
#define PRV_CPSS_DXCH_PORT_BOBCAT2_MAXIMAL_SINGLE_BANK_FIFO_FILL_LEVEL_CNS  70
#define PRV_CPSS_DXCH_PORT_BOBCAT2_NUMBER_OF_MPPM_CLIENTS_CNS   2
#define PRV_CPSS_DXCH_PORT_BOBCAT2_OTHERS_LATENCY_CNS           267

#define PRV_CPSS_DXCH_PORT_BOBCAT2_PAYLOAD_CREDITS_CNS  541
#define PRV_CPSS_DXCH_PORT_BOBCAT2_HEADER_CREDITS_CNS   541


/*******************************************************************************
* prvCpssDxChPortBcat2PtpInterfaceWidthSelect
*
* DESCRIPTION:
*       Set PTP interface width.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portMacNum  - port MAC number (not CPU port)
*       speed       - port data speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong speed
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortBcat2PtpInterfaceWidthSelect
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          portMacNum,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_U32      regAddr;            /* register address */
    GT_U32      interfaceWidth;     /* PTP interface width */

    /* Interface width select */
    switch(speed)
    {
        case CPSS_PORT_SPEED_10_E:
        case CPSS_PORT_SPEED_100_E:
        case CPSS_PORT_SPEED_1000_E:
            interfaceWidth = 0;
            break;
        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_12000_E:
        case CPSS_PORT_SPEED_2500_E:
        case CPSS_PORT_SPEED_5000_E:
        case CPSS_PORT_SPEED_13600_E:
        case CPSS_PORT_SPEED_16000_E:
        case CPSS_PORT_SPEED_15000_E:
        case CPSS_PORT_SPEED_11800_E:
            interfaceWidth = 1;
            break;
        case CPSS_PORT_SPEED_40000_E:
        case CPSS_PORT_SPEED_47200_E:
            interfaceWidth = 2;
            break;
        case CPSS_PORT_SPEED_75000_E:
        case CPSS_PORT_SPEED_100G_E:
        case CPSS_PORT_SPEED_50000_E:
        case CPSS_PORT_SPEED_140G_E:
            interfaceWidth = 3;
            break;

        case CPSS_PORT_SPEED_20000_E:
            /* for Caelum devices 20G port should be treated as 256bit width */
            if(PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum) ||
               PRV_CPSS_DXCH_CAELUM_CHECK_MAC(devNum) ||
               PRV_CPSS_DXCH_ALDRIN_CHECK_MAC(devNum)
              )
            {
                interfaceWidth = 2;
            }
            else
            {
                interfaceWidth = 1;
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, portMacNum).PTPGeneralCtrl;

    /* Set interface width select bits[3:2] */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 2, 2, interfaceWidth);
}

/*******************************************************************************
* prvCpssDxChPortBcat2InterlakenDelete
*
* DESCRIPTION:
*       Reset ILKN physical layer.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBcat2InterlakenDelete
(
    IN  GT_U8                           devNum
)
{
    GT_STATUS               rc; /* return code */
    GT_U32                  i;  /* iterator */
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr; /* ptr to ports entry in
                                                            mapping shadow DB */

    /*-------------------------------------------------*/
    /* disable all channels and config them to 0 speed */
    /*-------------------------------------------------*/
    for(i = 0; i < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; i++)
    {
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, i,
                                                       &portMapShadowPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(portMapShadowPtr->valid)
        {
            if(CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E ==
                            portMapShadowPtr->portMap.mappingType)
            {
                rc = cpssDxChPortIlknChannelEnableSet(devNum, i,
                                                      CPSS_PORT_DIRECTION_BOTH_E,
                                                      GT_FALSE);
                if(rc != GT_OK)
                {
                    return rc;
                }

                rc = cpssDxChPortIlknChannelSpeedSet(devNum, i,
                                                     CPSS_PORT_DIRECTION_BOTH_E,
                                                     0, NULL);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortBcat2PortReset
*
* DESCRIPTION:
*       Power down port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - interface mode according to which power down port
*       speed     - port data speed according to which power down port
*       qsgmiiFullDelete    - GT_TRUE - power down QSGMII interface
*                             GT_FALSE - just free resources of this specific
*                                       member of QSGMII quadruplet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBcat2PortReset
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_BOOL                         qsgmiiFullDelete
)
{
    GT_STATUS               rc;         /* return code */
    GT_U32                  portMacMap; /* number of mac mapped to this physical port */
    MV_HWS_PORT_STANDARD    portMode;   /* port interface/speed in terms of HWS */
#ifndef GM_USED
    GT_U32                   macNum, /* number of MAC used by port for given ifMode */
                             pcsNum, /* number of PCS used by port for given ifMode */
                             sdVecSize; /* size of serdes redundancy array */
    GT_U32                   *sdVectorPtr; /* serdes redundancy array */
#endif

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacMap);

    rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
    if(rc != GT_OK)
    {
        return rc;
    }

#ifndef GM_USED
    if(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
    {
        /* reset just units used for loopback */
        if(CPSS_PORT_SPEED_1000_E == speed)
        {
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsMacReset(devNum[%d], portGroup[%d], macNum[%d], macType[%d], action[%d])", devNum, 0, portMacMap, GEMAC_X, RESET);
            rc = mvHwsMacReset(devNum, 0, portMacMap, GEMAC_X, RESET);
        }
        else
        {
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsMacReset(devNum[%d], portGroup[%d], macNum[%d], macType[%d], action[%d])", devNum, 0, portMacMap, XLGMAC, RESET);
            rc = mvHwsMacReset(devNum, 0, portMacMap, XLGMAC, RESET);
        }
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "Error in mvHwsMacReset, code %d for port[%d]\n", rc, portMacMap);
        }
        CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%d], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, portMacMap, portMode);
        rc = hwsPortsParamsCfgGet(devNum, 0,
                                  portMacMap, portMode, &macNum,
                                  &pcsNum, &sdVecSize, &sdVectorPtr);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "Error in hwsPortsParamsCfgGet, code %d for port[%d]\n", rc, portMacMap);
        }
        if(CPSS_PORT_SPEED_1000_E == speed)
        {
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPcsReset(devNum[%d], portGroup[%d], pcsNum[%d], pcsType[%d], action[%d])", devNum, 0, pcsNum, GPCS, RESET);
            rc = mvHwsPcsReset(devNum, 0, pcsNum, GPCS, RESET);
        }
        else
        {
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPcsReset(devNum[%d], portGroup[%d], pcsNum[%d], pcsType[%d], action[%d])", devNum, 0, pcsNum, MMPCS, RESET);
            rc = mvHwsPcsReset(devNum, 0, pcsNum, MMPCS, RESET);
        }
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "Error in mvHwsPcsReset, code %d for port[%d]\n", rc, portMacMap);
        }
    }
    else
    {
        if((qsgmiiFullDelete) && (portMode != NON_SUP_MODE))
        {
            if(ifMode == CPSS_PORT_INTERFACE_MODE_QSGMII_E)
            {
                /* the QSGMII sets 4 ports on the same SERDES and those 4 MACs should be aligned */
                /* NOTE: the qsgmiiFullDelete makes sure that 'last' port 'valid' in the 4 ports
                   will get into this code */
                portMacMap = portMacMap & (~0x3);/* clear the 2 LSBits */
            }


            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortReset(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], action[%d])", devNum, 0, portMacMap, portMode, PORT_POWER_DOWN);
            rc = mvHwsPortReset(devNum, 0, portMacMap, portMode, PORT_POWER_DOWN);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "Error in mvHwsPortReset, code %d for port[%d], portMode[%d]\n", rc, portMacMap, portMode);
            }
        }
    }
#else
    /* to prevent warnings in GM compilation */
    (GT_VOID)qsgmiiFullDelete;
#endif

    return GT_OK;
}

#if 0
/*******************************************************************************
* prvCpssDxChPortBcat2TxFifoThresholdsSet
*
* DESCRIPTION:
*   Set the thresholds in the port?s TXFIFO per it's current status/interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       ifMode      - interface mode
*       speed       - port data speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For B0 and above.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBcat2TxFifoThresholdsSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
#define PRV_CPSS_BOBCAT2_PORT_GE_FIFO_HIGH_WM_CNS   139

    GT_STATUS   rc;     /* return code */
    GT_U32      portMacMap; /* number of MAC mapped on this physical port */
    GT_U32      regAddr;/* register address */
    GT_U32      txFifoMinTh, /* GE TX FIFO minimum threshod */
                wmValue;     /* high and low water marks values */
    GT_U32      txRdThr;    /* Number of FIFO entries required to commence
                                reading from the FIFO.*/

    if(!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {/* not needed for A0 */
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacMap);

    switch (ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
        case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
        case CPSS_PORT_INTERFACE_MODE_100BASE_FX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                    perPortRegs[portMacMap].gePortFIFOConfigReg1;
            txFifoMinTh = (speed <= CPSS_PORT_SPEED_1000_E) ? 50 : 136;
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 6, 8, txFifoMinTh);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* When setting the GIGE TxFifoMinTh need to also set the WM fields
             *  so that: 140 > High_WM > Low_WM > MinTh.
             *  Any WM values that meet the above constrains are OK.
             */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                    perPortRegs[portMacMap].gePortFIFOConfigReg0;
            wmValue = ((txFifoMinTh+1)<<8) | PRV_CPSS_BOBCAT2_PORT_GE_FIFO_HIGH_WM_CNS;
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, wmValue);
            if(rc != GT_OK)
            {
                return rc;
            }
            break;

        default:

            switch(speed)
            {
                case CPSS_PORT_SPEED_10000_E:
                case CPSS_PORT_SPEED_12000_E:
                    txRdThr = 22;
                    break;
                case CPSS_PORT_SPEED_20000_E:
                    txRdThr = 25;
                    break;

                case CPSS_PORT_SPEED_40000_E:
                    txRdThr = 31;
                    break;

                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                            perPortRegs[portMacMap].xlgPortFIFOsThresholdsConfig;
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 5, txRdThr);
            if(rc != GT_OK)
            {
                return rc;
            }

            break;
    }

    return GT_OK;
}
#endif

/*******************************************************************************
* prvCpssDxChPortBc2PtpTimeStampFixWa
*
* DESCRIPTION:
*   Set the thresholds in ports Tx FIFO
*
* APPLICABLE DEVICES:
*        Bobcat2
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       WA fix PTP timestamp problem
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortBc2PtpTimeStampFixWa
(
    IN  GT_U8                           devNum
)
{
    GT_STATUS rc;           /* return code */
    GT_U32    port;         /* port number */
    GT_U32    txFifoLoWm;   /* Tx FIFO low level value */
    GT_U32    txFifoHiWm;   /* Tx FIFO high level value */
    GT_U32    txFifoMin;    /* Tx FIFO minimal level value */
    GT_U32    txFifoSize;   /* Tx FIFO size */
    GT_U32    txRdThreshold;/* Tx FIFO read threshold */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    txFifoLoWm = 0xC;
    txFifoHiWm = 0xE;
    txFifoMin = 0x9;
    txFifoSize = 0xB;
    txRdThreshold = 0xA;

    /* Port FIFO Thresholds Confiruration Registers */
    for (port= 0; (port < PRV_CPSS_PP_MAC(devNum)->numOfPorts); port++)
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, port);

        rc = prvCpssHwPpSetRegField(devNum,
              PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[port].gePortFIFOConfigReg0,
              0, 16, ((txFifoLoWm << 8) | txFifoHiWm));
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssHwPpSetRegField(devNum,
              PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[port].gePortFIFOConfigReg1,
              6, 8, txFifoMin);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_GE_PORT_GE_ONLY_E != PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, port))
        {
            rc = prvCpssHwPpSetRegField(devNum, 
                  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[port].xlgPortFIFOsThresholdsConfig, 
                  5, 11, ((txRdThreshold << 6) | txFifoSize));
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    PRV_CPSS_DXCH_PP_MAC(devNum)->errata.info_CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E.enabled = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortBcat2PortDelete
*
* DESCRIPTION:
*       Power down port and free all allocated for it resources.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - interface mode according to which power down port
*       speed     - port data speed according to which power down port
*       qsgmiiFullDelete    - GT_TRUE - power down QSGMII interface
*                             GT_FALSE - just free resources of this specific
*                                       member of QSGMII quadruplet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBcat2PortDelete
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_BOOL                         qsgmiiFullDelete
)
{
    GT_STATUS               rc;         /* return code */
    GT_U32                  portMacMap; /* number of mac mapped to this physical port */
    CPSS_PORT_INTERFACE_MODE_ENT *portIfModePtr;/* interface configured on port now */
    CPSS_PORT_SPEED_ENT          *portSpeedPtr; /* speed configured on port now */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacMap);

    portIfModePtr = &(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacMap));
    if((CPSS_PORT_INTERFACE_MODE_ILKN4_E == ifMode)
        ||  (CPSS_PORT_INTERFACE_MODE_ILKN8_E == ifMode)
        ||  (CPSS_PORT_INTERFACE_MODE_ILKN4_E == *portIfModePtr)
        ||  (CPSS_PORT_INTERFACE_MODE_ILKN8_E == *portIfModePtr))
    {/* if new or old mode is ILKN */
        rc = prvCpssDxChPortBcat2InterlakenDelete(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* if on port was configured interface - reset physical layer and free resources */
    if(*portIfModePtr != CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        portSpeedPtr = &(PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacMap));
        rc = prvCpssDxChPortBcat2PortReset(devNum, portNum, *portIfModePtr,
                                           *portSpeedPtr, qsgmiiFullDelete);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChBcat2PortResourcesConfig(devNum, portNum, ifMode,
                                                 speed, GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Set PTP interface width */
        rc = prvCpssDxChPortBcat2PtpInterfaceWidthSelect(devNum, portMacMap,
                                                         CPSS_PORT_SPEED_10_E);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortPizzaArbiterIfDelete(devNum, portNum);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* reset port according to new interface */
    rc = prvCpssDxChPortBcat2PortReset(devNum, portNum, ifMode, speed,
                                       qsgmiiFullDelete);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortBcat2InterlakenConfigSet
*
* DESCRIPTION:
*       Configure Interlaken interface.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBcat2InterlakenConfigSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      bwInGBit;   /* bandwidth of ILKN interface in Gb */
    GT_U32      estBWMBps;  /* real bandwidth in Bps based on allocation
                                                                granularity */
    GT_U32      regAddr;/* register address */

    /* init PR memory management */
    prvCpssDxChPortIlknPrMemInit();

    bwInGBit = (CPSS_PORT_SPEED_20000_E == speed) ? 20 : 40;
    rc = prvCpssDxChPortIlknIFBWSet(devNum, bwInGBit, /*OUT*/&estBWMBps);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.PR.PRFunctional.minMaxPkts;
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0A00003F);
    if(GT_OK != rc)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.PR.PRFunctional.general;

    /* enable to recalculate CRC in outgoing descriptor */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, 1);
}

/*******************************************************************************
* prvCpssDxChPortBcat2InterlakenTxDmaEnableSet
*
* DESCRIPTION:
*       Set Tx DMA enable state for Interlaken interface.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortBcat2InterlakenTxDmaEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;/* register address */


    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_ilkn.supported == GT_FALSE)
    {
        /* nothing needed */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.ILKN_WRP.
                                                        txdmaConverterControl0;

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 17, 1, BOOL2BIT_MAC(enable));

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortBcat2FcaBusWidthSet
*
* DESCRIPTION:
*       FCA bus width configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       ifMode  - interface mode
*       speed   - port data speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported speed for given port
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortBcat2FcaBusWidthSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS rc; /* return code */
    GT_U32 regAddr; /* register address */
    GT_U32 regValue; /* register value */
    GT_U32 fieldOffset; /* register value */
    GT_U32 fieldLength; /* register value */
    GT_U32 portMacNum; /* number of mac mapped to this physical port */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacNum);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
    fieldOffset = 4;
    fieldLength = 3;

    if(( ifMode == CPSS_PORT_INTERFACE_MODE_ILKN4_E  ) ||
       ( ifMode == CPSS_PORT_INTERFACE_MODE_ILKN8_E  ) ||
       ( ifMode == CPSS_PORT_INTERFACE_MODE_ILKN12_E ) ||
       ( ifMode == CPSS_PORT_INTERFACE_MODE_ILKN16_E ) ||
       ( ifMode == CPSS_PORT_INTERFACE_MODE_ILKN24_E ))
    {
        regValue = 4; /* width 512bit */
    }
    else
    {
        switch(speed)
        {
            case CPSS_PORT_SPEED_10_E:
            case CPSS_PORT_SPEED_100_E:
            case CPSS_PORT_SPEED_1000_E:
                regValue = 0; /* width 8bit */
                break;

            case CPSS_PORT_SPEED_10000_E:
            case CPSS_PORT_SPEED_12000_E:
            case CPSS_PORT_SPEED_2500_E:
            case CPSS_PORT_SPEED_5000_E:
            case CPSS_PORT_SPEED_11800_E:
                regValue = 1; /* width 64bit */
                break;


            case CPSS_PORT_SPEED_40000_E:
            case CPSS_PORT_SPEED_47200_E:
                regValue = 3; /* width256 */
                break;

            case CPSS_PORT_SPEED_20000_E:
                if(PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum) ||
                   PRV_CPSS_DXCH_CAELUM_CHECK_MAC(devNum) ||
                   PRV_CPSS_DXCH_ALDRIN_CHECK_MAC(devNum) )
                {
                    regValue = 3; /* width256 */
                }
                else
                {
                    regValue = 1; /* width 64bit */
                }
                break;


            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    rc =  prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, regValue);
    return rc;
}

/*******************************************************************************
* bcat2QsgmiiConfig
*
* DESCRIPTION:
*       Check if port needs configuration on MAC/PCS/Serdes levels,
*       force link down on ports of its quadruplet if application not
*       configured them yet.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       fullConfigPtr - call port delete/create or just configure pizza
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       In QSGMII can't configure only one port, because of internal physical
*       synchronization issues, but we don't want application get linkup interrupts
*       from ports it
*
*******************************************************************************/
static GT_STATUS bcat2QsgmiiConfig
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 powerUp,
    OUT GT_BOOL                 *fullConfigPtr
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;  /* iterator */
    GT_PHYSICAL_PORT_NUM    portMacNum; /* MAC number mapped to given port */
    GT_U32  firstInQuadruplet;  /* mac number of first port in quadruplet */
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,
                                                                 portMacNum);

    if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* do nothing - no such configuration */
    }
    else
    {
        /* because force link down doesn't prevent packets to enter queue,
           must set link down here */
        if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
        {
            value = ((powerUp) ? 0 /*use MAC indication*/: 2/* force link down*/);

            rc = prvCpssDxChHwEgfEftFieldSet(devNum,portMacNum,
                PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_PHYSICAL_PORT_LINK_STATUS_MASK_E,
                value);
        }
        else /* sip_5_20 */
        {
            value = ((powerUp) ? 1 /* force link up*/: 0/* force link down*/);

            if(value == 0)
            {
                /* the call is on 'physical port' (not on 'MAC port') */
                rc = prvCpssDxChHwEgfEftFieldSet(devNum,portNum,
                    PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_PHYSICAL_PORT_LINK_STATUS_MASK_E,
                    value);
            }
            else
            {
                /* we can not force link UP ... we need to wait for interrupt
                   from the MAC to know that ports is actually 'link UP'

                   FIX JIRA : CPSS-4899 : Ports 0-11 in link up when working with higher ports
                */
                rc = GT_OK;
            }
        }

        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
    }

    *fullConfigPtr = GT_TRUE;
    if((GT_TRUE == powerUp) && (CPSS_PORT_INTERFACE_MODE_QSGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum)))
    {
        *fullConfigPtr = GT_FALSE;
        return GT_OK;
    }

    firstInQuadruplet = portMacNum&0xFFFFFFFC;
    if(GT_TRUE == powerUp)
    {
        for(i = firstInQuadruplet; i < firstInQuadruplet+4; i++)
        {
            if(i == portMacNum)
            {
                /* set application configuration for force link down */
                rc = prvCpssDxChPortForceLinkDownEnableSet(devNum,portNum,
                            CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.info_PRV_CPSS_DXCH_LION2_DISMATCH_PORT_LINK_WA_E.portForceLinkDownBmpPtr,portNum));
                if(rc != GT_OK)
                {
                    return rc;
                }

                continue;
            }
            if(CPSS_PORT_INTERFACE_MODE_NA_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, i))
            {
                /* because HWS configures all four ports of QSGMII quadruplet
                   when application requires to configure one of them,
                   force link down on GE MAC of port to prevent link on ports
                   not configured yet by application */
                PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,i,&regAddr);
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum,
                                                       CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                       regAddr, 0, 1, 1) != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                }
            }
            else if(CPSS_PORT_INTERFACE_MODE_QSGMII_E ==
                                        PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, i))
            {
                *fullConfigPtr = GT_FALSE;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
    }
    else
    {
        for(i = firstInQuadruplet; i < firstInQuadruplet+4; i++)
        {
            if(i != portMacNum)
            {
                if(CPSS_PORT_INTERFACE_MODE_NA_E !=
                                            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, i))
                {
                    *fullConfigPtr = GT_FALSE;
                    break;
                }
            }
        }

        if(*fullConfigPtr == GT_TRUE)
        {
            for(i = firstInQuadruplet; i < firstInQuadruplet+4; i++)
            {
                /* set application configuration for force link down */
                rc = prvCpssDxChPortForceLinkDownEnableSet(devNum,portNum,
                            CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.info_PRV_CPSS_DXCH_LION2_DISMATCH_PORT_LINK_WA_E.portForceLinkDownBmpPtr,portNum));
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        else
        {
            /* set force link down enable in case that SERDES is still in power up state*/
            rc = prvCpssDxChPortForceLinkDownEnableSet(devNum,portNum,GT_TRUE);
            if(rc != GT_OK)
            {
                return rc;
            }

        }
    }

    return GT_OK;
}

#ifndef GM_USED
/*******************************************************************************
* prvCpssDxChLedPortTypeConfig
*
* DESCRIPTION:
*       Configures the type of the port connected to the LED.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLedPortTypeConfig
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum
)
{
    GT_U32      portMacNum;      /* port MAC number */
    GT_U32      regAddr;         /* register address */
    GT_STATUS   rc;              /* return status*/
    GT_U32      ledInterfaceNum; /* LED interface number */
    GT_U32      ledPort;         /* LED port number */
    GT_U32      bitOffset;       /* register field offset */
    GT_U32      fieldValue;      /* register field value */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* port MAC type */
    GT_U32      class1Source;    /* source indication of class #1 - link status */

    if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum) == 0)
    {
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacNum);

    rc = cpssDxChLedStreamPortPositionGet(devNum, portNum, &ledPort);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (ledPort == 0x3f)
    {
        /* LED port not connected */
        return GT_OK;
    }

    if (ledPort > BOBCAT2_LED_INTERFACE_PORTS_NUM_CNS)
    {
        /* LED ports valid range is 0..11 */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortLedInterfaceGet(devNum, portNum, &ledInterfaceNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* use default source #1 for class 1 indication. */
    class1Source = 1;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.LED[ledInterfaceNum].portTypeConfig[0];
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portMacNum);
    switch (portMacType)
    {
        case PRV_CPSS_PORT_NOT_EXISTS_E:
        case PRV_CPSS_PORT_FE_E:
        case PRV_CPSS_PORT_GE_E:
            fieldValue = 0;
            /* use Link_test_fail indication for link status of GIG ports */
            class1Source = 0xa;
            break;
        case PRV_CPSS_PORT_XG_E:
        case PRV_CPSS_PORT_XLG_E:
            fieldValue = 1;
            break;
        case PRV_CPSS_PORT_CG_E:
            fieldValue = 2;
            break;
        case PRV_CPSS_PORT_ILKN_E:
            fieldValue = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    bitOffset = ledPort * 2;

    /* Ports type configuration */
    rc =  prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 2, fieldValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (GT_FALSE != PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_BOBCAT2_LED_LINK_GIG_INDICATION_WA_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->GOP.LED[ledInterfaceNum].portIndicationSelect[ledPort];
        bitOffset = 5;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 5, class1Source);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}
#endif

/*******************************************************************************
* prvCpssDxChPortSerdesFreeCheck
*
* DESCRIPTION:
*        Check that the Serdes for port Configuration is not free and not used
*        by another port
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       ifMode     - interface mode
*       speed      - port data speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_STATE             - on port cannot be configured when the serdes, that port need
*                                  for configuration is not free
*       GT_NOT_SUPPORTED         - i/f mode/speed pair not supported
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSerdesFreeCheck
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS               rc;           /* return code */
    GT_U32                  macNum;
    GT_U32                  pcsNum;
    GT_U32                  sdVecSize;
    GT_U32                 *sdVectorPtr;
    MV_HWS_PORT_STANDARD    portMode;
    GT_U32                  portRef;
    GT_U32                  divider;
    GT_U32                  difference;
    GT_U32                  portGroup;
    GT_BOOL                 extendedMode;
    GT_U32                  portMacMap;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacMap);

    if(PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacMap) != PRV_CPSS_GE_PORT_GE_ONLY_E)
    {

        /* Checking is serdes, needs for current port don't used already by port with mac number BEFORE current mac number */
        if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            rc = cpssDxChPortExtendedModeEnableGet(devNum, portNum, &extendedMode);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
            }

            if(GT_TRUE == extendedMode)
            {
                return GT_OK;
            }
        }

        for(divider = 2; divider <= 4; divider += 2)
        {
            portRef = (portMacMap / divider) * divider;
            difference = portMacMap - portRef;
            if(difference > 0)
            {
                /* skip not initialized ports */
                if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portRef].portIfMode == CPSS_PORT_INTERFACE_MODE_NA_E)
                {
                    continue;
                }
                rc = prvCpssLion2CpssIfModeToHwsTranslate(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portRef].portIfMode,
                    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portRef].portSpeed, &portMode);

                if(GT_NOT_INITIALIZED == rc)
                {
                    continue;
                }

                if(rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
                }

                portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portRef);

                rc = hwsPortsParamsCfgGet(devNum, portGroup,
                                          portRef, portMode, &macNum, &pcsNum,
                                          &sdVecSize, &sdVectorPtr);

                if(rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
                }

                if(sdVecSize > difference)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
        }

        /* Checking is serdes, needs for current port don't used already by port with mac number AFTER current mac number */
        rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }


        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacMap);

        rc = hwsPortsParamsCfgGet(devNum, portGroup,
                              portMacMap, portMode, &macNum, &pcsNum,
                              &sdVecSize, &sdVectorPtr);

        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        for(divider = 1; divider < sdVecSize; divider++)
        {

            if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
            {
                rc = cpssDxChPortExtendedModeEnableGet(devNum, portNum + divider, &extendedMode);
                if(rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
                }

                if(GT_TRUE == extendedMode)
                {
                    continue;
                }
            }

            rc = prvCpssLion2CpssIfModeToHwsTranslate(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacMap + divider].portIfMode,
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacMap + divider].portSpeed, &portMode);

            if(GT_NOT_INITIALIZED == rc)
            {
                continue;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortBcat2ModeSpeedSet
*
* DESCRIPTION:
*       Configure Interface mode and speed on a specified port and execute
*        on port's serdeses power up sequence; or configure power down on port's
*        serdeses.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number (or CPU port)
*       powerUp   - serdes power:
*                       GT_TRUE - up;
*                       GT_FALSE - down;
*       ifMode    - interface mode (related only for serdes power up [powerUp==GT_TRUE])
*       speed     - port data speed (related only for serdes power up [powerUp==GT_TRUE])
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBcat2ModeSpeedSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  GT_BOOL                         powerUp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS               rc;         /* return code */
    GT_PHYSICAL_PORT_NUM    portNum;    /* iterator */
    GT_U32                  portMacMap; /* number of mac mapped to this physical port */
    GT_BOOL                 supported;  /* is ifMode/speed supported on port */
    MV_HWS_PORT_STANDARD    portMode;
    GT_BOOL                 lbPort;
    MV_HWS_REF_CLOCK_SUP_VAL refClock;
    MV_HWS_PORT_INIT_FLAVOR_CFG  portFlavorParams;
    CPSS_PORT_INTERFACE_MODE_ENT *portIfModePtr;
    CPSS_PORT_SPEED_ENT          *portSpeedPtr;
    GT_BOOL                  fullConfig; /* because in QSGMII mode one serdes serves four ports
                                                    when one port of the quadruplete configured three
                                                    others configured too, so when application asks
                                                    to configure another port from quadruplete we don't need
                                                    to execute full configuretion of new port */
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    GT_U32                   originalPortStateU32;
    GT_BOOL                  originalPortState = GT_TRUE; /* port enable state
                                                            defined by application */


    if(powerUp == GT_TRUE)
    {
        rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChSerdesRefClockTranslateCpss2Hws(devNum, &refClock);
        if (rc != GT_OK)
        {
            return rc;
        }

        lbPort = (CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode) ?
                                                        GT_TRUE : GT_FALSE;
    }
    else
    {/* to prevent warnings */
        portMode = NON_SUP_MODE;
        refClock = MHz_156;
        lbPort = GT_FALSE;
    }

    for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum))
        {
            PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                         portNum,
                                                                         portMacMap);
        }
        else
        {
            continue;
        }

        rc = prvCpssDxChPortSerdesFreeCheck(devNum,portNum,ifMode,speed);
        if(rc != GT_OK)
        {
             return rc;
        }

        if ((PRV_CPSS_GE_PORT_GE_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, portMacMap))
                && (portMacMap < 48) && (CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
                && (portMacMap != (portMacMap & 0xFFFFFFFC)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        portIfModePtr = &(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacMap));
        portSpeedPtr = &(PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacMap));

        if(powerUp == GT_FALSE)
        {
            /* we remove the ports, so need to use current interface and speed
             * if configured, not the values from the caller */
            if((CPSS_PORT_INTERFACE_MODE_NA_E != *portIfModePtr) &&
                                        (CPSS_PORT_SPEED_NA_E != *portSpeedPtr))
            {
                ifMode = *portIfModePtr;
                speed =  *portSpeedPtr;
            }
        }

        rc = prvCpssDxChPortInterfaceSpeedSupportLion2PlusGet(devNum, portMacMap,
                                                              ifMode, speed,
                                                              &supported);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(!supported)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChPortStateDisableSet(devNum, portNum, &originalPortStateU32);
        if (rc != GT_OK)
        {
            return rc;
        }
        originalPortState = originalPortStateU32;

        fullConfig = GT_TRUE;
        if(CPSS_PORT_INTERFACE_MODE_QSGMII_E == ifMode)
        {
            rc = bcat2QsgmiiConfig(devNum, portNum, powerUp, &fullConfig);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        rc = prvCpssDxChPortBcat2PortDelete(devNum, portNum, ifMode, speed,
                                            fullConfig);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* save new interface mode in DB after prvCpssDxChPortBcat2PortDelete */
        *portIfModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
        *portSpeedPtr = CPSS_PORT_SPEED_NA_E;

        if(!powerUp)
        {
            rc = prvCpssDxChPortStateEnableSet(devNum, portNum, (GT_U32)originalPortState);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssDxChPortBcat2InterlakenTxDmaEnableSet(devNum, GT_FALSE);
            if (rc != GT_OK)
            {
                return rc;
            }

            continue;
        }

        rc = prvCpssDxChPortPizzaArbiterIfConfigure(devNum, portNum,
            ((speed < CPSS_PORT_SPEED_1000_E) ? CPSS_PORT_SPEED_1000_E : speed));
        if (rc != GT_OK)
        {
            return rc;
        }
#ifndef GM_USED
        rc = prvCpssDxChBcat2PortResourcesConfig(devNum, portNum, ifMode, speed,
                                                 GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
#endif
        /* Set FCA interface width */
        rc = prvCpssDxChPortBcat2FcaBusWidthSet(devNum, portNum, ifMode, speed);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Set PTP interface width */
        rc = prvCpssDxChPortBcat2PtpInterfaceWidthSelect(devNum, portMacMap, speed);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(portMapShadowPtr->portMap.trafficManagerEn)
        {
            /* Port speed calibration value for TM Flow Control */
            rc = prvCpssDxChTmGlueFlowControlPortSpeedSet(devNum,
                                              portMapShadowPtr->portMap.tmPortIdx,
                                                          speed);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if((CPSS_PORT_INTERFACE_MODE_ILKN4_E == ifMode)
            || (CPSS_PORT_INTERFACE_MODE_ILKN8_E == ifMode))
        {/* if new mode is ILKN */
            rc = prvCpssDxChPortBcat2InterlakenConfigSet(devNum, speed);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssDxChPortBcat2InterlakenTxDmaEnableSet(devNum, GT_TRUE);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if(fullConfig)
        {
            switch(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacMap].
                   portSerdesTuningType)
            {
                case CPSS_PORT_SERDES_TUNING_NONE_E:
                    portFlavorParams.portFlavor = PORT_REGULAR_INIT;
                    break;
                case CPSS_PORT_SERDES_TUNING_RX_ONLY_E:
                    portFlavorParams.portFlavor = PORT_RX_TRAIN_INIT;
                    break;
                case CPSS_PORT_SERDES_TUNING_TRX_E:
                    portFlavorParams.portFlavor = PORT_TRX_TRAIN_INIT;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(CPSS_PORT_INTERFACE_MODE_QSGMII_E == ifMode)
            {/* provide to HWS first port in quadruplet and it will configure
                all other ports if needed */
                portMacMap &= 0xFFFFFFFC;
            }

            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortFlavorInit(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], lbPort[%d], refClock[%d], refClockSource[%d], portFlavorParams{portFlavor[%d]})", devNum, 0, portMacMap, portMode, lbPort, refClock, PRIMARY_LINE_SRC, portFlavorParams.portFlavor);
            rc = mvHwsPortFlavorInit(devNum, 0, portMacMap, portMode, lbPort,
                                     refClock,
                                     PRIMARY_LINE_SRC,
                                     portFlavorParams);
            if (rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                return rc;
            }
        }

        prvCpssDxChPortTypeSet(devNum, portMacMap, ifMode, speed);

        /* update addresses of mac registers accordingly to used MAC GE/XLG/etc. */
        rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, portNum,ifMode);
        if (rc != GT_OK)
        {
            return rc;
        }

        if((CPSS_PORT_SPEED_10_E == speed) || (CPSS_PORT_SPEED_100_E == speed))
        {/* HWS doesn't support 10/100M, so after 1G configured by HWS, fix speed here */
            rc = geMacUnitSpeedSet(devNum, portNum, speed);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

#ifndef GM_USED
        /* Configures type of the port connected to the LED */
        rc = prvCpssDxChLedPortTypeConfig(devNum, portNum);
        if (rc != GT_OK)
        {
            return rc;
        }
#endif
        /* save new interface mode in DB */
        *portIfModePtr = ifMode;
        *portSpeedPtr = speed;

         /*Creating Port LoopBack*/
        if ((PRV_CPSS_PP_MAC(devNum)->revision > 0) && lbPort)
        {
            rc = prvCpssDxChPortSerdesLoopbackActivate (devNum, portNum, speed);
            if (rc != GT_OK)
            {
                 return rc;
            }
        }

        rc = prvCpssDxChPortStateEnableSet(devNum, portNum, (GT_U32)originalPortState);
        if (rc != GT_OK)
        {
            return rc;
        }

    } /* for(portNum = 0; */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortLoopbackDisableRestore
*
* DESCRIPTION:
*       Takes care to Set loopback to disable if it was
*       previousely set to enable.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number
*       powerUp   - serdes power:
*                       GT_TRUE - up;
*                       GT_FALSE - down;
*       ifMode    - interface mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortLoopbackDisableRestore
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  GT_BOOL                         powerUp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;  /* iterator */
    CPSS_PORT_INTERFACE_MODE_ENT prevIfMode; /* interface mode before setting */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    for(i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, i))
        {
            if(powerUp == GT_TRUE)
            {
                prevIfMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, i);
                if(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == prevIfMode)
                {
                    rc = prvCpssDxChPortLion2InternalLoopbackEnableSet(devNum, i, GT_FALSE);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
            else
            {
                if(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
                {
                    rc = prvCpssDxChPortLion2InternalLoopbackEnableSet(devNum, i, GT_FALSE);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
        }
        else
        {
            continue;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMoveDetect
*
* DESCRIPTION:
*       Detect if port moved from regular to extended MAC or back. which means
*        that although no change was in interface/speed and link on old
*        connection still up - port must be completely reconfigured.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       movedPtr  - GT_TRUE - change happend
*                   GT_FALSE - no change
*
* RETURNS:
*       GT_OK     - on success,
*       otherwise fail.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMoveDetect
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *movedPtr
)
{
    GT_STATUS               rc; /* return code */
    CPSS_PP_FAMILY_TYPE_ENT devFamily;  /* device family */

    devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;

    *movedPtr = GT_FALSE;

    if(CPSS_PP_FAMILY_DXCH_LION2_E == devFamily)
    {
        GT_BOOL extendedMode;   /* is port use extended MAC */
        GT_U32  macStatusRegAddr;/* current MAC status register */
        GT_U32  currentMacOffset; /* current offset of MAC registers of given
                                                                        port */
        GT_U32  localPort;

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
        if((localPort != 9) && (localPort != 11))
        {
            return GT_OK;
        }

        rc = cpssDxChPortExtendedModeEnableGet(devNum, portNum, &extendedMode);
        if(rc != GT_OK)
        {
            return rc;
        }

        PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum, portNum, &macStatusRegAddr);
        currentMacOffset = macStatusRegAddr & 0xF000;
        if(((GT_FALSE == extendedMode) && (currentMacOffset > 0xB000))
            || ((GT_TRUE == extendedMode) && (currentMacOffset <= 0xB000)))
        {/* i.e. extended mode changed before portModeSpeedSet engaged and it
            must be completely executed although link is still up and no change
            was in interface/speed */
            *movedPtr = GT_TRUE;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortModeSpeedSet
*
* DESCRIPTION:
*       Configure Interface mode and speed on a specified port and execute
*        on port's serdeses power up sequence; or configure power down on port's
*        serdeses.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number (or CPU port)
*       powerUp   - serdes power:
*                       GT_TRUE - up;
*                       GT_FALSE - down;
*       ifMode    - interface mode (related only for serdes power up [powerUp==GT_TRUE])
*       speed     - port data speed (related only for serdes power up [powerUp==GT_TRUE])
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        Supposed to replace old API's:
*           cpssDxChPortInterfaceModeSet
*           cpssDxChPortSpeedSet
*           cpssDxChPortSerdesPowerStatusSet
*
*       The API rolls back a port's mode and speed to their last values
*       if they cannot be set together on the device.
*
*       Pay attention! Before configure CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E
*       MUST execute power down for port with this interface.
*
*       Pay attention! Unlike other interfaces which are ready to forward traffic
*       after this API pass, interlaken interfaces require call afterwards
*       cpssDxChPortIlknChannelSpeedSet to configure channel.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortModeSpeedSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  GT_BOOL                         powerUp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;  /* iterator */
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN ifModeSetFuncPtr; /* pointer to
                                    [devFamily][ifMode] specific function */
    PRV_CPSS_DXCH_PORT_SERDES_POWER_STATUS_SET_FUN serdesPowerSetFuncPtr;
                            /* pointer to [devFamily] specific function */
    CPSS_PORT_INTERFACE_MODE_ENT prevIfMode =CPSS_PORT_INTERFACE_MODE_NA_E;
                            /* stores port's last interface mode*/
    CPSS_PORT_SPEED_ENT          prevSpeed; /* stores port's last speed*/
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_GET_FUN ifModeGetFuncPtr; /* pointer to
                                    [devFamily] specific function */
    GT_BOOL originalPortState; /* port enable state defined by application */
    GT_BOOL linkUp;             /* is link currently up */
    GT_BOOL moved;              /* port exit moved from regular to extended MAC */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E);

    if(powerUp == GT_TRUE)
    {
        if((ifMode >= CPSS_PORT_INTERFACE_MODE_NA_E) ||
                (ifMode < CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if((speed < CPSS_PORT_SPEED_10_E) || (speed >= CPSS_PORT_SPEED_NA_E))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* we not care about those ifMode,speed from the caller ! */
        /* as the port is going down */
        /* set dummy values */
        ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;
        speed  = CPSS_PORT_SPEED_1000_E;
    }

    if((PRV_CPSS_SIP_5_10_CHECK_MAC(devNum)) ||
        (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = prvCpssDxChPortLoopbackDisableRestore(devNum, portsBmp, powerUp, ifMode);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum) || (PRV_CPSS_DXCH_CAELUM_CHECK_MAC(devNum)))
    {
        return prvCpssDxChPortCaelumModeSpeedSet(devNum, portsBmp, powerUp, ifMode,
                                                speed);
    }
    if (PRV_CPSS_DXCH_ALDRIN_CHECK_MAC(devNum))
    {
        CPSS_TBD_BOOKMARK_ALDRIN /* use Caelum APIs so far */
        return prvCpssDxChPortCaelumModeSpeedSet(devNum, portsBmp, powerUp, ifMode,
                                                speed);
    }

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        return prvCpssDxChPortBcat2ModeSpeedSet(devNum, portsBmp, powerUp, ifMode,
                                                speed);
    }

    if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        return prvCpssDxChPortXcat3ModeSpeedSet(devNum, portsBmp, powerUp, ifMode,
                                                speed);
    }

    if(powerUp == GT_TRUE)
    {
        ifModeSetFuncPtr = PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->setPortInterfaceMode[ifMode];
        ifModeGetFuncPtr = PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->getPortInterfaceMode;
        if(ifModeSetFuncPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        ifModeGetFuncPtr = NULL;
        ifModeSetFuncPtr = NULL;
    }

    if((serdesPowerSetFuncPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->devObj.portPtr->
                                                setSerdesPowerStatus) == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for(i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, i))
        {
            PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,i);
        }
        else
        {
            continue;
        }

        if(i != CPSS_CPU_PORT_NUM_CNS)
        {
            GT_BOOL resNew;
            /*
            // resOld = supportedPortsModes[PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,i)][ifMode];
            */

            rc = prvCpssDxChPortSupportedModeCheck(devNum,i,ifMode,/*OUT*/&resNew);
            if (rc != GT_OK)
            {
                return GT_OK;
            }

            if (resNew == GT_FALSE)
            {

                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
        }

        /* save port's previous interface mode for rollback*/
        if ((ifModeGetFuncPtr != NULL) &&
                            (ifModeGetFuncPtr(devNum,i,&prevIfMode) != GT_OK))
        {
            prevIfMode = CPSS_PORT_INTERFACE_MODE_NA_E;
        }

        /* Get previous port speed, if failed set to N/A*/
        rc = prvCpssDxChPortSpeedGet(devNum,i,&prevSpeed);
        if(rc != GT_OK)
            prevSpeed = CPSS_PORT_SPEED_NA_E;

        /* if port already configured to required interface and link is up
            no need to reconfigure it */
        if((GT_TRUE == powerUp) && (prevIfMode == ifMode) && (prevSpeed == speed))
        {
            rc = cpssDxChPortLinkStatusGet(devNum, i, &linkUp);
            if (rc != GT_OK)
            {
                return rc;
            }

            if(linkUp)
            {
                /* check if new configuration done on different MAC used for
                    same physical port i.e. port move from regular MAC to
                    extended or back */
                rc = prvCpssDxChPortMoveDetect(devNum, i, &moved);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if(!moved)
                {/* move to next port if needed otherwise return OK */
                    continue;
                }
            }
        }

        /* disable port before port's units reset/power down to prevent traffic stuck */
        originalPortState = GT_FALSE;
        if(((CPSS_PORT_INTERFACE_MODE_HGL_E != prevIfMode) &&
                                (CPSS_PORT_INTERFACE_MODE_NA_E != prevIfMode))
            || ((CPSS_PORT_SPEED_15000_E != prevSpeed) &&
                                        (CPSS_PORT_SPEED_16000_E != prevSpeed)))
        {
            rc = prvCpssDxChPortEnableGet(devNum, i, &originalPortState);
            if (rc != GT_OK)
            {
                return rc;
            }

            if(originalPortState != GT_FALSE)
            {
                rc = prvCpssDxChPortEnableSet(devNum, i, GT_FALSE);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        if(GT_TRUE == powerUp)
        {
            /* configure requested interface mode on port */
            if((rc = ifModeSetFuncPtr(devNum, i, ifMode)) != GT_OK)
            {
                goto rollBackSpeedMode;
            }
            /* cpu port has not entry in phyPortInfoArray - skip SW port info DB update */
            if(i != CPSS_CPU_PORT_NUM_CNS)
            {
                /* update current port type - used to define which mac unit currently
                    in use by port */
                if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
                {
                    prvCpssDxChPortTypeSet(devNum,i,ifMode,speed);
                }

                /* save new interface mode in DB */
                PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, i) = ifMode;
            }

            /* Set port speed, if failed roll back changes*/
            rc = prvCpssDxChPortSpeedSet(devNum,i,speed);
            if(rc != GT_OK)
                goto rollBackSpeedMode;

            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                rc = prvCpssDxChPortForceLinkDownEnableSet(devNum, i, GT_FALSE);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

        }
        else
        {
            if(PRV_CPSS_PORT_FE_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,i))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG); /* Fast Ethernet ports have no serdes,
                                            so power down not applicable for them */

            if((CPSS_PORT_INTERFACE_MODE_NA_E == prevIfMode)
                                        || (CPSS_PORT_SPEED_NA_E == prevSpeed))
            {
                PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, i) = ifMode;
                PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, i) = speed;
            }

            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                rc = prvCpssDxChPortForceLinkDownEnableSet(devNum, i, GT_TRUE);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

        }

        if((i == CPSS_CPU_PORT_NUM_CNS) ||
            (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,i) == PRV_CPSS_PORT_FE_E))
        {
            continue;
        }

        rc = serdesPowerSetFuncPtr(devNum, i, CPSS_PORT_DIRECTION_BOTH_E, 0xFF, powerUp);
        if(rc != GT_OK)
        {
            goto rollBackSpeedMode;
        }

        if(originalPortState != GT_FALSE)
        {
            rc = prvCpssDxChPortEnableSet(devNum,i,GT_TRUE);
            if (rc != GT_OK)
            {
                goto rollBackSpeedMode;
            }
        }
    }

    return GT_OK;

    /* Rolls back changes to port and returns the rc which was retruned*/
rollBackSpeedMode:
     if ((ifModeGetFuncPtr != NULL) && (prevSpeed != CPSS_PORT_SPEED_NA_E)
         &&   (prevIfMode != CPSS_PORT_INTERFACE_MODE_NA_E))
     {
         ifModeSetFuncPtr(devNum, i, prevIfMode);
         if(i != CPSS_CPU_PORT_NUM_CNS)
         {
            /* roll back port type */
            if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                prvCpssDxChPortTypeSet(devNum,i,prevIfMode,speed);
            }

            /* return old interface mode to DB */
            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, i) = prevIfMode;
        }
         prvCpssDxChPortSpeedSet(devNum,i,prevSpeed);
         if((i != CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,i) != PRV_CPSS_PORT_FE_E))
             serdesPowerSetFuncPtr(devNum, i, CPSS_PORT_DIRECTION_BOTH_E, 0xFF, powerUp);
     }

    return rc;
}

/*******************************************************************************
* cpssDxChPortModeSpeedSet
*
* DESCRIPTION:
*       Configure Interface mode and speed on a specified port and execute
*        on port's serdeses power up sequence; or configure power down on port's
*        serdeses.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number (or CPU port)
*       powerUp   - serdes power:
*                       GT_TRUE - up;
*                       GT_FALSE - down;
*       ifMode    - interface mode (related only for serdes power up [powerUp==GT_TRUE])
*       speed     - port data speed (related only for serdes power up [powerUp==GT_TRUE])
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        Supposed to replace old API's:
*           cpssDxChPortInterfaceModeSet
*           cpssDxChPortSpeedSet
*           cpssDxChPortSerdesPowerStatusSet
*
*       The API rolls back a port's mode and speed to their last values
*       if they cannot be set together on the device.
*
*       Pay attention! Before configure CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E
*       MUST execute power down for port with this interface.
*
*       Pay attention! Unlike other interfaces which are ready to forward traffic
*       after this API pass, interlaken interfaces require call afterwards
*       cpssDxChPortIlknChannelSpeedSet to configure channel.
*
*******************************************************************************/
GT_STATUS cpssDxChPortModeSpeedSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  GT_BOOL                         powerUp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortModeSpeedSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portsBmp, powerUp, ifMode, speed));

    rc = internal_cpssDxChPortModeSpeedSet(devNum, portsBmp, powerUp, ifMode, speed);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portsBmp, powerUp, ifMode, speed));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortExtendedModeEnableSet
*
* DESCRIPTION:
*       Define which GE and XG MAC ports 9 and 11 of every mini-GOP will use -
*       from local mini-GOP or extended from other mini-GOP
*
* APPLICABLE DEVICES:
*       xCat3; Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (not CPU port)
*                  (APPLICABLE RANGES: Lion2: 9,11; xCat3: 25,27)
*       enable   - extended mode:
*                       GT_TRUE - use extended MAC;
*                       GT_FALSE - use local MAC;
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For now if application interested to implement GE and XG modes of ports 9
*       and 11 of every mini-GOP over extended MAC's, it can call this function
*       at init stage once for port 9 and once for port 11 and enough
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortExtendedModeEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS               rc;         /* return code */
    GT_U32                  localPort;  /* number of port in GOP */
    GT_U32                  portGroupId;/* iterator of port groups */
    GT_U32                  regAddr;    /* address of register */
    GT_U32                  fieldData;  /* data to write to register field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_XCAT2_E
                                            | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        if((localPort != 25) && (localPort != 27))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        CPSS_LOG_INFORMATION_MAC("Calling: hwsPortExtendedModeCfg(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], extendedMode[%d])", devNum, 0, portNum, _10GBase_KR/*don't care for XCATC3*/, enable);
        rc = hwsPortExtendedModeCfg(devNum, 0, portNum, _10GBase_KR/*don't care for XCATC3*/, enable);
        if(GT_OK != rc)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChXcat3PortMacRegAddrSwap(devNum, portNum, enable);
        if(GT_OK != rc)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portNum);

        if((localPort != 9) && (localPort != 11))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(enable)
        {
            switch(PRV_CPSS_PP_MAC(devNum)->devType)
            {
                case CPSS_LION_2_3_THREE_MINI_GOPS_DEVICES_CASES_MAC:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                default:
                    break;
            }
        }

        fieldData = BOOL2BIT_MAC(enable);

        /* use extended or regular MAC for RX DMA */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.rxdmaIfConfig[localPort/8];
        rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                             (localPort%8)*4+3, 1, fieldData);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        /* use extended or regular MAC for TX DMA */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaExtendedPortsConfig;
        rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                             ((9 == localPort) ? 0 : 1), 1, fieldData);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortExtendedModeEnableSet
*
* DESCRIPTION:
*       Define which GE and XG MAC ports 9 and 11 of every mini-GOP will use -
*       from local mini-GOP or extended from other mini-GOP
*
* APPLICABLE DEVICES:
*       xCat3; Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (not CPU port)
*                  (APPLICABLE RANGES: Lion2: 9,11; xCat3: 25,27)
*       enable   - extended mode:
*                       GT_TRUE - use extended MAC;
*                       GT_FALSE - use local MAC;
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For now if application interested to implement GE and XG modes of ports 9
*       and 11 of every mini-GOP over extended MAC's, it can call this function
*       at init stage once for port 9 and once for port 11 and enough
*******************************************************************************/
GT_STATUS cpssDxChPortExtendedModeEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortExtendedModeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortExtendedModeEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortExtendedModeEnableGet
*
* DESCRIPTION:
*       Read which GE and XG MAC ports 9 and 11 of every mini-GOP will use -
*           from local mini-GOP or extended from other mini-GOP
*
* APPLICABLE DEVICES:
*       xCat3; Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (not CPU port)
*
* OUTPUTS:
*       enablePtr - extended mode:
*                       GT_TRUE - use extended MAC;
*                       GT_FALSE - use local MAC;
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - enablePtr is NULL
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortExtendedModeEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      localPort;  /* number of port in GOP */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_XCAT2_E
                                            | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        if((localPort != 25) && (localPort != 27))
        {
            *enablePtr = GT_FALSE;
            return GT_OK;
        }

        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortExtendedModeCfgGet(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], *extendedMode)", devNum, 0, localPort, _10GBase_KR);
        rc = mvHwsPortExtendedModeCfgGet(devNum, 0, localPort, _10GBase_KR /*don't care for XCAT3*/,enablePtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
    }
    else
    {
            GT_U32      portGroupId;/* core number */
            GT_U32      regAddr;    /* register address */

        if((localPort != 9) && (localPort != 11))
        {
                    *enablePtr = GT_FALSE;
            return GT_OK;
        }

        switch(PRV_CPSS_PP_MAC(devNum)->devType)
        {
            case CPSS_LION_2_3_THREE_MINI_GOPS_DEVICES_CASES_MAC:
                *enablePtr = GT_FALSE;
                return GT_OK;
            default:
                break;
        }

            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaExtendedPortsConfig;
            rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                         ((9 == localPort) ? 0 : 1), 1, (GT_U32*)enablePtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortExtendedModeEnableGet
*
* DESCRIPTION:
*       Read which GE and XG MAC ports 9 and 11 of every mini-GOP will use -
*           from local mini-GOP or extended from other mini-GOP
*
* APPLICABLE DEVICES:
*       xCat3; Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (not CPU port)
*
* OUTPUTS:
*       enablePtr - extended mode:
*                       GT_TRUE - use extended MAC;
*                       GT_FALSE - use local MAC;
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - enablePtr is NULL
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortExtendedModeEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortExtendedModeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortExtendedModeEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortInterfaceSpeedSupportLion2PlusGet
*
* DESCRIPTION:
*       Check if given pair ifMode and speed supported by given port on
*        given device
*
* APPLICABLE DEVICES:
*        xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - port MAC number (not CPU port)
*       ifMode    - interface mode
*       speed     - port data speed
*
* OUTPUTS:
*       supportedPtr ? GT_TRUE ? (ifMode; speed) supported
*                      GT_FALSE ? (ifMode; speed) not supported
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - if one of input parameters wrong
*       GT_BAD_PTR        - if supportedPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortInterfaceSpeedSupportLion2PlusGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          portMacNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    OUT GT_BOOL                         *supportedPtr
)
{
    GT_STATUS            rc;            /* return code */
    GT_PHYSICAL_PORT_NUM localPort;     /* number of port in port group */
    MV_HWS_PORT_STANDARD portMode;      /* port i/f mode and speed translated to BlackBox enum */
    GT_U32               macNum,        /* number of MAC used by port for given ifMode */
                         pcsNum,        /* number of PCS used by port for given ifMode */
                         sdVecSize;     /* size of serdes redundancy array */
    GT_U32               *sdVectorPtr;  /* serdes redundancy array */
    GT_U32               portGroup;     /* local core number */

    if((PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_LION2_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    *supportedPtr = GT_FALSE;

    switch(ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
            if((!PRV_CPSS_SIP_5_CHECK_MAC(devNum)) &&
              (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
                return GT_OK;
            }

            switch(speed)
            {
                case CPSS_PORT_SPEED_10_E:
                case CPSS_PORT_SPEED_100_E:
                case CPSS_PORT_SPEED_1000_E:
                    portMode = QSGMII;
                    break;
                default:
                    return GT_OK;
            }
            break;

        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_10_E:
                case CPSS_PORT_SPEED_100_E:
                case CPSS_PORT_SPEED_1000_E:
                    portMode = SGMII;
                    break;
                case CPSS_PORT_SPEED_2500_E:
                    portMode = SGMII2_5;
                    break;
                default:
                    return GT_OK;
            }
            break;

        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
            if(CPSS_PORT_SPEED_10000_E == speed)
                portMode = _10GBase_KX4;
            else if(CPSS_PORT_SPEED_16000_E == speed)
            {
                if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E)
                    return GT_OK;
                else
                    portMode = HGS4;
            }
            else if(CPSS_PORT_SPEED_20000_E == speed)
                portMode = _20GBase_KX4;
            else
                return GT_OK;
            break;

        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            if(CPSS_PORT_SPEED_1000_E == speed)
                portMode = _1000Base_X;
            else
                return GT_OK;
            break;

        case CPSS_PORT_INTERFACE_MODE_HX_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_10000_E:
                    portMode = _10GBase_KX2;
                    break;
                case CPSS_PORT_SPEED_5000_E:
                    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
                    {
                        return GT_OK;
                    }
                    portMode = _5GBase_HX;
                    break;
                default:
                    return GT_OK;
            }
            break;

        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            if(CPSS_PORT_SPEED_10000_E == speed)
                portMode = RXAUI;
            else
                return GT_OK;
            break;

        case CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E:
            /* loopback port implemented over SR_LR */
        case CPSS_PORT_INTERFACE_MODE_KR_E:
        case CPSS_PORT_INTERFACE_MODE_SR_LR_E:
            /* KR and SR_LR support same speeds range */
            switch(speed)
            {
                case CPSS_PORT_SPEED_1000_E:
                    portMode = _1000Base_X;
                    break;
                case CPSS_PORT_SPEED_5000_E:
                    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
                       (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode))
                    {
                        portMode = _5GBaseR;
                    }
                    else
                    {
                        return GT_OK;
                    }
                    break;
                case CPSS_PORT_SPEED_10000_E:
                    portMode = _10GBase_KR;
                    break;
                case CPSS_PORT_SPEED_12000_E:
                    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                    {
                        portMode = _12_5GBase_KR;
                    }
                    else
                    {
                        return GT_OK;
                    }
                    break;
                case CPSS_PORT_SPEED_20000_E:
                    portMode = _20GBase_KR;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    if(CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
                    {/* as we see just 10G and 20G make sense to use for
                        loopback ports */
                        return GT_OK;
                    }
                    portMode = _40GBase_KR;
                    break;
                default:
                    return GT_OK;
            }
            break;

        case CPSS_PORT_INTERFACE_MODE_HGL_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_15000_E:
                    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E)
                        return GT_OK;
                    portMode = HGL;
                    break;
                case CPSS_PORT_SPEED_16000_E:
                    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E)
                        return GT_OK;
                    portMode = HGL16G;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    portMode = XLHGL_KR4;
                    break;
                default:
                    return GT_OK;
            }
            break;

        case CPSS_PORT_INTERFACE_MODE_ILKN4_E:
            if(!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                return GT_OK;

            switch(speed)
            {
                case CPSS_PORT_SPEED_20000_E:
                    portMode = INTLKN_4Lanes_6_25G;
                    break;
                default:
                    return GT_OK;
            }
            break;

        case CPSS_PORT_INTERFACE_MODE_ILKN8_E:
            if(!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                return GT_OK;

            switch(speed)
            {
                case CPSS_PORT_SPEED_20000_E:
                    portMode = INTLKN_8Lanes_3_125G;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    portMode = INTLKN_8Lanes_6_25G;
                    break;
                default:
                    return GT_OK;
            }
            break;


        case CPSS_PORT_INTERFACE_MODE_QX_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
                return GT_OK;

            switch(speed)
            {
                case CPSS_PORT_SPEED_2500_E:
                    portMode = _2_5GBase_QX;
                    break;
                case CPSS_PORT_SPEED_5000_E:
                    portMode = _5GBase_DQX;
                    break;
                default:
                    return GT_OK;
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_XHGS_SR_E:
            if(PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
            {
                *supportedPtr = GT_FALSE;
                return GT_OK;
            }
        case CPSS_PORT_INTERFACE_MODE_XHGS_E:
            switch (speed)
            {
                case CPSS_PORT_SPEED_11800_E:
                    portMode = (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode) ?
                                                               _12GBaseR : _12GBase_SR;
                    break;
                case CPSS_PORT_SPEED_47200_E:
                    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
                    {
                        *supportedPtr = GT_FALSE;
                        return GT_OK;
                    }
                    portMode = (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode) ?
                                                               _48GBaseR : _48GBase_SR;
                    break;
                default:
                {
                    *supportedPtr = GT_FALSE;
                    return GT_OK;
                }
            }
            break;
        default:
            return GT_OK;
    }

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portMacNum);

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    if(   (!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
       && (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
       && (localPort >= 12))
    {/* to avoid coverity's OVERRUN_STATIC */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    rc = hwsPortsParamsCfgGet(devNum, portGroup,
                              localPort, portMode, &macNum, &pcsNum,
                              &sdVecSize, &sdVectorPtr);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }
    if(NA_NUM == macNum)
    {/* i.e. current port can't support required ifMode */
        return GT_OK;
    }

    *supportedPtr = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortInterfaceSpeedSupportGet
*
* DESCRIPTION:
*       Check if given pair ifMode and speed supported by given port on
*        given device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (not CPU port)
*       ifMode    - interface mode
*       speed     - port data speed
*
* OUTPUTS:
*       supportedPtr ? GT_TRUE ? (ifMode; speed) supported
*                      GT_FALSE ? (ifMode; speed) not supported
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - if one of input parameters wrong
*       GT_BAD_PTR        - if supportedPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInterfaceSpeedSupportGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    OUT GT_BOOL                         *supportedPtr
)
{
    GT_STATUS rc;
    GT_PHYSICAL_PORT_NUM localPort; /* number of port in port group */
    GT_U32  portMacNum;      /* MAC number */
    GT_BOOL ifIsSupported;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(supportedPtr);

    if((ifMode >= CPSS_PORT_INTERFACE_MODE_NA_E)
       || (speed >= CPSS_PORT_SPEED_NA_E)
       || (ifMode < CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E)
       || (speed < CPSS_PORT_SPEED_10_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *supportedPtr = GT_FALSE;

    /* check that this type of port supports given interface */
    /*
    //if(!supportedPortsModes[PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum)][ifMode])
    //{
    //    return GT_OK;
    //}
    */

    rc = prvCpssDxChPortSupportedModeCheck(devNum,portNum,ifMode,/*OUT*/&ifIsSupported);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (ifIsSupported == GT_FALSE)
    {
        return GT_OK;
    }


    if((!PRV_CPSS_SIP_5_CHECK_MAC(devNum)) &&
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {/* check that interface supported by this device family */

        CPSS_PP_FAMILY_TYPE_ENT     devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;
        PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN ifFunsPtr = prvCpssDxChFillPortIfFunctionsSetFind(devFamily,ifMode);
        if (ifFunsPtr == NULL)
        {
            return GT_OK;
        }

    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_LION2_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        return prvCpssDxChPortInterfaceSpeedSupportLion2PlusGet(devNum, portMacNum,
                                                                ifMode, speed,
                                                                supportedPtr);
    }
    else
    {
        if (PRV_CPSS_PORT_FE_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum))
        {
            if(speed > CPSS_PORT_SPEED_100_E)
                return GT_OK;

            if((CPSS_PORT_INTERFACE_MODE_SGMII_E == ifMode) ||
               (CPSS_PORT_INTERFACE_MODE_1000BASE_X_E == ifMode))
            {/* portTypeOptions is not enough for FE ports */
                return GT_OK;
            }
        }
        else
        {
            if(CPSS_DXCH_PORT_SERDES_SPEED_NA_E == serdesFrequency[ifMode][speed])
                return GT_OK;
        }

        if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portMacNum);
            if(((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (localPort%2 != 0)) ||
                ((ifMode == CPSS_PORT_INTERFACE_MODE_HX_E) && (localPort%2 != 0)) ||
                ((ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E) && (localPort != 10)))
            {/* not supported */
                return GT_OK;
            }

            switch(speed)
            {
                case CPSS_PORT_SPEED_12000_E:
                case CPSS_PORT_SPEED_13600_E:
                    return GT_OK; /* not supported */
                default:
                    break;
            }
        }

        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
        {
            switch(speed)
            {
                case CPSS_PORT_SPEED_20000_E:
                case CPSS_PORT_SPEED_40000_E:
                    return GT_OK;
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
                    return GT_OK;
            }
            if ((GT_TRUE == PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portMacNum)) &&
                (CPSS_PORT_INTERFACE_MODE_QSGMII_E == ifMode))
            {
                return GT_OK;
            }
        }

        *supportedPtr = GT_TRUE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInterfaceSpeedSupportGet
*
* DESCRIPTION:
*       Check if given pair ifMode and speed supported by given port on
*        given device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (not CPU port)
*       ifMode    - interface mode
*       speed     - port data speed
*
* OUTPUTS:
*       supportedPtr ? GT_TRUE ? (ifMode; speed) supported
*                      GT_FALSE ? (ifMode; speed) not supported
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - if one of input parameters wrong
*       GT_BAD_PTR        - if supportedPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterfaceSpeedSupportGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    OUT GT_BOOL                         *supportedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInterfaceSpeedSupportGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ifMode, speed, supportedPtr));

    rc = internal_cpssDxChPortInterfaceSpeedSupportGet(devNum, portNum, ifMode, speed, supportedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ifMode, speed, supportedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssLion2CpssIfModeToHwsTranslate
*
* DESCRIPTION:
*       Translate port interface mode and speed from CPSS enum to hwService enum
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       cpssIfMode - i/f mode in CPSS format
*       cpssSpeed  - port speed in CPSS format
*
* OUTPUTS:
*       hwsIfModePtr - port i/f mode in format of hwServices library
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_SUPPORTED         - i/f mode/speed pair not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssLion2CpssIfModeToHwsTranslate
(
    IN  CPSS_PORT_INTERFACE_MODE_ENT    cpssIfMode,
    IN  CPSS_PORT_SPEED_ENT             cpssSpeed,
    OUT MV_HWS_PORT_STANDARD            *hwsIfModePtr
)
{
    *hwsIfModePtr = NON_SUP_MODE;

    switch(cpssIfMode)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            *hwsIfModePtr = (CPSS_PORT_SPEED_2500_E == cpssSpeed) ?
                                SGMII2_5 : SGMII;
            break;
        case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
            *hwsIfModePtr = QSGMII;
            break;
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
            switch(cpssSpeed)
            {
                case CPSS_PORT_SPEED_10000_E:
                    *hwsIfModePtr = _10GBase_KX4;
                    break;
                case CPSS_PORT_SPEED_20000_E:
                    *hwsIfModePtr = _20GBase_KX4;
                    break;
                case CPSS_PORT_SPEED_12000_E:
                    *hwsIfModePtr = HGS;
                    break;
                case CPSS_PORT_SPEED_16000_E:
                    *hwsIfModePtr = HGS4;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            *hwsIfModePtr = _1000Base_X;
            break;
        case CPSS_PORT_INTERFACE_MODE_HX_E:
            switch(cpssSpeed)
            {
                case CPSS_PORT_SPEED_10000_E:
                    *hwsIfModePtr = _10GBase_KX2;
                    break;
                case CPSS_PORT_SPEED_5000_E:
                    *hwsIfModePtr = _5GBase_HX;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_QX_E:
            switch(cpssSpeed)
            {
                case CPSS_PORT_SPEED_2500_E:
                    *hwsIfModePtr = _2_5GBase_QX;
                    break;
                case CPSS_PORT_SPEED_5000_E:
                    *hwsIfModePtr = _5GBase_DQX;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            *hwsIfModePtr = RXAUI;
            break;
        case CPSS_PORT_INTERFACE_MODE_KR_E:
            switch(cpssSpeed)
            {
                case CPSS_PORT_SPEED_10000_E:
                    *hwsIfModePtr = _10GBase_KR;
                    break;
                case CPSS_PORT_SPEED_12000_E:
                    *hwsIfModePtr = _12_5GBase_KR;
                    break;
                case CPSS_PORT_SPEED_20000_E:
                    *hwsIfModePtr = _20GBase_KR;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    *hwsIfModePtr = _40GBase_KR;
                    break;
                case CPSS_PORT_SPEED_100G_E:
                    *hwsIfModePtr = _100GBase_KR10;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_HGL_E:
            switch(cpssSpeed)
            {
                case CPSS_PORT_SPEED_15000_E:
                    *hwsIfModePtr = HGL;
                    break;
                case CPSS_PORT_SPEED_16000_E:
                    *hwsIfModePtr = HGL16G;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    *hwsIfModePtr = XLHGL_KR4;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_CHGL_12_E:
            *hwsIfModePtr = CHGL;
            break;
        case CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E:
        case CPSS_PORT_INTERFACE_MODE_SR_LR_E:
            switch(cpssSpeed)
            {
                case CPSS_PORT_SPEED_1000_E:
                    *hwsIfModePtr = _1000Base_X;
                    break;

                case CPSS_PORT_SPEED_5000_E:
                    *hwsIfModePtr = _5GBaseR;
                    break;

                case CPSS_PORT_SPEED_10000_E:
                    *hwsIfModePtr = _10GBase_SR_LR;
                    break;

                case CPSS_PORT_SPEED_20000_E:
                    *hwsIfModePtr = _20GBase_SR_LR;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    *hwsIfModePtr = _40GBase_SR_LR;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_ILKN4_E:
            switch(cpssSpeed)
            {
                case CPSS_PORT_SPEED_20000_E:
                    *hwsIfModePtr = INTLKN_4Lanes_6_25G;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_ILKN8_E:
            switch(cpssSpeed)
            {
                case CPSS_PORT_SPEED_20000_E:
                    *hwsIfModePtr = INTLKN_8Lanes_3_125G;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    *hwsIfModePtr = INTLKN_8Lanes_6_25G;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_XHGS_E:
        case CPSS_PORT_INTERFACE_MODE_XHGS_SR_E:
            switch (cpssSpeed)
            {
                case CPSS_PORT_SPEED_11800_E:
                    *hwsIfModePtr = (CPSS_PORT_INTERFACE_MODE_XHGS_E == cpssIfMode) ?
                                                               _12GBaseR : _12GBase_SR;
                    break;

                case CPSS_PORT_SPEED_47200_E:
                    *hwsIfModePtr = (CPSS_PORT_INTERFACE_MODE_XHGS_E == cpssIfMode) ?
                                                               _48GBaseR : _48GBase_SR;
                    break;

                default:
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
                }
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortLion2InterfaceModeHwGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port from HW.
*
* APPLICABLE DEVICES:
*        Lion2; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*
* COMMENTS:
*       Because of specific of QSGMII interface in scenario when application configured
*       just one port of quadruplet before SW reset, after reset catchup will
*       return all four ports as configured to QSGMII - nothing to do with that,
*       because force_link_down, that CPSS uses as WA to cover fact that
*       all port of quadruplet configured at once, can't serve sign for ports not
*       configured by application before reset, because could be application
*       set it to force_link_down.
*******************************************************************************/
GT_STATUS prvCpssDxChPortLion2InterfaceModeHwGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U32 localPort;   /* number of port in local core */
    GT_STATUS   rc;     /* return code */
    MV_HWS_PORT_STANDARD    portMode = NON_SUP_MODE; /* port mode in HWS format */
    GT_U32  portMacMap;

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacMap);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacMap);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacMap);

    if(PRV_CPSS_GE_PORT_GE_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacMap))
    {
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortInterfaceGet(devNum[%d], portGroup[%d], phyPortNum[%d], *portModePtr)", devNum, portGroupId, localPort);
        rc = mvHwsPortInterfaceGet(devNum, portGroupId, localPort&0xFFFFFFFC, &portMode);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
        if (QSGMII == portMode)
        {
            goto hws2cpss;
        }
    }

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortInterfaceGet(devNum[%d], portGroup[%d], phyPortNum[%d], *portModePtr)", devNum, portGroupId, localPort);
    rc = mvHwsPortInterfaceGet(devNum, portGroupId, localPort, &portMode);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
    }

hws2cpss:
    switch(portMode)
    {
        case _100Base_FX:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_100BASE_FX_E;
            break;

        case SGMII:
        case SGMII2_5:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_SGMII_E;
            break;

        case _1000Base_X:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
            break;

        case QSGMII:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_QSGMII_E;
            break;

        case _10GBase_KX4:
        case _20GBase_KX4:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_XGMII_E;
            break;

        case _10GBase_KX2:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_HX_E;
            break;

        case _10GBase_KR:
        case _20GBase_KR:
        case _40GBase_KR:
        case _100GBase_KR10:
        case _12_5GBase_KR:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_KR_E;
            break;

        case HGL:
        case HGL16G:
        case XLHGL_KR4:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_HGL_E;
            break;

        case RXAUI:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_RXAUI_E;
            break;

        case _10GBase_SR_LR:
        case _20GBase_SR_LR:
        case _40GBase_SR_LR:
        case _100GBase_SR10:
        case _5GBaseR:

            *ifModePtr = CPSS_PORT_INTERFACE_MODE_SR_LR_E;
CPSS_TBD_BOOKMARK_LION2
/* need to add support for CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E; */
            break;

        case _12GBaseR:
        case _48GBaseR:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_XHGS_E;
            break;
        case _12GBase_SR:
        case _48GBase_SR:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_XHGS_SR_E;
            break;
        case CHGL:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_CHGL_12_E;
            break;

        case INTLKN_12Lanes_6_25G:
        case INTLKN_12Lanes_10_3125G:
        case INTLKN_12Lanes_12_5G:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_ILKN12_E;
            break;

        case INTLKN_16Lanes_6_25G:
        case INTLKN_16Lanes_10_3125G:
        case INTLKN_16Lanes_12_5G:
        case INTLKN_16Lanes_3_125G:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_ILKN16_E;
            break;

        case INTLKN_24Lanes_6_25G:
        case INTLKN_24Lanes_3_125G:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_ILKN24_E;
            break;

        case INTLKN_4Lanes_3_125G:
        case INTLKN_4Lanes_6_25G:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_ILKN4_E;
            break;

        case INTLKN_8Lanes_3_125G:
        case INTLKN_8Lanes_6_25G:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_ILKN8_E;
            break;

        default:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortLionInterfaceModeHwGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port from HW.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortLionInterfaceModeHwGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
#define PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_GE_CNS  0
#define PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_XG_CNS  1
#define PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_XLG_CNS 3
#define PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_HGL_CNS 7

    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* register value*/
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U32 localPort;   /* number of port in local core */
    GT_BOOL portMacResetState; /* is MAC of port in reset state */
    GT_STATUS   rc;     /* return code */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort == 8 || localPort == 9 || localPort == 11)
    {/* check if port 10 configured to 40G */
        GT_U32    tempPort;

        tempPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,10);
        /* Get MAC type in MacModeSelect */
        /* MAC Mode is selected in XG MAC */
        PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,tempPort,PRV_CPSS_PORT_XG_E,&regAddr);
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 13, 3, &value) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        if(PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_XLG_CNS == value)
        {
            return GT_OK;
        }
    }

    /* Get MAC type in MacModeSelect */
    /* MAC Mode is selected in XG MAC */
    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 13, 3, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    switch(value)
    {
        case PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_GE_CNS:
            /* get PortType from Port MAC Control Register0*/
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_PORT_GE_E,&regAddr);
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr,
                                                   1, 1, &value) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }

            *ifModePtr = (value == 0) ? CPSS_PORT_INTERFACE_MODE_SGMII_E :
                                            CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
            break;

        case PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_XG_CNS:

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                    localPort*2, 1, &value) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }

            if(1 == value)
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_RXAUI_E;
            }
            else
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_XGMII_E;
                if (localPort % 2 == 0)
                {
                    /* Get LaneActive in Global Configuration0 register*/
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].xgGlobalConfReg0;
                    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
                            portGroupId, regAddr, 5, 2, &value) != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                    }
                    if(1 == value)
                    {
                        *ifModePtr = CPSS_PORT_INTERFACE_MODE_HX_E;
                    }
                }
            }

            break;

        case PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_XLG_CNS:
            if(localPort != 10)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
            else
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_XLG_E;
            }
            break;

        case PRV_CPSS_DXCH_PORT_MAC_MODE_SELECT_HGL_CNS:
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_HGL_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* update portType on base of hw value of ifMode, must do it here,
       because other functions use SW DB, value configured here based on
       ifMode that could be not in use is still OK */
    prvCpssDxChPortTypeSet(devNum, portNum, *ifModePtr, CPSS_PORT_SPEED_NA_E);

    rc = prvCpssDxChPortMacResetStateGet(devNum, portNum, &portMacResetState);
    if(rc != GT_OK)
    {
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
        return rc;
    }

    if(GT_TRUE == portMacResetState)
    {/* if calculated MAC in reset then port interface actually not defined */
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
        return GT_OK;
    }

    /* check if odd port is in local_xgmii mode, in theory even port also
       can be in local_xgmii, but it could not be distinguished */
    if((localPort%2 != 0) && (CPSS_PORT_INTERFACE_MODE_XGMII_E == *ifModePtr))
    {
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E;
    }

    return GT_OK;
}


/******************************************************************************
* internal_cpssDxChPortFecModeSet
*
* DESCRIPTION:
*       Configure Forward error correction (FEC) mode of port
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - Forward error correction mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_NOT_INITIALIZED    - if port not configured
*     GT_NOT_SUPPORTED - if FEC not supported on interface mode defined now on port
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortFecModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_DXCH_PORT_FEC_MODE_ENT mode
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      localPort;  /* number of port in GOP */
    GT_U32      portGroupId;/* iterator of port groups */
    GT_BOOL     portFecEn;  /* FEC state on port */
    GT_U32      portMacNum; /* MAC number */
    MV_HWS_PORT_STANDARD portMode;  /* port i/f mode and speed translated to
                                        BlackBox enum */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    switch(mode)
    {
        case CPSS_DXCH_PORT_FEC_MODE_ENABLED_E:
            portFecEn = GT_TRUE;
            break;

        case CPSS_DXCH_PORT_FEC_MODE_DISABLED_E:
            portFecEn = GT_FALSE;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portMacNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portMacNum);

    if((CPSS_PORT_INTERFACE_MODE_NA_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portMacNum))
       || (CPSS_PORT_SPEED_NA_E == PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,portMacNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssLion2CpssIfModeToHwsTranslate(
                                    PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portMacNum),
                                    PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,portMacNum),
                                    &portMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(portMode)
    {
        case _10GBase_KR:
        case _20GBase_KR:
        case _40GBase_KR:
            break;
        default:
            if(GT_TRUE == portFecEn)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            else
            {
                GT_U32      macNum;     /* number of used MAC unit */
                GT_U32      pcsNum;     /* number of used PCS unit */
                GT_U32      sdVecSize;    /* size of serdes redundancy array */
                GT_U32      *sdVectorPtr; /* serdes redundancy array */

                CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%d], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, portMode);
                rc = hwsPortsParamsCfgGet(devNum, 0,
                                          localPort, portMode, &macNum, &pcsNum,
                                          &sdVecSize, &sdVectorPtr);
                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }

                switch(sdVecSize)
                {
                    case 1:
                        portMode = _10GBase_KR;
                        break;
                    case 2:
                        portMode = _20GBase_KR;
                        break;
                    default:
                        portMode = _40GBase_KR;
                        break;
                }
            }
            break;
    }

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortFecCofig(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], portFecEn[%d])", devNum, portGroupId, localPort, portMode, portFecEn);
    rc = mvHwsPortFecCofig(devNum, portGroupId, localPort, portMode, portFecEn);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
    }

    return rc;
}

/******************************************************************************
* cpssDxChPortFecModeSet
*
* DESCRIPTION:
*       Configure Forward error correction (FEC) mode of port
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - Forward error correction mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_NOT_INITIALIZED    - if port not configured
*     GT_NOT_SUPPORTED - if FEC not supported on interface mode defined now on port
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS cpssDxChPortFecModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_DXCH_PORT_FEC_MODE_ENT mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFecModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode));

    rc = internal_cpssDxChPortFecModeSet(devNum, portNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChPortFecModeGet
*
* DESCRIPTION:
*       Read current Forward error correction (FEC) mode of port
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       modePtr   - current Forward error correction mode
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_NOT_INITIALIZED    - if port not configured
*     GT_BAD_PTR       - modePtr == NULL
*     GT_NOT_SUPPORTED - if FEC not supported on interface mode defined now on port
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortFecModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_FEC_MODE_ENT *modePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      localPort;  /* number of port in GOP */
    GT_U32      portGroupId;/* iterator of port groups */
    GT_BOOL     portFecEn;  /* FEC state on port */
    GT_U32      portMacNum; /* MAC number */
    MV_HWS_PORT_STANDARD portMode;  /* port i/f mode and speed translated to
                                        BlackBox enum */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    rc = prvCpssLion2CpssIfModeToHwsTranslate(
                                    PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portMacNum),
                                    PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,portMacNum),
                                    &portMode);
    if(rc != GT_OK)
    {
        *modePtr = CPSS_DXCH_PORT_FEC_MODE_DISABLED_E;
        return GT_OK;
    }

    switch(portMode)
    {
        case _10GBase_KR:
        case _20GBase_KR:
        case _40GBase_KR:
            break;
        default:
            *modePtr = CPSS_DXCH_PORT_FEC_MODE_DISABLED_E;
            return GT_OK;
    }

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portMacNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portMacNum);

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortFecCofigGet(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], *portFecEn)", devNum, portGroupId, localPort, portMode);
    rc = mvHwsPortFecCofigGet(devNum, portGroupId, localPort, portMode, &portFecEn);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
    }

    switch(portFecEn)
    {
        case GT_TRUE:
            *modePtr = CPSS_DXCH_PORT_FEC_MODE_ENABLED_E;
            break;

        case GT_FALSE:
            *modePtr = CPSS_DXCH_PORT_FEC_MODE_DISABLED_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/******************************************************************************
* cpssDxChPortFecModeGet
*
* DESCRIPTION:
*       Read current Forward error correction (FEC) mode of port
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       modePtr   - current Forward error correction mode
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_NOT_INITIALIZED    - if port not configured
*     GT_BAD_PTR       - modePtr == NULL
*     GT_NOT_SUPPORTED - if FEC not supported on interface mode defined now on port
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS cpssDxChPortFecModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_FEC_MODE_ENT *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFecModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modePtr));

    rc = internal_cpssDxChPortFecModeGet(devNum, portNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* prvCpssDxChPortInterfaceModeHwGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port from HW.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2; Lion2; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortInterfaceModeHwGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_U32      portMacNum; /* MAC number */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                        | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(ifModePtr);

    *ifModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;

    if(CPSS_CPU_PORT_NUM_CNS == portMacNum)
    {
        return cpuPortIfModeGet(devNum,portMacNum,ifModePtr);
    }

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            rc = xcatPortIfModeHwGet(devNum, portNum, ifModePtr);
            break;

        case CPSS_PP_FAMILY_DXCH_LION_E:
            rc = prvCpssDxChPortLionInterfaceModeHwGet(devNum,portNum,ifModePtr);
            break;

        case CPSS_PP_FAMILY_DXCH_LION2_E:
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            rc = prvCpssDxChPortLion2InterfaceModeHwGet(devNum,portNum,ifModePtr);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return rc;
}

GT_STATUS prvCpssDxChPortInterruptDisambiguation
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *extendedModePtr
)
{
    GT_STATUS   rc;
    GT_U32      localPort;

    *extendedModePtr = GT_FALSE;
    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        return GT_OK;
    }

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
    if((localPort != 9) && (localPort != 11))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    rc = cpssDxChPortExtendedModeEnableGet(devNum, portNum,
                                           extendedModePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortXcat3PortDelete
*
* DESCRIPTION:
*       Power down port and free all allocated for it resources.
*
* APPLICABLE DEVICES:
*       xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - interface mode according to which power down port
*       speed     - port data speed according to which power down port
*       qsgmiiFullDelete    - GT_TRUE - power down QSGMII interface
*                             GT_FALSE - just free resources of this specific
*                                       member of QSGMII quadruplet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortXcat3PortDelete
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_BOOL                         qsgmiiFullDelete
)
{
    GT_STATUS               rc;         /* return code */
    CPSS_PORT_INTERFACE_MODE_ENT *portIfModePtr;/* interface configured on port now */
    CPSS_PORT_SPEED_ENT          *portSpeedPtr; /* speed configured on port now */

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    portIfModePtr = &(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum));


    /* if on port was configured interface - reset physical layer and free resources */
    if(*portIfModePtr != CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        portSpeedPtr = &(PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portNum));
        rc = prvCpssDxChPortBcat2PortReset(devNum, portNum, *portIfModePtr,
                                           *portSpeedPtr, qsgmiiFullDelete);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        CPSS_TBD_BOOKMARK_XCAT3

/*      should be checked later

        rc = prvCpssDxChPortXcat3ResourcesConfig(devNum, portNum, ifMode,
                                                 speed, GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
*/

        /* reset port according to new interface */
        rc = prvCpssDxChPortBcat2PortReset(devNum, portNum, ifMode, speed,
                                           qsgmiiFullDelete);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortXcat3FcaBusWidthSet
*
* DESCRIPTION:
*       FCA bus width configuration.
*
* APPLICABLE DEVICES:
*       xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       speed   - port data speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported speed for given port
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortXcat3FcaBusWidthSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS rc; /* return code */
    GT_U32 regAddr; /* register address */
    GT_U32 regValue; /* register value */
    GT_U32 fieldOffset; /* register value */
    GT_U32 fieldLength; /* register value */
    PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

    fcaStcPtr = (portNum == CPSS_CPU_PORT_NUM_CNS) ?
         &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
         &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portNum);

    regAddr = fcaStcPtr->FCACtrl;

    fieldOffset = 4;
    fieldLength = 3;

    switch(speed)
    {
        case CPSS_PORT_SPEED_10_E:
        case CPSS_PORT_SPEED_100_E:
        case CPSS_PORT_SPEED_1000_E:
            regValue = 0; /* width 8bit */

            /*  On the stacking interfaces regardless of the port
                speed (1G or 10G) the bus width is ALWAYS 64 bits.
                Thus, you should set it to TypeB (0x1).*/
            if((portNum >= 24) && (portNum <= 27))
            {
                regValue = 1; /* width 64bit */
            }
            break;

        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_11800_E:
        case CPSS_PORT_SPEED_2500_E:
        case CPSS_PORT_SPEED_5000_E:
        case CPSS_PORT_SPEED_20000_E:
            regValue = 1; /* width 64bit */
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc =  prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, regValue);
    return rc;
}


/*******************************************************************************
* prvCpssDxChPortXcat3ModeSpeedSet
*
* DESCRIPTION:
*       Configure Interface mode and speed on a specified port and execute
*        on port's serdeses power up sequence; or configure power down on port's
*        serdeses.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number (or CPU port)
*       powerUp   - serdes power:
*                       GT_TRUE - up;
*                       GT_FALSE - down;
*       ifMode    - interface mode (related only for serdes power up [powerUp==GT_TRUE])
*       speed     - port data speed (related only for serdes power up [powerUp==GT_TRUE])
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortXcat3ModeSpeedSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  GT_BOOL                         powerUp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS               rc;         /* return code */
    GT_PHYSICAL_PORT_NUM    portNum;    /* iterator */
    GT_BOOL                 supported;  /* is ifMode/speed supported on port */
    MV_HWS_PORT_STANDARD    portMode;
    GT_BOOL                 lbPort;
    MV_HWS_REF_CLOCK_SUP_VAL refClock;
    MV_HWS_REF_CLOCK_SOURCE  refClockSource;
    MV_HWS_PORT_INIT_FLAVOR_CFG  portFlavorParams;
    CPSS_PORT_INTERFACE_MODE_ENT *portIfModePtr;
    CPSS_PORT_SPEED_ENT          *portSpeedPtr;
    GT_BOOL                  fullConfig; /* because in QSGMII mode one serdes serves four ports
                                                    when one port of the quadruplete configured three
                                                    others configured too, so when application asks
                                                    to configure another port from quadruplete we don't need
                                                    to execute full configuretion of new port */
    GT_U32                   originalPortState = 1; /* GT_TRUE - port enable state
                                                            defined by application */
    GT_U32  firstInQuadruplet;  /* mac number of first port in quadruplet */
    GT_U32  regAddr;  /* register address */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddrPtr;

    if(powerUp == GT_TRUE)
    {
        rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChSerdesRefClockTranslateCpss2Hws(devNum, &refClock);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        lbPort = (CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode) ?
                                                        GT_TRUE : GT_FALSE;
    }
    else
    {/* to prevent warnings */
        portMode = NON_SUP_MODE;
        refClock = MHz_156;
        lbPort = GT_FALSE;
    }

    for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum))
        {
            PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
        }
        else
        {
            continue;
        }

        rc = prvCpssDxChPortSerdesFreeCheck(devNum,portNum,ifMode,speed);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        if ((PRV_CPSS_GE_PORT_GE_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, portNum))
                && (portNum < 24) && (CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
                && (portNum != (portNum & 0xFFFFFFFC)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        portIfModePtr = &(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum));
        portSpeedPtr = &(PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portNum));

        if(powerUp == GT_FALSE)
        {
            /* we remove the portFs, so need to use current interface and speed
             * if configured, not the values from the caller */
            if((CPSS_PORT_INTERFACE_MODE_NA_E != *portIfModePtr) &&
                                        (CPSS_PORT_SPEED_NA_E != *portSpeedPtr))
            {
                ifMode = *portIfModePtr;
                speed =  *portSpeedPtr;
            }
        }

        rc = prvCpssDxChPortInterfaceSpeedSupportLion2PlusGet(devNum, portNum,
                                                              ifMode, speed,
                                                              &supported);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        if(!supported)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChPortStateDisableSet(devNum, portNum, &originalPortState);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        fullConfig = GT_TRUE;
        if(CPSS_PORT_INTERFACE_MODE_QSGMII_E == ifMode)
        {
            rc = bcat2QsgmiiConfig(devNum, portNum, powerUp, &fullConfig);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
            }
        }

        rc = prvCpssDxChPortXcat3PortDelete(devNum, portNum, ifMode, speed,
                                            fullConfig);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        /* save new interface mode in DB */
        *portIfModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
        *portSpeedPtr = CPSS_PORT_SPEED_NA_E;

        if(!powerUp)
        {
            rc = prvCpssDxChPortStateEnableSet(devNum, portNum, originalPortState);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
            }

            continue;
        }

        CPSS_TBD_BOOKMARK_XCAT3

        /* For setting RXAUI port interface first port in quadruplet also
        should be already configurated with RXAUI port interface */
        if(CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode)
        {
            firstInQuadruplet = portNum & 0xFFFFFFFC;
            if(firstInQuadruplet != portNum)
            {
                CPSS_PORT_INTERFACE_MODE_ENT ifMode;
                rc = internal_cpssDxChPortInterfaceModeGet(devNum, firstInQuadruplet, &ifMode);
                if(rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
                }
                if(CPSS_PORT_INTERFACE_MODE_RXAUI_E != ifMode)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
        }

/*
        should be checked later

        rc = prvCpssDxChBcat2PortResourcesConfig(devNum, portNum, ifMode, speed,
                                                 GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
*/
        /* Set FCA interface width */
        rc = prvCpssDxChPortXcat3FcaBusWidthSet(devNum, portNum, speed);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        /* Set Count External FC En enable */
        if(PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portNum) == PRV_CPSS_GE_PORT_GE_ONLY_E)
        {
            regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
            regAddr = regsAddrPtr->macRegs.perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl4;
            rc =  prvCpssHwPpSetRegField(devNum, regAddr, 8, 1, 1);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
            }
        }

        if(fullConfig)
        {
            switch(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portSerdesTuningType)
            {
                case CPSS_PORT_SERDES_TUNING_NONE_E:
                    portFlavorParams.portFlavor = PORT_REGULAR_INIT;
                    break;
                case CPSS_PORT_SERDES_TUNING_RX_ONLY_E:
                    portFlavorParams.portFlavor = PORT_RX_TRAIN_INIT;
                    break;
                case CPSS_PORT_SERDES_TUNING_TRX_E:
                    portFlavorParams.portFlavor = PORT_TRX_TRAIN_INIT;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(CPSS_PORT_INTERFACE_MODE_QSGMII_E == ifMode)
            {/* provide to HWS first port in quadruplet and it will configure
                all other ports if needed */
                firstInQuadruplet = portNum & 0xFFFFFFFC;
            }
            else
            {
                firstInQuadruplet = portNum;
            }

            switch(speed)
            {
                case CPSS_PORT_SPEED_10_E:
                case CPSS_PORT_SPEED_100_E:
                case CPSS_PORT_SPEED_1000_E:
                case CPSS_PORT_SPEED_2500_E:
                    refClockSource = PRIMARY_LINE_SRC;
                    break;

                case CPSS_PORT_SPEED_5000_E:
                case CPSS_PORT_SPEED_10000_E:
                case CPSS_PORT_SPEED_11800_E:
                case CPSS_PORT_SPEED_20000_E:

                    refClockSource = SECONDARY_LINE_SRC;
                    break;

                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portRefClock.enableOverride == GT_TRUE)
            {
                switch(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portRefClock.portRefClockSource)
                {
                    case CPSS_PORT_REF_CLOCK_SOURCE_PRIMARY_E:
                        refClockSource = PRIMARY_LINE_SRC;
                        break;
                    case CPSS_PORT_REF_CLOCK_SOURCE_SECONDARY_E:
                        refClockSource = SECONDARY_LINE_SRC;
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortFlavorInit(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], lbPort[%d], refClock[%d], refClockSource[%d], portFlavorParams{portFlavor[%d]})", devNum, 0, firstInQuadruplet, portMode, lbPort, refClock, refClockSource, portFlavorParams.portFlavor);
            rc = mvHwsPortFlavorInit(devNum, 0, firstInQuadruplet, portMode, lbPort,
                                     refClock,
                                     refClockSource,
                                     portFlavorParams);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
            }
        }

        if((CPSS_PORT_SPEED_10_E == speed) || (CPSS_PORT_SPEED_100_E == speed))
        {/* HWS doesn't support 10/100M, so after 1G configured by HWS, fix speed here */
            PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&regAddr);
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, 0);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssHwPpSetRegField(devNum, regAddr, 5, 1, (CPSS_PORT_SPEED_10_E == speed) ? 0 : 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }


        prvCpssDxChPortTypeSet(devNum, portNum, ifMode, speed);

        /* update addresses of mac registers accordingly to used MAC GE/XLG/etc. */
        rc = prvCpssDxChHwXcat3RegAddrPortMacUpdate(devNum, portNum);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        /* save new interface mode in DB */
        *portIfModePtr = ifMode;
        *portSpeedPtr = speed;

        /*Creating Port LoopBack*/
        if(lbPort)
        {
            rc = prvCpssDxChPortSerdesLoopbackActivate (devNum, portNum, speed);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
            }
        }

        rc = prvCpssDxChPortStateEnableSet(devNum, portNum, originalPortState);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

    } /* for(portNum = 0; */

    return GT_OK;
}


static GT_STATUS internal_cpssDxChPortRefClockSourceOverrideEnableSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         overrideEnable,
    IN  CPSS_PORT_REF_CLOCK_SOURCE_ENT  refClockSource
)
{
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_XCAT2_E
                                            | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum, portMacNum);

    if(overrideEnable == GT_FALSE)
    {
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portRefClock.enableOverride = overrideEnable;
        /* invalidate entry */
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portRefClock.portRefClockSource = CPSS_PORT_REF_CLOCK_SOURCE_LAST_E;
    }
    else
    {
        if((refClockSource == CPSS_PORT_REF_CLOCK_SOURCE_PRIMARY_E) || (refClockSource == CPSS_PORT_REF_CLOCK_SOURCE_SECONDARY_E))
        {
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portRefClock.enableOverride = overrideEnable;
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portRefClock.portRefClockSource = refClockSource;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortRefClockSourceOverrideEnableSet
*
* DESCRIPTION:
*       Enables/disables reference clock source override for port create.
*
* APPLICABLE DEVICES:
*       xCat3; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*       overrideEnable  - override
*                           GT_TRUE - use preconfigured refClock source;
*                           GT_FALSE - use default refClock source;
*       refClockSource  - reference clock source.
*                         Not relevant when overrideEnable is false.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number, device, refClockSource
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortRefClockSourceOverrideEnableSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         overrideEnable,
    IN  CPSS_PORT_REF_CLOCK_SOURCE_ENT  refClockSource
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortRefClockSourceOverrideEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, overrideEnable, refClockSource));

    rc = internal_cpssDxChPortRefClockSourceOverrideEnableSet(devNum, portNum, overrideEnable, refClockSource);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, overrideEnable, refClockSource));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

static GT_STATUS internal_cpssDxChPortRefClockSourceOverrideEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_BOOL                         *overrideEnablePtr,
    OUT CPSS_PORT_REF_CLOCK_SOURCE_ENT  *refClockSourcePtr
)
{
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_XCAT2_E
                                            | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(overrideEnablePtr);
    CPSS_NULL_PTR_CHECK_MAC(refClockSourcePtr);

    *overrideEnablePtr = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portRefClock.enableOverride;
    *refClockSourcePtr = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portRefClock.portRefClockSource;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortRefClockSourceOverrideEnableGet
*
* DESCRIPTION:
*       Gets status of reference clock source override for port create.
*
* APPLICABLE DEVICES:
*       xCat3; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*
* OUTPUTS:
*       overrideEnablePtr  - (pointer to) override enable status
*       refClockSourcePtr  - (pointer to) reference clock source.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number, device
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortRefClockSourceOverrideEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_BOOL                         *overrideEnablePtr,
    OUT CPSS_PORT_REF_CLOCK_SOURCE_ENT  *refClockSourcePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortRefClockSourceOverrideEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, overrideEnablePtr, refClockSourcePtr));

    rc = internal_cpssDxChPortRefClockSourceOverrideEnableGet(devNum, portNum, overrideEnablePtr, refClockSourcePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, overrideEnablePtr, refClockSourcePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


#if 0
/*******************************************************************************
* prvCpssDxChPortCaelumTrafficFlushCheck
*
* DESCRIPTION:
*       Check all traffic to the port was flushed.
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       NONE.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_TIMEOUT        - when not all traffic was flushed.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortCaelumTrafficFlushCheck
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS rc;               /* return code */
    GT_U32 regAddr;             /* register address */
    GT_U32 regValue;            /* register value */
    GT_U32 regValue1;           /* register value */
    GT_U32 fieldOffset;         /* register offset value */
    GT_U32 fieldLen;            /* register length value */
    GT_U32 tryCounter;          /* check counter */

    /*
        1. Read Port<%n>DescCounter in the TXQ and wait for it to reach 0. This counter
           indicates how many descriptors are allocated for the port in the TXQ queues.
    */
    rc = prvCpssDxChPortCaelumRegAddrGet(devNum,portNum,
                                         PRV_CAELUM_REG_FIELDS_TXQ_Q_PORT_DESC_COUNTER_E,
                                         &regAddr, &fieldOffset, &fieldLen);
    if(GT_OK != rc)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue);
    if(GT_OK != rc)
    {
        return rc;
    }
    tryCounter = 100;
    while(regValue != 0)
    {
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue);
        if(GT_OK != rc)
        {
            return rc;
        }
        tryCounter--;
    }
    /* in case of error - return TIMEOUT value */
    if(tryCounter == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_TIMEOUT, LOG_ERROR_NO_MSG);
    }

    /*
        2. Read TxDMA Port <%n> Credit Counter register in the TXQ and wait for it to reach
           the corresponding configured descriptor credits threshold in the
           TXDMA (Descriptor Credits SCDMA %p).
    */
    rc = prvCpssDxChPortCaelumRegAddrGet(devNum,portNum,
                                         PRV_CAELUM_REG_FIELDS_TXDMA_DESC_CREDITS_SCDMA_E,
                                         &regAddr, &fieldOffset, &fieldLen);
    if(GT_OK != rc)
    {
        return rc;
    }
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    rc = prvCpssDxChPortCaelumRegAddrGet(devNum,portNum,
                                         PRV_CAELUM_REG_FIELDS_TXQ_DQ_TXDMA_PORT_CREDIT_COUNTER_E,
                                         &regAddr, &fieldOffset, &fieldLen);
    if(GT_OK != rc)
    {
        return rc;
    }
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue1);
    if(GT_OK != rc)
    {
        return rc;
    }

    tryCounter = 100;
    while(regValue != regValue1)
    {
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue1);
        if(GT_OK != rc)
        {
            return rc;
        }
        tryCounter--;
    }
    /* in case of error - return TIMEOUT value */
    if(tryCounter == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_TIMEOUT, LOG_ERROR_NO_MSG);
    }

    /*
        3. In the TXDMA regfile - read the TXFIFO credits counters status and wait for it
           to reach zero. There is a separate counter for headers (scdma_%p_header_txfifo_counter)
           and payloads (scdma_%p_payload_txfifo_counter). These counters count the amount of
           data that is inside the TXDMA/TXFIFO on its way to the port.
    */
    rc = prvCpssDxChPortCaelumRegAddrGet(devNum,portNum,
                                         PRV_CAELUM_REG_FIELDS_TXDMA_SCDMA_HEADER_TXFIFO_COUNTER_E,
                                         &regAddr, &fieldOffset, &fieldLen);
    if(GT_OK != rc)
    {
        return rc;
    }
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue);
    if(GT_OK != rc)
    {
        return rc;
    }
    tryCounter = 100;
    while(regValue != 0)
    {
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue);
        if(GT_OK != rc)
        {
            return rc;
        }
        tryCounter--;
    }
    /* in case of error - return TIMEOUT value */
    if(tryCounter == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_TIMEOUT, LOG_ERROR_NO_MSG);
    }


    rc = prvCpssDxChPortCaelumRegAddrGet(devNum,portNum,
                                         PRV_CAELUM_REG_FIELDS_TXDMA_SCDMA_PAYLOAD_TXFIFO_COUNTER_E,
                                         &regAddr, &fieldOffset, &fieldLen);
    if(GT_OK != rc)
    {
        return rc;
    }
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue);
    if(GT_OK != rc)
    {
        return rc;
    }
    tryCounter = 100;
    while(regValue != 0)
    {
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLen, &regValue);
        if(GT_OK != rc)
        {
            return rc;
        }
        tryCounter--;
    }
    /* in case of error - return TIMEOUT value */
    if(tryCounter == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_TIMEOUT, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}
#endif


/*******************************************************************************
* prvCpssDxChPortCaelumPortDelete
*
* DESCRIPTION:
*       Power down port and free all allocated for it resources.
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - interface mode according to which power down port
*       speed     - port data speed according to which power down port
*       qsgmiiFullDelete    - GT_TRUE - power down QSGMII interface
*                             GT_FALSE - just free resources of this specific
*                                       member of QSGMII quadruplet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortCaelumPortDelete
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_BOOL                         qsgmiiFullDelete
)
{
    GT_STATUS               rc;         /* return code */
    GT_U32                  portMacMap; /* number of mac mapped to this physical port */
    CPSS_PORT_INTERFACE_MODE_ENT *portIfModePtr;/* interface configured on port now */
    CPSS_PORT_SPEED_ENT          *portSpeedPtr; /* speed configured on port now */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacMap);

    portIfModePtr = &(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacMap));

    /* if on port was configured interface - reset physical layer and free resources */
    if(*portIfModePtr != CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        portSpeedPtr = &(PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacMap));
        rc = prvCpssDxChPortBcat2PortReset(devNum, portNum, *portIfModePtr,
                                           *portSpeedPtr, qsgmiiFullDelete);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumPortDelete: error in prvCpssDxChPortBcat2PortReset, portNum = %d\n", portNum);
        }

        if (!PRV_CPSS_DXCH_ALDRIN_CHECK_MAC(devNum))
        {
            CPSS_TBD_BOOKMARK_ALDRIN /* add pizza and resources support for aldrin in future */
            rc = prvCpssDxChCaelumPortResourcesRelease(devNum, portNum);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumPortDelete: error in prvCpssDxChCaelumPortResourcesRelease, portNum = %d\n", portNum);
            }
        }

        /* Set PTP interface width */
        rc = prvCpssDxChPortBcat2PtpInterfaceWidthSelect(devNum, portMacMap,
                                                         CPSS_PORT_SPEED_10_E);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChCaelumPortResourcesRelease: error in prvCpssDxChPortBcat2PtpInterfaceWidthSelect, portNum = %d\n", portNum);
        }

        if (!PRV_CPSS_DXCH_ALDRIN_CHECK_MAC(devNum))
        {
            CPSS_TBD_BOOKMARK_ALDRIN /* add pizza and resources support for aldrin in future */
            rc = prvCpssDxChPortPizzaArbiterIfDelete(devNum, portNum);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChCaelumPortResourcesRelease: error in prvCpssDxChPortPizzaArbiterIfDelete, portNum = %d\n", portNum);
            }
        }
    }

    /* reset port according to new interface */
    rc = prvCpssDxChPortBcat2PortReset(devNum, portNum, ifMode, speed,
                                       qsgmiiFullDelete);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChCaelumPortResourcesRelease: error in prvCpssDxChPortBcat2PortReset, portNum = %d\n", portNum);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortCaelumModeSpeedSet
*
* DESCRIPTION:
*       Configure Interface mode and speed on a specified port and execute
*       on port's serdeses power up sequence; or configure power down on port's
*       serdeses.
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number (or CPU port)
*       powerUp   - serdes power:
*                       GT_TRUE - up;
*                       GT_FALSE - down;
*       ifMode    - interface mode (related only for serdes power up [powerUp==GT_TRUE])
*       speed     - port data speed (related only for serdes power up [powerUp==GT_TRUE])
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortCaelumModeSpeedSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  GT_BOOL                         powerUp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS               rc;         /* return code */
    GT_PHYSICAL_PORT_NUM    portNum;    /* iterator */
    GT_U32                  portMacMap; /* number of mac mapped to this physical port */
    GT_BOOL                 supported;  /* is ifMode/speed supported on port */
    MV_HWS_PORT_STANDARD    portMode;
    GT_BOOL                 lbPort;
    MV_HWS_REF_CLOCK_SUP_VAL refClock;
    MV_HWS_REF_CLOCK_SOURCE  refClockSource = PRIMARY_LINE_SRC;
    MV_HWS_PORT_INIT_FLAVOR_CFG  portFlavorParams;
    CPSS_PORT_INTERFACE_MODE_ENT *portIfModePtr;
    CPSS_PORT_SPEED_ENT          *portSpeedPtr;
    GT_BOOL                 fullConfig; /* because in QSGMII mode one serdes serves four ports
                                                    when one port of the quadruplete configured three
                                                    others configured too, so when application asks
                                                    to configure another port from quadruplete we don't need
                                                    to execute full configuretion of new port */
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    GT_U32                  originalPortStateU32;
    GT_BOOL                 originalPortState = GT_TRUE; /* port enable state
                                                            defined by application */

    if(powerUp == GT_TRUE)
    {
        rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChSerdesRefClockTranslateCpss2Hws(devNum, &refClock);
        if (rc != GT_OK)
        {
            return rc;
        }

        lbPort = (CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode) ?
                                                        GT_TRUE : GT_FALSE;
    }
    else
    {/* to prevent warnings */
        portMode = NON_SUP_MODE;
        refClock = MHz_156;
        lbPort = GT_FALSE;
    }

    for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum))
        {
            PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                         portNum,
                                                                         portMacMap);
        }
        else
        {
            continue;
        }

        rc = prvCpssDxChPortSerdesFreeCheck(devNum,portNum,ifMode,speed);
        if(rc != GT_OK)
        {
             return rc;
        }

        if ((PRV_CPSS_GE_PORT_GE_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, portMacMap))
                && (portMacMap < 48) && (CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E == ifMode)
                && (portMacMap != (portMacMap & 0xFFFFFFFC)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        portIfModePtr = &(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacMap));
        portSpeedPtr = &(PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacMap));

        if(powerUp == GT_FALSE)
        {
            /* we remove the ports, so need to use current interface and speed
             * if configured, not the values from the caller */
            if((CPSS_PORT_INTERFACE_MODE_NA_E != *portIfModePtr) &&
                                        (CPSS_PORT_SPEED_NA_E != *portSpeedPtr))
            {
                ifMode = *portIfModePtr;
                speed =  *portSpeedPtr;
            }
        }

        rc = prvCpssDxChPortInterfaceSpeedSupportLion2PlusGet(devNum, portMacMap,
                                                              ifMode, speed,
                                                              &supported);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(!supported)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        /****************************************************************************************
            PART 1 - port disable

            This configuration will trigger:
                  a. The MAC will issue an indication towards the EGF
                     to stop sending packets to it.
                  b. The MAC will issue ready to the TXFIFO and thus the
                     traffic on the way to the port will be flushed.

            PART 2 - Check all traffic to the port was flushed

            1. Read Port<%n>DescCounter in the TXQ and wait for it to reach 0.
               This counter indicates how many descriptors are allocated for the port in the TXQ queues.
            2. Read TxDMA Port <%n> Credit Counter register in the TXQ and wait for it to reach the corresponding configured descriptor credits threshold in the TXDMA (Descriptor Credits SCDMA %p).
            3. In the TXDMA regfile - read the TXFIFO credits counters status and wait for it to reach zero.
               There is a separate counter for headers (scdma_%p_header_txfifo_counter) and payloads (scdma_%p_payload_txfifo_counter).
               These counters count the amount of data that is inside the TXDMA/TXFIFO on its way to the port.

        *****************************************************************************************/

        rc = prvCpssDxChPortStateDisableSet(devNum, portNum, &originalPortStateU32);
        if (rc != GT_OK)
        {
            return rc;
        }
        originalPortState = originalPortStateU32;

        fullConfig = GT_TRUE;
        if(CPSS_PORT_INTERFACE_MODE_QSGMII_E == ifMode)
        {
            rc = bcat2QsgmiiConfig(devNum, portNum, powerUp, &fullConfig);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in bcat2QsgmiiConfig, portNum = %d, powerUp = %d\n", portNum, powerUp);
            }
        }

        /****************************************************************************************
            PART 3 - Remove resources allocated to the port

        *****************************************************************************************/
        rc = prvCpssDxChPortCaelumPortDelete(devNum, portNum, ifMode, speed,
                                            fullConfig);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChPortCaelumPortDelete, portNum = %d, ifMode = %d, speed = %d\n", portNum, ifMode, speed);
        }

        /* save new interface mode in DB after prvCpssDxChPortBcat2PortDelete */
        *portIfModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
        *portSpeedPtr = CPSS_PORT_SPEED_NA_E;

        if(!powerUp)
        {
            rc = prvCpssDxChPortStateEnableSet(devNum, portNum, (GT_U32)originalPortState);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChPortStateEnableSet, portNum = %d\n", portNum);
            }

            continue;
        }

        if (!PRV_CPSS_DXCH_ALDRIN_CHECK_MAC(devNum))
        {
            CPSS_TBD_BOOKMARK_ALDRIN /* add pizza and resources support for aldrin in future */

            rc = prvCpssDxChPortPizzaArbiterIfConfigure(devNum, portNum,
                ((speed < CPSS_PORT_SPEED_1000_E) ? CPSS_PORT_SPEED_1000_E : speed));
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChPortPizzaArbiterIfConfigure, portNum = %d\n", portNum);
            }
#ifndef GM_USED
            rc = prvCpssDxChCaelumPortResourcesConfig(devNum, portNum, ifMode, speed);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChCaelumPortResourcesConfig, portNum = %d\n", portNum);
            }
#endif
        }
        /* Set FCA interface width */
        rc = prvCpssDxChPortBcat2FcaBusWidthSet(devNum, portNum, ifMode, speed);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChPortBcat2FcaBusWidthSet, portNum = %d\n", portNum);
        }

        /* Set PTP interface width */
        rc = prvCpssDxChPortBcat2PtpInterfaceWidthSelect(devNum, portMacMap, speed);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChPortBcat2PtpInterfaceWidthSelect, portNum = %d\n", portNum);
        }

        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChPortPhysicalPortMapShadowDBGet, portNum = %d\n", portNum);
        }

        if(portMapShadowPtr->portMap.trafficManagerEn)
        {
            /* Port speed calibration value for TM Flow Control */
            rc = prvCpssDxChTmGlueFlowControlPortSpeedSet(devNum,
                                              portMapShadowPtr->portMap.tmPortIdx,
                                                          speed);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChTmGlueFlowControlPortSpeedSet, portNum = %d\n", portNum);
            }
        }

        if(fullConfig)
        {
            switch(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacMap].
                   portSerdesTuningType)
            {
                case CPSS_PORT_SERDES_TUNING_NONE_E:
                    portFlavorParams.portFlavor = PORT_REGULAR_INIT;
                    break;
                case CPSS_PORT_SERDES_TUNING_RX_ONLY_E:
                    portFlavorParams.portFlavor = PORT_RX_TRAIN_INIT;
                    break;
                case CPSS_PORT_SERDES_TUNING_TRX_E:
                    portFlavorParams.portFlavor = PORT_TRX_TRAIN_INIT;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(CPSS_PORT_INTERFACE_MODE_QSGMII_E == ifMode)
            {/* provide to HWS first port in quadruplet and it will configure
                all other ports if needed */
                portMacMap &= 0xFFFFFFFC;
            }

            if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacMap].portRefClock.enableOverride == GT_TRUE)
            {
                switch(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacMap].portRefClock.portRefClockSource)
                {
                    case CPSS_PORT_REF_CLOCK_SOURCE_PRIMARY_E:
                        refClockSource = PRIMARY_LINE_SRC;
                        break;
                    case CPSS_PORT_REF_CLOCK_SOURCE_SECONDARY_E:
                        refClockSource = SECONDARY_LINE_SRC;
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }

            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortFlavorInit(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], lbPort[%d], refClock[%d], refClockSource[%d], portFlavorParams{portFlavor[%d]})", devNum, 0, portMacMap, portMode, lbPort, refClock, PRIMARY_LINE_SRC , portFlavorParams.portFlavor);
            rc = mvHwsPortFlavorInit(devNum, 0, portMacMap, portMode, lbPort,
                                     refClock,
                                     refClockSource,
                                     portFlavorParams);
            if (rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                return rc;
            }
            CPSS_LOG_INFORMATION_MAC("Exit: mvHwsPortFlavorInit(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], lbPort[%d], refClock[%d], refClockSource[%d], portFlavorParams{portFlavor[%d]})", devNum, 0, portMacMap, portMode, lbPort, refClock, PRIMARY_LINE_SRC , portFlavorParams.portFlavor);
        }

        prvCpssDxChPortTypeSet(devNum, portMacMap, ifMode, speed);

        /* update addresses of mac registers accordingly to used MAC GE/XLG/etc. */
        rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, portNum,ifMode);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChHwRegAddrPortMacUpdate, portNum = %d\n", portNum);
        }

        if((CPSS_PORT_SPEED_10_E == speed) || (CPSS_PORT_SPEED_100_E == speed))
        {/* HWS doesn't support 10/100M, so after 1G configured by HWS, fix speed here */
            rc = geMacUnitSpeedSet(devNum, portNum, speed);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in geMacUnitSpeedSet, portNum = %d\n", portNum);
            }
        }

#ifndef GM_USED
        /* Configures type of the port connected to the LED */
        rc = prvCpssDxChLedPortTypeConfig(devNum, portNum);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChLedPortTypeConfig, portNum = %d\n", portNum);
        }
#endif
        /* save new interface mode in DB */
        *portIfModePtr = ifMode;
        *portSpeedPtr = speed;

        /*Creating Port LoopBack*/
        if(lbPort)
        {
            rc = prvCpssDxChPortSerdesLoopbackActivate (devNum, portNum, speed);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChPortSerdesLoopbackActivate, portNum = %d\n", portNum);
            }
        }

        rc = prvCpssDxChPortStateEnableSet(devNum, portNum, (GT_U32)originalPortState);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPortCaelumModeSpeedSet: error in prvCpssDxChPortStateEnableSet, portNum = %d\n", portNum);
        }

    } /* for(portNum = 0; */

    return GT_OK;
}


/******************************************************************************
* internal_cpssDxChPortResourceTmBandwidthSet
*
* DESCRIPTION:
*       Define system bandwith through TM port. Following this configuration
*       TM port resources will be allocated.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       tmBandwidthMbps - system TM bandwidth
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad parameters
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE     - on no ports connected to TM
*
* COMMENTS:
*       Note:
*           - API should not be called under traffic
*           - zero bandwidth will release resources of TM for non TM ports
******************************************************************************/
GT_STATUS internal_cpssDxChPortResourceTmBandwidthSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  tmBandwidthMbps
)
{
    GT_STATUS                                   rc;                         /* return code */
    CPSS_DXCH_DETAILED_PORT_MAP_STC          *portMapShadowPtr;   /* pointer to mapping database */
    GT_PHYSICAL_PORT_NUM                        portNum;                    /* port number loop iterator */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_XCAT3_E
                                            | CPSS_LION2_E | CPSS_BOBCAT3_E);



    /*-------------------------------------------------------------------------------------*
     *  find at least one port connected to TM                                             *
     *  if there is no ports mapped to TM, the determination of TM bandwidth is meaningless *
     *-------------------------------------------------------------------------------------*/
    for(portNum = 0; portNum < PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum); portNum++)
    {
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, /*OUT*/&portMapShadowPtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"cpssDxChPortResourceTmBandwidthSet: error in prvCpssDxChPortPhysicalPortMapShadowDBGet for port = %d\n", portNum);
        }
        if((portMapShadowPtr->valid == GT_TRUE) && (portMapShadowPtr->portMap.trafficManagerEn == GT_TRUE))
        {
            break;
        }
    }
    if(portNum == PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"cpssDxChPortResourceTmBandwidthSet: there are no ports connected to TM\n");
    }


    /* BobK family */
    if(PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChCaelumPortResourceStaticTmSet(devNum, tmBandwidthMbps);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"cpssDxChPortResourceTmBandwidthSet: error in prvCpssDxChCaelumPortResourceStaticTmSet()");
        }
    }
    else if(PRV_CPSS_DXCH_CAELUM_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChCaelumPortResourceDynamicTmSet(devNum, tmBandwidthMbps);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"cpssDxChPortResourceTmBandwidthSet: error in prvCpssDxChCaelumPortResourceDynamicTmSet()");
        }
    }
    else if(PRV_CPSS_DXCH_BOBCAT2_B0_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChBcat2B0PortResourceTmSet(devNum, tmBandwidthMbps);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"cpssDxChPortResourceTmBandwidthSet: error in prvCpssDxChCaelumPortResourceTmSet()");
        }
    }
    else if (PRV_CPSS_DXCH_BOBCAT2_A0_CHECK_MAC(devNum))
    {
        /*--------------------------------------------------------*
         * just configure TM for max resources(no specific BW)    *
         *--------------------------------------------------------*/
        rc = prvCpssDxChPort_Bcat2A0_TM_ResourcesConfig(devNum); 
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChPort_Bcat2A0_TM_ResourcesConfig(): error ()");
        }
     }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortDynamicPizzaArbiterIfTMBWSet(devNum,tmBandwidthMbps);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"cpssDxChPortResourceTmBandwidthSet: error in prvCpssDxChPortDynamicPizzaArbiterIfTMBWSet()");
    }


    return GT_OK;
}

/******************************************************************************
* cpssDxChPortResourceTmBandwidthSet
*
* DESCRIPTION:
*       Define system bandwith through TM port. Following this configuration
*       TM port resources will be allocated.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       tmBandwidthMbps - system TM bandwidth
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad parameters
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE     - on no ports connected to TM
*
* COMMENTS:
*       Note:
*           - API should not be called under traffic
*           - zero bandwidth will release resources of TM for non TM ports
******************************************************************************/
GT_STATUS cpssDxChPortResourceTmBandwidthSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  tmBandwidthMbps
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortResourceTmBandwidthSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tmBandwidthMbps));

    rc = internal_cpssDxChPortResourceTmBandwidthSet(devNum, tmBandwidthMbps);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tmBandwidthMbps));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChPortResourceTmBandwidthGet
*
* DESCRIPTION:
*       Get system bandwith through TM port. Following this configuration
*       TM port resources will be allocated.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       tmBandwidthMbpsPtr - (pointer to) system TM bandwidth
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad parameters
*       GT_FAIL          - on error
*       GT_BAD_PTR       - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE     - on no ports connected to TM
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS internal_cpssDxChPortResourceTmBandwidthGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *tmBandwidthMbpsPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_XCAT3_E
                                            | CPSS_LION2_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(tmBandwidthMbpsPtr);

    /* BobK family */
    if(PRV_CPSS_DXCH_CETUS_CHECK_MAC(devNum) || (PRV_CPSS_DXCH_CAELUM_CHECK_MAC(devNum)))
    {
        return prvCpssDxChCaelumPortResourceTmGet(devNum, tmBandwidthMbpsPtr);
    }

    /* Bobcat2 family */
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
        /*return prvCpssDxChBc2PortResourceTmSet(devNum, tmBandwidthMbps);*/
    }

    return GT_OK;
}

/******************************************************************************
* cpssDxChPortResourceTmBandwidthGet
*
* DESCRIPTION:
*       Get system bandwith through TM port. Following this configuration
*       TM port resources will be allocated.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       tmBandwidthMbpsPtr - (pointer to) system TM bandwidth
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad parameters
*       GT_FAIL          - on error
*       GT_BAD_PTR       - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE     - on no ports connected to TM
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS cpssDxChPortResourceTmBandwidthGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *tmBandwidthMbpsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortResourceTmBandwidthGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tmBandwidthMbpsPtr));

    rc = internal_cpssDxChPortResourceTmBandwidthGet(devNum, tmBandwidthMbpsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tmBandwidthMbpsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

