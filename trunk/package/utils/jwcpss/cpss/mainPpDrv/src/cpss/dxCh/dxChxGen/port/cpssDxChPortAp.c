/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenPortAp.c
*
* DESCRIPTION:
*       CPSS implementation for 802.3ap standard (defines the auto negotiation
*       for backplane Ethernet) configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortAp.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGlue.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

#include <common/siliconIf/mvSiliconIf.h>
#include <port/mvHwsPortApInitIf.h>
#include <private/mvPortModeElements.h>
#include <port/mvHwsPortCtrlApInitIf.h>
#include <port/mvHwsIpcApis.h>
#include <port/mvHwsPortCtrlApDefs.h>

static GT_STATUS prvCpssDxChPortApHighLevelConfigurationSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         portCreate,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed    
);

#define AP_DBG_EN
#ifdef AP_DBG_EN
    static GT_U32   allowPrint=0;/* option to disable the print in runtime*/
    #define AP_DBG_PRINT_MAC(x) if(allowPrint) cpssOsPrintf x

extern GT_U32 apPrintControl(IN GT_U32  allowPrintNew)
{
    GT_U32  oldState = allowPrint;
    allowPrint = allowPrintNew;

    return oldState;
}

static GT_U32   apSemPrintEn = 0; /* option to disable the print in runtime*/
extern GT_U32 apSemPrint(IN GT_U32  allowPrintNew)
{
    GT_U32  oldState = apSemPrintEn;
    apSemPrintEn = allowPrintNew;

    return oldState;
}

#else
    #define AP_DBG_PRINT_MAC(x)
#endif

/* CPSS suggested defaults for AP configuration */
static CPSS_DXCH_PORT_AP_PARAMS_STC prvCpssDxChPortApDefaultParams =
{
    /* fcPause */  GT_TRUE,
    /* fcAsmDir */ CPSS_DXCH_PORT_AP_FLOW_CONTROL_SYMMETRIC_E,
    /* fecSupported */ GT_TRUE,
    /* fecRequired */ GT_FALSE,
    /* noneceDisable */ GT_TRUE, CPSS_TBD_BOOKMARK_LION2 /* to fix when Z80 algorithm fixed */
    /* laneNum */       0,
    /* modesAdvertiseArr */
    {
        {CPSS_PORT_INTERFACE_MODE_1000BASE_X_E, CPSS_PORT_SPEED_1000_E}
        ,{CPSS_PORT_INTERFACE_MODE_XGMII_E, CPSS_PORT_SPEED_10000_E}
        ,{CPSS_PORT_INTERFACE_MODE_KR_E, CPSS_PORT_SPEED_10000_E}
        ,{CPSS_PORT_INTERFACE_MODE_KR_E, CPSS_PORT_SPEED_40000_E}
        ,{CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E, CPSS_PORT_SPEED_NA_E}
    }
};

/*******************************************************************************
* internal_cpssDxChPortApEnableSet
*
* DESCRIPTION:
*       Enable/disable AP engine (loads AP code into shared memory and starts AP
*       engine).
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupsBmp   - bitmap of cores where to run AP engine
*                           (0x7FFFFFF - for ALL)
*       enable  - GT_TRUE  - enable AP on port group
*                 GT_FALSE - disbale
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApEnableSet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_BOOL             enable
)
{
    GT_U32      portGroupId;/* local core number */
    GT_STATUS   rc;         /* return code */
    
    if(NULL == PRV_CPSS_PP_MAC(devNum))
    {/* because for BC2 engaged before info about devices configured */
        AP_DBG_PRINT_MAC(("cpssDxChPortApEnableSet:devNum=%d,portGroupsBmp=%d,enable=%d\n", 
                            devNum, portGroupsBmp, enable));

        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsServiceCpuEnable(devNum[%d], enable[%d])", devNum, enable);        
        rc = mvHwsServiceCpuEnable(devNum, enable); 
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        }
    }
    else
    {            
        PRV_CPSS_DEV_CHECK_MAC(devNum);
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                              | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                              | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E 
                                              | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    
        AP_DBG_PRINT_MAC(("cpssDxChPortApEnableSet:devNum=%d,portGroupsBmp=%d,enable=%d\n", 
                            devNum, portGroupsBmp, enable));
        
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp,
                                                          portGroupId)
        {
            if(enable)
            {
                CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApEngineInit(devNum[%d], portGroupId[%d])", devNum, portGroupId);
                rc = mvHwsApEngineInit(devNum,portGroupId);
            }
            else
            {
                CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApEngineStop(devNum[%d], portGroupId[%d])", devNum, portGroupId);
                rc = mvHwsApEngineStop(devNum,portGroupId);
            }
            if(rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp,
                                                        portGroupId)
    }
    
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortApEnableSet
*
* DESCRIPTION:
*       Enable/disable AP engine (loads AP code into shared memory and starts AP
*       engine).
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupsBmp   - bitmap of cores where to run AP engine
*                           (0x7FFFFFF - for ALL)
*       enable  - GT_TRUE  - enable AP on port group
*                 GT_FALSE - disbale
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Pay attention: for BC2 must be engaged before port LIB init i.e. before
*                      phase1 init!
*
*******************************************************************************/
GT_STATUS cpssDxChPortApEnableSet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_BOOL             enable
)
{
    GT_STATUS rc;
    
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApEnableSet);    
    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, enable));
    
    rc = internal_cpssDxChPortApEnableSet(devNum, portGroupsBmp, enable);    
    
    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, enable));
    CPSS_API_UNLOCK_MAC(0,0);   
    
    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortApEnableGet
*
* DESCRIPTION:
*       Get AP engine enabled and functional on port group (local core) status.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core)
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - AP enabled and functional on port group
*                    GT_FALSE - disabled or not functional
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port group or device
*       GT_BAD_PTR               - enabledPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupNum,
    OUT GT_BOOL *enabledPtr
)
{
    GT_STATUS       rc;         /* return code */
    GT_U32 counter1;   /* AP watchdog value first read */

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(enabledPtr);

    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApEngineInitGet(devNum[%d], portGroupNum[%d], *enabledPtr)", devNum, portGroupNum);
        (GT_VOID)mvHwsApEngineInitGet(devNum,portGroupNum,enabledPtr);
    
        if(GT_FALSE == *enabledPtr)
        {
            AP_DBG_PRINT_MAC(("mvHwsApEngineInitGet:enabled=%d\n", *enabledPtr));
            return GT_OK;
        }
        CPSS_LOG_INFORMATION_MAC("Calling: mvApCheckCounterGet(devNum[%d], portGroupNum[%d], *counter1)", devNum, portGroupNum);
        rc = mvApCheckCounterGet(devNum,portGroupNum,&counter1);
        AP_DBG_PRINT_MAC(("mvApCheckCounterGet_1:rc=%d,counter=%d\n", rc, counter1));
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
    }
    else
    {    
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsServiceCpuEnableGet(devNum[%d])", devNum);        
        AP_DBG_PRINT_MAC(("mvHwsServiceCpuEnableGet(%d)\n", devNum));
        *enabledPtr = mvHwsServiceCpuEnableGet(devNum);
        CPSS_LOG_INFORMATION_MAC("service cpu %s", (*enabledPtr) ? "enabled" : "disabled");
        AP_DBG_PRINT_MAC(("service cpu %s\n", (*enabledPtr) ? "enabled" : "disabled"));
    }
    
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortApEnableGet
*
* DESCRIPTION:
*       Get AP engine enabled and functional on port group (local core) status.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core)
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - AP enabled and functional on port group
*                    GT_FALSE - disabled or not functional
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port group or device
*       GT_BAD_PTR               - enabledPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupNum,
    OUT GT_BOOL *enabledPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupNum, enabledPtr));

    rc = internal_cpssDxChPortApEnableGet(devNum, portGroupNum, enabledPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupNum, enabledPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortLion2ApModesVectorBuild
*
* DESCRIPTION:
*       Get array of port modes in CPSS format and build advertisement array in
*       HWS format.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       modesAdvertiseArrPtr - array of advertised modes in CPSS format
*
* OUTPUTS:
*       modesVectorPtr - bitmap of advertised modes in HWS format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortLion2ApModesVectorBuild
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  CPSS_PORT_MODE_SPEED_STC *modesAdvertiseArrPtr,
    OUT GT_U32                   *modesVectorPtr
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;   /* iterator */
    GT_U32      localPort; /* number of port in core */
    GT_U32      macNum;     /* number of used MAC unit */
    GT_U32      pcsNum;     /* number of used PCS unit */
    GT_U32      sdVecSize;         /* size of serdes redundancy array */
    GT_U32      *sdVectorPtr;      /* serdes redundancy array */

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    for(i = 0, *modesVectorPtr = 0; 
         (modesAdvertiseArrPtr[i].ifMode != CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E)
             && (modesAdvertiseArrPtr[i].ifMode != CPSS_PORT_INTERFACE_MODE_NA_E)
             && (i < CPSS_DXCH_PORT_AP_IF_ARRAY_SIZE_CNS);
         i++)
    {
        switch(modesAdvertiseArrPtr[i].ifMode)
        {
            case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
                if(modesAdvertiseArrPtr[i].speed != CPSS_PORT_SPEED_1000_E)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%s], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, "_1000Base_X");
				rc = hwsPortsParamsCfgGet(devNum, 0, localPort, _1000Base_X, &macNum,
                                          &pcsNum, &sdVecSize, &sdVectorPtr);
                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }
                if(NA_NUM == macNum)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return GT_BAD_PARAM");
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                *modesVectorPtr |= _1000Base_KX_Bit0;
                break;
            case CPSS_PORT_INTERFACE_MODE_XGMII_E:
                if(modesAdvertiseArrPtr[i].speed != CPSS_PORT_SPEED_10000_E)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%s], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, "_10GBase_KX4");
				rc = hwsPortsParamsCfgGet(devNum, 0, localPort, _10GBase_KX4, &macNum,
                                          &pcsNum, &sdVecSize, &sdVectorPtr);
                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }
                if(NA_NUM == macNum)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return GT_BAD_PARAM");
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                *modesVectorPtr |= _10GBase_KX4_Bit1;
                break;
            case CPSS_PORT_INTERFACE_MODE_KR_E:
                if(CPSS_PORT_SPEED_10000_E == modesAdvertiseArrPtr[i].speed)
                {
                    CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%s], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, "_10GBase_KR");
                    rc = hwsPortsParamsCfgGet(devNum, 0, localPort, _10GBase_KR, &macNum,
                                              &pcsNum, &sdVecSize, &sdVectorPtr);
                    *modesVectorPtr |= _10GBase_KR_Bit2;
                }
                else if(CPSS_PORT_SPEED_40000_E == modesAdvertiseArrPtr[i].speed)
                {
                    CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%s], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, "_40GBase_KR");
                    rc = hwsPortsParamsCfgGet(devNum, 0, localPort, _40GBase_KR, &macNum,
                                              &pcsNum, &sdVecSize, &sdVectorPtr);
                    *modesVectorPtr |= _40GBase_KR4_Bit3;
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }
                if(NA_NUM == macNum)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return GT_BAD_PARAM");
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortBc2ApModesVectorBuild
*
* DESCRIPTION:
*       Get array of port modes in CPSS format and build advertisement array in
*       HWS format.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       modesAdvertiseArrPtr - array of advertised modes in CPSS format
*
* OUTPUTS:
*       modesVectorPtr - bitmap of advertised modes in HWS format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBc2ApModesVectorBuild
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  CPSS_PORT_MODE_SPEED_STC *modesAdvertiseArrPtr,
    OUT GT_U32                   *modesVectorPtr
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;   /* iterator */
    GT_U32      localPort; /* number of port in core */
    GT_U32      macNum;     /* number of used MAC unit */
    GT_U32      pcsNum;     /* number of used PCS unit */
    GT_U32      sdVecSize;         /* size of serdes redundancy array */
    GT_U32      *sdVectorPtr;      /* serdes redundancy array */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 localPort);

    for(i = 0, *modesVectorPtr = 0; 
         (modesAdvertiseArrPtr[i].ifMode != CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E)
             && (modesAdvertiseArrPtr[i].ifMode != CPSS_PORT_INTERFACE_MODE_NA_E)
             && (i < CPSS_DXCH_PORT_AP_IF_ARRAY_SIZE_CNS);
         i++)
    {
        switch(modesAdvertiseArrPtr[i].ifMode)
        {
            case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
                if(modesAdvertiseArrPtr[i].speed != CPSS_PORT_SPEED_1000_E)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%s], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, "_1000Base_X");
				rc = hwsPortsParamsCfgGet(devNum, 0, localPort, _1000Base_X, &macNum,
                                          &pcsNum, &sdVecSize, &sdVectorPtr);
                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }
                if(NA_NUM == macNum)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return GT_BAD_PARAM");
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                AP_CTRL_1000Base_KX_SET(*modesVectorPtr, 1);
                break;
            case CPSS_PORT_INTERFACE_MODE_XGMII_E:
                if(modesAdvertiseArrPtr[i].speed != CPSS_PORT_SPEED_10000_E)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%s], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, "_10GBase_KX4");
				rc = hwsPortsParamsCfgGet(devNum, 0, localPort, _10GBase_KX4, &macNum,
                                          &pcsNum, &sdVecSize, &sdVectorPtr);
                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }
                if(NA_NUM == macNum)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return GT_BAD_PARAM");
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                AP_CTRL_10GBase_KX4_SET(*modesVectorPtr, 1);
                break;
            case CPSS_PORT_INTERFACE_MODE_KR_E:
                if(CPSS_PORT_SPEED_10000_E == modesAdvertiseArrPtr[i].speed)
                {
                    CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%s], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, "_10GBase_KR");
                    rc = hwsPortsParamsCfgGet(devNum, 0, localPort, _10GBase_KR, &macNum,
                                              &pcsNum, &sdVecSize, &sdVectorPtr);
                    AP_CTRL_10GBase_KR_SET(*modesVectorPtr, 1);
                }
                else if(CPSS_PORT_SPEED_40000_E == modesAdvertiseArrPtr[i].speed)
                {
                    CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%s], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, 0, localPort, "_40GBase_KR");
                    rc = hwsPortsParamsCfgGet(devNum, 0, localPort, _40GBase_KR, &macNum,
                                              &pcsNum, &sdVecSize, &sdVectorPtr);
                    AP_CTRL_40GBase_KR4_SET(*modesVectorPtr, 1);
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }
                if(NA_NUM == macNum)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return GT_BAD_PARAM");
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

	if (0 == *modesVectorPtr)
	{
		CPSS_LOG_INFORMATION_MAC("Advertisement modes not defined");
		CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
	}

	return GT_OK; 
}

/*******************************************************************************
* internal_cpssDxChPortApPortConfigSet
*
* DESCRIPTION:
*       Enable/disable AP process on port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*       apEnable    - AP enable/disable on port
*       apParamsPtr - (ptr to) AP parameters for port
*                               (NULL - for CPSS defaults).
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
*   For now allowed negotiation on serdes lanes of port 0-3, because in Lion2
*   just these options are theoreticaly possible.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApPortConfigSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         apEnable,
    IN  CPSS_DXCH_PORT_AP_PARAMS_STC    *apParamsPtr
)
{
    GT_STATUS                rc;        /* return code */
    GT_U32                   portGroup; /* local core number */
    GT_U32                   phyPortNum;/* port number in local core */
    MV_HWS_AP_CFG            apCfg;     /* AP parameters of port in HWS format */
    CPSS_DXCH_PORT_AP_PARAMS_STC  *localApParamsPtr;/* temporary pointer to
                                        AP configuration parameters structure */

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 phyPortNum);

    AP_DBG_PRINT_MAC(("cpssDxChPortApPortConfigSet:devNum=%d,portNum=%d,apEnable=%d\n", 
                        devNum, portNum, apEnable));

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    if(CPSS_PP_FAMILY_DXCH_BOBCAT2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        if (PRV_CPSS_GE_PORT_GE_ONLY_E == 
                        PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, phyPortNum))
        {
            return GT_NOT_SUPPORTED;
        }
    }
    
    if(GT_FALSE == apEnable)
    {
        if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortStop(devNum[%d], portGroup[%d], phyPortNum[%d], action[%s])", devNum, portGroup, phyPortNum, "PORT_POWER_DOWN");
            rc = mvHwsApPortStop(devNum,portGroup,phyPortNum,PORT_POWER_DOWN);
            if(rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            }        
        }
        else
        {            
            MV_HWS_AP_PORT_STATUS   apStatus;

            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlStatusGet(devNum[%d], portGroup[%d], phyPortNum[%d] &apStatus)", devNum, portGroup, phyPortNum); 
            cpssOsMemSet(&apStatus, 0, sizeof(MV_HWS_AP_PORT_STATUS));
            rc = mvHwsApPortCtrlStatusGet(devNum, portGroup, phyPortNum, &apStatus);
            if(rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                return rc;
            }
            if(apStatus.smState != 0)/* if AP enabled on port stop it as required */
            {
				GT_U32	portState; /* (dummy) current enable port state */
				rc = prvCpssDxChPortStateDisableSet(devNum, portNum, &portState);
				if(rc != GT_OK)
				{
					return rc;
				}

                CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlStop(devNum[%d], portGroup[%d], phyPortNum[%d])", devNum, portGroup, phyPortNum);
                rc = mvHwsApPortCtrlStop(devNum,portGroup,phyPortNum);
                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }
            }
CPSS_TBD_BOOKMARK_BOBCAT2            
            if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, phyPortNum) != CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                /* temporary till new AP IPC mechanism fully functional */
                rc = prvCpssDxChPortApHighLevelConfigurationSet(devNum, portNum, GT_FALSE,
                                                                CPSS_PORT_INTERFACE_MODE_KR_E,
                                                                CPSS_PORT_SPEED_10000_E);
                if(rc != GT_OK)
                {
                    AP_DBG_PRINT_MAC(("prvCpssDxChPortApHighLevelConfigurationSet:devNum=%d,portNum=%d,FALSE\n", 
                                        devNum, portNum));
                    return rc;
                }
            }
                        
            /* enable port MAC after it was disabled when AP enabled and 
               could be left disabled if there was no link */
            rc = prvCpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
            if(rc!=GT_OK)
            {
                AP_DBG_PRINT_MAC(("prvCpssDxChPortEnableSet:devNum=%d,portNum=%d,TRUE\n", 
                                    devNum, portNum));
                return rc;
            }
        }
        return rc;
    }

    localApParamsPtr = (apParamsPtr != NULL) ? apParamsPtr :
                                                &prvCpssDxChPortApDefaultParams;

    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {    
        rc = prvCpssDxChPortLion2ApModesVectorBuild(devNum, portNum,
                                               localApParamsPtr->modesAdvertiseArr,
                                               &apCfg.modesVector);
    }
    else
    {
        rc = prvCpssDxChPortBc2ApModesVectorBuild(devNum, portNum,
                                               localApParamsPtr->modesAdvertiseArr,
                                               &apCfg.modesVector);
    }
    
    if(rc != GT_OK)
    {
        AP_DBG_PRINT_MAC(("ApModesVectorBuild:devNum=%d,portNum=%d\n", 
                            devNum, portNum));
        return rc;
    }

    if((localApParamsPtr->fcAsmDir != CPSS_DXCH_PORT_AP_FLOW_CONTROL_SYMMETRIC_E)
        && (localApParamsPtr->fcAsmDir != CPSS_DXCH_PORT_AP_FLOW_CONTROL_ASYMMETRIC_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    apCfg.fcAsmDir  = localApParamsPtr->fcAsmDir;
    apCfg.fcPause   = localApParamsPtr->fcPause;
    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {    
        if((apCfg.modesVector & _10GBase_KR_Bit2) ||
           (apCfg.modesVector & _40GBase_KR4_Bit3))
        {
            apCfg.fecReq    = localApParamsPtr->fecRequired;
            apCfg.fecSup    = localApParamsPtr->fecSupported;
        }
        else
        {
            apCfg.fecReq    = GT_FALSE;
            apCfg.fecSup    = GT_FALSE;
        }
    }
    else
    {
        if(AP_CTRL_10GBase_KR_GET(apCfg.modesVector) ||
           AP_CTRL_40GBase_KR4_GET(apCfg.modesVector))
        {
            apCfg.fecReq    = localApParamsPtr->fecRequired;
            apCfg.fecSup    = localApParamsPtr->fecSupported;
        }
        else
        {
            apCfg.fecReq    = GT_FALSE;
            apCfg.fecSup    = GT_FALSE;
        }
    }

    apCfg.nonceDis  = localApParamsPtr->noneceDisable;
    if(localApParamsPtr->laneNum > 3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        apCfg.apLaneNum = localApParamsPtr->laneNum;
    }
    else
    {
        GT_U32 macNum;
        GT_U32 pcsNum;
        GT_U32 sdVecSize;
        GT_U32 *sdVectorPtr;
        
        rc = hwsPortsParamsCfgGet(devNum, portGroup,
                                  phyPortNum, _10GBase_KR, &macNum, &pcsNum,
                                  &sdVecSize, &sdVectorPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
		if (NULL == sdVectorPtr)
		{
			return GT_BAD_STATE;
		}
		apCfg.apLaneNum = sdVectorPtr[0] + localApParamsPtr->laneNum; 
    }

    apCfg.refClockCfg.refClockSource = PRIMARY_LINE_SRC;

    rc = prvCpssDxChSerdesRefClockTranslateCpss2Hws(devNum,
                                                 &apCfg.refClockCfg.refClockFreq);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
CPSS_TBD_BOOKMARK_LION2 /* for now optimization algorithms cause AP excide 500 mSec */
        rc = cpssDxChPortSerdesAutoTuneOptAlgSet(devNum, portNum,
                                                 CPSS_PORT_SERDES_TRAINING_OPTIMISATION_NONE_E);
        if (rc != GT_OK)
        {
            return rc;
        }
        phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortStart(devNum[%d], portGroup[%d], phyPortNum[%d], apCfg{apLaneNum[%d], modesVector[%d], fcPause[%d], fcAsmDir[%d], fecSup[%d], fecReq[%d], nonceDis[%d], refClockCfg{refClockFreq[%d], refClockSource[%d]}})", 
                            devNum, portGroup, phyPortNum, apCfg.apLaneNum,apCfg.modesVector,apCfg.fcPause,apCfg.fcAsmDir,apCfg.fecSup,apCfg.fecReq, apCfg.nonceDis, apCfg.refClockCfg.refClockFreq,apCfg.refClockCfg.refClockSource);
    
        AP_DBG_PRINT_MAC(("mvHwsApPortStart:devNum=%d,portGroup=%d,phyPortNum=%d,\
    laneNum=%d,modesVector=0x%x,fcAsmDir=%d,fcPause=%d,fecReq=%d,fecSup=%d,nonceDis=%d,\
    refClock=%d,refClockSource=%d\n",
                          devNum, portGroup, phyPortNum, apCfg.apLaneNum,
                          apCfg.modesVector,apCfg.fcAsmDir,apCfg.fcPause,apCfg.fecReq,
                          apCfg.fecSup, apCfg.nonceDis, apCfg.refClockCfg.refClockFreq,
                          apCfg.refClockCfg.refClockSource));
        rc = mvHwsApPortStart(devNum, portGroup, phyPortNum, &apCfg);
    }
    else
    {/* BC2 */
        
        /* disable port MAC to prevent traffic during link establish */
        rc = prvCpssDxChPortEnableSet(devNum, portNum, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
CPSS_TBD_BOOKMARK_BOBCAT2        
        /* temporary till new AP IPC mechanism fully functional */
        rc = prvCpssDxChPortApHighLevelConfigurationSet(devNum, portNum, GT_TRUE,
                                                        CPSS_PORT_INTERFACE_MODE_KR_E,
                                                        CPSS_PORT_SPEED_10000_E);
        if(rc != GT_OK)
        {
            return rc;
        }
        
        PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                     portNum,
                                                                     phyPortNum);

        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlStart(devNum[%d], portGroup[%d], phyPortNum[%d], apCfg{apLaneNum[%d], modesVector[%d], fcPause[%d], fcAsmDir[%d], fecSup[%d], fecReq[%d], nonceDis[%d], refClockCfg{refClockFreq[%d], refClockSource[%d]}})", 
                            devNum, portGroup, phyPortNum, apCfg.apLaneNum,apCfg.modesVector,apCfg.fcPause,apCfg.fcAsmDir,apCfg.fecSup,apCfg.fecReq, apCfg.nonceDis, apCfg.refClockCfg.refClockFreq,apCfg.refClockCfg.refClockSource);
        AP_DBG_PRINT_MAC(("mvHwsApPortCtrlStart:devNum=%d,portGroup=%d,phyPortNum=%d,\
    laneNum=%d,modesVector=0x%x,fcAsmDir=%d,fcPause=%d,fecReq=%d,fecSup=%d,nonceDis=%d,\
    refClock=%d,refClockSource=%d\n",
                          devNum, portGroup, phyPortNum, apCfg.apLaneNum,
                          apCfg.modesVector,apCfg.fcAsmDir,apCfg.fcPause,apCfg.fecReq,
                          apCfg.fecSup, apCfg.nonceDis, apCfg.refClockCfg.refClockFreq,
                          apCfg.refClockCfg.refClockSource));
        rc = mvHwsApPortCtrlStart(devNum, portGroup, phyPortNum, &apCfg);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        }
    }
    
	if(rc != GT_OK)
	{
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
	}
	return rc;
}

/*******************************************************************************
* cpssDxChPortApPortConfigSet
*
* DESCRIPTION:
*       Enable/disable AP process on port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*       apEnable    - AP enable/disable on port
*       apParamsPtr - (ptr to) AP parameters for port
*                               (NULL - for CPSS defaults).
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
*   For now allowed negotiation on serdes lanes of port 0-3, because in Lion2
*   just these options are theoreticaly possible.
*
*******************************************************************************/
GT_STATUS cpssDxChPortApPortConfigSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         apEnable,
    IN  CPSS_DXCH_PORT_AP_PARAMS_STC    *apParamsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApPortConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, apEnable, apParamsPtr));

    rc = internal_cpssDxChPortApPortConfigSet(devNum, portNum, apEnable, apParamsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, apEnable, apParamsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortApPortConfigGet
*
* DESCRIPTION:
*       Get AP configuration of port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat3.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       apEnablePtr - AP enable/disable on port
*       apParamsPtr - (ptr to) AP parameters of port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apEnablePtr or apParamsPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApPortConfigGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_BOOL                         *apEnablePtr,
    OUT CPSS_DXCH_PORT_AP_PARAMS_STC    *apParamsPtr
)
{
    GT_STATUS       rc;         /* return code */
    MV_HWS_AP_CFG   apCfg;      /* AP configuration parameters */
    GT_U32          portGroup;  /* local core number */
    GT_U32          phyPortNum; /* port number in local core */
    GT_U32          i;          /* iterator */

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 phyPortNum);
    CPSS_NULL_PTR_CHECK_MAC(apEnablePtr);
    CPSS_NULL_PTR_CHECK_MAC(apParamsPtr);

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    cpssOsMemSet(&apCfg, 0, sizeof(MV_HWS_AP_CFG));
    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApEngineInitGet(devNum[%d], portGroup[%d], *apEnablePtr)", devNum, portGroup);
        (GT_VOID)mvHwsApEngineInitGet(devNum,portGroup,apEnablePtr);
        if(GT_FALSE == *apEnablePtr)
        {
            return GT_OK;
        }
        phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortConfigGet(devNum[%d], portGroup[%d], phyPortNum[%d] *apEnablePtr, *apCfg)", devNum, portGroup, phyPortNum);
        rc = mvHwsApPortConfigGet(devNum,portGroup,phyPortNum,apEnablePtr,&apCfg);
        AP_DBG_PRINT_MAC(("mvHwsApPortConfigGet:rc=%d,devNum=%d,portGroup=%d,phyPortNum=%d,\
    apLaneNum=%d,modesVector=0x%x,fcAsmDir=%d,fcPause=%d,fecReq=%d,fecSup=%d,nonceDis=%d,\
    refClockFreq=%d,refClockSource=%d,apEnable=%d\n",
                          rc, devNum, portGroup, phyPortNum, apCfg.apLaneNum,
                          apCfg.modesVector,apCfg.fcAsmDir,apCfg.fcPause,apCfg.fecReq,
                          apCfg.fecSup, apCfg.nonceDis, apCfg.refClockCfg.refClockFreq,
                          apCfg.refClockCfg.refClockSource, *apEnablePtr));
    }
    else
    {       
        MV_HWS_AP_PORT_STATUS   apStatus;

        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsServiceCpuEnableGet(devNum[%d])", devNum);        
        *apEnablePtr = mvHwsServiceCpuEnableGet(devNum);
        if(GT_FALSE == *apEnablePtr)
        {
            return GT_OK;
        }
        if (PRV_CPSS_GE_PORT_GE_ONLY_E == 
                        PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, phyPortNum))
        {
            *apEnablePtr = GT_FALSE;
            return GT_OK;
        }

        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlStatusGet(devNum[%d], portGroup[%d], phyPortNum[%d] &apStatus)", devNum, portGroup, phyPortNum); 
        cpssOsMemSet(&apStatus, 0, sizeof(MV_HWS_AP_PORT_STATUS));
        rc = mvHwsApPortCtrlStatusGet(devNum, portGroup, phyPortNum, &apStatus);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
        *apEnablePtr = (0 == apStatus.smState) ? GT_FALSE : GT_TRUE;
        if(GT_TRUE == *apEnablePtr)/* if AP disabled on port no need to read garbage */
        {
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlCfgGet(devNum[%d], portGroup[%d], phyPortNum[%d] *apCfg)", devNum, portGroup, phyPortNum); 
            rc = mvHwsApPortCtrlCfgGet(devNum,portGroup,phyPortNum,&apCfg);
            AP_DBG_PRINT_MAC(("mvHwsApPortCtrlCfgGet:rc=%d,devNum=%d,portGroup=%d,phyPortNum=%d,\
apLaneNum=%d,modesVector=0x%x,fcAsmDir=%d,fcPause=%d,fecReq=%d,fecSup=%d,nonceDis=%d,\
refClockFreq=%d,refClockSource=%d\n",
                              rc, devNum, portGroup, phyPortNum, apCfg.apLaneNum,
                              apCfg.modesVector,apCfg.fcAsmDir,apCfg.fcPause,apCfg.fecReq,
                              apCfg.fecSup, apCfg.nonceDis, apCfg.refClockCfg.refClockFreq,
                              apCfg.refClockCfg.refClockSource));
        }
    }
        
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
    }

    apParamsPtr->fcAsmDir = apCfg.fcAsmDir;
    apParamsPtr->fcPause = apCfg.fcPause;
    apParamsPtr->fecRequired = apCfg.fecReq;
    apParamsPtr->fecSupported = apCfg.fecSup;    
    apParamsPtr->noneceDisable = apCfg.nonceDis;

    i = 0;
    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {    
        apParamsPtr->laneNum = apCfg.apLaneNum;

		if(apCfg.modesVector & _10GBase_KX4_Bit1)
		{
			apParamsPtr->modesAdvertiseArr[i].ifMode =
				CPSS_PORT_INTERFACE_MODE_XGMII_E;
			apParamsPtr->modesAdvertiseArr[i++].speed = CPSS_PORT_SPEED_10000_E;
		}

		if(apCfg.modesVector & _1000Base_KX_Bit0)
		{
			apParamsPtr->modesAdvertiseArr[i].ifMode =
				CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
			apParamsPtr->modesAdvertiseArr[i++].speed = CPSS_PORT_SPEED_1000_E;
		}

		if(apCfg.modesVector & _10GBase_KR_Bit2)
		{
			apParamsPtr->modesAdvertiseArr[i].ifMode =
				CPSS_PORT_INTERFACE_MODE_KR_E;
			apParamsPtr->modesAdvertiseArr[i++].speed = CPSS_PORT_SPEED_10000_E;
		}

		if(apCfg.modesVector & _40GBase_KR4_Bit3)
		{
			apParamsPtr->modesAdvertiseArr[i].ifMode =
				CPSS_PORT_INTERFACE_MODE_KR_E;
			apParamsPtr->modesAdvertiseArr[i++].speed = CPSS_PORT_SPEED_40000_E;
		}
    }
    else
    {
		if (apCfg.modesVector != 0)
		{
			GT_U32 macNum;
			GT_U32 pcsNum;
			GT_U32 sdVecSize;
			GT_U32 *sdVectorPtr;

			rc = hwsPortsParamsCfgGet(devNum, portGroup,
                                      phyPortNum, _10GBase_KR, &macNum, &pcsNum,
                                      &sdVecSize, &sdVectorPtr);
			if(rc != GT_OK)
			{
				return rc;
			}
			if (NULL == sdVectorPtr)
			{
				return GT_BAD_STATE;
			}
			apParamsPtr->laneNum = apCfg.apLaneNum - sdVectorPtr[0];
		}
		else
		{/* HWS pass garbage in such case */
			cpssOsMemSet(apParamsPtr, 0, sizeof(CPSS_DXCH_PORT_AP_PARAMS_STC));
		}

		if(apCfg.modesVector & (1<<_10GBase_KX4_Bit1))
		{
			apParamsPtr->modesAdvertiseArr[i].ifMode =
				CPSS_PORT_INTERFACE_MODE_XGMII_E;
			apParamsPtr->modesAdvertiseArr[i++].speed = CPSS_PORT_SPEED_10000_E;
		}

		if(apCfg.modesVector & (1<<_1000Base_KX_Bit0))
		{
			apParamsPtr->modesAdvertiseArr[i].ifMode =
				CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
			apParamsPtr->modesAdvertiseArr[i++].speed = CPSS_PORT_SPEED_1000_E;
		}

		if(apCfg.modesVector & (1<<_10GBase_KR_Bit2))
		{
			apParamsPtr->modesAdvertiseArr[i].ifMode =
				CPSS_PORT_INTERFACE_MODE_KR_E;
			apParamsPtr->modesAdvertiseArr[i++].speed = CPSS_PORT_SPEED_10000_E;
		}

		if(apCfg.modesVector & (1<<_40GBase_KR4_Bit3))
		{
			apParamsPtr->modesAdvertiseArr[i].ifMode =
				CPSS_PORT_INTERFACE_MODE_KR_E;
			apParamsPtr->modesAdvertiseArr[i++].speed = CPSS_PORT_SPEED_40000_E;
		}
    }
        
	for(;i<CPSS_DXCH_PORT_AP_IF_ARRAY_SIZE_CNS;i++)
	{
		apParamsPtr->modesAdvertiseArr[i].ifMode =
			CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E;
		apParamsPtr->modesAdvertiseArr[i].speed = CPSS_PORT_SPEED_NA_E;
	}

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortApPortConfigGet
*
* DESCRIPTION:
*       Get AP configuration of port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       apEnablePtr - AP enable/disable on port
*       apParamsPtr - (ptr to) AP parameters of port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apEnablePtr or apParamsPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApPortConfigGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_BOOL                         *apEnablePtr,
    OUT CPSS_DXCH_PORT_AP_PARAMS_STC    *apParamsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApPortConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, apEnablePtr, apParamsPtr));

    rc = internal_cpssDxChPortApPortConfigGet(devNum, portNum, apEnablePtr, apParamsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, apEnablePtr, apParamsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortApPortStatusGet
*
* DESCRIPTION:
*       Get status of AP on port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       apStatusPtr - (ptr to) AP parameters for port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apStatusPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - AP engine not run
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApPortStatusGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_PORT_AP_STATUS_STC   *apStatusPtr
)
{
    GT_STATUS   rc; /* return status */
    GT_U32                   portGroup;
    GT_U32                   apPortNum;
    MV_HWS_AP_PORT_STATUS    apResult;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 apPortNum);
    CPSS_NULL_PTR_CHECK_MAC(apStatusPtr);

    cpssOsMemSet(&apResult, 0, sizeof(MV_HWS_AP_PORT_STATUS));
    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        apPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortStatusGet(devNum[%d], portGroup[%d], apPortNum[%d], *apResult)", devNum, portGroup, apPortNum);
        rc = mvHwsApPortStatusGet(devNum, portGroup, apPortNum, &apResult);
    }
    else
    {
        if (PRV_CPSS_GE_PORT_GE_ONLY_E == 
                        PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, apPortNum))
        {
            AP_DBG_PRINT_MAC(("GE_only port\n"));
            apStatusPtr->hcdFound = GT_FALSE;
            return GT_OK;
        }
        
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlStatusGet(devNum[%d], portGroup[%d], apPortNum[%d], *apResult)", devNum, portGroup, apPortNum);
        rc = mvHwsApPortCtrlStatusGet(devNum, portGroup, apPortNum, &apResult);
    }

    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
    }
    
    AP_DBG_PRINT_MAC(("cpssDxChPortApPortStatusGet:mvHwsApPortStatusGet:portGroup=%d,apPortNum=%d,\
apResult:apLaneNum=%d,hcdFound=%d,hcdLinkStatus=%d,\
hcdFecEn=%d,hcdFcPause=%d,hcdFcAsmDir=%d,postApPortMode=%d,\
postApPortNum=%d,preApPortNum=%d\n",
                      portGroup, apPortNum,
                      apResult.apLaneNum,
                      apResult.hcdResult.hcdFound,
                      apResult.hcdResult.hcdLinkStatus,
                      apResult.hcdResult.hcdFecEn,
                      apResult.hcdResult.hcdFcRxPauseEn,
                      apResult.hcdResult.hcdFcTxPauseEn,
                      apResult.postApPortMode,
                      apResult.postApPortNum,
                      apResult.preApPortNum));

    cpssOsMemSet(apStatusPtr, 0, sizeof(CPSS_DXCH_PORT_AP_STATUS_STC));
    if((apStatusPtr->hcdFound = apResult.hcdResult.hcdFound) != GT_TRUE)
    {/* nothing interesting any more */
        return GT_OK;
    }

    apStatusPtr->fecEnabled = apResult.hcdResult.hcdFecEn;

    switch(apResult.postApPortMode)
    {
        case _1000Base_X:
            apStatusPtr->portMode.ifMode = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
            apStatusPtr->portMode.speed = CPSS_PORT_SPEED_1000_E;
            break;
        case _10GBase_KX4:
            apStatusPtr->portMode.ifMode = CPSS_PORT_INTERFACE_MODE_XGMII_E;
            apStatusPtr->portMode.speed = CPSS_PORT_SPEED_10000_E;
            break;
        case _10GBase_KR:
            apStatusPtr->portMode.ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
            apStatusPtr->portMode.speed = CPSS_PORT_SPEED_10000_E;
            break;
        case _40GBase_KR:
            apStatusPtr->portMode.ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
            apStatusPtr->portMode.speed = CPSS_PORT_SPEED_40000_E;
            break;
        case _10GBase_SR_LR:
            apStatusPtr->portMode.ifMode = CPSS_PORT_INTERFACE_MODE_SR_LR_E;
            apStatusPtr->portMode.speed = CPSS_PORT_SPEED_10000_E;
            break;
        case _40GBase_SR_LR:
            apStatusPtr->portMode.ifMode = CPSS_PORT_INTERFACE_MODE_SR_LR_E;
            apStatusPtr->portMode.speed = CPSS_PORT_SPEED_40000_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        apStatusPtr->postApPortNum  = apResult.postApPortNum;
    }
    else
    {
        rc = prvCpssDxChPortPhysicalPortMapReverseMappingGet(devNum, 
            PRV_CPSS_DXCH_PORT_TYPE_MAC_E, apResult.postApPortNum,
            &apStatusPtr->postApPortNum);
        if(rc != GT_OK)
        {
            AP_DBG_PRINT_MAC(("prvCpssDxChPortPhysicalPortMapReverseMappingGet(devNum=%d,PRV_CPSS_DXCH_PORT_TYPE_MAC_E,apResult.postApPortNum=%d):rc=%d\n",
                      devNum, apResult.postApPortNum, rc));
            return rc;
        }
    }
    apStatusPtr->fcTxPauseEn = apResult.hcdResult.hcdFcTxPauseEn;
    apStatusPtr->fcRxPauseEn = apResult.hcdResult.hcdFcRxPauseEn;

    AP_DBG_PRINT_MAC(("apStatusPtr:fcTxPauseEn=%d,fcRxPauseEn=%d,fecEnabled=%d,hcdFound=%d,\
                        ifMode=%d,speed=%d,postApPortNum=%d\n",
                      apStatusPtr->fcTxPauseEn,apStatusPtr->fcRxPauseEn,apStatusPtr->
                      fecEnabled,apStatusPtr->hcdFound,
                      apStatusPtr->portMode.ifMode,apStatusPtr->portMode.speed,
                      apStatusPtr->postApPortNum));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortApPortStatusGet
*
* DESCRIPTION:
*       Get status of AP on port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       apStatusPtr - (ptr to) AP parameters for port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apStatusPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - AP engine not run
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApPortStatusGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_PORT_AP_STATUS_STC   *apStatusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApPortStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, apStatusPtr));

    rc = internal_cpssDxChPortApPortStatusGet(devNum, portNum, apStatusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, apStatusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortApResolvedPortsBmpGet
*
* DESCRIPTION:
*       Get bitmap of ports on port group (local core) where AP process finished
*       with agreed for both sides resolution
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
*       portGroupNum - number of port group (local core)
*
* OUTPUTS:
*       apResolvedPortsBmpPtr  - 1's set for ports of local core where AP
*                                   resolution acheaved
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port group or device
*       GT_BAD_PTR               - apResolvedPortsBmpPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApResolvedPortsBmpGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupNum,
    OUT GT_U32  *apResolvedPortsBmpPtr
)
{
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(apResolvedPortsBmpPtr);

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortResolutionMaskGet(devNum[%d], portGroupNum[%d], *apResolvedPortsBmpPtr)", devNum, portGroupNum);
    rc = mvHwsApPortResolutionMaskGet(devNum,portGroupNum,apResolvedPortsBmpPtr);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        AP_DBG_PRINT_MAC(("mvHwsApPortResolutionMaskGet:portGroupNum=%d,rc=%d\n",
                          portGroupNum, rc));
        return rc;
    }

    AP_DBG_PRINT_MAC(("cpssDxChPortApResolvedPortsBmpGet:devNum=%d,portGroupNum=%d,apResolvedPortsBmp=0x%x\n",
                      devNum, portGroupNum, *apResolvedPortsBmpPtr));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortApResolvedPortsBmpGet
*
* DESCRIPTION:
*       Get bitmap of ports on port group (local core) where AP process finished
*       with agreed for both sides resolution
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core)
*
* OUTPUTS:
*       apResolvedPortsBmpPtr  - 1's set for ports of local core where AP
*                                   resolution acheaved
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port group or device
*       GT_BAD_PTR               - apResolvedPortsBmpPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApResolvedPortsBmpGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupNum,
    OUT GT_U32  *apResolvedPortsBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApResolvedPortsBmpGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupNum, apResolvedPortsBmpPtr));

    rc = internal_cpssDxChPortApResolvedPortsBmpGet(devNum, portGroupNum, apResolvedPortsBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupNum, apResolvedPortsBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortApSetActiveMode
*
* DESCRIPTION:
*       Update port's AP active lanes according to new interface.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - number of physical port
*       ifMode    - interface mode
*       speed     - port data speed
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
static GT_STATUS internal_cpssDxChPortApSetActiveMode
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS               rc;        /* return code */
    GT_U32                  portGroup; /* local core number */
    GT_U32                  phyPortNum;/* port number in local core */
    MV_HWS_PORT_STANDARD    portMode;  /* port i/f mode and speed translated to
                                            BlackBox enum */

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E
                                          | CPSS_XCAT3_E | CPSS_BOBCAT3_E | CPSS_CAELUM_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    AP_DBG_PRINT_MAC(("cpssDxChPortApSetActiveMode:devNum=%d,portNum=%d,ifMode=%d,speed=%d\n", 
                        devNum, portNum, ifMode, speed));

    rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
    if(rc != GT_OK)
    {
        AP_DBG_PRINT_MAC(("cpssDxChPortApSetActiveMode(%d,%d,%d,%d):prvCpssLion2CpssIfModeToHwsTranslate:rc=%d\n",
                          devNum, portNum, ifMode, speed, rc));
        return rc;
    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortSetActiveLanes(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d])", devNum, portGroup, phyPortNum, portMode);
    rc = mvHwsApPortSetActiveLanes(devNum, portGroup, phyPortNum, portMode);
	if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
	}
    AP_DBG_PRINT_MAC(("mvHwsApPortSetActiveLanes(%d,%d,%d,%d):rc=%d\n",
                      devNum, portGroup, phyPortNum, portMode, rc));

    return rc;
}

/*******************************************************************************
* cpssDxChPortApSetActiveMode
*
* DESCRIPTION:
*       Update port's AP active lanes according to new interface.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - number of physical port
*       ifMode    - interface mode
*       speed     - port data speed
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
GT_STATUS cpssDxChPortApSetActiveMode
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApSetActiveMode);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ifMode, speed));

    rc = internal_cpssDxChPortApSetActiveMode(devNum, portNum, ifMode, speed);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ifMode, speed));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChPortApLock
*
* DESCRIPTION:
*       Acquires lock so host and AP machine won't access the same 
*		resource at the same time.
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
*
* OUTPUTS:
*       statePtr    - (ptr to) port state:
*                           GT_TRUE - locked by HOST - can be configured
*                           GT_FALSE - locked by AP processor - access forbidden
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_BAD_PTR           - statePtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApLock
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *statePtr
)
{
    GT_STATUS   rc;         /* return code */
	GT_U32	    portGroup;  /* local core number */
	GT_U32	    phyPortNum; /* number of port in local core */

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E
                                          | CPSS_XCAT3_E | CPSS_BOBCAT3_E | CPSS_CAELUM_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    if(apSemPrintEn)
    {
        cpssOsPrintf("cpssDxChPortApLock:devNum=%d,portNum=%d,", 
                            devNum, portNum);
    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    CPSS_LOG_INFORMATION_MAC("Calling: mvApLockGet(devNum[%d], portGroup[%d], phyPortNum[%d])", devNum, portGroup, phyPortNum);
    rc = mvApLockGet(devNum, portGroup, phyPortNum);
    *statePtr = (GT_OK == rc) ? GT_TRUE : GT_FALSE;
    if(apSemPrintEn)
    {
        cpssOsPrintf("state=%d\n", *statePtr);
    }
    if((GT_OK == rc) || (GT_NO_RESOURCE == rc))
    {
        return GT_OK;
    }
    else
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
    }
}

/*******************************************************************************
* cpssDxChPortApLock
*
* DESCRIPTION:
*       Acquires lock so host and AP machine won't access the same 
*		resource at the same time.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       statePtr    - (ptr to) port state:
*                           GT_TRUE - locked by HOST - can be configured
*                           GT_FALSE - locked by AP processor - access forbidden
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_BAD_PTR           - statePtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApLock
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApLock);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr));

    rc = internal_cpssDxChPortApLock(devNum, portNum, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortApUnLock
*
* DESCRIPTION:
*       Releases the synchronization lock (between Host and AP machine).
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
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApUnLock
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
	GT_STATUS	rc;
	GT_U32	    portGroup;  /* local core number */
	GT_U32	    phyPortNum; /* number of port in local core */

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E
                                          | CPSS_XCAT3_E | CPSS_BOBCAT3_E | CPSS_CAELUM_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    if(apSemPrintEn)
    {
        cpssOsPrintf("cpssDxChPortApUnLock:devNum=%d,portNum=%d\n", 
                            devNum, portNum);
    }
    CPSS_LOG_INFORMATION_MAC("Calling: mvApLockRelease(devNum[%d], portGroup[%d], phyPortNum[%d])", devNum, portGroup, phyPortNum);
    rc = mvApLockRelease(devNum, portGroup, phyPortNum);
	if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
    }
	return rc;
}

/*******************************************************************************
* cpssDxChPortApUnLock
*
* DESCRIPTION:
*       Releases the synchronization lock (between Host and AP machine).
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApUnLock
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApUnLock);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum));

    rc = internal_cpssDxChPortApUnLock(devNum, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChPortApHighLevelConfigurationSet
*
* DESCRIPTION:
*       Allocate/release high level port data path resources (pizza etc.) for
*       port where AP enabled, while physical port interface configuration made 
*       by HWS in co-processor.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       portCreate - GT_TRUE - port create;
*                    GT_FALSE - port delete;
*       ifMode    - interface mode
*       speed     - port data speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortApHighLevelConfigurationSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         portCreate,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed    
)
{
    GT_STATUS rc; /* return code */
    GT_U32    portMacMap; /* number of mac mapped to this physical port */
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    CPSS_PORT_INTERFACE_MODE_ENT *portIfModePtr;
    CPSS_PORT_SPEED_ENT          *portSpeedPtr;
            
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                 portNum,
                                                                 portMacMap);
        
    portIfModePtr = &(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacMap));
    portSpeedPtr = &(PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacMap));
    
    if(!portCreate)
    {
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

        rc = prvCpssDxChPortBcat2InterlakenTxDmaEnableSet(devNum, GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
        
        /* save new interface mode in DB */
        *portIfModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
        *portSpeedPtr = CPSS_PORT_SPEED_NA_E;
        
        return GT_OK;
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
        
    prvCpssDxChPortTypeSet(devNum, portMacMap, ifMode, speed);

    /* update addresses of mac registers accordingly to used MAC GE/XLG/etc. */
    rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, portNum, ifMode);
    if (rc != GT_OK)
    {
        return rc;
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

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortApStatsGet
*
* DESCRIPTION:
*       Returns the AP port statistics information.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       apStatsPtr - (ptr to) AP statistics information
*       intropAbilityMaxIntervalPtr - (ptr to) Introp Ability Max Interval parameter - will help to represent number of failed HCD cycles 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apEnablePtr or apParamsPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApStatsGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_PORT_AP_STATS_STC     *apStatsPtr,
    OUT GT_U16                          *intropAbilityMaxIntervalPtr
)
{
    GT_STATUS               rc;         /* return code */
    GT_U32                  portGroup;  /* local core number */
    GT_U32                  phyPortNum; /* port number in local core */
    MV_HWS_AP_PORT_STATS    apStats;    /* AP statistics information in HWS format*/
    MV_HWS_AP_PORT_INTROP   apIntrop;  /* AP port introp information in HWS format */

    PRV_CPSS_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT3_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,phyPortNum);
    CPSS_NULL_PTR_CHECK_MAC(apStatsPtr);
    CPSS_NULL_PTR_CHECK_MAC(intropAbilityMaxIntervalPtr);

    cpssOsMemSet(&apStats, 0, sizeof(MV_HWS_AP_PORT_STATS));
    cpssOsMemSet(&apIntrop, 0, sizeof(MV_HWS_AP_PORT_INTROP));

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if (PRV_CPSS_GE_PORT_GE_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, phyPortNum))
    {
        return GT_OK;
    }

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlStatsGet(devNum[%d], portGroup[%d], phyPortNum[%d])", devNum, portGroup, phyPortNum);
    rc = mvHwsApPortCtrlStatsGet(devNum, portGroup, phyPortNum, &apStats);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("mvHwsApPortCtrlStatsGet: Hws return code is %d", rc);
        return rc;
    }

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlIntropGet(devNum[%d], portGroup[%d], phyPortNum[%d])", devNum, portGroup, phyPortNum);
    rc = mvHwsApPortCtrlIntropGet(devNum,portGroup,phyPortNum,&apIntrop);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("mvHwsApPortCtrlIntropGet:Hws return code is %d", rc);
        return rc;
    }

    *intropAbilityMaxIntervalPtr = apIntrop.abilityMaxInterval;

    apStatsPtr->txDisCnt = apStats.txDisCnt;
    apStatsPtr->abilityCnt = apStats.abilityCnt;
    apStatsPtr->abilitySuccessCnt = apStats.abilitySuccessCnt;
    apStatsPtr->linkFailCnt = apStats.linkFailCnt;
    apStatsPtr->linkSuccessCnt = apStats.linkSuccessCnt;
    apStatsPtr->hcdResoultionTime = apStats.hcdResoultionTime;
    apStatsPtr->linkUpTime = apStats.linkUpTime;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortApStatsGet
*
* DESCRIPTION:
*       Returns the AP port statistics information.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       apStatsPtr - (ptr to) AP statistics information
*       intropAbilityMaxIntervalPtr - (ptr to) Introp Ability Max Interval parameter - will help to represent number of failed HCD cycles 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apEnablePtr or apParamsPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApStatsGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_PORT_AP_STATS_STC     *apStatsPtr,
    OUT GT_U16                          *intropAbilityMaxIntervalPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApStatsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, apStatsPtr));

    rc = internal_cpssDxChPortApStatsGet(devNum, portNum, apStatsPtr, intropAbilityMaxIntervalPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, apStatsPtr, intropAbilityMaxIntervalPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortApStatsReset
*
* DESCRIPTION:
*       Reset AP port statistics information
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortApStatsReset
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
	GT_STATUS	rc;
	GT_U32	    portGroup;  /* local core number */
	GT_U32	    phyPortNum; /* number of port in local core */

    PRV_CPSS_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT3_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,phyPortNum);

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortStatsReset(devNum[%d], portGroup[%d], phyPortNum[%d])", devNum, portGroup, phyPortNum);

    rc = mvHwsApPortCtrlStatsReset(devNum, portGroup, phyPortNum);

	if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
    }
	return rc;
}

/*******************************************************************************
* cpssDxChPortApStatsReset
*
* DESCRIPTION:
*       Reset AP port statistics information
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApStatsReset
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApStatsReset);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum));

    rc = internal_cpssDxChPortApStatsReset(devNum, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortApIntropSet
*
* DESCRIPTION:
*       Set AP port introp information
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*       apIntropPtr - (ptr to) AP introp parameters
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
static GT_STATUS internal_cpssDxChPortApIntropSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_DXCH_PORT_AP_INTROP_STC    *apIntropPtr
)
{
    GT_STATUS               rc;        /* return code */
    GT_U32                  portGroup; /* local core number */
    GT_U32                  phyPortNum;/* port number in local core */
    MV_HWS_AP_PORT_INTROP   apIntrop;  /* AP port introp information in HWS format */

    PRV_CPSS_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT3_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,phyPortNum);
    CPSS_NULL_PTR_CHECK_MAC(apIntropPtr);

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    apIntrop.attrBitMask = apIntropPtr->attrBitMask;
    apIntrop.txDisDuration = apIntropPtr->txDisDuration;
    apIntrop.abilityDuration = apIntropPtr->abilityDuration;
    apIntrop.abilityMaxInterval = apIntropPtr->abilityMaxInterval;
    apIntrop.abilityFailMaxInterval = apIntropPtr->abilityFailMaxInterval;
    apIntrop.apLinkDuration = apIntropPtr->apLinkDuration;
    apIntrop.apLinkMaxInterval = apIntropPtr->apLinkMaxInterval;
    apIntrop.pdLinkDuration = apIntropPtr->pdLinkDuration;
    apIntrop.pdLinkMaxInterval = apIntropPtr->pdLinkMaxInterval;

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlIntropSet(devNum[%d], portGroup[%d], phyPortNum[%d])", devNum, portGroup, phyPortNum);

    rc = mvHwsApPortCtrlIntropSet(devNum,portGroup,phyPortNum,&apIntrop);

    if(rc != GT_OK)
	{
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
	}

    return rc;

}

/*******************************************************************************
* cpssDxChPortApIntropSet
*
* DESCRIPTION:
*       Set AP port introp information
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*       apIntropPtr - (ptr to) AP introp parameters
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
GT_STATUS cpssDxChPortApIntropSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_DXCH_PORT_AP_INTROP_STC    *apIntropPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApIntropSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, apIntropPtr));

    rc = internal_cpssDxChPortApIntropSet(devNum, portNum, apIntropPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, apIntropPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortApIntropGet
*
* DESCRIPTION:
*       Returns AP port introp information
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*       
* OUTPUTS:
*       apIntropPtr - (ptr to) AP introp parameters
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
static GT_STATUS internal_cpssDxChPortApIntropGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT  CPSS_DXCH_PORT_AP_INTROP_STC    *apIntropPtr
)
{
    GT_STATUS               rc;        /* return code */
    GT_U32                  portGroup; /* local core number */
    GT_U32                  phyPortNum;/* port number in local core */
    MV_HWS_AP_PORT_INTROP   apIntrop;  /* AP port introp information in HWS format */

    PRV_CPSS_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                          | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT3_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,phyPortNum);
    CPSS_NULL_PTR_CHECK_MAC(apIntropPtr);

    cpssOsMemSet(&apIntrop, 0, sizeof(MV_HWS_AP_PORT_INTROP));

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsApPortCtrlIntropGet(devNum[%d], portGroup[%d], phyPortNum[%d])", devNum, portGroup, phyPortNum);

    rc = mvHwsApPortCtrlIntropGet(devNum,portGroup,phyPortNum,&apIntrop);

    if(rc != GT_OK)
	{
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
	}

    apIntropPtr->attrBitMask = apIntrop.attrBitMask;
    apIntropPtr->txDisDuration = apIntrop.txDisDuration;
    apIntropPtr->abilityDuration = apIntrop.abilityDuration;
    apIntropPtr->abilityMaxInterval = apIntrop.abilityMaxInterval;
    apIntropPtr->abilityFailMaxInterval = apIntrop.abilityFailMaxInterval;
    apIntropPtr->apLinkDuration = apIntrop.apLinkDuration;
    apIntropPtr->apLinkMaxInterval = apIntrop.apLinkMaxInterval;
    apIntropPtr->pdLinkDuration = apIntrop.pdLinkDuration;
    apIntropPtr->pdLinkMaxInterval = apIntrop.pdLinkMaxInterval;

    return rc;

}

/*******************************************************************************
* cpssDxChPortApIntropGet
*
* DESCRIPTION:
*       Returns AP port introp information
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*
*
* OUTPUTS:
*       apIntropPtr - (ptr to) AP introp parameters
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
GT_STATUS cpssDxChPortApIntropGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT  CPSS_DXCH_PORT_AP_INTROP_STC    *apIntropPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortApIntropGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, apIntropPtr));

    rc = internal_cpssDxChPortApIntropGet(devNum, portNum, apIntropPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, apIntropPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



