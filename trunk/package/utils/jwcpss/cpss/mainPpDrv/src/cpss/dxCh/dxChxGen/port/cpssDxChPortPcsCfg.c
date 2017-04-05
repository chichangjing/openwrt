/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPcsCfg.c
*
* DESCRIPTION:
*       CPSS implementation for PCS configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>

#include <common/siliconIf/mvSiliconIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <private/mvPortModeElements.h>
#include <mvHwsPortCfgIf.h>

/*******************************************************************************
* prvCpssDxChPortHwsPcsTypeGet
*
* DESCRIPTION:
*       Get PCS unit type used by port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*
* OUTPUTS:
*       pcsTypePtr - (ptr to) current PCS type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - statePtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortHwsPcsTypeGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT MV_HWS_PORT_PCS_TYPE    *pcsTypePtr
)
{
    CPSS_PORT_SPEED_ENT speed;
    GT_U32              portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E |
                                          CPSS_CH1_DIAMOND_E | CPSS_CH2_E |
                                          CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E |
                                          CPSS_XCAT2_E );
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    speed = PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum);
    switch(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum))
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            *pcsTypePtr = GPCS;
            break;
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
        case CPSS_PORT_INTERFACE_MODE_HX_E:
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            *pcsTypePtr = XPCS;
            break;
        case CPSS_PORT_INTERFACE_MODE_KR_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_10000_E:
                case CPSS_PORT_SPEED_20000_E:
                case CPSS_PORT_SPEED_40000_E:
                    *pcsTypePtr = MMPCS;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_HGL_E:
            switch(speed)
            {
                case CPSS_PORT_SPEED_15000_E:
                case CPSS_PORT_SPEED_16000_E:
                    *pcsTypePtr = HGLPCS;
                    break;
                case CPSS_PORT_SPEED_40000_E:
                    *pcsTypePtr = MMPCS;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PORT_INTERFACE_MODE_NO_SERDES_PORT_E:
        case CPSS_PORT_INTERFACE_MODE_SR_LR_E:
        case CPSS_PORT_INTERFACE_MODE_XHGS_E:
        case CPSS_PORT_INTERFACE_MODE_XHGS_SR_E:
            *pcsTypePtr = MMPCS;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* isPortInXgMode
*
* DESCRIPTION:
*       Returns mode for specified XG/HX port on specified device.
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE  - The port is in XG mode.
*       GT_FALSE - The port is in HX/QX mode.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL isPortInXgMode
(
    IN  GT_U8  dev,
    IN  GT_PHYSICAL_PORT_NUM port
)
{
    GT_STATUS rc;           /* return code      */
    GT_U32    regAddr;      /* register address */
    GT_U32    regValue;     /* register content */
    GT_U32    portMacNum;   /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(dev, port, portMacNum);

    if (PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(dev,portMacNum) == PRV_CPSS_XG_PORT_HX_QX_ONLY_E)
        return GT_FALSE;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->macRegs.hxPortGlobalConfig;
    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        return GT_TRUE;

    /* at this point the port can be either XG or HX/QX - read port25Mode field
       to get the mode */
    rc = prvCpssDrvHwPpPortGroupGetRegField(
        dev, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portMacNum),
        regAddr, 15, 1, &regValue);
    if (rc != GT_OK)
    {
        return GT_FALSE;
    }

    return BIT2BOOL_MAC(regValue);
}

/*******************************************************************************
* prvCpssDxChPortPcsLoopbackEnableSet
*
* DESCRIPTION:
*       Set the PCS Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPcsLoopbackEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL     enable
)
{
    GT_U32 regAddr;     /* register address                    */
    GT_U32 value;       /* value to write into the register    */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U32          portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    value = BOOL2BIT_MAC(enable);

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 6;
    }

    if (isPortInXgMode (devNum, portNum) == GT_FALSE)  /* HX port */
    {
        if((25 == portNum) || (26 == portNum))
        {
            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

            /* First reset the HX port PCS */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
                hxPortConfig0[portNum-25];
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 11, 1, 1) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }

            /* Now set the MACLoopBackEn field */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
                hxPortConfig1[portNum-25];
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 7, 1, value) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }

            /* Finally set the HX PCS bit to "not reset" */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
                hxPortConfig0[portNum-25];
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 11, 1, 0) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            return GT_OK;
        }
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 13;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 13;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/******************************************************************************
* internal_cpssDxChPortPcsLoopbackModeSet
*
* DESCRIPTION:
*       Configure loopback of specific type on PCS.
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
*       mode      - define loopback type or no loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortPcsLoopbackModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT    mode
)
{
    GT_STATUS               rc;         /* return code */
    GT_U32                  portGroupId;/* core number of port */
    GT_U32                  localPort;  /* port number in local core */
    MV_HWS_PORT_STANDARD    portMode;   /* port interface in HWS format */
    MV_HWS_PORT_LB_TYPE     lbType;     /* loopback type in HWS format */
    GT_U32                  portMacNum;      /* MAC number */    
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;/* current interface of port */
    CPSS_PORT_SPEED_ENT     speed;/* current speed of port */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                          CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                          CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum);
    speed =  PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum);
    rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
    if(rc != GT_OK)
        return rc;

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                     PRV_CPSS_DXCH_LION2_GIGE_MAC_LINK_STATUS_WA_E))
    {
        rc = prvCpssDxChPortLion2GeLinkStatusWaEnableSet(devNum, portNum,
                                     (CPSS_DXCH_PORT_PCS_LOOPBACK_TX2RX_E != mode));
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                               PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E)
       || (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
       || (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        if(CPSS_DXCH_PORT_PCS_LOOPBACK_TX2RX_E == mode)
        {/* restore application configuration for force link down if loopback enabled */
            rc = prvCpssDxChPortForceLinkDownEnableSet(devNum,portNum,
           CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                              info_PRV_CPSS_DXCH_LION2_DISMATCH_PORT_LINK_WA_E.
                                portForceLinkDownBmpPtr,portMacNum));
            if(rc != GT_OK)
                return rc;
        }
    }

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLoopbackSet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], lpPlace[%d], lbType[%d])",devNum, portGroupId, localPort, portMode, HWS_PCS, DISABLE_LB);
    /* clear any old loopback type - must because of mvHwsSerdesLoopback
        internal implementation */
    rc = mvHwsPortLoopbackSet(devNum, portGroupId, localPort, portMode,
                              HWS_PCS, DISABLE_LB);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
    }

    switch(mode)
    {
        case CPSS_DXCH_PORT_PCS_LOOPBACK_DISABLE_E:
            lbType = DISABLE_LB;
            break;
        case CPSS_DXCH_PORT_PCS_LOOPBACK_TX2RX_E:
            lbType = TX_2_RX_LB;
            break;
        case CPSS_DXCH_PORT_PCS_LOOPBACK_RX2TX_E:
            if(PRV_CPSS_PORT_GE_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            lbType = RX_2_TX_LB;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(lbType != DISABLE_LB)
    {
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLoopbackSet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], lpPlace[%d], lbType[%d])",devNum, portGroupId, localPort, portMode, HWS_PCS, lbType);
        rc = mvHwsPortLoopbackSet(devNum, portGroupId, localPort, portMode,
                                  HWS_PCS, lbType);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                               PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E))
    {
        if((CPSS_PORT_SPEED_10000_E == speed) &&
           ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) ||
               (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode) ||
            (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode) ||
            (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E == ifMode)))
        {
            if(CPSS_DXCH_PORT_PCS_LOOPBACK_TX2RX_E != mode)
            {/* run WA again to configure force link down state appropriate for
                current state of port */
                return prvCpssDxChPortLion2LinkFixWa(devNum, portNum);
            }
        }
    }

    return GT_OK;
}

/******************************************************************************
* cpssDxChPortPcsLoopbackModeSet
*
* DESCRIPTION:
*       Configure loopback of specific type on PCS.
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
*       mode      - define loopback type or no loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS cpssDxChPortPcsLoopbackModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT    mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPcsLoopbackModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode));

    rc = internal_cpssDxChPortPcsLoopbackModeSet(devNum, portNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChPortPcsLoopbackModeGet
*
* DESCRIPTION:
*       Configure loopback of specific type on PCS.
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
*       modePtr - current loopback type or no loopback
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_BAD_PTR       - modePtr == NULL
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortPcsLoopbackModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    OUT CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT    *modePtr
)
{
    GT_STATUS               rc;         /* return code */
    GT_U32                  portGroupId;/* core number of port */
    GT_U32                  localPort;  /* port number in local core */
    MV_HWS_PORT_STANDARD    portMode;   /* port interface in HWS format */
    MV_HWS_PORT_LB_TYPE     lbType;     /* loopback type in HWS format */
    GT_U32                  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                          CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                          CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    rc = prvCpssLion2CpssIfModeToHwsTranslate(
                                PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum),
                                PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum),
                                &portMode);
    if(rc != GT_OK)
        return rc;

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLoopbackStatusGet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], lpPlace[%d], *lbType)",devNum, portGroupId, localPort, portMode, HWS_PCS, &lbType);
    rc = mvHwsPortLoopbackStatusGet(devNum, portGroupId, localPort, portMode,
                                    HWS_PCS, &lbType);
    if(rc != GT_OK)
	{
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
	}

    switch(lbType)
    {
        case DISABLE_LB:
            *modePtr = CPSS_DXCH_PORT_PCS_LOOPBACK_DISABLE_E;
            break;
        case TX_2_RX_LB:
            *modePtr = CPSS_DXCH_PORT_PCS_LOOPBACK_TX2RX_E;
            break;
        case RX_2_TX_LB:
            *modePtr = CPSS_DXCH_PORT_PCS_LOOPBACK_RX2TX_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/******************************************************************************
* cpssDxChPortPcsLoopbackModeGet
*
* DESCRIPTION:
*       Configure loopback of specific type on PCS.
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
*       modePtr - current loopback type or no loopback
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_BAD_PTR       - modePtr == NULL
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS cpssDxChPortPcsLoopbackModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    OUT CPSS_DXCH_PORT_PCS_LOOPBACK_MODE_ENT    *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPcsLoopbackModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modePtr));

    rc = internal_cpssDxChPortPcsLoopbackModeGet(devNum, portNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortXgPcsResetStateSet
*
* DESCRIPTION:
*       Set XPCS Reset state of XG unit including internal features.
*
* APPLICABLE DEVICES:
*       DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       state    - Reset state
*               GT_TRUE   - Port Xpcs is under Reset
*               GT_FALSE - Port Xpcs is Not under Reset, normal operation
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
*       XLG doesn't need such treatment, because it has just one bit for reset.
*       This API for internal usage purposed to run on start and finish of port
*           ifMode & speed reconfiguration.
*******************************************************************************/
GT_STATUS prvCpssDxChPortXgPcsResetStateSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 state
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* register address */
    GT_U32      value;          /* value to write into the register */
    GT_U32      portGroupId;    /*the port group Id - support multi-port-groups
                                    device */
    GT_U32      portMacNum;              /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E
                                          | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_LION2_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))

    {
        return cpssDxChPortPcsResetSet(devNum, portNum,
                                       CPSS_PORT_PCS_RESET_MODE_ALL_E,
                                       state);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum,
                                                                    portMacNum);
        portGroupId =
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portNum);

        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_XLG_E)
        {
            value = BOOL2BIT_MAC(state);
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->xlgRegs.pcs40GRegs.commonCtrl;
            if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                    9, 1, value);
            if (rc != GT_OK)
                return rc;
        }
        else
        {
            value = BOOL2BIT_MAC(!state);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                          macRegs.perPortRegs[portMacNum].xgGlobalConfReg0;
            if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        0, 1, value);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,10);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortXgPcsResetStateGet
*
* DESCRIPTION:
*       Set XPCS Reset state of XG unit including internal features.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*
* OUTPUTS:
*       statePtr - Current reset state
*               GT_TRUE   - Port Xpcs is under Reset
*               GT_FALSE - Port Xpcs is Not under Reset, normal operation
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - statePtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortXgPcsResetStateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *statePtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      portGroupId;    /*the port group Id - support multi-port-groups
                                    device */
    GT_U32      portMacNum;              /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E |
                                          CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    portGroupId =
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                    portNum);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_LION2_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        GT_U32    macNum;       /* MAC unit number used now by port */
        GT_U32    pcsNum;       /* PCS unit number used now by port */
        GT_U32    sdVecSize;    /* number of serdeses used now by port */
        GT_U32    *sdVectorPtr; /* ptr to array of serdeses used now by port */
        GT_U32    localPort;    /* number of port in local core */
        MV_HWS_PORT_STANDARD    portMode;   /* port ifMode in HWS format */
        MV_HWS_PORT_PCS_TYPE    pcsType; /* PCS type used by port according to
                                            current interface */
        GT_U32                  numOfLanes;

        rc = prvCpssLion2CpssIfModeToHwsTranslate(
                                    PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum),
                                    PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum),
                                    &portMode);
        if(rc != GT_OK)
        {
            return rc;
        }

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);
        CPSS_LOG_INFORMATION_MAC("Calling: hwsPortsParamsCfgGet(devNum[%d], portGroup[%d], portNum[%d], portMode[%d], *macNum, *pcsNum, *sdVecSize, **sdVector)", devNum, portGroupId, localPort, portMode);
        rc = hwsPortsParamsCfgGet(devNum, portGroupId,
                                  localPort, portMode, &macNum,
                                  &pcsNum, &sdVecSize, &sdVectorPtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }

        rc = prvCpssDxChPortHwsPcsTypeGet(devNum, portNum, &pcsType);
        if(rc != GT_OK)
        {
            return rc;
        }

        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPcsActiveStatusGet(devNum[%d], portGroup[%d], pcsNum[%d], pcsType[%d], *numOfLanes)", devNum, portGroupId, pcsNum, pcsType);
        rc = mvHwsPcsActiveStatusGet(devNum, portGroupId, pcsNum, pcsType,
                                     &numOfLanes);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
        *statePtr = (0 == numOfLanes) ? GT_TRUE : GT_FALSE;
    }
    else
    {
        GT_U32      regAddr;        /* register address */
        GT_U32      value;          /* value to write into the register */

        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_XLG_E)
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->xlgRegs.pcs40GRegs.commonCtrl;
            if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                    9, 1, &value);
            if (rc != GT_OK)
                return rc;

            *statePtr = BIT2BOOL_MAC(value);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                          macRegs.perPortRegs[portMacNum].xgGlobalConfReg0;
            if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                        0, 1, &value);
            if(rc != GT_OK)
            {
                return rc;
            }

            *statePtr = !BIT2BOOL_MAC(value);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortPcsResetSet
*
* DESCRIPTION:
*       Set/unset the PCS reset for given mode on port.
*
* APPLICABLE DEVICES:
*       xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       mode    - Tx/Rx/All
*       state   - If GT_TRUE, enable reset
*                   If GT_FALSE, disable reset
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
*   Reset only PCS unit used by port for currently configured interface.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPcsResetSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_PCS_RESET_MODE_ENT   mode,
    IN  GT_BOOL                        state
)
{
    GT_STATUS rc;           /* return code */
    GT_U32    macNum;       /* MAC unit number used now by port */
    GT_U32    pcsNum;       /* PCS unit number used now by port */
    GT_U32    sdVecSize;    /* number of serdeses used now by port */
    GT_U32    *sdVectorPtr; /* ptr to array of serdeses used now by port */
    GT_U32    portGroup;    /* local core number */
    GT_U32    localPort;    /* number of port in local core */
    MV_HWS_RESET action;    /* reset state in HWS format */
    MV_HWS_PORT_STANDARD    portMode;   /* port ifMode in HWS format */
    MV_HWS_PORT_PCS_TYPE    pcsType; /* PCS unit type used by port in HWS format */
    GT_U32                  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E |
                                          CPSS_CH1_DIAMOND_E | CPSS_CH2_E |
                                          CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E |
                                          CPSS_XCAT2_E );
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    rc = prvCpssLion2CpssIfModeToHwsTranslate(
                                PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum),
                                PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum),
                                &portMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portMacNum);

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    rc = hwsPortsParamsCfgGet(devNum, portGroup,
                              localPort, portMode, &macNum,
                              &pcsNum, &sdVecSize, &sdVectorPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    action = (GT_TRUE == state) ? RESET : UNRESET;
    rc = prvCpssDxChPortHwsPcsTypeGet(devNum, portNum, &pcsType);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(mode)
    {
        case CPSS_PORT_PCS_RESET_MODE_RX_E:
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPcsRxReset(devNum[%d], portGroup[%d], pcsNum[%d], pcsType[%d], action[%d])", devNum, portGroup, pcsNum, pcsType, action);
            rc = mvHwsPcsRxReset(devNum, portGroup, pcsNum, pcsType, action);
            break;

        case CPSS_PORT_PCS_RESET_MODE_TX_E:
            CPSS_TBD_BOOKMARK_LION2
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);

        case CPSS_PORT_PCS_RESET_MODE_ALL_E:
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPcsReset(devNum[%d], portGroup[%d], pcsNum[%d], pcsType[%d], action[%d])", devNum, portGroup, pcsNum, pcsType, action);
            rc = mvHwsPcsReset(devNum, portGroup, pcsNum, pcsType, action);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortPcsResetSet
*
* DESCRIPTION:
*       Set/unset the PCS reset for given mode on port.
*
* APPLICABLE DEVICES:
*       xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       mode    - Tx/Rx/All
*       state   - If GT_TRUE, enable reset
*                   If GT_FALSE, disable reset
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
*   Reset only PCS unit used by port for currently configured interface.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPcsResetSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_PCS_RESET_MODE_ENT   mode,
    IN  GT_BOOL                        state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPcsResetSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode, state));

    rc = internal_cpssDxChPortPcsResetSet(devNum, portNum, mode, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cpssDxChPortPcsResetGet
*
* DESCRIPTION:
*       Get PCS reset state for given mode of port.
*
* APPLICABLE DEVICES:
*       xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - Tx/Rx/All
*
* OUTPUTS:
*       statePtr     - If GT_TRUE, PCS reset for given mode set
*                      If GT_FALSE, PCS not in reset for given mode
*                      if (mode == All) GT_FALSE means at least one mode not in
*                      reset
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - statePtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   Get reset state of PCS unit used by port for currently configured interface.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPcsResetGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_PCS_RESET_MODE_ENT   mode,
    OUT GT_BOOL                 *statePtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E |
                                          CPSS_CH1_DIAMOND_E | CPSS_CH2_E |
                                          CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E |
                                          CPSS_XCAT2_E );
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    CPSS_TBD_BOOKMARK_LION2 /* implementation needed */
    (GT_VOID)mode;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortPcsGearBoxStatusGet
*
* DESCRIPTION:
*   Return PCS Gear Box lock status (true - locked /false - not locked).
*
* APPLICABLE DEVICES:
*   Lion2; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*   DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*   devNum - physical device number
*   portNum - physical port number
*
* OUTPUTS:
*   gbLockPtr - Gear Box lock state on serdes:
*                       GT_TRUE  - locked;
*                       GT_FALSE - not locked.
*
* RETURNS:
*   GT_OK                       - on success.
*   GT_BAD_PARAM                - on bad parameters
*   GT_BAD_PTR                  - gbLockPtr == NULL
*   GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPcsGearBoxStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *gbLockPtr
)
{
    GT_STATUS               rc;   /* return code */
    GT_U32                  portGroup; /* local core number */
    GT_U32                  phyPortNum; /* number of port in local core */
    MV_HWS_PORT_STANDARD    portMode;   /* port ifMode in HWS format */
    GT_U32                  portMacNum;              /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                          | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    rc = prvCpssLion2CpssIfModeToHwsTranslate(
                                PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum),
                                PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum),
                                &portMode);
    if(rc != GT_OK)
        return rc;

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortCheckGearBox(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], *laneLock)", devNum, portGroup, phyPortNum, portMode);
    rc = mvHwsPortCheckGearBox(devNum, portGroup, phyPortNum, portMode, gbLockPtr);

	if(rc != GT_OK)
	{
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
	}
    return rc;
}

/*******************************************************************************
* cpssDxChPortPcsGearBoxStatusGet
*
* DESCRIPTION:
*   Return PCS Gear Box lock status (true - locked /false - not locked).
*
* APPLICABLE DEVICES:
*   Lion2; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*   DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*   devNum - physical device number
*   portNum - physical port number
*
* OUTPUTS:
*   gbLockPtr - Gear Box lock state on serdes:
*                       GT_TRUE  - locked;
*                       GT_FALSE - not locked.
*
* RETURNS:
*   GT_OK                       - on success.
*   GT_BAD_PARAM                - on bad parameters
*   GT_BAD_PTR                  - gbLockPtr == NULL
*   GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPcsGearBoxStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *gbLockPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPcsGearBoxStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, gbLockPtr));

    rc = internal_cpssDxChPortPcsGearBoxStatusGet(devNum, portNum, gbLockPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, gbLockPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* cpssDxChPortPcsSyncStatusGet
*
* DESCRIPTION:
*   Return PCS Sync status from XGKR sync block.
*
* APPLICABLE DEVICES:
*   Lion2; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*   DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*   devNum - physical device number
*   portNum - physical port number
*
* OUTPUTS:
*   syncPtr - Sync status from XGKR sync block :
*                       GT_TRUE  - synced;
*                       GT_FALSE - not synced.
*
* RETURNS:
*   GT_OK                       - on success.
*   GT_BAD_PARAM                - on bad parameters
*   GT_BAD_PTR                  - syncPtr == NULL
*   GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS cpssDxChPortPcsSyncStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *syncPtr
)
{
    GT_STATUS               rc;   	/* return code */
    GT_U32                  portGroup;	/* local core number */
    GT_U32                  phyPortNum; /* number of port in local core */
    MV_HWS_PORT_STANDARD    portMode;   /* port ifMode in HWS format */
    MV_HWS_TEST_GEN_STATUS  status;	/* structure for sync status get */
    GT_U32                  i; 		/* iterator */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                          | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    rc = prvCpssLion2CpssIfModeToHwsTranslate(
                                PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum), 
                                PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portNum), 
                                &portMode);
    if(rc != GT_OK)
        return rc;

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
    i = 0;
    do
    {
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortTestGeneratorStatus(devNum[%d], portGroup[%d], phyPortNum[%d], portMode[%d], portPattern[%d], *status)",devNum, portGroup, phyPortNum, portMode, TEST_GEN_PRBS7);
        rc = mvHwsPortTestGeneratorStatus(devNum, portGroup, phyPortNum, portMode, 
                                    TEST_GEN_PRBS7,/* not used by API, value doesn't really matter */
                                    &status);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
        if(0 == status.checkerLock)
        {/* no lock/sync */
            break;
        }
        
    }while(i++ < 1000);

    *syncPtr = BIT2BOOL_MAC(status.checkerLock);

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortXgLanesSwapEnableSet
*
* DESCRIPTION:
*       Enable/Disable swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*
*                - GT_FALSE - Normal operation (no swapping)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortXgLanesSwapEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{

    GT_U32      regAddr;                 /* register address */
    GT_U32      value;                   /* register field value */
    GT_BOOL     resetState;              /* original XPCS reset state */
    GT_U32      lane;                    /* iterator */
    GT_STATUS   rc;                      /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      portMacNum;              /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    /* Configure Reset PCS */
    rc = prvCpssDxChPortXgPcsResetStateGet(devNum,portNum,&resetState);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(resetState != GT_TRUE)
    {
        rc = prvCpssDxChPortXgPcsResetStateSet(devNum,portNum,GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for(lane = 0; lane < 4; lane++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              macRegs.perPortRegs[portMacNum].laneConfig1[lane];
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        /* Swap Lanes */
        if(enable == GT_TRUE)
        {
            value = (3 - lane) | ((3 - lane) << 3);
        }
        else
        {
            value = lane | (lane << 3);
        }

        /* Configure RxSwpSel / TxSwpSel */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 8, 6, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* restore PSC Reset state */
    if(resetState != GT_TRUE)
    {
        rc = prvCpssDxChPortXgPcsResetStateSet(devNum,portNum,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3PortXgLanesSwapEnableGet
*
* DESCRIPTION:
*       Gets status of swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
*
* OUTPUTS:
*       enablePtr - pointer to status of swapping XAUI PHY SERDES Lanes.
*                    - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*                    - GT_FALSE - Normal operation (no swapping)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PortXgLanesSwapEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr;                 /* register address */
    GT_U32      value;                   /* register field value */
    GT_STATUS   rc;                      /* return status */
    GT_U32      portMacNum;              /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
          macRegs.perPortRegs[portMacNum].laneConfig1[0];
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* status of swapping XAUI PHY SERDES Lanes */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portMacNum),
            regAddr, 8, 6, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortXgLanesSwapEnableSet
*
* DESCRIPTION:
*       Enable/Disable swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*
*                - GT_FALSE - Normal operation (no swapping)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortXgLanesSwapEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_U32      regAddr;           /* register address */
    GT_U32      value;             /* register field value */
    GT_U32      resetState;        /* reset state */
    GT_STATUS   rc;                /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      portMacNum;        /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) < PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        return prvCpssDxChPortXgLanesSwapEnableSet(devNum, portNum, enable);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            smiRegs.smi10GePhyConfig0[portNum];
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        /* Get Reset state */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &resetState);

        if(resetState == 1)
        {
            /* Reset XAUI PHY */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0);
            if(rc != GT_OK)
            {
                return rc;
            }

        }

        value = resetState | (((enable == GT_TRUE) ? 1 : 0) << 4);

                        /* Set XAUI PHY and Lanes Swap bits */
        return prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x11, value);
    }

}

/*******************************************************************************
* cpssDxChPortXgLanesSwapEnableSet
*
* DESCRIPTION:
*       Enable/Disable swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*
*                - GT_FALSE - Normal operation (no swapping)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgLanesSwapEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortXgLanesSwapEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortXgLanesSwapEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortXgLanesSwapEnableGet
*
* DESCRIPTION:
*       Gets status of swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
*
* OUTPUTS:
*       enablePtr - pointer to status of swapping XAUI PHY SERDES Lanes.
*                    - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*                    - GT_FALSE - Normal operation (no swapping)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortXgLanesSwapEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */
    GT_U32      portMacNum;        /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) < PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        return prvCpssDxCh3PortXgLanesSwapEnableGet(devNum, portNum, enablePtr);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            smiRegs.smi10GePhyConfig0[portNum];
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
                regAddr, 4, 1, &value);

        if(rc != GT_OK)
        {
            return rc;
        }

        *enablePtr = BIT2BOOL_MAC(value);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXgLanesSwapEnableGet
*
* DESCRIPTION:
*       Gets status of swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
*
* OUTPUTS:
*       enablePtr - pointer to status of swapping XAUI PHY SERDES Lanes.
*                    - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*                    - GT_FALSE - Normal operation (no swapping)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgLanesSwapEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortXgLanesSwapEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortXgLanesSwapEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortXgPscLanesSwapSet
*
* DESCRIPTION:
*       Set swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - wrong SERDES lane
*       GT_BAD_VALUE    - multiple connection detected
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortXgPscLanesSwapSet
(
    IN GT_U8 devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    IN GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
)
{
    GT_U32      regAddr;                 /* register address */
    GT_U32      value;                   /* register field value */
    GT_BOOL     resetState;              /* original reset state */
    GT_U32      lane;                    /* iterator */
    GT_U32      rxLaneBitMap,txLaneBitMap;/* auxilary bit maps to check multiple conn. */
    GT_STATUS   rc;                      /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      portMacNum;              /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(rxSerdesLaneArr);
    CPSS_NULL_PTR_CHECK_MAC(txSerdesLaneArr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) < PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    rxLaneBitMap = 0x0;
    txLaneBitMap = 0x0;

    for( lane = 0 ; lane < CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ; lane++ )
    {
        if(rxSerdesLaneArr[lane] >= CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ||
           txSerdesLaneArr[lane] >= CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        rxLaneBitMap |= ( 1 << rxSerdesLaneArr[lane]);
        txLaneBitMap |= ( 1 << txSerdesLaneArr[lane]);
    }

    if( (rxLaneBitMap != (( 1 << CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ) - 1)) ||
        (txLaneBitMap != (( 1 << CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ) - 1)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
    }

    /* Configure Reset PCS */
    rc = prvCpssDxChPortXgPcsResetStateGet(devNum,portNum,&resetState);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(resetState != GT_TRUE)
    {
        rc = prvCpssDxChPortXgPcsResetStateSet(devNum,portNum,GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for( lane = 0 ; lane < CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ; lane++ )
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].laneConfig1[lane];
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        value = txSerdesLaneArr[lane] + (rxSerdesLaneArr[lane] << 3);

        /* Configure RxSwpSel / TxSwpSel */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 8, 6, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* restore PSC Reset state */
    if(resetState != GT_TRUE)
    {
        rc = prvCpssDxChPortXgPcsResetStateSet(devNum,portNum,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortXgPscLanesSwapSet
*
* DESCRIPTION:
*       Set swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - wrong SERDES lane
*       GT_BAD_VALUE    - multiple connection detected
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgPscLanesSwapSet
(
    IN GT_U8 devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    IN GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortXgPscLanesSwapSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, rxSerdesLaneArr, txSerdesLaneArr));

    rc = internal_cpssDxChPortXgPscLanesSwapSet(devNum, portNum, rxSerdesLaneArr, txSerdesLaneArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, rxSerdesLaneArr, txSerdesLaneArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortXgPscLanesSwapGet
*
* DESCRIPTION:
*       Get swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*
* OUTPUTS:
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortXgPscLanesSwapGet
(
    IN GT_U8 devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    OUT GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
)
{
    GT_U32      regAddr;                 /* register address */
    GT_U32      value;                   /* register field value */
    GT_U32      lane;                    /* iterator */
    GT_STATUS   rc;                      /* return status */
    GT_U32      portMacNum;              /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(rxSerdesLaneArr);
    CPSS_NULL_PTR_CHECK_MAC(txSerdesLaneArr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) < PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for( lane = 0 ; lane < CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ; lane++ )
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              macRegs.perPortRegs[portMacNum].laneConfig1[lane];
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        /* Get RxSwpSel / TxSwpSel */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
                regAddr, 8, 6, &value);
        if(rc != GT_OK)
        {
            return rc;
        }

        txSerdesLaneArr[lane] = value & 0x7;
        rxSerdesLaneArr[lane] = ( value >> 3 ) & 0x7;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXgPscLanesSwapGet
*
* DESCRIPTION:
*       Get swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*
* OUTPUTS:
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgPscLanesSwapGet
(
    IN GT_U8 devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    OUT GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortXgPscLanesSwapGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, rxSerdesLaneArr, txSerdesLaneArr));

    rc = internal_cpssDxChPortXgPscLanesSwapGet(devNum, portNum, rxSerdesLaneArr, txSerdesLaneArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, rxSerdesLaneArr, txSerdesLaneArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



