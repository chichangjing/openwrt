/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPcl.h
*
* DESCRIPTION:
*       CPSS DXCH private PCL lib API implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 50 $
*******************************************************************************/
#ifndef __prvCpssDxChPclh
#define __prvCpssDxChPclh

#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/tcam/private/prvCpssDxChTcam.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* there are 50 UDBs for IPCL/EPCL */
#define PRV_CPSS_DXCH_LION3_PCL_UDBS_NUM_CNS    50


/* Convert Attribute modify QoS command to hardware value */
#define PRV_CPSS_PCL_CONVERT_ATTR_MODIFY_TO_HW_VAL_MAC(_val, _cmd)          \
    switch (_cmd)                                                       \
    {                                                                   \
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:              \
            _val = 0;                                                   \
            break;                                                      \
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:                     \
            _val = 1;                                                   \
            break;                                                      \
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:                    \
            _val = 2;                                                   \
            break;                                                      \
        default:                                                        \
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                    \
    }

/* Convert hardware value to Attribute modify QoS command */
#define PRV_CPSS_PCL_CONVERT_HW_VAL_TO_ATTR_MODIFY_MAC(_cmd, _val)          \
    switch (_val)                                                       \
    {                                                                   \
        case 0:                                                         \
            _cmd = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;        \
            break;                                                      \
        case 1:                                                         \
            _cmd = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;               \
            break;                                                      \
        case 2:                                                         \
            _cmd = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;              \
            break;                                                      \
        default:                                                        \
            _cmd =  (_val);                                             \
    }


/* check that the devFamily is one of DxCh xCat and above */
/*                      - else return GT_BAD_PARAM        */
#define PRV_CPSS_DXCH_PCL_XCAT_DEV_CHECK_MAC(devNum)                 \
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))               \
    {                                                                \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                         \
    }



/* check rule size and index macro */
#define PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(_devNum, _ruleSize, _ruleIndex)  \
    {                                                                                \
        GT_STATUS _rc;                                                               \
        GT_U32    _absIndex;                                                         \
                                                                                     \
        _rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(                                \
            _devNum, _ruleSize, _ruleIndex, 0/*ruleSegment*/, &_absIndex);           \
        if (_rc != GT_OK)                                                            \
        {                                                                            \
            return _rc;                                                              \
        }                                                                            \
    }

/* Check PCL direction and return GT_BAD_PARAM if wrong */
#define PRV_CPSS_DXCH_PCL_DIRECTION_CHECK_MAC(_dir) \
    switch(_dir)                                    \
    {                                               \
        case CPSS_PCL_DIRECTION_INGRESS_E:          \
        case CPSS_PCL_DIRECTION_EGRESS_E:           \
            break;                                  \
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);               \
    }

/* get rule size value macro */
#define PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(_ruleSizeType, _ruleSizeVal) \
    switch (_ruleSizeType)                                                     \
    {                                                                          \
        case CPSS_PCL_RULE_SIZE_STD_E: _ruleSizeVal = 1;                       \
            break;                                                             \
        case CPSS_PCL_RULE_SIZE_EXT_E: _ruleSizeVal = 2;                       \
            break;                                                             \
        case CPSS_PCL_RULE_SIZE_ULTRA_E: _ruleSizeVal = 3;                     \
            break;                                                             \
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                          \
    }

/* Converts PCL rule format to rule size value */
#define PRV_CPSS_DXCH_PCL_CONVERT_RULE_FORMAT_TO_TCAM_RULE_SIZE_VAL_MAC(_tcamRuleSize, _ruleFormat) \
    switch (_ruleFormat)                                                                            \
    {                                                                                               \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E:                                            \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E:                                             \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_10_B_E;                                        \
            break;                                                                                  \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E:                                            \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E:                                             \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_20_B_E;                                        \
            break;                                                                                  \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E:                                            \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E:                                             \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_40_B_E;                                        \
            break;                                                                                  \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E:                                            \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E:                                             \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_50_B_E;                                        \
            break;                                                                                  \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:                                      \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:                                       \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:                                       \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:                                       \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:                                        \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:                                        \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:                                           \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E:                            \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E:                                 \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E:                                            \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E:                                             \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_60_B_E;                                        \
            break;                                                                                  \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E:                          \
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E:                         \
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E:                               \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_80_B_E;                                        \
            break;                                                                                  \
        default:                                                                                    \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_30_B_E;                                        \
            break;                                                                                  \
    }


/* Converts PCL rule size to rule size value */
#define PRV_CPSS_DXCH_PCL_CONVERT_RULE_SIZE_TO_TCAM_RULE_SIZE_VAL_MAC(_tcamRuleSize, _pclRuleSize)  \
    switch (_pclRuleSize)                                                                           \
    {                                                                                               \
        case CPSS_PCL_RULE_SIZE_STD_E:                                                              \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_30_B_E;                                        \
            break;                                                                                  \
        case CPSS_PCL_RULE_SIZE_EXT_E:                                                              \
            _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_60_B_E;                                        \
            break;                                                                                  \
        case CPSS_PCL_RULE_SIZE_ULTRA_E:                                                            \
             _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_80_B_E;                                       \
            break;                                                                                  \
        case CPSS_PCL_RULE_SIZE_10_BYTES_E:                                                         \
             _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_10_B_E;                                       \
            break;                                                                                  \
        case CPSS_PCL_RULE_SIZE_20_BYTES_E:                                                         \
             _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_20_B_E;                                       \
            break;                                                                                  \
        case CPSS_PCL_RULE_SIZE_40_BYTES_E:                                                         \
             _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_40_B_E;                                       \
            break;                                                                                  \
        case CPSS_PCL_RULE_SIZE_50_BYTES_E:                                                         \
             _tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_50_B_E;                                       \
            break;                                                                                  \
        default:                                                                                    \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                                    \
            break;                                                                                  \
    }

/* Check PCL lookup number and return GT_BAD_PARAM if wrong */
#define PRV_CPSS_DXCH_PCL_LOOKUP_NUM_CHECK_MAC(_lookup) \
    if(CPSS_PCL_LOOKUP_0_1_E < (_lookup))               \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

/* number of TCAM raws in the DxCh3 devices */
#define PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS           3584

/* number of IPCL Configuration Table rows in the Dx devices */
#define PRV_DXCH_IPCL_CONF_TBL_ROW_NUMBER_CNS(dev)   \
        (GT_U32)(                                    \
        PRV_CPSS_SIP_5_20_CHECK_MAC(dev) ? (_4K + 512) : /*4608*/ \
        PRV_CPSS_SIP_5_CHECK_MAC(dev)    ? (_4K + 256) : /*4352*/ \
        (_4K + 128))/*4224*/

/* number of EPCL Configuration Table rows in the Dx devices */
#define PRV_DXCH_EPCL_CONF_TBL_ROW_NUMBER_CNS(dev)    \
        (GT_U32)(                                     \
        PRV_CPSS_SIP_5_20_CHECK_MAC(dev) ? (_4K + 512) : /*4608*/ \
        PRV_CPSS_SIP_5_CHECK_MAC(dev)    ? (_4K + 256) : /*4352*/ \
        (_4K + 64))/*4160*/

/*
 * typedef enum PRV_CPSS_DXCH_PCL_TCAM_OPERATION
 *
 * Description:TCAM operations
 *
 * Enumerations:
 *
 *    PRV_CPSS_DXCH_PCL_TCAM_READ_ACTION         - read action
 *    PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION        - write action
 *    PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_VALID    -
 *                                 write TCAM pattern and valid
 *    PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID  -
 *                                 write TCAM pattern and invalid
 *    PRV_CPSS_DXCH_PCL_TCAM_WRITE_MASK          - write TCAM mask
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_PCL_TCAM_READ_ACTION,
    PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION,
    PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_VALID,
    PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID,
    PRV_CPSS_DXCH_PCL_TCAM_WRITE_MASK

} PRV_CPSS_DXCH_PCL_TCAM_OPERATION;

/*
 * typedef: struct PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC
 *
 * Description: PCL Configuration Table Entry Per Lookup Attributes.
 *              All values converted to HW format
 *
 * Fields:
 *    enableLookup           - Enable Lookup bit
 *    pclId                  - PCL-ID 10 bits
 *    dualLookup             - For xCat and above devices.
 *                             Enables lookup0_0.
 *    pclIdL01               - PCL-ID for lookup0_1 10 bits
 *                             For xCat and above devices.
 *    nonIpKeyType           - type of TCAM search key for NON-IP packets.
 *    ipv4KeyType            - type of TCAM search key for IPV4 packets
 *    ipv6KeyType            - type of TCAM search key for IPV6 packets
 *    udbKeyBitmapEnable     - replace 32 UDBs in the key by 256-bit bitmap
 *    tcamSegmentMode        - TCAM Segment Mode (for parallel lookup)
 *
 * Comments:
 *
 *   For Ch+ devices
 *   nonIpKeyType is a standard/extended key bit value
 *   ipv4KeyType  is L2QOS/L4 bit
 *               regarless than for exteded key it used only for IPV6
 *   ipv6KeyType  is IPV6-DIP LOOKUP1 value
 *               regarless it used only for standard IPV6 keys
 *   The "entry valid" bit 28 always set to 1 (valid)
 *
 *   The Write function ingnores not relevant fields, The read function
 *   returns zeros in not relevant fields.
 */
typedef struct
{
    GT_U8     enableLookup;
    GT_U32    pclId;
    GT_U8     dualLookup;
    GT_U32    pclIdL01;
    GT_U32    nonIpKeyType;
    GT_U32    ipv4KeyType;
    GT_U32    ipv6KeyType;
    GT_U32    udbKeyBitmapEnable;
    GT_U32    tcamSegmentMode;

} PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC;

/* amount of fields in Lookup configuration entry         */
/* fields described in PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC */
#define PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_AMOUNT_CNS 10

/* maximal number of Lookup configuration fields */
/* added "1" for DxCh1 "entry valid" bit         */
#define PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_MAX_NUM_CNS \
    (PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_AMOUNT_CNS + 1)

/*
 * typedef: struct PRV_PCL_CFG_LOOKUP_POS_STC
 *
 * Description: PCL Configuration Table Entry Per Lookup bit offsets and lengths.
 *              Not relevant data length specified as 0;
 *
 * Fields:
 *    enableLookupOff  - Offset of Enable Lookup bit
 *    enableLookupLen  - Length of Enable Lookup bit
 *    pclIdOff         - Offset of Lookp0_0 PCL-ID 10 bits
 *    pclIdLen         - Length of Lookp0_0 PCL-ID 10 bits
 *    nonIpKeyTypeOff  - Offset of type of TCAM search key for NON-IP packets.
 *    nonIpKeyTypeLen  - Length of type of TCAM search key for NON-IP packets.
 *    ipv4KeyTypeOff   - Offset of type of TCAM search key for IPV4 packets
 *    ipv4KeyTypeLen   - Length of type of TCAM search key for IPV4 packets
 *    ipv6KeyTypeOff   - Offset of type of TCAM search key for IPV6 packets
 *    ipv6KeyTypeLen   - Length of type of TCAM search key for IPV6 packets
 *    dualLookupOff    - Offset of Enable Lookup0_1 bit
 *    dualLookupLen    - Length of Enable Lookup bit
 *    pclIdL01Off      - Offset of Lookup0_1 PCL-ID 10 bits
 *    pclIdL01Len      - Length of Lookup0_1 PCL-ID 10 bits
 *
 * Comments:
 *   dualLookup - only for Ch3 devices IPCL lookup0
 *
 *   For Ch+ devices
 *   nonIpKeyType is a standard/extended key bit value
 *   ipv4KeyType  is L2QOS/L4 bit
 *               regarless than for exteded key it used only for IPV6
 *   ipv6KeyType  is IPV6-DIP LOOKUP1 value
 *               regarless it used only for standard IPV6 keys
 *   The "entry valid" bit 28 always set to 1 (valid)
 *
 *   The Write function ingnores not relevant fields, The read function
 *   returns zeros in not relevant fields.
 *   Not relevant offset (xxxOff) is 0xFF, not relevant length ( xxxLen) is 0.
 */
typedef struct
{
    GT_U8    enableLookupOff;
    GT_U8    enableLookupLen;
    GT_U8    pclIdOff;
    GT_U8    pclIdLen;
    GT_U8    nonIpKeyTypeOff;
    GT_U8    nonIpKeyTypeLen;
    GT_U8    ipv4KeyTypeOff;
    GT_U8    ipv4KeyTypeLen;
    GT_U8    ipv6KeyTypeOff;
    GT_U8    ipv6KeyTypeLen;
    GT_U8    dualLookupOff;
    GT_U8    dualLookupLen;
    GT_U8    pclIdL01Off;
    GT_U8    pclIdL01Len;
    GT_U8    udbKeyBitmapEnableOff;
    GT_U8    udbKeyBitmapEnableLen;
    GT_U8    tcamSegmentModeOff;
    GT_U8    tcamSegmentModeLen;
} PRV_PCL_CFG_LOOKUP_POS_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_PCL_RULE_STORE_PORT_GROUP_MODIFIED_FLD_STC
 * Description: Fields being converted due to port group.
 *
 * Fields:
 *    isEgress           - GT_TRUE - egress, GT_FALSE - ingress
 *    ruleSize           - ruleSize in standard rules
 *    srcPortMask        - srcPort field mask.
 *    srcPortPattern     - srcPort field pattern.
 *    portListBmpMask    - portListBmp field mask.
 *    portListBmpPattern - portListBmp field pattern.
 *
 * Comments:
 */
typedef struct
{
    GT_U32                             ruleSize;
    GT_BOOL                            isEgress;
    GT_U32                             srcPortMask;
    GT_U32                             srcPortPattern;
    GT_U32                             portListBmpMask;
    GT_U32                             portListBmpPattern;
} PRV_CPSS_DXCH_PCL_RULE_STORE_PORT_GROUP_MODIFIED_FLD_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_PCL_RULE_STC_PORT_GROUP_MODIFIED_PTRS_STC
 * Description: pointers to Sources of fields being converted due to port group.
 *
 * Fields:
 *    srcPortPtr        - (pointer to) srcPort field. - when NULL ignored
 *    portListBmpPtr    - (pointer to) portListBmp field mask. - when NULL ignored
 *
 * Comments:
 */
typedef struct
{
    GT_U32                             *srcPortPtr;
    CPSS_PORTS_BMP_STC                 *portListBmpPtr;
} PRV_CPSS_DXCH_PCL_RULE_STC_PORT_GROUP_MODIFIED_PTRS_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_PCL_RULE_SOURCE_PORT_GROUP_MODIFIED_FLD_STC
 * Description: Sources of fields being converted due to port group.
 *
 * Fields:
 *    isEgress              - GT_TRUE - egress, GT_FALSE - ingress
 *    ruleSize              - ruleSize in standard rules
 *    srcPortMask           - srcPort field mask.
 *    srcPortPattern        - srcPort field pattern.
 *    portListBmpMaskPtr    - (pointer to) portListBmp field mask.
 *    portListBmpPatternPtr - (pointer to) portListBmp field pattern.
 *
 * Comments:
 */
typedef struct
{
    GT_U32                             ruleSize;
    GT_BOOL                            isEgress;
    GT_U32                             srcPortMask;
    GT_U32                             srcPortPattern;
    CPSS_PORTS_BMP_STC                 *portListBmpMaskPtr;
    CPSS_PORTS_BMP_STC                 *portListBmpPatternPtr;
} PRV_CPSS_DXCH_PCL_RULE_SOURCE_PORT_GROUP_MODIFIED_FLD_STC;


/*******************************************************************************
* prvCpssDxChLion2PclTcamParityCalc
*
* DESCRIPTION:
*       Function calculates parity values for Policy TCAM standard rules.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       xDataArr        - X data array.
*       yDataArr        - Y data array.
*
* OUTPUTS:
*       xParityPtr      - (pointer to) calculated X parity
*       yParityPtr      - (pointer to) calculated Y parity
*
* RETURNS :
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvCpssDxChLion2PclTcamParityCalc
(
    IN  GT_U32 xDataArr[8],
    IN  GT_U32 yDataArr[8],
    OUT GT_U32 *xParityPtr,
    OUT GT_U32 *yParityPtr
);


/*******************************************************************************
* prvCpssDxChPclLookupParamCheck
*
* DESCRIPTION:
*       Checks lookup paraqmeters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress lookup0
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclLookupParamCheck
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleAbsoluteIndexGet
*
* DESCRIPTION:
*       Gets absolute rule index, i.e. it's action index in action table.
*       It also is an index of the first standard-sized part of the rule.
*       The application rule index is an amount of rules with the same
*       size that can be placed before the given rule
*       In complicated TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - sizeof rule, measured in standard rules
*       ruleIndex       - index of rule
*       ruleSegment     - the number of STD-rule sized segment of rule
* OUTPUTS:
*       ruleAbsIndexPtr - (pointer to) rule absolute index
*
* RETURNS :
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleAbsoluteIndexGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ruleSize,
    IN  GT_U32                            ruleIndex,
    IN  GT_U32                            ruleSegment,
    OUT GT_U32                            *ruleAbsIndexPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamOperationWait
*
* DESCRIPTION:
*       Waits for operation completion (if started)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamOperationWait
(
    IN  GT_U8                         devNum
);

/*******************************************************************************
* prvCpssDxChPclTcamOperationStart
*
* DESCRIPTION:
*       Starts Read/Write/Invalidate rule(standard and extended)
*       Mask/Pattern/Action
*       Does exchance between intermediate regsters set and TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule, (APPLICABLE RANGES: 0..1023), extended (APPLICABLE RANGES: 0..511)
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*       tcamOperation  - read Action, write Mask/Pattern/Action, invalidate
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamOperationStart
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ruleSize,
    IN  GT_U32                            ruleIndex,
    IN  GT_BOOL                           waitPrevReady,
    IN  PRV_CPSS_DXCH_PCL_TCAM_OPERATION  tcamOperation
);

/*******************************************************************************
* prvCpssDxChPclTcamOperDataPut
*
* DESCRIPTION:
*       Writes PCE(standard and extended) Mask/Pattern/Action data
*       to addressable regster set for use by TCAM operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - sizeof rule, measured in standard rules
*       tcamOperation  - write Mask/Pattern/Action
*       dataPtr        - data - 3 words for action,
*                               6/12 bytes for pattern/mask
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       other           - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamOperDataPut
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ruleSize,
    IN  PRV_CPSS_DXCH_PCL_TCAM_OPERATION  tcamOperation,
    IN  GT_U32                            *dataPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamActionGet
*
* DESCRIPTION:
*       Reads PCE(standard and extended) Action data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of PCE  - owner of action to read
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*                        relevant only for DxCh1 and DxCh2 indirect read
*                        ignored for DxCh3 and above devices
* OUTPUTS:
*       dataPtr        - HW data for action
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamActionGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       waitPrevReady,
    OUT GT_U32                        *dataPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamReadStdRuleAddrGet
*
* DESCRIPTION:
*       Calculate standard rule Mask/Pattern direct read address
*       both data and control registers
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule  - owner of action to read
*       wordIndex      - index of word in the rule
*       pattern        - GT_TRUE - pattern, else mask
* OUTPUTS:
*       addrPtr        - address of:  6 short rule words for pattern/mask
*                        the long rule represented as two shorts
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*       For Cheetah line consists of 8 registers
*           0,1,2,4,5,6 - 32-bit data,
*           3,7 - control (bit1 - "valid", bit3 - "long")
*       For Cheetah2 line consists of 10 registers
*           called in the document "data0-4" and "control0-4"
*           the control1 and control2 also contain 16 bits of data.
*           The short rule contains 24 * 8 = 192 bits,
*           "control" means bit19-18 "compMode", bit17 "valid"
*           "compMode" is 1 - short, 2 - long in pattern and 3 in mask
*           The representation in this program is
*           word0 - data-bit31:0
*           word1 - "control"
*           word2 - data-bit63:32
*           word3 - "control" data-bit79:64
*           word4 - data-bit111:80
*           word5 - "control" data-bit127:112
*           word6 - data-bit159:128
*           word7 - "control"
*           word8 - data-bit191:160
*           word9 - "control"
*       For Cheetah3 line consists of 8 registers
*           called in the document "data0-3" and "control0-3
*           the control0-3 also contain 16 bits of data each (bits 15:0).
*           The short rule contains 24 * 8 = 192 bits,
*           "control" means bit19-18 "compMode", bit17 "valid" , bit16 "spare"
*           "compMode" is 1 - standard, 2 - extended, 3 - ultra
*           The representation in this program is
*           word0 - data-bit31:0
*           word1 - "control" data-bit47:32
*           word2 - data-bit79:48
*           word3 - "control" data-bit95:80
*           word4 - data-bit127:96
*           word5 - "control" data-bit143:128
*           word6 - data-bit175:144
*           word7 - "control" data-bit191:176
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamReadStdRuleAddrGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        wordIndex,
    IN  GT_BOOL                       pattern,
    OUT GT_U32                        *addrPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleDataGet
*
* DESCRIPTION:
*       Reads standard rule Mask/Pattern data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule  - owner of action to read
*       pattern        - GT_TRUE - pattern, else mask
* OUTPUTS:
*       dataPtr        - data:  6 words for pattern/mask
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*        Cheetah and  Cheetah2, not relevant for Cheetah3
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleDataGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       pattern,
    OUT GT_U32                        *dataPtr
);

/*******************************************************************************
*  prvCpssDxChPclTcamRuleDataGet
*
* DESCRIPTION:
*       Reads rule Mask/Pattern data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
* OUTPUTS:
*       maskPtr        - data:  (ruleSize * 6) words for mask
*       patternPtr     - data:  (ruleSize * 6) words for pattern
*                        for xCat2 and above: (ruleSize * 7) words
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleDataGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_U32                        *maskPtr,
    OUT GT_U32                        *patternPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleStateGet
*
* DESCRIPTION:
*       Reads standard rule control and converts it to state
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule  - owner of action to read
* OUTPUTS:
*       validPtr       -  GT_TRUE - valid
*       ruleSizePtr    -  sizeof rule, measured in standard rules
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*        Cheetah and  Cheetah2, not relevant for Cheetah3
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT GT_U32                        *ruleSizePtr
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleStateGet
*
* DESCRIPTION:
*       Reads standard rule control and converts it to state
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
* OUTPUTS:
*       validPtr            -  GT_TRUE - valid
*       ruleFoundSizePtr    -  sizeof rule, measured in standard rules
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT GT_U32                        *ruleFoundSizePtr
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleWrite
*
* DESCRIPTION:
*       Writes standard or extended rule Action, Mask and Pattern to TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
*       validRule      - GT_TRUE - valid, GT_FALSE - invalid
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*       actionPtr      - action
*       maskPtr        - mask
*       patternPtr     - pattern
* OUTPUTS:
*       None.
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*       For all devices the mask and pattern of rule
*       is a sequence standard sized segments.
*       For all CH devices, xCat and Lion such segment is
*       192 bits in 6 32-bit words.
*       For xCat2 and above devices such segment is
*       206 bits in 7 32-bit words, padded 18 bits unused.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleWrite
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       validRule,
    IN  GT_BOOL                       waitPrevReady,
    IN  GT_U32                        *actionPtr,
    IN  GT_U32                        *maskPtr,
    IN  GT_U32                        *patternPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleActionUpdate
*
* DESCRIPTION:
*       Writes standard or extended rule Action to TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
*       actionPtr      - action
* OUTPUTS:
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleActionUpdate
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        *actionPtr
);

/*******************************************************************************
* prvCpssDxChPclRuleCopy
*
* DESCRIPTION:
*  The function dedicated for one of port groups of multi port group device or
*  for not multi port group device.
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclRuleCopy should be
*  called. And after this cpssDxChPclRuleInvalidate should be used to invalidate
*  Rule in old position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupId           - The port group Id. relevant only to 'multi-port-groups' devices.
*                          supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSizeVal      - size of Rule in STD rules.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN GT_U32                             portGroupId,
    IN GT_U32                             ruleSizeVal,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
);

/*******************************************************************************
* prvCpssDxChPclLion3RuleCopy
*
* DESCRIPTION:
*       This function copies the Rule's mask, pattern and action to new TCAM position.
*       The source Rule is not invalidated by the function. To implement move TCAM
*       Rule from old position to new one at first prvCpssDxChTcamRuleCopy
*       should be called. And after this cpssDxChTcamGortGroupRuleValidStatusSet should
*       be used to invalidate rule in old position.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                             supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize            - size of rule.
*       ruleSrcIndex        - index of the rule in the TCAM from which pattern,
*                             mask and action are taken.
*       ruleDstIndex        - index of the rule in the TCAM to which pattern,
*                             mask and action are placed
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter, and when the rule cannot start in a given index.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclLion3RuleCopy
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  CPSS_DXCH_TCAM_RULE_SIZE_ENT  ruleSize,
    IN  GT_U32                        ruleSrcIndex,
    IN  GT_U32                        ruleDstIndex
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleActionSw2HwConvert
*
* DESCRIPTION:
*       Converts TCAM Action from SW to HW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       swActionPtr      - action in SW format
* OUTPUTS:
*       hwActionPtr      - action in HW format (4 words)
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_OUT_OF_RANGE  - one of the parameters is out of range.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleActionSw2HwConvert
(
    IN  GT_U8                         devNum,
    IN  CPSS_DXCH_PCL_ACTION_STC      *swActionPtr,
    OUT GT_U32                        *hwActionPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleActionHw2SwConvert
*
* DESCRIPTION:
*       Converts TCAM Action from HW to SW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       direction        - Policy direction:
*                          Ingress or Egress
*                          Needed for parsing
*       hwActionPtr      - action in HW format (4 words)
* OUTPUTS:
*       swActionPtr      - action in SW format
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_BAD_STATE     - one of the values not supported.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleActionHw2SwConvert
(
    IN  GT_U8                         devNum,
    IN CPSS_PCL_DIRECTION_ENT         direction,
    IN  GT_U32                        *hwActionPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC      *swActionPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleDataSw2HwConvert
*
* DESCRIPTION:
*       Converts TCAM pattern or mask from SW to HW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleFormat     - one of 13 supported key formats
*       swDataPtr      - rule pattern or mask in SW format
* OUTPUTS:
*       hwDataPtr      - rule pattern or mask in HW format (6 or 12 words)
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_OUT_OF_RANGE  - one of the parameters is out of range.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleDataSw2HwConvert
(
    IN  GT_U8                              devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_UNT      *swDataPtr,
    OUT GT_U32                             *hwDataPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleDataHw2SwConvert
*
* DESCRIPTION:
*       Converts TCAM pattern or mask from HW to SW format
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       ruleFormat     - format of the Rule.
*       hwDataPtr      - rule pattern or mask in HW format.
* OUTPUTS:
*       swDataPtr      - rule pattern or mask in SW format
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_OUT_OF_RANGE  - one of the parameters is out of range.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleDataHw2SwConvert
(
    IN  GT_U8                              devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN  GT_U32                             *hwDataPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT      *swDataPtr
);

/*******************************************************************************
* prvCpssDxChPclCfgTblEntryIndexGet
*
* DESCRIPTION:
*    Calculates the index of PCL Configuration table entry's for given interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: either port or VLAN
*       direction         - Policy direction:
*                           Ingress or Egress
*
* OUTPUTS:
*       indexPtr          - (pointer to) calculated index
*       portGroupIdPtr    - (pointer to) the port group Id that the index refers to.
*                               needed to support multi-port-groups device.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_HW_ERROR        - on HW error
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclCfgTblEntryIndexGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC         *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    OUT GT_U32                          *indexPtr,
    OUT GT_U32                          *portGroupIdPtr
);

/*******************************************************************************
* prvCpssDxChPclCfgTblHwSelectTblAndPos
*
* DESCRIPTION:
*    Selects the PCL Configuration table HW id and the field positions descriptor.
*    The position descriptor is a SW structure that contains offsets lengts
*    of the relevasnt subfields of the table entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       subLookupNum      - Sub lookup Number (APPLICABLE DEVICES: xCat2)
*
* OUTPUTS:
*       tableIdPtr        - (pointer to) HW table Id
*       posPtrPtr         - (pointer to) (pointer to) position descriptor
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclCfgTblHwSelectTblAndPos
(
    IN  GT_U8                                  devNum,
    IN  CPSS_PCL_DIRECTION_ENT                 direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT             lookupNum,
    IN  GT_U32                                 subLookupNum,
    OUT PRV_CPSS_DXCH_TABLE_ENT                *tableIdPtr,
    OUT PRV_PCL_CFG_LOOKUP_POS_STC             **posPtrPtr
);

/*******************************************************************************
* prvCpssDxChPclCfgTblHwWrite
*
* DESCRIPTION:
*    Write PCL Configuration table entry (fields in HW format)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId            - port group Id , to support multi-port-groups device
*       direction         - Policy direction:
*                           Ingress or Egress
*       entryIndex        - entry Index
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       subLookupNum      - Sub lookup Number (APPLICABLE DEVICES: xCat2)
*       lookupCfgPtr      - HW lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclCfgTblHwWrite
(
    IN GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN CPSS_PCL_DIRECTION_ENT                 direction,
    IN GT_U32                                 entryIndex,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT             lookupNum,
    IN  GT_U32                                subLookupNum,
    IN PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC    *lookupCfgPtr
);

/*******************************************************************************
* prvCpssDxChPclCfgTblHwRead
*
* DESCRIPTION:
*    Read PCL Configuration table entry (fields in HW format)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId            - port group Id , to support multi-port-groups device
*       direction         - Policy direction:
*                           Ingress or Egress
*       entryIndex        - entry Index
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       subLookupNum      - Sub lookup Number (APPLICABLE DEVICES: xCat2)
*
* OUTPUTS:
*       lookupCfgPtr      - HW lookup configuration
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclCfgTblHwRead
(
    IN  GT_U8                                  devNum,
    IN  GT_U32                                 portGroupId,
    IN  CPSS_PCL_DIRECTION_ENT                 direction,
    IN  GT_U32                                 entryIndex,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT             lookupNum,
    IN  GT_U32                                 subLookupNum,
    OUT PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC    *lookupCfgPtr
);

/* CH3 specific utils */

/* DXCH3 devices keeps X/Y tcam data format                     */
/* (0,0) means "match any value", (1,1) - "match nothing"       */
/* (0,1) - "match 0", (1,0) - "match 1"                         */
/* such conversions will be provided for one-to-one mapping     */

/* mask | pattern | X | Y |  comment            */
/*  0   | 0       | 0 | 0 |  "match any value"  */
/*  0   | 1       | 1 | 1 |  "match nothing"    */
/*  1   | 0       | 0 | 1 |  "match 0"          */
/*  1   | 1       | 1 | 0 | " match 1"          */


/* mask and pattern -> X */
#define PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(_m, _p) (_p)

/* mask and pattern -> Y */
#define PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(_m, _p) (_m ^ _p)

/* XY-> mask */
#define PRV_CPSS_DXCH_PCL_XY_TO_M_MAC(_x, _y) (_x ^ _y)

/* XY-> pattern */
#define PRV_CPSS_DXCH_PCL_XY_TO_P_MAC(_x, _y) (_x)

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleDataXandYGet
*
* DESCRIPTION:
*       Reads standard rule X and Y data
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*
* OUTPUTS:
*       xPtr          - data:  6 words for X
*       yPtr          - data:  6 words for Y
*                       For xCat2 and above devices - 7 words.
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleDataXandYGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    OUT GT_U32                        *xPtr,
    OUT GT_U32                        *yPtr
);

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleStateXandYGet
*
* DESCRIPTION:
*       Reads standard rule X and Y control bits of the 0-th 48-bit column of 4
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*
* OUTPUTS:
*       xValidPtr     -  xValidPtr
*       yValidPtr     -  yValidPtr
*       xSizePtr      -  xSizePtr
*       ySizePtr      -  ySizePtr
*       xSparePtr     -  xSparePtr
*       ySparePtr     -  ySparePtr
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleStateXandYGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    OUT GT_U32                        *xValidPtr,
    OUT GT_U32                        *yValidPtr,
    OUT GT_U32                        *xSizePtr,
    OUT GT_U32                        *ySizePtr,
    OUT GT_U32                        *xSparePtr,
    OUT GT_U32                        *ySparePtr
);

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleValidStateXandYSet
*
* DESCRIPTION:
*       Writes standard rule X and Y valid bits of the 0-th 48-bit column of 4
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*       xValid         -  xValid
*       yValid         -  yValid
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleValidStateXandYSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        xValid,
    IN  GT_U32                        yValid
);

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleValidStateSet
*
* DESCRIPTION:
*       Writes standard rule X and Y valid bits
*       The algorithms for xCat2 and above and
*       the algorithm for DxCh3, xCat, xCat3, Lion are different.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*       valid          -  valid
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleValidStateSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       valid
);

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleXandYSet
*
* DESCRIPTION:
*       Writes standard rule X and Y data and control bits
*       The first 48-bit column control bits supposed to be invalid and will
*       be overwritten last
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       portGroupId        - The port group Id. relevant only to 'multi-port-groups' devices.
*                       supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex     - index of rule
*       xPtr          - data:  6 words for X
*       yPtr          - data:  6 words for Y
*       xValid0       - xValid0 bit of the word0
*       yValid0       - yValid0 bit of the word0
*       xValid        - xValid  bit of the words 1-5
*       yValid        - yValid  bit of the words 1-5
*       xSize         - xSize
*       ySize         - ySize
*       xSpare        - xSpare
*       ySpare        - ySpare
*
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleXandYSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        *xPtr,
    IN  GT_U32                        *yPtr,
    IN  GT_U32                        xValid0,
    IN  GT_U32                        yValid0,
    IN  GT_U32                        xValid,
    IN  GT_U32                        yValid,
    IN  GT_U32                        xSize,
    IN  GT_U32                        ySize,
    IN  GT_U32                        xSpare,
    IN  GT_U32                        ySpare
);

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleSizeAndValidSet
*
* DESCRIPTION:
*       Writes standard rule X and Y data and control bits
*       The previous TCAM rule state supposed to be invalid and will
*       be overwritten last.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId        - The port group Id. relevant only to 'multi-port-groups' devices.
*                       supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex     - index of rule
*       xPtr          - data:  6 words for X
*       yPtr          - data:  6 words for Y
*                       xPtr and yPtr for xCat2 and above devices are 7 words
*                       The contain 206-bit data starting with pclId and
*                       not contain the 2-bit rule size.
*       validRule     - valid
*       ruleSize      - rule Size (APPLICABLE RANGES: 1..3)
*
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleSizeAndValidSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        *xPtr,
    IN  GT_U32                        *yPtr,
    IN  GT_BOOL                       validRule,
    IN  GT_U32                        ruleSize
);

/*******************************************************************************
* prvCpssDxChPclRuleDataTo7WordsAdjust
*
* DESCRIPTION:
*   The function adjusts the data converted from SW to HW format
*   to 7 words per standard rule format - devices that use only 2 first
*   bits as control bits (from 52 * 4 bits of standard rule).
*   1) Drugging high rule segments by such way that each 6-word segment
*      will be padded by the 7-th word contains zero.
*   2) For egress rules the first segment of the rule left shifted to 1
*      because the "bit0 == valid" not exists for xCat2 and above devices.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       isEgress         - GT_TRUE egress format, GT_FALSE ingress format.
*       ruleSize         - amount of Standard segments of rule.
*       hwMaskArr        - array with  pattern converted to HW format
*       hwPatternArr     - array with pattern converted to HW format
*
* OUTPUTS:
*       hwMaskArr        - array with  pattern converted to HW format
*       hwPatternArr     - array with pattern converted to HW format
*
* RETURNS:
*       None.
*
* COMMENTS:
*   The xCat2 specific "key size" bits are added at low level write
*   function prvCpssDxChPclTcamStdRuleSizeAndValidSet.
*******************************************************************************/
GT_VOID prvCpssDxChPclRuleDataTo7WordsAdjust
(
    IN     GT_BOOL                                  isEgress,
    IN     GT_U32                                   ruleSize,
    INOUT  GT_U32                                   hwMaskArr[],
    INOUT  GT_U32                                   hwPatternArr[]
);

/*******************************************************************************
* prvCpssDxChPclRuleGetDirAndSize
*
* DESCRIPTION:
*   The function Gets Direction and size of rule format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ruleFormat       - format of the Rule.
*
* OUTPUTS:
*       isEgressPtr      - (pointer to) GT_TRUE - egress, GT_FALSE - ingress
*       ruleSizePtr      - (pointer to) size of the rule in standard rules
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleGetDirAndSize
(
    IN   CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    OUT  GT_BOOL                            *isEgressPtr,
    OUT  GT_U32                             *ruleSizePtr
);

/*******************************************************************************
* prvCpssDxChPclRuleGetModifiedFieldsSourcesPointers
*
* DESCRIPTION:
*   The function Gets pointers of source fields modified for port group id.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       ruleFormat       - format of the Rule.
*       rulePtr          - (pointer to) rule.
*
* OUTPUTS:
*       fldSrcPtr        - (pointer to) structure to get pointers into rulePtr .
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleGetModifiedFieldsSourcesPointers
(
    IN   CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT                      ruleFormat,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_UNT                           *rulePtr,
    OUT  PRV_CPSS_DXCH_PCL_RULE_STC_PORT_GROUP_MODIFIED_PTRS_STC *fldSrcPtr
);

/*******************************************************************************
* prvCpssDxChPclRuleGetModifiedFieldsSources
*
* DESCRIPTION:
*   The function Gets source of fields modified for port group id.
*   and the port list bmp needed from xCat2.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       ruleFormat       - format of the Rule.
*       maskPtr          - (pointer to) rule mask
*       patternPtr       - (pointer to) rule pattren
*
* OUTPUTS:
*       fldSrcPtr        - (pointer to) structure to get results in it.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*       The function used also for xCat2 becase the port list bitmap
*       treated only via this function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleGetModifiedFieldsSources
(
    IN   CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT                        ruleFormat,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_UNT                             *maskPtr,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_UNT                             *patternPtr,
    OUT  PRV_CPSS_DXCH_PCL_RULE_SOURCE_PORT_GROUP_MODIFIED_FLD_STC *fldSrcPtr
);

/*******************************************************************************
* prvCpssDxChPclRuleDataConvertForPortGroup
*
* DESCRIPTION:
*   The function converts source port and port bitmap fields values
*   according to port-group Id.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum           - device number
*       portGroupId      - port Groups Id for multi port group devices
*       fldSrcPtr        - source values of the fields
*
* OUTPUTS:
*       storeFldPtr      - converted values of the fields
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleDataConvertForPortGroup
(
    IN   GT_U8                                                     devNum,
    IN   GT_U32                                                    portGroupId,
    IN   PRV_CPSS_DXCH_PCL_RULE_SOURCE_PORT_GROUP_MODIFIED_FLD_STC *fldSrcPtr,
    OUT  PRV_CPSS_DXCH_PCL_RULE_STORE_PORT_GROUP_MODIFIED_FLD_STC  *storeFldPtr
);

/*******************************************************************************
* prvCpssDxChPclRuleDataStorePortGroupModifiedFields
*
* DESCRIPTION:
*   The function stores such fields that values of them
*   depend of port-group owning the TCAM
*   For Bobcat2; Caelum; Bobcat3 they are source port and port list bitmap.
*   The field value calculation done out of this function.
*   This function just adds the values to rule in HW format.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum           - device number
*       addedFldPtr      - fields with port-group depended values
*       hwMaskArr        - array with mask converted to HW format
*       hwPatternArr     - array with pattern converted to HW format
*
* OUTPUTS:
*       hwMaskArr        - array with mask adjusted for xCat2
*       hwPatternArr     - array with pattern adjusted for xCat2
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong addedFldPtr data
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleDataStorePortGroupModifiedFields
(
    IN     GT_U8                                                    devNum,
    IN     PRV_CPSS_DXCH_PCL_RULE_STORE_PORT_GROUP_MODIFIED_FLD_STC *addedFldPtr,
    INOUT  GT_U32                                                   hwMaskArr[],
    INOUT  GT_U32                                                   hwPatternArr[]
);

/*******************************************************************************
* prvCpssDxChPclRuleDataLoadPortGroupModifiedFields
*
* DESCRIPTION:
*   The function loads such fields that values of them
*   depend of port-group owning the TCAM
*   For Bobcat2; Caelum; Bobcat3 they are source port and port list bitmap.
*   This function just loads the values from rule in HW format.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum           - device number
*       hwMaskArr        - array with mask converted to HW format
*       hwPatternArr     - array with pattern converted to HW format
*       addedFldPtr      -  isEgress and ruleSize are input fields
*
* OUTPUTS:
*       addedFldPtr      - fields with port-group depended values
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleDataLoadPortGroupModifiedFields
(
    IN     GT_U8                                                    devNum,
    IN     GT_U32                                                   hwMaskArr[],
    IN     GT_U32                                                   hwPatternArr[],
    INOUT  PRV_CPSS_DXCH_PCL_RULE_STORE_PORT_GROUP_MODIFIED_FLD_STC *addedFldPtr
);

/*******************************************************************************
* prvCpssDxChPclRuleDataAccumulateFromPortGroup
*
* DESCRIPTION:
*   The function accumulates source port and port bitmap fields values
*   using values retrieved to port-group Id.
*   Conversion done if needed.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       portGroupId       - port Groups Id for multi port group devices
*       storeFldPtr       - (pointer to)HW values of the fields
*
* OUTPUTS:
*       maskSrcPtrsPtr    - (pointer to)pointers into the mask rule structure
*       patternSrcPtrsPtr - (pointer to)pointers into the pattern rule structure
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleDataAccumulateFromPortGroup
(
    IN   GT_U8                                                      devNum,
    IN   GT_U32                                                     portGroupId,
    IN   PRV_CPSS_DXCH_PCL_RULE_STORE_PORT_GROUP_MODIFIED_FLD_STC   *storeFldPtr,
    OUT  PRV_CPSS_DXCH_PCL_RULE_STC_PORT_GROUP_MODIFIED_PTRS_STC    *maskSrcPtrsPtr,
    OUT  PRV_CPSS_DXCH_PCL_RULE_STC_PORT_GROUP_MODIFIED_PTRS_STC    *patternSrcPtrsPtr
);

/*******************************************************************************
* prvCpssDxChPclRuleDataSw2HwConvert
*
* DESCRIPTION:
*   The function Converts the Policy Rule Mask, Pattern and Action
*   to HW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       hwMaskArr        - array for mask converted to HW format
*       hwPatternArr     - array for pattern converted to HW format
*       hwActionArr      - array for action converted to HW format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleDataSw2HwConvert
(
    IN   GT_U8                              devNum,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN   CPSS_DXCH_PCL_ACTION_STC           *actionPtr,
    OUT  GT_U32                             hwMaskArr[],
    OUT  GT_U32                             hwPatternArr[],
    OUT  GT_U32                             hwActionArr[]
);

/*******************************************************************************
* prvCpssDxChPclTcamRuleModifiedWrite
*
* DESCRIPTION:
*       Writes rule Action, Mask and Pattern to TCAM.
*       Supports rules of any sizes.
*       Supports SrcPort field modification for multi port group devices
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
*       egressRule     - GT_TRUE - egress Rule, GT_FALSE ingress
*       validRule      - GT_TRUE - valid, GT_FALSE - invalid
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*                        relevant only for DxCh1 and DxCh2 devices,
*                        ignored for DxCh3 and above devices
*       actionPtr      - action
*       maskPtr        - mask
*       patternPtr     - pattern
* OUTPUTS:
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleModifiedWrite
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       egressRule,
    IN  GT_BOOL                       validRule,
    IN  GT_BOOL                       waitPrevReady,
    IN  GT_U32                        *actionPtr,
    IN  GT_U32                        *maskPtr,
    IN  GT_U32                        *patternPtr
);

/*******************************************************************************
* prvCpssDxChPclActionSizeAndAllignmentGet
*
* DESCRIPTION:
*       Gets action size and alignment.
*       extern for parsing in log
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       sizePtr        - (pointer to) amount words in HW action
*       alignmentPtr   - (pointer to) alignment of HW action in words
*
* RETURNS :
*       None.
*
* COMMENTS:
*
*******************************************************************************/
extern GT_VOID prvCpssDxChPclActionSizeAndAllignmentGet
(
    IN  GT_U8                         devNum,
    OUT GT_U32                        *sizePtr,
    OUT GT_U32                        *alignmentPtr
);

/* size of HW <==> SW Key TYpe conversion tables */
#define PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS  8

/* Packet type Ehernet Other */
#define PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS   0

/* Packet type IPV4 */
#define PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS     1

/* Packet type IPV6 */
#define PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS     2

/* reserved HW value stamp  */
#define PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS 0xFF

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChPclh */

