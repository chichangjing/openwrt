/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*  cpssDxChBrgFdbAu.c
*
* DESCRIPTION:
*       This file provides the CPSS DxCh FDB Address Update (AU) functionality
*
*
* FILE REVISION NUMBER:
*       $Revision: 59 $
*
*******************************************************************************/

/* #define CPSS_DXCH_FDB_DUMP */

#ifdef CPSS_DXCH_FDB_DUMP
    #define debugPrint(_x)      cpssOsPrintf _x
#else
    #define debugPrint(_x)
#endif
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
/* get DxCh FDB types */
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrg.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
/* check errata */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChErrataMng.h>
/* get driver level structs information */
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbAu.h>


/* check that mask enabled/not and application asked for the SW filtering  */
#define NON_SOURCE_PORT_GROUP_FILTER_CHECK_MAC(_devNum)             \
    (PRV_CPSS_FDB_AUQ_FROM_NON_SOURCE_PORT_GROUP_WA_E(_devNum) &&   \
     PRV_CPSS_FDB_AUQ_FROM_NON_SOURCE_PORT_GROUP_FILTER_ENABLED_WA_E(_devNum))

#define HW_FORMAT_2_CMD_MAC(hwData,macCmd)                                 \
    macCmd = (((hwData) == 0) ? CPSS_MAC_TABLE_FRWRD_E  :                  \
                    (((hwData) == 1) ? CPSS_MAC_TABLE_MIRROR_TO_CPU_E :    \
                    (((hwData) == 2) ? CPSS_MAC_TABLE_CNTL_E :             \
                    (((hwData) == 3) ? CPSS_MAC_TABLE_DROP_E :             \
                    (((hwData) == 4) ? CPSS_MAC_TABLE_SOFT_DROP_E :        \
                                       CPSS_MAC_TABLE_FRWRD_E)))));

/* number of words in the Au (address update) message */
#define CPSS_DXCH_FDB_AU_MSG_WORDS_SIZE_CNS   8


/* the number of AU messages to */
GT_U32  prvCpssDxChPerPortGroupNumOfAu  = 1;/* <-- allow to modify from test in order to check performances*/

/* mask to remove the LSBit from a value .
   needed for dual device Id logic,
for example :
    x = 4
    y = 5

    (x & MASK_DUAL_DEVICE_ID_LSB_CNS) == (y & MASK_DUAL_DEVICE_ID_LSB_CNS)
*/
#define MASK_DUAL_DEVICE_ID_LSB_CNS     0xFFFFFFFE

/*******************************************************************************
* auDescMultiPortGroupNonSourceFilterCheck
*
* DESCRIPTION:
*       This function check if need to filter message from the AUQ/FUQ due to
*       'non source port group'
*
*       When Enabled - AU/FU messages are filtered by CPSS if the MAC entry
*       does NOT reside on the local port group, i.e. the entry port[5:4] != port group
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum             - device number
*       portGroupId        - the portGroupId - for multi-port-groups support
*       swapedAuDescExtPtr    - (pointer) to the AU descriptor filled by PP.
*                   this is pointer to 'swapped' auDesc
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - need to filter the message
*       GT_FALSE - NO need to filter the message
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_BOOL auDescMultiPortGroupNonSourceFilterCheck
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  PRV_CPSS_AU_DESC_EXT_8_STC    *swapedAuDescExtPtr
)
{
    GT_U32  associatedDevNum;
    GT_U32  portGroupIdValue;
    GT_U32  updType;

     /* When Enabled - AU/FU messages are filtered by CPSS if the MAC entry
       does NOT reside on the local port group, i.e. the entry port[5:4] != port group */

    /* The FDB message type 5 (CPSS_SA_E) is used only for Cheetah 2 devices
     for FDB Upload messages.*/
    updType = (GT_U8)U32_GET_FIELD_MAC(swapedAuDescExtPtr->elem0.word0, 4, 3);
    if(updType == 5)
       updType = CPSS_FU_E;

    switch(updType)
    {
        case CPSS_AA_E:
        case CPSS_TA_E:
        case CPSS_FU_E:
             /*Only messages that generated by the 'Aging deamon'
               (auto/triggered action ) , apply to the filter */
            break;
        default:
            /* those messages are not filtered by the device .*/
            return GT_FALSE;
    }

    associatedDevNum = U32_GET_FIELD_MAC(swapedAuDescExtPtr->elem0.word3, 7, 5);

    if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(associatedDevNum))
    {
        /* check associatedDevNum without LSB */
        if((PRV_CPSS_HW_DEV_NUM_MAC(devNum) & MASK_DUAL_DEVICE_ID_LSB_CNS) != (associatedDevNum & MASK_DUAL_DEVICE_ID_LSB_CNS))
        {
            return GT_FALSE;
        }
    }
    else
    /* check that the entry associated with current device */
    if(PRV_CPSS_HW_DEV_NUM_MAC(devNum) != associatedDevNum)
    {
        return GT_FALSE;
    }

    /* type must be CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E */
    if(0 != U32_GET_FIELD_MAC(swapedAuDescExtPtr->elem0.word3,19, 2))
    {
        return GT_FALSE;
    }

    /* check multiple bit */
    if(1 == U32_GET_FIELD_MAC(swapedAuDescExtPtr->elem0.word2,15, 1))
    {
        return GT_FALSE;
    }

    /* check multicast bit (bit 40 in macAddr[])*/
    if(1 == U32_GET_FIELD_MAC(swapedAuDescExtPtr->elem0.word1,24, 1))
    {
        return GT_FALSE;
    }

    /* check isTrunk bit  */
    if(1 == U32_GET_FIELD_MAC(swapedAuDescExtPtr->elem0.word2,17, 1))
    {
        return GT_FALSE;
    }

    /* check portGroupId that is bits 4,5 in the portNum that is from bit 18 -> 6 bits
       (for Bobcat2; Caelum; Bobcat3 3 bits)*/
    portGroupIdValue = U32_GET_FIELD_MAC(swapedAuDescExtPtr->elem0.word2,22,3);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        /* Lion2 even though has 8 port groups , still the port number holds only 6 bits
        so only 2 bits in it for the port group id */
        portGroupIdValue &= 0x3;
    }
    else
    {
        portGroupIdValue &= PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PORT_GROUP_ID_MAC(devNum);
    }

    if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(associatedDevNum)
       && (associatedDevNum & 1))
    {
        /* the MSB of the port group ID is in the LSB on the associatedDevNum */
        portGroupIdValue |= 1 << (PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId - 1);
    }

    /* check that the entry associated with current device */
    if(portGroupId == portGroupIdValue)
    {
        return GT_FALSE;
    }

    /* need to filter the message */
    return GT_TRUE;
}


/*******************************************************************************
* prvAuDesc2UpdMsgUcRoute
*
* DESCRIPTION:
*       This function is called whenever an address update message is received.
*       It translates the descriptor into UC Route format
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - number of device whose descriptor queue is processed
*       auMsgArrayOfWordsPtr - (pointer to) consider the AU message as array of words
*
* OUTPUTS:
*       addrUpMsgPtr     - (pointer to) MAC format of entry
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvAuDesc2UpdMsgUcRoute
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          *auMsgArrayOfWordsPtr,
    OUT CPSS_MAC_UPDATE_MSG_EXT_STC     *addrUpMsgPtr
)
{
    CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryExtKeyPtr; /* pointer to mac entry */
    GT_U32                  hwValue;             /* hardware value of the field      */
    GT_U32                  wordNum;/* used for ipv6 address */


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_MSG_TYPE_E,
        hwValue)
    addrUpMsgPtr->updType = (CPSS_UPD_MSG_TYPE_ENT)hwValue;


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TARGET_DEVICE_E,
        hwValue)
    addrUpMsgPtr->associatedHwDevNum = hwValue;


    /* The FDB message type CPSS_SA_E is used only for Cheetah 2 devices
        for FDB Upload messages.*/
    if(addrUpMsgPtr->updType == CPSS_SA_E)
        addrUpMsgPtr->updType = CPSS_FU_E;


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_CHAIN_TOO_LONG_E,
        hwValue);
    addrUpMsgPtr->naChainIsTooLong = BIT2BOOL_MAC(hwValue);

    addrUpMsgPtr->queryHwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);

    if((addrUpMsgPtr->updType == CPSS_NA_E) ||
       (addrUpMsgPtr->updType == CPSS_QR_E) ||
       (addrUpMsgPtr->updType == CPSS_QA_E))
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_OFFSET_E,
            hwValue);
        addrUpMsgPtr->entryOffset = hwValue;

    }

    if((addrUpMsgPtr->updType == CPSS_AA_E) ||
        (addrUpMsgPtr->updType == CPSS_TA_E))
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MAC_ADDR_INDEX_E,
            hwValue);
        addrUpMsgPtr->macEntryIndex = hwValue;
    }

    macEntryExtKeyPtr = &(addrUpMsgPtr->macEntry.key);

    /* set Uc Route key */
    switch(macEntryExtKeyPtr->entryType)
    {
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:

        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E,
            hwValue);
        macEntryExtKeyPtr->key.ipv4Unicast.vrfId = hwValue;

        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV4_DIP_E,
            hwValue);
        macEntryExtKeyPtr->key.ipv4Unicast.dip[3] = (GT_U8)(hwValue >> 0);
        macEntryExtKeyPtr->key.ipv4Unicast.dip[2] = (GT_U8)(hwValue >> 8);
        macEntryExtKeyPtr->key.ipv4Unicast.dip[1] = (GT_U8)(hwValue >> 16);
        macEntryExtKeyPtr->key.ipv4Unicast.dip[0] = (GT_U8)(hwValue >> 24);

        break;
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
        /* macEntryType is CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E*/
        /* get IPv6 UC DIP */
        for(wordNum = 0; wordNum < 4; wordNum++)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
               LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_0_E + wordNum,
               hwValue);

            macEntryExtKeyPtr->key.ipv6Unicast.dip[((3 - wordNum) * 4)]     = (GT_U8)(hwValue >> 24);
            macEntryExtKeyPtr->key.ipv6Unicast.dip[((3 - wordNum) * 4) + 1] = (GT_U8)(hwValue >> 16);
            macEntryExtKeyPtr->key.ipv6Unicast.dip[((3 - wordNum) * 4) + 2] = (GT_U8)(hwValue >> 8);
            macEntryExtKeyPtr->key.ipv6Unicast.dip[((3 - wordNum) * 4) + 3] = (GT_U8)(hwValue);
        }

        /* get NH data bank number */
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
           LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_NH_DATA_BANK_NUM_E,
           hwValue);
        addrUpMsgPtr->macEntry.fdbRoutingInfo.nextHopDataBankNumber = hwValue;

        break;
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
         /* For AA type - No support for message of type
            CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E
            in this case the message should be filtered
            in auDesc2UpdMsg.
            For TA type we should expect message on the ipv6_data */

        if(addrUpMsgPtr->updType == CPSS_AA_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        else
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E,
                hwValue);
            macEntryExtKeyPtr->key.ipv6Unicast.vrfId = hwValue;
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* If this is a QR message copy entryFound */
    if (CPSS_QR_E == addrUpMsgPtr->updType)
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_ENTRY_FOUND_E,
            hwValue);
        addrUpMsgPtr->entryWasFound = BIT2BOOL_MAC(hwValue);
    }
    else
    {
        addrUpMsgPtr->entryWasFound = GT_FALSE;
    }

    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_SKIP_E,
        hwValue);
    addrUpMsgPtr->skip = BIT2BOOL_MAC(hwValue);

    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E,
        hwValue);
    addrUpMsgPtr->aging = BIT2BOOL_MAC(hwValue);

    addrUpMsgPtr->macEntry.age = addrUpMsgPtr->aging;

    /* ttl/hop decrement enable */
    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_DEC_TTL_OR_HOP_COUNT_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.ttlHopLimitDecEnable =  BIT2BOOL_MAC(hwValue);

    /* Bypass TTL Options Or Hop Extension */
    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.ttlHopLimDecOptionsExtChkByPass =  BIT2BOOL_MAC(hwValue);

    /* ingress mirror to analyzer index */
    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_INGRESS_MIRROR_TO_ANALYZER_INDEX_E,
        hwValue);

    if (hwValue==0)
    {
        addrUpMsgPtr->macEntry.fdbRoutingInfo.ingressMirror=GT_FALSE;
        addrUpMsgPtr->macEntry.fdbRoutingInfo.ingressMirrorToAnalyzerIndex = 0;
    }
    else
    {
        addrUpMsgPtr->macEntry.fdbRoutingInfo.ingressMirror=GT_TRUE;
        addrUpMsgPtr->macEntry.fdbRoutingInfo.ingressMirrorToAnalyzerIndex = hwValue-1;

    }

    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_MARKING_EN_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.qosProfileMarkingEnable = BIT2BOOL_MAC(hwValue);


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_INDEX_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.qosProfileIndex = hwValue;


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_PRECEDENCE_E,
        hwValue);

    switch(hwValue)
    {
    case 0:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        break;
    default:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        break;
    }


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MODIFY_UP_E,
        hwValue);
    switch(hwValue)
    {
    case 0:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
        break;
    case 1:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        break;
    case 2:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        break;
    default:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.modifyUp = hwValue;
        break;
    }


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MODIFY_DSCP_E,
        hwValue);
    switch(hwValue)
    {
    case 0:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
        break;
    case 1:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        break;
    case 2:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        break;
    default:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.modifyDscp = hwValue;
        break;
    }

    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_COUNTER_SET_INDEX_E,
        hwValue);
    switch(hwValue)
    {
    case 0:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.countSet = CPSS_IP_CNT_SET0_E;
        break;
    case 1:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.countSet = CPSS_IP_CNT_SET1_E;
        break;
    case 2:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.countSet = CPSS_IP_CNT_SET2_E;
        break;
    case 3:
         addrUpMsgPtr->macEntry.fdbRoutingInfo.countSet = CPSS_IP_CNT_SET3_E;
        break;
    case 7:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.countSet = CPSS_IP_CNT_NO_SET_E;
        break;
    default:
        addrUpMsgPtr->macEntry.fdbRoutingInfo.countSet = hwValue;
        break;
    }


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ARP_BC_TRAP_MIRROR_EN_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.trapMirrorArpBcEnable=BIT2BOOL_MAC(hwValue);

    /* dip access level */
    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_DIP_ACCESS_LEVEL_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.dipAccessLevel = hwValue;

    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ICMP_REDIRECT_EXCEP_MIRROR_EN_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.ICMPRedirectEnable = BIT2BOOL_MAC(hwValue);

    /* mtu profile index */
    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MTU_INDEX_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.mtuProfileIndex = hwValue;

    /* set vlan associated with this entry */
    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_NEXT_HOP_EVLAN_E,
        hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.nextHopVlanId = (GT_U16)hwValue;


    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_USE_VIDX_E,
        hwValue);
    if(hwValue==1)/* use vidx*/
    {
        addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

         /* get vidx value */
         LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
             LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EVIDX_E,
             hwValue);

         addrUpMsgPtr->macEntry.dstInterface.vidx = (GT_U16)hwValue;
         if(addrUpMsgPtr->macEntry.dstInterface.vidx==0xFFF)
         {
             addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_VID_E;
             addrUpMsgPtr->macEntry.dstInterface.vlanId = addrUpMsgPtr->macEntry.fdbRoutingInfo.nextHopVlanId;
         }
    }
    else /* not a vidx */
    {

         LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
             LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E,
             hwValue);
         if(hwValue==1)/* trunk interface */
         {
             LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
               LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TRUNK_NUM_E,
               hwValue);

             addrUpMsgPtr->macEntry.dstInterface.trunkId = (GT_TRUNK_ID)hwValue;
             addrUpMsgPtr->macEntry.dstInterface.hwDevNum = addrUpMsgPtr->associatedHwDevNum;
             addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

         }
         else/* port interface */
         {
             LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EPORT_NUM_E,
                 hwValue);

            addrUpMsgPtr->macEntry.dstInterface.devPort.portNum = hwValue;
            addrUpMsgPtr->macEntry.dstInterface.devPort.hwDevNum = addrUpMsgPtr->associatedHwDevNum;
            addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
         }
    }

    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
     LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_START_E,
     hwValue);
    addrUpMsgPtr->macEntry.fdbRoutingInfo.isTunnelStart = BIT2BOOL_MAC(hwValue);

    if(addrUpMsgPtr->macEntry.fdbRoutingInfo.isTunnelStart==GT_TRUE)
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_PTR_E,
        hwValue);
         addrUpMsgPtr->macEntry.fdbRoutingInfo.nextHopTunnelPointer=hwValue;
    }
    else
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
        LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ARP_PTR_E,
        hwValue);
        addrUpMsgPtr->macEntry.fdbRoutingInfo.nextHopARPPointer=hwValue;
    }

    return GT_OK;
}

/*******************************************************************************
* auDesc2UpdMsg
*
* DESCRIPTION:
*       This function is called whenever an address update message is received.
*       It translates the descriptor into CPSS_MAC_UPDATE_MSG_EXT_STC format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - number of device whose descriptor queue is processed
*       portGroupId    - the portGroupId - for multi-port-groups support
*       auDescExtPtr   - (pointer to) the AU descriptor filled by PP.
*       auDescrFromDma - AU descriptor is from DMA queue
*                        GT_TRUE - AU descriptor is from DMA queue
*                        GT_FALSE - AU descriptor is from FIFO but not DMA queue
*
* OUTPUTS:
*       addrUpMsgPtr     - MAC format of entry
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS auDesc2UpdMsg
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  PRV_CPSS_AU_DESC_EXT_8_STC    *auDescExtPtr,
    IN  GT_BOOL                       auDescrFromDma,
    OUT CPSS_MAC_UPDATE_MSG_EXT_STC   *addrUpMsgPtr
)
{
    GT_STATUS                       rc=GT_OK;
    PRV_CPSS_AU_DESC_EXT_8_STC      auDescExt;      /* Holds the address update         */
                                                    /* descriptor after byte-swapping.  */
    GT_U32                                  multiple;
    GT_BOOL                                 fixFEr22;       /* GT_TRUE - handle errata 22 */
    CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryExtKeyPtr; /* pointer to mac entry key */
    GT_U16                                  vlanId;         /* vlan id */
    PRV_CPSS_AU_DESC_STC                *auDescPtr = &auDescExtPtr->elem0;
    GT_U32                              auMessageNumWords;
    GT_U32                  naFormatVersion; /* Address message format version       */
    GT_U32                  oldPortTrunk;   /* old port or trunk number                    */
    GT_U32                  hwValue;             /* hardware value of the field      */
    GT_U32                  *auMsgArrayOfWordsPtr;/*consider the AU message as array of words */
    PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC specialFields;/*special Muxed fields in the AU msg */
    GT_U32                  macEntryType=0;         /* entry type   */
    GT_U32                  useVidx = 0;/*indication of using VIDX*/
    GT_U32                  isTrunk;/*indication of using trunk*/

    auMessageNumWords = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.auMessageNumOfWords;


    if (auDescrFromDma == GT_TRUE)
    {
        /* AU descriptor from DMA queue must be handled by swap procedure. */

        AU_DESC_SWAP_MAC(&auDescExtPtr->elem0,auDescExt.elem0);

        if(auMessageNumWords == 8)
        {
            AU_DESC_SWAP_MAC(&auDescExtPtr->elem1,auDescExt.elem1);
        }

    }
    else
    {
        /* AU descriptor from FIFO already handled by swap procedure in the
           prvCpssHwPpPortGroupReadRegister. */
        auDescExt = *auDescExtPtr;
    }

    /* consider the AU message as array of words .
       by this we can treat it as 'fields format in table entry' */
    auMsgArrayOfWordsPtr = &(auDescExt.elem0.word0);

    if(NON_SOURCE_PORT_GROUP_FILTER_CHECK_MAC(devNum))
    {
        /* check if need to 'filter' the message */

        /* function auDescMultiPortGroupNonSourceFilterCheck , must be called with
           'swapped' auDesc */
        if(GT_TRUE == auDescMultiPortGroupNonSourceFilterCheck(devNum,portGroupId,&auDescExt))
        {
            /* need to filter the message */
            AU_DESC_RESET_MAC(auDescPtr);
            return GT_OK;
        }
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance == GT_TRUE)
    {
        /* single FDB instance , state 'unaware' mode */
        addrUpMsgPtr->portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }
    else
    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* multi port groups support . needed even in unified FDBs for the trunk
        entries*/
        addrUpMsgPtr->portGroupId = portGroupId;
    }
    else
    {
        /* other devices are 'unaware' */
        addrUpMsgPtr->portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
         /* get entry type */
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_FDB_ENTRY_TYPE_E,
            hwValue);
        macEntryType = hwValue;

        /* UC Route entry */
        if(macEntryType >= CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E &&
           macEntryType <= CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E)
        {
            cpssOsMemSet(&(addrUpMsgPtr->macEntry), 0, sizeof(CPSS_MAC_ENTRY_EXT_STC));
            addrUpMsgPtr->macEntry.key.entryType = macEntryType;

            rc = prvAuDesc2UpdMsgUcRoute(devNum, auMsgArrayOfWordsPtr, addrUpMsgPtr);
            if(rc != GT_OK)
            {
                AU_DESC_RESET_MAC(auDescPtr);
                return GT_OK;
            }

            /* we are done with parsing the UC entry ! */
            return GT_OK;
        }

        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_MSG_TYPE_E,
            hwValue)
        addrUpMsgPtr->updType = (CPSS_UPD_MSG_TYPE_ENT)hwValue;


        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_DEV_ID_E,
            hwValue)
        addrUpMsgPtr->associatedHwDevNum = hwValue;
    }
    else
    {
        addrUpMsgPtr->updType = (CPSS_UPD_MSG_TYPE_ENT)U32_GET_FIELD_MAC(auDescExt.elem0.word0, 4, 3);
        addrUpMsgPtr->associatedHwDevNum = U32_GET_FIELD_MAC(auDescExt.elem0.word3, 7, 5);
    }

    /* The FDB message type 5 (CPSS_SA_E) is used only for Cheetah 2 devices
     for FDB Upload messages.*/
    if(addrUpMsgPtr->updType == 5)
        addrUpMsgPtr->updType = CPSS_FU_E;

    if ((PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        && (addrUpMsgPtr->updType == CPSS_NA_E))
    {
        naFormatVersion  = 1;
    }
    else
    {
        naFormatVersion  = 0;
    }

    if(addrUpMsgPtr->updType == CPSS_AA_E)
    {
        fixFEr22 = PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                PRV_CPSS_DXCH_FDB_AGING_WITH_REMOVAL_MODE_WA_E);

        if((fixFEr22 == GT_TRUE) &&
           (0 == (PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.devTable &
                    (1 << addrUpMsgPtr->associatedHwDevNum))))
          {
            /* FEr #22:
               In Aging with removal mode, AA message is sent for entries
               removed due to an invalid device
            */

            /* Workaround :
               The CPU should ignore those messages,
               after ensuring that the DeviceId is invalid.
            */

            AU_DESC_RESET_MAC(auDescPtr);
            return GT_OK;

        }
    }

    macEntryExtKeyPtr = &(addrUpMsgPtr->macEntry.key);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        (void) prvCpssDxChBrgFdbLion3FdbAuMsgSpecialMuxedFieldsGet(devNum,auMsgArrayOfWordsPtr,macEntryType,&specialFields);

        if(specialFields.srcId == PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            addrUpMsgPtr->macEntry.sourceID = 0;
        }
        else
        {
            addrUpMsgPtr->macEntry.sourceID = specialFields.srcId;
        }

        addrUpMsgPtr->macEntry.userDefined = specialFields.udb;

        if(specialFields.origVid1 == PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
        {
            addrUpMsgPtr->vid1 = 0;
        }
        else
        {
            addrUpMsgPtr->vid1 = (GT_U16)specialFields.origVid1;
        }

        addrUpMsgPtr->macEntry.key.vid1 = addrUpMsgPtr->vid1;

        if(naFormatVersion)
        {
            /* the message not hold the fields of daSecurityLevel,saAccessLevel */
            addrUpMsgPtr->macEntry.daSecurityLevel = 0;
            addrUpMsgPtr->macEntry.saSecurityLevel = 0;
        }
        else
        {
            if(specialFields.daAccessLevel == PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
            {
                addrUpMsgPtr->macEntry.daSecurityLevel = 0;
            }
            else
            {
                addrUpMsgPtr->macEntry.daSecurityLevel = specialFields.daAccessLevel;
            }

            if(specialFields.saAccessLevel == PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS)
            {
                addrUpMsgPtr->macEntry.saSecurityLevel = 0;
            }
            else
            {
                addrUpMsgPtr->macEntry.saSecurityLevel = specialFields.saAccessLevel;
            }
        }

        macEntryExtKeyPtr->entryType = macEntryType;
        macEntryExtKeyPtr->vid1 = addrUpMsgPtr->macEntry.key.vid1;
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E, hwValue);
        vlanId = (GT_U16)hwValue;
        
        if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum) &&
            PRV_CPSS_DXCH_FDB_LOOKUP_KEY_MODE_PER_FID_IS_DOUBLE_MAC(devNum, vlanId))
        {
            switch (macEntryExtKeyPtr->entryType)
            {
                case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
                    macEntryExtKeyPtr->entryType =
                        CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E;
                    break;
                case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
                    macEntryExtKeyPtr->entryType =
                        CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E;
                    break;
                case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
                    macEntryExtKeyPtr->entryType =
                        CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E;
                    break;
                default:
                    break;
            }
        }
        
        if(macEntryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
        {
            macEntryExtKeyPtr->key.macVlan.vlanId = vlanId;

            LION3_FDB_AU_MSG_FIELD_MAC_ADDR_GET_MAC(devNum, auMsgArrayOfWordsPtr,
                &macEntryExtKeyPtr->key.macVlan.macAddr.arEther[0]);

            if(macEntryExtKeyPtr->key.macVlan.macAddr.arEther[0] & 1)
            {
                useVidx = 1;
            }
        }
        else
        {
            macEntryExtKeyPtr->key.ipMcast.vlanId = vlanId;
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_DIP_E,
                hwValue);
            macEntryExtKeyPtr->key.ipMcast.dip[3] = (GT_U8)(hwValue >> 0);
            macEntryExtKeyPtr->key.ipMcast.dip[2] = (GT_U8)(hwValue >> 8);
            macEntryExtKeyPtr->key.ipMcast.dip[1] = (GT_U8)(hwValue >> 16);
            macEntryExtKeyPtr->key.ipMcast.dip[0] = (GT_U8)(hwValue >> 24);

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SIP_E,
                hwValue);
            macEntryExtKeyPtr->key.ipMcast.sip[3] = (GT_U8)(hwValue >> 0);
            macEntryExtKeyPtr->key.ipMcast.sip[2] = (GT_U8)(hwValue >> 8);
            macEntryExtKeyPtr->key.ipMcast.sip[1] = (GT_U8)(hwValue >> 16);
            macEntryExtKeyPtr->key.ipMcast.sip[0] = (GT_U8)(hwValue >> 24);
        }

        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_E,
            hwValue);
        addrUpMsgPtr->macEntry.appSpecificCpuCode = BIT2BOOL_MAC(hwValue);

    }
    else
    {
        if (naFormatVersion == 0)
        {
            macEntryExtKeyPtr->entryType = (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 19, 2);
        }
        else
        {
            /* xCat2 use only MAC address NA message */
            macEntryExtKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        }

        if(macEntryExtKeyPtr->entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
        {
            macEntryExtKeyPtr->key.macVlan.vlanId =
                                         (GT_U16)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 0, 12);
            vlanId = macEntryExtKeyPtr->key.macVlan.vlanId;

            macEntryExtKeyPtr->key.macVlan.macAddr.arEther[5] =
                                          (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word0, 16, 8);
            macEntryExtKeyPtr->key.macVlan.macAddr.arEther[4] =
                                          (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word0, 24, 8);
            macEntryExtKeyPtr->key.macVlan.macAddr.arEther[3] =
                                           (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word1, 0, 8);
            macEntryExtKeyPtr->key.macVlan.macAddr.arEther[2] =
                                           (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word1, 8, 8);
            macEntryExtKeyPtr->key.macVlan.macAddr.arEther[1] =
                                          (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word1, 16, 8);
            macEntryExtKeyPtr->key.macVlan.macAddr.arEther[0] =
                                          (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word1, 24, 8);

            if(macEntryExtKeyPtr->key.macVlan.macAddr.arEther[0] & 1)
            {
                useVidx = 1;
            }
        }
        else /* The entry is IPv4 or IPv6 Multicast */
        {
            macEntryExtKeyPtr->key.ipMcast.vlanId =
                                         (GT_U16)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 0, 12);
            vlanId = macEntryExtKeyPtr->key.ipMcast.vlanId;

            macEntryExtKeyPtr->key.ipMcast.dip[3] =
                (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word0, 16, 8);
            macEntryExtKeyPtr->key.ipMcast.dip[2] =
                (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word0, 24, 8);
            macEntryExtKeyPtr->key.ipMcast.dip[1] =
                 (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word1, 0, 8);
            macEntryExtKeyPtr->key.ipMcast.dip[0] =
                 (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word1, 8, 8);
            macEntryExtKeyPtr->key.ipMcast.sip[3] =
                 (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word1, 16, 8);
            macEntryExtKeyPtr->key.ipMcast.sip[2] =
                (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word1, 24, 8);
            macEntryExtKeyPtr->key.ipMcast.sip[1] =
                (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 0, 8);
            macEntryExtKeyPtr->key.ipMcast.sip[0] =
                 (GT_U8)(U32_GET_FIELD_MAC(auDescExt.elem0.word3, 8, 4) |
                 (U32_GET_FIELD_MAC(auDescExt.elem0.word3, 27, 4) << 4));
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            if (naFormatVersion == 0)
            {
                addrUpMsgPtr->macEntry.daSecurityLevel =
                    (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word0, 1, 3);
                addrUpMsgPtr->macEntry.saSecurityLevel =
                    (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word0, 12, 3);
            }
            else
            {
                /* set default values */
                addrUpMsgPtr->macEntry.daSecurityLevel =
                addrUpMsgPtr->macEntry.saSecurityLevel = 0;
            }
            addrUpMsgPtr->macEntry.appSpecificCpuCode =
                                        (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 29, 1);
        }
    }

    /* If this is a QR message copy entryFound */
    if (CPSS_QR_E == addrUpMsgPtr->updType)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_ENTRY_FOUND_E,
                hwValue);
            addrUpMsgPtr->entryWasFound = BIT2BOOL_MAC(hwValue);
        }
        else
        {
            addrUpMsgPtr->entryWasFound = (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word0, 15, 1);
        }
    }
    else
    {
        addrUpMsgPtr->entryWasFound = GT_FALSE;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_SKIP_E,
            hwValue);
        addrUpMsgPtr->skip = BIT2BOOL_MAC(hwValue);

        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E,
            hwValue);
        addrUpMsgPtr->aging = BIT2BOOL_MAC(hwValue);
    }
    else
    {
        if (naFormatVersion == 0)
        {
            addrUpMsgPtr->skip =  (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 12, 1);
            addrUpMsgPtr->aging = (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 13, 1);
        }
        else
        {
            /* set default values */
            addrUpMsgPtr->skip  = GT_FALSE;
            addrUpMsgPtr->aging = GT_TRUE;
        }
    }

    addrUpMsgPtr->macEntry.age = addrUpMsgPtr->aging;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_SP_UNKNOWN_E,
            hwValue);
        addrUpMsgPtr->macEntry.spUnknown = BIT2BOOL_MAC(hwValue);
    }
    else
    {
        addrUpMsgPtr->macEntry.spUnknown =
            BIT2BOOL_MAC(((auDescExt.elem0.word2 >> 14) & 1));
    }

    if(macEntryExtKeyPtr->entryType != CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E &&
       macEntryExtKeyPtr->entryType != CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E)
    {
        /*ipmc-ipv4/6*/
        useVidx = 1;
    }

    if (naFormatVersion == 0)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_MULTIPLE_E,
                hwValue);
            multiple = hwValue;
        }
        else
        {
            multiple = U32_GET_FIELD_MAC(auDescExt.elem0.word2,15, 1);
        }
    }
    else
    {
        /* set default values */
        /*this format not hold info about VIDX */
        useVidx = 0;
        multiple = 0;
    }

    if(multiple || useVidx)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_VIDX_E,
                hwValue);
            addrUpMsgPtr->macEntry.dstInterface.vidx = (GT_U16)hwValue;
        }
        else
        {
            addrUpMsgPtr->macEntry.dstInterface.vidx =
                (GT_U16)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 17, 12);
        }

        /* multiple or multicast */
        if(addrUpMsgPtr->macEntry.dstInterface.vidx == 0xFFF)
        {
            addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_VID_E;
            addrUpMsgPtr->macEntry.dstInterface.vlanId = vlanId;
        }
        else
        {
            addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        }

    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* is trunk */
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_IS_TRUNK_E,
                hwValue);
            isTrunk = hwValue;
            if(isTrunk)
            {
                LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_TRUNK_NUM_E,
                    hwValue);
                addrUpMsgPtr->macEntry.dstInterface.trunkId = (GT_TRUNK_ID)hwValue;
                addrUpMsgPtr->macEntry.dstInterface.hwDevNum = addrUpMsgPtr->associatedHwDevNum;
                addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;
            }
            else
            {
                LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_EPORT_NUM_E,
                    hwValue);
                addrUpMsgPtr->macEntry.dstInterface.devPort.portNum = hwValue;

                addrUpMsgPtr->macEntry.dstInterface.devPort.hwDevNum =
                    addrUpMsgPtr->associatedHwDevNum;
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(addrUpMsgPtr->macEntry.dstInterface));
                addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
            }
        }
        else
        {
            addrUpMsgPtr->macEntry.userDefined = U32_GET_FIELD_MAC(auDescExt.elem0.word2, 25, 4);

            if((GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 17, 1) == 1)
            {/* the message is associated with a trunk */
                addrUpMsgPtr->macEntry.dstInterface.trunkId =
                    (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 18, 7);

                addrUpMsgPtr->macEntry.dstInterface.hwDevNum = addrUpMsgPtr->associatedHwDevNum;
                addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;
            }
            else/* the message is associated with a port */
            {
                addrUpMsgPtr->macEntry.dstInterface.devPort.portNum =
                    (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 18, 6);
                addrUpMsgPtr->macEntry.dstInterface.devPort.hwDevNum =
                    addrUpMsgPtr->associatedHwDevNum;
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(addrUpMsgPtr->macEntry.dstInterface));
                addrUpMsgPtr->macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
            }
        }
    }

    /* set associated hwDevNum with the ownDevNum of the hemisphere */
    if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(addrUpMsgPtr->associatedHwDevNum) &&
       (addrUpMsgPtr->macEntry.key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E
        || addrUpMsgPtr->macEntry.key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E))
    {
        /* set associated hwDevNum without the hemisphere indication */
        addrUpMsgPtr->associatedHwDevNum &= MASK_DUAL_DEVICE_ID_LSB_CNS;
    }


    if (naFormatVersion == 0)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_IS_STATIC_E,
                hwValue);
            addrUpMsgPtr->macEntry.isStatic = BIT2BOOL_MAC(hwValue);

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_DA_QOS_PARAM_SET_IDX_E,
                hwValue);
            addrUpMsgPtr->macEntry.daQosIndex = hwValue;

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SA_QOS_PARAM_SET_IDX_E,
                hwValue);
            addrUpMsgPtr->macEntry.saQosIndex = hwValue;

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_DA_CMD_E,
                hwValue);
            HW_FORMAT_2_CMD_MAC(hwValue,addrUpMsgPtr->macEntry.daCommand);

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SA_CMD_E,
                hwValue);
            HW_FORMAT_2_CMD_MAC(hwValue,addrUpMsgPtr->macEntry.saCommand);

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_SA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E,
                hwValue);
            addrUpMsgPtr->macEntry.saMirrorToRxAnalyzerPortEn = BIT2BOOL_MAC(hwValue);

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E,
                hwValue);
            addrUpMsgPtr->macEntry.daMirrorToRxAnalyzerPortEn = BIT2BOOL_MAC(hwValue);

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_DA_ROUTE_E,
                hwValue);
            addrUpMsgPtr->macEntry.daRoute = BIT2BOOL_MAC(hwValue);
        }
        else
        {
            addrUpMsgPtr->macEntry.isStatic = (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 18, 1);

            addrUpMsgPtr->macEntry.daQosIndex = (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 15, 3);
            addrUpMsgPtr->macEntry.saQosIndex = (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 12, 3);

            HW_FORMAT_2_CMD_MAC((GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 21, 3),
                            addrUpMsgPtr->macEntry.daCommand);
            HW_FORMAT_2_CMD_MAC((GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 24, 3),
                             addrUpMsgPtr->macEntry.saCommand);


            addrUpMsgPtr->macEntry.mirrorToRxAnalyzerPortEn =
                                            (GT_BOOL)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 31, 1);

            addrUpMsgPtr->macEntry.daRoute = (GT_BOOL)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 30, 1);
        }
    }
    else
    {
        /* set default values */
        addrUpMsgPtr->macEntry.isStatic = GT_FALSE;
        addrUpMsgPtr->macEntry.daQosIndex =
        addrUpMsgPtr->macEntry.saQosIndex = 0;
        addrUpMsgPtr->macEntry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
        addrUpMsgPtr->macEntry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
        addrUpMsgPtr->macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        addrUpMsgPtr->macEntry.daRoute = 0;
        addrUpMsgPtr->macEntry.saMirrorToRxAnalyzerPortEn = GT_FALSE;
        addrUpMsgPtr->macEntry.daMirrorToRxAnalyzerPortEn = GT_FALSE;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
            LION3_FDB_AU_MSG_TABLE_FIELDS_CHAIN_TOO_LONG_E,
            hwValue);
        addrUpMsgPtr->naChainIsTooLong = BIT2BOOL_MAC(hwValue);
    }
    else
    {
        addrUpMsgPtr->macEntry.sourceID = (GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 2, 5);

        addrUpMsgPtr->naChainIsTooLong = (GT_BOOL)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 31, 1);
    }

    addrUpMsgPtr->queryHwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);

    if((addrUpMsgPtr->updType == CPSS_NA_E) ||
       (addrUpMsgPtr->updType == CPSS_QR_E) ||
       (addrUpMsgPtr->updType == CPSS_QA_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_OFFSET_E,
                hwValue);
            addrUpMsgPtr->entryOffset = hwValue;
        }
        else
        {
            addrUpMsgPtr->entryOffset = U32_GET_FIELD_MAC(auDescExt.elem0.word0, 7, 5);
        }
    }

    if((addrUpMsgPtr->updType == CPSS_AA_E) ||
        (addrUpMsgPtr->updType == CPSS_TA_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_8_0_E,
                hwValue);
            addrUpMsgPtr->macEntryIndex = hwValue;

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_20_9_E,
                hwValue);
            addrUpMsgPtr->macEntryIndex |= (hwValue << 9);
        }
        else
        {
            addrUpMsgPtr->macEntryIndex = U32_GET_FIELD_MAC(auDescExt.elem0.word0, 7, 8);
            if(macEntryExtKeyPtr->entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
            {
                addrUpMsgPtr->macEntryIndex |=
                    ((U32_GET_FIELD_MAC(auDescExt.elem0.word3, 27, 4) << 8) |
                     (U32_GET_FIELD_MAC(auDescExt.elem0.word3, 0, 2) << 12));
            }
        }
    }

    if (naFormatVersion == 1)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_UP0_E,
                hwValue);
            addrUpMsgPtr->up0 = hwValue;

            LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_IS_MOVED_E,
                hwValue);
            addrUpMsgPtr->isMoved = BIT2BOOL_MAC(hwValue);

            if (addrUpMsgPtr->isMoved == GT_TRUE)
            {
                LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_SRC_ID_E,
                    hwValue);
                addrUpMsgPtr->oldSrcId = hwValue;

                LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_DEVICE_E,
                    hwValue);
                addrUpMsgPtr->oldAssociatedHwDevNum = hwValue;

                /* old was trunk ? */
                LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                    LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_IS_TRUNK_E,
                    hwValue);
                isTrunk = hwValue;
                if(isTrunk)
                {
                    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                        LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_TRUNK_NUM_E,
                        hwValue);
                    /* the old interface is associated with a trunk */
                    addrUpMsgPtr->oldDstInterface.trunkId = (GT_TRUNK_ID)hwValue;
                    addrUpMsgPtr->oldDstInterface.type = CPSS_INTERFACE_TRUNK_E;

                    /* set associated hwDevNum with the ownDevNum of the hemisphere */
                    if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(addrUpMsgPtr->oldAssociatedHwDevNum))
                    {
                        /* set associated hwDevNum without the hemisphere indication */
                        addrUpMsgPtr->oldAssociatedHwDevNum &= MASK_DUAL_DEVICE_ID_LSB_CNS;
                    }
                }
                else
                {
                    LION3_FDB_AU_MSG_FIELD_GET_MAC(devNum,auMsgArrayOfWordsPtr,
                        LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_EPORT_E,
                        hwValue);
                    /* the old interface is associated with a port */
                    addrUpMsgPtr->oldDstInterface.devPort.portNum = hwValue;
                    addrUpMsgPtr->oldDstInterface.devPort.hwDevNum = addrUpMsgPtr->oldAssociatedHwDevNum;
                    PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(addrUpMsgPtr->oldDstInterface));
                    addrUpMsgPtr->oldDstInterface.type = CPSS_INTERFACE_PORT_E;
                }

            }

        }
        else
        {
            /* vid1 */
            addrUpMsgPtr->vid1 =
                (GT_U16)(U32_GET_FIELD_MAC(auDescExt.elem0.word0, 1, 2) |            /* bits 1..2 */
                         (U32_GET_FIELD_MAC(auDescExt.elem0.word0, 12, 3) << 2) |    /* bits 12..14 */
                         (U32_GET_FIELD_MAC(auDescExt.elem0.word3, 27, 4) << 5) |    /* bits 123..126 */
                         (U32_GET_FIELD_MAC(auDescExt.elem0.word3, 0, 2) << 9) |     /* bits 96..97 */
                         (U32_GET_FIELD_MAC(auDescExt.elem0.word0, 15, 1) << 11));   /* bit 15 */

            /* up0 */
            addrUpMsgPtr->up0 = U32_GET_FIELD_MAC(auDescExt.elem0.word3, 12, 3);

            /* check if the station was moved */
            addrUpMsgPtr->isMoved = (GT_BOOL)U32_GET_FIELD_MAC(auDescExt.elem0.word2, 16, 1);

            if (addrUpMsgPtr->isMoved == GT_TRUE)
            {
                addrUpMsgPtr->oldSrcId =
                    (U32_GET_FIELD_MAC(auDescExt.elem0.word3, 31, 1) |           /* bit 127 */
                     (U32_GET_FIELD_MAC(auDescExt.elem0.word3, 21, 3) << 1) |    /* bits 117..119 */
                     (U32_GET_FIELD_MAC(auDescExt.elem0.word2, 30, 1) << 4));    /* bit 94   */

                addrUpMsgPtr->oldAssociatedHwDevNum =
                    (GT_U8)(U32_GET_FIELD_MAC(auDescExt.elem0.word3, 19, 2) |           /* bit 115..116 */
                            (U32_GET_FIELD_MAC(auDescExt.elem0.word3, 15, 3) << 2));    /* bit 111..113 */

                oldPortTrunk = (U32_GET_FIELD_MAC(auDescExt.elem0.word0, 3, 1) |         /* bit 3 */
                                (U32_GET_FIELD_MAC(auDescExt.elem0.word2, 15, 1) << 1) | /* bit 79 */
                                (U32_GET_FIELD_MAC(auDescExt.elem0.word2, 13, 1) << 2) | /* bit 77 */
                                (U32_GET_FIELD_MAC(auDescExt.elem0.word2, 12, 1) << 3) | /* bit 76 */
                                (U32_GET_FIELD_MAC(auDescExt.elem0.word3, 24, 3) << 4)); /* bits 120..122 */

                /* check if old interface type is port or trunk */
                if((GT_U8)U32_GET_FIELD_MAC(auDescExt.elem0.word3, 18, 1) == 1)
                {
                    /* the old interface is associated with a trunk */
                    addrUpMsgPtr->oldDstInterface.trunkId = (GT_TRUNK_ID)oldPortTrunk;
                    addrUpMsgPtr->oldDstInterface.type = CPSS_INTERFACE_TRUNK_E;

                    /* set associated hwDevNum with the ownDevNum of the hemisphere */
                    if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(addrUpMsgPtr->oldAssociatedHwDevNum))
                    {
                        /* set associated hwDevNum without the hemisphere indication */
                        addrUpMsgPtr->oldAssociatedHwDevNum &= MASK_DUAL_DEVICE_ID_LSB_CNS;
                    }
                }
                else
                {
                    /* the old interface is associated with a port */
                    addrUpMsgPtr->oldDstInterface.devPort.portNum =
                        (GT_U8)oldPortTrunk;
                    addrUpMsgPtr->oldDstInterface.devPort.hwDevNum =
                        (GT_U8)addrUpMsgPtr->oldAssociatedHwDevNum;
                    PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(addrUpMsgPtr->oldDstInterface));
                    addrUpMsgPtr->oldDstInterface.type = CPSS_INTERFACE_PORT_E;
                }
            }
        }

        /* set old associated hwDevNum with the ownDevNum of the hemisphere */
        if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(addrUpMsgPtr->oldAssociatedHwDevNum) &&
           (addrUpMsgPtr->macEntry.key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E
            || addrUpMsgPtr->macEntry.key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E))
        {
            /* set associated hwDevNum without the hemisphere indication */
            addrUpMsgPtr->oldAssociatedHwDevNum &= MASK_DUAL_DEVICE_ID_LSB_CNS;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* auMsgFifoBlockDevGet
*
* DESCRIPTION:
*       Device specific get function for AU messages from FIFO -
*       from specific port group - for multi-port-groups support
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       portGroupId       - the portGroupId - for multi-port-groups support
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL     - on failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
static GT_STATUS auMsgFifoBlockGet
(
    IN     GT_U8                       devNum,
    IN     GT_U32                      portGroupId,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr

)
{
    GT_U32                  auCnt;          /* current AU count             */
    GT_U32                  auMaxCnt;       /* size of AU arr from user     */
    GT_U32                  auQBaseAddr;    /* register address             */
    PRV_CPSS_AU_DESC_EXT_8_STC auDescExt;         /* AU descriptor                */
    PRV_CPSS_AU_DESC_STC    *auDescPtr;         /* AU descriptor                */
    GT_U32                  *auWordPtr;     /* pointer to AU table          */
    GT_STATUS               retVal;         /* return value                 */
    GT_U32                  wordNum;        /* number of read words         */
    GT_U32      numWordsInAuMsg;        /* number of words in the message */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        numWordsInAuMsg = 6;
        auQBaseAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.FDBAddrUpdateMsgs.AUMsgToCPU.AUMsgToCPU;
    }
    else
    {
        numWordsInAuMsg = 4;
        auQBaseAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;
    }

    auDescPtr = &auDescExt.elem0;

    auMaxCnt = *numOfAuPtr;

    *numOfAuPtr = 0;
    for (auCnt = 0; auCnt < auMaxCnt; auCnt++)
    {
        auWordPtr = &auDescExt.elem0.word0;


        /* get the AU message from HW */
        for (wordNum = 0; wordNum < numWordsInAuMsg;
               wordNum++, auWordPtr++)
        {
            retVal = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,auQBaseAddr,auWordPtr);
            if (GT_OK != retVal)
            {
                return retVal;
            }

            if (0xFFFFFFFF == *auWordPtr)
            {
                if (0 == wordNum)
                {
                    /* no more addresses ! */
                    return /* it's not error for log */ GT_NO_MORE;
                }
            }
        }

        /* get the Address Update message from AU descriptor */
        retVal = auDesc2UpdMsg(devNum,portGroupId, &auDescExt, GT_FALSE, &auMessagesPtr[(*numOfAuPtr)]);
        if(retVal != GT_OK)
            return retVal;

        /* check that the function auDesc2UpdMsg(...) does not reset the AU
          descriptor. An application should not get the message if
          auDesc2UpdMsg resets the descriptor. */
        if(AU_DESC_IS_NOT_VALID(auDescPtr))
        {
                auCnt--;/* try to find other message to put for application */
        }
        else
        {
            /* increment the actual number of AU messages */
            (*numOfAuPtr)++;

        }
    }

    return GT_OK;

}


/*******************************************************************************
* prvDxChBrgFdbAuMsgFifoBlockDevGet
*
* DESCRIPTION:
*       Device specific get function for AU messages from FIFO
*
*       for multi-port groups device :
*           AU messages -
*           1. Unified FDB mode:
*              portGroup indication is required , for:
*                 a. Aging calculations of Trunks
*                 b. Aging calculations of regular entries when DA refresh is enabled.
*              In these 2 cases entries are considered as aged-out only if AA is
*              receives from all 4 portGroups.
*           2. In Unified-Linked FDB mode and Linked FDB 128K mode:
*              portGroup indication is required for these reasons:
*                 a. Aging calculations of Trunk and regular entries which have
*                    been written to multiple portGroups.
*                 b. New addresses (NA) source portGroup indication is required
*                    so application can add new entry to the exact portGroup
*                    which saw this station (especially on trunk entries).
*                 c. Indication on which portGroup has removed this address when
*                    AA of delete is fetched (relevant when same entry was written
*                    to multiple portGroups).
*                 d. Indication on which portGroup currently has this address
*                    when QR (query reply) is being fetched (relevant when same
*                    entry was written to multiple portGroups).
*
*           the function fetch AU messages from the queues in a round robin manner
*           (in steps of one entry).the function store which queue was last checked,
*           such that next message should be fetched from the next queue that
*           hold messages until the number of requested messages could be
*           supplied to application.
*           As one can understand, algorithm described above might create
*           undesired functional behaviors for the application:
*           1. Out-of-order messages -
*               since the fetching of messages from the 4 portGroups, is not
*               synchronized with messages' arrival order, possibly, application
*               would treat the updates not in the right order -
*               for example station X has been relocated to location A and then
*               location B - this will create 2 NA messages, NA with location A and
*               then NA with location B, if we'll treat these messages in wrong
*               order - we'll end up configuring station to location B and then A.
*               (specifically this example is not so bad cause even we do set station
*               with location A additional NA would be sent repeating location B
*               until rightly configured).
*               Note that there is no WA that can fix this undesired behavior.
*
*           FU messages -
*           since FU is triggered on all port groups , 'duplicated' messages may
*           appear on entries that reside on several/all port groups.
*
*           the function fetch FU messages from the queues in a round robin manner
*           (in steps of one entry).the function store which queue was last checked,
*           such that next message should be fetched from the next queue that
*           hold messages until the number of requested messages could be
*           supplied to application.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL     - on failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
static GT_STATUS prvDxChBrgFdbAuMsgFifoBlockDevGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr

)
{
    GT_STATUS               rc = GT_OK;
    GT_U32                  auCnt;          /* current AU count             */
    GT_U32                  auMaxCnt;       /* size of AU arr from user     */
    GT_U32                  portGroupsBmp;/* port groups to query - support multi-port-groups device */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  perPortGroupNumOfAu = 1;/* number of AU to serve from each port group , before query other port groups */
    GT_U32  *lastPortGroupServedPtr;/* pointer to the last port group served */
    GT_BOOL singleFdbInstance ;/*indication that although the device is 'multi port groups'
                                  the 'FDB unit' is 'single instance' and NOT per 'port group'
                                  the Bobcat2; Caelum; Bobcat3 is such device(multi port groups but single FDB instance).
                                  all 'non multi port groups'(ch1,2,3,xcat,xcat2) also considered 'single instance'
                                  Lion,Lion2 - considered FDB 'per port group'*/

    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* single instance AUQ */
        singleFdbInstance = GT_TRUE;
    }
    else if(GT_TRUE == PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* single instance AUQ */
        singleFdbInstance = GT_TRUE;
    }
    else
    {
        singleFdbInstance = GT_FALSE;
    }

    if(singleFdbInstance == GT_TRUE)
    {
        /* non multi-port-groups device */
        return auMsgFifoBlockGet(devNum ,
                CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,
                numOfAuPtr ,
                auMessagesPtr);
    }

    lastPortGroupServedPtr = &PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.auqPortGroupIdLastServed;

    auMaxCnt = *numOfAuPtr;

    /* start with bmp of all active port groups */
    portGroupsBmp = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp;

    /* we start we the next port group , from the last treated */
    portGroupId =  *lastPortGroupServedPtr;

    for (auCnt = 0; (auCnt < auMaxCnt); auCnt += perPortGroupNumOfAu)
    {
        do
        {
            portGroupId++;
            if(portGroupId > PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup)
            {
                portGroupId = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
            }
        }
        while(0 == (portGroupsBmp & (1 << portGroupId)) );/* look for port group to query */

        perPortGroupNumOfAu = prvCpssDxChPerPortGroupNumOfAu;

        rc = auMsgFifoBlockGet(devNum ,portGroupId, &perPortGroupNumOfAu , &auMessagesPtr[auCnt]);
        if(rc == GT_OK)
        {
            /* this port group returned the messages */
            /* continue to other port groups         */
        }
        else if(rc == GT_NO_MORE)
        {
            /* this port group has no more messages */
            /* continue to other port groups , but remove this port group from the bmp to query */
            portGroupsBmp &= ~(1 << portGroupId);

            if(portGroupsBmp == 0)
            {
                /* update the number of messages returned */
                auCnt += perPortGroupNumOfAu;
                /* all port groups finished */
                break;
            }
        }
        else
        {
            /* error */
            return rc;
        }

    }

    *numOfAuPtr = auCnt;

    /* save the info about the last port group served */
    *lastPortGroupServedPtr = portGroupId;

    return rc;
}


/*******************************************************************************
* auFuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of AU / FU messages , the max number
*       of elements defined by the caller
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number from which AU / FU are taken
*       portGroupId       - the portGroupId - for multi-port-groups support
*       queueType       - type of message queue to retrieve entries from
*       numOfAuFuPtr - (pointer to)actual number of AU / FU messages that
*                          were received
*
* OUTPUTS:
*       numOfAuFuPtr - (pointer to)actual number of AU / FU messages that
*                          were received
*       auFuMessagesPtr - array that holds received AU / FUmessages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK           - on success
*       GT_NO_MORE      - the action succeeded and there are no more waiting
*                         AU messages
*
*       GT_FAIL         - on failure
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - not all results of the CNC block upload
*                         retrieved from the common used FU and CNC
*                         upload queue
*
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
static GT_STATUS auFuMsgBlockGet
(
    IN     GT_U8                       devNum,
    IN     GT_U32                      portGroupId,
    IN     MESSAGE_QUEUE_ENT           queueType,
    INOUT  GT_U32                      *numOfAuFuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auFuMessagesPtr
)
{
    PRV_CPSS_AU_DESC_STC        *descPtr;   /*pointer to the current descriptor*/
    PRV_CPSS_AU_DESC_EXT_8_STC  *descExtPtr;   /*pointer to the current descriptor*/
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;  /* pointer to the descriptors DB
                                                 of the device */
    GT_U32                     *auMemPtr;/* pointer to the SDMA memory , to start of current message */
    GT_U32                      ii;            /* iterator */
    GT_U32                      auMaxCnt;      /* number of AU / FU messages */
    GT_STATUS                   rc;            /* return code                */
    GT_U32                      numOfWaAu;/* get messages one by one from the secondary AUQ*/
    GT_U32                      regValue;
    GT_U32                      auMessageNumBytes;
    GT_BOOL                     useDoubleAuq;  /* support configuration of two AUQ memory regions */
    GT_BOOL                     enableRewind; /* enable rewind of AUQ when last message is handled */
    GT_BOOL                     isFuQueue;    /* GT_TRUE - FU Queue, GT_FALSE - AU Queue */

    auMessageNumBytes = 4 * PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.auMessageNumOfWords;
    useDoubleAuq = PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useDoubleAuq;

     switch(queueType)
    {
        case MESSAGE_QUEUE_PRIMARY_FUQ_E:
            /* check that the FUQ initialized */
            PRV_CPSS_FUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

            /* use separate FU queue if enabled, otherwise use common AU queue */
            descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
            isFuQueue = GT_TRUE;

            /* not all upload CNC counter messages retrieved from the common */
            /* used queue                                                   */
            if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum)
                && (descCtrlPtr->unreadCncCounters != 0))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            break;

        case MESSAGE_QUEUE_SECONDARY_AUQ_E:
            descCtrlPtr = (PRV_CPSS_SECONDARY_AUQ_INDEX_MAC(devNum, portGroupId) == 0) ?
                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]) :
                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId]);
            isFuQueue = GT_FALSE;
            break;
        case MESSAGE_QUEUE_PRIMARY_AUQ_E:
            /* check that the AUQ initialized */
            PRV_CPSS_AUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

            descCtrlPtr = (PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId) == 0) ?
                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]) :
                &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);
            isFuQueue = GT_FALSE;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);/*should not happen*/
    }

    /* enable rewind by default */
    enableRewind = GT_TRUE;

    /* when the primary AUQ is called , and it's SW index is 0 , we need to
       check if there a reason to query the secondary AUQ , because those
       messages comes chronologically after the 'previous' primary auq messages
       and before the current messages in the primary AUQ */
    if((descCtrlPtr->currDescIdx == 0) &&
       (queueType == MESSAGE_QUEUE_PRIMARY_AUQ_E) &&
       (PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue == GT_TRUE) &&
       (PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState == PRV_CPSS_AUQ_STATE_ALL_EMPTY_E))
    {
        numOfWaAu = *numOfAuFuPtr;
        rc = auFuMsgBlockGet(devNum,portGroupId,MESSAGE_QUEUE_SECONDARY_AUQ_E,
                                  &numOfWaAu,auFuMessagesPtr);

        if(rc != GT_NO_MORE)
        {
            /* if we got the GT_OK it means that the buffer was filled with
            messages from the secondary queue , otherwise there was error and we
            exit the function anyway */
            *numOfAuFuPtr = numOfWaAu;
            return rc;
        }

        /* NOTE: meaning we got GT_NO_MORE , so we need to continue and getting
                messages from the primary AUQ */

        /* set ii to continue handling messages from the primary queue */
        ii = numOfWaAu;
    }
    else
    {
        /* initialize ii , to indicate that no messages already exist in the buffer */
        ii = 0;
        /* initialize rc - because function return rc */
        rc = GT_OK;
    }


    /* the pointer to start of 'next message to handle'  */
    auMemPtr = (GT_U32 *)(descCtrlPtr->blockAddr + (auMessageNumBytes * descCtrlPtr->currDescIdx));

    auMaxCnt = *numOfAuFuPtr;

    *numOfAuFuPtr = ii;

    /* read number of AU descriptors */
    for (/*ii was already initialized*/; ii < auMaxCnt; ii++)
    {
        /* point descPtr to the current AU descriptor in the queue */
        descExtPtr= (PRV_CPSS_AU_DESC_EXT_8_STC*)auMemPtr;
        descPtr = (PRV_CPSS_AU_DESC_STC*)auMemPtr;

        if(AU_DESC_IS_NOT_VALID(descPtr))
        {
            /* no more AU descriptors */
            return /* it's not error for log */ GT_NO_MORE;
        }

        rc = auDesc2UpdMsg(devNum, portGroupId ,descExtPtr , GT_TRUE, &auFuMessagesPtr[ii]);
        if(rc != GT_OK)
            return rc;

        if(AU_DESC_IS_NOT_VALID(descPtr))
        {
            /* the function auDesc2UpdMsg(...) reset the info */
            /* meaning that Application should not get the message */

            ii--;/* try to find other message to put for application */
        }
        else
        {
            AU_DESC_RESET_MAC(descPtr);

            /* increment the actual number of AU messages */
            (*numOfAuFuPtr)++;
        }


        descCtrlPtr->currDescIdx = ((descCtrlPtr->currDescIdx + 1) %
                                   descCtrlPtr->blockSize);

        auMemPtr = (GT_U32 *)(descCtrlPtr->blockAddr + (auMessageNumBytes * descCtrlPtr->currDescIdx));

        /* reached the end of AU block */
        if(descCtrlPtr->currDescIdx == 0)
        {
            if(queueType == MESSAGE_QUEUE_PRIMARY_AUQ_E &&
               PRV_CPSS_PP_MAC(devNum)->intCtrl.
                   auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue == GT_TRUE)
            {
                /* Change state of primary AUQ */
                if(useDoubleAuq == GT_TRUE)
                {
                    switch(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState)
                    {
                        case PRV_CPSS_AUQ_STATE_ALL_FULL_E:
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState =
                                PRV_CPSS_AUQ_STATE_ONE_EMPTY_E;
                            break;
                        case PRV_CPSS_AUQ_STATE_ONE_EMPTY_E:
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState =
                                PRV_CPSS_AUQ_STATE_ALL_EMPTY_E;
                            break;
                        default:
                            /* Should not happen */
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                    }
                }
                else
                {
                    PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState =
                        PRV_CPSS_AUQ_STATE_ALL_EMPTY_E;
                }
            }

            if((PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue  == GT_TRUE) &&
               queueType == MESSAGE_QUEUE_PRIMARY_AUQ_E &&
               (PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState == PRV_CPSS_AUQ_STATE_ALL_EMPTY_E))
            {
                /* get messages from the secondary AUQ , that was set by the WA
                in function cpssDxChBrgFdbTriggerAuQueueWa(...) */

                /* do not rewind primary AUQ till it's secondary is not full
                   and all messages are not retrieved from it. */
                if(useDoubleAuq == GT_TRUE)
                {
                    enableRewind = GT_FALSE;
                }

                /* increment ii because the message in [ii] was already placed */
                /* and we not continuing the above loop on ii */
                ii++;

                for ( /*continue ii */  ; ii < auMaxCnt; ii++)
                {
                    /* get the messages one-by-one */
                    numOfWaAu = 1;
                    rc = auFuMsgBlockGet(devNum,portGroupId,MESSAGE_QUEUE_SECONDARY_AUQ_E,
                                    &numOfWaAu,&auFuMessagesPtr[ii]);
                    if(rc == GT_OK && numOfWaAu == 1)
                    {
                        /* increment the actual number of AU messages */
                        (*numOfAuFuPtr)++;
                    }
                    else
                    {
                        /* no more messages to pull from the secondary pool */
                        break;
                    }
                }
            }

            /* when this function called to 'get messages from the secondary AUQ
               we must not write to the PP it's base address again because
               we need to restore the primary base address , but the primary
               base address will be set on it's own code flow */
            if(queueType == MESSAGE_QUEUE_SECONDARY_AUQ_E)
            {
                if(useDoubleAuq == GT_TRUE)
                {
                    switch(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState)
                    {
                        case PRV_CPSS_AUQ_STATE_ONE_EMPTY_E:
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState = PRV_CPSS_AUQ_STATE_ALL_EMPTY_E;
                            /* state that we finished reading the secondary AUQ till the end
                               of it , so no need to query it again */
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.
                                auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue = GT_FALSE;
                            /* Reset state of primary AUQ when last secondary AUQ has been read */
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState = PRV_CPSS_AUQ_STATE_ALL_FULL_E;
                            break;
                        case PRV_CPSS_AUQ_STATE_ALL_FULL_E:
                            /* WA triggered twice and current secondary AUQ is empty. Make another secondary AUQ to be active. */
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activeSecondaryAuqIndex ^= 1;
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].secondaryState = PRV_CPSS_AUQ_STATE_ONE_EMPTY_E;
                            /* Change state of primary AUQ to one-empty */
                            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState = PRV_CPSS_AUQ_STATE_ONE_EMPTY_E;
                            break;
                        default:
                            /* Should not happen */
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                    }

                    /* The secondary queue is empty.
                       Update primary AUQ size and base address and set rewind flag in the WA DB. */
                    enableRewind = GT_TRUE;

                    descCtrlPtr = (PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId) == 0) ?
                        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]) :
                        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);

                    queueType = MESSAGE_QUEUE_PRIMARY_AUQ_E;

                    /* now we will set the 'primary' AUQ base address*/
                }
                else
                {
                    /* state that we finished reading the secondary AUQ till the end
                       of it , so no need to query it again */
                    PRV_CPSS_PP_MAC(devNum)->intCtrl.
                        auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue = GT_FALSE;
                    /* Reset state of primary AUQ when last secondary AUQ has been read */
                    PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].primaryState = PRV_CPSS_AUQ_STATE_ALL_FULL_E;

                    /* DO NOT write the secondary base address to the HW ,
                        only the function cpssDxChBrgFdbTriggerAuQueueWa do that
                        and the application must call it , ONLY when a deadlock is
                        detected by the application
                    */

                    /* this will exit the function with GT_OK */
                    break;
                }
            }

            /* change AUQ size for primary AUQ.
               Do not change it for FUQ because it constant one.
               Secondary queue flow cannot get to this point. */
            if(queueType == MESSAGE_QUEUE_PRIMARY_AUQ_E && enableRewind == GT_TRUE)
            {
                /* get a the value of the AUQ size */
                rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId,
                            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl,
                            0, 29,
                            &regValue);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if(regValue != descCtrlPtr->blockSize)
                {
                    /* set the 'primary' AUQ size */
                    rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,
                                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl,
                                0, 29,
                                descCtrlPtr->blockSize);/* 'primary' AUQ */
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
                /* now we will set the 'primary' AUQ base address*/
            }

            if (enableRewind == GT_TRUE)
            {
                rc = prvCpssDxChFullQueueRewind(
                    devNum, portGroupId, isFuQueue, descCtrlPtr);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            if(queueType == MESSAGE_QUEUE_PRIMARY_AUQ_E && enableRewind == GT_TRUE)
            {
                /* indicate the primary AUQ was rewind */
                PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind = GT_TRUE;

                if(useDoubleAuq == GT_TRUE)
                {
                    /* If current primary AUQ reached the end - change primary AUQ */
                    PRV_CPSS_PP_MAC(devNum)->intCtrl.activeAuqIndex[portGroupId] ^= 1;

                    debugPrint((" auFuMsgBlockGet : AUQ%d is active\n",
                                PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId)));
                }
            }

            /* this will exit the function with GT_OK */
            break; /*  for (ii was already initialized; ii < auMaxCnt; ii++) */
        }
    }

    return rc;
}

/*******************************************************************************
* prvDxChBrgFdbAuFuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of AU / FU messages , the max number
*       of elements defined by the caller
*
*       for multi-port groups device :
*           AU messages -
*           1. Unified FDB mode:
*              portGroup indication is required , for:
*                 a. Aging calculations of Trunks
*                 b. Aging calculations of regular entries when DA refresh is enabled.
*              In these 2 cases entries are considered as aged-out only if AA is
*              receives from all 4 portGroups.
*           2. In Unified-Linked FDB mode and Linked FDB 128K mode:
*              portGroup indication is required for these reasons:
*                 a. Aging calculations of Trunk and regular entries which have
*                    been written to multiple portGroups.
*                 b. New addresses (NA) source portGroup indication is required
*                    so application can add new entry to the exact portGroup
*                    which saw this station (especially on trunk entries).
*                 c. Indication on which portGroup has removed this address when
*                    AA of delete is fetched (relevant when same entry was written
*                    to multiple portGroups).
*                 d. Indication on which portGroup currently has this address
*                    when QR (query reply) is being fetched (relevant when same
*                    entry was written to multiple portGroups).
*
*           the function fetch AU messages from the queues in a round robin manner
*           (in steps of one entry).the function store which queue was last checked,
*           such that next message should be fetched from the next queue that
*           hold messages until the number of requested messages could be
*           supplied to application.
*           As one can understand, algorithm described above might create
*           undesired functional behaviors for the application:
*           1. Out-of-order messages -
*               since the fetching of messages from the 4 portGroups, is not
*               synchronized with messages' arrival order, possibly, application
*               would treat the updates not in the right order -
*               for example station X has been relocated to location A and then
*               location B - this will create 2 NA messages, NA with location A and
*               then NA with location B, if we'll treat these messages in wrong
*               order - we'll end up configuring station to location B and then A.
*               (specifically this example is not so bad cause even we do set station
*               with location A additional NA would be sent repeating location B
*               until rightly configured).
*               Note that there is no WA that can fix this undesired behavior.
*
*           FU messages -
*           since FU is triggered on all port groups , 'duplicated' messages may
*           appear on entries that reside on several/all port groups.
*
*           the function fetch FU messages from the queues in a round robin manner
*           (in steps of one entry).the function store which queue was last checked,
*           such that next message should be fetched from the next queue that
*           hold messages until the number of requested messages could be
*           supplied to application.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - the device number from which AU / FU are taken
*       queueType     - AUQ or FUQ. FUQ valid for DxCh2 and above.
*       numOfAuFuPtr  - (pointer to)actual number of AU / FU messages that
*                          were received
*
* OUTPUTS:
*       numOfAuFuPtr - (pointer to)actual number of AU / FU messages that
*                          were received
*       auFuMessagesPtr - array that holds received AU / FUmessages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK           - on success
*       GT_NO_MORE      - the action succeeded and there are no more waiting
*                         AU messages
*
*       GT_FAIL         - on failure
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - not all results of the CNC block upload
*                         retrieved from the common used FU and CNC
*                         upload queue
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvDxChBrgFdbAuFuMsgBlockGet
(
    IN     GT_U8                       devNum,
    IN     MESSAGE_QUEUE_ENT           queueType,
    INOUT  GT_U32                      *numOfAuFuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auFuMessagesPtr
)
{
    GT_STATUS               rc = GT_OK;
    GT_U32                  auCnt;          /* current AU count             */
    GT_U32                  auMaxCnt;       /* size of AU arr from user     */
    GT_U32                  portGroupsBmp;/* port groups to query - support multi-port-groups device */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  perPortGroupNumOfAu = prvCpssDxChPerPortGroupNumOfAu;/* number of AU to serve from each port group , before query other port groups */
    GT_U32  *lastPortGroupServedPtr;/* pointer to the last port group served */
    GT_BOOL singleFdbInstance ;/*indication that although the device is 'multi port groups'
                                  the 'FDB unit' is 'single instance' and NOT per 'port group'
                                  the Bobcat2; Caelum; Bobcat3 is such device(multi port groups but single FDB instance).
                                  all 'non multi port groups'(ch1,2,3,xcat,xcat2) also considered 'single instance'
                                  Lion,Lion2 - considered FDB 'per port group'*/

     if ((systemRecoveryInfo.systemRecoveryState != CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E) &&
         (systemRecoveryInfo.systemRecoveryMode.continuousAuMessages == GT_FALSE))
    {
        *numOfAuFuPtr = 0;/* no messages to retrieve */
        return /* it's not error for log */ GT_NO_MORE;
    }


    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* single instance AUQ and FUQ */
        singleFdbInstance = GT_TRUE;
    }
    else if(GT_TRUE == PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
    {
        /* single instance AUQ (primary/secondary)
            the FUQ is per core !!! BUT !!! the FDB upload triggered on single FDB instance
            so no need to loop on all cores.
         */
        singleFdbInstance = GT_TRUE;
    }
    else
    {
        singleFdbInstance = GT_FALSE;
    }

    if(singleFdbInstance == GT_TRUE)
    {
        /* single instance AUQ (primary/secondary) or single instance FUQ */
        return auFuMsgBlockGet(devNum ,
                CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,
                queueType,
                numOfAuFuPtr ,
                auFuMessagesPtr);
    }

    if(queueType != MESSAGE_QUEUE_PRIMARY_FUQ_E)
    {
        lastPortGroupServedPtr = &PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.auqPortGroupIdLastServed;
    }
    else
    {
        lastPortGroupServedPtr = &PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.fuqPortGroupIdLastServed;
    }


    auMaxCnt = *numOfAuFuPtr;

    /* start with bmp of all active port groups */
    portGroupsBmp = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp;

    /* we start we the next port group , from the last treated */
    portGroupId =  *lastPortGroupServedPtr;

    for (auCnt = 0; (auCnt < auMaxCnt); auCnt += perPortGroupNumOfAu)
    {
        do
        {
            portGroupId++;
            if(portGroupId > PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup)
            {
                portGroupId = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
            }
        }
        while(0 == (portGroupsBmp & (1 << portGroupId)) );/* look for port group to query */

        perPortGroupNumOfAu = prvCpssDxChPerPortGroupNumOfAu;

        rc = auFuMsgBlockGet(devNum ,portGroupId, queueType, &perPortGroupNumOfAu , &auFuMessagesPtr[auCnt]);
        if(rc == GT_OK)
        {
            /* this port group returned the messages */
            /* continue to other port groups */
        }
        else if(rc == GT_NO_MORE)
        {
            /* this port group has no more messages */
            /* continue to other port groups , but remove this port group from the bmp to query */
            portGroupsBmp &= ~(1 << portGroupId);

            if(portGroupsBmp == 0)
            {
                /* update the number of messages returned */
                auCnt += perPortGroupNumOfAu;
                /* all port groups finished */
                break;
            }
        }
        else
        {
            /* error */
            return rc;
        }

    }

    *numOfAuFuPtr = auCnt;

    /* save the info about the last port group served */
    *lastPortGroupServedPtr = portGroupId;

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbFuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place FU messages in common FDB Address Update (AU) messages
*       queue or in separate queue only for FU messages. The function
*       cpssDxChHwPpPhase2Init configures queue for FU messages by the
*       fuqUseSeparate parameter. If common AU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate FU queue is used then function returns only FU messages.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - the device number from which FU are taken
*       numOfFuPtr - (pointer to)max number of FU messages to get
*
* OUTPUTS:
*       numOfFuPtr - (pointer to)actual number of FU messages that were received
*       fuMessagesPtr - array that holds received FU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfFuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - not all results of the CNC block upload
*                          retrieved from the common used FU and CNC
*                          upload queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           since FU is triggered on all port groups , 'duplicated' messages may
*           appear on entries that reside on several/all port groups.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbFuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfFuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *fuMessagesPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    CPSS_NULL_PTR_CHECK_MAC(numOfFuPtr);
    CPSS_NULL_PTR_CHECK_MAC(fuMessagesPtr);

    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
        (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                        enabled == GT_FALSE))
    {
       if(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_TRUE)
       {
            return prvDxChBrgFdbAuFuMsgBlockGet(devNum,MESSAGE_QUEUE_PRIMARY_FUQ_E,numOfFuPtr,fuMessagesPtr);
       }
       else
       {
            return prvDxChBrgFdbAuFuMsgBlockGet(devNum,MESSAGE_QUEUE_PRIMARY_AUQ_E,numOfFuPtr,fuMessagesPtr);
       }
    }
    else
    {
        return
          prvDxChBrgFdbAuMsgFifoBlockDevGet(devNum,numOfFuPtr,fuMessagesPtr);
    }

}

/*******************************************************************************
* cpssDxChBrgFdbFuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place FU messages in common FDB Address Update (AU) messages
*       queue or in separate queue only for FU messages. The function
*       cpssDxChHwPpPhase2Init configures queue for FU messages by the
*       fuqUseSeparate parameter. If common AU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate FU queue is used then function returns only FU messages.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - the device number from which FU are taken
*       numOfFuPtr - (pointer to)max number of FU messages to get
*
* OUTPUTS:
*       numOfFuPtr - (pointer to)actual number of FU messages that were received
*       fuMessagesPtr - array that holds received FU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfFuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - not all results of the CNC block upload
*                          retrieved from the common used FU and CNC
*                          upload queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           since FU is triggered on all port groups , 'duplicated' messages may
*           appear on entries that reside on several/all port groups.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbFuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfFuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *fuMessagesPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbFuMsgBlockGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, numOfFuPtr, fuMessagesPtr));

    rc = internal_cpssDxChBrgFdbFuMsgBlockGet(devNum, numOfFuPtr, fuMessagesPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, numOfFuPtr, fuMessagesPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           1. Unified FDB mode:
*              portGroup indication is required , for:
*                 a. Aging calculations of Trunks
*                 b. Aging calculations of regular entries when DA refresh is enabled.
*              In these 2 cases entries are considered as aged-out only if AA is
*              receives from all 4 portGroups.
*           2. In Unified-Linked FDB mode and Linked FDB mode:
*              portGroup indication is required for these reasons:
*                 a. Aging calculations of Trunk and regular entries which have
*                    been written to multiple portGroups.
*                 b. New addresses (NA) source portGroup indication is required
*                    so application can add new entry to the exact portGroup
*                    which saw this station (especially on trunk entries).
*                 c. Indication on which portGroup has removed this address when
*                    AA of delete is fetched (relevant when same entry was written
*                    to multiple portGroups).
*                 d. Indication on which portGroup currently has this address
*                    when QR (query reply) is being fetched (relevant when same
*                    entry was written to multiple portGroups).
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(numOfAuPtr);
    CPSS_NULL_PTR_CHECK_MAC(auMessagesPtr);

    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                   info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                       enabled == GT_FALSE))
    {
        return
          prvDxChBrgFdbAuFuMsgBlockGet(devNum,MESSAGE_QUEUE_PRIMARY_AUQ_E,numOfAuPtr,auMessagesPtr);
    }
    else
    {
        return
          prvDxChBrgFdbAuMsgFifoBlockDevGet(devNum,numOfAuPtr,auMessagesPtr);
    }

}

/*******************************************************************************
* cpssDxChBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           1. Unified FDB mode:
*              portGroup indication is required , for:
*                 a. Aging calculations of Trunks
*                 b. Aging calculations of regular entries when DA refresh is enabled.
*              In these 2 cases entries are considered as aged-out only if AA is
*              receives from all 4 portGroups.
*           2. In Unified-Linked FDB mode and Linked FDB mode:
*              portGroup indication is required for these reasons:
*                 a. Aging calculations of Trunk and regular entries which have
*                    been written to multiple portGroups.
*                 b. New addresses (NA) source portGroup indication is required
*                    so application can add new entry to the exact portGroup
*                    which saw this station (especially on trunk entries).
*                 c. Indication on which portGroup has removed this address when
*                    AA of delete is fetched (relevant when same entry was written
*                    to multiple portGroups).
*                 d. Indication on which portGroup currently has this address
*                    when QR (query reply) is being fetched (relevant when same
*                    entry was written to multiple portGroups).
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAuMsgBlockGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, numOfAuPtr, auMessagesPtr));

    rc = internal_cpssDxChBrgFdbAuMsgBlockGet(devNum, numOfAuPtr, auMessagesPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, numOfAuPtr, auMessagesPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/************************************************************************
* auqFuqMessagesNumberGet
*
* DESCRIPTION:
*       The function scan the AU/FU queues and returns the number of
*       AU/FU messages in the queue.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number on which AU are counted
*       portGroupId       - the portGroupId - for multi-port-groups support
*       queueType - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       numOfAuPtr - (pointer to) number of AU messages in the specified queue.
*       endOfQueueReachedPtr - (pointer to) GT_TRUE: The queue reached to the end.
*                                        GT_FALSE: else
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on not initialized queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
GT_STATUS auqFuqMessagesNumberGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  MESSAGE_QUEUE_ENT             queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_BOOL                      *endOfQueueReachedPtr
)
{
    PRV_CPSS_AU_DESC_STC        *descPtr;   /*pointer to the current descriptor*/
    PRV_CPSS_AU_DESC_EXT_8_STC  *descExtPtr;/*pointer to the current extended descriptor*/
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;  /* pointer to the descriptors DB
                                                 of the device */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrl1Ptr = 0; /* pointer to the descriptors DB
                                                for additional primary AUQ of the device */

    GT_U32                     *auMemPtr;/* pointer to the SDMA memory , to start of current message */
    GT_U32                      auCounter;
    GT_U32                      ii;            /* iterator */
    GT_U32                      auMessageNumBytes;
    GT_U32                      auMessageNumWords;
    GT_U32                      numberOfQueues = 1; /* number of queues */
    GT_U32                      currentQueue;   /* iterator */
    GT_BOOL                     useDoubleAuq;   /* support configuration of two AUQ memory regions */

    auMessageNumWords = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.auMessageNumOfWords;
    auMessageNumBytes = 4 * auMessageNumWords;

    useDoubleAuq = PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.useDoubleAuq;

    if(queueType == MESSAGE_QUEUE_PRIMARY_FUQ_E)
    {
        /* check that the FUQ initialized */
        PRV_CPSS_FUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

        /* use separate FU queue if enabled, otherwise use common AU queue */
        descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);

        /* not all upload CNC counter messages retrieved from the common */
        /* used queue                                                   */
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum)
            && (descCtrlPtr->unreadCncCounters != 0))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* check that the AUQ initialized */
        PRV_CPSS_AUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

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
    }

    *endOfQueueReachedPtr = GT_FALSE;
    auCounter = 0;
    ii = descCtrlPtr->currDescIdx;

    for(currentQueue = 0; currentQueue < numberOfQueues; currentQueue++)
    {
        /* Second iteration for additional AUQ */
        if(currentQueue == 1)
        {
             descCtrlPtr = descCtrl1Ptr;
             ii = 0;
        }

        /* the pointer to start of 'next message to handle'  */
        auMemPtr = (GT_U32 *)(descCtrlPtr->blockAddr + (auMessageNumBytes * descCtrlPtr->currDescIdx));

        /* read number of AU descriptors */
        for (; ii < descCtrlPtr->blockSize; ii++, auMemPtr+=auMessageNumWords)
        {
            /* point descPtr to the current AU descriptor in the queue */
            descExtPtr= (PRV_CPSS_AU_DESC_EXT_8_STC*)auMemPtr;
            descPtr = (PRV_CPSS_AU_DESC_STC*)auMemPtr;


            if(AU_DESC_IS_NOT_VALID(descPtr))
            {
                /* no more AU descriptors */
                break;
            }

            if(NON_SOURCE_PORT_GROUP_FILTER_CHECK_MAC(devNum))
            {
                PRV_CPSS_AU_DESC_EXT_8_STC    auDescExt;    /* Holds the address updated        */
                                                            /* descriptor after byte-swapping.  */
                AU_DESC_SWAP_MAC(&descExtPtr->elem0,auDescExt.elem0);

                if(auMessageNumWords == 8)
                {
                    AU_DESC_SWAP_MAC(&descExtPtr->elem1,auDescExt.elem1);
                }

                /* function auDescMultiPortGroupNonSourceFilterExtCheck , must be called with
                   'swapped' auDesc */
                if(GT_TRUE == auDescMultiPortGroupNonSourceFilterCheck(devNum,portGroupId,&auDescExt))
                {
                    /* 'filter' this AU/FU message */
                    continue;
                }
            }

            auCounter++;
        }
    }

    if( ii == descCtrlPtr->blockSize )
    {
        *endOfQueueReachedPtr = GT_TRUE;
    }

    *numOfAuPtr = auCounter;
    return GT_OK;
}

/************************************************************************
* auqFuqClearUnreadMessages
*
* DESCRIPTION:
*       The function scan the AU/FU queues, clear all unread
*       AU/FU messages in the queue and calculate number of messages
*       that remained till the end of the queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - the device number on which AU are counted
*       portGroupId           - the portGroupId - for multi-port-groups support
*       queueType             - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       numOfAuPtr            - (pointer to) number of AU messages processed in the specified queue.
*       numMsgTillQueueEndPtr - (pointer to) number of AU messages remained to fill the queue.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on not initialized queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
GT_STATUS auqFuqClearUnreadMessages
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  MESSAGE_QUEUE_ENT             queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_U32                       *numMsgTillQueueEndPtr
)
{
    PRV_CPSS_AU_DESC_STC        *descPtr;   /*pointer to the current descriptor*/
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;  /* pointer to the descriptors DB
                                                 of the device */
    GT_U32                      auCounter;
    PRV_CPSS_AU_DESC_STC        *descBlockPtr; /* AU descriptors block */
    GT_U32                      ii;            /* iterator */
    switch(queueType)
    {
    case MESSAGE_QUEUE_PRIMARY_AUQ_E:
        /* check that the AUQ initialized */
        PRV_CPSS_AUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);
        descCtrlPtr = (PRV_CPSS_AUQ_INDEX_MAC(devNum, portGroupId) == 0) ?
            &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]) :
            &(PRV_CPSS_PP_MAC(devNum)->intCtrl.au1DescCtrl[portGroupId]);

        break;
    case MESSAGE_QUEUE_PRIMARY_FUQ_E:
        /* check that the FUQ initialized */
        PRV_CPSS_FUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

        /* use separate FU queue if enabled, otherwise use common AU queue */
        descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);

        /* not all upload CNC counter messages retrieved from the common */
        /* used queue                                                   */
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum)
            && (descCtrlPtr->unreadCncCounters != 0))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        break;
    case MESSAGE_QUEUE_SECONDARY_AUQ_E:
        PRV_CPSS_SECONDARY_AUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);
        descCtrlPtr = (PRV_CPSS_SECONDARY_AUQ_INDEX_MAC(devNum, portGroupId) == 0) ?
            &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]) :
            &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAu1DescCtrl[portGroupId]);
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get address of AU descriptors block */
    descBlockPtr = (PRV_CPSS_AU_DESC_STC*)(descCtrlPtr->blockAddr);

    /* pointer to the current descriptor */
    descPtr = &(descBlockPtr[descCtrlPtr->currDescIdx]);

    /* read number of AU descriptors */
    for (ii = descCtrlPtr->currDescIdx, auCounter = 0; ii < descCtrlPtr->blockSize; ii++ ,descPtr++)
    {
        if(AU_DESC_IS_NOT_VALID(descPtr))
        {
            /* no more AU descriptors */
            break;
        }
        /* increment software descriptor pointer*/
         descCtrlPtr->currDescIdx =
            (( descCtrlPtr->currDescIdx + 1) %  descCtrlPtr->blockSize);

        AU_DESC_RESET_MAC(descPtr);
        auCounter++;
    }
    /* after this cycle software pointer will be the same as hardware pointer */
    if ((descCtrlPtr->currDescIdx == 0) && (auCounter > 0))
    {
        *numMsgTillQueueEndPtr = 0;
    }
    else
    {
        *numMsgTillQueueEndPtr = descCtrlPtr->blockSize - descCtrlPtr->currDescIdx;
    }
    *numOfAuPtr = auCounter;
    return GT_OK;
}

/************************************************************************
* internal_cpssDxChBrgFdbAuqFuqMessagesNumberGet
*
* DESCRIPTION:
*       The function scan the AU/FU queues and returns the number of
*       AU/FU messages in the queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number on which AU are counted
*       queueType - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       numOfAuPtr - (pointer to) number of AU messages in the specified queue.
*                    for multi-port groups device :
*                       number of AU messages returned is the summary from AUQs
*                       of all port groups.
*       endOfQueueReachedPtr - (pointer to)
*                       for NON multi-port groups device :
*                           GT_TRUE:
*                               The queue reached to the end.
*                           GT_FALSE:
*                               The queue NOT reached to the end.
*                       for multi-port groups device :
*                           GT_TRUE:
*                               At least one queue of the port groups has reached
*                               its end.(this would alert the application to
*                               start fetching the waiting messages, as at least
*                               one of the AUQs can't accept additional messages).
*                           GT_FALSE:
*                               No queue reached to the end.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on not initialized queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbAuqFuqMessagesNumberGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_BOOL                      *endOfQueueReachedPtr
)
{
    GT_STATUS               rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  coreValue;
    GT_BOOL portGroupEndOfQueueReached;
    MESSAGE_QUEUE_ENT   internalQueueType;/* internal enum for queue type */

    CPSS_NULL_PTR_CHECK_MAC(numOfAuPtr);
    CPSS_NULL_PTR_CHECK_MAC(endOfQueueReachedPtr);
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(queueType != CPSS_DXCH_FDB_QUEUE_TYPE_FU_E &&
       queueType != CPSS_DXCH_FDB_QUEUE_TYPE_AU_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *numOfAuPtr = 0;
    *endOfQueueReachedPtr = GT_FALSE;

    if(queueType == CPSS_DXCH_FDB_QUEUE_TYPE_AU_E ||
       PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_FALSE)
    {
        internalQueueType = MESSAGE_QUEUE_PRIMARY_AUQ_E;
    }
    else
    {
        internalQueueType = MESSAGE_QUEUE_PRIMARY_FUQ_E;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc =  auqFuqMessagesNumberGet(devNum ,portGroupId,
                internalQueueType,&coreValue,&portGroupEndOfQueueReached);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(portGroupEndOfQueueReached == GT_TRUE)
        {
            /* if any of the port groups , reached end of queue , we state the device
               reached end of queue*/
            *endOfQueueReachedPtr = GT_TRUE;
        }

        /* summary of all the port groups */
        *numOfAuPtr += coreValue;

        /* check if need to break loop on device that supports single FDB unit instance */
        PRV_CPSS_DXCH_FDB_SINGLE_INSTANCE_FDB_BREAK_MAC(devNum);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;

}

/************************************************************************
* cpssDxChBrgFdbAuqFuqMessagesNumberGet
*
* DESCRIPTION:
*       The function scan the AU/FU queues and returns the number of
*       AU/FU messages in the queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number on which AU are counted
*       queueType - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       numOfAuPtr - (pointer to) number of AU messages in the specified queue.
*                    for multi-port groups device :
*                       number of AU messages returned is the summary from AUQs
*                       of all port groups.
*       endOfQueueReachedPtr - (pointer to)
*                       for NON multi-port groups device :
*                           GT_TRUE:
*                               The queue reached to the end.
*                           GT_FALSE:
*                               The queue NOT reached to the end.
*                       for multi-port groups device :
*                           GT_TRUE:
*                               At least one queue of the port groups has reached
*                               its end.(this would alert the application to
*                               start fetching the waiting messages, as at least
*                               one of the AUQs can't accept additional messages).
*                           GT_FALSE:
*                               No queue reached to the end.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on not initialized queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
GT_STATUS cpssDxChBrgFdbAuqFuqMessagesNumberGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_BOOL                      *endOfQueueReachedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbAuqFuqMessagesNumberGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueType, numOfAuPtr, endOfQueueReachedPtr));

    rc = internal_cpssDxChBrgFdbAuqFuqMessagesNumberGet(devNum, queueType, numOfAuPtr, endOfQueueReachedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueType, numOfAuPtr, endOfQueueReachedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgFdbSecondaryAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller --> from the secondary AUQ.
*       The function cpssDxChHwPpPhase2Init configures queue for secondary AU
*       messages. If common FU queue is used then function returns all AU
*       messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgFdbSecondaryAuMsgBlockGet
(
    IN      GT_U8                                    devNum,
    INOUT   GT_U32                                   *numOfAuPtr,
    OUT     CPSS_MAC_UPDATE_MSG_EXT_STC              *auMessagesPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(numOfAuPtr);
    CPSS_NULL_PTR_CHECK_MAC(auMessagesPtr);


    /* CPU message stalls when the AU buffer is full (FEr#3119)*/
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
             PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E) == GT_FALSE)
    {
        /* the secondary AUQ not active */
        *numOfAuPtr = 0;

        return /* it's not error for log */ GT_NO_MORE;
    }


    return prvDxChBrgFdbAuFuMsgBlockGet(devNum,
            MESSAGE_QUEUE_SECONDARY_AUQ_E,
            numOfAuPtr,
            auMessagesPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbSecondaryAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller --> from the secondary AUQ.
*       The function cpssDxChHwPpPhase2Init configures queue for secondary AU
*       messages. If common FU queue is used then function returns all AU
*       messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecondaryAuMsgBlockGet
(
    IN      GT_U8                                    devNum,
    INOUT   GT_U32                                   *numOfAuPtr,
    OUT     CPSS_MAC_UPDATE_MSG_EXT_STC              *auMessagesPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgFdbSecondaryAuMsgBlockGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, numOfAuPtr, auMessagesPtr));

    rc = internal_cpssDxChBrgFdbSecondaryAuMsgBlockGet(devNum, numOfAuPtr, auMessagesPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, numOfAuPtr, auMessagesPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

