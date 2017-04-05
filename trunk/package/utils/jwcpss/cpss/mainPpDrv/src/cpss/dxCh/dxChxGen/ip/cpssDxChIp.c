 /*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIp.c
*
* DESCRIPTION:
*       The CPSS DXCH Ip HW structures APIs
*
* FILE REVISION NUMBER:
*       $Revision: 94 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/ip/private/prvCpssDxChIpLog.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/dxCh/dxChxGen/ip/private/prvCpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/private/lpm/hw/prvCpssDxChLpmHw.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunk.h>

/* Static function declarations */
static GT_STATUS prvCpssDxChIpHwFormat2MllStruct
(
    IN  GT_U8                   devNum,
    IN   GT_U32                 *hwData,
    IN   GT_BOOL                isFirstMll,
    OUT  CPSS_DXCH_IP_MLL_STC   *mllPairEntryPtr
);
static GT_STATUS prvCpssDxChIpMllStruct2HwFormat
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairWriteForm,
    IN  CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr,
    OUT GT_U32                                      *hwData
);

static GT_STATUS lion3IpMllStruct2HwFormat/*lion3 : sw to hw */
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairWriteForm,
    IN  CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr,
    OUT GT_U32                                      *hwData
);
static GT_STATUS lion3IpMllHw2StructFormat/*lion3 : hw to sw */
(
    IN  GT_U8                                        devNum,
    IN  CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairReadForm,
    OUT CPSS_DXCH_IP_MLL_PAIR_STC                  *mllPairEntryPtr,
    IN GT_U32                                       *hwDataPtr
);


/* number of LTT enrties */
#define PRV_CPSS_DXCH_ROUTER_TCAM_ENRTY_NUM_MAX_CNS    5119


/* number of ARP ADDRESS enrties */
#define PRV_CPSS_DXCH_ARP_ADDR_INDEX_NUM_MAX_CNS    1023

/* number of ARP ADDRESS enrties */
#define PRV_CPSS_DXCH2_ARP_ADDR_INDEX_NUM_MAX_CNS    4095

/* check CH LTT table Index */
#define PRV_CPSS_DXCH_ROUTER_TCAM_INDEX_CHECK_MAC(_lttTTIndex)     \
        if((_lttTTIndex) > PRV_CPSS_DXCH_ROUTER_TCAM_ENRTY_NUM_MAX_CNS)   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);


/* Maximal number of QoS Profiles  for a device */
#define PRV_CPSS_DXCH_ARP_ADDR_INDEX_MAX_MAC(_devNum)                           \
    (((PRV_CPSS_PP_MAC(_devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E) ?   \
     PRV_CPSS_DXCH2_ARP_ADDR_INDEX_NUM_MAX_CNS : PRV_CPSS_DXCH_ARP_ADDR_INDEX_NUM_MAX_CNS)

/* check CH ARP table Index */
#define PRV_CPSS_DXCH_ARP_ADDR_INDEX_CHECK_MAC(_devNum,_arpAddrIndex)     \
        if((_arpAddrIndex) > (GT_U32)(PRV_CPSS_DXCH_ARP_ADDR_INDEX_MAX_MAC(_devNum)))   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

/* Convert hardware value to Aatribute tunnel start passenger type */
#define PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_ATTR_TUNNEL_START_PASS_TYPE_MAC(_attr, _val)  \
    switch (_val)                                                       \
    {                                                                   \
        case 0:                                                         \
            _attr = CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;               \
            break;                                                      \
        case 1:                                                         \
            _attr = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;                     \
            break;                                                      \
        default:                                                        \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                         \
    }

#define CPSS_DXCH_SIZE_OF_MLL_NODE_IN_BYTE  (16)
#define CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD  (CPSS_DXCH_SIZE_OF_MLL_NODE_IN_BYTE >> 2)


/* number of bits in the Bobcat3 - MLL entry */
#define MAX_IP_MLL_ENTRY_BITS_SIZE_CNS       162


/* Bobcat2; Caelum; Bobcat3 - number of word for the rule */
#define NUM_WORDS_RULE_CNS  8 /* need to be like TTI_RULE_SIZE_CNS in cpssDxChTti.c */

/* Bobcat2; Caelum; Bobcat3 - macro assumption : value holds the 'value' and set it into hwFormatArray */
#define LION3_IPVX_FIELD_VALUE_SET_MAC(_dev,_fieldName) \
    LION3_IPVX_FIELD_SET_MAC(_dev,hwDataPtr,_fieldName,value)

/* Bobcat2; Caelum; Bobcat3 - macro assumption : hwFormatArray holds the entry and macro fills the value with 'value'*/
#define LION3_IPVX_FIELD_VALUE_GET_MAC(_dev,_fieldName) \
    LION3_IPVX_FIELD_GET_MAC(_dev,hwDataPtr,_fieldName,value)

/*******************************************************************************
* prvCpssDxCh2Ipv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh2Ipv4PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[6];    /* TCAM key in hw format  */
    GT_U32  maskArr[6];   /* TCAM mask in hw format */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32  mcGroupIndex;
    GT_U32  mcGroupIndexMask;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (routerTtiTcamColumn >= 5)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (prefixPtr->isMcSource == GT_TRUE)
    {
        if (prefixPtr->mcGroupIndexRow >= fineTuningPtr->tableSize.router)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (prefixPtr->mcGroupIndexColumn >= 5)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        /* in Ch2 mc source can reside only on second and third columns */
        if ((routerTtiTcamColumn != 1) && (routerTtiTcamColumn != 2))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (prefixPtr->isMcSource == GT_TRUE)
        mcGroupIndex = (prefixPtr->mcGroupIndexColumn * fineTuningPtr->tableSize.router) + prefixPtr->mcGroupIndexRow;
    else
        mcGroupIndex = 0;

    if (maskPtr->isMcSource == GT_TRUE)
        mcGroupIndexMask = (maskPtr->mcGroupIndexColumn * fineTuningPtr->tableSize.router) + maskPtr->mcGroupIndexRow;
    else
        mcGroupIndexMask = 0;

    /***********************************/
    /* convert to TCAM hardware format */
    /***********************************/

    /* clear hw data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);

    maskArr[2] = 0xFFFF;
    maskArr[3] = 0xFFFF0000;

    /* set the ip address & mask*/
    switch (routerTtiTcamColumn)
    {
        case 0:
        keyArr[0] = (prefixPtr->ipAddr.arIP[3]) |
                    (prefixPtr->ipAddr.arIP[2] << 8) |
                    (prefixPtr->ipAddr.arIP[1] << 16)|
                    (prefixPtr->ipAddr.arIP[0] << 24);

        maskArr[0] = (maskPtr->ipAddr.arIP[3]) |
                     (maskPtr->ipAddr.arIP[2] << 8) |
                     (maskPtr->ipAddr.arIP[1] << 16)|
                     (maskPtr->ipAddr.arIP[0] << 24);
        break;

    case 1:
        keyArr[1] = (prefixPtr->ipAddr.arIP[3]) |
                    (prefixPtr->ipAddr.arIP[2] << 8) |
                    (prefixPtr->ipAddr.arIP[1] << 16)|
                    (prefixPtr->ipAddr.arIP[0] << 24);

        keyArr[2] = (mcGroupIndex & 0x3FFF);

        maskArr[1] = (maskPtr->ipAddr.arIP[3]) |
                     (maskPtr->ipAddr.arIP[2] << 8) |
                     (maskPtr->ipAddr.arIP[1] << 16)|
                     (maskPtr->ipAddr.arIP[0] << 24);

        maskArr[2] = (mcGroupIndexMask & 0x3FFF);

        break;

    case 2:
        keyArr[2] = (prefixPtr->ipAddr.arIP[3] << 16) |
                    (prefixPtr->ipAddr.arIP[2] << 24);

        keyArr[3] = (prefixPtr->ipAddr.arIP[1]) |
                    (prefixPtr->ipAddr.arIP[0] << 8) |
                       (mcGroupIndex & 0x3FFF) << 16;

        maskArr[2] |= (maskPtr->ipAddr.arIP[3] << 16)|
                      (maskPtr->ipAddr.arIP[2] << 24);

        maskArr[3] |= (maskPtr->ipAddr.arIP[1]) |
                      (maskPtr->ipAddr.arIP[0] << 8) |
                      (mcGroupIndexMask & 0x3FFF) << 16;

        break;

    case 3:
        keyArr[4] = (prefixPtr->ipAddr.arIP[3]) |
                    (prefixPtr->ipAddr.arIP[2] << 8) |
                    (prefixPtr->ipAddr.arIP[1] << 16)|
                    (prefixPtr->ipAddr.arIP[0] << 24);

        maskArr[4] = (maskPtr->ipAddr.arIP[3]) |
                     (maskPtr->ipAddr.arIP[2] << 8) |
                     (maskPtr->ipAddr.arIP[1] << 16)|
                     (maskPtr->ipAddr.arIP[0] << 24);
        break;

    case 4:
        keyArr[5] = (prefixPtr->ipAddr.arIP[3]) |
                    (prefixPtr->ipAddr.arIP[2] << 8) |
                    (prefixPtr->ipAddr.arIP[1] << 16)|
                    (prefixPtr->ipAddr.arIP[0] << 24);

        maskArr[5] = (maskPtr->ipAddr.arIP[3]) |
                     (maskPtr->ipAddr.arIP[2] << 8) |
                     (maskPtr->ipAddr.arIP[1] << 16)|
                     (maskPtr->ipAddr.arIP[0] << 24);
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /*****************************/
    /* write the TCAM key & mask */
    /*****************************/

    return  prvCpssDxChRouterTunnelTermTcamKeyMaskWriteEntry(devNum,
                                                             CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                             routerTtiTcamRow,routerTtiTcamColumn,
                                                             keyArr,maskArr);
}

/*******************************************************************************
* prvCpssDxCh3Ipv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3Ipv4PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[NUM_WORDS_RULE_CNS];      /* TCAM key in hw format  */
    GT_U32  maskArr[NUM_WORDS_RULE_CNS];     /* TCAM mask in hw format */
    GT_U32  wordOffset;     /* word offset in hw format to set */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (routerTtiTcamColumn >= 4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (prefixPtr->isMcSource == GT_TRUE)
    {
        if (prefixPtr->mcGroupIndexRow >= fineTuningPtr->tableSize.router)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (prefixPtr->mcGroupIndexColumn >= 4)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
    {
        if (prefixPtr->vrId >= 4096)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /***********************************/
    /* convert to TCAM hardware format */
    /***********************************/

    /* clear hw data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);

    /* set the ip address & mask*/
    switch (routerTtiTcamColumn)
    {
    case 0:
    case 2:
        wordOffset = (routerTtiTcamColumn == 0) ? 0 : 3;

        keyArr[wordOffset] = (prefixPtr->ipAddr.arIP[3]) |
                             (prefixPtr->ipAddr.arIP[2] << 8) |
                             (prefixPtr->ipAddr.arIP[1] << 16)|
                             (prefixPtr->ipAddr.arIP[0] << 24);

        maskArr[wordOffset] = (maskPtr->ipAddr.arIP[3]) |
                              (maskPtr->ipAddr.arIP[2] << 8) |
                              (maskPtr->ipAddr.arIP[1] << 16)|
                              (maskPtr->ipAddr.arIP[0] << 24);

        if (prefixPtr->isMcSource == GT_FALSE)
        {
            keyArr[wordOffset + 1] = (prefixPtr->vrId & 0xfff) |
                                     (0 << 15);     /* indicates not Mc source */

            maskArr[wordOffset + 1] = (maskPtr->vrId & 0xfff) |
                                      (1 << 15);    /* Mc source bit can't be masked */
        }
        else
        {
            keyArr[wordOffset + 1] = (prefixPtr->mcGroupIndexRow & 0x1fff) |
                                     ((prefixPtr->mcGroupIndexColumn & 0x3) << 13) |
                                     (1 << 15);     /* indicates Mc source */

            maskArr[wordOffset + 1] = (maskPtr->mcGroupIndexRow & 0x1fff) |
                                      ((maskPtr->mcGroupIndexColumn & 0x3) << 13) |
                                      (1 << 15);    /* Mc source bit can't be masked */
        }
        break;

    case 1:
    case 3:
        wordOffset = (routerTtiTcamColumn == 1) ? 1 : 4;

        keyArr[wordOffset] = (prefixPtr->ipAddr.arIP[3] << 16) |
                             (prefixPtr->ipAddr.arIP[2] << 24);

        keyArr[wordOffset + 1] = (prefixPtr->ipAddr.arIP[1]) |
                                 (prefixPtr->ipAddr.arIP[0] << 8);

        maskArr[wordOffset] = (maskPtr->ipAddr.arIP[3] << 16) |
                              (maskPtr->ipAddr.arIP[2] << 24);

        maskArr[wordOffset + 1] = (maskPtr->ipAddr.arIP[1]) |
                                  (maskPtr->ipAddr.arIP[0] << 8);

        if (prefixPtr->isMcSource == GT_FALSE)
        {
            keyArr[wordOffset + 1] |= ((prefixPtr->vrId & 0xfff) << 16) |
                                      (0 << 31);     /* indicates not Mc source */

            maskArr[wordOffset + 1] |= ((maskPtr->vrId & 0xfff) << 16) |
                                       (1 << 31);    /* Mc source bit can't be masked */
        }
        else
        {
            keyArr[wordOffset + 1] |= ((prefixPtr->mcGroupIndexRow & 0x1fff) << 16) |
                                      ((prefixPtr->mcGroupIndexColumn & 0x3) << 29) |
                                      (1 << 31);     /* indicates Mc source */

            maskArr[wordOffset + 1] |= ((maskPtr->mcGroupIndexRow & 0x1fff) << 16) |
                                       ((maskPtr->mcGroupIndexColumn & 0x3) << 29) |
                                       (1 << 31);    /* Mc source bit can't be masked */
        }
        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /*****************************/
    /* write the TCAM key & mask */
    /*****************************/

    return  prvCpssDxChRouterTunnelTermTcamKeyMaskWriteEntry(devNum,
                                                             CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                             routerTtiTcamRow,routerTtiTcamColumn,
                                                             keyArr,maskArr);
}


/*******************************************************************************
* internal_cpssDxChIpv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   1.  IP prefix entries reside in router TCAM. The router TCAM is organized
*       in rows, where each row contains 4 column entries for Ch3 and 5 column
*       entries for Ch2. Each line can hold:
*       - 1 TTI (tunnel termination interface) entry
*       - 1 ipv6 address
*       - 4 (ch3)/ 5 (ch2) ipv4 addresses
*       Note that if a line is used for ipv6 address or TTI entry then
*       it can't be used for ipv4 addresses and if an entry in a line is used
*       for ipv4 address, then the other line entries must hold ipv4 addresses
*       as well.
*   2.  The match for prefixes is done using a first match , thus if two prefixes
*       can match a packet (they have the same prefix , but diffrent prefix
*       length) and we want LPM to work on them we have to make sure to put
*       the longest prefix before the short one.
*       Search order is by columns, first column, then second and so on.
*       All and all default UC prefix 0.0.0.0/mask 0 must reside at the last
*       possible ipv4 prefix, and default MC prefix 224.0.0.0/ mask 240.0.0.0
*       must reside at the index before it.
*   3.  In Ch2 MC source addresses can reside only on the second and third
*       columns (on other PP, there is no such restriction).
*   4.  In Ch2 there is no support for multiple virtual routers. Therefore in
*       Ch2 vrId is ignored.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpv4PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    /* make sure the device is Ch2 and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        return prvCpssDxCh2Ipv4PrefixSet(devNum,
                                         routerTtiTcamRow,routerTtiTcamColumn,
                                         prefixPtr,maskPtr);
    default:
        return prvCpssDxCh3Ipv4PrefixSet(devNum,
                                         routerTtiTcamRow,routerTtiTcamColumn,
                                         prefixPtr,maskPtr);
    }
}

/*******************************************************************************
* cpssDxChIpv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   1.  IP prefix entries reside in router TCAM. The router TCAM is organized
*       in rows, where each row contains 4 column entries for Ch3 and 5 column
*       entries for Ch2. Each line can hold:
*       - 1 TTI (tunnel termination interface) entry
*       - 1 ipv6 address
*       - 4 (ch3)/ 5 (ch2) ipv4 addresses
*       Note that if a line is used for ipv6 address or TTI entry then
*       it can't be used for ipv4 addresses and if an entry in a line is used
*       for ipv4 address, then the other line entries must hold ipv4 addresses
*       as well.
*   2.  The match for prefixes is done using a first match , thus if two prefixes
*       can match a packet (they have the same prefix , but diffrent prefix
*       length) and we want LPM to work on them we have to make sure to put
*       the longest prefix before the short one.
*       Search order is by columns, first column, then second and so on.
*       All and all default UC prefix 0.0.0.0/mask 0 must reside at the last
*       possible ipv4 prefix, and default MC prefix 224.0.0.0/ mask 240.0.0.0
*       must reside at the index before it.
*   3.  In Ch2 MC source addresses can reside only on the second and third
*       columns (on other PP, there is no such restriction).
*   4.  In Ch2 there is no support for multiple virtual routers. Therefore in
*       Ch2 vrId is ignored.
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpv4PrefixSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTtiTcamRow, routerTtiTcamColumn, prefixPtr, maskPtr));

    rc = internal_cpssDxChIpv4PrefixSet(devNum, routerTtiTcamRow, routerTtiTcamColumn, prefixPtr, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTtiTcamRow, routerTtiTcamColumn, prefixPtr, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxCh2Ipv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh2Ipv4PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[NUM_WORDS_RULE_CNS];          /* TCAM key in hw format            */
    GT_U32  maskArr[NUM_WORDS_RULE_CNS];         /* TCAM mask in hw format           */
    GT_U32  actionArr[6];       /* TCAM action (rule) in hw format  */
    GT_U32  validArr[5];        /* TCAM line valid bits             */
    GT_U32  compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32  retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (routerTtiTcamColumn >= 5)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

     /* clear data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    cpssOsMemSet(prefixPtr, 0, sizeof(CPSS_DXCH_IPV4_PREFIX_STC));
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_IPV4_PREFIX_STC));

    /* read hw data */
    retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    &keyArr[0],&maskArr[0],
                                                    &actionArr[0]);
    if (retVal != GT_OK)
        return retVal;

    /* convert from HW format to ip address & mask */
    /***********************************************/

    switch (routerTtiTcamColumn)
    {
    case 0:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 24,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 24,8);

        break;

    case 1:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 24,8);

        prefixPtr->mcGroupIndexRow    = U32_GET_FIELD_MAC(keyArr[2], 0,10);
        prefixPtr->mcGroupIndexColumn = U32_GET_FIELD_MAC(keyArr[2], 10,4);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 24,8);

        maskPtr->mcGroupIndexRow    = U32_GET_FIELD_MAC(maskArr[2], 0,10);
        maskPtr->mcGroupIndexColumn = U32_GET_FIELD_MAC(maskArr[2], 10,4);

        break;

    case 2:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 16,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 24,8);

        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 0,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 8,8);

        prefixPtr->mcGroupIndexRow    = U32_GET_FIELD_MAC(keyArr[3], 16,10);
        prefixPtr->mcGroupIndexColumn = U32_GET_FIELD_MAC(keyArr[3], 26,4);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 16,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 24,8);

        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 0,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 8,8);

        maskPtr->mcGroupIndexRow    = U32_GET_FIELD_MAC(maskArr[3], 16,10);
        maskPtr->mcGroupIndexColumn = U32_GET_FIELD_MAC(maskArr[3], 26,4);

        break;

    case 3:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 24,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 24,8);

        break;

    case 4:
        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 24,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 24,8);

        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if ((prefixPtr->mcGroupIndexColumn != 0) || (prefixPtr->mcGroupIndexRow != 0))
        maskPtr->isMcSource = prefixPtr->isMcSource = GT_TRUE;
    else
        maskPtr->isMcSource = prefixPtr->isMcSource = GT_FALSE;

    /* entry holds valid IPv4 prefix if the followings applies:
         - the entry is valid
         - the compare mode of the entry is single compare mode */
    if ((validArr[routerTtiTcamColumn] == 1) && (compareModeArr[routerTtiTcamColumn] == 0))
        *validPtr = GT_TRUE;
    else
        *validPtr = GT_FALSE;

    return retVal;
}

/*******************************************************************************
* prvCpssDxCh3Ipv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3Ipv4PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  GT_U32                      routerTtiTcamColumn,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[NUM_WORDS_RULE_CNS];          /* TCAM key in hw format            */
    GT_U32  maskArr[NUM_WORDS_RULE_CNS];         /* TCAM mask in hw format           */
    /* TCAM action (rule) in hw format  */
    GT_U32  actionArr[PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_MAX_LINE_ACTION_SIZE_CNS];
    GT_U32  validArr[5];        /* TCAM line valid bits             */
    GT_U32  compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32  wordOffset;         /* word offset in hw format to set  */
    GT_U32  retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (routerTtiTcamColumn >= 4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

     /* clear data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    cpssOsMemSet(prefixPtr, 0, sizeof(CPSS_DXCH_IPV4_PREFIX_STC));
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_IPV4_PREFIX_STC));

    /* read hw data */

    retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    &keyArr[0],&maskArr[0],
                                                    &actionArr[0]);
    if (retVal != GT_OK)
        return retVal;

    /* convert from HW format to ip address & mask */
    /***********************************************/

    switch (routerTtiTcamColumn)
    {
    case 0:
    case 2:
        wordOffset = (routerTtiTcamColumn == 0) ? 0 : 3;

        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 0,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 8,8);
        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 16,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 24,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 0,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 8,8);
        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 16,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 24,8);

        prefixPtr->isMcSource = maskPtr->isMcSource =
            (((keyArr[wordOffset + 1] >> 15) & 0x1) == 0) ? GT_FALSE : GT_TRUE;

        if (prefixPtr->isMcSource == GT_FALSE)
        {
            prefixPtr->vrId = (keyArr[wordOffset + 1] & 0xfff);
            maskPtr->vrId =   (maskArr[wordOffset + 1] & 0xfff);
        }
        else
        {
            prefixPtr->mcGroupIndexRow =    ((keyArr[wordOffset + 1] & 0x1fff));
            prefixPtr->mcGroupIndexColumn = ((keyArr[wordOffset + 1] >> 13) & 0x3);
            maskPtr->mcGroupIndexRow =      ((maskArr[wordOffset + 1] & 0x1fff));
            maskPtr->mcGroupIndexColumn =   ((maskArr[wordOffset + 1] >> 13) & 0x3);
        }

        break;

    case 1:
    case 3:
        wordOffset = (routerTtiTcamColumn == 1) ? 1 : 4;

        prefixPtr->ipAddr.arIP[3] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 16,8);
        prefixPtr->ipAddr.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset], 24,8);

        prefixPtr->ipAddr.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset + 1], 0,8);
        prefixPtr->ipAddr.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(keyArr[wordOffset + 1], 8,8);

        maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 16,8);
        maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset], 24,8);

        maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset + 1], 0,8);
        maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[wordOffset + 1], 8,8);

        prefixPtr->isMcSource = maskPtr->isMcSource =
            (((keyArr[wordOffset + 1] >> 31) & 0x1) == 0) ? GT_FALSE : GT_TRUE;

        if (prefixPtr->isMcSource == GT_FALSE)
        {
            prefixPtr->vrId = ((keyArr[wordOffset + 1] >> 16) & 0xfff);
            maskPtr->vrId =   ((maskArr[wordOffset + 1] >> 16) & 0xfff);
        }
        else
        {
            prefixPtr->mcGroupIndexRow =    ((keyArr[wordOffset + 1] >> 16) & 0x1fff);
            prefixPtr->mcGroupIndexColumn = ((keyArr[wordOffset + 1] >> 29) & 0x3);
            maskPtr->mcGroupIndexRow =      ((maskArr[wordOffset + 1] >> 16) & 0x1fff);
            maskPtr->mcGroupIndexColumn =   ((maskArr[wordOffset + 1] >> 29) & 0x3);
        }

        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* entry holds valid IPv4 prefix if the followings applies:
         - the entry is valid
         - the compare mode of the entry is single compare mode */
    if ((validArr[routerTtiTcamColumn] == 1) && (compareModeArr[routerTtiTcamColumn] == 0))
        *validPtr = GT_TRUE;
    else
        *validPtr = GT_FALSE;

    return retVal;
}

/*******************************************************************************
* internal_cpssDxChIpv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpv4PrefixGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTtiTcamRow,
    IN  GT_U32      routerTtiTcamColumn,
    OUT GT_BOOL     *validPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    /* make sure the device is Ch2 and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        return prvCpssDxCh2Ipv4PrefixGet(devNum,
                                         routerTtiTcamRow,routerTtiTcamColumn,
                                         validPtr,prefixPtr,maskPtr);
    default:
        return prvCpssDxCh3Ipv4PrefixGet(devNum,
                                         routerTtiTcamRow,routerTtiTcamColumn,
                                         validPtr,prefixPtr,maskPtr);
    }
}

/*******************************************************************************
* cpssDxChIpv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTtiTcamRow,
    IN  GT_U32      routerTtiTcamColumn,
    OUT GT_BOOL     *validPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV4_PREFIX_STC   *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpv4PrefixGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTtiTcamRow, routerTtiTcamColumn, validPtr, prefixPtr, maskPtr));

    rc = internal_cpssDxChIpv4PrefixGet(devNum, routerTtiTcamRow, routerTtiTcamColumn, validPtr, prefixPtr, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTtiTcamRow, routerTtiTcamColumn, validPtr, prefixPtr, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpv4PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv4 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*       routerTtiTcamColumn       - the TCAM column to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpv4PrefixInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTtiTcamRow,
    IN  GT_U32      routerTtiTcamColumn
)
{
    GT_U32  retVal = GT_OK;         /* function return code */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        if (routerTtiTcamColumn >= 5)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    default:
        if (routerTtiTcamColumn >= 4)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    }

    retVal = prvCpssDxChRouterTunnelTermTcamInvalidateEntry(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow,routerTtiTcamColumn);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpv4PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv4 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*       routerTtiTcamColumn       - the TCAM column to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv4PrefixInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTtiTcamRow,
    IN  GT_U32      routerTtiTcamColumn
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpv4PrefixInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTtiTcamRow, routerTtiTcamColumn));

    rc = internal_cpssDxChIpv4PrefixInvalidate(devNum, routerTtiTcamRow, routerTtiTcamColumn);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTtiTcamRow, routerTtiTcamColumn));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxCh2Ipv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh2Ipv6PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[NUM_WORDS_RULE_CNS];   /* TCAM key in hw format  */
    GT_U32  maskArr[NUM_WORDS_RULE_CNS];  /* TCAM mask in hw format */
    GT_U32  retVal;      /* function return code   */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (prefixPtr->isMcSource == GT_TRUE)
    {
        if (prefixPtr->mcGroupIndexRow >= fineTuningPtr->tableSize.router)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /***********************************/
    /* convert to TCAM hardware format */
    /***********************************/

    /* clear hw data */
     cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
     cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);

    /* set the ip address */
    /**********************/

    keyArr[0] = (prefixPtr->ipAddr.arIP[15]) |
                (prefixPtr->ipAddr.arIP[14] << 8) |
                (prefixPtr->ipAddr.arIP[13] << 16)|
                (prefixPtr->ipAddr.arIP[12] << 24);

    keyArr[1] = (prefixPtr->ipAddr.arIP[11]) |
                (prefixPtr->ipAddr.arIP[10] << 8) |
                (prefixPtr->ipAddr.arIP[9]  << 16)|
                (prefixPtr->ipAddr.arIP[8]  << 24);

    if (prefixPtr->isMcSource == GT_TRUE)
    {
        keyArr[2] = (prefixPtr->mcGroupIndexRow & 0x3FFF);
    }

    keyArr[3] = 0;

    keyArr[4] = (prefixPtr->ipAddr.arIP[7]) |
                (prefixPtr->ipAddr.arIP[6] << 8) |
                (prefixPtr->ipAddr.arIP[5] << 16)|
                (prefixPtr->ipAddr.arIP[4] << 24);

    keyArr[5] = (prefixPtr->ipAddr.arIP[3]) |
                (prefixPtr->ipAddr.arIP[2] << 8) |
                (prefixPtr->ipAddr.arIP[1] << 16)|
                (prefixPtr->ipAddr.arIP[0] << 24);

    /* set the mask */
    /****************/

    maskArr[0] = (maskPtr->ipAddr.arIP[15]) |
                 (maskPtr->ipAddr.arIP[14] << 8) |
                 (maskPtr->ipAddr.arIP[13] << 16)|
                 (maskPtr->ipAddr.arIP[12] << 24);

    maskArr[1] = (maskPtr->ipAddr.arIP[11]) |
                 (maskPtr->ipAddr.arIP[10] << 8) |
                 (maskPtr->ipAddr.arIP[9]  << 16)|
                 (maskPtr->ipAddr.arIP[8]  << 24);

    if (prefixPtr->isMcSource == GT_TRUE)
    {
        maskArr[2] = (maskPtr->mcGroupIndexRow & 0x3FFF) |
                    (0xffffc000);
    }

    maskArr[3] = 0xFFFFFFFF;

    maskArr[4] = (maskPtr->ipAddr.arIP[7]) |
                 (maskPtr->ipAddr.arIP[6] << 8) |
                 (maskPtr->ipAddr.arIP[5] << 16)|
                 (maskPtr->ipAddr.arIP[4] << 24);

    maskArr[5] = (maskPtr->ipAddr.arIP[3]) |
                 (maskPtr->ipAddr.arIP[2] << 8) |
                 (maskPtr->ipAddr.arIP[1] << 16)|
                 (maskPtr->ipAddr.arIP[0] << 24);


    /*****************************/
    /* write the TCAM key & mask */
    /*****************************/

    retVal =  prvCpssDxChRouterTunnelTermTcamKeyMaskWriteLine(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                              routerTtiTcamRow,keyArr,maskArr);

    if (retVal != GT_OK)
        return retVal;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxCh3Ipv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3Ipv6PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[NUM_WORDS_RULE_CNS];   /* TCAM key in hw format  */
    GT_U32  maskArr[NUM_WORDS_RULE_CNS];  /* TCAM mask in hw format */
    GT_U32  retVal;      /* function return code   */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (prefixPtr->isMcSource == GT_TRUE)
    {
        if (prefixPtr->mcGroupIndexRow >= fineTuningPtr->tableSize.router)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
    {
        if (prefixPtr->vrId >= 4096)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /***********************************/
    /* convert to TCAM hardware format */
    /***********************************/

    /* clear hw data */
     cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
     cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);

    /* set the ip address key */
    /**************************/

    keyArr[0] = (prefixPtr->ipAddr.arIP[15]) |
                (prefixPtr->ipAddr.arIP[14] << 8) |
                (prefixPtr->ipAddr.arIP[13] << 16)|
                (prefixPtr->ipAddr.arIP[12] << 24);

    keyArr[1] = (prefixPtr->ipAddr.arIP[11]) |
                (prefixPtr->ipAddr.arIP[10] << 8) |
                (prefixPtr->ipAddr.arIP[9]  << 16)|
                (prefixPtr->ipAddr.arIP[8]  << 24);

    keyArr[2] = (prefixPtr->ipAddr.arIP[7]) |
                (prefixPtr->ipAddr.arIP[6] << 8) |
                (prefixPtr->ipAddr.arIP[5] << 16)|
                (prefixPtr->ipAddr.arIP[4] << 24);

    keyArr[3] = (prefixPtr->ipAddr.arIP[3]) |
                (prefixPtr->ipAddr.arIP[2] << 8) |
                (prefixPtr->ipAddr.arIP[1] << 16)|
                (prefixPtr->ipAddr.arIP[0] << 24);

    /* set the ip address mask */
    /***************************/

    maskArr[0] = (maskPtr->ipAddr.arIP[15]) |
                 (maskPtr->ipAddr.arIP[14] << 8) |
                 (maskPtr->ipAddr.arIP[13] << 16)|
                 (maskPtr->ipAddr.arIP[12] << 24);

    maskArr[1] = (maskPtr->ipAddr.arIP[11]) |
                 (maskPtr->ipAddr.arIP[10] << 8) |
                 (maskPtr->ipAddr.arIP[9]  << 16)|
                 (maskPtr->ipAddr.arIP[8]  << 24);

    maskArr[2] = (maskPtr->ipAddr.arIP[7]) |
                 (maskPtr->ipAddr.arIP[6] << 8) |
                 (maskPtr->ipAddr.arIP[5] << 16)|
                 (maskPtr->ipAddr.arIP[4] << 24);

    maskArr[3] = (maskPtr->ipAddr.arIP[3]) |
                 (maskPtr->ipAddr.arIP[2] << 8) |
                 (maskPtr->ipAddr.arIP[1] << 16)|
                 (maskPtr->ipAddr.arIP[0] << 24);

    /* set the vr Id or MC Group Index */
    /***********************************/

    if (prefixPtr->isMcSource == GT_FALSE)
    {
        keyArr[4] = (prefixPtr->vrId & 0xfff) |
                    (0 << 15); /* indicates not Mc Source */

        maskArr[4] = (maskPtr->vrId & 0xfff) |
                      0xffff8000;
    }
    else
    {
        keyArr[4] = (prefixPtr->mcGroupIndexRow & 0x7fff) |
                    (1 << 15); /* indicate Mc source */

        maskArr[4] = (maskPtr->mcGroupIndexRow & 0x7fff) |
                     0xffffffff;
    }

    keyArr[5] = 0;

    maskArr[5] = 0xffffffff;


    /*****************************/
    /* write the TCAM key & mask */
    /*****************************/
    retVal =  prvCpssDxChRouterTunnelTermTcamKeyMaskWriteLine(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                              routerTtiTcamRow,keyArr,maskArr);
    if (retVal != GT_OK)
        return retVal;

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChIpv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpv6PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    /* make sure the device is Ch2 and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        return prvCpssDxCh2Ipv6PrefixSet(devNum,routerTtiTcamRow,
                                         prefixPtr,maskPtr);
    default:
        return prvCpssDxCh3Ipv6PrefixSet(devNum,routerTtiTcamRow,
                                         prefixPtr,maskPtr);
    }
}

/*******************************************************************************
* cpssDxChIpv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    IN  CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpv6PrefixSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTtiTcamRow, prefixPtr, maskPtr));

    rc = internal_cpssDxChIpv6PrefixSet(devNum, routerTtiTcamRow, prefixPtr, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTtiTcamRow, prefixPtr, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxCh2Ipv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh2Ipv6PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[NUM_WORDS_RULE_CNS];          /* TCAM key in hw format            */
    GT_U32  maskArr[NUM_WORDS_RULE_CNS];         /* TCAM mask in hw format           */
    GT_U32  actionArr[6];       /* TCAM action (rule) in hw format  */
    GT_U32  validArr[5];        /* TCAM line valid bits             */
    GT_U32  compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32  retVal = GT_OK;     /* function return code             */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32  i;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

     /* clear hw data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    /* clear output data */
    *validPtr = GT_FALSE;
    cpssOsMemSet(prefixPtr, 0, sizeof(CPSS_DXCH_IPV6_PREFIX_STC));
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_IPV6_PREFIX_STC));

    retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    &keyArr[0],
                                                    &maskArr[0],
                                                    &actionArr[0]);
    if (retVal != GT_OK)
        return retVal;

    /* convert from HW format to ip address */
    /****************************************/

    prefixPtr->ipAddr.arIP[15] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 0,8);
    prefixPtr->ipAddr.arIP[14] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 8,8);
    prefixPtr->ipAddr.arIP[13] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 16,8);
    prefixPtr->ipAddr.arIP[12] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 24,8);

    prefixPtr->ipAddr.arIP[11] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 0,8);
    prefixPtr->ipAddr.arIP[10] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 8,8);
    prefixPtr->ipAddr.arIP[9]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 16,8);
    prefixPtr->ipAddr.arIP[8]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 24,8);

    prefixPtr->mcGroupIndexRow = U32_GET_FIELD_MAC(keyArr[2], 0,14);

    /* keyArr[3] = 0; */

    prefixPtr->ipAddr.arIP[7]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 0,8);
    prefixPtr->ipAddr.arIP[6]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 8,8);
    prefixPtr->ipAddr.arIP[5]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 16,8);
    prefixPtr->ipAddr.arIP[4]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[4], 24,8);

    prefixPtr->ipAddr.arIP[3]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 0,8);
    prefixPtr->ipAddr.arIP[2]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 8,8);
    prefixPtr->ipAddr.arIP[1]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 16,8);
    prefixPtr->ipAddr.arIP[0]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[5], 24,8);

    /* convert from HW format to mask */
    /**********************************/

    maskPtr->ipAddr.arIP[15]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 0,8);
    maskPtr->ipAddr.arIP[14]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 8,8);
    maskPtr->ipAddr.arIP[13]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 16,8);
    maskPtr->ipAddr.arIP[12]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 24,8);

    maskPtr->ipAddr.arIP[11]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 0,8);
    maskPtr->ipAddr.arIP[10]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 8,8);
    maskPtr->ipAddr.arIP[9]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 16,8);
    maskPtr->ipAddr.arIP[8]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 24,8);

    maskPtr->mcGroupIndexRow   = U32_GET_FIELD_MAC(maskArr[2], 0,14);

    /* maskArr[3] = 0xFFFFFFFF; */

    maskPtr->ipAddr.arIP[7]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 0,8);
    maskPtr->ipAddr.arIP[6]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 8,8);
    maskPtr->ipAddr.arIP[5]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 16,8);
    maskPtr->ipAddr.arIP[4]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[4], 24,8);

    maskPtr->ipAddr.arIP[3]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 0,8);
    maskPtr->ipAddr.arIP[2]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 8,8);
    maskPtr->ipAddr.arIP[1]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 16,8);
    maskPtr->ipAddr.arIP[0]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[5], 24,8);

    if (prefixPtr->mcGroupIndexRow != 0)
        prefixPtr->isMcSource = maskPtr->isMcSource = GT_TRUE;
    else
        prefixPtr->isMcSource = maskPtr->isMcSource = GT_FALSE;

    /* line holds valid IPv6 prefix if the following applies:
         - all entries are valid
         - the compare mode or all entries is row compare
         - keyArr[3] bit 31 must be 0 (to indicate IPv6 entry and not TT entry) */
    *validPtr = GT_TRUE;
    for (i = 0 ; i < 5 ; i++)
    {
        /* if entry is not valid or is single compare mode, whole line is not valid */
        if ((validArr[i] == 0) || (compareModeArr[i] == 0))
        {
            *validPtr = GT_FALSE;
            break;
        }
    }
    /* if whole line is valid, verify it is indeed IPv6 entry and not TT entry */
    if ((*validPtr == GT_TRUE) && (((keyArr[3] >> 31) & 0x1) != 0))
    {
        *validPtr = GT_FALSE;
    }

    return retVal;
}


/*******************************************************************************
* prvCpssDxCh3Ipv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3Ipv6PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_U32  keyArr[NUM_WORDS_RULE_CNS];          /* TCAM key in hw format            */
    GT_U32  maskArr[NUM_WORDS_RULE_CNS];         /* TCAM mask in hw format           */
    /* TCAM action (rule) in hw format  */
    GT_U32  actionArr[PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_MAX_LINE_ACTION_SIZE_CNS];
    GT_U32  validArr[5];        /* TCAM line valid bits             */
    GT_U32  compareModeArr[5];  /* TCAM line compare mode           */
    GT_U32  retVal = GT_OK;     /* function return code             */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32  i;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

     /* clear hw data */
    cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * NUM_WORDS_RULE_CNS);
    /* clear output data */
    *validPtr = GT_FALSE;
    cpssOsMemSet(prefixPtr, 0, sizeof(CPSS_DXCH_IPV6_PREFIX_STC));
    cpssOsMemSet(maskPtr, 0, sizeof(CPSS_DXCH_IPV6_PREFIX_STC));

    retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    routerTtiTcamRow,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    &keyArr[0],
                                                    &maskArr[0],
                                                    &actionArr[0]);
    if (retVal != GT_OK)
        return retVal;

    /* convert from HW format to ip address */
    /****************************************/

    prefixPtr->ipAddr.arIP[15] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 0,8);
    prefixPtr->ipAddr.arIP[14] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 8,8);
    prefixPtr->ipAddr.arIP[13] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 16,8);
    prefixPtr->ipAddr.arIP[12] = (GT_U8)U32_GET_FIELD_MAC(keyArr[0], 24,8);

    prefixPtr->ipAddr.arIP[11] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 0,8);
    prefixPtr->ipAddr.arIP[10] = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 8,8);
    prefixPtr->ipAddr.arIP[9]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 16,8);
    prefixPtr->ipAddr.arIP[8]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[1], 24,8);

    prefixPtr->ipAddr.arIP[7]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 0,8);
    prefixPtr->ipAddr.arIP[6]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 8,8);
    prefixPtr->ipAddr.arIP[5]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 16,8);
    prefixPtr->ipAddr.arIP[4]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[2], 24,8);

    prefixPtr->ipAddr.arIP[3]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 0,8);
    prefixPtr->ipAddr.arIP[2]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 8,8);
    prefixPtr->ipAddr.arIP[1]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 16,8);
    prefixPtr->ipAddr.arIP[0]  = (GT_U8)U32_GET_FIELD_MAC(keyArr[3], 24,8);

    /* convert from HW format to mask */
    /**********************************/

    maskPtr->ipAddr.arIP[15]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 0,8);
    maskPtr->ipAddr.arIP[14]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 8,8);
    maskPtr->ipAddr.arIP[13]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 16,8);
    maskPtr->ipAddr.arIP[12]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[0], 24,8);

    maskPtr->ipAddr.arIP[11]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 0,8);
    maskPtr->ipAddr.arIP[10]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 8,8);
    maskPtr->ipAddr.arIP[9]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 16,8);
    maskPtr->ipAddr.arIP[8]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[1], 24,8);

    maskPtr->ipAddr.arIP[7]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 0,8);
    maskPtr->ipAddr.arIP[6]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 8,8);
    maskPtr->ipAddr.arIP[5]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 16,8);
    maskPtr->ipAddr.arIP[4]    = (GT_U8)U32_GET_FIELD_MAC(maskArr[2], 24,8);

    maskPtr->ipAddr.arIP[3]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 0,8);
    maskPtr->ipAddr.arIP[2]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 8,8);
    maskPtr->ipAddr.arIP[1]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 16,8);
    maskPtr->ipAddr.arIP[0]   = (GT_U8)U32_GET_FIELD_MAC(maskArr[3], 24,8);

    /* get the vr Id or MC Group Index */
    /***********************************/

    prefixPtr->isMcSource = maskPtr->isMcSource =
        (((keyArr[4] >> 15) & 0x1) == 1) ? GT_TRUE : GT_FALSE;

    if (prefixPtr->isMcSource == GT_FALSE)
    {
        prefixPtr->vrId = (keyArr[4] & 0xfff);
        maskPtr->vrId   = (maskArr[4] & 0xfff);
    }
    else
    {
        prefixPtr->mcGroupIndexRow = (keyArr[4] & 0x7fff);
        maskPtr->mcGroupIndexRow   = (maskArr[4] & 0x7fff);
    }

    /* line holds valid IPv6 prefix if the following applies:
         - all entries are valid
         - the compare mode or all entries is row compare
         - keyArr[5] bit 31 must be 0 (to indicate IPv6 entry and not TT entry) */
    *validPtr = GT_TRUE;
    for (i = 0 ; i < 4 ; i++)
    {
        /* if entry is not valid or is single compare mode, whole line is not valid */
        if ((validArr[i] == 0) || (compareModeArr[i] == 0))
        {
            *validPtr = GT_FALSE;
            break;
        }
    }
    /* if whole line is valid, verify it is indeed IPv6 entry and not TTI entry */
    if ((*validPtr == GT_TRUE) && (((keyArr[5] >> 31) & 0x1) != 0))
    {
        *validPtr = GT_FALSE;
    }

    return retVal;
}


/*******************************************************************************
* internal_cpssDxChIpv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpv6PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    /* make sure the device is Ch2 and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        return prvCpssDxCh2Ipv6PrefixGet(devNum,routerTtiTcamRow,
                                         validPtr,prefixPtr,maskPtr);
    default:
        return prvCpssDxCh3Ipv6PrefixGet(devNum,routerTtiTcamRow,
                                         validPtr,prefixPtr,maskPtr);
    }
}

/*******************************************************************************
* cpssDxChIpv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerTtiTcamRow,
    OUT GT_BOOL                     *validPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *prefixPtr,
    OUT CPSS_DXCH_IPV6_PREFIX_STC   *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpv6PrefixGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTtiTcamRow, validPtr, prefixPtr, maskPtr));

    rc = internal_cpssDxChIpv6PrefixGet(devNum, routerTtiTcamRow, validPtr, prefixPtr, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTtiTcamRow, validPtr, prefixPtr, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpv6PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv6 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that invalidating ipv6 address (that takes whole TCAM line) will
*       result invalidating all columns in that line.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpv6PrefixInvalidate
(
    IN  GT_U8           devNum,
    IN  GT_U32          routerTtiTcamRow
)
{
    GT_U32  retVal = GT_OK;         /* function return code */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerTtiTcamRow >= fineTuningPtr->tableSize.router)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    retVal = prvCpssDxChRouterTunnelTermTcamInvalidateLine(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,routerTtiTcamRow);

    return retVal;
}

/*******************************************************************************
* cpssDxChIpv6PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv6 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that invalidating ipv6 address (that takes whole TCAM line) will
*       result invalidating all columns in that line.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpv6PrefixInvalidate
(
    IN  GT_U8           devNum,
    IN  GT_U32          routerTtiTcamRow
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpv6PrefixInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerTtiTcamRow));

    rc = internal_cpssDxChIpv6PrefixInvalidate(devNum, routerTtiTcamRow);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerTtiTcamRow));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - the device number.
*       lttTtiRow     - the entry's row index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttTtiColumn   - the entry's column index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttEntryPtr   - the lookup translation table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active or
*                         invalid ipv6MCGroupScopeLevel value.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Lookup translation table is shared with the Tunnel Termination (TT)
*       Table.
*       If the relevant index in the Router Tcam is in line used for tunnel
*       termination key then the indexes in the LTT/TT table will function as
*       Tunnel Termination Action, and you shouldn't write LTT entries in these
*       indexes. It works Vice-verse.
*       For Ipv6 Entries the relevant index is the line prefix (same index sent
*       as parameter to the ipv6 prefix set API). The other LTT/TT entries
*       related to that TCAM line are left unused.
*
*       FEr#2018 - if the PCL action uses redirection to Router Lookup Translation Table (LTT)
*                 (policy based routing) or if the TTI action uses redirection to LTT
*                 (TTI base routing), then the LTT index written in the PCL/TTI action is
*                  restricted only to column 0 of the LTT row.
*       NOTE: - Since LTT entries can be used also for Router TCAM routing, this API is not
*               affected directly by this errata. However when the LTT should be pointed from
*               PCL action or LTT action, it should be set at column 0.
*             - The API affected by this errata are PCL rule set, PCL action update,
*               TTI rule set and TTI action update.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLttWrite
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          lttTtiRow,
    IN  GT_U32                          lttTtiColumn,
    IN  CPSS_DXCH_IP_LTT_ENTRY_STC      *lttEntryPtr
)
{
    GT_U32 lltEntryHwFormat = 0;
    GT_U32 lltEntryHwFormatLength=0;
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* ltt size is the same as router TCAM size */
    if (lttTtiRow >= PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        if (lttTtiColumn >= 5)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    default:
        if (lttTtiColumn >= 4)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    }
    CPSS_NULL_PTR_CHECK_MAC(lttEntryPtr);

    /* check ltt parameters */
    switch (lttEntryPtr->routeType)
    {
    case CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_QOS_E:
    case CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        if (lttEntryPtr->numOfPaths > CPSS_DXCH_IP_MAX_ECMP_QOS_GROUP)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
    {
        if(lttEntryPtr->routeType == CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_QOS_E)
        {
            if (lttEntryPtr->numOfPaths > CPSS_DXCH_IP_MAX_LION_QOS_GROUP)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        else
        {
            if (lttEntryPtr->numOfPaths > CPSS_DXCH_IP_MAX_LION_ECMP_GROUP)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

    }
    if (lttEntryPtr->routeEntryBaseIndex >= fineTuningPtr->tableSize.routerNextHop)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    switch(lttEntryPtr->ipv6MCGroupScopeLevel)
    {
    case CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E:
    case CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E:
    case CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E:
    case CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* build the 22 bits of the llt entry */
    if(lttEntryPtr->routeType == CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_QOS_E)
        lltEntryHwFormat = 0x1;

    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        lltEntryHwFormat |= (lttEntryPtr->numOfPaths & 0x7) << 1;
        lltEntryHwFormat |= (BOOL2BIT_MAC(lttEntryPtr->ucRPFCheckEnable)) << 4;
        lltEntryHwFormat |= (lttEntryPtr->ipv6MCGroupScopeLevel & 0x3) << 5;
        lltEntryHwFormat |= (BOOL2BIT_MAC(lttEntryPtr->sipSaCheckMismatchEnable)) << 7;
        lltEntryHwFormat |= lttEntryPtr->routeEntryBaseIndex << 8;
        lltEntryHwFormatLength   = 22;
    }
    else
    {
        lltEntryHwFormat |= (lttEntryPtr->numOfPaths & 0x3F) << 1;
        lltEntryHwFormat |= (BOOL2BIT_MAC(lttEntryPtr->ucRPFCheckEnable)) << 7;
        lltEntryHwFormat |= (lttEntryPtr->ipv6MCGroupScopeLevel & 0x3) << 8;
        lltEntryHwFormat |= (BOOL2BIT_MAC(lttEntryPtr->sipSaCheckMismatchEnable)) << 10;
        lltEntryHwFormat |= lttEntryPtr->routeEntryBaseIndex << 11;
        lltEntryHwFormatLength   = 25;
    }

    /* now set the 22 bits in the hw according the location of the entry in the line */
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* for Cheeatah2 devices */
    case CPSS_PP_FAMILY_CHEETAH2_E:
        switch(lttTtiColumn)
        {
        case 0:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,0,0,22,lltEntryHwFormat);
            break;

        case 1:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,0,22,10,lltEntryHwFormat);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,1,0,12,lltEntryHwFormat >> 10);
            break;

        case 2:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,1,12,20,lltEntryHwFormat);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,2,0,2,lltEntryHwFormat >> 20);
            break;

        case 3:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,2,2,22,lltEntryHwFormat);
            break;

        case 4:
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,2,24,8,lltEntryHwFormat);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                     lttTtiRow,3,0,14,lltEntryHwFormat >> 8);
            break;
        }
        break;

        /* for cheetah3 devices */
    default:
        retVal = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
                                                 lttTtiRow,lttTtiColumn,0,
                                                 lltEntryHwFormatLength,
                                                 lltEntryHwFormat);
        break;
    }

    return (retVal);
}

/*******************************************************************************
* cpssDxChIpLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - the device number.
*       lttTtiRow     - the entry's row index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttTtiColumn   - the entry's column index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttEntryPtr   - the lookup translation table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active or
*                         invalid ipv6MCGroupScopeLevel value.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Lookup translation table is shared with the Tunnel Termination (TT)
*       Table.
*       If the relevant index in the Router Tcam is in line used for tunnel
*       termination key then the indexes in the LTT/TT table will function as
*       Tunnel Termination Action, and you shouldn't write LTT entries in these
*       indexes. It works Vice-verse.
*       For Ipv6 Entries the relevant index is the line prefix (same index sent
*       as parameter to the ipv6 prefix set API). The other LTT/TT entries
*       related to that TCAM line are left unused.
*
*       FEr#2018 - if the PCL action uses redirection to Router Lookup Translation Table (LTT)
*                 (policy based routing) or if the TTI action uses redirection to LTT
*                 (TTI base routing), then the LTT index written in the PCL/TTI action is
*                  restricted only to column 0 of the LTT row.
*       NOTE: - Since LTT entries can be used also for Router TCAM routing, this API is not
*               affected directly by this errata. However when the LTT should be pointed from
*               PCL action or LTT action, it should be set at column 0.
*             - The API affected by this errata are PCL rule set, PCL action update,
*               TTI rule set and TTI action update.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLttWrite
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          lttTtiRow,
    IN  GT_U32                          lttTtiColumn,
    IN  CPSS_DXCH_IP_LTT_ENTRY_STC      *lttEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLttWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lttTtiRow, lttTtiColumn, lttEntryPtr));

    rc = internal_cpssDxChIpLttWrite(devNum, lttTtiRow, lttTtiColumn, lttEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lttTtiRow, lttTtiColumn, lttEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpLttRead
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number.
*       lttTtiRow       - the entry's row index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*       lttTtiColumn    - the entry's column index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpLttWrite
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLttRead
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             lttTtiRow,
    IN  GT_U32                             lttTtiColumn,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC         *lttEntryPtr
)
{
    GT_U32      data = 0, data2 = 0;
    GT_STATUS   retVal = GT_OK;
    GT_U32      ipv6MCGroupScopeLevelOffset=0;
    GT_U32      lltEntryHwFormatLength=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    /* ltt size is the same as router TCAM size */
    if (lttTtiRow >= PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        if (lttTtiColumn >= 5)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    default:
        if (lttTtiColumn >= 4)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    }
    CPSS_NULL_PTR_CHECK_MAC(lttEntryPtr);

    /* now get the 22 bits in the hw according the location of the entry in the line */
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* for cheetah2 devices */
    case CPSS_PP_FAMILY_CHEETAH2_E:
        switch(lttTtiColumn)
        {
        case 0:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,0,0,22,&data);
            break;

        case 1:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,0,22,10,&data);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,1,0,12,&data2);
            data |= (data2 << 10);
            break;

        case 2:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,1,12,20,&data);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,2,0,2,&data2);
            data |= (data2 << 20);
            break;

        case 3:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,2,2,22,&data);
            break;

        case 4:
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,2,24,8,&data);
            if (retVal != GT_OK)
                break;
            retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
                                                    lttTtiRow,3,0,14,&data2);
            data |= (data2 << 8);
            break;
        }
        break;

        /* for cheetah3 devices */
    default:
        if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            lltEntryHwFormatLength = 22;
        }
        else
        {
            lltEntryHwFormatLength = 25;
        }

        retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
                                                lttTtiRow,lttTtiColumn,0,lltEntryHwFormatLength,&data);
        break;
    }

    if(data & 0x1)
        lttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_QOS_E;
    else
        lttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;

    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        lttEntryPtr->numOfPaths               = U32_GET_FIELD_MAC(data, 1,3);
        lttEntryPtr->ucRPFCheckEnable         = BOOL2BIT_MAC(U32_GET_FIELD_MAC(data, 4,1));
        lttEntryPtr->sipSaCheckMismatchEnable = BOOL2BIT_MAC(U32_GET_FIELD_MAC(data, 7,1));
        lttEntryPtr->routeEntryBaseIndex      = U32_GET_FIELD_MAC(data, 8,13);
        ipv6MCGroupScopeLevelOffset           = 5;
    }
    else
    {
        lttEntryPtr->numOfPaths               = U32_GET_FIELD_MAC(data, 1,6);
        lttEntryPtr->ucRPFCheckEnable         = BOOL2BIT_MAC(U32_GET_FIELD_MAC(data, 7,1));
        lttEntryPtr->sipSaCheckMismatchEnable = BOOL2BIT_MAC(U32_GET_FIELD_MAC(data, 10,1));
        lttEntryPtr->routeEntryBaseIndex      = U32_GET_FIELD_MAC(data,11,13);
        ipv6MCGroupScopeLevelOffset           = 8;

    }

    switch(U32_GET_FIELD_MAC(data,ipv6MCGroupScopeLevelOffset,2))
    {
    case 0:
        lttEntryPtr->ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
        break;
    case 1:
        lttEntryPtr->ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E;
        break;
    case 2:
        lttEntryPtr->ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E;
        break;
    case 3:
        lttEntryPtr->ipv6MCGroupScopeLevel = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
        break;
    default:
        break;
    }

    return (retVal);
}

/*******************************************************************************
* cpssDxChIpLttRead
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number.
*       lttTtiRow       - the entry's row index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*       lttTtiColumn    - the entry's column index in LTT table (equivalent to
*                         the router tcam entry it is attached to)
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpLttWrite
*
*******************************************************************************/
GT_STATUS cpssDxChIpLttRead
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             lttTtiRow,
    IN  GT_U32                             lttTtiColumn,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC         *lttEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLttRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lttTtiRow, lttTtiColumn, lttEntryPtr));

    rc = internal_cpssDxChIpLttRead(devNum, lttTtiRow, lttTtiColumn, lttEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lttTtiRow, lttTtiColumn, lttEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to write the array
*       routeEntriesArray     - the uc route entries array
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to write)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case uRPF ECMP/QOS is globally enabled, then for ECMP/QOS block
*       with x route entries, additional route entry should be included after
*       the block with the uRPF route entry information.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpUcRouteEntriesWrite
(
    IN GT_U8                           devNum,
    IN GT_U32                          baseRouteEntryIndex,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 i;
    GT_U32 hwData[4];
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntriesArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (baseRouteEntryIndex >= fineTuningPtr->tableSize.routerNextHop)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    if((baseRouteEntryIndex + numOfRouteEntries) > fineTuningPtr->tableSize.routerNextHop)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    retVal = prvCpssDxChIpUcRouteEntriesCheck(devNum,routeEntriesArray,numOfRouteEntries);
    if(retVal != GT_OK)
        return retVal;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E;
    }
    else
    {
        tableType = PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E;
    }
    for (i = 0; i < numOfRouteEntries ; i++)
    {
        /* Convert route entry to HW format */
        /* currently only regular route entry is supported */
        retVal = prvCpssDxChIpConvertUcEntry2HwFormat(devNum,&routeEntriesArray[i],hwData);
        if(retVal != GT_OK)
            return retVal;

        /* Write to hw table */
        retVal =
            prvCpssDxChWriteTableEntry(devNum,
                                       tableType,
                                       baseRouteEntryIndex + i,
                                       hwData);
        if(retVal != GT_OK)
            return retVal;
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to write the array
*       routeEntriesArray     - the uc route entries array
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to write)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case uRPF ECMP/QOS is globally enabled, then for ECMP/QOS block
*       with x route entries, additional route entry should be included after
*       the block with the uRPF route entry information.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRouteEntriesWrite
(
    IN GT_U8                           devNum,
    IN GT_U32                          baseRouteEntryIndex,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpUcRouteEntriesWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, baseRouteEntryIndex, routeEntriesArray, numOfRouteEntries));

    rc = internal_cpssDxChIpUcRouteEntriesWrite(devNum, baseRouteEntryIndex, routeEntriesArray, numOfRouteEntries);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, baseRouteEntryIndex, routeEntriesArray, numOfRouteEntries));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Reads an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to start reading
*       routeEntriesArray     - the uc route entries array, for each element
*                               set the uc route entry type to determine how
*                               entry data will be interpreted
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to read)
*
*
* OUTPUTS:
*       routeEntriesArray     - the uc route entries array read
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_BAD_PTR     - one of the parameters is NULL pointer
*       GT_BAD_STATE   - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpUcRouteEntriesWrite
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpUcRouteEntriesRead
(
    IN    GT_U8                             devNum,
    IN    GT_U32                            baseRouteEntryIndex,
    INOUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC   *routeEntriesArray,
    IN    GT_U32                            numOfRouteEntries
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 i;
    GT_U32 hwData[4];
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntriesArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (baseRouteEntryIndex >= fineTuningPtr->tableSize.routerNextHop)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    if((baseRouteEntryIndex + numOfRouteEntries) > fineTuningPtr->tableSize.routerNextHop)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E;
    }
    else
    {
        tableType = PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E;
    }
    /* Convert route entry to HW format */
    for (i = 0; i < numOfRouteEntries ; i++)
    {
        retVal =
            prvCpssDxChReadTableEntry(devNum,
                                      tableType,
                                      baseRouteEntryIndex + i,
                                      hwData);

        if(retVal != GT_OK)
            return retVal;

        retVal =
            prvCpssDxChIpConvertHwFormat2UcEntry(devNum,hwData,&routeEntriesArray[i]);

        if(retVal != GT_OK)
            return retVal;

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Reads an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to start reading
*       routeEntriesArray     - the uc route entries array, for each element
*                               set the uc route entry type to determine how
*                               entry data will be interpreted
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to read)
*
*
* OUTPUTS:
*       routeEntriesArray     - the uc route entries array read
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_BAD_PTR     - one of the parameters is NULL pointer
*       GT_BAD_STATE   - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See cpssDxChIpUcRouteEntriesWrite
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpUcRouteEntriesRead
(
    IN    GT_U8                             devNum,
    IN    GT_U32                            baseRouteEntryIndex,
    INOUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC   *routeEntriesArray,
    IN    GT_U32                            numOfRouteEntries
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpUcRouteEntriesRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, baseRouteEntryIndex, routeEntriesArray, numOfRouteEntries));

    rc = internal_cpssDxChIpUcRouteEntriesRead(devNum, baseRouteEntryIndex, routeEntriesArray, numOfRouteEntries);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, baseRouteEntryIndex, routeEntriesArray, numOfRouteEntries));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpMcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes a MC route entry to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the Index in the Route entries table
*       routeEntryPtr   - the MC route entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpMcRouteEntriesWrite
(
    IN GT_U8                           devNum,
    IN GT_U32                          routeEntryIndex,
    IN CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
)
{
    GT_U32 hwData[4];
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routeEntryIndex >= fineTuningPtr->tableSize.routerNextHop)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    retVal = prvCpssDxChIpMcRouteEntryCheck(devNum,routeEntryPtr);
    if(retVal != GT_OK)
        return retVal;

    /* Convert route entry to HW format */
    retVal = prvCpssDxChIpConvertMcEntry2HwFormat(devNum,routeEntryPtr,hwData);
    if(retVal != GT_OK)
        return retVal;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E;
    }
    else
    {
        tableType = PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E;
    }

    /* Write to hw table */
    retVal = prvCpssDxChWriteTableEntry(devNum,
                                        tableType,
                                        routeEntryIndex,
                                        hwData);

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes a MC route entry to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the Index in the Route entries table
*       routeEntryPtr   - the MC route entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMcRouteEntriesWrite
(
    IN GT_U8                           devNum,
    IN GT_U32                          routeEntryIndex,
    IN CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpMcRouteEntriesWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routeEntryIndex, routeEntryPtr));

    rc = internal_cpssDxChIpMcRouteEntriesWrite(devNum, routeEntryIndex, routeEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routeEntryIndex, routeEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpMcRouteEntriesRead
*
* DESCRIPTION:
*    Reads a MC route entry from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpMcRouteEntriesRead
(
    IN GT_U8                            devNum,
    IN GT_U32                           routeEntryIndex,
    OUT CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
)
{
    GT_U32 hwData[4];
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routeEntryIndex >= fineTuningPtr->tableSize.routerNextHop)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E;
    }
    else
    {
        tableType = PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E;
    }

    /* Write to hw table */
    retVal = prvCpssDxChReadTableEntry(devNum,
                                       tableType,
                                       routeEntryIndex,
                                       hwData);
    if(retVal != GT_OK)
        return retVal;

    /* Convert route entry to HW format */
    retVal = prvCpssDxChIpConvertHwFormat2McEntry(devNum,routeEntryPtr,hwData);

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMcRouteEntriesRead
*
* DESCRIPTION:
*    Reads a MC route entry from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMcRouteEntriesRead
(
    IN GT_U8                            devNum,
    IN GT_U32                           routeEntryIndex,
    OUT CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpMcRouteEntriesRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routeEntryIndex, routeEntryPtr));

    rc = internal_cpssDxChIpMcRouteEntriesRead(devNum, routeEntryIndex, routeEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routeEntryIndex, routeEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
*
* DESCRIPTION:
*    set router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*                                             bits.
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
(
    IN  GT_U8   devNum,
    IN  GT_U32  routerNextHopTableAgeBitsEntryIndex,
    IN  GT_U32  routerNextHopTableAgeBitsEntry
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   retVal = GT_OK;
    GT_U32      hwStep;         /* hw index step */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* the number of age bits is same as the number of route */
    /* entries, and each age bits entry holds 32 age bits    */
    if (routerNextHopTableAgeBitsEntryIndex >= fineTuningPtr->tableSize.routerNextHop / 32)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        retVal = prvCpssDxChWriteTableEntry(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E,
                                            routerNextHopTableAgeBitsEntryIndex,
                                            &routerNextHopTableAgeBitsEntry);
    }
    else
    {
        switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
        case CPSS_PP_FAMILY_CHEETAH2_E:
            hwStep = 0x10;
            break;
        default:
            hwStep = 0x04;
            break;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ucMcRouteEntriesAgeBitsBase +
                  routerNextHopTableAgeBitsEntryIndex * hwStep;

        retVal = prvCpssHwPpWriteRegister(devNum, regAddr, routerNextHopTableAgeBitsEntry);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
*
* DESCRIPTION:
*    set router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*                                             bits.
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
(
    IN  GT_U8   devNum,
    IN  GT_U32  routerNextHopTableAgeBitsEntryIndex,
    IN  GT_U32  routerNextHopTableAgeBitsEntry
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpRouterNextHopTableAgeBitsEntryWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerNextHopTableAgeBitsEntryIndex, routerNextHopTableAgeBitsEntry));

    rc = internal_cpssDxChIpRouterNextHopTableAgeBitsEntryWrite(devNum, routerNextHopTableAgeBitsEntryIndex, routerNextHopTableAgeBitsEntry);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerNextHopTableAgeBitsEntryIndex, routerNextHopTableAgeBitsEntry));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpRouterNextHopTableAgeBitsEntryRead
*
* DESCRIPTION:
*     read router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
* OUTPUTS:
*       routerNextHopTableAgeBitsEntryPtr   - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpRouterNextHopTableAgeBitsEntryRead
(
    IN  GT_U8   devNum,
    IN  GT_U32  routerNextHopTableAgeBitsEntryIndex,
    OUT GT_U32  *routerNextHopTableAgeBitsEntryPtr
)
{
    GT_U32      regAddr = 0;        /* register address */
    GT_STATUS   retVal = GT_OK;
    GT_U32      hwStep;         /* hw index step */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32      portGroupId;    /* port group Id */
    GT_U32      tempRouterNextHopTableAgeBitsEntry=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routerNextHopTableAgeBitsEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* the number of age bits is same as the number of route */
    /* entries, and each age bits entry holds 32 age bits    */
    if (routerNextHopTableAgeBitsEntryIndex >= fineTuningPtr->tableSize.routerNextHop / 32)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    *routerNextHopTableAgeBitsEntryPtr=0;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
        case CPSS_PP_FAMILY_CHEETAH2_E:
            hwStep = 0x10;
            break;
        default:
            hwStep = 0x04;
            break;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ucMcRouteEntriesAgeBitsBase +
                  routerNextHopTableAgeBitsEntryIndex * hwStep;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            retVal = prvCpssDxChPortGroupReadTableEntry(devNum, portGroupId,
                                                        PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E,
                                                        routerNextHopTableAgeBitsEntryIndex,
                                                        &tempRouterNextHopTableAgeBitsEntry);
        }
        else
        {
            retVal = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId, regAddr,
                                                         &tempRouterNextHopTableAgeBitsEntry);
        }
        if (retVal != GT_OK)
            return retVal;

        (*routerNextHopTableAgeBitsEntryPtr) |= tempRouterNextHopTableAgeBitsEntry;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return retVal;
}

/*******************************************************************************
* cpssDxChIpRouterNextHopTableAgeBitsEntryRead
*
* DESCRIPTION:
*     read router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
* OUTPUTS:
*       routerNextHopTableAgeBitsEntryPtr   - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterNextHopTableAgeBitsEntryRead
(
    IN  GT_U8   devNum,
    IN  GT_U32  routerNextHopTableAgeBitsEntryIndex,
    OUT GT_U32  *routerNextHopTableAgeBitsEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpRouterNextHopTableAgeBitsEntryRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerNextHopTableAgeBitsEntryIndex, routerNextHopTableAgeBitsEntryPtr));

    rc = internal_cpssDxChIpRouterNextHopTableAgeBitsEntryRead(devNum, routerNextHopTableAgeBitsEntryIndex, routerNextHopTableAgeBitsEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerNextHopTableAgeBitsEntryIndex, routerNextHopTableAgeBitsEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpMLLPairWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) pair entry to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllPairWriteForm.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       MLL entries are two words long.
*       In Lion and above devices the whole entry is written when last word is
*       set.
*       In PP prior to lion The atomic hw write operation is done on one word.
*       Writing order of MLL parameters could be significant as some parameters
*       in word[1],[3] depend on parameters in word[0],[2].
*       Erroneous handling of mllRPFFailCommand/ nextHopTunnelPointer may
*       result with bad traffic. nextPointer may result with PP infinite loop.
*       cpssDxChIpMLLPairWrite handles the nextPointer parameter in the
*       following way:
*       if (last == 1) first set word[0] or word[2].
*       if (last == 0) first set word[3].
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpMLLPairWrite
(
    IN GT_U8                                        devNum,
    IN GT_U32                                       mllPairEntryIndex,
    IN CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairWriteForm,
    IN CPSS_DXCH_IP_MLL_PAIR_STC                    *mllPairEntryPtr
)
{
    GT_U32    hwAddr;
    GT_U32    hwData[CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD];
    GT_U32    lion3HwData[BITS_TO_WORDS_MAC(MAX_IP_MLL_ENTRY_BITS_SIZE_CNS)];
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_BOOL   isWriteOnLastWord = GT_FALSE;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(mllPairEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if(mllPairEntryIndex >= fineTuningPtr->tableSize.mllPairs)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if((mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E) ||
           (mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E))
        {
            retVal = prvCpssDxChReadTableEntry(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E,
                                            mllPairEntryIndex,
                                            lion3HwData);
            if(retVal != GT_OK)
            {
                return retVal;
            }
        }

        /* update the HW entry */
        retVal = lion3IpMllStruct2HwFormat(devNum,mllPairWriteForm,mllPairEntryPtr,lion3HwData);
        if(retVal != GT_OK)
        {
            return retVal;
        }

        retVal = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E,
                                        mllPairEntryIndex,
                                        lion3HwData);
        return retVal;
    }

    /*in here we need to add all devices where entry is written on last word*/
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        isWriteOnLastWord = GT_TRUE;
    }

    if((mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E))
    {
        switch(mllPairEntryPtr->firstMllNode.mllRPFFailCommand)
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            case CPSS_PACKET_CMD_DROP_HARD_E:
            case CPSS_PACKET_CMD_DROP_SOFT_E:
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            case CPSS_PACKET_CMD_BRIDGE_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if((mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E))
    {
        switch(mllPairEntryPtr->secondMllNode.mllRPFFailCommand)
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            case CPSS_PACKET_CMD_DROP_HARD_E:
            case CPSS_PACKET_CMD_DROP_SOFT_E:
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            case CPSS_PACKET_CMD_BRIDGE_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    hwAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllTableBase +
             mllPairEntryIndex * 0x10;

    /*in case we are writing either first or second mll we need to read word 1 for tunnel start ptr*/
    if((mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E))
    {
        retVal = prvCpssHwPpReadRam(devNum,
                                       hwAddr + 4,
                                       CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                       &hwData[1]);
    }

    retVal = prvCpssDxChIpMllStruct2HwFormat(devNum,mllPairWriteForm,mllPairEntryPtr,hwData);
    if(retVal != GT_OK)
        return retVal;


    if (isWriteOnLastWord)
    {
        switch(mllPairWriteForm)
        {
        case CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E:
            /*we need to modify only the first mll and we read the last word
              so it will not change*/
            retVal = prvCpssHwPpReadRam(devNum,
                                           hwAddr + 12,
                                           CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                           &hwData[3]);

            retVal = prvCpssHwPpWriteRam(devNum,
                                            hwAddr,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                            &hwData[0]);

            retVal = prvCpssHwPpWriteRam(devNum,
                                            hwAddr + 12,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                            &hwData[3]);
            break;

        case CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E:
            /*we need to write word 1 for second mll ts bits*/
            retVal = prvCpssHwPpWriteRam(devNum,
                                            hwAddr + 4,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                            &hwData[1]);

            /*we are writing both words and overwriting the last word*/
            retVal = prvCpssHwPpWriteRam(devNum,
                                            hwAddr + 8,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                            &hwData[2]);
            break;

        case CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E:
            retVal = prvCpssHwPpWriteRam(devNum,
                                            hwAddr,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD,
                                            &hwData[0]);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        switch(mllPairWriteForm)
        {
        case CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E:
            if(mllPairEntryPtr->firstMllNode.last)
            {
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                                &hwData[0]);
            }
            else
            {
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr + 4,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[1]);
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[0]);
            }
            break;

        case CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E:
            if(mllPairEntryPtr->secondMllNode.last)
            {
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr + 8,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                                &hwData[2]);
            }
            else
            {
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr + 12,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[3]);
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr + 8,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[2]);
            }
            /*we need to write word 1 anyhow for tunnel ptr bits*/
            retVal = prvCpssHwPpWriteRam(devNum,
                                            hwAddr + 4,
                                            CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                            &hwData[1]);
            break;

        case CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E:
            if(mllPairEntryPtr->firstMllNode.last)
            {
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD,
                                                &hwData[0]);
            }
            else if(mllPairEntryPtr->secondMllNode.last)
            {
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr + 8,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                                &hwData[2]);
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/2,
                                                &hwData[0]);
            }
            else
            {
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr + 12,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[3]);
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr + 8,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[2]);
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr + 4,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[1]);
                retVal = prvCpssHwPpWriteRam(devNum,
                                                hwAddr,
                                                CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD/4,
                                                &hwData[0]);
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMLLPairWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) pair entry to hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllPairWriteForm.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       MLL entries are two words long.
*       In Lion and above devices the whole entry is written when last word is
*       set.
*       In PP prior to lion The atomic hw write operation is done on one word.
*       Writing order of MLL parameters could be significant as some parameters
*       in word[1],[3] depend on parameters in word[0],[2].
*       Erroneous handling of mllRPFFailCommand/ nextHopTunnelPointer may
*       result with bad traffic. nextPointer may result with PP infinite loop.
*       cpssDxChIpMLLPairWrite handles the nextPointer parameter in the
*       following way:
*       if (last == 1) first set word[0] or word[2].
*       if (last == 0) first set word[3].
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLPairWrite
(
    IN GT_U8                                        devNum,
    IN GT_U32                                       mllPairEntryIndex,
    IN CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairWriteForm,
    IN CPSS_DXCH_IP_MLL_PAIR_STC                    *mllPairEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpMLLPairWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mllPairEntryIndex, mllPairWriteForm, mllPairEntryPtr));

    rc = internal_cpssDxChIpMLLPairWrite(devNum, mllPairEntryIndex, mllPairWriteForm, mllPairEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mllPairEntryIndex, mllPairWriteForm, mllPairEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpMLLPairRead
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      mllPairEntryIndex,
    IN CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairReadForm,
    OUT CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr
)
{
    GT_U32    hwAddr;
    GT_U32    hwData[CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD];
    GT_U32    lion3HwData[BITS_TO_WORDS_MAC(MAX_IP_MLL_ENTRY_BITS_SIZE_CNS)];
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(mllPairEntryPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if(mllPairEntryIndex >= fineTuningPtr->tableSize.mllPairs)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        retVal = prvCpssDxChReadTableEntry(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E,
                                        mllPairEntryIndex,
                                        lion3HwData);
        if(retVal != GT_OK)
        {
            return retVal;
        }

        /* convert HW entry to SW format*/
        retVal = lion3IpMllHw2StructFormat(devNum,mllPairReadForm,mllPairEntryPtr,lion3HwData);
        return retVal;
    }

    hwAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllTableBase +
             mllPairEntryIndex * 0x10;

    retVal = prvCpssHwPpReadRam(devNum, hwAddr,
                                    CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD, hwData);
    if(retVal != GT_OK)
        return retVal;

    /* read generic data */
    switch(mllPairReadForm)
    {
         case CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E:
            retVal = prvCpssDxChIpHwFormat2MllStruct(devNum,&hwData[0], GT_TRUE, &mllPairEntryPtr->firstMllNode);
            if(retVal != GT_OK)
                return retVal;
            break;

        case CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E:
            retVal = prvCpssDxChIpHwFormat2MllStruct(devNum,&hwData[0], GT_FALSE, &mllPairEntryPtr->secondMllNode);
            if(retVal != GT_OK)
                return retVal;
            break;

        case CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E:
            retVal = prvCpssDxChIpHwFormat2MllStruct(devNum,&hwData[0], GT_TRUE, &mllPairEntryPtr->firstMllNode);
            if(retVal != GT_OK)
                return retVal;

            retVal = prvCpssDxChIpHwFormat2MllStruct(devNum,&hwData[0], GT_FALSE, &mllPairEntryPtr->secondMllNode);
            if(retVal != GT_OK)
                return retVal;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((mllPairReadForm == CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E) ||
        (mllPairReadForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E))
    {
        /* Next MLL Pointer */
        mllPairEntryPtr->nextPointer = (GT_U16)U32_GET_FIELD_MAC(hwData[3], 20, 12);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLPairRead
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      mllPairEntryIndex,
    IN CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairReadForm,
    OUT CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpMLLPairRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mllPairEntryIndex, mllPairReadForm, mllPairEntryPtr));

    rc = internal_cpssDxChIpMLLPairRead(devNum, mllPairEntryIndex, mllPairReadForm, mllPairEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mllPairEntryIndex, mllPairReadForm, mllPairEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpMLLLastBitWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) Last bit to the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       lastBit           - the Last bit
*       mllEntryPart      - to which part of the mll Pair to write the Last bit
*                           for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllEntryPart.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpMLLLastBitWrite
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           mllPairEntryIndex,
    IN GT_BOOL                                          lastBit,
    IN CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT    mllEntryPart
)
{
    GT_U32      hwAddr;
    GT_U32      hwData[CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD];
    GT_STATUS   retVal = GT_OK;
    GT_U32      hwWord;
    GT_U32      offsetInWord;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if(mllPairEntryIndex >= fineTuningPtr->tableSize.mllPairs)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch(mllEntryPart)
        {
            case CPSS_DXCH_IP_MLL_PAIR_WRITE_FIRST_LAST_BIT_E:
            case CPSS_DXCH_IP_MLL_PAIR_WRITE_SECOND_LAST_BIT_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        retVal = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E,
                                        mllPairEntryIndex,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        (mllEntryPart == CPSS_DXCH_IP_MLL_PAIR_WRITE_SECOND_LAST_BIT_E) ?
                                            LION3_IP_MLL_TABLE_FIELDS_LAST_1_E :
                                            LION3_IP_MLL_TABLE_FIELDS_LAST_0_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        BOOL2BIT_MAC(lastBit));
        return retVal;
    }

    hwAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipMtRegs.mllTableBase +
             mllPairEntryIndex * 0x10;

    /* here we need to add all devices where entry write is triggered upon writing last word */
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* read the table entry from hardware */
        retVal = prvCpssHwPpReadRam(devNum,hwAddr,CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD,hwData);
        if (retVal != GT_OK)
            return retVal;

        switch(mllEntryPart)
        {
        case CPSS_DXCH_IP_MLL_PAIR_WRITE_FIRST_LAST_BIT_E:
            hwWord = 0;
            offsetInWord = 0;
            break;
        case CPSS_DXCH_IP_MLL_PAIR_WRITE_SECOND_LAST_BIT_E:
            hwWord = 2;
            offsetInWord = 0;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        hwData[hwWord] &= (~(1 << offsetInWord));
        hwData[hwWord] |= (BOOL2BIT_MAC(lastBit) << offsetInWord);

        /* write the updated entry to the hardware */
        retVal = prvCpssHwPpWriteRam(devNum,hwAddr,CPSS_DXCH_SIZE_OF_MLL_NODE_IN_WORD,hwData);
        if (retVal != GT_OK)
            return retVal;
    }
    else
    {
        switch(mllEntryPart)
        {
        case CPSS_DXCH_IP_MLL_PAIR_WRITE_FIRST_LAST_BIT_E:
            retVal = prvCpssHwPpSetRegField(devNum,hwAddr,0,1,BOOL2BIT_MAC(lastBit));
            break;
        case CPSS_DXCH_IP_MLL_PAIR_WRITE_SECOND_LAST_BIT_E:
            retVal = prvCpssHwPpSetRegField(devNum,hwAddr+8,0,1,BOOL2BIT_MAC(lastBit));
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpMLLLastBitWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) Last bit to the hw.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       lastBit           - the Last bit
*       mllEntryPart      - to which part of the mll Pair to write the Last bit
*                           for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllEntryPart.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpMLLLastBitWrite
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           mllPairEntryIndex,
    IN GT_BOOL                                          lastBit,
    IN CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT    mllEntryPart
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpMLLLastBitWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mllPairEntryIndex, lastBit, mllEntryPart));

    rc = internal_cpssDxChIpMLLLastBitWrite(devNum, mllPairEntryIndex, lastBit, mllEntryPart);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mllPairEntryIndex, lastBit, mllEntryPart));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpRouterArpAddrWrite
*
* DESCRIPTION:
*    write a ARP MAC address to the router ARP / Tunnel start / NAT table (for NAT capable devices)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later
*                         in the UC Route entry Arp nextHopARPPointer
*                         field)
*       arpMacAddrPtr   - the ARP MAC address to write
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on devNum not active.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       pay attention that the router ARP table is shared with a tunnel start
*       table and NAT table (for NAT capable devices),
*       each tunnel start / NAT (for NAT capable devices) entry takes 4 ARP Address.
*       that is Tunnel start <n> / NAT (for NAT capable devices) <n>
*       takes ARP addresses <4n>,<4n+1>,<4n+2>,<4n+3>
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpRouterArpAddrWrite
(
    IN GT_U8                         devNum,
    IN GT_U32                        routerArpIndex,
    IN GT_ETHERADDR                  *arpMacAddrPtr
)
{
    GT_U32  hwData[2];
    GT_U32  retVal;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(arpMacAddrPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerArpIndex >= fineTuningPtr->tableSize.routerArp)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    /* convert the ARP MAC address to hw format */
    hwData[0] =  arpMacAddrPtr->arEther[5] |
                 (arpMacAddrPtr->arEther[4] << 8) |
                 (arpMacAddrPtr->arEther[3] << 16)|
                 (arpMacAddrPtr->arEther[2] << 24);
    hwData[1] =  arpMacAddrPtr->arEther[1] |
                 (arpMacAddrPtr->arEther[0] << 8);

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:
        retVal = prvCpssDxChWriteTableEntry(devNum,
                                            PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
                                            routerArpIndex,hwData);

        break;
    default:
        retVal = prvCpssDxChHwHaTunnelStartArpNatTableEntryWrite(devNum,
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ARP_E,
                    routerArpIndex,
                    hwData);
        break;
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpRouterArpAddrWrite
*
* DESCRIPTION:
*    write a ARP MAC address to the router ARP / Tunnel start / NAT table (for NAT capable devices)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later
*                         in the UC Route entry Arp nextHopARPPointer
*                         field)
*       arpMacAddrPtr   - the ARP MAC address to write
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on devNum not active.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       pay attention that the router ARP table is shared with a tunnel start
*       table and NAT table (for NAT capable devices),
*       each tunnel start / NAT (for NAT capable devices) entry takes 4 ARP Address.
*       that is Tunnel start <n> / NAT (for NAT capable devices) <n>
*       takes ARP addresses <4n>,<4n+1>,<4n+2>,<4n+3>
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterArpAddrWrite
(
    IN GT_U8                         devNum,
    IN GT_U32                        routerArpIndex,
    IN GT_ETHERADDR                  *arpMacAddrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpRouterArpAddrWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerArpIndex, arpMacAddrPtr));

    rc = internal_cpssDxChIpRouterArpAddrWrite(devNum, routerArpIndex, arpMacAddrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerArpIndex, arpMacAddrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpRouterArpAddrRead
*
* DESCRIPTION:
*    read a ARP MAC address from the router ARP / Tunnel start / NAT table (for NAT capable devices)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later in the
*                         UC Route entry Arp nextHopARPPointer field)
*
*
* OUTPUTS:
*       arpMacAddrPtr   - the ARP MAC address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on devNum not active.
*       GT_OUT_OF_RANGE          - on routerArpIndex bigger then 4095 or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see cpssDxChIpRouterArpAddrWrite
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpRouterArpAddrRead
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       routerArpIndex,
    OUT GT_ETHERADDR                 *arpMacAddrPtr
)
{
    GT_U32  hwData[2];
    GT_U32  retVal;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(arpMacAddrPtr);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if (routerArpIndex >= fineTuningPtr->tableSize.routerArp)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:

        retVal = prvCpssDxChReadTableEntry(devNum,
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
                                           routerArpIndex,hwData);

        break;
    default:
        retVal = prvCpssDxChHwHaTunnelStartArpNatTableEntryRead(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ARP_E,
                                            routerArpIndex,
                                            hwData);
        break;
    }

    arpMacAddrPtr->arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],0,8);
    arpMacAddrPtr->arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],8,8);
    arpMacAddrPtr->arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],16,8);
    arpMacAddrPtr->arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwData[0],24,8);

    arpMacAddrPtr->arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],0,8);
    arpMacAddrPtr->arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwData[1],8,8);

    return retVal;
}

/*******************************************************************************
* cpssDxChIpRouterArpAddrRead
*
* DESCRIPTION:
*    read a ARP MAC address from the router ARP / Tunnel start / NAT table (for NAT capable devices)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later in the
*                         UC Route entry Arp nextHopARPPointer field)
*
*
* OUTPUTS:
*       arpMacAddrPtr   - the ARP MAC address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on devNum not active.
*       GT_OUT_OF_RANGE          - on routerArpIndex bigger then 4095 or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see cpssDxChIpRouterArpAddrWrite
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpRouterArpAddrRead
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       routerArpIndex,
    OUT GT_ETHERADDR                 *arpMacAddrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpRouterArpAddrRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, routerArpIndex, arpMacAddrPtr));

    rc = internal_cpssDxChIpRouterArpAddrRead(devNum, routerArpIndex, arpMacAddrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, routerArpIndex, arpMacAddrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChIpConvertUcEntry2HwFormat
*
* DESCRIPTION:
*       This function converts a given ip uc entry to the HW format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       ipUcRouteEntryPtr - The entry to be converted.
*
* OUTPUTS:
*       hwDataPtr  - The entry in the HW format representation.
*
* RETURNS:
*       GT_BAD_PARAM    - on invalid parameter.
*       GT_OK           - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpConvertUcEntry2HwFormat
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *ipUcRouteEntryPtr,
    OUT GT_U32 *hwDataPtr
)
{
    GT_U8         packetCmd;        /* packet command */
    GT_U8         cpuCodeIdx;       /* cpu code index */
    GT_U8         modifyDscp;       /* modify packet Dscp command */
    GT_U8         modifyUp;         /* modify packet user priority field */
    GT_HW_DEV_NUM targHwDev;        /* target HW device */
    GT_PORT_NUM   targPort;         /* target port   */
    GT_U8         counterSet;
    GT_U32        value;            /* used for Lion3 macro */
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC  *routeEntry = NULL;
    GT_BOOL       vid1Assignment;
    GT_STATUS     rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* clear hw data */
    cpssOsMemSet(hwDataPtr, 0, sizeof(GT_U32) * 4);

    switch (ipUcRouteEntryPtr->type)
    {
    case CPSS_DXCH_IP_UC_ROUTE_ENTRY_E:

        routeEntry = &ipUcRouteEntryPtr->entry.regularEntry;   /* to make the code more readable */

        switch(routeEntry->cmd)
        {
            case CPSS_PACKET_CMD_ROUTE_E:
                packetCmd= 0;
                break;
            case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
                packetCmd= 1;
                break;
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                packetCmd= 2;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                packetCmd= 3;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                packetCmd= 4;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        switch(routeEntry->cpuCodeIdx)
        {
            case CPSS_DXCH_IP_CPU_CODE_IDX_0_E:
                cpuCodeIdx= 0;
                break;
            case CPSS_DXCH_IP_CPU_CODE_IDX_1_E:
                cpuCodeIdx= 1;
                break;
            case CPSS_DXCH_IP_CPU_CODE_IDX_2_E:
                cpuCodeIdx= 2;
                break;
            case CPSS_DXCH_IP_CPU_CODE_IDX_3_E:
                cpuCodeIdx= 3;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        switch(routeEntry->modifyDscp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
                modifyDscp= 0;
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                modifyDscp= 2;
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                modifyDscp= 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        switch(routeEntry->modifyUp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
                modifyUp= 0;
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                modifyUp= 2;
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                modifyUp= 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        counterSet = (GT_U8)((routeEntry->countSet == CPSS_IP_CNT_NO_SET_E) ?
                                            7 : routeEntry->countSet);

        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            value = packetCmd;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COMMAND_E);

            value = routeEntry->ttlHopLimDecOptionsExtChkByPass;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E);

            value = cpuCodeIdx;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_CPU_CODE_INDEX_E);

            if (routeEntry->ingressMirror)
            {
                value = routeEntry->ingressMirrorToAnalyzerIndex + 1;
            }
            else
            {
                value = 0;
            }
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E);

            /* Get VID1 assignment enabling status */
            rc = cpssDxChIpUcRoutingVid1AssignEnableGet(devNum, &vid1Assignment);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (vid1Assignment == GT_TRUE)
            {
                value = routeEntry->nextHopVlanId1;
                LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_VID1_E);
            }
            else
            {
                value = routeEntry->qosPrecedence;
                LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E);

                value = routeEntry->qosProfileMarkingEnable;
                LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E);

                value = routeEntry->qosProfileIndex;
                LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_INDEX_E);

                value = modifyDscp;
                LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_DSCP_E);

                value = modifyUp;
                LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E);
            }

            value = routeEntry->nextHopVlanId;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_EVLAN_E);

            value = routeEntry->ttlHopLimitDecEnable;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E);

            value = routeEntry->siteId;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_DEST_SITE_ID_E);

            value = routeEntry->scopeCheckingEnable;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_SCOPE_CHECK_EN_E);

            value = routeEntry->ICMPRedirectEnable;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ICMP_REDIRECT_EXCEP_MIRROR_E);

            value = counterSet;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COUNTER_SET_INDEX_E);

            value = routeEntry->mtuProfileIndex;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MTU_INDEX_E);

            value = routeEntry->trapMirrorArpBcEnable;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ARP_BC_TRAP_MIRROR_EN_E);

            value = routeEntry->appSpecificCpuCodeEnable;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E);

            value = routeEntry->dipAccessLevel;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DIP_ACCESS_LEVEL_E);

            value = routeEntry->sipAccessLevel;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_SIP_ACCESS_LEVEL_E);

            value = routeEntry->unicastPacketSipFilterEnable;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_SIP_FILTER_EN_E);
        }
        else
        {
            hwDataPtr[0] |= ((packetCmd & 0x7)                                              |
                     ((routeEntry->ttlHopLimDecOptionsExtChkByPass & 0x1) << 3) |
                     ((cpuCodeIdx & 0x3) << 4)                                      |
                     ((routeEntry->ingressMirror & 0x1) << 6)                   |
                     ((routeEntry->qosPrecedence & 0x1) << 7)                   |
                     ((routeEntry->qosProfileMarkingEnable & 0x1) << 8)         |
                     ((routeEntry->qosProfileIndex & 0x7F) << 9)                |
                     ((modifyDscp & 0x3) << 16)                                     |
                     ((modifyUp & 0x3) << 18)                                       |
                     ((routeEntry->nextHopVlanId & 0xFFF) << 20));
            hwDataPtr[1] |= ((routeEntry->ttlHopLimitDecEnable & 0x1) << 18);
            hwDataPtr[2] |= ((routeEntry->siteId & 0x1)                               |
                             ((routeEntry->scopeCheckingEnable & 0x1) << 1)           |
                             ((routeEntry->ICMPRedirectEnable & 0x1) << 2)            |
                             ((counterSet & 0x7) << 3)                                |
                             ((routeEntry->mtuProfileIndex & 0x7) << 6)               |
                             ((routeEntry->trapMirrorArpBcEnable & 0x1) << 9)         |
                             ((routeEntry->appSpecificCpuCodeEnable & 0x1) << 10)     |
                             ((routeEntry->dipAccessLevel & 0x7) << 11)               |
                             ((routeEntry->sipAccessLevel & 0x7) << 14)               |
                             ((routeEntry->unicastPacketSipFilterEnable & 0x1) << 17));
        }

        if ((routeEntry->cmd == CPSS_PACKET_CMD_ROUTE_E) ||
            (routeEntry->cmd == CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E))
        {
            /* outlif */
            if ((routeEntry->nextHopInterface.type == CPSS_INTERFACE_VIDX_E)||
                (routeEntry->nextHopInterface.type == CPSS_INTERFACE_VID_E))
            {
                if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                {
                    value = 1;  /* use VIDX */
                    LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E);

                    if(routeEntry->nextHopInterface.type == CPSS_INTERFACE_VID_E)
                    {
                        value = 0xFFF;
                    }
                    else
                    {
                        value = routeEntry->nextHopInterface.vidx;
                    }
                    LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EVIDX_E);
                }
                else
                {
                    hwDataPtr[1] |= 0x1; /* use VIDX */
                    if(routeEntry->nextHopInterface.type == CPSS_INTERFACE_VID_E)
                    {
                        hwDataPtr[1] |= (0xFFF << 1);
                    }
                    else
                    {
                        hwDataPtr[1] |= (routeEntry->nextHopInterface.vidx & 0xFFF) << 1;
                    }
                }
            }
            else if (routeEntry->nextHopInterface.type == CPSS_INTERFACE_TRUNK_E)
            {
                /* TRUNK */
                if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                {
                    value = 0;  /* use VIDX */
                    LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E);

                    value = 1;  /* target is trunk */
                    LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E);

                    value = routeEntry->nextHopInterface.trunkId;
                    LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_TRUNK_ID_E);
                }
                else
                {
                    hwDataPtr[1] |= (1 << 1) | /* target is trunk */
                        ((routeEntry->nextHopInterface.trunkId & 0x7F) << 6);
                }
            }
            else
            {
                 if (routeEntry->nextHopInterface.type == CPSS_INTERFACE_PORT_E)
                 {
                    /* PORT */
                    targHwDev  = (PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                        routeEntry->nextHopInterface.devPort.hwDevNum,
                        routeEntry->nextHopInterface.devPort.portNum));
                    targPort = (PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                        routeEntry->nextHopInterface.devPort.hwDevNum,
                        routeEntry->nextHopInterface.devPort.portNum));
                    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                    {
                        value = 0;  /* use VIDX */
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E);

                        value = 0;  /* target is trunk */
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E);

                        value = targHwDev;
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_DEV_E);

                        value = targPort;
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_EPORT_E);
                    }
                    else
                    {
                        hwDataPtr[1] |= ((targPort & 0x3F) << 2) | ((targHwDev & 0x1F) << 8);
                    }
                 }
                 else
                 {
                      CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                 }
            }

            if (routeEntry->isTunnelStart == GT_FALSE)  /* Link Layer */
            {
                /* ARP MAC DA Pointer */
                if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                {
                    value = 0;  /* start of tunnel */
                    LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E);

                    value = routeEntry->nextHopARPPointer;
                    LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ARP_PTR_E);
                }
                else
                {
                    hwDataPtr[1] |= ((routeEntry->nextHopARPPointer & 0x1FFF) << 19);
                    hwDataPtr[2] |= (((routeEntry->nextHopARPPointer >> 13) & 0x1) << 19);
                }
            }
            else /* tunnel start */
            {
                if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                {
                    /* for bobcat2 B0 and above NAT is supported */
                    if ((PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))&&
                        (routeEntry->isNat == GT_TRUE))
                    {
                        value = 1;  /* start of tunnel */
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E);

                        value = 1;  /* tunnel start pointer in HW is actually a NAT pointer */
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_TS_IS_NAT_E);

                        value = routeEntry->nextHopNatPointer;
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TUNNEL_PTR_E);

                    }
                    else
                    {
                        value = 1;  /* start of tunnel */
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E);

                        value = routeEntry->nextHopTunnelPointer;
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TUNNEL_PTR_E);

                        /* force the <tunnel type> to be 'other' (not 'Ethernet') */
                        value = 1;  /* <tunnel type> to be 'other' */
                        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_TUNNEL_TYPE_E);
                    }
                }
                else
                {
                    hwDataPtr[1] |=
                        (((routeEntry->isTunnelStart & 0x1) << 17) |
                         (0x1 << 31) | /* <tunnel type> to be 'other' */
                         ((routeEntry->nextHopTunnelPointer & 0xFFF) << 19));
                }
            }
        }

        break;

    case CPSS_DXCH_IP_UC_ECMP_RPF_E:

        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            hwDataPtr[0] |= (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[0]) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[1] << 13) |
                        ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] & 0x3f) << 26);

            hwDataPtr[1] |= ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] & 0x1fc0) >> 6) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[3] << 7) |
                        ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[4] & 0xfff) << 20);

            hwDataPtr[2] |= ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[4] & 0x1000) >> 12) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[5] << 1) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[6] << 14) |
                        ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[7] & 0x1f) << 27);

            hwDataPtr[3] |= ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[7] & 0x1fe0) >> 5);
        }
        else
        {
            hwDataPtr[0] |= (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[0]) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[1] << 12) |
                        ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] & 0xff) << 24);

            hwDataPtr[1] |= ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] & 0xf00) >> 8) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[3] << 4) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[4] << 16) |
                        ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[5] & 0xf) << 28);

            hwDataPtr[2] |= ((ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[5] & 0xff0) >> 4) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[6] << 8) |
                        (ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[7] << 20);

            /* hwDataPtr[3] = 0; */
        }

        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpConvertHwFormat2UcEntry
*
* DESCRIPTION:
*       This function converts a given ip uc entry to the HW format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum             - the device number
*       hwDataPtr          - The entry in the HW format representation.
*       ipUcRouteEntryPtr  - Set the route entry type to determine how hw data will be
*                         interpreted
*
* OUTPUTS:
*       ipUcRouteEntryPtr - The entry to be converted.
*
* RETURNS:
*       GT_BAD_PARAM    - on invalid parameter.
*       GT_OK           - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpConvertHwFormat2UcEntry
(
    IN  GT_U8                               devNum,
    IN  GT_U32 *hwDataPtr,
    INOUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *ipUcRouteEntryPtr
)
{
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC     *regularEntry = NULL;
    GT_U32 value, cmd, cpuCodeIdx, modifyDscp = 0, modifyUp = 0;
    GT_BOOL vid1Assignment = GT_FALSE;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* clear output data */
    cpssOsMemSet(&ipUcRouteEntryPtr->entry, 0, sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT));

    switch (ipUcRouteEntryPtr->type)
    {
    case CPSS_DXCH_IP_UC_ROUTE_ENTRY_E:

        regularEntry = &ipUcRouteEntryPtr->entry.regularEntry; /* to make the code more readable... */

        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COMMAND_E);
            cmd = value;

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_CPU_CODE_INDEX_E);
            cpuCodeIdx = value;

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E);
            regularEntry->ttlHopLimDecOptionsExtChkByPass = BIT2BOOL_MAC(value);

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E);
            if (value)
            {
                regularEntry->ingressMirrorToAnalyzerIndex = value - 1;
                regularEntry->ingressMirror = GT_TRUE;
            }
            else
            {
                regularEntry->ingressMirror = GT_FALSE;
            }

            /* Get VID1 assignment enabling status */
            rc = cpssDxChIpUcRoutingVid1AssignEnableGet(devNum, &vid1Assignment);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (vid1Assignment == GT_TRUE)
            {
                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_VID1_E);
                regularEntry->nextHopVlanId1 = (GT_U16)value;
            }
            else
            {
                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E);
                regularEntry->qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)value;

                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E);
                regularEntry->qosProfileMarkingEnable = BIT2BOOL_MAC(value);

                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_INDEX_E);
                regularEntry->qosProfileIndex = value;

                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_DSCP_E);
                modifyDscp = value;

                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E);
                modifyUp = value;
            }

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_EVLAN_E);
            regularEntry->nextHopVlanId = (GT_U16)value;

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E);
            regularEntry->ttlHopLimitDecEnable = BIT2BOOL_MAC(value);

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_DEST_SITE_ID_E);
            regularEntry->siteId = (CPSS_IP_SITE_ID_ENT)value;

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_SCOPE_CHECK_EN_E);
            regularEntry->scopeCheckingEnable = BIT2BOOL_MAC(value);

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ICMP_REDIRECT_EXCEP_MIRROR_E);
            regularEntry->ICMPRedirectEnable = BIT2BOOL_MAC(value);

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COUNTER_SET_INDEX_E);
            if (value == 7)
            {
                regularEntry->countSet = CPSS_IP_CNT_NO_SET_E;
            }
            else
            {
                regularEntry->countSet = (CPSS_IP_CNT_SET_ENT)value;
            }

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MTU_INDEX_E);
            regularEntry->mtuProfileIndex = value;

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ARP_BC_TRAP_MIRROR_EN_E);
            regularEntry->trapMirrorArpBcEnable = BIT2BOOL_MAC(value);

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E);
            regularEntry->appSpecificCpuCodeEnable = BIT2BOOL_MAC(value);

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DIP_ACCESS_LEVEL_E);
            regularEntry->dipAccessLevel = value;

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_SIP_ACCESS_LEVEL_E);
            regularEntry->sipAccessLevel = value;

            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_SIP_FILTER_EN_E);
            regularEntry->unicastPacketSipFilterEnable = BIT2BOOL_MAC(value);
        }
        else
        {
            cmd = U32_GET_FIELD_MAC(hwDataPtr[0],0,3);
            cpuCodeIdx = U32_GET_FIELD_MAC(hwDataPtr[0],4,2);
            modifyDscp = U32_GET_FIELD_MAC(hwDataPtr[0],16,2);
            modifyUp = U32_GET_FIELD_MAC(hwDataPtr[0],18,2);
            regularEntry->ttlHopLimDecOptionsExtChkByPass = U32_GET_FIELD_MAC(hwDataPtr[0],3,1);
            regularEntry->ingressMirror                   = U32_GET_FIELD_MAC(hwDataPtr[0],6,1);
            regularEntry->qosPrecedence                   = U32_GET_FIELD_MAC(hwDataPtr[0],7,1);
            regularEntry->qosProfileMarkingEnable         = U32_GET_FIELD_MAC(hwDataPtr[0],8,1);
            regularEntry->qosProfileIndex                 = U32_GET_FIELD_MAC(hwDataPtr[0],9,7);
            regularEntry->nextHopVlanId           = (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[0],20,12);
            regularEntry->ttlHopLimitDecEnable = U32_GET_FIELD_MAC(hwDataPtr[1],18,1);
            regularEntry->siteId                       = U32_GET_FIELD_MAC(hwDataPtr[2],0,1);
            regularEntry->scopeCheckingEnable          = U32_GET_FIELD_MAC(hwDataPtr[2],1,1);
            regularEntry->ICMPRedirectEnable           = U32_GET_FIELD_MAC(hwDataPtr[2],2,1);
            regularEntry->countSet                     = U32_GET_FIELD_MAC(hwDataPtr[2],3,3);
            if (regularEntry->countSet == 7)
            {
                regularEntry->countSet = CPSS_IP_CNT_NO_SET_E;
            }
            regularEntry->mtuProfileIndex              = U32_GET_FIELD_MAC(hwDataPtr[2],6,3);
            regularEntry->trapMirrorArpBcEnable        = U32_GET_FIELD_MAC(hwDataPtr[2],9,1);
            regularEntry->appSpecificCpuCodeEnable     = U32_GET_FIELD_MAC(hwDataPtr[2],10,1);
            regularEntry->dipAccessLevel               = U32_GET_FIELD_MAC(hwDataPtr[2],11,3);
            regularEntry->sipAccessLevel               = U32_GET_FIELD_MAC(hwDataPtr[2],14,3);
            regularEntry->unicastPacketSipFilterEnable = U32_GET_FIELD_MAC(hwDataPtr[2],17,1);
        }

        switch(cmd)
        {
            case 0:
                regularEntry->cmd = CPSS_PACKET_CMD_ROUTE_E;
                break;
            case 1:
                regularEntry->cmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
                break;
            case 2:
                regularEntry->cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
                break;
            case 3:
                regularEntry->cmd = CPSS_PACKET_CMD_DROP_HARD_E;
                break;
            case 4:
                regularEntry->cmd = CPSS_PACKET_CMD_DROP_SOFT_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        switch(cpuCodeIdx)
        {
            case 0:
                regularEntry->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_0_E;
                break;
            case 1:
                regularEntry->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_1_E;
                break;
            case 2:
                regularEntry->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_2_E;
                break;
            case 3:
                regularEntry->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_3_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        switch(modifyDscp)
        {
            case 0:
                regularEntry->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
                break;
            case 1:
                regularEntry->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                break;
            case 2:
                regularEntry->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        switch(modifyUp)
        {
            case 0:
                regularEntry->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
                break;
            case 1:
                regularEntry->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                break;
            case 2:
                regularEntry->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        if ((regularEntry->cmd == CPSS_PACKET_CMD_ROUTE_E  ) ||
            (regularEntry->cmd == CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E ))
        {
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E);
                if (value == 1)
                {
                    LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EVIDX_E);
                    if(value==0xFFF)
                    {
                        /* VID */
                        regularEntry->nextHopInterface.type = CPSS_INTERFACE_VID_E;
                        regularEntry->nextHopInterface.vlanId = 0xFFF;
                    }
                    else
                    {
                        /* VIDX */
                        regularEntry->nextHopInterface.type = CPSS_INTERFACE_VIDX_E;
                        regularEntry->nextHopInterface.vidx = (GT_U16)value;
                    }
                }
                else
                {
                    LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E);
                    if (value == 1)
                    {
                        /* TRUNK */
                        regularEntry->nextHopInterface.type = CPSS_INTERFACE_TRUNK_E;
                        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_TRUNK_ID_E);
                        regularEntry->nextHopInterface.trunkId = (GT_TRUNK_ID)value;
                    }
                    else
                    {
                        /* PORT */
                        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_DEV_E);
                        regularEntry->nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)value;
                        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_EPORT_E);
                        regularEntry->nextHopInterface.devPort.portNum = (GT_PORT_NUM)value;
                        PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(regularEntry->nextHopInterface));
                    }
                }

                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E);
                if (value == 1)
                {
                     /* for bobcat2 B0 and above NAT is supported */
                    if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
                    {
                        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_TS_IS_NAT_E);
                        if(value == 1)
                        {
                            regularEntry->isTunnelStart = GT_TRUE;
                            regularEntry->isNat = GT_TRUE;
                            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TUNNEL_PTR_E);
                            regularEntry->nextHopNatPointer = value;
                        }
                        else
                        {
                            regularEntry->isTunnelStart = GT_TRUE;
                            regularEntry->isNat = GT_FALSE;
                            LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TUNNEL_PTR_E);
                            regularEntry->nextHopTunnelPointer = value;
                        }
                    }
                    else
                    {
                        regularEntry->isTunnelStart = GT_TRUE;
                        regularEntry->isNat = GT_FALSE;
                        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TUNNEL_PTR_E);
                        regularEntry->nextHopTunnelPointer = value;
                    }
                }
                else
                {
                    regularEntry->isTunnelStart = GT_FALSE;
                    regularEntry->isNat = GT_FALSE;
                    LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ARP_PTR_E);
                    regularEntry->nextHopARPPointer = value;
                }
            }
            else /* not eArch*/
            {
                if (U32_GET_FIELD_MAC(hwDataPtr[1],0,1) == 1)
                {
                    regularEntry->nextHopInterface.type    = CPSS_INTERFACE_VIDX_E;
                    regularEntry->nextHopInterface.vidx    =
                        (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[1],1,13);
                }
                else if(U32_GET_FIELD_MAC(hwDataPtr[1],1,1) == 1)
                {
                    regularEntry->nextHopInterface.type    = CPSS_INTERFACE_TRUNK_E;
                    regularEntry->nextHopInterface.trunkId =
                        (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[1],6,7);
                }
                else
                {
                    regularEntry->nextHopInterface.type    = CPSS_INTERFACE_PORT_E;
                    regularEntry->nextHopInterface.devPort.hwDevNum =
                        U32_GET_FIELD_MAC(hwDataPtr[1],8,5);
                    regularEntry->nextHopInterface.devPort.portNum =
                        U32_GET_FIELD_MAC(hwDataPtr[1],2,6);
                    PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(regularEntry->nextHopInterface));
                }

                if(U32_GET_FIELD_MAC(hwDataPtr[1],17,1) == 0)
                {
                    regularEntry->isTunnelStart = GT_FALSE;
                        regularEntry->nextHopARPPointer =
                            U32_GET_FIELD_MAC(hwDataPtr[1],19,13) | (U32_GET_FIELD_MAC(hwDataPtr[2],19,1) << 13);
                }
                else
                {
                    regularEntry->isTunnelStart = GT_TRUE;
                    regularEntry->nextHopTunnelPointer = U32_GET_FIELD_MAC(hwDataPtr[1],19,12);
                }
            }
        }

        break;

    case CPSS_DXCH_IP_UC_ECMP_RPF_E:

        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[0] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],0,13));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[1] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],13,13));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],26,6) | (U32_GET_FIELD_MAC(hwDataPtr[1],0,7) << 6));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[3] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[1],7,13));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[4] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[1],20,12) | (U32_GET_FIELD_MAC(hwDataPtr[2],0,1) << 12));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[5] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[2],1,13));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[6] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[2],14,13));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[7] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[2],27,5) | (U32_GET_FIELD_MAC(hwDataPtr[3],0,8) << 5));
        }
        else
        {
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[0] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],0,12));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[1] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],12,12));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[2] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[0],24,8) | (U32_GET_FIELD_MAC(hwDataPtr[1],0,4) << 8));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[3] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[1],4,12));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[4] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[1],16,12));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[5] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[1],28,4) | (U32_GET_FIELD_MAC(hwDataPtr[2],0,8) << 4));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[6] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[2],8,12));
            ipUcRouteEntryPtr->entry.ecmpRpfCheck.vlanArray[7] =
                (GT_U16)(U32_GET_FIELD_MAC(hwDataPtr[2],20,12));
        }
        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpConvertMcEntry2HwFormat
*
* DESCRIPTION:
*       This function converts a given ip mc entry to the HW format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum            - the device number
*       ipMcRouteEntryPtr - The entry to be converted.
*
* OUTPUTS:
*       hwDataPtr  - The entry in the HW format representation.
*
* RETURNS:
*       GT_BAD_PARAM    - on invalid parameter.
*       GT_OK           - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpConvertMcEntry2HwFormat
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *ipMcRouteEntryPtr,
    OUT GT_U32 *hwDataPtr
)
{
    GT_U8         packetCmd;        /* packet command   */
    GT_U8         cpuCodeIdx;       /* cpu code index   */
    GT_U8         rpfFailCmd = 0;       /* RPF Fail command */
    GT_U8         counterSet;       /* counter Set */
    GT_U8         modifyDscp;       /* modify DSCP*/
    GT_U8         modifyUp;         /* modify UP */
    GT_U32        value;            /* used for Lion3 macro */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* clear hw data */
    cpssOsMemSet(hwDataPtr, 0, sizeof(GT_U32) * 4);

    switch(ipMcRouteEntryPtr->cmd)
    {
        case CPSS_PACKET_CMD_ROUTE_E:
            packetCmd = 0;
            break;
        case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            packetCmd = 1;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            packetCmd = 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            packetCmd = 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            packetCmd = 4;
            break;
        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            packetCmd = 5;
            break;
        case CPSS_PACKET_CMD_BRIDGE_E:
            packetCmd = 6;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(ipMcRouteEntryPtr->cpuCodeIdx)
    {
         case CPSS_DXCH_IP_CPU_CODE_IDX_0_E:
             cpuCodeIdx = 0;
             break;
         case CPSS_DXCH_IP_CPU_CODE_IDX_1_E:
             cpuCodeIdx = 1;
             break;
         case CPSS_DXCH_IP_CPU_CODE_IDX_2_E:
             cpuCodeIdx = 2;
             break;
         case CPSS_DXCH_IP_CPU_CODE_IDX_3_E:
             cpuCodeIdx = 3;
             break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(ipMcRouteEntryPtr->modifyDscp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            modifyDscp = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            modifyDscp = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            modifyDscp = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(ipMcRouteEntryPtr->modifyUp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            modifyUp = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            modifyUp = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            modifyUp = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(ipMcRouteEntryPtr->multicastRPFCheckEnable == GT_TRUE)
    {
        switch(ipMcRouteEntryPtr->RPFFailCommand)
        {
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            rpfFailCmd = 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            rpfFailCmd = 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            rpfFailCmd = 4;
            break;
        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            rpfFailCmd = 5;
            break;
        case CPSS_PACKET_CMD_BRIDGE_E:
            rpfFailCmd = 6;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* default value for rpfFailsCmd is CPSS_PACKET_CMD_TRAP_TO_CPU_E */
         rpfFailCmd = 2;
    }

    counterSet = (GT_U8)((ipMcRouteEntryPtr->countSet == CPSS_IP_CNT_NO_SET_E) ?
                                                7 : ipMcRouteEntryPtr->countSet);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        value = packetCmd;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COMMAND_E);

        value = ipMcRouteEntryPtr->ttlHopLimDecOptionsExtChkByPass;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E);

        value = cpuCodeIdx;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_CPU_CODE_INDEX_E);

        if (ipMcRouteEntryPtr->ingressMirror)
        {
            value = ipMcRouteEntryPtr->ingressMirrorToAnalyzerIndex + 1;
        }
        else
        {
            value = 0;
        }
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E);

        value = ipMcRouteEntryPtr->qosPrecedence;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E);

        value = ipMcRouteEntryPtr->qosProfileMarkingEnable;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E);

        value = ipMcRouteEntryPtr->qosProfileIndex;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_INDEX_E);

        value = modifyDscp;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_DSCP_E);

        value = modifyUp;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E);

        if (ipMcRouteEntryPtr->multicastIngressVlanCheck == CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_RPF_CHECK_E)
        {
            value = ipMcRouteEntryPtr->multicastRPFVlan;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_E);
        }
        else    /* CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_BIDIRECTIONAL_TREE_CHECK_E */
        {
            value = ipMcRouteEntryPtr->multicastRPFRoutingSharedTreeIndex;
            LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_MRST_INDEX_E);
        }

        value = (ipMcRouteEntryPtr->externalMLLPointer & 0x1FFF);
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EXTERNAL_MLL_PTR_E);

        value = (ipMcRouteEntryPtr->externalMLLPointer >> 13);
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EXTERNAL_MLL_PTR_MSB_BITS_15_13_E);

        value = rpfFailCmd;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_INGRESS_VLAN_CHECK_FAIL_CMD_E);

        value = ipMcRouteEntryPtr->multicastRPFFailCommandMode;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_RPF_FAIL_CMD_MODE_E);

        value = ipMcRouteEntryPtr->ttlHopLimitDecEnable;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E);

        value = ipMcRouteEntryPtr->internalMLLPointer;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INTERNAL_MLL_PTR_E);

        value = ipMcRouteEntryPtr->siteId;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_DEST_SITE_ID_E);

        value = ipMcRouteEntryPtr->scopeCheckingEnable;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_SCOPE_CHECK_EN_E);

        value = counterSet;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COUNTER_SET_INDEX_E);

        value = ipMcRouteEntryPtr->mtuProfileIndex;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MTU_INDEX_E);

        value = ipMcRouteEntryPtr->appSpecificCpuCodeEnable;
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E);

        if (ipMcRouteEntryPtr->multicastRPFCheckEnable)
        {
            if (ipMcRouteEntryPtr->multicastIngressVlanCheck == CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_RPF_CHECK_E)
            {
                value = 1;
            }
            else    /* CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_BIDIRECTIONAL_TREE_CHECK_E */
            {
                value = 2;
            }
        }
        else
        {
            value = 0;
        }
        LION3_IPVX_FIELD_VALUE_SET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_INGRESS_VLAN_CHECK_E);
    }
    else
    {
        hwDataPtr[0] |= ((packetCmd & 0x7)                                            |
                    ((ipMcRouteEntryPtr->ttlHopLimDecOptionsExtChkByPass & 0x1) << 3)|
                    ((cpuCodeIdx & 0x3) << 4)                                     |
                    ((ipMcRouteEntryPtr->ingressMirror & 0x1) << 6)                  |
                    ((ipMcRouteEntryPtr->qosPrecedence & 0x1) << 7)                  |
                    ((ipMcRouteEntryPtr->qosProfileMarkingEnable & 0x1) << 8)        |
                    ((ipMcRouteEntryPtr->qosProfileIndex & 0x7F) << 9)               |
                    ((modifyDscp & 0x3) << 16)                    |
                    ((modifyUp & 0x3) << 18)                      |
                    ((ipMcRouteEntryPtr->multicastRPFVlan & 0xFFF) << 20));
        hwDataPtr[1] |= ((ipMcRouteEntryPtr->externalMLLPointer & 0x1FFF)                |
                    ((rpfFailCmd & 0x7) << 14)                                    |
                    ((ipMcRouteEntryPtr->multicastRPFFailCommandMode & 0x1) << 17)   |
                    ((ipMcRouteEntryPtr->ttlHopLimitDecEnable & 0x1) << 18)          |
                    ((ipMcRouteEntryPtr->internalMLLPointer & 0x1FFF) << 19));
        hwDataPtr[2] |= ((ipMcRouteEntryPtr->siteId & 0x1)                                |
                     ((ipMcRouteEntryPtr->scopeCheckingEnable & 0x1) << 1)            |
                     ((counterSet & 0x7) << 3)                                     |
                     ((ipMcRouteEntryPtr->mtuProfileIndex & 0x7) << 6)                |
                     ((ipMcRouteEntryPtr->appSpecificCpuCodeEnable & 0x1) << 10)      |
                     ((ipMcRouteEntryPtr->multicastRPFCheckEnable & 0x1) << 18));

        /* hwDataPtr[3] = 0; */
    }

   return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpConvertHwFormat2McEntry
*
* DESCRIPTION:
*       This function converts a given ip mc entry to the HW format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum       - the device number
*       hwDataPtr    - The mc entry in the HW format to be converted..
*
* OUTPUTS:
*       ipMcRouteEntryPtr - The mc entry data.
*
* RETURNS:
*       GT_BAD_PARAM    - on invalid parameter.
*       GT_OK           - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpConvertHwFormat2McEntry
(
    IN   GT_U8                           devNum,
    OUT  CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *ipMcRouteEntryPtr,
    IN   GT_U32 *hwDataPtr
)
{
    GT_U32 value, cmd, cpuCodeIdx, qosPrecedence, modifyDscp, modifyUp,
           RPFFailCommand, multicastRPFFailCommandMode, siteId, countSet;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COMMAND_E);
        cmd = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_CPU_CODE_INDEX_E);
        cpuCodeIdx = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E);
        ipMcRouteEntryPtr->ttlHopLimDecOptionsExtChkByPass = BIT2BOOL_MAC(value);

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E);
        if (value)
        {
            ipMcRouteEntryPtr->ingressMirrorToAnalyzerIndex = value - 1;
            ipMcRouteEntryPtr->ingressMirror = GT_TRUE;
        }
        else
        {
            ipMcRouteEntryPtr->ingressMirror = GT_FALSE;
        }

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E);
        qosPrecedence = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E);
        ipMcRouteEntryPtr->qosProfileMarkingEnable = BIT2BOOL_MAC(value);

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_INDEX_E);
        ipMcRouteEntryPtr->qosProfileIndex = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_DSCP_E);
        modifyDscp = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E);
        modifyUp = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_INGRESS_VLAN_CHECK_E);
        if (value == 0)
        {
            ipMcRouteEntryPtr->multicastRPFCheckEnable = GT_FALSE;
        }
        else
        {
            ipMcRouteEntryPtr->multicastRPFCheckEnable = GT_TRUE;
            if (value == 1)
            {
                ipMcRouteEntryPtr->multicastIngressVlanCheck = CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_RPF_CHECK_E;
                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_E);
                ipMcRouteEntryPtr->multicastRPFVlan = (GT_U16)value;
            }
            else    /* value == 2 */
            {
                ipMcRouteEntryPtr->multicastIngressVlanCheck = CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_BIDIRECTIONAL_TREE_CHECK_E;
                LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_MRST_INDEX_E);
                ipMcRouteEntryPtr->multicastRPFRoutingSharedTreeIndex = (GT_U8)value;
            }
        }

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EXTERNAL_MLL_PTR_E);
        ipMcRouteEntryPtr->externalMLLPointer = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EXTERNAL_MLL_PTR_MSB_BITS_15_13_E);
        ipMcRouteEntryPtr->externalMLLPointer |= (value << 13);

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_RPF_FAIL_CMD_MODE_E);
        multicastRPFFailCommandMode = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E);
        ipMcRouteEntryPtr->ttlHopLimitDecEnable = BIT2BOOL_MAC(value);

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INTERNAL_MLL_PTR_E);
        ipMcRouteEntryPtr->internalMLLPointer = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_DEST_SITE_ID_E);
        siteId = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_SCOPE_CHECK_EN_E);
        ipMcRouteEntryPtr->scopeCheckingEnable = BIT2BOOL_MAC(value);

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MTU_INDEX_E);
        ipMcRouteEntryPtr->mtuProfileIndex = value;

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E);
        ipMcRouteEntryPtr->appSpecificCpuCodeEnable = BIT2BOOL_MAC(value);

        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COUNTER_SET_INDEX_E);
        countSet = value;
    }
    else
    {
        cmd = U32_GET_FIELD_MAC(hwDataPtr[0],0,3);
        cpuCodeIdx = U32_GET_FIELD_MAC(hwDataPtr[0],4,2);
        ipMcRouteEntryPtr->ttlHopLimDecOptionsExtChkByPass = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[0],3,1));
        ipMcRouteEntryPtr->ingressMirror                   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[0],6,1));
        qosPrecedence = U32_GET_FIELD_MAC(hwDataPtr[0],7,1);
        ipMcRouteEntryPtr->qosProfileMarkingEnable         = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[0],8,1));
        ipMcRouteEntryPtr->qosProfileIndex                 = U32_GET_FIELD_MAC(hwDataPtr[0],9,7);
        ipMcRouteEntryPtr->multicastRPFVlan        = (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[0],20,12);
        modifyDscp = U32_GET_FIELD_MAC(hwDataPtr[0],16,2);
        modifyUp = U32_GET_FIELD_MAC(hwDataPtr[0],18,2);
        ipMcRouteEntryPtr->multicastRPFCheckEnable  = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[2],18,1));
        ipMcRouteEntryPtr->externalMLLPointer = U32_GET_FIELD_MAC(hwDataPtr[1],0,13);
        multicastRPFFailCommandMode = U32_GET_FIELD_MAC(hwDataPtr[1],17,1);
        ipMcRouteEntryPtr->ttlHopLimitDecEnable            = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[1],18,1));
        ipMcRouteEntryPtr->internalMLLPointer              = U32_GET_FIELD_MAC(hwDataPtr[1],19,13);
        siteId = U32_GET_FIELD_MAC(hwDataPtr[2],0,1);
        ipMcRouteEntryPtr->scopeCheckingEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[2],1,1));
        ipMcRouteEntryPtr->mtuProfileIndex = U32_GET_FIELD_MAC(hwDataPtr[2],6,3);
        ipMcRouteEntryPtr->appSpecificCpuCodeEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[2],10,1));
        countSet = U32_GET_FIELD_MAC(hwDataPtr[2],3,3);
    }


    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        LION3_IPVX_FIELD_VALUE_GET_MAC(devNum,LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_INGRESS_VLAN_CHECK_FAIL_CMD_E);
        RPFFailCommand = value;
    }
    else
    {
        RPFFailCommand = U32_GET_FIELD_MAC(hwDataPtr[1],14,3);
    }
    switch(RPFFailCommand)
    {
    case 2:
        ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        break;
    case 3:
        ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_DROP_HARD_E;
        break;
    case 4:
        ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
        break;
    case 5:
        ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
        break;
    case 6:
        ipMcRouteEntryPtr->RPFFailCommand = CPSS_PACKET_CMD_BRIDGE_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }


    switch(cmd)
    {
        case 0:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_ROUTE_E;
            break;
        case 1:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;
        case 2:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 5:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;
        case 6:
            ipMcRouteEntryPtr->cmd = CPSS_PACKET_CMD_BRIDGE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch(cpuCodeIdx)
    {
         case 0:
             ipMcRouteEntryPtr->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_0_E;
             break;
         case 1:
             ipMcRouteEntryPtr->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_1_E;
             break;
         case 2:
             ipMcRouteEntryPtr->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_2_E;
             break;
         case 3:
             ipMcRouteEntryPtr->cpuCodeIdx = CPSS_DXCH_IP_CPU_CODE_IDX_3_E;
             break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch(qosPrecedence)
    {
        case 0:
            ipMcRouteEntryPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            break;
        case 1:
            ipMcRouteEntryPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch(modifyDscp)
    {
        case 0:
            ipMcRouteEntryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            ipMcRouteEntryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            ipMcRouteEntryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        case 3:
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch(modifyUp)
    {
        case 0:
            ipMcRouteEntryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            ipMcRouteEntryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            ipMcRouteEntryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        case 3:
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch(multicastRPFFailCommandMode)
    {
        case 0:
            ipMcRouteEntryPtr->multicastRPFFailCommandMode = CPSS_DXCH_IP_MULTICAST_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E;
            break;
        case 1:
            ipMcRouteEntryPtr->multicastRPFFailCommandMode = CPSS_DXCH_IP_MULTICAST_MLL_RPF_FAIL_COMMAND_MODE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch(siteId)
    {
        case 0:
            ipMcRouteEntryPtr->siteId = CPSS_IP_SITE_ID_INTERNAL_E;
            break;
        case 1:
            ipMcRouteEntryPtr->siteId = CPSS_IP_SITE_ID_EXTERNAL_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch(countSet)
    {
        case 0:
            ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_SET0_E;
            break;
        case 1:
            ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_SET1_E;
            break;
        case 2:
            ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_SET2_E;
            break;
        case 3:
            ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_SET3_E;
            break;
        case 4:
            ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_NO_SET_E;
            break;
        case 7:
            ipMcRouteEntryPtr->countSet = CPSS_IP_CNT_NO_SET_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpUcRouteEntriesCheck
*
* DESCRIPTION:
*       Check validity of the route entry parametrers, in all entries
*       of routeEntriesArray.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum               - the device number
*       routeEntriesArray    - the uc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on all valid parameters.
*       GT_BAD_PARAM    - on invalid parameter.
*       GT_OUT_OF_RANGE - on out of range parameter.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpUcRouteEntriesCheck
(
    IN GT_U8                           devNum,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
)
{
    GT_U32 i,j;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC  *routeEntry = NULL;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_PORT_NUM   portTrunk;/*tmp port/trunk*/
    GT_HW_DEV_NUM devHwTmp;/*tmp HW device*/

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(routeEntriesArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    for (i = 0; i<numOfRouteEntries; i++)
    {
        switch (routeEntriesArray[i].type)
        {
        case CPSS_DXCH_IP_UC_ROUTE_ENTRY_E:

            routeEntry = &routeEntriesArray[i].entry.regularEntry; /* to make the code more readable... */

            switch(routeEntry->cmd)
            {
            case CPSS_PACKET_CMD_ROUTE_E:
            case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            case CPSS_PACKET_CMD_DROP_HARD_E:
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            switch(routeEntry->cpuCodeIdx)
            {
            case CPSS_DXCH_IP_CPU_CODE_IDX_0_E:
            case CPSS_DXCH_IP_CPU_CODE_IDX_1_E:
            case CPSS_DXCH_IP_CPU_CODE_IDX_2_E:
            case CPSS_DXCH_IP_CPU_CODE_IDX_3_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            switch(routeEntry->qosPrecedence)
            {
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            switch(routeEntry->modifyUp)
            {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            switch(routeEntry->modifyDscp)
            {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            switch(routeEntry->countSet)
            {
            case CPSS_IP_CNT_SET0_E:
            case CPSS_IP_CNT_SET1_E:
            case CPSS_IP_CNT_SET2_E:
            case CPSS_IP_CNT_SET3_E:
            case CPSS_IP_CNT_NO_SET_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            switch(routeEntry->siteId)
            {
            case CPSS_IP_SITE_ID_INTERNAL_E:
            case CPSS_IP_SITE_ID_EXTERNAL_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(routeEntry->nextHopVlanId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                if (routeEntry->nextHopVlanId1 > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            }

            switch(routeEntry->nextHopInterface.type)
            {
            case CPSS_INTERFACE_PORT_E:
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(routeEntry->nextHopInterface.devPort.hwDevNum,
                                                       routeEntry->nextHopInterface.devPort.portNum);
                devHwTmp = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                    routeEntry->nextHopInterface.devPort.hwDevNum,
                    routeEntry->nextHopInterface.devPort.portNum);
                portTrunk = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                    routeEntry->nextHopInterface.devPort.hwDevNum,
                    routeEntry->nextHopInterface.devPort.portNum);
                if (portTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                if (devHwTmp > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                break;
            case CPSS_INTERFACE_TRUNK_E:
                if (routeEntry->nextHopInterface.trunkId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                break;
            case CPSS_INTERFACE_VIDX_E:
                if (routeEntry->nextHopInterface.vidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                break;
            case CPSS_INTERFACE_VID_E:
                /* in this case we will set vidx to be 0xFFF and useVidx=GT_TRUE */
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            if (routeEntry->qosProfileIndex >= BIT_7)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            if(routeEntry->mtuProfileIndex > 7)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

            /* Tunnel pointer and ARP pointer reside on the same HW bits,
               the meaning of those bits is determine by those bits          */
            if(routeEntry->isTunnelStart == GT_TRUE)
            {
                 if ((PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))&&
                     (routeEntry->isNat == GT_TRUE))
                 {
                     if (routeEntry->nextHopNatPointer >= fineTuningPtr->tableSize.tunnelStart)
                         CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                 }
                 else
                 {
                     if (routeEntry->nextHopTunnelPointer >= fineTuningPtr->tableSize.tunnelStart)
                         CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                 }
            }
            else/* ARP */
            {
                /* ARP pointer is relevant only if packet */
                /* command is route or route and mirror   */
                if ((routeEntry->cmd == CPSS_PACKET_CMD_ROUTE_E) ||
                    (routeEntry->cmd == CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E))
                {
                    if (routeEntry->nextHopARPPointer >= fineTuningPtr->tableSize.routerArp)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }

            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                if ((routeEntry->ingressMirror == GT_TRUE) &&
                    (routeEntry->ingressMirrorToAnalyzerIndex > PRV_CPSS_DXCH_IP_MIRROR_ANALYZER_MAX_INDEX_CNS))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            }

            break;

        case CPSS_DXCH_IP_UC_ECMP_RPF_E:

            for (j = 0 ; j < 8 ; j++)
                {
                if (routeEntriesArray[i].entry.ecmpRpfCheck.vlanArray[j] > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpMcRouteEntryCheck
*
* DESCRIPTION:
*       Check validity of the route entry parametrers, in all entries
*       of routeEntriesArray.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum           - the device number
*       routeEntryPtr    - the Mc route entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on all valid parameters.
*       GT_BAD_PARAM    - on invalid parameter.
*       GT_OUT_OF_RANGE - on out of range parameter.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpMcRouteEntryCheck
(
    IN GT_U8                           devNum,
    IN CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *routeEntryPtr
)
{
    switch(routeEntryPtr->cmd)
    {
    case CPSS_PACKET_CMD_ROUTE_E:
    case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
    case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
    case CPSS_PACKET_CMD_DROP_HARD_E:
    case CPSS_PACKET_CMD_DROP_SOFT_E:
    case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
    case CPSS_PACKET_CMD_BRIDGE_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(routeEntryPtr->cpuCodeIdx)
    {
    case CPSS_DXCH_IP_CPU_CODE_IDX_0_E:
    case CPSS_DXCH_IP_CPU_CODE_IDX_1_E:
    case CPSS_DXCH_IP_CPU_CODE_IDX_2_E:
    case CPSS_DXCH_IP_CPU_CODE_IDX_3_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(routeEntryPtr->qosPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(routeEntryPtr->modifyUp)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(routeEntryPtr->modifyDscp)
    {
    case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
    case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(routeEntryPtr->countSet)
    {
    case CPSS_IP_CNT_SET0_E:
    case CPSS_IP_CNT_SET1_E:
    case CPSS_IP_CNT_SET2_E:
    case CPSS_IP_CNT_SET3_E:
    case CPSS_IP_CNT_NO_SET_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(routeEntryPtr->siteId)
    {
    case CPSS_IP_SITE_ID_INTERNAL_E:
    case CPSS_IP_SITE_ID_EXTERNAL_E:
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(routeEntryPtr->multicastRPFCheckEnable)
    {
        switch(routeEntryPtr->multicastRPFFailCommandMode)
        {
        case CPSS_DXCH_IP_MULTICAST_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E:
        case CPSS_DXCH_IP_MULTICAST_MLL_RPF_FAIL_COMMAND_MODE_E:

            switch(routeEntryPtr->RPFFailCommand)
            {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            case CPSS_PACKET_CMD_DROP_SOFT_E:
            case CPSS_PACKET_CMD_DROP_HARD_E:
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            case CPSS_PACKET_CMD_BRIDGE_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if((routeEntryPtr->multicastRPFCheckEnable == GT_TRUE) &&
        (routeEntryPtr->multicastRPFVlan > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(routeEntryPtr->mtuProfileIndex > 7)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if ((routeEntryPtr->ingressMirror == GT_TRUE) &&
            (routeEntryPtr->ingressMirrorToAnalyzerIndex > PRV_CPSS_DXCH_IP_MIRROR_ANALYZER_MAX_INDEX_CNS))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        switch (routeEntryPtr->multicastIngressVlanCheck)
        {
            case CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_RPF_CHECK_E:
            case CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_BIDIRECTIONAL_TREE_CHECK_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (routeEntryPtr->multicastRPFRoutingSharedTreeIndex > PRV_CPSS_DXCH_IP_SHARED_TREE_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpMllStruct2HwFormat
*
* DESCRIPTION:
*       This function converts a given ip Mll struct to the hardware format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - the device number
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - (points to ) the entry to be converted.
*
* OUTPUTS:
*       hwDataPtr            - The entry in the HW format representation.
*
* RETURNS:
*       GT_OK             - on all valid parameters.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChIpMllStruct2HwFormat
(
    IN GT_U8                                        devNum,
    IN  CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairWriteForm,
    IN  CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr,
    OUT GT_U32                                      *hwDataPtr
)
{
    GT_U32 rpfFailCommand;
    GT_U32 tunnelStartPassengerType = 0;
    GT_U32 hwPort,hwDev;

    if((mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E))
    {
        switch(mllPairEntryPtr->firstMllNode.mllRPFFailCommand)
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                rpfFailCommand = 2;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                rpfFailCommand = 3;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                rpfFailCommand = 4;
                break;
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
                rpfFailCommand = 5;
                break;
            case CPSS_PACKET_CMD_BRIDGE_E:
                rpfFailCommand = 6;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        hwDataPtr[0] = (BOOL2BIT_MAC(mllPairEntryPtr->firstMllNode.last)                |
                     (BOOL2BIT_MAC(mllPairEntryPtr->firstMllNode.isTunnelStart) << 1)|
                     ((mllPairEntryPtr->firstMllNode.nextHopVlanId & 0xFFF) << 3)|
                     ((rpfFailCommand & 0x7) << 29));

        switch(mllPairEntryPtr->firstMllNode.nextHopInterface.type)
        {
            case CPSS_INTERFACE_PORT_E:
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                    mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.hwDevNum,
                    mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.portNum);
                hwDev =  PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                    mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.hwDevNum,
                    mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.portNum);
                hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                    mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.hwDevNum,
                    mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.portNum);
                hwDataPtr[0] |= ((0x0 << 15) | (0x0 << 16)                                                      |
                              ((hwPort & 0x3F) << 17)|
                              ((hwDev & 0x1F) << 23));
                break;
            case CPSS_INTERFACE_TRUNK_E:
                hwDataPtr[0] |= ((0x0 << 15) | (0x1 << 16) |
                              ((mllPairEntryPtr->firstMllNode.nextHopInterface.trunkId & 0x7F) << 21));
                break;
            case CPSS_INTERFACE_VIDX_E:
                hwDataPtr[0] |= ((0x1 << 15) |
                             ((mllPairEntryPtr->firstMllNode.nextHopInterface.vidx & 0xFFF) << 16));

                break;
            case CPSS_INTERFACE_VID_E:
                hwDataPtr[0] |= ((0x1 << 15) | (0xFFF << 16));
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        }

        /* tunnelStartPassengerType should be handled only in supported devices and
           when isTunnelStart is GT_TRUE*/
        if (((PRV_CPSS_PP_MAC(devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH2_E) &&
            (mllPairEntryPtr->firstMllNode.isTunnelStart == GT_TRUE))
        {
             switch(mllPairEntryPtr->firstMllNode.tunnelStartPassengerType)
            {
                case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
                    tunnelStartPassengerType = 0;
                    break;
                case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
                    tunnelStartPassengerType = 1;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

        /*in case we dont write whole entry we need to save 2 bits from word 1
          for the tunnel pointer of second mll*/
        if (mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E)
            mllPairEntryPtr->secondMllNode.nextHopTunnelPointer = U32_GET_FIELD_MAC(hwDataPtr[1],22,2) << 10;

        /*we will use |= so the second mll bits we have read before the function will not be overide*/
        hwDataPtr[1] = ((mllPairEntryPtr->firstMllNode.nextHopTunnelPointer & 0x3FF)       |
                       ((tunnelStartPassengerType & 0x1) << 10)                            |
                       ((mllPairEntryPtr->firstMllNode.ttlHopLimitThreshold & 0xFF) << 11) |
                       (BOOL2BIT_MAC(mllPairEntryPtr->firstMllNode.excludeSrcVlan) << 19));

        if ((PRV_CPSS_PP_MAC(devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /*we write to word 1 also the second mll save value we extracted*/
            hwDataPtr[1] |= (mllPairEntryPtr->firstMllNode.nextHopTunnelPointer & 0xC00) << 10;
            hwDataPtr[1] |= (mllPairEntryPtr->secondMllNode.nextHopTunnelPointer & 0xC00) << 12;
        }

    }

    if((mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E) ||
       (mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E))
    {
        switch(mllPairEntryPtr->secondMllNode.mllRPFFailCommand)
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                rpfFailCommand = 2;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                rpfFailCommand = 3;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                rpfFailCommand = 4;
                break;
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
                rpfFailCommand = 5;
                break;
            case CPSS_PACKET_CMD_BRIDGE_E:
                rpfFailCommand = 6;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        hwDataPtr[2] = (BOOL2BIT_MAC(mllPairEntryPtr->secondMllNode.last)                 |
                     (BOOL2BIT_MAC(mllPairEntryPtr->secondMllNode.isTunnelStart) << 1) |
                     ((mllPairEntryPtr->secondMllNode.nextHopVlanId & 0xFFF) << 3) |
                     ((rpfFailCommand & 0x7) << 29));

        switch(mllPairEntryPtr->secondMllNode.nextHopInterface.type)
        {
            case CPSS_INTERFACE_PORT_E:
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                    mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.hwDevNum,
                    mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.portNum);
                hwDev =  PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                    mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.hwDevNum,
                    mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.portNum);
                hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                    mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.hwDevNum,
                    mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.portNum);
                hwDataPtr[2] |= ((0x0 << 15) | (0x0 << 16)                                                       |
                              ((hwPort & 0x3F) << 17)|
                              ((hwDev & 0x1F) << 23));
                break;
            case CPSS_INTERFACE_TRUNK_E:
                hwDataPtr[2] |= ((0x0 << 15) | (0x1 << 16) |
                              ((mllPairEntryPtr->secondMllNode.nextHopInterface.trunkId & 0x7F) << 21));
                break;
            case CPSS_INTERFACE_VIDX_E:
                hwDataPtr[2] |= ((0x1 << 15) |
                             ((mllPairEntryPtr->secondMllNode.nextHopInterface.vidx & 0xFFF) << 16));

                break;
            case CPSS_INTERFACE_VID_E:
                hwDataPtr[0] |= ((0x1 << 15) | (0xFFF << 16));
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        }

        /* tunnelStartPassengerType should be handled only in supported devices and
           when isTunnelStart is GT_TRUE*/
        if (((PRV_CPSS_PP_MAC(devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH2_E) &&
            (mllPairEntryPtr->secondMllNode.isTunnelStart == GT_TRUE))
        {
             switch(mllPairEntryPtr->secondMllNode.tunnelStartPassengerType)
            {
                case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
                    tunnelStartPassengerType = 0;
                    break;
                case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
                    tunnelStartPassengerType = 1;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

        hwDataPtr[3] = ((mllPairEntryPtr->secondMllNode.nextHopTunnelPointer & 0x3FF)       |
                       ((tunnelStartPassengerType & 0x1) << 10)                             |
                       ((mllPairEntryPtr->secondMllNode.ttlHopLimitThreshold & 0xFF) << 11) |
                       (BOOL2BIT_MAC(mllPairEntryPtr->secondMllNode.excludeSrcVlan) << 19)  |
                       ((mllPairEntryPtr->nextPointer & 0xFFF) << 20));

        if ((PRV_CPSS_PP_MAC(devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /*we write to word 1 only to relevant mll and we dont modify what we red for the other mll*/
            switch (mllPairWriteForm)
            {
                case CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E:
                    /*need to clean old garbage value and then set this bit*/
                    U32_SET_FIELD_MAC(hwDataPtr[1],22,2,0);
                    hwDataPtr[1] |= (mllPairEntryPtr->secondMllNode.nextHopTunnelPointer & 0xC00) << 12;
                    break;
                case CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E:
                    /*nothing to do we changed it in first if*/
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpHwFormat2MllStruct
*
* DESCRIPTION:
*       This function converts a given ip Mll struct to the hardware format.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - the device number
*       hwDataPtr     - The entry in the HW format representation.
*       isFirstMll    - if the MLL analyzed is the first MLL in the pair
*
* OUTPUTS:
*       mllEntryPtr   - The entry to be converted.
*
* RETURNS:
*       GT_OK           - on all valid parameters.
*       GT_BAD_STATE    - on invalid hardware value read
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChIpHwFormat2MllStruct
(
    IN   GT_U8                  devNum,
    IN   GT_U32                 *hwDataPtr,
    IN   GT_BOOL                isFirstMll,
    OUT  CPSS_DXCH_IP_MLL_STC   *mllEntryPtr
)
{
    GT_U32 useVidx0;
    GT_U32 targetIsTrunk;

    GT_U32 relativeLocation = 0;

    if (isFirstMll == GT_FALSE)
    {
        relativeLocation = 2;
    }

    /* clear output data */
    cpssOsMemSet(mllEntryPtr, 0, sizeof(CPSS_DXCH_IP_MLL_STC));

    mllEntryPtr->last          = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],0,1));
    mllEntryPtr->isTunnelStart = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],1,1));
    mllEntryPtr->nextHopVlanId = (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],3,12);

    useVidx0      = U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],15,1);
    targetIsTrunk = U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],16,1);

    if(useVidx0 == 1)
    {
        mllEntryPtr->nextHopInterface.type = CPSS_INTERFACE_VIDX_E;
        mllEntryPtr->nextHopInterface.vidx =
            (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],16,12);

        if(mllEntryPtr->nextHopInterface.vidx == 0xFFF)
        {
            mllEntryPtr->nextHopInterface.type = CPSS_INTERFACE_VID_E;
            mllEntryPtr->nextHopInterface.vlanId = 0xFFF;
        }
    }
    else if(targetIsTrunk == 1)
    {
        mllEntryPtr->nextHopInterface.type = CPSS_INTERFACE_TRUNK_E;
        mllEntryPtr->nextHopInterface.trunkId =
            (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],21,7);
    }
    else
    {
        mllEntryPtr->nextHopInterface.type = CPSS_INTERFACE_PORT_E;
        mllEntryPtr->nextHopInterface.devPort.portNum =
            (GT_U8)U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],17,6);
        mllEntryPtr->nextHopInterface.devPort.hwDevNum  =
            (GT_U8)U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],23,5);
        PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(mllEntryPtr->nextHopInterface));
    }

    switch(U32_GET_FIELD_MAC(hwDataPtr[relativeLocation],29,3))
    {
        case 2:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 5:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;
        case 6:
            mllEntryPtr->mllRPFFailCommand = CPSS_PACKET_CMD_BRIDGE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    mllEntryPtr->nextHopTunnelPointer = U32_GET_FIELD_MAC(hwDataPtr[relativeLocation+1],0,10);
    mllEntryPtr->ttlHopLimitThreshold = (GT_U16)U32_GET_FIELD_MAC(hwDataPtr[relativeLocation+1],11,8);
    mllEntryPtr->excludeSrcVlan       = U32_GET_FIELD_MAC(hwDataPtr[relativeLocation+1],19,1);
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH2_E)
    {
        if (isFirstMll)
        {
            mllEntryPtr->nextHopTunnelPointer |= U32_GET_FIELD_MAC(hwDataPtr[1],20,2) << 10;
        }
        else
        {
            mllEntryPtr->nextHopTunnelPointer |= U32_GET_FIELD_MAC(hwDataPtr[1],22,2) << 10;
        }

    }

    /* tunnelStartPassengerType should be handled only in supported devices and
       when isTunnelStart is GT_TRUE*/
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH2_E) &&
        (mllEntryPtr->isTunnelStart == GT_TRUE))
    {
        PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_ATTR_TUNNEL_START_PASS_TYPE_MAC(
                mllEntryPtr->tunnelStartPassengerType,
                (U32_GET_FIELD_MAC(hwDataPtr[relativeLocation+1],10,1)));
    }

    return GT_OK;
}

/*******************************************************************************
* lion3IpMllStruct2HwFormatSubSection
*
* DESCRIPTION:
*       SIP5 : This function converts a given ip Mll struct to the hardware format.
*              convert only sub section (0 or 1)
*              for section 1 , it also set the 'next MLL'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       mllPairEntryPtr   - (points to ) the entry to be converted.
*       sectionIndex      - section index 0 or 1.
*
* OUTPUTS:
*       hwDataPtr            - The entry in the HW format representation.
*
* RETURNS:
*       GT_OK             - on all valid parameters.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion3IpMllStruct2HwFormatSubSection
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr,
    OUT GT_U32                                      *hwDataPtr,
    IN  GT_U32                                       sectionIndex
)
{
    GT_U32  value;/*tmp value to set to HW*/
    CPSS_DXCH_IP_MLL_STC *currSwPtr;/*pointer to current SW entry (in each HW line there are 2 entries)*/

    /* pointer to current SW info */
    currSwPtr = sectionIndex ? &mllPairEntryPtr->secondMllNode :
                               &mllPairEntryPtr->firstMllNode ;

    if(sectionIndex)
    {
        value = mllPairEntryPtr->nextPointer;
        LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
            LION3_IP_MLL_TABLE_FIELDS_NEXT_MLL_PTR_E
            ,value);
    }

    value = currSwPtr->last;
    LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_LAST_1_E :
            LION3_IP_MLL_TABLE_FIELDS_LAST_0_E
        ,value);

    switch(currSwPtr->mllRPFFailCommand)
    {
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            value = 2;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            value = 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            value = 4;
            break;
        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            value = 5;
            break;
        case CPSS_PACKET_CMD_BRIDGE_E:
            value = 6;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_MLL_RPF_FAIL_CMD_1_E :
            LION3_IP_MLL_TABLE_FIELDS_MLL_RPF_FAIL_CMD_0_E
        ,value);

    value = currSwPtr->nextHopVlanId;
    LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_MLL_EVID_1_E :
            LION3_IP_MLL_TABLE_FIELDS_MLL_EVID_0_E
        ,value);


    value = ((currSwPtr->nextHopInterface.type == CPSS_INTERFACE_VIDX_E) ||
             (currSwPtr->nextHopInterface.type == CPSS_INTERFACE_VID_E)) ? 1 : 0;
    LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_USE_VIDX_1_E :
            LION3_IP_MLL_TABLE_FIELDS_USE_VIDX_0_E
        ,value);

    if(value)
    {
        if(currSwPtr->nextHopInterface.type == CPSS_INTERFACE_VID_E)
        {
            value = 0xFFF;
        }
        else
        {
            value = currSwPtr->nextHopInterface.vidx;
        }

        LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
            sectionIndex ?
                LION3_IP_MLL_TABLE_FIELDS_EVIDX_1_E :
                LION3_IP_MLL_TABLE_FIELDS_EVIDX_0_E
            ,value);
    }
    else
    {
        value = (currSwPtr->nextHopInterface.type == CPSS_INTERFACE_TRUNK_E) ? 1 :0;
        LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
            sectionIndex ?
                LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E :
                LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E
            ,value);

        if(currSwPtr->nextHopInterface.type == CPSS_INTERFACE_TRUNK_E)
        {
            value = currSwPtr->nextHopInterface.trunkId;
            LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
                sectionIndex ?
                    LION3_IP_MLL_TABLE_FIELDS_TRG_TRUNK_ID_1_E :
                    LION3_IP_MLL_TABLE_FIELDS_TRG_TRUNK_ID_0_E
                ,value);
        }
        else
        if(currSwPtr->nextHopInterface.type == CPSS_INTERFACE_PORT_E)
        {
            value = currSwPtr->nextHopInterface.devPort.portNum;
            LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
                sectionIndex ?
                    LION3_IP_MLL_TABLE_FIELDS_TRG_EPORT_1_E :
                    LION3_IP_MLL_TABLE_FIELDS_TRG_EPORT_0_E
                ,value);

            value = currSwPtr->nextHopInterface.devPort.hwDevNum;
            LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
                sectionIndex ?
                    LION3_IP_MLL_TABLE_FIELDS_TRG_DEV_1_E :
                    LION3_IP_MLL_TABLE_FIELDS_TRG_DEV_0_E
                ,value);
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    value = BOOL2BIT_MAC(currSwPtr->isTunnelStart);
    LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_START_OF_TUNNEL_1_E :
            LION3_IP_MLL_TABLE_FIELDS_START_OF_TUNNEL_0_E
        ,value);

    if(value)
    {
        value = currSwPtr->nextHopTunnelPointer;
        LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
            sectionIndex ?
                LION3_IP_MLL_TABLE_FIELDS_TUNNEL_PTR_1_E :
                LION3_IP_MLL_TABLE_FIELDS_TUNNEL_PTR_0_E
            ,value);

        switch(currSwPtr->tunnelStartPassengerType)
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

        LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
            sectionIndex ?
                LION3_IP_MLL_TABLE_FIELDS_TUNNEL_TYPE_1_E :
                LION3_IP_MLL_TABLE_FIELDS_TUNNEL_TYPE_0_E
            ,value);
    }

    value = currSwPtr->ttlHopLimitThreshold;
    LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_TTL_THRESHOLD_1_OR_HOP_LIMIT_THRESHOLD_1_E :
            LION3_IP_MLL_TABLE_FIELDS_TTL_THRESHOLD_0_OR_HOP_LIMIT_THRESHOLD_0_E
        ,value);


    value = BOOL2BIT_MAC(currSwPtr->excludeSrcVlan);
    LION3_IP_MLL_FIELD_SET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_EXCLUDE_SRC_VLAN_1_E :
            LION3_IP_MLL_TABLE_FIELDS_EXCLUDE_SRC_VLAN_0_E
        ,value);

    return GT_OK;
}

/*******************************************************************************
* lion3IpMllHw2StructFormatSubSection
*
* DESCRIPTION:
*       SIP5 : This function converts a given ip Mll hardware format to struct (SW) .
*              convert only sub section (0 or 1)
*              for section 1 , it also set the 'next MLL'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       hwDataPtr         - The entry in the HW format , the need to be converted to SW.
*       sectionIndex      - section index 0 or 1.
*
* OUTPUTS:
*       mllPairEntryPtr   - (points to ) the converted entry.
*
* RETURNS:
*       GT_OK             - on all valid parameters.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion3IpMllHw2StructFormatSubSection
(
    IN GT_U8                                        devNum,
    OUT  CPSS_DXCH_IP_MLL_PAIR_STC                  *mllPairEntryPtr,
    IN GT_U32                                       *hwDataPtr,
    IN  GT_U32                                       sectionIndex
)
{
    GT_U32  value;/*tmp value to get from HW*/
    CPSS_DXCH_IP_MLL_STC *currSwPtr;/*pointer to current SW entry (in each HW line there are 2 entries)*/

    /* pointer to current SW info */
    currSwPtr = sectionIndex ? &mllPairEntryPtr->secondMllNode :
                               &mllPairEntryPtr->firstMllNode ;

    if(sectionIndex)
    {
        LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
            LION3_IP_MLL_TABLE_FIELDS_NEXT_MLL_PTR_E
            ,value);
        mllPairEntryPtr->nextPointer = (GT_U16)value;
    }

    LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_LAST_1_E :
            LION3_IP_MLL_TABLE_FIELDS_LAST_0_E
        ,value);
    currSwPtr->last = BIT2BOOL_MAC(value);

    LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_MLL_RPF_FAIL_CMD_1_E :
            LION3_IP_MLL_TABLE_FIELDS_MLL_RPF_FAIL_CMD_0_E
        ,value);
    switch(value)
    {
        case 2:
            currSwPtr->mllRPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            currSwPtr->mllRPFFailCommand = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            currSwPtr->mllRPFFailCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 5:
            currSwPtr->mllRPFFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;
        case 6:
            currSwPtr->mllRPFFailCommand = CPSS_PACKET_CMD_BRIDGE_E;
            break;
        case 0:/* reset value of HW */
            currSwPtr->mllRPFFailCommand = CPSS_PACKET_CMD_FORWARD_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_MLL_EVID_1_E :
            LION3_IP_MLL_TABLE_FIELDS_MLL_EVID_0_E
        ,value);
    currSwPtr->nextHopVlanId = (GT_U16)value;


    LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_USE_VIDX_1_E :
            LION3_IP_MLL_TABLE_FIELDS_USE_VIDX_0_E
        ,value);

    if(value)
    {
        LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
            sectionIndex ?
                LION3_IP_MLL_TABLE_FIELDS_EVIDX_1_E :
                LION3_IP_MLL_TABLE_FIELDS_EVIDX_0_E
            ,value);

        if(value != 0xFFF)
        {
            currSwPtr->nextHopInterface.type = CPSS_INTERFACE_VIDX_E;
            currSwPtr->nextHopInterface.vidx = (GT_U16)value;
        }
        else
        {
            currSwPtr->nextHopInterface.type = CPSS_INTERFACE_VID_E;
            currSwPtr->nextHopInterface.vlanId = 0xFFF;
        }
    }
    else
    {
        LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
            sectionIndex ?
                LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E :
                LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E
            ,value);

        if(value)
        {
            currSwPtr->nextHopInterface.type = CPSS_INTERFACE_TRUNK_E;
            LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
                sectionIndex ?
                    LION3_IP_MLL_TABLE_FIELDS_TRG_TRUNK_ID_1_E :
                    LION3_IP_MLL_TABLE_FIELDS_TRG_TRUNK_ID_0_E
                ,value);
            currSwPtr->nextHopInterface.trunkId = (GT_TRUNK_ID)value;
        }
        else
        {
            currSwPtr->nextHopInterface.type = CPSS_INTERFACE_PORT_E;

            LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
                sectionIndex ?
                    LION3_IP_MLL_TABLE_FIELDS_TRG_EPORT_1_E :
                    LION3_IP_MLL_TABLE_FIELDS_TRG_EPORT_0_E
                ,value);
            currSwPtr->nextHopInterface.devPort.portNum = value;

            LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
                sectionIndex ?
                    LION3_IP_MLL_TABLE_FIELDS_TRG_DEV_1_E :
                    LION3_IP_MLL_TABLE_FIELDS_TRG_DEV_0_E
                ,value);
            currSwPtr->nextHopInterface.devPort.hwDevNum = value;
        }
    }

    LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_START_OF_TUNNEL_1_E :
            LION3_IP_MLL_TABLE_FIELDS_START_OF_TUNNEL_0_E
        ,value);
    currSwPtr->isTunnelStart = BIT2BOOL_MAC(value);

    if(value)
    {
        LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
            sectionIndex ?
                LION3_IP_MLL_TABLE_FIELDS_TUNNEL_PTR_1_E :
                LION3_IP_MLL_TABLE_FIELDS_TUNNEL_PTR_0_E
            ,value);
        currSwPtr->nextHopTunnelPointer = value;

        LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
            sectionIndex ?
                LION3_IP_MLL_TABLE_FIELDS_TUNNEL_TYPE_1_E :
                LION3_IP_MLL_TABLE_FIELDS_TUNNEL_TYPE_0_E
            ,value);

        PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_ATTR_TUNNEL_START_PASS_TYPE_MAC(
                currSwPtr->tunnelStartPassengerType,
                value);
    }

    LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_TTL_THRESHOLD_1_OR_HOP_LIMIT_THRESHOLD_1_E :
            LION3_IP_MLL_TABLE_FIELDS_TTL_THRESHOLD_0_OR_HOP_LIMIT_THRESHOLD_0_E
        ,value);
    currSwPtr->ttlHopLimitThreshold = (GT_U16)value;

    LION3_IP_MLL_FIELD_GET_MAC(devNum,hwDataPtr,
        sectionIndex ?
            LION3_IP_MLL_TABLE_FIELDS_EXCLUDE_SRC_VLAN_1_E :
            LION3_IP_MLL_TABLE_FIELDS_EXCLUDE_SRC_VLAN_0_E
        ,value);
    currSwPtr->excludeSrcVlan = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* lion3IpMllStruct2HwFormat
*
* DESCRIPTION:
*       SIP5 : This function converts a given ip Mll struct to the hardware format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       mllPairForm       - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - (points to ) the entry to be converted.
*
* OUTPUTS:
*       hwDataPtr            - The entry in the HW format representation.
*
* RETURNS:
*       GT_OK             - on all valid parameters.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion3IpMllStruct2HwFormat
(
    IN GT_U8                                        devNum,
    IN  CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairForm,
    IN  CPSS_DXCH_IP_MLL_PAIR_STC                   *mllPairEntryPtr,
    OUT GT_U32                                      *hwDataPtr
)
{
    GT_STATUS rc = GT_BAD_PARAM;/* indication that none of the cases used */

    if(mllPairForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E ||
       mllPairForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E)
    {
        /* build section 0 */
        rc = lion3IpMllStruct2HwFormatSubSection(
                devNum,mllPairEntryPtr,hwDataPtr,0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(mllPairForm == CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E ||
       mllPairForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E)
    {
        /* build section 1 + next pointer */
        rc = lion3IpMllStruct2HwFormatSubSection(
                devNum,mllPairEntryPtr,hwDataPtr,1);
    }

    return rc;
}


/*******************************************************************************
* lion3IpMllHw2StructFormat
*
* DESCRIPTION:
*       SIP5 : This function converts a given ip Mll hardware format to struct (SW) .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       mllPairForm       - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       hwDataPtr         - The entry in the HW format , the need to be converted to SW.
*
* OUTPUTS:
*       mllPairEntryPtr   - (points to ) the converted entry.
*
* RETURNS:
*       GT_OK             - on all valid parameters.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion3IpMllHw2StructFormat
(
    IN GT_U8                                        devNum,
    IN  CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairForm,
    OUT  CPSS_DXCH_IP_MLL_PAIR_STC                  *mllPairEntryPtr,
    IN GT_U32                                       *hwDataPtr
)
{
    GT_STATUS rc = GT_BAD_PARAM;/* indication that none of the cases used */

    if(mllPairForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E ||
       mllPairForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E)
    {
        /* build section 0 */
        rc = lion3IpMllHw2StructFormatSubSection(devNum,
                mllPairEntryPtr,hwDataPtr,0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(mllPairForm == CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E ||
       mllPairForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E)
    {
        /* build section 1 + next pointer */
        rc = lion3IpMllHw2StructFormatSubSection(devNum,
                mllPairEntryPtr,hwDataPtr,1);
    }

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpEcmpEntryWrite
*
* DESCRIPTION:
*       Write an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpEcmpEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       ecmpEntryIndex,
    IN CPSS_DXCH_IP_ECMP_ENTRY_STC  *ecmpEntryPtr
)
{
    PRV_CPSS_DXCH_LPM_ECMP_ENTRY_STC    lpmEcmpEntry;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                          CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                          CPSS_XCAT2_E | CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ecmpEntryPtr);

    lpmEcmpEntry.randomEnable = ecmpEntryPtr->randomEnable;
    lpmEcmpEntry.numOfPaths = ecmpEntryPtr->numOfPaths;
    lpmEcmpEntry.nexthopBaseIndex = ecmpEntryPtr->routeEntryBaseIndex;

    return prvCpssDxChLpmHwEcmpEntryWrite(devNum, ecmpEntryIndex, &lpmEcmpEntry);
}

/*******************************************************************************
* cpssDxChIpEcmpEntryWrite
*
* DESCRIPTION:
*       Write an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       ecmpEntryIndex,
    IN CPSS_DXCH_IP_ECMP_ENTRY_STC  *ecmpEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpEcmpEntryWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ecmpEntryIndex, ecmpEntryPtr));

    rc = internal_cpssDxChIpEcmpEntryWrite(devNum, ecmpEntryIndex, ecmpEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ecmpEntryIndex, ecmpEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpEcmpEntryRead
*
* DESCRIPTION:
*       Read an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*
* OUTPUTS:
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpEcmpEntryRead
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       ecmpEntryIndex,
    OUT CPSS_DXCH_IP_ECMP_ENTRY_STC  *ecmpEntryPtr
)
{
    PRV_CPSS_DXCH_LPM_ECMP_ENTRY_STC    lpmEcmpEntry;
    GT_STATUS                           rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                          CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                          CPSS_XCAT2_E | CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ecmpEntryPtr);

    rc = prvCpssDxChLpmHwEcmpEntryRead(devNum, ecmpEntryIndex, &lpmEcmpEntry);
    if (rc == GT_OK)
    {
        ecmpEntryPtr->randomEnable = lpmEcmpEntry.randomEnable;
        ecmpEntryPtr->numOfPaths = lpmEcmpEntry.numOfPaths;
        ecmpEntryPtr->routeEntryBaseIndex = lpmEcmpEntry.nexthopBaseIndex;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChIpEcmpEntryRead
*
* DESCRIPTION:
*       Read an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*
* OUTPUTS:
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpEntryRead
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       ecmpEntryIndex,
    OUT CPSS_DXCH_IP_ECMP_ENTRY_STC  *ecmpEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpEcmpEntryRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ecmpEntryIndex, ecmpEntryPtr));

    rc = internal_cpssDxChIpEcmpEntryRead(devNum, ecmpEntryIndex, ecmpEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ecmpEntryIndex, ecmpEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpEcmpHashNumBitsSet
*
* DESCRIPTION:
*       Set the start bit and the number of bits needed by the L3 ECMP hash
*       mechanism. Those parameters are used to select the ECMP member
*       within the ECMP block according to one of the following formulas:
*       - when randomEnable bit in the ECMP entry is GT_FALSE:
*           ECMP member offset =
*           ((size of ECMP block) * (the value received between [startBit] and
*           [startBit + numOfBits - 1])) >> numOfBits
*       - when randomEnable bit in the ECMP entry is GT_TRUE:
*           ECMP member offset =
*           ((size of ECMP block) * (random seed value)) >> numOfBits
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       startBit        - the index of the first bit that is needed by the L3
*                         ECMP hash mechanism (APPLICABLE RANGES: 0..31)
*       numOfBits       - the number of bits that is needed by the L3 ECMP hash
*                         mechanism (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong startBit or numOfBits
*
* COMMENTS:
*       startBit is relevant only when randomEnable field in the entry is set to
*       GT_FALSE.
*       startBit + numOfBits must not exceed 32.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpEcmpHashNumBitsSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       startBit,
    IN GT_U32                       numOfBits
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChTrunkLion3HashNumBitsSet(devNum,
                                               PRV_CPSS_DXCH_TRUNK_LION3_HASH_CLIENT_TYPE_L3_ECMP_E,
                                               startBit,
                                               numOfBits);
}

/*******************************************************************************
* cpssDxChIpEcmpHashNumBitsSet
*
* DESCRIPTION:
*       Set the start bit and the number of bits needed by the L3 ECMP hash
*       mechanism. Those parameters are used to select the ECMP member
*       within the ECMP block according to one of the following formulas:
*       - when randomEnable bit in the ECMP entry is GT_FALSE:
*           ECMP member offset =
*           ((size of ECMP block) * (the value received between [startBit] and
*           [startBit + numOfBits - 1])) >> numOfBits
*       - when randomEnable bit in the ECMP entry is GT_TRUE:
*           ECMP member offset =
*           ((size of ECMP block) * (random seed value)) >> numOfBits
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       startBit        - the index of the first bit that is needed by the L3
*                         ECMP hash mechanism (APPLICABLE RANGES: 0..31)
*       numOfBits       - the number of bits that is needed by the L3 ECMP hash
*                         mechanism (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong startBit or numOfBits
*
* COMMENTS:
*       startBit is relevant only when randomEnable field in the entry is set to
*       GT_FALSE.
*       startBit + numOfBits must not exceed 32.
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpHashNumBitsSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       startBit,
    IN GT_U32                       numOfBits
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpEcmpHashNumBitsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, startBit, numOfBits));

    rc = internal_cpssDxChIpEcmpHashNumBitsSet(devNum, startBit, numOfBits);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, startBit, numOfBits));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpEcmpHashNumBitsGet
*
* DESCRIPTION:
*       Get the start bit and the number of bits needed by the L3 ECMP hash
*       mechanism. Those parameters are used to select the ECMP member
*       within the ECMP block according to one of the following formulas:
*       - when randomEnable bit in the ECMP entry is GT_FALSE:
*           ECMP member offset =
*           ((size of ECMP block) * (the value received between [startBit] and
*           [startBit + numOfBits - 1])) >> numOfBits
*       - when randomEnable bit in the ECMP entry is GT_TRUE:
*           ECMP member offset =
*           ((size of ECMP block) * (random seed value)) >> numOfBits
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       startBitPtr     - (pointer to) the index of the first bit that is needed
*                         by the L3 ECMP hash mechanism
*       numOfBitsPtr    - (pointer to) the number of bits that is needed by the
*                         L3 ECMP hash mechanism
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       startBit is relevant only when randomEnable field in the entry is set to
*       GT_FALSE.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpEcmpHashNumBitsGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *startBitPtr,
    OUT GT_U32                      *numOfBitsPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(startBitPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfBitsPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChTrunkLion3HashBitsSelectionGet(devNum,
                                                    PRV_CPSS_DXCH_TRUNK_LION3_HASH_CLIENT_TYPE_L3_ECMP_E,
                                                    startBitPtr,
                                                    numOfBitsPtr);
}

/*******************************************************************************
* cpssDxChIpEcmpHashNumBitsGet
*
* DESCRIPTION:
*       Get the start bit and the number of bits needed by the L3 ECMP hash
*       mechanism. Those parameters are used to select the ECMP member
*       within the ECMP block according to one of the following formulas:
*       - when randomEnable bit in the ECMP entry is GT_FALSE:
*           ECMP member offset =
*           ((size of ECMP block) * (the value received between [startBit] and
*           [startBit + numOfBits - 1])) >> numOfBits
*       - when randomEnable bit in the ECMP entry is GT_TRUE:
*           ECMP member offset =
*           ((size of ECMP block) * (random seed value)) >> numOfBits
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       startBitPtr     - (pointer to) the index of the first bit that is needed
*                         by the L3 ECMP hash mechanism
*       numOfBitsPtr    - (pointer to) the number of bits that is needed by the
*                         L3 ECMP hash mechanism
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       startBit is relevant only when randomEnable field in the entry is set to
*       GT_FALSE.
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpHashNumBitsGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *startBitPtr,
    OUT GT_U32                      *numOfBitsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpEcmpHashNumBitsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, startBitPtr, numOfBitsPtr));

    rc = internal_cpssDxChIpEcmpHashNumBitsGet(devNum, startBitPtr, numOfBitsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, startBitPtr, numOfBitsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpEcmpHashSeedValueSet
*
* DESCRIPTION:
*       Set the "seed value" parameter used by the L3 ECMP hash mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       seed            - the "seed value" parameter used by the L3 ECMP hash
*                         (APPLICABLE RANGES: 1..0xffffffff)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong seed
*
* COMMENTS:
*       The "seed value" parameter is relevant only for ECMP entries that are
*       configured with randomEnable == GT_TRUE.
*       0 is illegal seed value.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpEcmpHashSeedValueSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       seed
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (seed == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* set the seed value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->IPvX.ECMPRoutingConfig.ECMPSeed;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, seed);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* in order that the new seed value will be uploaded we need to set
       <Seed Load Enable> to 1 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->IPvX.ECMPRoutingConfig.ECMPConfig;
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, 1);
}

/*******************************************************************************
* cpssDxChIpEcmpHashSeedValueSet
*
* DESCRIPTION:
*       Set the "seed value" parameter used by the L3 ECMP hash mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       seed            - the "seed value" parameter used by the L3 ECMP hash
*                         (APPLICABLE RANGES: 1..0xffffffff)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong seed
*
* COMMENTS:
*       The "seed value" parameter is relevant only for ECMP entries that are
*       configured with randomEnable == GT_TRUE.
*       0 is illegal seed value.
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpHashSeedValueSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       seed
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpEcmpHashSeedValueSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, seed));

    rc = internal_cpssDxChIpEcmpHashSeedValueSet(devNum, seed);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, seed));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpEcmpHashSeedValueGet
*
* DESCRIPTION:
*       Get the "seed value" parameter used by the L3 ECMP hash mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       seedPtr         - (pointer to) the "seed value" parameter used by the L3
*                         ECMP hash
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The "seed value" parameter is relevant only for ECMP entries that are
*       configured with randomEnable == GT_TRUE.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpEcmpHashSeedValueGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *seedPtr
)
{
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(seedPtr);

    /* get the seed value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->IPvX.ECMPRoutingConfig.ECMPSeed;
    return prvCpssHwPpReadRegister(devNum, regAddr, seedPtr);
}

/*******************************************************************************
* cpssDxChIpEcmpHashSeedValueGet
*
* DESCRIPTION:
*       Get the "seed value" parameter used by the L3 ECMP hash mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       seedPtr         - (pointer to) the "seed value" parameter used by the L3
*                         ECMP hash
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The "seed value" parameter is relevant only for ECMP entries that are
*       configured with randomEnable == GT_TRUE.
*
*******************************************************************************/
GT_STATUS cpssDxChIpEcmpHashSeedValueGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *seedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpEcmpHashSeedValueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, seedPtr));

    rc = internal_cpssDxChIpEcmpHashSeedValueGet(devNum, seedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, seedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

