/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgPe.c
*
* DESCRIPTION:
*       Bridge Port Extender (BPE) DxCh cpss implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgPe.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>

/*******************************************************************************
* internal_cpssDxChBrgPeEnableSet
*
* DESCRIPTION:
*    Enables globally Bridge Port Extender (BPE) feature.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE:  BPE feature enabled.
*                 GT_FALSE: BPE feature disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPeEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32  hwValue;    /* HW Value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |CPSS_XCAT_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    hwValue = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssHwPpSetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     egrTxQConf.br802_1ConfigRegs.globalConfig,
                0, 1, hwValue);
}

/*******************************************************************************
* cpssDxChBrgPeEnableSet
*
* DESCRIPTION:
*    Enables globally Bridge Port Extender (BPE) feature.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE:  BPE feature enabled.
*                 GT_FALSE: BPE feature disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPeEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgPeEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPeEnableGet
*
* DESCRIPTION:
*    Gets Bridge Port Extender (BPE) global enabling status.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE:  BPE feature enabled.
*                                 GT_FALSE: BPE feature disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPeEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;       /* return code  */
    GT_U32    hwValue;  /* HW Value     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |CPSS_XCAT_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    rc = prvCpssHwPpGetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     egrTxQConf.br802_1ConfigRegs.globalConfig,
                0, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE ;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgPeEnableGet
*
* DESCRIPTION:
*    Gets Bridge Port Extender (BPE) global enabling status.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE:  BPE feature enabled.
*                                 GT_FALSE: BPE feature disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPeEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgPeEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPePortEnableSet
*
* DESCRIPTION:
*    Enables per port Bridge Port Extender (BPE) feature.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*       enable  - GT_TRUE:  port enabled for BPE feature.
*                 GT_FALSE: port disabled for BPE feature.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPePortEnableSet
(
    IN GT_U8                  devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL                enable
)
{
    GT_STATUS rc;       /* return code  */
    GT_U32    hwValue;  /* HW Value     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |CPSS_XCAT_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    hwValue = (enable == GT_TRUE) ? 1 : 0;

    rc = prvCpssHwPpSetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.srcPortBpeEnable,
                portNum, 1, hwValue);

    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssHwPpSetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.perPortBpeEnable,
                portNum, 1, hwValue);
}

/*******************************************************************************
* cpssDxChBrgPePortEnableSet
*
* DESCRIPTION:
*    Enables per port Bridge Port Extender (BPE) feature.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*       enable  - GT_TRUE:  port enabled for BPE feature.
*                 GT_FALSE: port disabled for BPE feature.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPePortEnableSet
(
    IN GT_U8                  devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL                enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPePortEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgPePortEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPePortEnableGet
*
* DESCRIPTION:
*    Gets Bridge Port Extender (BPE) enabling status per port.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*
* OUTPUTS:
*       enablePtr - (pointer to) GT_TRUE:  port enabled for BPE feature.
*                                GT_FALSE: port disabled for BPE feature.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device or port number
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPePortEnableGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL                *enablePtr
)
{
    GT_STATUS rc;       /* return code  */
    GT_U32    hwValue;  /* HW Value     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |CPSS_XCAT_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    rc = prvCpssHwPpGetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.srcPortBpeEnable,
                portNum, 1, &hwValue);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE ;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgPePortEnableGet
*
* DESCRIPTION:
*    Gets Bridge Port Extender (BPE) enabling status per port.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*
* OUTPUTS:
*       enablePtr - (pointer to) GT_TRUE:  port enabled for BPE feature.
*                                GT_FALSE: port disabled for BPE feature.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device or port number
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPePortEnableGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL                *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPePortEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgPePortEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPePortPcidMcFilterEnableSet
*
* DESCRIPTION:
*    Configures <port,PCID> pair for downsteam multicast source filtering.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*       pcid           - Port E-Channel Identifier (APPLICABLE RANGES: 0..4095)
*       mcFilterEnable - GT_TRUE:  Multicast source filtering is enabled for the
*                                  <port,PCID> pair.
*                      - GT_FALSE: Multicast source filtering is disabled for the
*                                 <port,PCID> pair.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device, port number or PCID value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPePortPcidMcFilterEnableSet
(
    IN GT_U8                  devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32                 pcid,
    IN GT_BOOL                mcFilterEnable
)
{
    GT_U32    hwValue;  /* HW Value     */
    GT_U32    bitIndex;/* bit index in the register for the portNum*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |CPSS_XCAT_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.bpePointToPointEChannels <= pcid)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    bitIndex = (portNum == CPSS_CPU_PORT_NUM_CNS) ? 28 : portNum;

    hwValue = (mcFilterEnable == GT_TRUE) ? 1 : 0;

    return prvCpssHwPpSetRegField(devNum,
          PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.ingressEcidTableBase +
                                     pcid * 0x10,
          bitIndex, 1, hwValue);
}

/*******************************************************************************
* cpssDxChBrgPePortPcidMcFilterEnableSet
*
* DESCRIPTION:
*    Configures <port,PCID> pair for downsteam multicast source filtering.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*       pcid           - Port E-Channel Identifier (APPLICABLE RANGES: 0..4095)
*       mcFilterEnable - GT_TRUE:  Multicast source filtering is enabled for the
*                                  <port,PCID> pair.
*                      - GT_FALSE: Multicast source filtering is disabled for the
*                                 <port,PCID> pair.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device, port number or PCID value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPePortPcidMcFilterEnableSet
(
    IN GT_U8                  devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32                 pcid,
    IN GT_BOOL                mcFilterEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPePortPcidMcFilterEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, pcid, mcFilterEnable));

    rc = internal_cpssDxChBrgPePortPcidMcFilterEnableSet(devNum, portNum, pcid, mcFilterEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, pcid, mcFilterEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPePortPcidMcFilterEnableGet
*
* DESCRIPTION:
*    Gets downsteam multicast source filtering enabling status for <port,PCID> pair.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       pcid    - Port E-Channel Identifier (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       mcFilterEnablePtr - (pointer to)
*                           GT_TRUE:  Multicast source filtering is enabled for the
*                                     <port,PCID> pair.
*                           GT_FALSE: Multicast source filtering is disabled for the
*                                     <port,PCID> pair.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device, port number or PCID value
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPePortPcidMcFilterEnableGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32                 pcid,
    OUT GT_BOOL                *mcFilterEnablePtr
)
{
    GT_STATUS rc;   /* return code  */
    GT_U32    hwValue;  /* HW Value     */
    GT_U32    bitIndex;/* bit index in the register for the portNum*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |CPSS_XCAT_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(mcFilterEnablePtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.bpePointToPointEChannels <= pcid)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    bitIndex = (portNum == CPSS_CPU_PORT_NUM_CNS) ? 28 : portNum;

    rc = prvCpssHwPpGetRegField(devNum,
          PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.ingressEcidTableBase +
                                     pcid * 0x10,
          bitIndex, 1, &hwValue);

    if (rc != GT_OK)
    {
        return rc;
    }

    *mcFilterEnablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE ;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgPePortPcidMcFilterEnableGet
*
* DESCRIPTION:
*    Gets downsteam multicast source filtering enabling status for <port,PCID> pair.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       pcid    - Port E-Channel Identifier (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       mcFilterEnablePtr - (pointer to)
*                           GT_TRUE:  Multicast source filtering is enabled for the
*                                     <port,PCID> pair.
*                           GT_FALSE: Multicast source filtering is disabled for the
*                                     <port,PCID> pair.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device, port number or PCID value
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPePortPcidMcFilterEnableGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32                 pcid,
    OUT GT_BOOL                *mcFilterEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPePortPcidMcFilterEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, pcid, mcFilterEnablePtr));

    rc = internal_cpssDxChBrgPePortPcidMcFilterEnableGet(devNum, portNum, pcid, mcFilterEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, pcid, mcFilterEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet
*
* DESCRIPTION:
*       Associate the cascade port (not DSA cascade port) with a trunkId.
*       this trunk id should represent the 'uplink trunk'
*       The purpose as described in the Functional specification is for "Trunk-ID for E-Tag<Ingress_E-CID_base>
*       assignment for multi-destination traffic"
*       NOTE: this function MUST not be called on 'Control Bridge'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number
*       cascadePortNum - physical port number of the BPE 802.1br cascade port (not DSA cascade port)
*       enable         - GT_TRUE:  associate the trunkId to cascadePortNum.
*                      - GT_FALSE: disassociate the trunkId from cascadePortNum.
*       trunkId        - the trunkId (of the uplink trunk)
*                        note: relevant only when enable == GT_TRUE
*                        APPLICABLE RANGE: 1..(4K-1).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad devNum, cascadePortNum
*       GT_OUT_OF_RANGE          - trunkId out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     cascadePortNum,
    IN GT_BOOL                  enable,
    IN GT_TRUNK_ID              trunkId
)
{
    GT_STATUS rc;   /* return code  */
    CPSS_TRUNK_MEMBER_STC   trunkMember;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,cascadePortNum);
    if (enable == GT_FALSE)
    {
        /* disassociate the port from the trunk of the 'uplink' */
        trunkMember.port = cascadePortNum;
        trunkMember.hwDevice = PRV_CPSS_HW_DEV_NUM_MAC(devNum);

        /* check if this cascade port is part of trunk by itself ! */
        rc = cpssDxChTrunkDbIsMemberOfTrunk(devNum,&trunkMember,&trunkId);
        if (rc == GT_OK)
        {
            /* trunkId was set */
        }
        else
        {
            trunkId = 0;
        }
    }
    else
    {
        /* trunkId from the caller */
        if (trunkId == 0)
        {
            /* non valid value for trunk . return GT_OUT_OF_RANGE to be the same as if trunkId >= 4k */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }

    /* set the trunkId in the port physical table*/
    return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
                                        cascadePortNum,/*global port*/
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_TRUNK_ID_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        trunkId);
}

/*******************************************************************************
* cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet
*
* DESCRIPTION:
*       Associate the cascade port (not DSA cascade port) with a trunkId.
*       this trunk id should represent the 'uplink trunk'
*       The purpose as described in the Functional specification is for "Trunk-ID for E-Tag<Ingress_E-CID_base>
*       assignment for multi-destination traffic"
*       NOTE: this function MUST not be called on 'Control Bridge'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number
*       cascadePortNum - physical port number of the BPE 802.1br cascade port (not DSA cascade port)
*       enable         - GT_TRUE:  associate the trunkId to cascadePortNum.
*                      - GT_FALSE: disassociate the trunkId from cascadePortNum.
*       trunkId        - the trunkId (of the uplink trunk)
*                        note: relevant only when enable == GT_TRUE
*                        APPLICABLE RANGE: 1..(4K-1).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad devNum, cascadePortNum
*       GT_OUT_OF_RANGE          - trunkId out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     cascadePortNum,
    IN GT_BOOL                  enable,
    IN GT_TRUNK_ID              trunkId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cascadePortNum, enable, trunkId));

    rc = internal_cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableSet(devNum, cascadePortNum, enable, trunkId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cascadePortNum, enable, trunkId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet
*
* DESCRIPTION:
*       Get the associate trunkId with the cascade port (not DSA cascade port) .
*       this trunk id should represent the 'uplink trunk'
*       The purpose as described in the Functional specification is for "Trunk-ID for E-Tag<Ingress_E-CID_base>
*       assignment for multi-destination traffic"
*       NOTE: this function MUST not be called on 'Control Bridge'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number
*       cascadePortNum - physical port number of the BPE 802.1br cascade port (not DSA cascade port)
*
* OUTPUTS:
*       enablePtr      - (pointer to) indication that trunkId associate to the cascadePortNum.
*                      - GT_TRUE:  trunkId associate to the cascadePortNum.
*                      - GT_FALSE: trunkId is not associate to the cascadePortNum.
*       trunkIdPtr     - the trunkId (of the uplink trunk)
*                        note: relevant only when (*enablePtr) == GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad devNum, cascadePortNum
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     cascadePortNum,
    OUT GT_BOOL                  *enablePtr,
    OUT GT_TRUNK_ID              *trunkIdPtr
)
{
    GT_STATUS rc;   /* return code  */
    CPSS_TRUNK_MEMBER_STC   trunkMember;
    GT_TRUNK_ID trunkId,haTrunkId;
    GT_U32  value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,cascadePortNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(trunkIdPtr);

    /* disassociate the port from the trunk of the 'uplink' */
    trunkMember.port = cascadePortNum;
    trunkMember.hwDevice = PRV_CPSS_HW_DEV_NUM_MAC(devNum);

    /* check if this cascade port is part of trunk by itself ! */
    rc = cpssDxChTrunkDbIsMemberOfTrunk(devNum,&trunkMember,&trunkId);
    if (rc == GT_OK)
    {
        /* trunkId was set */
    }
    else
    {
        trunkId = 0;
    }

    /* set the trunkId in the port physical table*/
    rc =  prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
                                        cascadePortNum,/*global port*/
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_TRUNK_ID_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    haTrunkId = (GT_TRUNK_ID)value;

    if (haTrunkId != trunkId)
    {
        *enablePtr = GT_TRUE;
        *trunkIdPtr = haTrunkId;

        if (haTrunkId == 0)
        {
            /* the port is member in 'orig trunk'
               (because trunkId != haTrunkId --> so trunkId != 0) ...
               but the 'HA' state 'no in trunk' ...
               how can this happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        *enablePtr = GT_FALSE;
        *trunkIdPtr = 0;/*don't care*/
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet
*
* DESCRIPTION:
*       Get the associate trunkId with the cascade port (not DSA cascade port) .
*       this trunk id should represent the 'uplink trunk'
*       The purpose as described in the Functional specification is for "Trunk-ID for E-Tag<Ingress_E-CID_base>
*       assignment for multi-destination traffic"
*       NOTE: this function MUST not be called on 'Control Bridge'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number
*       cascadePortNum - physical port number of the BPE 802.1br cascade port (not DSA cascade port)
*
* OUTPUTS:
*       enablePtr      - (pointer to) indication that trunkId associate to the cascadePortNum.
*                      - GT_TRUE:  trunkId associate to the cascadePortNum.
*                      - GT_FALSE: trunkId is not associate to the cascadePortNum.
*       trunkIdPtr     - the trunkId (of the uplink trunk)
*                        note: relevant only when (*enablePtr) == GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad devNum, cascadePortNum
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     cascadePortNum,
    OUT GT_BOOL                  *enablePtr,
    OUT GT_TRUNK_ID              *trunkIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cascadePortNum, enablePtr, trunkIdPtr));

    rc = internal_cpssDxChBrgPeCascadePortAssociateToUplinkTrunkEnableGet(devNum, cascadePortNum, enablePtr, trunkIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cascadePortNum, enablePtr, trunkIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

