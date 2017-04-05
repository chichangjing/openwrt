/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssInit.c
*
* DESCRIPTION:
*       Initialization function for PP's , regardless to PP's types.
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/init/private/prvCpssGenInitLog.h>
#include <cpss/generic/init/cpssInit.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/generic/private/prvCpssGlobalMutex.h>

#if ( ((defined EX_FAMILY) || (defined MX_FAMILY)) && !(defined CPSS_PRODUCT))
#   include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#endif /* (defined EX_FAMILY) || (defined MX_FAMILY) */

#define DEBUG_PRINT(x) cpssOsPrintf x

#define PRINT_FORMAT_d_MAC(field)    \
    " %s = [%d] \n",#field,field

#define PRINT_FORMAT_8_8x_MAC(field)    \
    " %s = [0x%8.8x] \n",#field,field

#define PRINT_FORMAT_4_4x_MAC(field)    \
    " %s = [0x%4.4x] \n",#field,field

/*******************************************************************************
* cpssPpInit
*
* DESCRIPTION:
*       This function initialize the internal DB of the CPSS regarding PPs.
*       This function must be called before any call to a PP functions ,
*       i.e before calling cpssExMxHwPpPhase1Init.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpInit
(
    void
)
{
    GT_STATUS rc;

    /************************************/
    /* initialize the cpss global mutex */
    /************************************/
    rc = cpssGlobalMtxInit();
    if(rc != GT_OK)
    {
        return rc;
    }

    /***********************/
    /* initialize the cpss */
    /***********************/
    rc = prvCpssSysConfigPhase1();
    if(rc != GT_OK)
    {
        return rc;
    }

    /*****************************/
    /* initialize the cpssDriver */
    /*****************************/
    rc = prvCpssDrvSysConfigPhase1();
    if(rc != GT_OK)
    {
        return rc;
    }

    sysGenGlobalInfo.cpssPpInitCalled = GT_TRUE;

    /* Disable the combining writes / read feature in the system controller */
    rc = cpssExtDrvEnableCombinedPciAccess(GT_FALSE,GT_FALSE);

    return rc;
}

/*******************************************************************************
* cpssPpDestroy
*
* DESCRIPTION:
*       This function destroy the internal DB of the CPSS regarding PPs.
*       This function should be called after all devices have been removed from
*       the CPSS and system need to 'clean up' before restart initialization again.
*       so function must be called before recalling cpssPpInit(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpDestroy
(
    void
)
{
    GT_STATUS   rc;

    /* explicit call to CPSS API protection lock to avoid deleting it while it still in use  */
    /* by other API. */
    CPSS_API_LOCK_MAC(0, 0);

    /**************************/
    /* destroy the cpssDriver */
    /**************************/
    /* already done in prvCpssDrvHwPpDevRemove(...) when last device removed */

    /********************/
    /* destroy the cpss */
    /********************/
    rc = prvCpssSysConfigDestroy();
    if(rc != GT_OK)
    {
        return rc;
    }

#if 0 /* we can not destroy this semaphore as it is used by CMD engine to lock
        any access to galtisWrappers/extern function.
        so destroying it will cause the caller of lock/unlock to:
        1. lock 'old' mutex
        2. call cpss to 'System reset' -->
            a. destroy 'old' mutex
            b. create 'new' mutex
        3. unlock 'new' mutex !!!  --> ERROR !!!
      */
    /*********************************/
    /* destroy the cpss global mutex */
    /*********************************/
    (void)cpssGlobalMtxDelete();
#endif /*0*/

    sysGenGlobalInfo.cpssPpInitCalled = GT_FALSE;

    return GT_OK;
}

extern GT_STATUS prvCpssDrvPpDump(GT_U8   devNum);

/*******************************************************************************
* internal_cpssPpDump
*
* DESCRIPTION:
*       Dump function , to print the info on a specific PP.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device Number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssPpDump
(
    IN GT_U8   devNum
)
{
    PRV_CPSS_GEN_PP_CONFIG_STC      *devInfoPtr;/* pointer to device info */
    PRV_CPSS_PORT_INFO_ARRAY_STC    *portInfoPtr;/* pointer to current port info */
    GT_U8   port;/* port iterator*/

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        DEBUG_PRINT(("bad device number [%d] \n",devNum));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    DEBUG_PRINT(("\n"));
    DEBUG_PRINT(("start Info about device number [%d]: \n",devNum));

    devInfoPtr = PRV_CPSS_PP_MAC(devNum);

    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->devNum)));
    DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->functionsSupportedBmp)));

    DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->devType)));
    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->revision)));
    DEBUG_PRINT((" devFamily = [%s] \n",
        devInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTC_E    ? " CPSS_PP_FAMILY_TWISTC_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTD_E    ? " CPSS_PP_FAMILY_TWISTD_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_SAMBA_E     ? " CPSS_PP_FAMILY_SAMBA_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_SALSA_E     ? " CPSS_PP_FAMILY_SALSA_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E   ? " CPSS_PP_FAMILY_CHEETAH_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E  ? " CPSS_PP_FAMILY_CHEETAH2_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH3_E  ? " CPSS_PP_FAMILY_CHEETAH3_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E  ? " CPSS_PP_FAMILY_DXCH_XCAT_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E  ? " CPSS_PP_FAMILY_DXCH_XCAT3_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E ? " CPSS_PP_FAMILY_DXCH_XCAT2_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E  ? " CPSS_PP_FAMILY_DXCH_LION_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E  ? " CPSS_PP_FAMILY_DXCH_LION2_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E  ? " CPSS_PP_FAMILY_DXCH_BOBCAT2_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E  ? " CPSS_PP_FAMILY_DXCH_BOBCAT3_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E  ? " CPSS_PP_FAMILY_DXCH_ALDRIN_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_TIGER_E     ? " CPSS_PP_FAMILY_TIGER_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_PUMA_E     ? " CPSS_PP_FAMILY_PUMA_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_PUMA3_E    ? " CPSS_PP_FAMILY_PUMA3_E ":

        " unknown "
    ));

    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->numOfPorts)));
    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->numOfVirtPorts)));

    DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->existingPorts.ports[0])));
    if(devInfoPtr->numOfPorts > (32*1))
        DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->existingPorts.ports[1])));
    if(devInfoPtr->numOfPorts > (32*2))
        DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->existingPorts.ports[2])));
    if(devInfoPtr->numOfPorts > (32*3))
        DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->existingPorts.ports[3])));
    if(devInfoPtr->numOfPorts > (32*4))
        DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->existingPorts.ports[4])));
    if(devInfoPtr->numOfPorts > (32*5))
        DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->existingPorts.ports[5])));
    if(devInfoPtr->numOfPorts > (32*6))
        DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->existingPorts.ports[6])));
    if(devInfoPtr->numOfPorts > (32*7))
        DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->existingPorts.ports[7])));

    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->baseCoreClock)));
    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->coreClock)));
    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->hwDevNum)));


    DEBUG_PRINT((" mngInterfaceType = [%s] \n",
        PRV_CPSS_HW_IF_PEX_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_PEX_E " :
        PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_PCI_E " :
        PRV_CPSS_HW_IF_SMI_COMPATIBLE_MAC(devNum)  ? " CPSS_CHANNEL_SMI_E " :
        PRV_CPSS_HW_IF_TWSI_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_TWSI_E " :
        " unknown "
    ));

    DEBUG_PRINT((" configInitAfterHwResetDone = [%s] \n",
            devInfoPtr->configInitAfterHwResetDone == GT_TRUE   ? " GT_TRUE " :
            devInfoPtr->configInitAfterHwResetDone == GT_FALSE  ? " GT_FALSE " :
            " unknown "
    ));

    DEBUG_PRINT((" ppHAState = [%s] \n",
            devInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E  ? " CPSS_SYS_HA_MODE_ACTIVE_E " :
            devInfoPtr->ppHAState == CPSS_SYS_HA_MODE_STANDBY_E ? " CPSS_SYS_HA_MODE_STANDBY_E " :
            " unknown "
    ));


    if(devInfoPtr->phyPortInfoArray == NULL)
    {
        DEBUG_PRINT((" devInfoPtr->phyPortInfoArray ERROR -- NULL POINTER !!! \n"));
    }
    else
    {
        portInfoPtr = &devInfoPtr->phyPortInfoArray[0];
        for (port = 0 ; port < devInfoPtr->numOfPorts ; port++ , portInfoPtr++)
        {
            DEBUG_PRINT((PRINT_FORMAT_d_MAC(port)));

            DEBUG_PRINT((PRINT_FORMAT_d_MAC(portInfoPtr->portFcParams.xonLimit)));
            DEBUG_PRINT((PRINT_FORMAT_d_MAC(portInfoPtr->portFcParams.xoffLimit)));

            DEBUG_PRINT((" portInfoPtr->portType = [%s] \n",
                        portInfoPtr->portType == PRV_CPSS_PORT_NOT_EXISTS_E ? " PRV_CPSS_PORT_NOT_EXISTS_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_FE_E         ? " PRV_CPSS_PORT_FE_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_GE_E         ? " PRV_CPSS_PORT_GE_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_XG_E         ? " PRV_CPSS_PORT_XG_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_XLG_E         ? " PRV_CPSS_PORT_XLG_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_HGL_E         ? " PRV_CPSS_PORT_HGL_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_CG_E         ? " PRV_CPSS_PORT_CG_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_NOT_APPLICABLE_E         ? " PRV_CPSS_PORT_NOT_APPLICABLE_E " :
            " unknown "
            ));

            DEBUG_PRINT((PRINT_FORMAT_4_4x_MAC(portInfoPtr->smiIfInfo.smiPortAddr)));
            DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(portInfoPtr->smiIfInfo.smiCtrlRegAddr)));
        }

        DEBUG_PRINT(("\n"));
        DEBUG_PRINT(("============== end of per port info =========== \n"));
        DEBUG_PRINT(("\n"));
    }

    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->useIsr)));
    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->cpuPortMode)));
    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->netifSdmaPortGroupId)));
    DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->maxIterationsOfBusyWait)));

    if(devInfoPtr->portGroupsInfo.isMultiPortGroupDevice != GT_FALSE)
    {
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.isMultiPortGroupDevice)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.numOfPortGroups)));
        DEBUG_PRINT((PRINT_FORMAT_8_8x_MAC(devInfoPtr->portGroupsInfo.activePortGroupsBmp)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.firstActivePortGroup)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.lastActivePortGroup)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.cpuPortPortGroupId)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.loopbackPortGroupId)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.auqPortGroupIdLastServed)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.fuqPortGroupIdLastServed)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.secureBreachNextPortGroupToServe)));
        DEBUG_PRINT((PRINT_FORMAT_d_MAC(devInfoPtr->portGroupsInfo.ptpNextPortGroupToServe)));
    }


    /* print more specific info */
#if ( ((defined EX_FAMILY) || (defined MX_FAMILY)) && !(defined CPSS_PRODUCT))
    {
        PRV_CPSS_EXMX_PP_CONFIG_STC  *exMxDevInfoPtr;

        DEBUG_PRINT(("\n"));

        DEBUG_PRINT((" This is ExMx device \n"));

        exMxDevInfoPtr = PRV_CPSS_EXMX_PP_MAC(devNum);

        DEBUG_PRINT((" exMxDevInfoPtr->uplinkCfg = [%s] \n",
            exMxDevInfoPtr->uplinkCfg == CPSS_PP_UPLINK_BACK_TO_BACK_E  ? " CPSS_PP_UPLINK_BACK_TO_BACK_E " :
            exMxDevInfoPtr->uplinkCfg == CPSS_PP_UPLINK_FA_E            ? " CPSS_PP_UPLINK_FA_E " :
            exMxDevInfoPtr->uplinkCfg == CPSS_PP_NO_UPLINK_E            ? " CPSS_PP_NO_UPLINK_E " :
            " unknown "
            ));
    }
#endif /* (defined EX_FAMILY) || (defined MX_FAMILY) */

    DEBUG_PRINT(("finished Info about device number [%d]: \n",devNum));
    DEBUG_PRINT(("\n"));
    DEBUG_PRINT(("\n"));

    /* call the driver to print it's info */
    prvCpssDrvPpDump(devNum);

    return GT_OK;

}

/*******************************************************************************
* cpssPpDump
*
* DESCRIPTION:
*       Dump function , to print the info on a specific PP.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device Number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpDump
(
    IN GT_U8   devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssPpDump);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssPpDump(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssSystemDualDeviceIdModeEnableSet
*
* DESCRIPTION:
*       This function declare 'This system support dual device Id'.
*       The function MUST be called for any system with Lion2 ('Dual device Id' device).
*       This means that application MUST not use 'odd' hwDevNum to any device in the system.
*       Allowed to use only : 0,2,4..30 (also for ch1,2,3,xcat1,2,Lion in this system)
*
*       For such system this function must be called before any call to a PP functions ,
*       i.e before calling cpssDxChHwPpPhase1Init(...).
*
*       The function MUST be called only after cpssPpInit(...)
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       enable  - indication that the system is in 'dual deviceId' mode.
*               GT_TRUE - This system support dual device Id
*               GT_FALSE - This system NOT support dual device Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_STATE             - the function called before call to cpssPpInit(...).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssSystemDualDeviceIdModeEnableSet
(
    IN GT_BOOL  enable
)
{
    if(sysGenGlobalInfo.cpssPpInitCalled == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    sysGenGlobalInfo.supportDualDeviceId = enable;

    return GT_OK;
}

/*******************************************************************************
* cpssSystemDualDeviceIdModeEnableSet
*
* DESCRIPTION:
*       This function declare 'This system support dual device Id'.
*       The function MUST be called for any system with Lion2 ('Dual device Id' device).
*       This means that application MUST not use 'odd' hwDevNum to any device in the system.
*       Allowed to use only : 0,2,4..30 (also for ch1,2,3,xcat1,2,Lion in this system)
*
*       For such system this function must be called before any call to a PP functions ,
*       i.e before calling cpssDxChHwPpPhase1Init(...).
*
*       The function MUST be called only after cpssPpInit(...)
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       enable  - indication that the system is in 'dual deviceId' mode.
*               GT_TRUE - This system support dual device Id
*               GT_FALSE - This system NOT support dual device Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_STATE             - the function called before call to cpssPpInit(...).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSystemDualDeviceIdModeEnableSet
(
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssSystemDualDeviceIdModeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, enable));

    rc = internal_cpssSystemDualDeviceIdModeEnableSet(enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssSystemDualDeviceIdModeEnableGet
*
* DESCRIPTION:
*       This function retrieve if 'This system support dual device Id'.
*       This means that application MUST not use 'odd' hwDevNum to any device in the system.
*       Allowed to use only : 0,2,4..30 (also for ch1,2,3,xcat1,2,Lion in this system)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       enablePtr  - (pointer to)indication that the system is in 'dual deviceId' mode.
*               GT_TRUE - This system support dual device Id
*               GT_FALSE - This system NOT support dual device Id
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssSystemDualDeviceIdModeEnableGet
(
    OUT GT_BOOL  *enablePtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    *enablePtr = sysGenGlobalInfo.supportDualDeviceId;

    return GT_OK;
}

/*******************************************************************************
* cpssSystemDualDeviceIdModeEnableGet
*
* DESCRIPTION:
*       This function retrieve if 'This system support dual device Id'.
*       This means that application MUST not use 'odd' hwDevNum to any device in the system.
*       Allowed to use only : 0,2,4..30 (also for ch1,2,3,xcat1,2,Lion in this system)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       enablePtr  - (pointer to)indication that the system is in 'dual deviceId' mode.
*               GT_TRUE - This system support dual device Id
*               GT_FALSE - This system NOT support dual device Id
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSystemDualDeviceIdModeEnableGet
(
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssSystemDualDeviceIdModeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, enablePtr));

    rc = internal_cpssSystemDualDeviceIdModeEnableGet(enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

