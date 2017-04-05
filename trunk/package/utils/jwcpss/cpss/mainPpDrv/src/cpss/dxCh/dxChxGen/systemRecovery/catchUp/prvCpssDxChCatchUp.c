/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCatchUp.c
*
* DESCRIPTION:
*       CPSS DxCh CatchUp functions.
*
* FILE REVISION NUMBER:
*       $Revision: 19 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/dxCh/dxChxGen/systemRecovery/catchUp/private/prvCpssDxChCatchUp.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChCfg.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbAu.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/generic/private/utils/prvCpssUnitGenArrDrv.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgResource.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* Dummy value to indicate that we are during catchup validity check. Used for debug purpose to
   verify that cathcup changed the value of this variable */
#define PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS     0xABCDABCD

static PRV_CPSS_DXCH_AUQ_ENABLE_DATA_STC *auqMsgEnableStatus[PRV_CPSS_MAX_PP_DEVICES_CNS] = {NULL};

static prvCpssCatchUpFuncPtr  catchUpFuncPtrArray[] = {
                                                prvCpssDxChHwDevNumCatchUp,prvCpssDxChCpuPortModeCatchUp,
                                                prvCpssDxChDevTableCatchUp,prvCpssDxChCpuSdmaPortGroupCatchUp,
                                                prvCpssDxChFdbHashParamsModeCatchUp,prvCpssDxChFdbActionHwDevNumActionHwDevNumMaskCatchUp,
                                                prvCpssDxChPrePendTwoBytesCatchUp, prvCpssDxChPolicerMemorySizeModeCatchUp,
                                                prvCpssDxChSecurBreachPortDropCntrModeAndPortGroupCatchUp,prvCpssDxChPortEgressCntrModeCatchUp,
                                                prvCpssDxChBridgeIngressCntrModeCatchUp,prvCpssDxChBridgeIngressDropCntrModeCatchUp,
                                                prvCpssDxChBridgeTag1VidFdbEnCatchUp,prvCpssDxChBridgeEgrVlanPortFltTabAccessModeCatchUp,
                                                prvCpssDxChPortModeParamsCatchUp,prvCpssDxChPortResourcesCatchUp,
                                                NULL
                                                };

typedef struct
{
    GT_HW_DEV_NUM                                   hwDevNum;
    CPSS_NET_CPU_PORT_MODE_ENT                      cpuPortMode;
    CPSS_MAC_VL_ENT                                 vlanMode;
    CPSS_MAC_HASH_FUNC_MODE_ENT                     hashMode;
    GT_U32                                          actionHwDevNum;
    GT_U32                                          actionHwDevNumMask;
    GT_BOOL                                         prePendTwoBytesHeader;
    GT_U32                                          policerInfoMemSize[2];
    CPSS_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT       securBreachDropCounterInfoCounterMode;
    GT_U32                                          securBreachDropCounterInfoPortGroupId;
    GT_U32                                          portEgressCntrModeInfoPortGroupId[2];
    GT_U32                                          bridgeIngressCntrModePortGroupId[2];
    GT_U32                                          cfgIngressDropCntrModePortGroupId;
    CPSS_PORT_INTERFACE_MODE_ENT                    portIfMode[CPSS_MAX_PORTS_NUM_CNS];
    CPSS_PORT_SPEED_ENT                             portSpeed[CPSS_MAX_PORTS_NUM_CNS];
    PRV_CPSS_PORT_TYPE_ENT                          portType[CPSS_MAX_PORTS_NUM_CNS];

    /* Lion, Lion2 */
    GT_U32                                          netifSdmaPortGroupId;

    /* xCat, xCat2, xCat3, Lion, Lion2 */
    GT_U32                                          devTable;

    /* SIP 5 */
    GT_BOOL                                         tag1VidFdbEn;
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT portVlanfltTabAccessMode;
    GT_U32                                          usedDescCredits[MAX_DP_CNS];
    GT_U32                                          usedHeaderCredits[MAX_DP_CNS];
    GT_U32                                          usedPayloadCredits[MAX_DP_CNS];
} PRV_CPSS_DXCH_CATCHUP_PARAMS_STC;

static PRV_CPSS_DXCH_CATCHUP_PARAMS_STC ppConfigCatchUpParams;

GT_STATUS prvCpssDxChNetIfPrePendTwoBytesHeaderFromHwGet
(
    IN  GT_U8        devNum,
    OUT  GT_BOOL    *enablePtr
);

/*******************************************************************************
* prvCpssDxChHwDevNumCatchUp
*
* DESCRIPTION:
*       Synchronize hw device number in software DB by its hw value
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwDevNumCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc = GT_OK;
    GT_HW_DEV_NUM hwDevNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    rc =  cpssDxChCfgHwDevNumGet(devNum,&hwDevNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* save HW devNum to the DB */
    PRV_CPSS_HW_DEV_NUM_MAC(devNum) = hwDevNum;
    return rc;
}

/*******************************************************************************
* prvCpssDxChCpuPortModeCatchUp
*
* DESCRIPTION:
*       Synchronize cpu port mode in software DB by its hw value
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCpuPortModeCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 regAddr;
    GT_U32 regValue = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* no such bit as there is no special MAC for CPU */
        regValue = 1;/*sdma*/
    }
    else
    {
        if (PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum) == GT_TRUE)
        {
            /* there is erratum : SDMA emulation*/
            regValue = 1; /* sdma*/
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
            rc = prvCpssHwPpGetRegField(devNum,regAddr,20,1,&regValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    switch(regValue)
    {
        case 0:
            PRV_CPSS_PP_MAC(devNum)->cpuPortMode = CPSS_NET_CPU_PORT_MODE_MII_E;
            break;
        case 1:
            PRV_CPSS_PP_MAC(devNum)->cpuPortMode = CPSS_NET_CPU_PORT_MODE_SDMA_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChCpuSdmaPortGroupCatchUp
*
* DESCRIPTION:
*       Synchronize cpu sdma port group in software DB by its hw value
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCpuSdmaPortGroupCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc;
    GT_U32 regAddr;
    GT_U32 regValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(!PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        return GT_OK;
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            /* Not used any more */
            return GT_OK;
        }
        else
        {
            /* bit 0 - <CpuPortMode> set to 0 'global mode' */
            /* bits 1..4 -  <CpuTargetCore> set to the 'SDMA_PORT_GROUP_ID'  */
            rc = prvCpssHwPpGetRegField(devNum,
                    PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.cpuPortDist,
                    0,5,
                    &regValue);
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.global.cpuPortDistribution;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,0,3,&regValue);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    if ((regValue & 0x1) == 0)
    {
        /* Global CPU port mode */
        /* update device DB */
        PRV_CPSS_PP_MAC(devNum)->netifSdmaPortGroupId = regValue >> 1;
    }
    else
    {
        /* cpss doesn't support Local CPU port mode */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChDevTableCatchUp
*
* DESCRIPTION:
*       Synchronize device table BMP in software DB by its hw value
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDevTableCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    devTableBmp;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return GT_OK;
    }

    rc =  cpssDxChBrgFdbDeviceTableGet(devNum,&devTableBmp);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* save device table to the DB */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.devTable = devTableBmp;
    return rc;
}

/*******************************************************************************
* prvCpssDxChFdbHashParamsModeCatchUp
*
* DESCRIPTION:
*       Synchronize fdb hash params in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFdbHashParamsModeCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc = GT_OK;
    CPSS_MAC_VL_ENT vlanMode;
    CPSS_MAC_HASH_FUNC_MODE_ENT hashMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    rc =  cpssDxChBrgFdbMacVlanLookupModeGet(devNum,&vlanMode);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = cpssDxChBrgFdbHashModeGet(devNum,&hashMode);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* Update FDB hash parameters */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode = vlanMode;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode = hashMode;
    return rc;
}

/*******************************************************************************
* prvCpssDxChFdbActionHwDevNumActionHwDevNumMaskCatchUp
*
* DESCRIPTION:
*       Synchronize Active device number and active device number mask
*       in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFdbActionHwDevNumActionHwDevNumMaskCatchUp
(
    IN GT_U8    devNum
)
{
    GT_U32   actDev;
    GT_U32   actDevMask;
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    rc =  cpssDxChBrgFdbActionActiveDevGet(devNum,&actDev,&actDevMask);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*
     * To avoid using bit 1 from PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS
     * inside cpssDxChBrgFdbActionActiveDevGet()
     */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNum == PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS &&
            CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* actDev &= (GT_U32)-1-1; */
        actDev = ppConfigCatchUpParams.actionHwDevNum;
        actDevMask = ppConfigCatchUpParams.actionHwDevNumMask;
    }

    /* update data */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNum = actDev;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNumMask = actDevMask;
    return rc;
}

/*******************************************************************************
* prvCpssDxChPrePendTwoBytesCatchUp
*
* DESCRIPTION:
*       Synchronize enable/disable pre-pending a two-byte header
*       in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPrePendTwoBytesCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc;
    GT_BOOL  readValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* get the value from the HW */
    rc = prvCpssDxChNetIfPrePendTwoBytesHeaderFromHwGet(devNum,&readValue);

    if(rc == GT_OK)
    {
        /* save info to the DB */
        PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader = BIT2BOOL_MAC(readValue);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChPolicerMemorySizeModeCatchUp
*
* DESCRIPTION:
*       Synchronize Policer Memory Size Mode in software DB by its hw values.
*
* APPLICABLE DEVICES:
*        Lion;  Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*         DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPolicerMemorySizeModeCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      iplr0MemSize;   /* IPLR 0 memory size                */
    GT_U32      iplr1MemSize;   /* IPLR 1 memory size                */
    CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT     mode;
    GT_U32      ram0Size;       /* RAM#0 size, used for device with
                                   3 memories like xCat2, Bobcat2, Caelum and Bobcat3 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            ram0Size = 172;
            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
             ram0Size = _8K;
            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            if(PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
            {
                /*
                    The partitions in BOBK are:
                    Memory 0 : 4888
                    Memory 1 : 1000
                    Memory 2 : 256
                */
                ram0Size = 4888;
            }
            else
            {
                ram0Size = _4K;
            }
            break;
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
            /*
                The partitions in Aldrin are:
                Memory 0 : 1536
                Memory 1 : 256
                Memory 2 : 256
            */
            CPSS_TBD_BOOKMARK_ALDRIN /* need to check code for "Memory 1 : 256" */
            ram0Size = 1536;
            break;
        default:
            ram0Size = _4K;
            break;
    }

    rc =  cpssDxChPolicerMemorySizeModeGet(devNum,&mode);
    if (rc != GT_OK)
    {
        return rc;
    }
    switch (mode)
    {
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E:
            iplr1MemSize = MIN(256, PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum);
            iplr0MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum - iplr1MemSize;
            break;
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E:
            iplr1MemSize = 0;
            iplr0MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum;
            break;
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E:
            /* Check that the second stage is supported, if not, then memory
               size of the single stage will become 0. */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->
                fineTuning.featureInfo.iplrSecondStageSupported == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            iplr0MemSize = 0;
            iplr1MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum;
            break;
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E:
            iplr0MemSize = MIN(256, PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum);
            iplr1MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum - iplr0MemSize;
            break;
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_4_E:
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->
                fineTuning.featureInfo.iplrSecondStageSupported == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) ||
                (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
            {
                iplr0MemSize = MIN(ram0Size, PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum);
                iplr1MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum - iplr0MemSize;
                break;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_5_E:
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->
                fineTuning.featureInfo.iplrSecondStageSupported == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) ||
                (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE))
            {
                iplr1MemSize = MIN(ram0Size, PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum);
                iplr0MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum - iplr1MemSize;
                break;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* update memory size in the policer db */
    PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[0] = iplr0MemSize;
    PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[1] = iplr1MemSize;
    return rc;
}

/*******************************************************************************
* prvCpssDxChSecurBreachPortDropCntrModeAndPortGroupCatchUp
*
* DESCRIPTION:
*       Synchronize Secure Breach Port Drop Counter Mode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSecurBreachPortDropCntrModeAndPortGroupCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc = GT_OK;             /* return code */
    GT_U32      regAddr;                /* hw register address */
    GT_U32      portGroupId;            /*the port group Id - support multi-port-groups device */
    GT_U32      regValue = 0;
    GT_U32      fieldOffset;            /* The start bit number in the register */
    GT_U32      fieldLength;            /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig1;
        fieldOffset = 17;
    }
    else
    {
        /* get address of Bridge Configuration Register1 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[1];
            fieldOffset = 19;
    }

    /* called without portGroupId , loop done inside the driver */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &regValue);
    if (rc != GT_OK)
        return rc;
    if (regValue  & 0x1)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.counterMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E;
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }
    else
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.counterMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E;

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                        bridgeEngineConfig.bridgeSecurityBreachDropCntrCfg0;
            fieldOffset = 0;
            fieldLength = 13;
        }
        else
        {
            fieldOffset = 1;
            fieldLength = 6;
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, fieldOffset, fieldLength, &regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
            if (regValue != PRV_CPSS_DXCH_NULL_PORT_NUM_CNS)
            {
                PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.portGroupId = portGroupId;
                break;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortEgressCntrModeCatchUp
*
* DESCRIPTION:
*       Synchronize Port Egress Counters Mode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortEgressCntrModeCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32 regAddr;
    GT_U32 value;
    GT_U32  i;
    GT_U32  cntrSetNum;
    GT_U32 portValue = 0;
    GT_U32 globalPort = 0;
    GT_U32 portGroupId = 0;
    GT_U32 txqNum = 0;
    GT_U32 *regValuePtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    txqNum = PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(devNum);
    regValuePtr = (GT_U32 *)cpssOsMalloc(sizeof(GT_U32) * txqNum);
    if (regValuePtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    cpssOsMemSet(regValuePtr,0,sizeof(GT_U32)* txqNum);
    for (cntrSetNum = 0; cntrSetNum < 2; cntrSetNum++)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                portEgressCntrModeInfo[cntrSetNum].portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
           continue;
        }
        else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].txQConfig;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.peripheralAccess.egressMibCounterSet.config[cntrSetNum];
        }
        for (i =0; i < txqNum; i++)
        {
            rc = prvCpssHwPpPortGroupReadRegister(devNum,
                   i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                   regAddr, &value);
            if(rc != GT_OK)
            {
                cpssOsFree(regValuePtr);
                return rc;
            }
            regValuePtr[i] = value;
        }
        /* check if all values are the same */
        for (i = 0; i < txqNum; i++ )
        {
            if (regValuePtr[0] != regValuePtr[i])
            {
                break;
            }
        }
        if ((i == txqNum) && (txqNum > 1))
        {
            /* it means all values are the same */
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                portEgressCntrModeInfo[cntrSetNum].portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
        else
            if ((txqNum == 1)&&(regValuePtr[0] & 0x1) == 0x0)
            {
                PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                    portEgressCntrModeInfo[cntrSetNum].portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            }
            else
            {
                for (i = 0; i < txqNum; i++ )
                {
                    portValue = (regValuePtr[i]>>4) & 0x3f;
                    if (portValue != PRV_CPSS_DXCH_NULL_PORT_NUM_CNS)
                    {
                        /* check if it is CPU port */
                        if ((portValue & 0xf) == 0xf)
                        {
                            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                                portEgressCntrModeInfo[cntrSetNum].portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
                        }
                        else
                        {
                            /* convert port value from port local HEM to global*/
                            globalPort = PRV_CPSS_DXCH_HEM_LOCAL_TO_GLOBAL_PORT(devNum,i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,portValue);
                             /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, globalPort);
                            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                                portEgressCntrModeInfo[cntrSetNum].portGroupId = portGroupId;
                        }
                        break;
                    }
                }
                if (i == txqNum)
                {
                    /* it means that all port values are PRV_CPSS_DXCH_NULL_PORT_NUM_CNS*/
                    cpssOsFree(regValuePtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
    }
    cpssOsFree(regValuePtr);
    return rc;
}

/*******************************************************************************
* prvCpssDxChBridgeIngressCntrModeCatchUp
*
* DESCRIPTION:
*       Synchronize Bridge Ingress Counters Mode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBridgeIngressCntrModeCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32 regAddr;
    GT_U32 value = 0;
    GT_U32  portGroupId;
    GT_U32  i = 0;
    GT_U32  entriesCounter = 0;
    GT_U32 cntrSetNum = 0;
    GT_U32 regValue[8][2] = {{0,0}};

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    for (cntrSetNum = 0; cntrSetNum < 2; cntrSetNum++)
    {
        entriesCounter = 0;
        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == GT_FALSE)
        {
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                bridgeIngressCntrMode[cntrSetNum].portGroupId  = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            continue;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                             brgCntrSet[cntrSetNum].cntrSetCfg;
        /* loop on all port groups :
            on the port group that 'own' the port , set the needed configuration
            on other port groups put 'NULL port'
        */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,regAddr,&value);
            if(rc != GT_OK)
            {
                return rc;
            }
            regValue[entriesCounter][0] = value;
            regValue[entriesCounter][1] = portGroupId;
            entriesCounter++;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

        /* check if all values are the same */
        for (i = 0; i < entriesCounter; i++ )
        {
            if (regValue[0][0] != regValue[i][0])
            {
                break;
            }
        }

        if ((i == entriesCounter) && (entriesCounter > 1))
        {
             /* it means all values are the same */
                PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                    bridgeIngressCntrMode[cntrSetNum].portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
        else
        {
            if ((entriesCounter == 1) && ((regValue[0][0] & 0x3) == 0x1))
            {
                PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                    bridgeIngressCntrMode[cntrSetNum].portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            }
            else
            {
                for (i = 0; i < entriesCounter; i++ )
                {
                    regValue[i][0] = (value >> 2) & 0x3f;

                    if (regValue[i][0] != PRV_CPSS_DXCH_NULL_PORT_NUM_CNS)
                    {
                        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                                bridgeIngressCntrMode[cntrSetNum].portGroupId = regValue[i][1];
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
        }
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChBridgeIngressDropCntrModeCatchUp
*
* DESCRIPTION:
*       Synchronize  Ingress Drop Counters Mode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBridgeIngressDropCntrModeCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32 regAddr;
    GT_U32 value;
    GT_U32  portGroupId;
    GT_U32  i = 0;
    GT_U32  entriesCounter = 0;
    GT_U32 regValue[8][2] = {{0,0}};

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    entriesCounter = 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
         PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.cfgIngressDropCntrMode.portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
         return GT_OK;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.ingressDropCntrConfReg;
    }

    /* loop on all port groups :
    */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,regAddr,&value);
        if(rc != GT_OK)
        {
            return rc;
        }
        regValue[entriesCounter][0] = value;
        regValue[entriesCounter][1] = portGroupId;
        entriesCounter++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    /* check if all values are the same */
    for (i = 0; i < entriesCounter; i++ )
    {
        if (regValue[0][0] != regValue[i][0])
        {
            break;
        }
    }
    if ((i == entriesCounter) && (entriesCounter > 1))
    {
        /* it means all values are the same */
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.cfgIngressDropCntrMode.portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }
    else
    {
        if ((entriesCounter == 1) && ((regValue[0][0] & 0x3) != 0x2))
        {
            /* it is not port mode */
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.cfgIngressDropCntrMode.portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
        else
        {
            for (i = 0; i < entriesCounter; i++ )
            {
                regValue[i][0] =  (regValue[i][0] >> 2) & 0xfff;
                if (regValue[i][0] != PRV_CPSS_DXCH_NULL_PORT_NUM_CNS)
                {
                    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.cfgIngressDropCntrMode.portGroupId = regValue[i][1];
                    break;
                }
                else
                {
                    continue;
                }
            }
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChBridgeTag1VidFdbEnCatchUp
*
* DESCRIPTION:
*       Synchronize tag1VidFdbEn in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.

*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBridgeTag1VidFdbEnCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc;
    GT_BOOL     enable;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum) == GT_FALSE)
    {
        return GT_OK;
    }

    /* get the value from the HW */
    rc = cpssDxChBrgFdbVid1AssignmentEnableGet(devNum,&enable);

    if(rc == GT_OK)
    {
        /* save info to the DB */
        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn = BIT2BOOL_MAC(enable);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChBridgeEgrVlanPortFltTabAccessModeCatchUp
*
* DESCRIPTION:
*       Synchronize vlanfltTabAccessMode in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.

*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBridgeEgrVlanPortFltTabAccessModeCatchUp
(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc;
    CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT fltTabAccessMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum) == GT_FALSE)
    {
        return GT_OK;
    }

    /* get the value from the HW */
    rc = cpssDxChBrgEgrFltVlanPortAccessModeGet(devNum,&fltTabAccessMode);

    if(rc == GT_OK)
    {
        /* save info to the DB */
        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.portVlanfltTabAccessMode = fltTabAccessMode;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortModeParamsCatchUp
*
* DESCRIPTION:
*       Synchronize Port Mode parameters in software DB by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortModeParamsCatchUp
(
    IN GT_U8    devNum
)
{
    GT_PHYSICAL_PORT_NUM port;
    GT_STATUS rc = GT_OK;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    CPSS_PORT_SPEED_ENT            speed;
    GT_BOOL isValid;
    GT_U32 portMac;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* loop over all GE and FE ports */
    for (port = 0; port < CPSS_MAX_PORTS_NUM_CNS; port++)
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = cpssDxChPortPhysicalPortMapIsValidGet(devNum, port, &isValid);
            if((rc != GT_OK) || (isValid != GT_TRUE))
            {
                continue;
            }
        }
        else
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, port);
        }

        if ((port == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            continue;
        }

        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, port, portMac);

        /*
         * If we enabled CatchUp Validation PRV_CPSS_DXCH_PORT_IFMODE_MAC macro will
         * return wrong value, so use saved ifMode value
         */
        if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,port) == PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS &&
                (CPSS_PP_FAMILY_DXCH_XCAT_E == PRV_CPSS_PP_MAC(devNum)->devFamily
                || CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(devNum)->devFamily
                || CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily
                || CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
        {
            ifMode = ppConfigCatchUpParams.portIfMode[portMac];
        }
        else
        {
            rc =  prvCpssDxChPortInterfaceModeHwGet(devNum, port, &ifMode);
            if (rc == GT_NOT_INITIALIZED)
            {
                continue;
            }
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* portType updated inside prvCpssDxChPortInterfaceModeHwGet */
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMac].portIfMode = ifMode;

        /*
         * If we enabled CatchUp Validation PRV_CPSS_DXCH_PORT_SPEED_MAC macro will
         * return wrong value, so use saved speed value
         */
        if(PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,port) == PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS &&
                (CPSS_PP_FAMILY_DXCH_XCAT_E == PRV_CPSS_PP_MAC(devNum)->devFamily
                || CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(devNum)->devFamily
                || CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily
                || CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
        {
            speed = ppConfigCatchUpParams.portSpeed[portMac];
        }
        else
        {
            rc = prvCpssDxChPortSpeedHwGet(devNum, port, &speed);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMac].portSpeed = speed;
        if((CPSS_PORT_SPEED_2500_E == speed) &&
            (CPSS_PORT_INTERFACE_MODE_1000BASE_X_E == ifMode))
        {
            /* SGMII 2.5G in HW implemented as 1000BaseX */
            PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMac].portIfMode =
                                            CPSS_PORT_INTERFACE_MODE_SGMII_E;
        }

        if (CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E == systemRecoveryInfo.systemRecoveryProcess)
        {
            if (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
            {
                if((ifMode != CPSS_PORT_INTERFACE_MODE_NA_E) &&
                   (speed != CPSS_PORT_SPEED_NA_E))
                {
                    rc = prvCpssDxChLion2PortTypeSet(devNum, port, ifMode, speed);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
                /* else leave cpss default defined in hwPpPhase1Part1 */
            }
            else
            {
                if(ifMode != CPSS_PORT_INTERFACE_MODE_NA_E)
                {
                    prvCpssDxChPortTypeSet(devNum, portMac, ifMode, speed);
                }
            }
        }

        /* update addresses of mac registers accordingly to used MAC 1G/XG/XLG */
        rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, port, ifMode);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortResourcesCatchUp
*
* DESCRIPTION:
*       Synchronize Port resources parameters in software DB by its hw values
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortResourcesCatchUp
(
    IN GT_U8    devNum
)
{
    GT_U32  txQDescrCredits;
    GT_U32  txFifoHeaderCredits;
    GT_U32  txFifoPayloadCredits;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum) == GT_FALSE)
    {
        return GT_OK;
    }

    if (PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        CPSS_TBD_BOOKMARK_BOBCAT3
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }
    else if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        CPSS_TBD_BOOKMARK_BOBCAT2_BOBK
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }
    else if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChPortBcat2B0ResourcesCalculateFromHW(devNum,
                                                            BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit,
                                                            &txQDescrCredits);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortBcat2B0ResourcesCalculateFromHW(devNum,
                                                            BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold,
                                                            &txFifoHeaderCredits);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortBcat2B0ResourcesCalculateFromHW(devNum,
                                                            BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold,
                                                            &txFifoPayloadCredits);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* zero the credits in shadow */
        rc = prvCpssDxChPortResourcesConfigDbDelete(devNum, /* dpIndex */0,
                                                    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[0],
                                                    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[0],
                                                    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[0]);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortResourcesConfigDbAdd(devNum, /* dpIndex */0, txQDescrCredits, txFifoHeaderCredits, txFifoPayloadCredits);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* port resources are not supported in Bobcat2 A0 - catchup should do nothing*/
        return GT_OK;
    }

    return GT_OK;
}

/************************************************************************
* dxChAuFuPtrUpdate
*
* DESCRIPTION:
*       The function scan the AU/FU queues and update AU/FU software queue pointer.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum            - the device number on which AU are counted
*       portGroupId       - the portGroupId - for multi-port-groups support
*       queueType         - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChAuFuPtrUpdate
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  MESSAGE_QUEUE_ENT             queueType
)
{
    PRV_CPSS_AU_DESC_STC        *descPtr;          /*pointer to the current descriptor*/
    PRV_CPSS_AU_DESC_STC        *descBlockPtr;     /* AU descriptors block */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;      /* pointer to the descriptors DB of the device */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrl1Ptr = 0; /* pointer to the descriptors DB
                                                      for additional primary AUQ of the device */
    GT_U32                      ii;                /* iterator */
    GT_U32                      auMessageNumBytes;
    GT_U32                      auMessageNumWords;
    GT_U32                      numberOfQueues = 1; /* number of queues */
    GT_U32                      currentQueue;       /* iterator */
    GT_BOOL                     useDoubleAuq;       /* support configuration of two AUQ memory regions */
    PRV_CPSS_AU_DESC_EXT_8_STC  *descExtPtr;        /*pointer to the current descriptor*/
     GT_U32                     *auMemPtr = NULL;   /* pointer to start of current message */
    CPSS_MAC_ENTRY_EXT_KEY_STC  macEntry;
    GT_U32                      portGroupsBmp;
    GT_U32                      qa_counter;
    GT_PORT_GROUPS_BMP          completedPortGroupsBmp = 0;
    GT_PORT_GROUPS_BMP          succeededPortGroupsBmp = 0;
    CPSS_MAC_UPDATE_MSG_EXT_STC auFuMessage;
    GT_STATUS rc = GT_OK;
    GT_32           intKey = 0;
    CPSS_SYSTEM_RECOVERY_INFO_STC tempSystemRecoveryInfo;
    tempSystemRecoveryInfo.systemRecoveryState = systemRecoveryInfo.systemRecoveryState;

    cpssOsMemSet(&macEntry,0,sizeof(CPSS_MAC_ENTRY_EXT_KEY_STC));
    cpssOsMemSet(&auFuMessage,0,sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC));
    macEntry.entryType                      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.macVlan.vlanId             = 0;
    macEntry.key.macVlan.macAddr.arEther[0] = 0x0;
    macEntry.key.macVlan.macAddr.arEther[1] = 0x1A;
    macEntry.key.macVlan.macAddr.arEther[2] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[3] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[4] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[5] = 0xFF;

    auMessageNumWords = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.auMessageNumOfWords;
    auMessageNumBytes = 4 * auMessageNumWords;

    useDoubleAuq = PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useDoubleAuq;

    switch (queueType)
    {
    case MESSAGE_QUEUE_PRIMARY_FUQ_E:
        descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
        break;
    case MESSAGE_QUEUE_PRIMARY_AUQ_E:
        descCtrlPtr = (PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId) == 0) ?
            &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]) :
            &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);

        if (useDoubleAuq == GT_TRUE)
        {
            numberOfQueues = 2;
            descCtrl1Ptr = (PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId) == 1) ?
                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]) :
                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);
        }
        break;
    case MESSAGE_QUEUE_SECONDARY_AUQ_E:
        descCtrlPtr = (PRV_CPSS_SECONDARY_AUQ_INDEX_MAC(devNum, portGroupId) == 0) ?
            &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]) :
            &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId]);
        if (useDoubleAuq == GT_TRUE)
        {
            numberOfQueues = 2;
            descCtrl1Ptr = (PRV_CPSS_SECONDARY_AUQ_INDEX_MAC(devNum, portGroupId) == 1) ?
                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]) :
                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId]);
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* get address of AU descriptors block */
    descBlockPtr = (PRV_CPSS_AU_DESC_STC*)(descCtrlPtr->blockAddr);

    /* pointer to the current descriptor */
    descPtr = &(descBlockPtr[descCtrlPtr->currDescIdx]);
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
    for(currentQueue = 0; currentQueue < numberOfQueues; currentQueue++)
    {
        /* Second iteration for additional AUQ */
        if(currentQueue == 1)
        {
             descCtrlPtr = descCtrl1Ptr;
             descPtr = &(descBlockPtr[descCtrlPtr->currDescIdx]);
        }
        for (ii = descCtrlPtr->currDescIdx; ii < descCtrlPtr->blockSize; ii++ ,descPtr++)
        {
            if(!(AU_DESC_IS_NOT_VALID(descPtr)))
            {
                /* no more not valid descriptors */
                break;
            }
            /* increment software descriptor pointer*/
             descCtrlPtr->currDescIdx =
                (( descCtrlPtr->currDescIdx + 1) %  descCtrlPtr->blockSize);
        }
        if (ii == descCtrlPtr->blockSize)
        {
            descCtrlPtr->currDescIdx = 0;
            descPtr = &(descBlockPtr[descCtrlPtr->currDescIdx]);
            if (queueType == MESSAGE_QUEUE_PRIMARY_AUQ_E)
            {
                /*In this case there is no new real message in the queue. In order to understand where software pointer is*/
                /* quary is sent from cpu to pp and pp will reply to cpu with quary response and this response should be found*/
                /* in AUQ*/
                portGroupsBmp = 0;
                portGroupsBmp = portGroupsBmp | (1<<portGroupId);

                qa_counter = 0;
                systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
                do
                {
                    rc =  cpssDxChBrgFdbPortGroupQaSend( devNum, portGroupsBmp,&macEntry);
                     if(rc != GT_OK)
                     {
                #ifdef ASIC_SIMULATION
                         cpssOsTimerWkAfter(1);
                #endif
                         qa_counter++;
                         if(qa_counter > 20)
                         {
                             cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                             systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                             return rc;
                         }
                     }
                 } while (rc != GT_OK);
                 systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                 /* verify that action is completed */
                 do
                 {
                     rc = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum,portGroupsBmp,&completedPortGroupsBmp,
                                                                       &succeededPortGroupsBmp);
                     if(rc != GT_OK)
                     {
                         cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                         return rc;
                     }
                 }
                 while ((completedPortGroupsBmp & portGroupsBmp)!= portGroupsBmp);
                 /* now perform search again */
                 for (ii = descCtrlPtr->currDescIdx; ii < descCtrlPtr->blockSize; ii++ ,descPtr++)
                 {
                     if(!(AU_DESC_IS_NOT_VALID(descPtr)))
                     {
                         /* no more not valid descriptors */
                         break;
                     }
                     /* increment software descriptor pointer*/
                      descCtrlPtr->currDescIdx =
                         (( descCtrlPtr->currDescIdx + 1) %  descCtrlPtr->blockSize);
                 }

                 if (ii == descCtrlPtr->blockSize)
                 {
                     descCtrlPtr->currDescIdx = 0;
                 }
                 else
                 {
                     /* entry was found */
                     /* check that this entry is QR and delete it*/
                     /* the pointer to start of 'next message to handle'  */
                     auMemPtr = (GT_U32 *)(descCtrlPtr->blockAddr + (auMessageNumBytes * descCtrlPtr->currDescIdx));
                     descExtPtr= (PRV_CPSS_AU_DESC_EXT_8_STC*)auMemPtr;
                     rc = auDesc2UpdMsg(devNum, portGroupId ,descExtPtr , GT_TRUE, &auFuMessage);
                     if(rc != GT_OK)
                     {
                         return rc;
                     }
                     if ( (auFuMessage.macEntry.key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)        &&
                          (0 == cpssOsMemCmp((GT_VOID*)&auFuMessage.macEntry.key.key.macVlan.macAddr,
                                             (GT_VOID*)&macEntry.key.macVlan.macAddr,
                                             sizeof (GT_ETHERADDR)))                                        &&
                          (auFuMessage.macEntry.key.key.macVlan.vlanId  == macEntry.key.macVlan.vlanId) )
                     {
                         AU_DESC_RESET_MAC(descPtr);
                         descCtrlPtr->currDescIdx++;
                     }
                 }
            }
        }
    }
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    return GT_OK;
}


/*******************************************************************************
* dxChHaAuFuSameMemCatchUp
*
* DESCRIPTION:
*       Synchronize  software DB AU/FU pointers by its hw values
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChHaAuFuSameMemCatchUp
(
    IN GT_U8    devNum
)
{
    MESSAGE_QUEUE_ENT             queueType;
    GT_U32                        portGroupId;
    GT_STATUS                     rc = GT_OK;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId].blockAddr != 0)
        {
            /* handle AUQs for this portGroup */
            queueType = MESSAGE_QUEUE_PRIMARY_AUQ_E;
            rc = dxChAuFuPtrUpdate(devNum, portGroupId, queueType);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId].blockAddr != 0)
        {
            /* handle secondary AUQs for this portGroup */
            queueType = MESSAGE_QUEUE_SECONDARY_AUQ_E;
            rc = dxChAuFuPtrUpdate(devNum,portGroupId, queueType);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].blockAddr != 0)
        {
            /* handle FUQs for this portGroup */
            queueType = MESSAGE_QUEUE_PRIMARY_FUQ_E;
            rc = dxChAuFuPtrUpdate(devNum,portGroupId, queueType);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}
/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpSameMemoryAuFuHandle
*
* DESCRIPTION:
*       Synchronize AUQ/FUQ software pointers by its hw values.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function could be used only if application guarantees constant
*       surviving cpu restart memory for AUQ/FUQ allocation.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChSystemRecoveryCatchUpSameMemoryAuFuHandle
(
    IN GT_U8    devNum
)
{
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;/* pointer to the module
                                                configure of the PP's database*/
    GT_U32 portGroupId;
    GT_STATUS rc = GT_OK;
    GT_UINTPTR                  phyAddr;/* The physical address of the AU block*/
    GT_UINTPTR virtAddr;
    GT_U32 queueSize;
    GT_U32 regAddr;
    GT_U32 auMessageNumBytes; /* number of bytes in AU/FU message */
    GT_U32 auqTotalSize = 0;      /* auq total size of all port groups*/
    GT_U32 fuqTotalSize = 0;      /* fuq total size of all port groups*/


    PRV_CPSS_AU_DESC_CTRL_STC    *auDescCtrlPtr = NULL;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* Configure the module configruation struct.   */
    moduleCfgPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg);
    if (moduleCfgPtr->useDoubleAuq == GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    auMessageNumBytes = 4 * PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.auMessageNumOfWords;
    /* restore AUQ/FUQ DB */
    cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl,0,sizeof(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl));
    cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl,0,sizeof(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl));
    cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl,0,sizeof(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl));
    cpssOsMemSet(&(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa),0,sizeof(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa));
    cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.activeAuqIndex,0,sizeof(PRV_CPSS_PP_MAC(devNum)->intCtrl.activeAuqIndex));
    cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl,0,sizeof(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl));
    cpssOsMemSet(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl,0,sizeof(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl));

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        auDescCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;

        rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,(GT_U32*)&phyAddr);
        if (rc != GT_OK)
        {
            return rc;
        }
        cpssOsPhy2Virt(phyAddr,&virtAddr);
        auDescCtrlPtr->blockAddr = virtAddr;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
        rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&queueSize);
        if (rc != GT_OK)
        {
            return rc;
        }
        auDescCtrlPtr->blockSize = queueSize;
        auqTotalSize += queueSize;
        auDescCtrlPtr->currDescIdx = 0;
        auDescCtrlPtr->unreadCncCounters = 0;

        if (moduleCfgPtr->fuqUseSeparate == GT_TRUE)
        {
            auDescCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;
            rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,(GT_U32*)&phyAddr);
            if (rc != GT_OK)
            {
                return rc;
            }
            cpssOsPhy2Virt(phyAddr,&virtAddr);
            auDescCtrlPtr->blockAddr = virtAddr;
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
            rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,0,31,&queueSize);/*num of entries in the block*/
            if (rc != GT_OK)
            {
                return rc;
            }
            auDescCtrlPtr->blockSize = queueSize;
            fuqTotalSize += queueSize;
            auDescCtrlPtr->currDescIdx = 0;
            auDescCtrlPtr->unreadCncCounters = 0;
        }
        /* Set primary AUQ index */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.activeAuqIndex[portGroupId] = 0;
        /* Set primary AUQ init state - 'FULL';
        When WA triggered for the first time - all primary AUQs are full */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState =
            PRV_CPSS_AUQ_STATE_ALL_FULL_E;
        /* Set secondary AUQ index */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activeSecondaryAuqIndex = 0;
        /* Set secondary AUQ state - 'EMPTY' */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState =
            PRV_CPSS_AUQ_STATE_ALL_EMPTY_E;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    moduleCfgPtr->auCfg.auDescBlock = (GT_U8*)PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[0].blockAddr;
    moduleCfgPtr->auCfg.auDescBlockSize = auqTotalSize * auMessageNumBytes;
    if (moduleCfgPtr->fuqUseSeparate == GT_TRUE)
    {
        moduleCfgPtr->fuCfg.fuDescBlock = (GT_U8*)PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[0].blockAddr;
        moduleCfgPtr->fuCfg.fuDescBlockSize = fuqTotalSize * auMessageNumBytes;
    }

    /* perform auq/fuq sw pointer tuning */
    rc = dxChHaAuFuSameMemCatchUp(devNum);
    return rc;
}

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpDiffMemoryAuHandle
*
* DESCRIPTION:
*       Synchronize AUQ software pointers by its hw values.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function could be used only if application couldn't guarantee constant
*       surviving cpu restart memory for AUQ/FUQ allocation.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChSystemRecoveryCatchUpDiffMemoryAuHandle
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    rc =  prvCpssDxChRestoreAuqCurrentStatus(devNum);
    return rc;
}

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpDiffMemoryFuHandle
*
* DESCRIPTION:
*       Synchronize FUQ software pointers by its hw values.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function could be used only if application couldn't guarantee constant
*       surviving cpu restart memory for AUQ/FUQ allocation.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChSystemRecoveryCatchUpDiffMemoryFuHandle
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc = GT_OK;
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;
    GT_U32                          regAddr;
    GT_U32                          portGroupId;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    moduleCfgPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg);
    if (moduleCfgPtr->fuqUseSeparate == GT_FALSE)
    {
        return GT_OK;
    }
    /* After this action on chip FIFO contents (if at all) is transferred */
    /* into FUQ defined during cpss init                                  */
    /* now sinchronization hw and sw FUQ pointers is required */

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].blockAddr != 0)
        {
            /* Enable FUQ for each portgroup  */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
            rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,31,1,1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpHandle
*
* DESCRIPTION:
*       Perform synchronization of hardware data and software DB after special init sequence.
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChSystemRecoveryCatchUpHandle
(
   GT_VOID
)
{
    GT_STATUS rc = GT_OK;
    GT_U8 devNum;
    GT_U32 i = 0;
    GT_U8 rxQueue = 0;
    CPSS_SYSTEM_RECOVERY_INFO_STC   tempSystemRecoveryInfo = systemRecoveryInfo;
    for (devNum = 0; devNum < PRV_CPSS_MAX_PP_DEVICES_CNS; devNum++)
    {
        if ( (prvCpssPpConfig[devNum] == NULL) ||
             (PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum) == 0) )
        {
            continue;
        }
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            while (catchUpFuncPtrArray[i] != NULL)
            {
                /* perform catch up*/
                rc = (*catchUpFuncPtrArray[i])(devNum);
                if (rc != GT_OK)
                {
                    return rc;
                }
                i++;
            }
        }
        if (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_TRUE)
        {
            /* HA mode - application provide the same memory for AUQ -- handle AUQ pointer*/
            rc = prvCpssDxChSystemRecoveryCatchUpSameMemoryAuFuHandle(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if(systemRecoveryInfo.systemRecoveryMode.haCpuMemoryAccessBlocked == GT_TRUE)
        {
            /*HA mode - application can't guarantee the same memory for AUQ */
            /* during cpss init stage special AUQ WA was done and AUQ was disable for messages.*/
            systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
            if (systemRecoveryInfo.systemRecoveryMode.continuousFuMessages == GT_FALSE)
            {
                /*Restore FUQ enable status. In this case hw should be written */

                rc = prvCpssDxChSystemRecoveryCatchUpDiffMemoryFuHandle(devNum);
                if (rc != GT_OK)
                {
                    systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                    return rc;
                }
            }

            if (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE)
            {
                /*Restore AUQ enable status. In this case hw should be written */
                rc =  prvCpssDxChSystemRecoveryCatchUpDiffMemoryAuHandle(devNum);
                if (rc != GT_OK)
                {
                    systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                    return rc;
                }
            }

            /* enable RxSDMA queues */
            /* write Rx SDMA Queues Reg - Enable Rx SDMA Queues */
            if (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)
            {
                for(rxQueue = 0; rxQueue < NUM_OF_RX_QUEUES; rxQueue++)
                {
                    /* Enable Rx SDMA Queue */
                    rc = cpssDxChNetIfSdmaRxQueueEnable(devNum, rxQueue, GT_TRUE);
                    if(rc != GT_OK)
                    {
                        systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
                        return rc;
                    }
                }
            }
            systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpRestorePpInfo
*
* DESCRIPTION:
*       Restore PP original info from ppConfigCatchUpParams
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChSystemRecoveryCatchUpRestorePpInfo
(
    IN  GT_U8   devNum
)
{
    GT_U32                  cntrSetNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32                  portMacNum;
    GT_U32                  dpIndex;
    GT_BOOL                 isValid;
    GT_STATUS               rc;

    PRV_CPSS_HW_DEV_NUM_MAC(devNum) = ppConfigCatchUpParams.hwDevNum;
    PRV_CPSS_PP_MAC(devNum)->cpuPortMode = ppConfigCatchUpParams.cpuPortMode;
    PRV_CPSS_PP_MAC(devNum)->netifSdmaPortGroupId = ppConfigCatchUpParams.netifSdmaPortGroupId;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.devTable = ppConfigCatchUpParams.devTable;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode = ppConfigCatchUpParams.vlanMode;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode = ppConfigCatchUpParams.hashMode;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNum = ppConfigCatchUpParams.actionHwDevNum;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNumMask = ppConfigCatchUpParams.actionHwDevNumMask;
    PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader = ppConfigCatchUpParams.prePendTwoBytesHeader;
    PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[0] = ppConfigCatchUpParams.policerInfoMemSize[0];
    PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[1] = ppConfigCatchUpParams.policerInfoMemSize[1];
    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.counterMode =
        ppConfigCatchUpParams.securBreachDropCounterInfoCounterMode;
    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.portGroupId =
        ppConfigCatchUpParams.securBreachDropCounterInfoPortGroupId;
    for (cntrSetNum = 0; cntrSetNum < 2; cntrSetNum++)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.portEgressCntrModeInfo[cntrSetNum].portGroupId =
            ppConfigCatchUpParams.portEgressCntrModeInfoPortGroupId[cntrSetNum];
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.bridgeIngressCntrMode[cntrSetNum].portGroupId =
            ppConfigCatchUpParams.bridgeIngressCntrModePortGroupId[cntrSetNum];
    }
    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.cfgIngressDropCntrMode.portGroupId =
        ppConfigCatchUpParams.cfgIngressDropCntrModePortGroupId;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn = ppConfigCatchUpParams.tag1VidFdbEn;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.portVlanfltTabAccessMode = ppConfigCatchUpParams.portVlanfltTabAccessMode;

    for (portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = cpssDxChPortPhysicalPortMapIsValidGet(devNum, portNum, &isValid);
            if ((rc != GT_OK) || (isValid != GT_TRUE))
            {
                continue;
            }
        }
        else
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        }

        if ((portNum == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            continue;
        }

        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portIfMode = ppConfigCatchUpParams.portIfMode[portMacNum];
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portSpeed = ppConfigCatchUpParams.portSpeed[portMacNum];
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portType = ppConfigCatchUpParams.portType[portMacNum];
    }

    for (dpIndex = 0; dpIndex < PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.maxDp; dpIndex++)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[dpIndex] =
            ppConfigCatchUpParams.usedDescCredits[dpIndex];
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[dpIndex] =
            ppConfigCatchUpParams.usedHeaderCredits[dpIndex];
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[dpIndex] =
            ppConfigCatchUpParams.usedPayloadCredits[dpIndex];
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpValidityCheckEnable
*
* DESCRIPTION:
*       Enable CatchUp validity check
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on failure
*
* COMMENTS:
*       Pre-condition for using this function:
*       CatchUp parameters in PP data structure must be equal to the HW values
*
*******************************************************************************/
GT_STATUS prvCpssDxChSystemRecoveryCatchUpValidityCheckEnable
(
    IN  GT_U8   devNum
)
{
    GT_U32                          cntrSetNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_U32                          portMacNum;
    GT_U32                          dpIndex;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode;
    CPSS_PORT_SPEED_ENT             speed;
    GT_BOOL                         isValid;
    GT_STATUS                       rc;

    /* Store original info in ppConfigCatchUpParams */
    ppConfigCatchUpParams.hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
    ppConfigCatchUpParams.cpuPortMode = PRV_CPSS_PP_MAC(devNum)->cpuPortMode;
    ppConfigCatchUpParams.netifSdmaPortGroupId = PRV_CPSS_PP_MAC(devNum)->netifSdmaPortGroupId;
    ppConfigCatchUpParams.devTable = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.devTable;
    ppConfigCatchUpParams.vlanMode = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode;
    ppConfigCatchUpParams.hashMode = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode;
    ppConfigCatchUpParams.actionHwDevNum = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNum;
    ppConfigCatchUpParams.actionHwDevNumMask = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNumMask;
    ppConfigCatchUpParams.prePendTwoBytesHeader = PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader;
    ppConfigCatchUpParams.policerInfoMemSize[0] = PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[0];
    ppConfigCatchUpParams.policerInfoMemSize[1] = PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[1];
    ppConfigCatchUpParams.securBreachDropCounterInfoCounterMode =
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.counterMode;
    ppConfigCatchUpParams.securBreachDropCounterInfoPortGroupId =
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.portGroupId;
    for (cntrSetNum = 0; cntrSetNum < 2; cntrSetNum++)
    {
        ppConfigCatchUpParams.portEgressCntrModeInfoPortGroupId[cntrSetNum] =
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.portEgressCntrModeInfo[cntrSetNum].portGroupId;
        ppConfigCatchUpParams.bridgeIngressCntrModePortGroupId[cntrSetNum] =
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.bridgeIngressCntrMode[cntrSetNum].portGroupId;
    }
    ppConfigCatchUpParams.cfgIngressDropCntrModePortGroupId =
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.cfgIngressDropCntrMode.portGroupId;
    ppConfigCatchUpParams.tag1VidFdbEn = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn;
    ppConfigCatchUpParams.portVlanfltTabAccessMode = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.portVlanfltTabAccessMode;
    for (portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = cpssDxChPortPhysicalPortMapIsValidGet(devNum, portNum, &isValid);
            if ((rc != GT_OK) || (isValid != GT_TRUE))
            {
                continue;
            }
        }
        else
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        }

        if ((portNum == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            continue;
        }

        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

        ppConfigCatchUpParams.portIfMode[portMacNum] = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portIfMode;
        ppConfigCatchUpParams.portSpeed[portMacNum] = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portSpeed;
        ppConfigCatchUpParams.portType[portMacNum] = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portType;
    }
    for (dpIndex = 0; dpIndex < PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.maxDp; dpIndex++)
    {
        ppConfigCatchUpParams.usedDescCredits[dpIndex] =
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[dpIndex];
        ppConfigCatchUpParams.usedHeaderCredits[dpIndex] =
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[dpIndex];
        ppConfigCatchUpParams.usedPayloadCredits[dpIndex] =
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[dpIndex];
    }

    /* Set all the parameters that catchup configures to chosen pattern */
    PRV_CPSS_HW_DEV_NUM_MAC(devNum) = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_PP_MAC(devNum)->cpuPortMode = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    if ((PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum)) &&
        (!((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE) && (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum)))))
    {
        /* Lion, Lion2, Bobcat3 */
        PRV_CPSS_PP_MAC(devNum)->netifSdmaPortGroupId = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    }
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        /* xCat, xCat2, xCat3, Lion, Lion2 */
        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.devTable = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    }
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNum = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNumMask = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[0] = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[1] = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.counterMode = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.portGroupId = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    for (cntrSetNum = 0; cntrSetNum < 2; cntrSetNum++)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.portEgressCntrModeInfo[cntrSetNum].portGroupId = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.bridgeIngressCntrMode[cntrSetNum].portGroupId = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    }
    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.cfgIngressDropCntrMode.portGroupId = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
        PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.portVlanfltTabAccessMode = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    }
    for (portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = cpssDxChPortPhysicalPortMapIsValidGet(devNum, portNum, &isValid);
            if ((rc != GT_OK) || (isValid != GT_TRUE))
            {
                continue;
            }
        }
        else
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        }

        if ((portNum == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            continue;
        }

        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

        rc =  prvCpssDxChPortInterfaceModeHwGet(devNum, portNum, &ifMode);
        if (rc == GT_NOT_INITIALIZED)
        {
            continue;
        }
        if (rc != GT_OK)
        {
            prvCpssDxChSystemRecoveryCatchUpRestorePpInfo(devNum);
            return rc;
        }

        speed = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portSpeed;

        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portIfMode = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portSpeed = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;

        if (CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E != systemRecoveryInfo.systemRecoveryProcess)
        {
            continue;
        }
        if (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            if ((ifMode == CPSS_PORT_INTERFACE_MODE_NA_E) || (speed == CPSS_PORT_SPEED_NA_E))
            {
                continue;
            }
        }
        else
        {
            if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                continue;
            }
        }
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portType = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    }

    for (dpIndex = 0; dpIndex < PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.maxDp; dpIndex++)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[dpIndex] = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[dpIndex] = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[dpIndex] = PRV_CPSS_DXCH_CATCHUP_PATTERN_CNS;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChSystemRecoveryCatchUpValidityCheck
*
* DESCRIPTION:
*       Validates that SW params hold the correct value from HW after CatchUp
*       process.
*
* APPLICABLE DEVICES:
*       Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on failure
*
* COMMENTS:
*       To use this API, cpssDxChCatchUpValidityCheckEnable must be called before
*       performing CatchUp
*
*******************************************************************************/
GT_STATUS prvCpssDxChSystemRecoveryCatchUpValidityCheck
(
    IN  GT_U8   devNum
)
{
    GT_U32                  cntrSetNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32                  portMacNum;
    GT_U32                  dpIndex;
    GT_BOOL                 isValid;
    GT_STATUS               rc = GT_OK;
    GT_STATUS               rc1;

    /* Compare PP info with CatchUp parameters */
    if (ppConfigCatchUpParams.hwDevNum != PRV_CPSS_HW_DEV_NUM_MAC(devNum))
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.cpuPortMode != PRV_CPSS_PP_MAC(devNum)->cpuPortMode)
    {
        rc = GT_FAIL;
        goto restore;
    }
    if ((PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum)) &&
        (!((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE) && (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum)))))
    {
        /* Lion, Lion2, Bobcat3 */
        if (ppConfigCatchUpParams.netifSdmaPortGroupId != PRV_CPSS_PP_MAC(devNum)->netifSdmaPortGroupId)
        {
            rc = GT_FAIL;
            goto restore;
        }
    }
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        /* xCat, xCat2, xCat3, Lion, Lion2 */
        if (ppConfigCatchUpParams.devTable != PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.devTable)
        {
            rc = GT_FAIL;
            goto restore;
        }
    }
    if (ppConfigCatchUpParams.vlanMode != PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode)
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.hashMode != PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode)
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.actionHwDevNum != PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNum)
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.actionHwDevNumMask != PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.actionHwDevNumMask)
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.prePendTwoBytesHeader != PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader)
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.policerInfoMemSize[0] != PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[0])
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.policerInfoMemSize[1] != PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[1])
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.securBreachDropCounterInfoCounterMode !=
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.counterMode)
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (ppConfigCatchUpParams.securBreachDropCounterInfoPortGroupId !=
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.securBreachDropCounterInfo.portGroupId)
    {
        rc = GT_FAIL;
        goto restore;
    }
    for (cntrSetNum = 0; cntrSetNum < 2; cntrSetNum++)
    {
        if (ppConfigCatchUpParams.portEgressCntrModeInfoPortGroupId[cntrSetNum] !=
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.portEgressCntrModeInfo[cntrSetNum].portGroupId)
        {
            rc = GT_FAIL;
            goto restore;
        }
        if (ppConfigCatchUpParams.bridgeIngressCntrModePortGroupId[cntrSetNum] !=
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.bridgeIngressCntrMode[cntrSetNum].portGroupId)
        {
            rc = GT_FAIL;
            goto restore;
        }
    }
    if (ppConfigCatchUpParams.cfgIngressDropCntrModePortGroupId !=
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.cfgIngressDropCntrMode.portGroupId)
    {
        rc = GT_FAIL;
        goto restore;
    }
    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        if (ppConfigCatchUpParams.tag1VidFdbEn != PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.tag1VidFdbEn)
        {
            rc = GT_FAIL;
            goto restore;
        }
        if (ppConfigCatchUpParams.portVlanfltTabAccessMode != PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.portVlanfltTabAccessMode)
        {
            rc = GT_FAIL;
            goto restore;
        }
    }
    for (portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = cpssDxChPortPhysicalPortMapIsValidGet(devNum, portNum, &isValid);
            if ((rc != GT_OK) || (isValid != GT_TRUE))
            {
                continue;
            }
        }
        else
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        }

        if ((portNum == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            continue;
        }

        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

        if (ppConfigCatchUpParams.portIfMode[portMacNum] != PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portIfMode)
        {
            rc = GT_FAIL;
            goto restore;
        }

        if (ppConfigCatchUpParams.portSpeed[portMacNum] != PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portSpeed)
        {
            rc = GT_FAIL;
            goto restore;
        }

        if (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            if ((ppConfigCatchUpParams.portIfMode[portMacNum] == CPSS_PORT_INTERFACE_MODE_NA_E) ||
                (ppConfigCatchUpParams.portSpeed[portMacNum] == CPSS_PORT_SPEED_NA_E))
            {
                continue;
            }
        }
        else
        {
            if (ppConfigCatchUpParams.portIfMode[portMacNum] == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                continue;
            }
        }

        if (ppConfigCatchUpParams.portType[portMacNum] != PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portType)
        {
            rc = GT_FAIL;
            goto restore;
        }
    }
    rc = 0;
    for (dpIndex = 0; dpIndex < PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.maxDp; dpIndex++)
    {
        if (ppConfigCatchUpParams.usedDescCredits[dpIndex] !=
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[dpIndex])
        {
            rc = GT_FAIL;
            goto restore;
        }

        if (ppConfigCatchUpParams.usedHeaderCredits[dpIndex] !=
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[dpIndex])
        {
            rc = GT_FAIL;
            goto restore;
        }

        if (ppConfigCatchUpParams.usedPayloadCredits[dpIndex] !=
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[dpIndex])
        {
            rc = GT_FAIL;
            goto restore;
        }
    }

restore:
    /* Restore PP original info */
    rc1 = prvCpssDxChSystemRecoveryCatchUpRestorePpInfo(devNum);
    if (rc == GT_OK)
    {
        rc = rc1;
    }
return rc;
}

/*******************************************************************************
* dxChEnableFdbUploadActionAndSaveFuqCurrentStatus
*
* DESCRIPTION:
*       This function configure FDB upload action for specific entry
*       and save current FUQ action status.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       actionDataPtr - pointer to action data.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on bad device.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChEnableFdbUploadActionAndSaveFuqCurrentStatus
(
    IN  GT_U8                             devNum,
    OUT PRV_CPSS_DXCH_FUQ_ACTION_DATA_STC *actionDataPtr
)
{
    GT_STATUS rc = GT_OK;

    rc = cpssDxChBrgFdbUploadEnableGet(devNum,&actionDataPtr->fdbUploadState);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Enable/Disable reading FDB entries via AU messages to the CPU*/
    rc =  cpssDxChBrgFdbUploadEnableSet(devNum, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* configure FDB to upload only this specific entry*/

    /* save vid and vid mask */
    rc =  cpssDxChBrgFdbActionActiveVlanGet(devNum,&actionDataPtr->currentVid,
                                            &actionDataPtr->currentVidMask);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set current vid and vid mask */
    rc =  cpssDxChBrgFdbActionActiveVlanSet(devNum,9,0xfff);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* save current action dev */
    rc =  cpssDxChBrgFdbActionActiveDevGet(devNum,&actionDataPtr->actDev,
                                           &actionDataPtr->actDevMask);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* set new action device */
    rc = cpssDxChBrgFdbActionActiveDevSet(devNum,30,0x1f);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* save current action interface */
    rc = cpssDxChBrgFdbActionActiveInterfaceGet(devNum,&actionDataPtr->actIsTrunk,&actionDataPtr->actIsTrunkMask,
                                                &actionDataPtr->actTrunkPort,&actionDataPtr->actTrunkPortMask);

    if (rc != GT_OK)
    {
        return rc;
    }
    /* set new action interface */
    rc =  cpssDxChBrgFdbActionActiveInterfaceSet(devNum,0,0,62,0x3f);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* save action trigger mode */
    rc = cpssDxChBrgFdbMacTriggerModeGet(devNum,&actionDataPtr->triggerMode);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* set new action trigger mode */
    rc = cpssDxChBrgFdbMacTriggerModeSet(devNum,CPSS_ACT_TRIG_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* save action mode */
    rc =  cpssDxChBrgFdbActionModeGet(devNum,&actionDataPtr->actionMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set fdb upload action mode */
    rc = cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* save action enable state*/
    rc = cpssDxChBrgFdbActionsEnableGet(devNum,&actionDataPtr->actionEnable);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* set action enable set */
    rc = cpssDxChBrgFdbActionsEnableSet(devNum,GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }
    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* save maskAuFuMsg2CpuOnNonLocal state*/
        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableGet(devNum,&actionDataPtr->maskAuFuMsg2CpuOnNonLocal);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* set maskAuFuMsg2CpuOnNonLocal state*/
        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet(devNum,GT_FALSE);
    }

    return rc;
}

/*******************************************************************************
* dxChRestoreCurrentFdbActionStatus
*
* DESCRIPTION:
*       This function restore FDB action data and apply it on the device.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       actionDataPtr - pointer to action data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChRestoreCurrentFdbActionStatus
(
    IN GT_U8    devNum,
    IN PRV_CPSS_DXCH_FUQ_ACTION_DATA_STC *actionDataPtr
)
{
    GT_STATUS rc = GT_OK;

    rc =  cpssDxChBrgFdbActionActiveVlanSet(devNum,actionDataPtr->currentVid,
                                            actionDataPtr->currentVidMask);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* restore saved action device */
    rc =  cpssDxChBrgFdbActionActiveDevSet(devNum,actionDataPtr->actDev,
                                           actionDataPtr->actDevMask);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* restore saved action interface */
    rc = cpssDxChBrgFdbActionActiveInterfaceSet(devNum,actionDataPtr->actIsTrunk,
                                                actionDataPtr->actIsTrunkMask,
                                                actionDataPtr->actTrunkPort,
                                                actionDataPtr->actTrunkPortMask);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* restore saved action trigger mode */
    rc = cpssDxChBrgFdbMacTriggerModeSet(devNum,actionDataPtr->triggerMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* restore  saved action mode */
    rc = cpssDxChBrgFdbActionModeSet(devNum,actionDataPtr->actionMode);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* restore saved action enable/disable mode */
    rc = cpssDxChBrgFdbActionsEnableSet(devNum,actionDataPtr->actionEnable);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* restore fdbUploadState enable/disable state */
    rc = cpssDxChBrgFdbUploadEnableSet(devNum,actionDataPtr->fdbUploadState);
    if (rc != GT_OK)
    {
        return rc;
    }
    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* restore  maskAuFuMsg2CpuOnNonLocal state*/
        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet(devNum,actionDataPtr->maskAuFuMsg2CpuOnNonLocal);
    }

    return rc;
}

/*******************************************************************************
* dxChAuqStatusMemoryFree
*
* DESCRIPTION:
*       This function free previously allocated AUQ status memory.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID dxChAuqStatusMemoryFree
(
    IN GT_U8    devNum
)
{
   if (auqMsgEnableStatus[devNum]->naToCpuPerPortPtr != NULL)
    {
        cpssOsFree(auqMsgEnableStatus[devNum]->naToCpuPerPortPtr);
        auqMsgEnableStatus[devNum]->naToCpuPerPortPtr = NULL;
    }
    if (auqMsgEnableStatus[devNum]->naStormPreventPortPtr != NULL)
    {
        cpssOsFree(auqMsgEnableStatus[devNum]->naStormPreventPortPtr);
        auqMsgEnableStatus[devNum]->naStormPreventPortPtr = NULL;
    }
    if (auqMsgEnableStatus[devNum] != NULL)
    {
        cpssOsFree(auqMsgEnableStatus[devNum]);
        auqMsgEnableStatus[devNum] = NULL;
    }
}

/*******************************************************************************
* dxChRestoreAuqCurrentStatus
*
* DESCRIPTION:
*       This function retieve  AUQ enable/disable message status
*       and apply it on the device.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChRestoreAuqCurrentStatus
(
    IN GT_U8    devNum
)
{
    GT_U32 i = 0;
    GT_STATUS rc = GT_OK;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /*Restore sending NA update messages to the CPU per port*/

    for(i=0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, i))
            continue;

        rc =  cpssDxChBrgFdbNaToCpuPerPortSet(devNum,(GT_U8)i,auqMsgEnableStatus[devNum]->naToCpuPerPortPtr[i]);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }
        rc =  cpssDxChBrgFdbNaStormPreventSet(devNum,(GT_U8)i,auqMsgEnableStatus[devNum]->naStormPreventPortPtr[i]);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }
    }

    /*Restore sending NA messages to the CPU indicating that the device
    cannot learn a new SA. */

    rc = cpssDxChBrgFdbNaMsgOnChainTooLongSet(devNum,auqMsgEnableStatus[devNum]->naToCpuLearnFail);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        return rc;
    }
    /* Restore the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
      message */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        rc = cpssDxChBrgFdbNaMsgVid1EnableSet(devNum,auqMsgEnableStatus[devNum]->naTag1VLANassignment);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }
    }

    /* restore sending to CPU status of AA and TA messages*/
    rc = cpssDxChBrgFdbAAandTAToCpuSet(devNum,auqMsgEnableStatus[devNum]->aaTaToCpu);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        return rc;
    }
    /* restore Sp AA message to CPU status*/
    rc =  cpssDxChBrgFdbSpAaMsgToCpuSet(devNum,auqMsgEnableStatus[devNum]->spAaMsgToCpu);
    dxChAuqStatusMemoryFree(devNum);

    return rc;
}

/*******************************************************************************
* dxChAuqFillByQuery
*
* DESCRIPTION:
*       The function fills AUQ and return the queueu state full/not full.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - the device number.
*       portGroupsBmp - bitmap of Port Groups.
*       macEntryPtr   - pointer to mac entry.
*
* OUTPUTS:
*       isAuqFullPtr - (pointer to) AUQ status:
*                       GT_TRUE - AUQ is full.
*                       GT_FALSE - otherwisw.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp or queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChAuqFillByQuery
(
    IN GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP         portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryPtr,
    OUT GT_BOOL                    *isAuqFullPtr
)
{
    GT_U32              qa_counter             = 0;
    GT_BOOL             auqIsFull              = GT_FALSE;
    GT_PORT_GROUPS_BMP  isFullPortGroupsBmp    = 0;
    GT_STATUS           rc                     = GT_OK;
    GT_PORT_GROUPS_BMP  completedPortGroupsBmp = 0;
    GT_PORT_GROUPS_BMP  succeededPortGroupsBmp = 0;

    while (auqIsFull == GT_FALSE)
    {
        /* check if AUQ full bit is set */
        rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,portGroupsBmp,CPSS_DXCH_FDB_QUEUE_TYPE_AU_E,&isFullPortGroupsBmp);
        if (rc != GT_OK)
        {
            return rc;
        }
        if((isFullPortGroupsBmp & portGroupsBmp)== portGroupsBmp)
        {
            /* queue is full */
            *isAuqFullPtr = GT_TRUE;
            return rc;
        }

        /* send quary */
        qa_counter = 0;
        do
        {
            rc =  cpssDxChBrgFdbPortGroupQaSend( devNum, portGroupsBmp, macEntryPtr);
            if(rc != GT_OK)
            {
    #ifdef ASIC_SIMULATION
                cpssOsTimerWkAfter(1);
    #endif
                qa_counter++;
                if(qa_counter > 20)
                {
                    return rc;
                }
            }
        } while (rc != GT_OK);

        /* verify that action is completed */
        completedPortGroupsBmp = 0;
        while ((completedPortGroupsBmp & portGroupsBmp)!= portGroupsBmp)
        {
            rc = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum,portGroupsBmp,&completedPortGroupsBmp,
                                                              &succeededPortGroupsBmp);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return rc;
}



/*******************************************************************************
* dxChDisableAuqAndSaveAuqCurrentStatus
*
* DESCRIPTION:
*       This function disable AUQ for messages and save current AUQ messages enable status.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDisableAuqAndSaveAuqCurrentStatus
(
    IN GT_U8    devNum
)
{
    GT_U32 i = 0;
    GT_STATUS rc = GT_OK;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* alocate memory for given device */
    auqMsgEnableStatus[devNum] = (PRV_CPSS_DXCH_AUQ_ENABLE_DATA_STC *)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_AUQ_ENABLE_DATA_STC));
    if (auqMsgEnableStatus[devNum] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    cpssOsMemSet(auqMsgEnableStatus[devNum],0,sizeof(auqMsgEnableStatus));
    auqMsgEnableStatus[devNum]->naToCpuPerPortPtr = (GT_BOOL*)cpssOsMalloc(sizeof(GT_BOOL)* PRV_CPSS_PP_MAC(devNum)->numOfPorts);
    if (auqMsgEnableStatus[devNum]->naToCpuPerPortPtr == NULL)
    {
        dxChAuqStatusMemoryFree(devNum);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    cpssOsMemSet(auqMsgEnableStatus[devNum]->naToCpuPerPortPtr,0,sizeof(GT_BOOL)* PRV_CPSS_PP_MAC(devNum)->numOfPorts);
    auqMsgEnableStatus[devNum]->naStormPreventPortPtr = (GT_BOOL*)cpssOsMalloc(sizeof(GT_BOOL)* PRV_CPSS_PP_MAC(devNum)->numOfPorts);
    if (auqMsgEnableStatus[devNum]->naStormPreventPortPtr == NULL)
    {
        dxChAuqStatusMemoryFree(devNum);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    cpssOsMemSet(auqMsgEnableStatus[devNum]->naStormPreventPortPtr,0,sizeof(GT_BOOL)* PRV_CPSS_PP_MAC(devNum)->numOfPorts);
    /*Disable sending NA update messages to the CPU per port*/

    for(i=0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, i))
            continue;

        /* at first save staus per port*/
        rc = cpssDxChBrgFdbNaToCpuPerPortGet(devNum,(GT_U8)i,&(auqMsgEnableStatus[devNum]->naToCpuPerPortPtr[i]));
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }

        rc =  cpssDxChBrgFdbNaStormPreventGet(devNum, (GT_U8)i,&(auqMsgEnableStatus[devNum]->naStormPreventPortPtr[i]));
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }

        rc =  cpssDxChBrgFdbNaToCpuPerPortSet(devNum,(GT_U8)i,GT_FALSE);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }
        rc =  cpssDxChBrgFdbNaStormPreventSet(devNum, (GT_U8)i,GT_FALSE);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }
    }

    /* save status (enabled/disabled) of sending NA messages to the CPU
       indicating that the device cannot learn a new SA */
    rc = cpssDxChBrgFdbNaMsgOnChainTooLongGet(devNum,&auqMsgEnableStatus[devNum]->naToCpuLearnFail);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        return rc;
    }
    /* disable sending NA messages to the CPU
       indicating that the device cannot learn a new SA */
    rc = cpssDxChBrgFdbNaMsgOnChainTooLongSet(devNum,GT_FALSE);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        return rc;
    }
    /* Get the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
      message */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        rc = cpssDxChBrgFdbNaMsgVid1EnableGet(devNum,&auqMsgEnableStatus[devNum]->naTag1VLANassignment);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }
        /* Disable sending of Tag1 VLAN Id assignment in vid1 field of the NA AU
          message */
        rc = cpssDxChBrgFdbNaMsgVid1EnableSet(devNum,GT_FALSE);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            return rc;
        }
    }

    /* save status of AA and AT messages */
    rc = cpssDxChBrgFdbAAandTAToCpuGet(devNum,&auqMsgEnableStatus[devNum]->aaTaToCpu);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        return rc;
    }

   /* Disable AA and AT messages */
    rc =  cpssDxChBrgFdbAAandTAToCpuSet(devNum,GT_FALSE);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        return rc;
    }

    /* save Sp AA message to CPU status*/
    rc =  cpssDxChBrgFdbSpAaMsgToCpuGet(devNum,&auqMsgEnableStatus[devNum]->spAaMsgToCpu);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        return rc;
    }

    /* Disable sending AA messages to the CPU indicating that the
    device aged-out storm prevention FDB entry */
    rc =  cpssDxChBrgFdbSpAaMsgToCpuSet(devNum,GT_FALSE);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHaAuqNonContinuousMsgModeHandle
*
* DESCRIPTION:
*       This function performs AUQ workaround after HA event. It makes PP to consider that
*       queue is full and to be ready for reprogramming.
*       The workaround should be used when application can't guarantee the same memory
*       allocated for AUQ before and after HA event.
*       Before calling this function application should disable access of device to CPU memory.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHaAuqNonContinuousMsgModeHandle
(
    IN GT_U8    devNum
)
{
    GT_STATUS                        rc;
    CPSS_MAC_ENTRY_EXT_KEY_STC       macEntry;
    GT_U32                           portGroupId;
    GT_PORT_GROUPS_BMP               portGroupsBmp = 0;
    GT_PORT_GROUPS_BMP               isFullPortGroupsBmp = 0;
    GT_BOOL                          auqIsFull = GT_FALSE;
    GT_U32                           regAddr = 0;
    GT_UINTPTR                       phyAddr;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
        enabled == GT_FALSE))
    {
        /* disable AUQ for messages and save current AUQ enable status for given device */
        rc = prvCpssDxChDisableAuqAndSaveAuqCurrentStatus(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }
        cpssOsMemSet(&macEntry,0,sizeof(CPSS_MAC_ENTRY_EXT_KEY_STC));
        macEntry.entryType                      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        macEntry.key.macVlan.vlanId             = 0;
        macEntry.key.macVlan.macAddr.arEther[0] = 0x0;
        macEntry.key.macVlan.macAddr.arEther[1] = 0x1A;
        macEntry.key.macVlan.macAddr.arEther[2] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[3] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[4] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[5] = 0xFF;

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            auqIsFull = GT_FALSE;
            /* first check if AUQ is full*/
            rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,portGroupsBmp,CPSS_DXCH_FDB_QUEUE_TYPE_AU_E,&isFullPortGroupsBmp);
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                return rc;
            }
            if((isFullPortGroupsBmp & portGroupsBmp)== portGroupsBmp)
            {
                /* WA sarts with AUQ full. In this case FIFO should be handled as well. */
                /* For this reason new AUQ of 64 messages size is defined. If there is  */
                /* something in FIFO it would be splashed into the new queue. After that*/
                /* this queue would be filled till the end by QR messages               */

                /* define queue size */
                regAddr =PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
                rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,0,31,64);
                if (rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    return rc;
                }
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;
                /* read physical AUQ address from PP */
                rc =  prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,(GT_U32*)(&phyAddr));
                if (rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    return rc;
                }
                /* define queue base address */
                rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,(GT_U32)phyAddr);
                if (rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    return rc;
                }

                /* now fill the FIFO queue */
                rc = dxChAuqFillByQuery(devNum,portGroupsBmp,&macEntry,&auqIsFull);
                if (rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    return rc;
                }
                if (auqIsFull == GT_TRUE)
                {
                    /* handle another port group */
                    continue;
                }
                else
                {
                    /* didn't succeed to fill AUQ */
                    dxChAuqStatusMemoryFree(devNum);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
            }
            /* now fill the queue */
            rc = dxChAuqFillByQuery(devNum,portGroupsBmp,&macEntry,&auqIsFull);
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                return rc;
            }
            if (auqIsFull == GT_TRUE)
            {
                /* handle another port group */
                continue;
            }
            else
            {
                /* didn't succeed to fill AUQ */
                dxChAuqStatusMemoryFree(devNum);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }
    return GT_OK;
}

/*******************************************************************************
* dxChScanFdbAndAddEntries
*
* DESCRIPTION:
*       This function scan FDB for valid entries and add special entires in order to
*       perform FUQ WA (making FUQ FULL) by optimal manner.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number.
*       portGroupsBmp        - bitmap of Port Groups.
*       fdbNumOfEntries      - number of entries in FDB
*       fuqSizeInEntries     - number of entries in current fuq
*       fdbEntryPtr          - pointer to special fdb entry
*
*
* OUTPUTS:
*       deleteEntryPtr       - pointer to boolean array contained indexes of added entries
*                              that should be deleted later.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChScanFdbAndAddEntries
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  GT_U32                        fdbNumOfEntries,
    IN  GT_U32                        fuqSizeInEntries,
    IN  CPSS_MAC_ENTRY_EXT_STC        *fdbEntryPtr,
    OUT GT_BOOL                       *deleteEntryPtr
)
{
    GT_STATUS       rc = GT_OK;
    GT_U32          numberOfFdbEntriesToAdd = 0;
    GT_BOOL         valid;
    GT_U32          index = 0;
    GT_BOOL         skip = GT_FALSE;

    /* Initialize numberOfFdbEntriesToAdd to the the maximum of FU queue size and FDB size. */
    if (fuqSizeInEntries > fdbNumOfEntries)
    {
        numberOfFdbEntriesToAdd = fdbNumOfEntries;
    }
    else
    {
        numberOfFdbEntriesToAdd = fuqSizeInEntries;
    }
    /* First scan of the FDB: find how many entries to add to the FDB */
    for (index = 0; index < fdbNumOfEntries; index++)
    {
        /* call cpss api function */
        rc = cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum, portGroupsBmp, index, &valid, &skip);
        if (rc != GT_OK)
        {
            cpssOsFree(deleteEntryPtr);
            return rc;
        }
        if ((valid == GT_TRUE) && (skip == GT_FALSE))
        {
            numberOfFdbEntriesToAdd--;
        }
        if (numberOfFdbEntriesToAdd == 0)
        {
            break;
        }
    }
    /* Second scan of the FDB: add entries */
    for (index = 0; index < fdbNumOfEntries; index++)
    {
        if (numberOfFdbEntriesToAdd == 0)
        {
            break;
        }

        rc = cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum,portGroupsBmp, index, &valid, &skip);
        if (rc != GT_OK)
        {
            cpssOsFree(deleteEntryPtr);
            return rc;
        }
        if ((valid == GT_FALSE) || (skip == GT_TRUE))
        {
            /* write the entry to the FDB */
            rc = cpssDxChBrgFdbPortGroupMacEntryWrite(devNum, portGroupsBmp, index, GT_FALSE, fdbEntryPtr);
            if (rc != GT_OK)
            {
                cpssOsFree(deleteEntryPtr);
                return rc;
            }
            deleteEntryPtr[index] = GT_TRUE;
            numberOfFdbEntriesToAdd--;
        }
    }
    return GT_OK;
}



/*******************************************************************************
* dxChFuqFillByUploadAction
*
* DESCRIPTION:
*       The function fills FUQ and return the queueu state full/not full.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - the device number.
*       portGroupsBmp - bitmap of Port Groups.
*       macEntryPtr   - pointer to mac entry.
*
* OUTPUTS:
*       isFuqFullPtr - (pointer to) FUQ status:
*                       GT_TRUE - FUQ is full.
*                       GT_FALSE - otherwisw.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp or queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChFuqFillByUploadAction
(
    IN GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP         portGroupsBmp,
    OUT GT_BOOL                    *isFuqFullPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_PORT_GROUPS_BMP isFullPortGroupsBmp;
    GT_BOOL fuqIsFull = GT_FALSE;
    GT_BOOL actFinished = GT_FALSE;
    GT_U32 trigCounter = 0;

    rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,portGroupsBmp,CPSS_DXCH_FDB_QUEUE_TYPE_FU_E,&isFullPortGroupsBmp);
    if (rc != GT_OK)
    {
        return rc;
    }
    if((isFullPortGroupsBmp & portGroupsBmp)== portGroupsBmp)
    {
        fuqIsFull = GT_TRUE;
    }
    else
    {
        fuqIsFull = GT_FALSE;
    }
    /* fill all FUQs on the device*/
    while (fuqIsFull == GT_FALSE)
    {
        /*  force the upload trigger */
        rc =  cpssDxChBrgFdbMacTriggerToggle(devNum);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* verify that action is completed */
        actFinished = GT_FALSE;
        trigCounter = 0;
        while (actFinished == GT_FALSE)
        {
            rc = cpssDxChBrgFdbTrigActionStatusGet(devNum,&actFinished);
            if(rc != GT_OK)
            {
                return rc;
            }
            if (actFinished == GT_FALSE)
            {
                trigCounter++;
            }
            else
            {
                trigCounter = 0;
                break;
            }
            if (trigCounter > 500)
            {
                rc =  prvCpssHwPpSetRegField(devNum,
                                                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0,
                                                1, 1, 0);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,portGroupsBmp,CPSS_DXCH_FDB_QUEUE_TYPE_FU_E,&isFullPortGroupsBmp);
        if (rc != GT_OK)
        {
            return rc;
        }
        if((isFullPortGroupsBmp & portGroupsBmp)== portGroupsBmp)
        {
            fuqIsFull = GT_TRUE;
        }
        else
        {
            fuqIsFull = GT_FALSE;
        }
    }
    *isFuqFullPtr = GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChHaFuqNonContinuousMsgModeHandle
*
* DESCRIPTION:
*       This function performs FUQ workaround after HA event. It makes PP to consider that
*       queue is full and to be ready for reprogramming.
*       The workaround should be used when application can't guarantee the same memory
*       allocated for FUQ before and after HA event.
*       Before calling this function application should disable access of device to CPU memory.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHaFuqNonContinuousMsgModeHandle
(
    IN GT_U8    devNum
)
{
    GT_STATUS                         rc = GT_OK;
    GT_U32                            regAddr;
    GT_BOOL                           actionCompleted = GT_FALSE;
    GT_U32                            inProcessBlocksBmp = 0;
    GT_U32                            inProcessBlocksBmp1 = 0;
    GT_U32                            fdbNumOfEntries;
    GT_BOOL                           *deleteEntryPtr = NULL;
    CPSS_MAC_ENTRY_EXT_STC            fdbEntry;
    GT_PORT_GROUPS_BMP                isFullPortGroupsBmp = 0;
    GT_PORT_GROUPS_BMP                portGroupsBmp = 0;
    GT_UINTPTR                        phyAddr;
    GT_BOOL                           fuqIsFull = GT_FALSE;
    GT_U32                            i = 0;
    GT_U32                            fuqSize = 0;
    PRV_CPSS_DXCH_FUQ_ACTION_DATA_STC actionData;
    GT_U32                            portGroupId = 0;
    GT_BOOL                           cncUploadIsHandled = GT_FALSE;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    cpssOsMemSet(&actionData,0,sizeof(PRV_CPSS_DXCH_FUQ_ACTION_DATA_STC));
    fdbNumOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb;
    /* Get address of FDB Action0 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    /* check that there is not not-finished FDB upload */
    rc = cpssDxChBrgFdbTrigActionStatusGet(devNum, &actionCompleted);
    if (rc != GT_OK)
    {
        return rc;
    }
    if(actionCompleted == GT_FALSE)
    {
        /* clear the trigger */
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, 0);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    /* check there are no CNC blocks yet being uploaded */
    rc = cpssDxChCncBlockUploadInProcessGet(devNum,&inProcessBlocksBmp);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (inProcessBlocksBmp != 0)
    {
        /* if CNC upload is under way let it be finished */
        cpssOsTimerWkAfter(10);
    }
    inProcessBlocksBmp = 0;
    deleteEntryPtr = (GT_BOOL *)cpssOsMalloc(sizeof(GT_BOOL) * fdbNumOfEntries);
    if (deleteEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* disable AUQ for messages and save current AUQ enable status for given device */
    rc = prvCpssDxChDisableAuqAndSaveAuqCurrentStatus(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* configure FDB upload action for specific entry and save current FUQ action status */
    rc = dxChEnableFdbUploadActionAndSaveFuqCurrentStatus(devNum,&actionData);
    if (rc != GT_OK)
    {
        dxChAuqStatusMemoryFree(devNum);
        cpssOsFree(deleteEntryPtr);
        return rc;
    }

    cpssOsMemSet(&fdbEntry,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));

    /* fill very specific fdb entry  */
    fdbEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    fdbEntry.key.key.macVlan.vlanId = 9;
    fdbEntry.key.key.macVlan.macAddr.arEther[0] = 0;
    fdbEntry.key.key.macVlan.macAddr.arEther[1] = 0x15;
    fdbEntry.key.key.macVlan.macAddr.arEther[2] = 0x14;
    fdbEntry.key.key.macVlan.macAddr.arEther[3] = 0x13;
    fdbEntry.key.key.macVlan.macAddr.arEther[4] = 0x12;
    fdbEntry.key.key.macVlan.macAddr.arEther[5] = 0x11;
    fdbEntry.dstInterface.devPort.hwDevNum = 30;
    fdbEntry.dstInterface.devPort.portNum = 62;
    fdbEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        cpssOsMemSet(deleteEntryPtr,0,sizeof(GT_BOOL)* fdbNumOfEntries);
        portGroupsBmp = (1 << portGroupId);
        /* first check if AUQ is full*/
        rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,portGroupsBmp,CPSS_DXCH_FDB_QUEUE_TYPE_AU_E,&isFullPortGroupsBmp);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            cpssOsFree(deleteEntryPtr);
            return rc;
        }
        if((isFullPortGroupsBmp & portGroupsBmp)== portGroupsBmp)
        {
            /* WA sarts with AUQ full. In this case FIFO can be full as well. */
            /* For this reason new AUQ of 64 messages size is defined. If there is  */
            /* something in FIFO it would be splashed into the new queue.         */

            /* define queue size */
            regAddr =PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
            rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,0,31,64);
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                cpssOsFree(deleteEntryPtr);
                return rc;
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;
            /* read physical AUQ address from PP */
            rc =  prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,(GT_U32*)(&phyAddr));
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                cpssOsFree(deleteEntryPtr);
                return rc;
            }
            /* define queue base address */
            rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,(GT_U32)phyAddr);
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                cpssOsFree(deleteEntryPtr);
                return rc;
            }
        }

        /* check if FUQ is full */
        rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,portGroupsBmp,CPSS_DXCH_FDB_QUEUE_TYPE_FU_E,&isFullPortGroupsBmp);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            cpssOsFree(deleteEntryPtr);
            return rc;
        }

        if((isFullPortGroupsBmp & portGroupsBmp)== portGroupsBmp)
        {
            /* WA sarts with FUQ full. In this case CNC upload action check is performed. */
            /* If upload action is ongoing lets it be finished by defining new fuq with   */
            /* CNC block size. (Asumtion is CNC upload command is given for different CNC */
            /* blocks sequentially  - one in a time).This process is proceeded until CNC  */
            /* is not finished.                                                           */
            /* If CNC upload is not a factor, FIFO should be handled as well.             */
            /* For this reason new FUQ of 64 messages size is defined. If there is        */
            /* something in FIFO it would be splashed into the new queue. After that      */
            /* this queue would be filled till the end by FDB upload action               */

            /* reset FDB action trigger if any */
            rc =  prvCpssHwPpSetRegField(devNum,
                                            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0,
                                            1, 1, 0);
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                cpssOsFree(deleteEntryPtr);
                return rc;
            }
            /* check if CNC upload takes place */
            rc =  cpssDxChCncPortGroupBlockUploadInProcessGet(devNum, portGroupsBmp, &inProcessBlocksBmp);
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                cpssOsFree(deleteEntryPtr);
                return rc;
            }
            if (inProcessBlocksBmp == 0)
            {
                /* cnc was not triggered and queue is full. The FIFO contents is unknown */
                /* In order to avoid CNC entries in FIFO define new queue by FIFO size   */
                fuqSize = 64;
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
                rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,0,30,fuqSize);
                if (rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    cpssOsFree(deleteEntryPtr);
                    return rc;
                }
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;
                /* read physical FUQ address from PP */
                rc =  prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,(GT_U32*)(&phyAddr));
                if (rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    cpssOsFree(deleteEntryPtr);
                    return rc;
                }
                /* define queue base address */
                rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,(GT_U32)phyAddr);
                if (rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    cpssOsFree(deleteEntryPtr);
                    return rc;
                }
            }
            else
            {
                cncUploadIsHandled = GT_TRUE;
                while (inProcessBlocksBmp != 0)
                {
                    /* only one block can be uploaded in given time */
                    inProcessBlocksBmp1 = inProcessBlocksBmp;
                    isFullPortGroupsBmp = 0;
                    rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,portGroupsBmp,CPSS_DXCH_FDB_QUEUE_TYPE_FU_E,&isFullPortGroupsBmp);
                    if (rc != GT_OK)
                    {
                        dxChAuqStatusMemoryFree(devNum);
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    if((isFullPortGroupsBmp & portGroupsBmp)!= portGroupsBmp)
                    {
                        /* queue is not full and cnc dump is not finished */
                        dxChAuqStatusMemoryFree(devNum);
                        cpssOsFree(deleteEntryPtr);
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                    fuqSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlockNumEntries + 64;/* cnc block size + FIFO */
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
                    rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,0,30,fuqSize);
                    if (rc != GT_OK)
                    {
                        dxChAuqStatusMemoryFree(devNum);
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;
                    /* read physical FUQ address from PP */
                    rc =  prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,(GT_U32*)(&phyAddr));
                    if (rc != GT_OK)
                    {
                        dxChAuqStatusMemoryFree(devNum);
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    /* define queue base address */
                    rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,(GT_U32)phyAddr);
                    if (rc != GT_OK)
                    {
                        dxChAuqStatusMemoryFree(devNum);
                        cpssOsFree(deleteEntryPtr);
                        return rc;
                    }
                    i = 0;
                    while (inProcessBlocksBmp == inProcessBlocksBmp1)
                    {
                        i++;
                        if (i > 1000)
                        {
                            dxChAuqStatusMemoryFree(devNum);
                            cpssOsFree(deleteEntryPtr);
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                        }
                        /* check if CNC upload takes place */
                        rc =  cpssDxChCncPortGroupBlockUploadInProcessGet(devNum, portGroupsBmp, &inProcessBlocksBmp);
                        if (rc != GT_OK)
                        {
                            dxChAuqStatusMemoryFree(devNum);
                            cpssOsFree(deleteEntryPtr);
                            return rc;
                        }
                    }
                }
            }
        }
        else
        {
            /* fuq is not full*/
            /* get current fuq size */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
            rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,0,30,&fuqSize);
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                cpssOsFree(deleteEntryPtr);
                return rc;
            }
        }
        /* in this point queue is not full : or from begining or new queue size 64  was defined  or  */
        /* new queue CNC size was defined.                                                           */
        /* now fill the current fuq by means of fdb upload action                                    */
        if (cncUploadIsHandled == GT_TRUE)
        {
            /* cnc upload was already handled */
            rc =  cpssDxChCncPortGroupBlockUploadInProcessGet(devNum, portGroupsBmp, &inProcessBlocksBmp);
            if (rc != GT_OK)
            {
                dxChAuqStatusMemoryFree(devNum);
                cpssOsFree(deleteEntryPtr);
                return rc;
            }
            if (inProcessBlocksBmp == 0)
            {
                /* set FDB to be the Queue owner */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
                rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 14, 1, 1);
                if (rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    cpssOsFree(deleteEntryPtr);
                    return rc;
                }
            }
            else
            {
                dxChAuqStatusMemoryFree(devNum);
                cpssOsFree(deleteEntryPtr);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

        }
        rc = dxChScanFdbAndAddEntries(devNum,portGroupsBmp,fdbNumOfEntries,fuqSize,&fdbEntry,deleteEntryPtr);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            cpssOsFree(deleteEntryPtr);
            return rc;
        }
        rc =  dxChFuqFillByUploadAction(devNum, portGroupsBmp,&fuqIsFull);
        if (rc != GT_OK)
        {
            dxChAuqStatusMemoryFree(devNum);
            cpssOsFree(deleteEntryPtr);
            return rc;
        }
        if (fuqIsFull == GT_FALSE)
        {
            /* queue is still not full*/
            dxChAuqStatusMemoryFree(devNum);
            cpssOsFree(deleteEntryPtr);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

         /* Now queue is full . Restore FDB configuration */
        for (i = 0; i < fdbNumOfEntries; i++)
        {
            if (deleteEntryPtr[i] == GT_TRUE)
            {
                rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,
                                                              portGroupId,
                                                              PRV_CPSS_DXCH_TABLE_FDB_E,
                                                              i,
                                                              0,
                                                              1,
                                                              1,
                                                              1);
                if(rc != GT_OK)
                {
                    dxChAuqStatusMemoryFree(devNum);
                    cpssOsFree(deleteEntryPtr);
                    return rc;
                }
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    cpssOsFree(deleteEntryPtr);
    /* restore configuration */
    rc =  dxChRestoreCurrentFdbActionStatus(devNum, &actionData);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc =  prvCpssDxChRestoreAuqCurrentStatus(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* Disable FUQ for each portgroup to prevent splashing FIFO into new defined queue */
        /* Further in the catch up stage fuq would be reenabled                            */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
        rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,31,1,0);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

