/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgE2Phy.c
*
* DESCRIPTION:
*       Eport to Physical port tables facility CPSS DxCh implementation.
*
*
* FILE REVISION NUMBER:
*       $Revision: 15$
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgE2Phy.h>

/*******************************************************************************
* internal_cpssDxChBrgEportToPhysicalPortTargetMappingTableSet
*
* DESCRIPTION:
*       Set physical info for the given ePort in the E2PHY mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       physicalInfoPtr - (pointer to) physical Information related to the ePort
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       E2PHY table is accessed by the packet's target ePort
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEportToPhysicalPortTargetMappingTableSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_INTERFACE_INFO_STC          *physicalInfoPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    hwFormat = 0;
    GT_U32    hwDev, hwPort;
    GT_U32    vidx;
    GT_U32    numOfBits;
    GT_U32    hwPortNumMax;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(physicalInfoPtr);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    numOfBits = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 23 : 22;


    switch (physicalInfoPtr->type)
    {
        case CPSS_INTERFACE_PORT_E:

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(physicalInfoPtr->devPort.hwDevNum,
                                                   physicalInfoPtr->devPort.portNum);

            hwDev =  PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(physicalInfoPtr->devPort.hwDevNum,
                                                         physicalInfoPtr->devPort.portNum);
            hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(physicalInfoPtr->devPort.hwDevNum,
                                                          physicalInfoPtr->devPort.portNum);

            hwPortNumMax = MAX(BIT_8,PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum));

            if((hwPort >= hwPortNumMax) ||
               (hwDev > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum)))
            {
                /* 8 bits for the physical port number and 10 bits for device number */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* build value to set to hw */
            U32_SET_FIELD_MAC(hwFormat,2,1,0); /* target is not trunk */
            U32_SET_FIELD_MAC(hwFormat,3,1,0); /* target not VIDX */
            U32_SET_FIELD_MAC(hwFormat,4,10,hwDev);
            U32_SET_FIELD_MAC(hwFormat,14,
                PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 9 : 8,hwPort);

            break;
         case CPSS_INTERFACE_TRUNK_E:
            if(physicalInfoPtr->trunkId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
            {
                /* 12 bits for the trunkId */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* build value to set to hw */
            U32_SET_FIELD_MAC(hwFormat,2,1,1); /* target is trunk */
            U32_SET_FIELD_MAC(hwFormat,3,1,0); /* target not VIDX */
            U32_SET_FIELD_MAC(hwFormat,4,12,physicalInfoPtr->trunkId);
            break;
        case CPSS_INTERFACE_VIDX_E:
        case CPSS_INTERFACE_VID_E:
            if(physicalInfoPtr->type == CPSS_INTERFACE_VID_E)
            {
                vidx = 0xFFF;
            }
            else
            {
                vidx = physicalInfoPtr->vidx;

                if (vidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VIDX_MAC(devNum))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            /* build value to set to hw */
            U32_SET_FIELD_MAC(hwFormat,2,1,0); /* target is not trunk */
            U32_SET_FIELD_MAC(hwFormat,3,1,1); /* target is VIDX */
            U32_SET_FIELD_MAC(hwFormat,4,12,vidx);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* MTU Index was added by a separate API: cpssDxChBrgGenMtuPortProfileIdxSet */
    /* we not change the bits 0,1 <MTU Index> */
    hwFormat >>= 2;

    /* set ePort-to-physical-port-target-mapping table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                     PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E,
                                     portNum,
                                     PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                     2,
                                     numOfBits - 2,
                                     hwFormat);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgEportToPhysicalPortTargetMappingTableSet
*
* DESCRIPTION:
*       Set physical info for the given ePort in the E2PHY mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       physicalInfoPtr - (pointer to) physical Information related to the ePort
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       E2PHY table is accessed by the packet's target ePort
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEportToPhysicalPortTargetMappingTableSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_INTERFACE_INFO_STC          *physicalInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEportToPhysicalPortTargetMappingTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, physicalInfoPtr));

    rc = internal_cpssDxChBrgEportToPhysicalPortTargetMappingTableSet(devNum, portNum, physicalInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, physicalInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEportToPhysicalPortTargetMappingTableGet
*
* DESCRIPTION:
*       Get physical info for the given ePort in the E2PHY mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       physicalInfoPtr - (pointer to) physical Information related to the ePort
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       E2PHY table is accessed by the packet's target ePort
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEportToPhysicalPortTargetMappingTableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_INTERFACE_INFO_STC             *physicalInfoPtr
)
{
    GT_STATUS rc;               /* return code          */
    GT_U32    hwFormat;
    GT_U32    value;
    GT_U32    numOfBits;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(physicalInfoPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);


    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    numOfBits = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 23 : 22;

    /* get ePort-to-physical-port-target-mapping table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                     PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E,
                                     portNum,
                                     PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                     0,
                                     numOfBits,
                                     &hwFormat);
    if(rc != GT_OK)
        return rc;

    if (U32_GET_FIELD_MAC(hwFormat,3,1) == 0) /* target is not VIDX */
    {
        if (U32_GET_FIELD_MAC(hwFormat,2,1) == 0) /* target is not trunk */
        {
            physicalInfoPtr->devPort.hwDevNum  = U32_GET_FIELD_MAC(hwFormat, 4,10);
            physicalInfoPtr->devPort.portNum = U32_GET_FIELD_MAC(hwFormat,14,
                PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 9 : 8);

            physicalInfoPtr->type = CPSS_INTERFACE_PORT_E;
        }
        else /* target is trunk */
        {

            physicalInfoPtr->trunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(hwFormat,4,12);
            physicalInfoPtr->type    = CPSS_INTERFACE_TRUNK_E;
        }
    }
    else /* target is VIDX */
    {
        value = U32_GET_FIELD_MAC(hwFormat,4,12);
        if(value == 0xFFF)
        {
            physicalInfoPtr->type = CPSS_INTERFACE_VID_E;
        }
        else
        {
            physicalInfoPtr->vidx = (GT_U16)value;
            physicalInfoPtr->type = CPSS_INTERFACE_VIDX_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgEportToPhysicalPortTargetMappingTableGet
*
* DESCRIPTION:
*       Get physical info for the given ePort in the E2PHY mapping table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       physicalInfoPtr - (pointer to) physical Information related to the ePort
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       E2PHY table is accessed by the packet's target ePort
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEportToPhysicalPortTargetMappingTableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_INTERFACE_INFO_STC             *physicalInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEportToPhysicalPortTargetMappingTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, physicalInfoPtr));

    rc = internal_cpssDxChBrgEportToPhysicalPortTargetMappingTableGet(devNum, portNum, physicalInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, physicalInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEportToPhysicalPortEgressPortInfoSet
*
* DESCRIPTION:
*       Set physical info for the given ePort in the HA Egress port table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       egressInfoPtr   - (pointer to) physical information related to the egress port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEportToPhysicalPortEgressPortInfoSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_NUM                          portNum,
    IN CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC   *egressInfoPtr
)
{
    GT_STATUS rc;
    GT_U32    hwDataArr[4];/* need to hold 'read/write entry' */
    GT_U32    value;/*field value*/

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(egressInfoPtr);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* read HA Egress ePort 1 table  */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   hwDataArr);
    if(rc != GT_OK)
    {
        return rc;
    }

    U32_SET_FIELD_BY_ID_MAC(hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
        LION3_HA_EPORT_TABLE_1_FIELDS_FORCE_ARP_TS_EPORT_DECISION_E,
        0);

    value = BOOL2BIT_MAC(egressInfoPtr->tunnelStart);
    U32_SET_FIELD_BY_ID_MAC(hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
        LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E,
        value);

    if (egressInfoPtr->tunnelStart == GT_TRUE)
    {
        if (egressInfoPtr->tunnelStartPtr > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TS_PTR_MAC(devNum))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        value = egressInfoPtr->tunnelStartPtr;
        U32_SET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER_E,
            value);

        switch(egressInfoPtr->tsPassengerPacketType)
        {
            case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
                value = 0;
                break;
            case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
                value = 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        U32_SET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_TS_PASSENGER_TYPE_E,
            value);
    }
    else
    {
        if (egressInfoPtr->arpPtr > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_ARP_PTR_MAC(devNum))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        value = egressInfoPtr->arpPtr;
        U32_SET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER_E,
            value);

        value = BOOL2BIT_MAC(egressInfoPtr->modifyMacSa);
        U32_SET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_SA_E,
            value);

        value = BOOL2BIT_MAC(egressInfoPtr->modifyMacDa);
        U32_SET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_DA_E,
            value);
    }


    /* Write HA Egress ePort 1 table (after update) */
    rc = prvCpssDxChWriteTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   hwDataArr);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgEportToPhysicalPortEgressPortInfoSet
*
* DESCRIPTION:
*       Set physical info for the given ePort in the HA Egress port table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       egressInfoPtr   - (pointer to) physical information related to the egress port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEportToPhysicalPortEgressPortInfoSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_NUM                          portNum,
    IN CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC   *egressInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEportToPhysicalPortEgressPortInfoSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egressInfoPtr));

    rc = internal_cpssDxChBrgEportToPhysicalPortEgressPortInfoSet(devNum, portNum, egressInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egressInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEportToPhysicalPortEgressPortInfoGet
*
* DESCRIPTION:
*       Get physical info for the given ePort in the HA Egress port table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       egressInfoPtr - (pointer to) physical Information related to the egress port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEportToPhysicalPortEgressPortInfoGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC  *egressInfoPtr
)
{
    GT_STATUS rc;
    GT_U32    hwDataArr[4];/* need to hold 'read entry' */
    GT_U32    value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(egressInfoPtr);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);


    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* read HA Egress ePort 1 table  */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                   hwDataArr);
    if(rc != GT_OK)
    {
        return rc;
    }

    U32_GET_FIELD_BY_ID_MAC(hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
        LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E,
        value);
    egressInfoPtr->tunnelStart = BIT2BOOL_MAC(value);

    if (egressInfoPtr->tunnelStart == GT_TRUE)
    {
        U32_GET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER_E,
            value);
        egressInfoPtr->tunnelStartPtr = value;

        U32_GET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_TS_PASSENGER_TYPE_E,
            value);

        switch(value)
        {
            case 0:
                egressInfoPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;
                break;
            case 1:
                egressInfoPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        U32_GET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER_E,
            value);
        egressInfoPtr->arpPtr = value;

        U32_GET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_SA_E,
            value);
        egressInfoPtr->modifyMacSa = BIT2BOOL_MAC(value);

        U32_GET_FIELD_BY_ID_MAC(hwDataArr,
            PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E].fieldsInfoPtr,
            LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_DA_E,
            value);
        egressInfoPtr->modifyMacDa = BIT2BOOL_MAC(value);
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgEportToPhysicalPortEgressPortInfoGet
*
* DESCRIPTION:
*       Get physical info for the given ePort in the HA Egress port table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       egressInfoPtr - (pointer to) physical Information related to the egress port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEportToPhysicalPortEgressPortInfoGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_DXCH_BRG_EGRESS_PORT_INFO_STC  *egressInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEportToPhysicalPortEgressPortInfoGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egressInfoPtr));

    rc = internal_cpssDxChBrgEportToPhysicalPortEgressPortInfoGet(devNum, portNum, egressInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egressInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

