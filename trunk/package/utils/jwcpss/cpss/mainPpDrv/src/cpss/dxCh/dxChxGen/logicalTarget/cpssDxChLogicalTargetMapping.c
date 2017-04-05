/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChLogicalTargetMapping.c
*
* DESCRIPTION:
*       Logical Target mapping.
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/cpssDxChLogicalTargetMapping.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/private/prvCpssDxChLogicalTargetLog.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>


/*******************************************************************************
* prvCpssDxChLogicalTargetHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given Logical Target Mapping Table Entry from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum             - physical device number.
*       hwFormatArrayPtr   - points to the configuration in HW format .
*
*
* OUTPUTS:
*       logicFormatPtr     - points to tunnel termination configuration in
*                            logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLogicalTargetHw2LogicFormat
(
    IN GT_U8                                    devNum,
    IN  GT_U32                                  *hwFormatArrayPtr,
    OUT CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC    *logicFormatPtr
)
{
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT egressVlanTagState;
    GT_U32  vidx;
    GT_U32 hwPort,hwDev;

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArrayPtr);

    if ((*hwFormatArrayPtr >> 13) & 0x1 ) /* check if packet should be tunneled */
    {
        logicFormatPtr->outputInterface.isTunnelStart = GT_TRUE;
        if ((*hwFormatArrayPtr >> 26) & 0x1 )
        {
            logicFormatPtr->outputInterface.tunnelStartInfo.passengerPacketType =
                                        CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
        }
        else
        {
            logicFormatPtr->outputInterface.tunnelStartInfo.passengerPacketType =
                                        CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;
        }
        if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
        {
            logicFormatPtr->outputInterface.tunnelStartInfo.ptr = ((*hwFormatArrayPtr >> 14) & 0x3ff );
        }
        else
        {
            logicFormatPtr->outputInterface.tunnelStartInfo.ptr = ((*hwFormatArrayPtr >> 14) & 0xfff );
        }
    }

    if (*hwFormatArrayPtr & 0x1) /* check if multi-target (Vidx) */
    {
        vidx = ((*hwFormatArrayPtr >> 1) & 0xfff );
        if(vidx == 0xFFF)
        {
            logicFormatPtr->outputInterface.physicalInterface.type = CPSS_INTERFACE_VID_E;
        }
        else
        {
            logicFormatPtr->outputInterface.physicalInterface.type = CPSS_INTERFACE_VIDX_E;
            logicFormatPtr->outputInterface.physicalInterface.vidx = (GT_U16)vidx;
        }
    }
    else
    {
        if ((*hwFormatArrayPtr >> 1) & 0x1) /* check if target is trunk */
        {
            /* it is trunkId interface or if packet should be tunneled -
               trunkId  + Tunnel-Start Pointer  interface*/
            logicFormatPtr->outputInterface.physicalInterface.type = CPSS_INTERFACE_TRUNK_E;
            logicFormatPtr->outputInterface.physicalInterface.trunkId = (GT_TRUNK_ID)((*hwFormatArrayPtr >> 6) & 0x7f );

        }
        else
        {
            /* it is  Single-target {Device, Port} or if packet should be tunneled -
               {Device, Port}  + Tunnel-Start Pointer  interface*/
            logicFormatPtr->outputInterface.physicalInterface.type = CPSS_INTERFACE_PORT_E;
            hwPort = (GT_U8)((*hwFormatArrayPtr >> 2) & 0x3f );
            hwDev = (GT_U8)((*hwFormatArrayPtr >> 8) & 0x1f );

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(hwDev,hwPort);

            logicFormatPtr->outputInterface.physicalInterface.devPort.hwDevNum = hwDev;
            logicFormatPtr->outputInterface.physicalInterface.devPort.portNum = hwPort;
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        logicFormatPtr->egressTagTpidIndex = ((*hwFormatArrayPtr >> 24) & 0x3 );
        logicFormatPtr->egressVlanFilteringEnable = BIT2BOOL_MAC((*hwFormatArrayPtr >> 27) & 0x1 );
        switch ((*hwFormatArrayPtr >> 29) & 0x7 )
        {
        case 0:
            egressVlanTagState = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
            logicFormatPtr->egressVlanTagStateEnable = GT_TRUE;
            break;
        case 1:
            egressVlanTagState = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            logicFormatPtr->egressVlanTagStateEnable = GT_TRUE;
            break;
        case 2:
            egressVlanTagState = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;
            logicFormatPtr->egressVlanTagStateEnable = GT_TRUE;
            break;
        case 3:
            egressVlanTagState = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
            logicFormatPtr->egressVlanTagStateEnable = GT_TRUE;
            break;
        case 4:
            egressVlanTagState = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;
            logicFormatPtr->egressVlanTagStateEnable = GT_TRUE;
            break;
        case 5:
            egressVlanTagState = CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E;
            logicFormatPtr->egressVlanTagStateEnable = GT_TRUE;
            break;
        case 6:
            egressVlanTagState = CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E;
            logicFormatPtr->egressVlanTagStateEnable = GT_TRUE;
            break;
        case 7:
            egressVlanTagState = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E; /* dummy */
            logicFormatPtr->egressVlanTagStateEnable = GT_FALSE;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        logicFormatPtr->egressVlanTagState = ((egressVlanTagState >> 29) & 0x7 );
        if ((hwFormatArrayPtr[1]) & 0x1)
        {
            logicFormatPtr->assignVid0Enable = BIT2BOOL_MAC(hwFormatArrayPtr[1] & 0x1 );
            logicFormatPtr->vid0 = (GT_U16)((hwFormatArrayPtr[1] >> 1) & 0xFFF );

        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLogicalTargetLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given Logical Target Mapping Table Entry from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum            - physical device number.
*       logicFormatPtr    - points to tunnel termination configuration in
*                           logic format
*
* OUTPUTS:
*       hwFormatArrayPtr  - points to the configuration in HW format .
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLogicalTargetLogic2HwFormat
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC   *logicFormatPtr,
    OUT GT_U32                                 *hwFormatArrayPtr
)
{
    GT_U32 useVidx;
    GT_U32 targetIsTrunk;
    GT_U32 egressVlanTagState;
    GT_U32 vidx;
    GT_U32 hwPort,hwDev;

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArrayPtr);

    *hwFormatArrayPtr = 0;
    if (logicFormatPtr->outputInterface.isTunnelStart == GT_TRUE)
    {
        *hwFormatArrayPtr |= ((BOOL2BIT_MAC(logicFormatPtr->outputInterface.isTunnelStart)) << 13);
        if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
        {
            if (logicFormatPtr->outputInterface.tunnelStartInfo.ptr >= BIT_10)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            if (logicFormatPtr->outputInterface.tunnelStartInfo.ptr >= BIT_12)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

        *hwFormatArrayPtr |= (logicFormatPtr->outputInterface.tunnelStartInfo.ptr << 14);
        switch(logicFormatPtr->outputInterface.tunnelStartInfo.passengerPacketType)
        {
        case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
            *hwFormatArrayPtr |= (0 << 26);
            break;
        case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
            *hwFormatArrayPtr |= (1 << 26);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    switch (logicFormatPtr->outputInterface.physicalInterface.type)
    {
    case CPSS_INTERFACE_PORT_E:
             PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                logicFormatPtr->outputInterface.physicalInterface.devPort.hwDevNum,
                logicFormatPtr->outputInterface.physicalInterface.devPort.portNum);
            hwDev =  PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                logicFormatPtr->outputInterface.physicalInterface.devPort.hwDevNum,
                logicFormatPtr->outputInterface.physicalInterface.devPort.portNum);
            hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                logicFormatPtr->outputInterface.physicalInterface.devPort.hwDevNum,
                logicFormatPtr->outputInterface.physicalInterface.devPort.portNum);

            if (hwDev >= BIT_5)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            if (hwPort >= BIT_6)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            useVidx = 0;
            targetIsTrunk = 0;
            *hwFormatArrayPtr |= useVidx                                                  |
                                 (targetIsTrunk << 1)                                     |
                                 (hwPort << 2) |
                                (hwDev << 8);
            /* the remained fields are not relevant */
            break;

    case CPSS_INTERFACE_TRUNK_E:
        if (logicFormatPtr->outputInterface.physicalInterface.trunkId >= BIT_7)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        useVidx = 0;
        targetIsTrunk = 1;
        *hwFormatArrayPtr |=  useVidx              |
                              (targetIsTrunk << 1) |
                              (logicFormatPtr->outputInterface.physicalInterface.trunkId << 6);

        /* the remained fields are not relevant */
        break;
    case CPSS_INTERFACE_VID_E:
    case CPSS_INTERFACE_VIDX_E:
        if(logicFormatPtr->outputInterface.physicalInterface.type == CPSS_INTERFACE_VID_E)
        {
            vidx = 0xFFF;
        }
        else
        {
            vidx = logicFormatPtr->outputInterface.physicalInterface.vidx;

            if (vidx >= BIT_12)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        *hwFormatArrayPtr |= 1 | (logicFormatPtr->outputInterface.physicalInterface.vidx << 1);
        /* the remained fields are not relevant */
        break;
    default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (logicFormatPtr->egressTagTpidIndex >= BIT_2)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        *hwFormatArrayPtr |= (logicFormatPtr->egressTagTpidIndex << 24);
        *hwFormatArrayPtr |= ((BOOL2BIT_MAC(logicFormatPtr->egressVlanFilteringEnable)) << 27);
        if (logicFormatPtr->egressVlanTagStateEnable)
        {
            switch (logicFormatPtr->egressVlanTagState)
            {
            case CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E:
                egressVlanTagState = 0;
                break;
            case CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E:
                egressVlanTagState = 1;
                break;
            case CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E:
                egressVlanTagState = 2;
                break;
            case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E:
                egressVlanTagState = 3;
                break;
            case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E:
                egressVlanTagState = 4;
                break;
            case CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E:
                egressVlanTagState = 5;
                break;
            case CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E:
                egressVlanTagState = 6;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            egressVlanTagState = 7;
        }
        *hwFormatArrayPtr |= (egressVlanTagState << 29);
        if (logicFormatPtr->assignVid0Enable)
        {
            hwFormatArrayPtr[1] |= (BOOL2BIT_MAC(logicFormatPtr->assignVid0Enable));
            if (logicFormatPtr->vid0 >= BIT_12)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            hwFormatArrayPtr[1] |= (logicFormatPtr->vid0 << 1);
        }
    }
    else
    {
        egressVlanTagState = 7;
        *hwFormatArrayPtr |= (egressVlanTagState << 29);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable logical port mapping feature on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       enable   - GT_TRUE:  enable
*                  GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 regAddr;
    GT_U32 value;
    GT_U32 fieldOffset;
    GT_U32 fieldLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E );

    value = (enable == GT_TRUE) ? 1 : 0;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;

    fieldOffset = 13;
    fieldLength = 1;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, value);

}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable logical port mapping feature on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       enable   - GT_TRUE:  enable
*                  GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChLogicalTargetMappingEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status logical port mapping feature on the
*       specified  device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - Pointer to the enable/disable state.
*                   GT_TRUE : enable,
*                   GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    value = 0;
    GT_U32    fieldOffset;
    GT_U32    fieldLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E );

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;

    fieldOffset = 13;
    fieldLength = 1;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLength, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status logical port mapping feature on the
*       specified  device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - Pointer to the enable/disable state.
*                   GT_TRUE : enable,
*                   GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChLogicalTargetMappingEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingDeviceEnableSet
*
* DESCRIPTION:
*       Enable/disable a target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                       in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       enable        - GT_TRUE:  enable
*                       GT_FALSE: disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingDeviceEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     logicalDevNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 regAddr;
    GT_U32 value;
    GT_U32 fieldOffset;
    GT_U32 fieldLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if (logicalDevNum > 31)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (logicalDevNum < 24)
        {
            fieldOffset = logicalDevNum + 8;
        }
        else
        {
            fieldOffset = logicalDevNum - 24;
        }
    }
    else
    {
        if ( (logicalDevNum < 24) || (logicalDevNum > 31) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        fieldOffset = logicalDevNum - 24;
    }

    value = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.logicalTargetDeviceMappingConfReg;
    fieldLength = 1;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, value);

}

/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableSet
*
* DESCRIPTION:
*       Enable/disable a target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                       in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       enable        - GT_TRUE:  enable
*                       GT_FALSE: disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingDeviceEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     logicalDevNum,
    IN  GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingDeviceEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, logicalDevNum, enable));

    rc = internal_cpssDxChLogicalTargetMappingDeviceEnableSet(devNum, logicalDevNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, logicalDevNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingDeviceEnableGet
*
* DESCRIPTION:
*       Get Enable/disable status of target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                       in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       enablePtr     - Pointer to the  Enable/disable state.
*                       GT_TRUE : enable,
*                       GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingDeviceEnableGet
(
    IN   GT_U8     devNum,
    IN  GT_U8      logicalDevNum,
    OUT  GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    value = 0;
    GT_U32    fieldOffset;
    GT_U32    fieldLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if (logicalDevNum > 31)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (logicalDevNum < 24)
        {
            fieldOffset = logicalDevNum + 8;
        }
        else
        {
            fieldOffset = logicalDevNum - 24;
        }
    }
    else
    {
        if ( (logicalDevNum < 24) || (logicalDevNum > 31) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        fieldOffset = logicalDevNum - 24;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.logicalTargetDeviceMappingConfReg;

    fieldLength = 1;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLength, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableGet
*
* DESCRIPTION:
*       Get Enable/disable status of target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                       in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       enablePtr     - Pointer to the  Enable/disable state.
*                       GT_TRUE : enable,
*                       GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingDeviceEnableGet
(
    IN   GT_U8     devNum,
    IN  GT_U8      logicalDevNum,
    OUT  GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingDeviceEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, logicalDevNum, enablePtr));

    rc = internal_cpssDxChLogicalTargetMappingDeviceEnableGet(devNum, logicalDevNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, logicalDevNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingTableEntrySet
*
* DESCRIPTION:
*       Set logical target mapping table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                                      in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       logicalPortNum               - logical port number.
*                                      (APPLICABLE RANGES: 0..63)
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingTableEntrySet
(
    IN GT_U8                           devNum,
    IN GT_U8                           logicalDevNum,
    IN GT_U8                           logicalPortNum,
    IN CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC  *logicalPortMappingTablePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 hwData[2] = {0};
    GT_U32 entryIndex = 0;
    GT_U32  converted_logicalDevNum,converted_logicalPortNum ;
    GT_HW_DEV_NUM  hwLogicalDevNum = logicalDevNum;/* cast to U32 to avoid warnings */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E );

    CPSS_NULL_PTR_CHECK_MAC(logicalPortMappingTablePtr);

    /* do proper convert for ports of 'dual device id' */
    PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
        hwLogicalDevNum,logicalPortNum);

    converted_logicalDevNum = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
        hwLogicalDevNum,logicalPortNum);

    converted_logicalPortNum = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
        hwLogicalDevNum,logicalPortNum);

    /* update the INPUT parameters */
    logicalDevNum =  (GT_U8)converted_logicalDevNum;
    logicalPortNum = (GT_U8)converted_logicalPortNum;

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if (logicalDevNum > 31)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if ( (logicalDevNum < 24) || (logicalDevNum > 31) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if (logicalPortNum > 63)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Converts a given Logical Target Mapping Table Entry from logic format
       to hardware format.
    */
    rc = prvCpssDxChLogicalTargetLogic2HwFormat(devNum,logicalPortMappingTablePtr, hwData);

    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Calculate entry index of Logical Target Mapping Table */
        entryIndex = (logicalDevNum << 6) | logicalPortNum ;

        /* set Logical Target Mapping Table Entry */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E,
                                        entryIndex, /* entryIndex   */
                                        hwData);    /* fieldValue   */

    }
    else
    {
        /* Calculate entry index of Logical Target Mapping Table */
        entryIndex = ((logicalDevNum - 24) << 6) | logicalPortNum ;

        /* set Logical Target Mapping Table Entry */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E,
                                             entryIndex, /* entryIndex   */
                                             0,          /* fieldWordNum */
                                             0,          /* fieldOffset  */
                                             27,         /* fieldLength  */
                                             hwData[0]); /* fieldValue   */
    }
    return rc;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntrySet
*
* DESCRIPTION:
*       Set logical target mapping table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                                      in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       logicalPortNum               - logical port number.
*                                      (APPLICABLE RANGES: 0..63)
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingTableEntrySet
(
    IN GT_U8                           devNum,
    IN GT_U8                           logicalDevNum,
    IN GT_U8                           logicalPortNum,
    IN CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC  *logicalPortMappingTablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingTableEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, logicalDevNum, logicalPortNum, logicalPortMappingTablePtr));

    rc = internal_cpssDxChLogicalTargetMappingTableEntrySet(devNum, logicalDevNum, logicalPortNum, logicalPortMappingTablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, logicalDevNum, logicalPortNum, logicalPortMappingTablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingTableEntryGet
*
* DESCRIPTION:
*       Get logical target mapping table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                                      in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       logicalPortNum               - logical port number.
*                                      (APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingTableEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            logicalDevNum,
    IN GT_U8                            logicalPortNum,
    OUT CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC  *logicalPortMappingTablePtr
)
{
   GT_STATUS rc = GT_OK;
    GT_U32 hwData[2] = {0};
    GT_U32 entryIndex = 0;
    GT_U32  converted_logicalDevNum,converted_logicalPortNum ;
    GT_HW_DEV_NUM  hwLogicalDevNum = logicalDevNum;/* cast to U32 to avoid warnings */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E );

    CPSS_NULL_PTR_CHECK_MAC(logicalPortMappingTablePtr);

    converted_logicalDevNum = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
        hwLogicalDevNum,logicalPortNum);

    converted_logicalPortNum = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
        hwLogicalDevNum,logicalPortNum);

    /* update the INPUT parameters */
    logicalDevNum =  (GT_U8)converted_logicalDevNum;
    logicalPortNum = (GT_U8)converted_logicalPortNum;

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if (logicalDevNum > 31)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if ( (logicalDevNum < 24) || (logicalDevNum > 31) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if (logicalPortNum > 63)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
         /* Calculate entry index of Logical Target Mapping Table */
        entryIndex = (logicalDevNum << 6) | logicalPortNum ;

        /* set Logical Target Mapping Table Entry */
        rc = prvCpssDxChReadTableEntry(devNum,
                                       PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E,
                                       entryIndex, /* entryIndex   */
                                       hwData);    /* fieldValue   */
    }
    else
    {
        /* Calculate entry index of Logical Target Mapping Table */
        entryIndex = ((logicalDevNum - 24) << 6) | logicalPortNum ;

        /* set Logical Target Mapping Table Entry */
        rc =  prvCpssDxChReadTableEntryField (devNum,
                                              PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E,
                                              entryIndex,   /* entryIndex   */
                                              0,            /* fieldWordNum */
                                              0,            /* fieldOffset  */
                                              27,           /* fieldLength  */
                                              &hwData[0]);  /* fieldValue   */
    }
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Converts a given Logical Target Mapping Table Entry from hardware format
       to logic format.
    */
    rc = prvCpssDxChLogicalTargetHw2LogicFormat(devNum,hwData, logicalPortMappingTablePtr);

    return rc;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntryGet
*
* DESCRIPTION:
*       Get logical target mapping table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      xCat (except revision C0), Lion, xCat2, Lion2: (APPLICABLE RANGES: 24..31)
*                                      in xCat starting from revision c0, xCat3: (APPLICABLE RANGES: 0..31)
*       logicalPortNum               - logical port number.
*                                      (APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingTableEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            logicalDevNum,
    IN GT_U8                            logicalPortNum,
    OUT CPSS_DXCH_LOGICAL_TARGET_MAPPING_STC  *logicalPortMappingTablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingTableEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, logicalDevNum, logicalPortNum, logicalPortMappingTablePtr));

    rc = internal_cpssDxChLogicalTargetMappingTableEntryGet(devNum, logicalDevNum, logicalPortNum, logicalPortMappingTablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, logicalDevNum, logicalPortNum, logicalPortMappingTablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet
*
* DESCRIPTION:
*      This function sets the mode in which the Logical Port Egress VLAN member
*      table is accessed, according to the amount of VIDs and Logical Ports in
*      the system.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       mode    - value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet
(
    IN GT_U8                                                                  devNum,
    IN CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT    mode
)
{
    GT_U32 regAddr;
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

    switch (mode)
    {
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_4K_VID_32_LP_E:
        value = 0;
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_2K_VID_64_LP_E:
        value = 1;
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_1K_VID_128_LP_E:
        value = 2;
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_512_VID_256_LP_E:
        value = 3;
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_256_VID_512_LP_E:
        value = 4;
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_128_VID_1K_LP_E:
        value = 5;
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_64_VID_2K_LP_E:
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        value = 6;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 21, 3, value);
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet
*
* DESCRIPTION:
*      This function sets the mode in which the Logical Port Egress VLAN member
*      table is accessed, according to the amount of VIDs and Logical Ports in
*      the system.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       mode    - value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet
(
    IN GT_U8                                                                  devNum,
    IN CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT    mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet
*
* DESCRIPTION:
*      This function gets the mode in which the Logical Port Egress VLAN member
*      table is accessed, according to the amount of VIDs and Logical Ports in
*      the system.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       modePtr - (pointer to) value to get.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet
(
    IN  GT_U8                                                                devNum,
    OUT CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT *modePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 regAddr;
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 21, 3, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (value)
    {
    case 0:
        *modePtr = CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_4K_VID_32_LP_E;
        break;
    case 1:
        *modePtr = CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_2K_VID_64_LP_E;
        break;
    case 2:
        *modePtr = CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_1K_VID_128_LP_E;
        break;
    case 3:
        *modePtr = CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_512_VID_256_LP_E;
        break;
    case 4:
        *modePtr = CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_256_VID_512_LP_E;
        break;
    case 5:
        *modePtr = CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_128_VID_1K_LP_E;
        break;
    case 6:
        *modePtr = CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_64_VID_2K_LP_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet
*
* DESCRIPTION:
*      This function gets the mode in which the Logical Port Egress VLAN member
*      table is accessed, according to the amount of VIDs and Logical Ports in
*      the system.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       modePtr - (pointer to) value to get.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet
(
    IN  GT_U8                                                                devNum,
    OUT CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet
*
* DESCRIPTION:
*      This function sets the number of packets dropped due to Logical Port
*      Egress VLAN Filtering.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       counter - value to set counter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet
(
    IN GT_U8    devNum,
    IN GT_U32   counter
)
{
    GT_U32 regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.vlanEgressFilterCounterReg;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 32, counter);
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet
*
* DESCRIPTION:
*      This function sets the number of packets dropped due to Logical Port
*      Egress VLAN Filtering.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       counter - value to set counter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet
(
    IN GT_U8    devNum,
    IN GT_U32   counter
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counter));

    rc = internal_cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterSet(devNum, counter);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counter));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet
*
* DESCRIPTION:
*      This function gets the number of packets dropped due to Logical Port
*      Egress VLAN Filtering.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - the device number
*
* OUTPUTS:
*       counterPtr - (pointer to) counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *counterPtr
)
{
    GT_U32 regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.vlanEgressFilterCounterReg;
    return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 32, counterPtr);

}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet
*
* DESCRIPTION:
*      This function gets the number of packets dropped due to Logical Port
*      Egress VLAN Filtering.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - the device number
*
* OUTPUTS:
*       counterPtr - (pointer to) counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counterPtr));

    rc = internal_cpssDxChLogicalTargetMappingEgressVlanFilteringDropCounterGet(devNum, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressMappingEntrySet
*
* DESCRIPTION:
*       Function sets the mapping of any discrete VIDs of the 4K VLAN range,
*       into a continuous range of VIDs.
*       Used to accommodate the cases where the 12-bit VLAN-ID must be mapped
*       into a smaller internal VID index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Mapping Table.
*                         (Applicable Range 0..4095)
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (Applicable Range 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressMappingEntrySet
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN GT_U32                       vlanIdIndex
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanIdIndex);


    /* set Logical Target Mapping Table Entry */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_XCAT_TABLE_EQ_VLAN_MAPPING_E,
                                         vlanId,        /* entryIndex   */
                                         0,             /* fieldWordNum */
                                         0,             /* fieldOffset  */
                                         12,            /* fieldLength  */
                                         vlanIdIndex);  /* fieldValue   */
    return rc;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMappingEntrySet
*
* DESCRIPTION:
*       Function sets the mapping of any discrete VIDs of the 4K VLAN range,
*       into a continuous range of VIDs.
*       Used to accommodate the cases where the 12-bit VLAN-ID must be mapped
*       into a smaller internal VID index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Mapping Table.
*                         (Applicable Range 0..4095)
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (Applicable Range 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMappingEntrySet
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN GT_U32                       vlanIdIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressMappingEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, vlanIdIndex));

    rc = internal_cpssDxChLogicalTargetMappingEgressMappingEntrySet(devNum, vlanId, vlanIdIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, vlanIdIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressMappingEntryGet
*
* DESCRIPTION:
*       Function gets the mapping of any discrete VIDs of the 4K VLAN range,
*       into a continuous range of VIDs.
*       Used to accommodate the cases where the 12-bit VLAN-ID must be mapped
*       into a smaller internal VID index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Mapping Table.
*                         (Applicable Range 0..4095)
*
* OUTPUTS:
*       vlanIdIndexPtr  - (pointer to) VLAN id index, use as value in
*                         the Mapping Table. (Applicable Range 0..4095)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressMappingEntryGet
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    OUT GT_U32                      *vlanIdIndexPtr
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(vlanIdIndexPtr);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

    /* set Logical Target Mapping Table Entry */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_XCAT_TABLE_EQ_VLAN_MAPPING_E,
                                        vlanId,           /* entryIndex   */
                                        0,                /* fieldWordNum */
                                        0,                /* fieldOffset  */
                                        12,               /* fieldLength  */
                                        vlanIdIndexPtr);  /* fieldValue   */
    return rc;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMappingEntryGet
*
* DESCRIPTION:
*       Function gets the mapping of any discrete VIDs of the 4K VLAN range,
*       into a continuous range of VIDs.
*       Used to accommodate the cases where the 12-bit VLAN-ID must be mapped
*       into a smaller internal VID index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Mapping Table.
*                         (Applicable Range 0..4095)
*
* OUTPUTS:
*       vlanIdIndexPtr  - (pointer to) VLAN id index, use as value in
*                         the Mapping Table. (Applicable Range 0..4095)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMappingEntryGet
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    OUT GT_U32                      *vlanIdIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressMappingEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, vlanIdIndexPtr));

    rc = internal_cpssDxChLogicalTargetMappingEgressMappingEntryGet(devNum, vlanId, vlanIdIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, vlanIdIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressMemberSet
*
* DESCRIPTION:
*       Function sets whether target logical port is a member of a given VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       logicalDevNum   - logical target device number. (APPLICABLE RANGES: 0..31)
*       logicalPortNum  - logical target port number. (APPLICABLE RANGES: 0..63)
*       isMember        - whether target logical port is member of a given VLAN.
*                         GT_TRUE: target logical port is member of a given VLAN
*                         GT_FALSE: target logical port isn't member of a given VLAN
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressMemberSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       vlanIdIndex,
    IN GT_U8                        logicalDevNum,
    IN GT_U8                        logicalPortNum,
    IN GT_BOOL                      isMember
)
{
    GT_STATUS rc = GT_OK;
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT    mode;
    GT_U32 X,Y, LP_IDX, VID_IDX = vlanIdIndex;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanIdIndex);
    if (logicalDevNum > 31 || logicalPortNum > 63)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet(devNum,&mode);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* LP_IDX=11-bit index {TrgDev 5 bit, TrgPort 6 bit} */
    LP_IDX = ((logicalDevNum & 0x1F) << 5) | (logicalPortNum & 0x3F);
    switch (mode)
    {
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_4K_VID_32_LP_E:
        /* X = {VID_IDX[11:6]}, Y = {VID_IDX[5:0], LP_IDX[4:0]} */
        X = (VID_IDX & 0xFC0) >> 6;
        Y = ((VID_IDX & 0x3F) << 5) | (LP_IDX & 0x1F);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_2K_VID_64_LP_E:
        /* X ={VID_IDX[10:5]}, Y = {VID_IDX[4:0],LP_IDX[5:0]} */
        X = (VID_IDX & 0x7E0) >> 5;
        Y = ((VID_IDX & 0x1F) << 6) | (LP_IDX & 0x3F);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_1K_VID_128_LP_E:
        /* X ={VID_IDX[9:4]}, Y = {VID_IDX[3:0],LP_IDX[6:0]} */
        X = (VID_IDX & 0x3F0) >> 4;
        Y = ((VID_IDX & 0xF) << 7) | (LP_IDX & 0x7F);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_512_VID_256_LP_E:
        /* X = {VID_IDX[8:3]}, Y = {VID_IDX[2:0],LP_IDX[7:0]}*/
        X = (VID_IDX & 0x1F8) >> 3;
        Y = ((VID_IDX & 0x7) << 8) | (LP_IDX & 0xFF);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_256_VID_512_LP_E:
        /* X = {VID_IDX[7:2]}, Y = {VID_IDX[1:0],LP_IDX[8:0]} */
        X = (VID_IDX & 0xFC) >> 2;
        Y = ((VID_IDX & 0x3) << 9) | (LP_IDX & 0x1FF);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_128_VID_1K_LP_E:
        /* X ={VID_IDX[6:1]}, Y = {VID_IDX[0],LP_IDX[9:0]} */
        X = (VID_IDX & 0x7E) >> 1;
        Y = ((VID_IDX & 0x1) << 10) | (LP_IDX & 0x3FF);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_64_VID_2K_LP_E:
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* X ={VID_IDX[5:0]}, Y = {LP_IDX[10:0]} */
        X = (VID_IDX & 0x3F);
        Y = (LP_IDX & 0x7FF);
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* set Logical Target Mapping Table Entry */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_XCAT_TABLE_EQ_LP_EGRESS_VLAN_MEMBER_E,
                                         Y,                                                 /* entryIndex   */
                                         PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,  /* fieldWordNum */
                                         X,                                                 /* fieldOffset  */
                                         1,                                                 /* fieldLength  */
                                         isMember);                                         /* fieldValue   */
    return rc;

}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberSet
*
* DESCRIPTION:
*       Function sets whether target logical port is a member of a given VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       logicalDevNum   - logical target device number. (APPLICABLE RANGES: 0..31)
*       logicalPortNum  - logical target port number. (APPLICABLE RANGES: 0..63)
*       isMember        - whether target logical port is member of a given VLAN.
*                         GT_TRUE: target logical port is member of a given VLAN
*                         GT_FALSE: target logical port isn't member of a given VLAN
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMemberSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       vlanIdIndex,
    IN GT_U8                        logicalDevNum,
    IN GT_U8                        logicalPortNum,
    IN GT_BOOL                      isMember
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressMemberSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanIdIndex, logicalDevNum, logicalPortNum, isMember));

    rc = internal_cpssDxChLogicalTargetMappingEgressMemberSet(devNum, vlanIdIndex, logicalDevNum, logicalPortNum, isMember);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanIdIndex, logicalDevNum, logicalPortNum, isMember));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressMemberGet
*
* DESCRIPTION:
*       Function gets whether target logical port is a member of a given VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       logicalDevNum   - logical target device number. (APPLICABLE RANGES: 0..31)
*       logicalPortNum  - logical target port number. (APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       isMemberPtr     - (Pointer to) whether target logical port is member
*              of a give VLAN.
*                         GT_TRUE: target logical port is member of a given VLAN
*                         GT_FALSE: target logical port is not member of a given VLAN
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressMemberGet
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       vlanIdIndex,
    IN  GT_U8                        logicalDevNum,
    IN  GT_U8                        logicalPortNum,
    OUT GT_BOOL                     *isMemberPtr
)
{
    GT_STATUS rc = GT_OK;
    CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_ENT    mode;
    GT_U32 X,Y, LP_IDX, VID_IDX = vlanIdIndex;
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(isMemberPtr);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanIdIndex);
    if (logicalDevNum > 31 || logicalPortNum > 63)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = cpssDxChLogicalTargetMappingEgressVlanMemberAccessModeGet(devNum,&mode);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* LP_IDX=11-bit index {TrgDev 5 bit, TrgPort 6 bit} */
    LP_IDX = ((logicalDevNum & 0x1F) << 5) | (logicalPortNum & 0x3F);
    switch (mode)
    {
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_4K_VID_32_LP_E:
        /* X = {VID_IDX[11:6]}, Y = {VID_IDX[5:0], LP_IDX[4:0]} */
        X = (VID_IDX & 0xFC0) >> 6;
        Y = ((VID_IDX & 0x3F) << 5) | (LP_IDX & 0x1F);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_2K_VID_64_LP_E:
        /* X ={VID_IDX[10:5]}, Y = {VID_IDX[4:0],LP_IDX[5:0]} */
        X = (VID_IDX & 0x7E0) >> 5;
        Y = ((VID_IDX & 0x1F) << 6) | (LP_IDX & 0x3F);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_1K_VID_128_LP_E:
        /* X ={VID_IDX[9:4]}, Y = {VID_IDX[3:0],LP_IDX[6:0]} */
        X = (VID_IDX & 0x3F0) >> 4;
        Y = ((VID_IDX & 0xF) << 7) | (LP_IDX & 0x7F);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_512_VID_256_LP_E:
        /* X = {VID_IDX[8:3]}, Y = {VID_IDX[2:0],LP_IDX[7:0]}*/
        X = (VID_IDX & 0x1F8) >> 3;
        Y = ((VID_IDX & 0x7) << 8) | (LP_IDX & 0xFF);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_256_VID_512_LP_E:
        /* X = {VID_IDX[7:2]}, Y = {VID_IDX[1:0],LP_IDX[8:0]} */
        X = (VID_IDX & 0xFC) >> 2;
        Y = ((VID_IDX & 0x3) << 9) | (LP_IDX & 0x1FF);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_128_VID_1K_LP_E:
        /* X ={VID_IDX[6:1]}, Y = {VID_IDX[0],LP_IDX[9:0]} */
        X = (VID_IDX & 0x7E) >> 1;
        Y = ((VID_IDX & 0x1) << 10) | (LP_IDX & 0x3FF);
        break;
    case CPSS_DXCH_LOGICAL_TARGET_MAPPING_EGRESS_VLAN_MEMBER_ACCESS_MODE_64_VID_2K_LP_E:
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* X ={VID_IDX[5:0]}, Y = {LP_IDX[10:0]} */
        X = (VID_IDX & 0x3F);
        Y = (LP_IDX & 0x7FF);
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* set Logical Target Mapping Table Entry */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_XCAT_TABLE_EQ_LP_EGRESS_VLAN_MEMBER_E,
                                         Y,                                                 /* entryIndex   */
                                         PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,  /* fieldWordNum */
                                         X,                                                 /* fieldOffset  */
                                         1,                                                 /* fieldLength  */
                                         &value);                                           /* fieldValue   */
     if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *isMemberPtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberGet
*
* DESCRIPTION:
*       Function gets whether target logical port is a member of a given VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       vlanIdIndex     - VLAN id index, use as value in the Mapping Table.
*                         (APPLICABLE RANGES: 0..4095)
*       logicalDevNum   - logical target device number. (APPLICABLE RANGES: 0..31)
*       logicalPortNum  - logical target port number. (APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       isMemberPtr     - (Pointer to) whether target logical port is member
*              of a give VLAN.
*                         GT_TRUE: target logical port is member of a given VLAN
*                         GT_FALSE: target logical port is not member of a given VLAN
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMemberGet
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       vlanIdIndex,
    IN  GT_U8                        logicalDevNum,
    IN  GT_U8                        logicalPortNum,
    OUT GT_BOOL                     *isMemberPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressMemberGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanIdIndex, logicalDevNum, logicalPortNum, isMemberPtr));

    rc = internal_cpssDxChLogicalTargetMappingEgressMemberGet(devNum, vlanIdIndex, logicalDevNum, logicalPortNum, isMemberPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanIdIndex, logicalDevNum, logicalPortNum, isMemberPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChLogicalTargetMappingEgressMemberTableClear
*
* DESCRIPTION:
*       Function clears Logical Port Egress VLAN member table.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChLogicalTargetMappingEgressMemberTableClear
(
    IN GT_U8                        devNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 zero = 0;
    GT_U32 Y;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

    for (Y=0; Y< _2K; Y++)
    {
        /* set Logical Target Mapping Table Entry */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_XCAT_TABLE_EQ_LP_EGRESS_VLAN_MEMBER_E,
                                            Y,                                                 /* entryIndex   */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,  /* fieldWordNum */
                                            0,                                                 /* fieldOffset  */
                                            32,                                                /* fieldLength  */
                                            &zero);                                             /* fieldValue   */
        if (rc != GT_OK)
        {
            return rc;
        }

         /* set Logical Target Mapping Table Entry */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_XCAT_TABLE_EQ_LP_EGRESS_VLAN_MEMBER_E,
                                            Y,                                                 /* entryIndex   */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,  /* fieldWordNum */
                                            32,                                                 /* fieldOffset  */
                                            32,                                                /* fieldLength  */
                                            &zero);                                             /* fieldValue   */

        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingEgressMemberTableClear
*
* DESCRIPTION:
*       Function clears Logical Port Egress VLAN member table.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEgressMemberTableClear
(
    IN GT_U8                        devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChLogicalTargetMappingEgressMemberTableClear);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChLogicalTargetMappingEgressMemberTableClear(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



