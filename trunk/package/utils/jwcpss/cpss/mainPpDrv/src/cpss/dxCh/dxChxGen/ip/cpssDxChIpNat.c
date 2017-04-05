/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpNat.c
*
* DESCRIPTION:
*       The The CPSS DXCH IP NAT APIs
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <stdlib.h>

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpNat.h>
#include <cpss/dxCh/dxChxGen/ip/private/prvCpssDxChIpLog.h>

/* the size of NAT entry in words */
#define NAT_HW_SIZE_CNS       6

/* macro assume : value hold the 'value' and set it into hwFormatArray */
#define LION3_HA_NAT44_FIELD_VALUE_SET_MAC(_dev,_fieldName) \
    LION3_HA_NAT44_FIELD_SET_MAC(_dev,hwFormatArray,_fieldName,value)

/* macro assume : hwFormatArray hold the entry and macro fill the value with 'value'*/
#define LION3_HA_NAT44_FIELD_VALUE_GET_MAC(_dev,_fieldName) \
    LION3_HA_NAT44_FIELD_GET_MAC(_dev,hwFormatArray,_fieldName,value)

/* macro assume : value hold the 'value' and set it into hwFormatArray */
#define LION3_HA_NAT66_FIELD_VALUE_SET_MAC(_dev,_fieldName) \
    LION3_HA_NAT66_FIELD_SET_MAC(_dev,hwFormatArray,_fieldName,value)

/* macro assume : hwFormatArray hold the entry and macro fill the value with 'value'*/
#define LION3_HA_NAT66_FIELD_VALUE_GET_MAC(_dev,_fieldName) \
    LION3_HA_NAT66_FIELD_GET_MAC(_dev,hwFormatArray,_fieldName,value)


/*******************************************************************************
*  prvCpssIpNatConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given NAT entry from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.

* INPUTS:
*       devNum            - physical device number
*       natType           - type of the NAT
*       logicFormatPtr    - (pointer to) NAT entry
*
* OUTPUTS:
*       hwFormatArray     - The entry in HW format (6 words).
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
static GT_STATUS prvCpssIpNatConfigLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_IP_NAT_TYPE_ENT                natType,
    IN  CPSS_DXCH_IP_NAT_ENTRY_UNT          *logicFormatPtr,
    OUT GT_U32                              *hwFormatArray
)
{
    GT_U32 value;

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*NAT_HW_SIZE_CNS);

    switch (natType)
    {
        case CPSS_IP_NAT_TYPE_NAT44_E:

            LION3_HA_NAT44_FIELD_MAC_DA_SET_MAC(devNum, hwFormatArray, &logicFormatPtr->nat44Entry.macDa.arEther[0]);

            value = BOOL2BIT_MAC(logicFormatPtr->nat44Entry.modifyDip);
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_MODIFY_DIP_E);

            value = (logicFormatPtr->nat44Entry.newDip.arIP[0] << 24) |
                    (logicFormatPtr->nat44Entry.newDip.arIP[1] << 16) |
                    (logicFormatPtr->nat44Entry.newDip.arIP[2] <<  8) |
                    (logicFormatPtr->nat44Entry.newDip.arIP[3] <<  0) ;
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NEW_DIP_E);

            value = BOOL2BIT_MAC(logicFormatPtr->nat44Entry.modifySip);
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_MODIFY_SIP_E);

            value = (logicFormatPtr->nat44Entry.newSip.arIP[0] << 24) |
                    (logicFormatPtr->nat44Entry.newSip.arIP[1] << 16) |
                    (logicFormatPtr->nat44Entry.newSip.arIP[2] <<  8) |
                    (logicFormatPtr->nat44Entry.newSip.arIP[3] <<  0) ;
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NEW_SIP_E);

            value = BOOL2BIT_MAC(logicFormatPtr->nat44Entry.modifyTcpUdpDstPort);
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_MODIFY_TCP_UDP_DST_PORT_E);

            /* check newTcpUdpDstPort value is legal (0...65535)*/
            value = logicFormatPtr->nat44Entry.newTcpUdpDstPort;
            if (value >= BIT_16)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NEW_TCP_UDP_DST_PORT_E);

            value = BOOL2BIT_MAC(logicFormatPtr->nat44Entry.modifyTcpUdpSrcPort);
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_MODIFY_TCP_UDP_SRC_PORT_E);

            /* check newTcpUdpSrcPort value is legal (0...65535)*/
            value = logicFormatPtr->nat44Entry.newTcpUdpSrcPort;
            if (value >= BIT_16)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NEW_TCP_UDP_SRC_PORT_E);
            /* set NAT Entry Type to be NAT44 */
            value=0;
            LION3_HA_NAT44_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NAT_ENTRY_TYPE_E);

            break;

        case CPSS_IP_NAT_TYPE_NAT66_E:
            LION3_HA_NAT66_FIELD_MAC_DA_SET_MAC(devNum, hwFormatArray, &logicFormatPtr->nat66Entry.macDa.arEther[0]);
            LION3_HA_NAT66_FIELD_ADDRESS_SET_MAC(devNum, hwFormatArray, &logicFormatPtr->nat66Entry.address.arIP[0]);

            switch (logicFormatPtr->nat66Entry.modifyCommand)
            {
                case CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E:
                    value=0;
                    break;
                case CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E:
                    value=1;
                    break;
                case CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_ADDRESS_E:
                    value=2;
                    break;
                case CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_ADDRESS_E:
                    value=3;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            LION3_HA_NAT66_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT66_TABLE_FIELDS_MODIFY_COMMAND_E);

            if ((logicFormatPtr->nat66Entry.modifyCommand==CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E)||
                (logicFormatPtr->nat66Entry.modifyCommand==CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E))
            {
                value = logicFormatPtr->nat66Entry.prefixSize;
                /* if sw value is 64 then we need to set hw value to be 0 */
                if (value==64)
                {
                    value=0;
                }
                else
                {
                    if((value==0)||(value >= BIT_6))
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                    }
                }
                LION3_HA_NAT66_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT66_TABLE_FIELDS_PREFIX_SIZE_E);
            }

            /* set NAT Entry Type to be NAT66 */
            value=1;
            LION3_HA_NAT66_FIELD_VALUE_SET_MAC(devNum,LION3_HA_NAT66_TABLE_FIELDS_NAT_ENTRY_TYPE_E);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/*******************************************************************************
*  prvCpssIpNatConfigHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given NAT entry from hardware format to logic format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*       hwFormatArray   - The entry in HW format (6 words).
*
* OUTPUTS:
*       natTypePtr      - (pointer to) type of the NAT*
*       logicFormatPtr  - (pointer to) NAT entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameters.
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS  prvCpssIpNatConfigHw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_IP_NAT_TYPE_ENT                *natTypePtr,
    OUT CPSS_DXCH_IP_NAT_ENTRY_UNT          *logicFormatPtr
)
{
    GT_U32 value;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    /* zero out hw format */
    cpssOsMemSet(logicFormatPtr,0,sizeof(CPSS_DXCH_IP_NAT_ENTRY_UNT));

    /* NAT Entry Type is located in the same bits in the nat44 entry and in nat66 entry,
       bits 190-191, so we can read either LION3_HA_NAT44_TABLE_FIELDS_NAT_ENTRY_TYPE_E or
       LION3_HA_NAT66_TABLE_FIELDS_NAT_ENTRY_TYPE_E to find the NAT Entry type of the entry  */
    LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NAT_ENTRY_TYPE_E);
    switch (value)
    {
        case 0:
            *natTypePtr = CPSS_IP_NAT_TYPE_NAT44_E;

            LION3_HA_NAT44_FIELD_MAC_DA_GET_MAC(devNum,hwFormatArray,&logicFormatPtr->nat44Entry.macDa.arEther[0]);

            LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_MODIFY_DIP_E);
            logicFormatPtr->nat44Entry.modifyDip= BIT2BOOL_MAC(value);

            LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NEW_DIP_E);
            logicFormatPtr->nat44Entry.newDip.arIP[3] = (GT_U8)(value >> 0);
            logicFormatPtr->nat44Entry.newDip.arIP[2] = (GT_U8)(value >> 8);
            logicFormatPtr->nat44Entry.newDip.arIP[1] = (GT_U8)(value >> 16);
            logicFormatPtr->nat44Entry.newDip.arIP[0] = (GT_U8)(value >> 24);

            LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_MODIFY_SIP_E);
            logicFormatPtr->nat44Entry.modifySip= BIT2BOOL_MAC(value);

            LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NEW_SIP_E);
            logicFormatPtr->nat44Entry.newSip.arIP[3] = (GT_U8)(value >> 0);
            logicFormatPtr->nat44Entry.newSip.arIP[2] = (GT_U8)(value >> 8);
            logicFormatPtr->nat44Entry.newSip.arIP[1] = (GT_U8)(value >> 16);
            logicFormatPtr->nat44Entry.newSip.arIP[0] = (GT_U8)(value >> 24);

            LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_MODIFY_TCP_UDP_DST_PORT_E);
            logicFormatPtr->nat44Entry.modifyTcpUdpDstPort= BIT2BOOL_MAC(value);

            LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NEW_TCP_UDP_DST_PORT_E);
            logicFormatPtr->nat44Entry.newTcpUdpDstPort= value;

            LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_MODIFY_TCP_UDP_SRC_PORT_E);
            logicFormatPtr->nat44Entry.modifyTcpUdpSrcPort= BIT2BOOL_MAC(value);

            LION3_HA_NAT44_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT44_TABLE_FIELDS_NEW_TCP_UDP_SRC_PORT_E);
            logicFormatPtr->nat44Entry.newTcpUdpSrcPort= value;

            break;
        case 1:
            *natTypePtr = CPSS_IP_NAT_TYPE_NAT66_E;

            if(!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
               CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

            LION3_HA_NAT66_FIELD_MAC_DA_GET_MAC(devNum,hwFormatArray,&logicFormatPtr->nat66Entry.macDa.arEther[0]);
            LION3_HA_NAT66_FIELD_ADDRESS_GET_MAC(devNum,hwFormatArray,&logicFormatPtr->nat66Entry.address.arIP[0]);

            LION3_HA_NAT66_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT66_TABLE_FIELDS_MODIFY_COMMAND_E);
            switch (value)
            {
                case 0:
                    logicFormatPtr->nat66Entry.modifyCommand = CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E;
                    break;
                case 1:
                    logicFormatPtr->nat66Entry.modifyCommand = CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E;
                    break;
                case 2:
                     logicFormatPtr->nat66Entry.modifyCommand = CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_ADDRESS_E;
                     break;
                case 3:
                     logicFormatPtr->nat66Entry.modifyCommand = CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_ADDRESS_E;
                     break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            LION3_HA_NAT66_FIELD_VALUE_GET_MAC(devNum,LION3_HA_NAT66_TABLE_FIELDS_PREFIX_SIZE_E);
            /* if hw value is 0 then we need to set sw value to be 64 */
            if (value==0)
            {
                value = 64;
            }
            logicFormatPtr->nat66Entry.prefixSize = value;


            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChIpNatEntrySet
*
* DESCRIPTION:
*       Set a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*       natType       - type of the NAT
*       entryPtr      - (pointer to) NAT entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table, Tunnel start entries table and router ARP addresses
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpNatEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              natIndex,
    IN  CPSS_IP_NAT_TYPE_ENT                natType,
    IN  CPSS_DXCH_IP_NAT_ENTRY_UNT          *entryPtr
)
{
    GT_U32      hwConfigArray[NAT_HW_SIZE_CNS]; /* tunnel start config in hardware format */
    GT_STATUS   rc;         /* function return code */
    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ENT entryType = PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_NAT_E;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    if((!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))||
       ((natType==CPSS_IP_NAT_TYPE_NAT66_E)&& !PRV_CPSS_SIP_5_15_CHECK_MAC(devNum)))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    if(natIndex >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    /* convert NAT entry to hw format */
    rc =  prvCpssIpNatConfigLogic2HwFormat(devNum,natType,entryPtr,hwConfigArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set NAT entry in HW */
    rc = prvCpssDxChHwHaTunnelStartArpNatTableEntryWrite(devNum,
                                                 CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                 entryType,
                                                 natIndex,
                                                 hwConfigArray);
    return rc;
}

/*******************************************************************************
* cpssDxChIpNatEntrySet
*
* DESCRIPTION:
*       Set a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*       natType       - type of the NAT
*       entryPtr      - (pointer to) NAT entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table, Tunnel start entries table and router ARP addresses
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpNatEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              natIndex,
    IN  CPSS_IP_NAT_TYPE_ENT                natType,
    IN  CPSS_DXCH_IP_NAT_ENTRY_UNT          *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpNatEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, natIndex, natType, entryPtr));

    rc = internal_cpssDxChIpNatEntrySet(devNum, natIndex, natType, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, natIndex, natType, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpNatEntryGet
*
* DESCRIPTION:
*       Get a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*
* OUTPUTS:
*       natTypePtr   - (pointer to) type of the NAT
*       entryPtr     - (pointer to) NAT entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table, Tunnel start entries table and router ARP addresses
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpNatEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             natIndex,
    OUT  CPSS_IP_NAT_TYPE_ENT               *natTypePtr,
    OUT  CPSS_DXCH_IP_NAT_ENTRY_UNT         *entryPtr
)
{
    GT_U32      hwConfigArray[NAT_HW_SIZE_CNS]; /* tunnel start config in hardware format */
    GT_STATUS   rc;         /* function return code */
    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ENT entryType=PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_NAT_E;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    if(!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    if(natIndex >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    /* get NAT entry from HW */
    rc = prvCpssDxChHwHaTunnelStartArpNatTableEntryRead(devNum,
                                                 CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                 entryType,
                                                 natIndex,
                                                 hwConfigArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* convert NAT entry hw format to CPSS format */
    rc =  prvCpssIpNatConfigHw2LogicFormat(devNum,hwConfigArray,natTypePtr,entryPtr);


    return rc;
}

/*******************************************************************************
* cpssDxChIpNatEntryGet
*
* DESCRIPTION:
*       Get a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*
* OUTPUTS:
*       natTypePtr   - (pointer to) type of the NAT
*       entryPtr     - (pointer to) NAT entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table, Tunnel start entries table and router ARP addresses
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS cpssDxChIpNatEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             natIndex,
    OUT  CPSS_IP_NAT_TYPE_ENT               *natTypePtr,
    OUT  CPSS_DXCH_IP_NAT_ENTRY_UNT         *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpNatEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, natIndex, natTypePtr, entryPtr));

    rc = internal_cpssDxChIpNatEntryGet(devNum, natIndex, natTypePtr, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, natIndex, natTypePtr, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpNatDroppedPacketsCntGet
*
* DESCRIPTION:
*      Get the counter that counts the packets that were dropped since NAT could not be applied.
*      If the L4 header is not within the header 64 bytes (For tunnel-terminated packet it must
*      be within the passenger header 64 bytes), the packet is dropped and it is counted by
*      the NAT Drop Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       natDropPktsPtr - (pointer to) the number of dropped packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Clear on read.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpNatDroppedPacketsCntGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *natDropPktsPtr
)
{
    GT_STATUS   rc;     /* return code            */
    GT_U32      regAddr;/* the register address   */
    GT_U32      value;  /* the value from regiter */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(natDropPktsPtr);

    if(!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).NATExceptionDropCounter;

    rc = prvCpssPortGroupsCounterSummary(devNum, regAddr, 0,32,&value,NULL);
    if (rc != GT_OK)
    {
        return rc;
    }

    *natDropPktsPtr  = value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpNatDroppedPacketsCntGet
*
* DESCRIPTION:
*      Get the counter that counts the packets that were dropped since NAT could not be applied.
*      If the L4 header is not within the header 64 bytes (For tunnel-terminated packet it must
*      be within the passenger header 64 bytes), the packet is dropped and it is counted by
*      the NAT Drop Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       natDropPktsPtr - (pointer to) the number of dropped packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Clear on read.
*
*******************************************************************************/
GT_STATUS cpssDxChIpNatDroppedPacketsCntGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *natDropPktsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpNatDroppedPacketsCntGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, natDropPktsPtr));

    rc = internal_cpssDxChIpNatDroppedPacketsCntGet(devNum, natDropPktsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, natDropPktsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


