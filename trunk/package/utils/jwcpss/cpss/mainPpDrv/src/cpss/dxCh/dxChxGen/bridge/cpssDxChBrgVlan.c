/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChBrgVlan.c
*
* DESCRIPTION:
*       CPSS DxCh VLAN facility implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 127 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrg.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>

/* max index of entry that may be configured to support
   port protocol based VLANs and QoS */
#define PRV_CPSS_DXCH_PORT_PROT_ENTRY_NUM_MAX_CNS  (7)

/* for xCat: max index of entry that may be configured
   to support port protocol based VLANs and QoS */
#define PRV_CPSS_DXCH_XCAT_PORT_PROT_ENTRY_NUM_MAX_CNS  (11)

/* maximal index of analyzer */
#define  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS 6

/*  check Port Protocol range, only 8 and 12(for xCat and above)
    protocols are supported */
#define PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(_devNum, _entryNum)    \
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(_devNum))              \
    {                                                                    \
        if((_entryNum) > PRV_CPSS_DXCH_XCAT_PORT_PROT_ENTRY_NUM_MAX_CNS) \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                         \
    }                                                                    \
    else                                                                 \
    {                                                                    \
        if((_entryNum) > PRV_CPSS_DXCH_PORT_PROT_ENTRY_NUM_MAX_CNS)      \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                         \
    }

/* max number of MRU indexes */
#define PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS     (7)

/* check MRU indx range */
#define PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(mruInd)   \
    if ((mruInd) > PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS) \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

/* check and convert IPM Bridging mode */
#define PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(_ipmBrgMode, _hwValue) \
    switch(_ipmBrgMode)                                           \
    {                                                             \
        case CPSS_BRG_IPM_SGV_E: _hwValue = 0; break;             \
        case CPSS_BRG_IPM_GV_E:  _hwValue = 1; break;             \
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                             \
    }

/* Converts flood VIDX mode to hardware value */
#define PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(_val, _cmd) \
    switch (_cmd)                                                       \
    {                                                                   \
        case CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E:             \
            _val = 0;                                                   \
            break;                                                      \
        case CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E:  \
            _val = 1;                                                   \
            break;                                                      \
        default:                                                        \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                        \
    }

#define PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(_val, _cmd)  \
    switch (_cmd)                                                            \
    {                                                                        \
        case CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E:                \
            _val = 0;                                                        \
            break;                                                           \
        case CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E:                     \
            _val = 1;                                                        \
            break;                                                           \
        case CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E:                     \
            _val = 2;                                                        \
            break;                                                           \
        case CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E:                  \
            _val = 3;                                                        \
            break;                                                           \
        default:                                                             \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                             \
    }

#define PRV_CPSS_DXCH_CONVERT_VLAN_TAG_MODE_TO_HW_VAL_MAC(_devNum, _val, _cmd)       \
        switch(_cmd)                                                        \
        {                                                                   \
            case CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E                :    \
                _val = 0;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E                    :    \
                _val = 1;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E                    :    \
                _val = 2;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E   :    \
                _val = 3;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E   :    \
                _val = 4;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E               :    \
                if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)\
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                    \
                _val = 5;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E           :    \
                _val = 6;                                                   \
                break;                                                      \
            case CPSS_DXCH_BRG_VLAN_PORT_DO_NOT_MODIFY_TAG_CMD_E       :    \
                if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)\
                    _val = 7;                                               \
                else                                                        \
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                    \
                break;                                                      \
            default:                                                        \
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                        \
        }

#define PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_VLAN_TAG_MODE_MAC(_devNum, _val, _cmd)\
            switch(_val)                                                    \
            {                                                               \
                case 0:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;          \
                    break;                                                  \
                case 1:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;              \
                    break;                                                  \
                case 2:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;              \
                    break;                                                  \
                case 3:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;\
                    break;                                                  \
                case 4:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;\
                    break;                                                  \
                case 5:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E;         \
                    break;                                                  \
                case 6:                                                     \
                    _cmd = CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E;     \
                    break;                                                  \
                case 7:                                                     \
                    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)\
                        _cmd = CPSS_DXCH_BRG_VLAN_PORT_DO_NOT_MODIFY_TAG_CMD_E; \
                    else                                                        \
                        _cmd = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)_val;       \
                    break;                                                  \
                default:                                                    \
                    _cmd = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)_val;       \
                    break;                                                  \
            }

#define PRV_CPSS_DXCH_CONVERT_UNREG_IPM_EVIDX_MODE_TO_HW_VAL_MAC(_val, _mode)       \
            switch(_mode)                                                           \
            {                                                                       \
                case CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_E:                \
                    _val = 0;                                                       \
                    break;                                                          \
                case CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV4_E:                \
                    _val = 1;                                                       \
                    break;                                                          \
                case CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV6_E:                \
                    _val = 2;                                                       \
                    break;                                                          \
                case CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_IPV6_INCREMENT_E: \
                    _val = 3;                                                       \
                    break;                                                          \
                default:                                                            \
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
            }

#define PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_UNREG_IPM_EVIDX_MODE_MAC(_val, _mode)             \
            switch(_val)                                                                  \
            {                                                                             \
                default:                                                                  \
                case 0:                                                                   \
                    _mode = CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_E;               \
                    break;                                                                \
                case 1:                                                                   \
                    _mode = CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV4_E;               \
                    break;                                                                \
                case 2:                                                                   \
                    _mode = CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_IPV6_E;               \
                    break;                                                                \
                case 3:                                                                   \
                    _mode = CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_IPV6_INCREMENT_E;\
                    break;                                                                \
            }

#define PRV_CPSS_DXCH_CONVERT_FDB_LOOKUP_KEY_MODE_TO_HW_VAL_MAC(_val, _mode) \
    switch(_mode)                                                            \
    {                                                                        \
        case CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_E:                        \
            _val = 0;                                                       \
            break;                                                           \
        case CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_VID1_E:                   \
            _val = 1;                                                       \
            break;                                                           \
        default:                                                             \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                             \
    }                                                                        \

#define PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_FDB_LOOKUP_KEY_MODE_MAC(_val, _mode) \
    switch(_val)                                                             \
    {                                                                        \
        default:                                                             \
        case 0:                                                              \
            _mode = CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_E;                 \
            break;                                                           \
        case 1:                                                              \
            _mode = CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_VID1_E;            \
            break;                                                           \
    }                                                                        \

/* maximal number of words in the DxCh and above vlan entry */
#define PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS      (6)

/* VLAN_INGRESS_ENTRY_WORDS_NUM_CNS - is used in Ingress VLAN related code for
   Lion, Lion2, Bobcat2, Caelum, Bobcat3 devices. This constant should be maximal one from
   all relevant devices.
   Number of bits in the Bobcat3;
   - L2i Ingress Bridge Port Membership Table entry - 512 bits  - maximal
   Number of bits in the Bobcat2; Caelum; Bobcat3;
   - L2i Ingress Bridge VLAN Table entry - 114 bits
   - L2i Ingress Bridge Port Membership Table entry - 256 bits  - maximal
   - L2i Span State Group Index Table entry - 12 bits
   Number of bits in the Lion; Lion2:
   - TxQ SHT Ingress VLAN Table - 140 bits */
#define L2I_INGRESS_VLAN_PORT_MEMBERS_SIZE_CNS  512
/* number of words in the Bobcat2; Caelum; Bobcat3 and above ingress vlan entry */
#define VLAN_INGRESS_ENTRY_WORDS_NUM_CNS     BITS_TO_WORDS_MAC(L2I_INGRESS_VLAN_PORT_MEMBERS_SIZE_CNS)
/* number of words in the BC3 - Egress vlan entry */
#define VLAN_EGRESS_ENTRY_WORDS_NUM_CNS      (16) /* support for 512 ports */

/* number of words in the Bobcat3 and above EGF_QAG Egress vlan entry */
#define EGF_QAG_EGRESS_VLAN_ENTRY_WORDS_NUM_CNS      (48) /* this support 512*3 bits */


/* the macro set field in the buffer according to the ingress vlan entry format*/
/* NOTE: the macro uses next 'implicit' fields:
hwDataArr - the buffer
prvCpssDxChLion3L2iIngressVlanTableFieldsFormat - the Bobcat2; Caelum; Bobcat3 vlan entry format

!!!!! So the macro gets only the value and the field name !!!!!!
*/
#define LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(_dev,_value,_fieldName)    \
    U32_SET_FIELD_BY_ID_MAC(hwDataArr,                          \
        PRV_CPSS_DXCH_PP_MAC(_dev)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_BRIDGE_INGRESS_EVLAN_E].fieldsInfoPtr,        \
        _fieldName,                                              \
        _value)

/* the macro get value of field from the buffer according to the ingress vlan entry format*/
/* NOTE: the macro uses next 'implicit' fields:
hwDataArr - the buffer
prvCpssDxChLion3L2iIngressVlanTableFieldsFormat - the Bobcat2; Caelum; Bobcat3 vlan entry format

!!!!! So the macro gets only the value and the field name !!!!!!
*/
#define LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(_dev,_value,_fieldName)    \
    U32_GET_FIELD_BY_ID_MAC(hwDataArr,                          \
        PRV_CPSS_DXCH_PP_MAC(_dev)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_BRIDGE_INGRESS_EVLAN_E].fieldsInfoPtr,        \
        _fieldName,                                              \
        _value)


/* the macro set value of field from the buffer according to the EGF_SHT egress vlan entry format*/
/* NOTE: the macro uses next 'implicit' fields:
hwDataArr - the buffer
prvCpssDxChLion3EgfShtEgressEVlanTableFieldsFormat - the Bobcat2; Caelum; Bobcat3 EGF_SHT egress vlan entry format

!!!!! So the macro gets only the value and the field name !!!!!!
*/
#define LION3_SET_FIELD_IN_EGF_SHT_EGRESS_VLAN_ENTRY_MAC(_dev,_value,_fieldName)    \
    U32_SET_FIELD_BY_ID_MAC(egfSht_hwDataArr,                          \
        PRV_CPSS_DXCH_PP_MAC(_dev)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EVLAN_E].fieldsInfoPtr,        \
        _fieldName,                                              \
        _value)

/* the macro get value of field from the buffer according to the EGF_SHT egress vlan entry format*/
/* NOTE: the macro uses next 'implicit' fields:
hwDataArr - the buffer
prvCpssDxChLion3EgfShtEgressEVlanTableFieldsFormat - the Bobcat2; Caelum; Bobcat3 EGF_SHT egress vlan entry format

!!!!! So the macro gets only the value and the field name !!!!!!
*/
#define LION3_GET_FIELD_IN_EGF_SHT_EGRESS_VLAN_ENTRY_MAC(_dev,_value,_fieldName)    \
    U32_GET_FIELD_BY_ID_MAC(egfSht_hwDataArr,                      \
        PRV_CPSS_DXCH_PP_MAC(_dev)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EVLAN_E].fieldsInfoPtr,        \
        _fieldName,                                                \
        _value)


static GT_STATUS vlanEntrySplitTableWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
);

static GT_STATUS vlanEntrySplitTableRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
);

/*******************************************************************************
* vlanEntrySplitTableMembersInHemisphereGet
*
* DESCRIPTION:
*       Returns VLAN members in specific hemisphere.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       hemisphere          - hemisphere to read from
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of vlan port members in the hemisphere
*                             ports are local to the hemishpere
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS vlanEntrySplitTableMembersInHemisphereGet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    IN  GT_U32                              hemisphere,
    OUT CPSS_PORTS_BMP_STC                  *portsMembersPtr
)
{
    GT_STATUS           rc;
    GT_U32              ingressHwData[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS];
    GT_U32              portGroupId;

    cpssOsMemSet(portsMembersPtr,0,sizeof(*portsMembersPtr));

    /* to get members in hemisphere 0, read from portGroup 0,
       and from hemisphere 1 read from portGroup 4 */
    portGroupId =
        (hemisphere * PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS);

    /* read entry from HW ingress table */
    rc = prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId,
        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
        (GT_U32)vlanId,/* vid */
        &(ingressHwData[0]));
    if(rc != GT_OK)
    {
        return rc;
    }

    /* convert HW format to SW format */
    /* retrieve port members from HW format (bits 2..65) */
    U32_GET_FIELD_IN_ENTRY_MAC(ingressHwData, 2,32,portsMembersPtr->ports[0]);
    U32_GET_FIELD_IN_ENTRY_MAC(ingressHwData,34,32,portsMembersPtr->ports[1]);

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanInit
*
* DESCRIPTION:
*       Initialize VLAN for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on system init error.
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanInit
(
    IN GT_U8    devNum
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanInit
*
* DESCRIPTION:
*       Initialize VLAN for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on system init error.
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanInit
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChBrgVlanInit(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChBrgCpuPortBmpConvert
*
* DESCRIPTION:
*       Convert port bitmap according to physical CPU port connection.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Xcat; Xcat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       isWriteConversion   - direction of conversion
*                             GT_TRUE - write conversion
*                             GT_FALSE - read conversion
*       portBitmapPtr       - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgCpuPortBmpConvert
(
    IN GT_U8                devNum,
    IN GT_BOOL              isWriteConversion,
    IN CPSS_PORTS_BMP_STC   *portBitmapPtr
)
{
    GT_U32 cpuPortPortGroupId; /* actual CPU port group */
    GT_U32 cpuPortNum;         /* actual(global) CPU port number */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* do not call this , as the CPU port not need the manipulation */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    cpuPortPortGroupId = (PRV_CPSS_PP_MAC(devNum)->cpuPortMode !=
                          CPSS_NET_CPU_PORT_MODE_SDMA_E) ?
                          PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(devNum) :
                          PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    cpuPortNum = (cpuPortPortGroupId * 16 + 15);

    if(GT_TRUE == isWriteConversion)
    {   /* write conversion - CPU port <63> to actual CPU port */
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(portBitmapPtr, CPSS_CPU_PORT_NUM_CNS))
        {
            if(CPSS_CPU_PORT_NUM_CNS != cpuPortNum)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(portBitmapPtr, cpuPortNum);
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(portBitmapPtr, CPSS_CPU_PORT_NUM_CNS);
            }
        }
    }
    else
    {   /* read conversion - actual CPU port to CPU port <63> */
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(portBitmapPtr, cpuPortNum))
        {
            if(CPSS_CPU_PORT_NUM_CNS != cpuPortNum)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(portBitmapPtr, CPSS_CPU_PORT_NUM_CNS);
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(portBitmapPtr, cpuPortNum);
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryBuild
*
* DESCRIPTION:
*       Builds vlan entry to buffer.
*
*       NOTE: for multi-port-groups device , this function called per port group , with the
*             per port group info
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* OUTPUTS:
*       hwDataArr[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS]
*                           - pointer to hw VLAN entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*                         or vlanInfoPtr->stgId
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryBuild
(
    IN GT_U8                        devNum,
    IN CPSS_PORTS_BMP_STC           *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC           *portsTaggingPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr,
    IN CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr,
    OUT GT_U32                      hwDataArr[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS]
)
{
    GT_U32      value;          /* for port vlan info building */
    GT_U32      value1;         /* for port vlan info building */
    GT_U32      fieldOffset;
    GT_U32      wordOffset;
    GT_U32      bitOffset;
    GT_U32      ii;
    GT_U32      maxPortNum;   /* max port number that vlan entry support */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    if(vlanInfoPtr->stgId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_STG_ID_MAC(devNum))/* 8 bits in HW */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            maxPortNum = 27;
            break;
        case CPSS_PP_FAMILY_DXCH_LION_E:
            maxPortNum = 12;/* per port group -- this function called per port group
                               , with the per port group info */
            break;
        default:
            maxPortNum = 28;
            break;
    }

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (portsMembersPtr->ports[0] >= ((GT_U32)(1 << maxPortNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if ((tr101Supported == GT_FALSE) && (portsTaggingPtr->ports[0] >= ((GT_U32)(1 << maxPortNum))))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* Cheetah related checks */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        if (/* IP Control To CPU - CH has only two values: ALL or NONE */
            (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_E) ||
            (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV6_E)
           )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* all next are ignored and may be set to any values:
           - Na Msg to CPU enable */
    }

    /* check ipCtrlToCpuEn */
    if(vlanInfoPtr->ipCtrlToCpuEn > CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E ||
       vlanInfoPtr->ipCtrlToCpuEn < CPSS_DXCH_BRG_IP_CTRL_NONE_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0, PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS * sizeof(GT_U32));

    /* Set Word0 */

    /* IPv6 Site ID - only for CH2 and above. CH and xCat2 set the bit to 0 */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        switch(vlanInfoPtr->ipv6SiteIdMode)
        {
            case CPSS_IP_SITE_ID_INTERNAL_E: value = 0; break;
            case CPSS_IP_SITE_ID_EXTERNAL_E: value = 1; break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        hwDataArr[0] |= (value << 23);
    }

    /* IPv6 IPM Bridging enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IpmBrgEn);
    hwDataArr[0] |= (value << 22);

    /* IPv4 IPM Bridging enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IpmBrgEn);
    hwDataArr[0] |= (value << 21);

    /* IPv6 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv6IpmBrgEn)
    {
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv6IpmBrgMode, value);
    hwDataArr[0] |= (value << 20);
    }

    /* IPv4 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv4IpmBrgEn)
    {
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv4IpmBrgMode, value);
    hwDataArr[0] |= (value << 19);
    }

    /* IPv4 Control traffic to CPU enable */
    if ((vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_E) ||
        (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E))
    {
        value = 1;
    }
    else
        value = 0;
    hwDataArr[0] |= (value << 18);

    /* IPv6 ICMP to CPU enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IcmpToCpuEn);
    hwDataArr[0] |= (value << 17);

    /* Mirror to Ingress Analyzer */
    value = BOOL2BIT_MAC(vlanInfoPtr->mirrToRxAnalyzerEn);
    hwDataArr[0] |= (value << 16);

    /* IPv4 IGMP to CPU enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IgmpToCpuEn);
    hwDataArr[0] |= (value << 15);

    /* Unknown unicast cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unkUcastCmd))
    hwDataArr[0] |= (value << 12);

    /* Unregistered IPv6 multicast cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv6McastCmd))
    hwDataArr[0] |= (value << 9);

    /* Unregistered IPv4 multicast cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv4McastCmd))
    hwDataArr[0] |= (value << 6);

    /* Unregistered Non IP multicast cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregNonIpMcastCmd))
    hwDataArr[0] |= (value << 3);


    /* NewSrc Address Is Not Security - revert the bit */
    value = 1 - (BOOL2BIT_MAC(vlanInfoPtr->unkSrcAddrSecBreach));
    hwDataArr[0] |= (value << 1);

    /* valid bit is set to 1 */
    hwDataArr[0] |= 1;


    /* Set Word2 */

    /* Span State Group Index */
    hwDataArr[2] |= ((vlanInfoPtr->stgId & 0xff) << 24);

    /* Unregistered Non IPv4 BC Cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregNonIpv4BcastCmd);
    hwDataArr[2] |= (value << 21);

    /* Unregistered IPv4 BC Cmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregIpv4BcastCmd);
    hwDataArr[2] |= (value << 18);

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* IPv6 Multicast Route enable */
        value = BOOL2BIT_MAC(vlanInfoPtr->ipv6McastRouteEn);
        hwDataArr[2] |= (value << 17);

        /* IPv4 Multicast Route enable */
        value = BOOL2BIT_MAC(vlanInfoPtr->ipv4McastRouteEn);
        hwDataArr[2] |= (value << 15);
    }

    /* IPv6 Unicast Route enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6UcastRouteEn);
    hwDataArr[2] |= (value << 16);

    /* IPv4 Unicast Route enable */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4UcastRouteEn);
    hwDataArr[2] |= (value << 14);


    /* Set Word3 - Cheetah has not Word 3 */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* IPv6 Control traffic to CPU enable */
        if ((vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV6_E) ||
            (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E))
        {
            value = 1;
        }
        else
            value = 0;
        hwDataArr[3] |= (value << 7);

        /* BCU DP Trap Mirror enable */
        value = BOOL2BIT_MAC(vlanInfoPtr->bcastUdpTrapMirrEn);
        hwDataArr[3] |= (value << 6);

        /* MRU Index (bit5 is reserved) */
        value = vlanInfoPtr->mruIdx;
        /* check MRU index */
        PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(value);
        hwDataArr[3] |= (value << 2);

        /* Na Msg to CPU enable */
        value = BOOL2BIT_MAC(vlanInfoPtr->naMsgToCpuEn);
        hwDataArr[3] |= (value << 1);

        /* Auto Learning disable */
        value = BOOL2BIT_MAC(vlanInfoPtr->autoLearnDisable);
        hwDataArr[3] |= value;

        if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E&&
             PRV_CPSS_PP_MAC(devNum)->revision == 0))
        {

            /* add constant 0xfff to bits 117:106 */
            hwDataArr[3] |= (0xFFF << 10);
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                /* xCat2 support constant 4095 (0xFFF) and values in range 0..1023 */
                if ((vlanInfoPtr->floodVidx != 0xFFF) &&
                    (vlanInfoPtr->floodVidx >= BIT_10))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            }
            else
            {
                if (vlanInfoPtr->floodVidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
            }

            /* Flood Vidx */
            hwDataArr[3] |= (vlanInfoPtr->floodVidx << 8);

            /* Multicast Local Switching Enable */
            hwDataArr[3] |= (BOOL2BIT_MAC(vlanInfoPtr->mcastLocalSwitchingEn) << 20);

            /* Port Isolation VLAN command */
            PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->portIsolationMode);
            hwDataArr[3] |= ( value << 21);

            /* set word5 for xCat */

            /* Unicast Local Switching Enable */
            hwDataArr[5] |= (BOOL2BIT_MAC(vlanInfoPtr->ucastLocalSwitchingEn) << 17);

            /* Flood VIDX Mode */
            PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->floodVidxMode);
            hwDataArr[5] |= (value << 18);
        }
    }



    /* build ports info */
    for (ii = 0; (ii < PRV_CPSS_PP_MAC(devNum)->numOfPorts); ii++)
    {
        /* get port information: member and tagging */
        value  = (CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr, ii) ? 1 : 0);

        if ( tr101Supported == GT_FALSE )
        {
            value |= (CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsTaggingPtr, ii) ? 2 : 0);

            value1 = 0;
        }
        else
        {
            if (portsTaggingCmdPtr->portsCmd[ii] >=
                CPSS_DXCH_BRG_VLAN_PORT_DO_NOT_MODIFY_TAG_CMD_E)
            {
                /* supported by Bobcat2 and above only */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            value |= ((portsTaggingCmdPtr->portsCmd[ii] & 1) ? 2 : 0);

            value1 = (portsTaggingCmdPtr->portsCmd[ii] >> 1);
        }

        if((value == 0) && (value1 == 0))
        {
            continue;
        }

        if(ii == 27) /* DxCh2 and above has 28 ports */
        {
            if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* port 27 data is placed in word 3 bits[23:24] */
                wordOffset = 3;
                bitOffset  = 23;
            }
            else
            {
                /* port 27 data is placed in word 3 bits[8:9] */
                wordOffset = 3;
                bitOffset  = 8;
            }
        }
        else
        {
            /* calculate first field offset in bits */
            fieldOffset = 24 + (ii * 2);    /* 24 first bits */
            wordOffset = fieldOffset >> 5;  /* / 32 */
            bitOffset  = fieldOffset & 0x1f; /* % 32 */
        }
        hwDataArr[wordOffset] |= value << bitOffset;

        /* set port tagging command for xCat and above devices */
        /* words 3,4,5 */
        if ( (tr101Supported != GT_FALSE) && (value1 != 0))
        {
            fieldOffset = 25 + (ii * 2);
            wordOffset = 3 + (fieldOffset >> 5);
            bitOffset  = fieldOffset & 0x1f;

            if (bitOffset == 31)
            {
                hwDataArr[wordOffset] |= (value1 & 1) << bitOffset;
                hwDataArr[wordOffset + 1] |= (value1 >> 1);
            }
            else
            {
                hwDataArr[wordOffset] |= value1 << bitOffset;
            }
        }
    }

    if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(
        portsMembersPtr, CPSS_CPU_PORT_NUM_CNS))
    {
        /* CPU port is a member of VLAN */
        hwDataArr[0] |= (1 << 2);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssBrgVlanHwEntryParse
*
* DESCRIPTION:
*       parse vlan entry from buffer.
*
*       NOTE: for multi-port-groups device , this function called per port group , to get the
*             per port group info
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - device number
*       hwDataArr - pointer to hw VLAN entry.
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryParse
(
    IN GT_U8                        devNum,
    IN GT_U32                       hwDataArr[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS],
    OUT CPSS_PORTS_BMP_STC          *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC          *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC *vlanInfoPtr,
    OUT GT_BOOL                     *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32      ii;
    GT_U32      value;
    GT_U32      value1 = 0; /* Variable used for words 4,5 parsing. Algorithm uses
                            assumption that the variable is initialized to 0 */
    GT_U32      fieldOffset;
    GT_U32      wordOffset;
    GT_U32      bitOffset;
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */
    GT_U32      maxPortNum;   /* max port number that vlan entry support */

    *isValidPtr = BIT2BOOL_MAC(hwDataArr[0] & 0x1);

    cpssOsMemSet(vlanInfoPtr,0,sizeof(*vlanInfoPtr));

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(portsMembersPtr);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported != GT_FALSE)
    {
        cpssOsMemSet(portsTaggingCmdPtr,0,sizeof(*portsTaggingCmdPtr));
    }
    else
    {
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(portsTaggingPtr);
    }

    /* Parse Word0 */

    /* NewSrc Address Is Not Security Breach - revert the value */
    value = U32_GET_FIELD_MAC(hwDataArr[0] , 1 , 1);
    vlanInfoPtr->unkSrcAddrSecBreach = BIT2BOOL_MAC(value - 1);

    /* Unregistered Non IP multicast cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[0] , 3 , 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpMcastCmd,
                                              value)

    /* Unregistered IPv4 multicast cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[0] , 6 , 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4McastCmd,
                                              value)

    /* Unregistered IPv6 multicast cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 9, 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv6McastCmd,
                                              value);

    /* Unknown unicast cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 12, 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unkUcastCmd,
                                              value);

    /* IPv4 IGMP to CPU enable */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 15, 1);
    vlanInfoPtr->ipv4IgmpToCpuEn = BIT2BOOL_MAC(value);

    /* Mirror to Ingress Analyzer */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 16, 1);
    vlanInfoPtr->mirrToRxAnalyzerEn = BIT2BOOL_MAC(value);

    /* IPv6 ICMP to CPU enable */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 17, 1);
    vlanInfoPtr->ipv6IcmpToCpuEn = BIT2BOOL_MAC(value);

    /* IPv4 Control to CPU enable, the IPv6 Control to Cpu will be read later
       and the ipCtrlToCpuEn will be changed accordingly */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 18, 1);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* for CH there are only two values: ALL or NONE */
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                        CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E :
                                        CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }
    else
    {
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                        CPSS_DXCH_BRG_IP_CTRL_IPV4_E :
                                        CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }

    /* IPv4 IPM Bridging mode */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 19, 1);
    vlanInfoPtr->ipv4IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                 CPSS_BRG_IPM_GV_E;

    /* IPv6 IPM Bridging mode */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 20, 1);
    vlanInfoPtr->ipv6IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                 CPSS_BRG_IPM_GV_E;

    /* IPv4 IPM Bridging enable */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 21, 1);
    vlanInfoPtr->ipv4IpmBrgEn = BIT2BOOL_MAC(value);

    /* IPv6 IPM Bridging enable */
    value = U32_GET_FIELD_MAC(hwDataArr[0], 22, 1);
    vlanInfoPtr->ipv6IpmBrgEn = BIT2BOOL_MAC(value);

     /* IPv6 Site ID */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah has not this bit
           - always return CPSS_IP_SITE_ID_INTERNAL_E */
        value = 0;
    }
    else
    {
        value = U32_GET_FIELD_MAC(hwDataArr[0], 23, 1);
    }

    vlanInfoPtr->ipv6SiteIdMode = (value == 0x0) ?
        CPSS_IP_SITE_ID_INTERNAL_E :
        CPSS_IP_SITE_ID_EXTERNAL_E;


    /* Parse Word2 */

    /* IPv4/6 Multicast Route enable */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* IPM disabled for CH */
        vlanInfoPtr->ipv4McastRouteEn = GT_FALSE;
        vlanInfoPtr->ipv6McastRouteEn = GT_FALSE;
    }
    else
    {
        /* IPv4 Multicast Route enable */
        value = U32_GET_FIELD_MAC(hwDataArr[2], 15, 1);
        vlanInfoPtr->ipv4McastRouteEn = BIT2BOOL_MAC(value);

        /* IPv6 Multicast Route enable */
        value = U32_GET_FIELD_MAC(hwDataArr[2], 17, 1);
        vlanInfoPtr->ipv6McastRouteEn = BIT2BOOL_MAC(value);

    }


    /* IPv4 Unicast Route enable */
    value = U32_GET_FIELD_MAC(hwDataArr[2], 14, 1);
    vlanInfoPtr->ipv4UcastRouteEn = BIT2BOOL_MAC(value);

    /* IPv6 Unicast Route enable */
    value = U32_GET_FIELD_MAC(hwDataArr[2], 16, 1);
    vlanInfoPtr->ipv6UcastRouteEn = BIT2BOOL_MAC(value);

    /* Unregistered IPv4 BC Cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[2], 18, 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4BcastCmd,
                                              value)
    /* Unregistered Non IPv4 BC Cmd */
    value = U32_GET_FIELD_MAC(hwDataArr[2], 21, 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpv4BcastCmd,
                                              value)

    /* Span State Group Index */
    vlanInfoPtr->stgId = U32_GET_FIELD_MAC(hwDataArr[2], 24, 8);


    /* Parse Word3 - Cheetah has not the Word 3*/

    /* check device type */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* disabled for CH */
        vlanInfoPtr->autoLearnDisable = GT_FALSE;
        vlanInfoPtr->naMsgToCpuEn = GT_TRUE;
        vlanInfoPtr->bcastUdpTrapMirrEn = GT_FALSE;
    }
    else
    {
        /* get IPv6 Control traffic to CPU enable */
        /* set IP Control to CPU enable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 7, 1);
        if (vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_IPV4_E)
        {
            if (value == 1)
                vlanInfoPtr->ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E;
        }
        else
        {
            if (value == 1)
                vlanInfoPtr->ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV6_E;
        }

        /* Auto Learning disable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 0, 1);
        vlanInfoPtr->autoLearnDisable = BIT2BOOL_MAC(value);

        /* Na Msg to CPU enable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 1, 1);
        vlanInfoPtr->naMsgToCpuEn = BIT2BOOL_MAC(value);

        /* MRU Index (bit5 is reserved) */
        vlanInfoPtr->mruIdx = U32_GET_FIELD_MAC(hwDataArr[3], 2, 3);

        /* BCU DP Trap Mirror enable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 6, 1);
        vlanInfoPtr->bcastUdpTrapMirrEn = BIT2BOOL_MAC(value);
    }

    /* xCat and above fields */
    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Flood VIDX */
        vlanInfoPtr->floodVidx = (GT_U16) U32_GET_FIELD_MAC(hwDataArr[3], 8, 12);

        /* Multicast Local Switching Enable */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 20, 1);
        vlanInfoPtr->mcastLocalSwitchingEn = BIT2BOOL_MAC(value);

        /* Port Isolation L2 VLAN command */
        value = U32_GET_FIELD_MAC(hwDataArr[3], 21, 2);
        switch (value)
        {
            case 0:
                vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
                break;
            case 1:
                vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E;
                break;
            case 2:
                vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E;
                break;
            default:
                vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E;
        }

        /* Unicast Local Switching Enable */
        value = U32_GET_FIELD_MAC(hwDataArr[5], 17, 1);
        vlanInfoPtr->ucastLocalSwitchingEn = BIT2BOOL_MAC(value);

        /* Flood VIDX Mode */
        value = U32_GET_FIELD_MAC(hwDataArr[5], 18, 1);
        vlanInfoPtr->floodVidxMode = (value == 0) ? CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E
                                                  : CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        maxPortNum = 12;/* per port group -- this function called per port group
                           , with the per port group info */
    }
    else
    {
        maxPortNum = PRV_CPSS_PP_MAC(devNum)->numOfPorts;
    }

    /* get ports info */
    for (ii = 0; ii < maxPortNum; ii++)
    {
        if(ii == 27)
        {
            if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* port 27 data is placed in word 3 bits[23:24] */
                wordOffset = 3;
                bitOffset  = 23;
            }
            else
            {
                /* port 27 data is placed in word 3 bits[8:9] */
                wordOffset = 3;
                bitOffset  = 8;
            }
        }
        else
        {
            /* calculate first field offset in bits */
            fieldOffset = 24 + (ii * 2);    /* 24 first bits */
            wordOffset = fieldOffset >> 5;  /* / 32 */
            bitOffset  = fieldOffset & 0x1f; /* % 32 */
        }

        value = (hwDataArr[wordOffset] >> bitOffset) & 0x3;

        if (tr101Supported != GT_FALSE)
        {
            /* calculate first field offset in bits */
            fieldOffset = 25 + (ii * 2);    /* 24 first bits */
            wordOffset = 3 + (fieldOffset >> 5);  /* / 32 */
            bitOffset  = fieldOffset & 0x1f; /* % 32 */

            if (bitOffset == 31)
            {
                value1 = (hwDataArr[wordOffset] >> bitOffset);
                value1 |= ((hwDataArr[wordOffset + 1] & 1) << 1 );
            }
            else
            {
                value1 = (hwDataArr[wordOffset] >> bitOffset) & 0x3;
            }
        }

        if ((value == 0) && (value1 == 0))
        {
            continue;
        }

        /* set port information: member and tagging */
        if (value & 1)
            CPSS_PORTS_BMP_PORT_SET_MAC(portsMembersPtr,ii) ;

        if (tr101Supported != GT_FALSE)
        {
            portsTaggingCmdPtr->portsCmd[ii] = value >> 1 ;
            portsTaggingCmdPtr->portsCmd[ii] |= (value1 << 1);
        }
        else
        {
            if (value & 2)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(portsTaggingPtr,ii) ;
            }
        }
    }

    if ((hwDataArr[0] & (1 << 2)) != 0)
    {
        /* CPU port is a member of VLAN */
        CPSS_PORTS_BMP_PORT_SET_MAC(
            portsMembersPtr, CPSS_CPU_PORT_NUM_CNS);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEntryWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32      hwData[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS];
                                  /* buffer to build entry; size is max  */
                                  /* possible size in Prestera system device */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanInfoPtr);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported != GT_FALSE)
    {
        /* for TR101 feature supported tagging command parameter must be valid */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingCmdPtr);
    }
    else
    {
        /* Simple port tagging */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingPtr);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = vlanEntrySplitTableWrite(devNum,vlanId,
                                     portsMembersPtr,
                                     portsTaggingPtr,
                                     vlanInfoPtr,
                                     portsTaggingCmdPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* build VLAN entry */
        rc = prvCpssBrgVlanHwEntryBuild(devNum,
                                        portsMembersPtr,
                                        portsTaggingPtr,
                                        vlanInfoPtr,
                                        portsTaggingCmdPtr,
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write VLAN entry to the VLAN Table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (GT_U32)vlanId,/* vid */
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* check for Cheetah3 and above device
       xCat2 does not support VRF ID */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) &&
        (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        if(vlanInfoPtr->vrfId >= BIT_12 )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* write VRF-ID entry */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                              (GT_U32)vlanId,
                                              0,
                                              0,
                                              12,
                                              vlanInfoPtr->vrfId);
        if (rc != GT_OK)
        {
            return rc;
        }
    }


    /* If Lion2 errata is initialized set port 14 (hemisphere 0) and
       port 64+14 (hemisphere 1) as a member in the vlan */
    if (((vlanInfoPtr->ipv4McastRouteEn == GT_TRUE)||(vlanInfoPtr->ipv6McastRouteEn == GT_TRUE)) &&
         (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_LION2_IPM_BRIDGE_COPY_GET_DROPPED_WA_E.enabled == GT_TRUE))
    {
        /* add port 14 to vlan */
        rc = cpssDxChBrgVlanMemberAdd(devNum, vlanId, 14, GT_FALSE, CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);

        if(rc != GT_OK)
            return rc;

        if (PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(devNum) >= 2)
        {
            /* add port 64+14to vlan */
            rc = cpssDxChBrgVlanMemberAdd(devNum, vlanId, 78, GT_FALSE, CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);

            if(rc != GT_OK)
                return rc;
        }

    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEntryWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, portsTaggingCmdPtr));

    rc = internal_cpssDxChBrgVlanEntryWrite(devNum, vlanId, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, portsTaggingCmdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, portsTaggingCmdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEntriesRangeWrite
*
* DESCRIPTION:
*       Function set multiple vlans with the same configuration
*       function needed for performances , when the device use "indirect" access
*       to the vlan table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       vlanId              - first VLAN Id
*       numOfEntries        - number of entries (vlans) to set
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong device number or vlanId
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEntriesRangeWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  GT_U32                                numOfEntries,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32      hwData[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS];
                                  /* buffer to build entry; size is max  */
                                  /* possible size in Prestera system device */
    GT_STATUS   rc;               /* return code*/
    GT_U32      i;                /* iterator */
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(
        devNum, (GT_U32)((GT_U32)vlanId + (numOfEntries - 1)));
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanInfoPtr);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported != GT_FALSE)
    {
        /* for TR101 feature supported tagging command parameter must be valid */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingCmdPtr);
    }
    else
    {
        /* Simple port tagging */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingPtr);
    }

    if (numOfEntries == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        for(i = 0; i < numOfEntries; i++)
        {
            rc = vlanEntrySplitTableWrite(devNum,(GT_U16)(vlanId + i),
                                         portsMembersPtr,
                                         portsTaggingPtr,
                                         vlanInfoPtr,
                                         portsTaggingCmdPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }
    else
    {
        /* build VLAN entry */
        rc = prvCpssBrgVlanHwEntryBuild(devNum,
                                        portsMembersPtr,
                                        portsTaggingPtr,
                                        vlanInfoPtr,
                                        portsTaggingCmdPtr,
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write VLAN entry to the VLAN Table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (GT_U32)vlanId,/* vid */
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* check for Cheetah3 and above device
       xCat2 does not support VRF ID */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) &&
        (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        if(vlanInfoPtr->vrfId >= BIT_12 )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* write VRF-ID entry */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                              (GT_U32)vlanId,
                                              0,
                                              0,
                                              12,
                                              vlanInfoPtr->vrfId);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    tableInfoPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_VLAN_E]);

     /* Write the above entry to all vlans */
    for(i = 1; i < numOfEntries; i++)
    {
        if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E)
        {
            /* write full vlan entry */
            rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_VLAN_E,
                                    (vlanId + i),
                                    hwData);
        }
        else
        {
            /*
                NOTE: the multi-port-groups device not need special case ,
                      because in indirect access , we need to loop on all port groups
                      without any change of 'data' , only 'control' changes.
                      and this loop on port groups will be done inside the cpssDriver
                      called from prvCpssDxChWriteTableEntry(...)
            */


            /* The indirect access implementation use NULL pointer to entry as
               sign to write only to control word. This result in all VLAN entries
               get same data written above.  */
            rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (vlanId + i),
                                        NULL);
        }

        if (rc != GT_OK)
        {
            return rc;
        }

        /* check for Cheetah3 and above device
           xCat2 does not support VRF ID */
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) &&
            (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)) &&
            (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            /* write VRF-ID entry */
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                  PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                                  (vlanId + i),
                                                  0,
                                                  0,
                                                  12,
                                                  vlanInfoPtr->vrfId);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanEntriesRangeWrite
*
* DESCRIPTION:
*       Function set multiple vlans with the same configuration
*       function needed for performances , when the device use "indirect" access
*       to the vlan table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number.
*       vlanId              - first VLAN Id
*       numOfEntries        - number of entries (vlans) to set
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong device number or vlanId
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntriesRangeWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  GT_U32                                numOfEntries,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEntriesRangeWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, numOfEntries, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, portsTaggingCmdPtr));

    rc = internal_cpssDxChBrgVlanEntriesRangeWrite(devNum, vlanId, numOfEntries, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, portsTaggingCmdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, numOfEntries, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, portsTaggingCmdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEntryRead
*
* DESCRIPTION:
*       Read vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                                 GT_TRUE = Vlan is valid
*                                 GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEntryRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
)
{
    GT_U32  hwData[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS];
                                    /* buffer to build entry; size is max     */
                                    /* possible size in Prestera system device*/
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(isValidPtr);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported != GT_FALSE)
    {
        /* for TR101 feature supported tagging command parameter must be valid */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingCmdPtr);
    }
    else
    {
        /* Simple port tagging */
        CPSS_NULL_PTR_CHECK_MAC(portsTaggingPtr);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = vlanEntrySplitTableRead(devNum,vlanId,
                                     portsMembersPtr,
                                     portsTaggingPtr,
                                     vlanInfoPtr,
                                     isValidPtr,
                                     portsTaggingCmdPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            /* do not call this , as the CPU port not need the manipulation */
        }
        else
        {
            /* CPU port bitmap conversion */
            rc = prvCpssDxChBrgCpuPortBmpConvert(devNum,
                                                 GT_FALSE,
                                                 portsMembersPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    else
    {
        /* read entry from HW */
        rc = prvCpssDxChReadTableEntry(devNum,
                                       PRV_CPSS_DXCH_TABLE_VLAN_E,
                                       (GT_U32)vlanId,/* vid */
                                       hwData);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssBrgVlanHwEntryParse(devNum,
                                        hwData,
                                        portsMembersPtr,
                                        portsTaggingPtr,
                                        vlanInfoPtr,
                                        isValidPtr,
                                        portsTaggingCmdPtr);

        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* check for Cheetah3 and above device
       xCat2 does not support VRF ID */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) &&
        (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* read VRF-ID entry */
        return prvCpssDxChReadTableEntryField(devNum,
                                              PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                              (GT_U32)vlanId,
                                              0,
                                              0,
                                              12,
                                              &(vlanInfoPtr->vrfId));
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanEntryRead
*
* DESCRIPTION:
*       Read vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bitmap of physical ports members in vlan
*                             CPU port supported
*       portsTaggingPtr     - (pointer to) bitmap of physical ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                                 GT_TRUE = Vlan is valid
*                                 GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) physical ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEntryRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, isValidPtr, portsTaggingCmdPtr));

    rc = internal_cpssDxChBrgVlanEntryRead(devNum, vlanId, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, isValidPtr, portsTaggingCmdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, portsMembersPtr, portsTaggingPtr, vlanInfoPtr, isValidPtr, portsTaggingCmdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* vlanMemberSet
*
* DESCRIPTION:
*       set port as vlan member/not and it's tag mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*       isMember        - is port member of the vlan
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       portTaggingCmd  - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vid or portTaggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS vlanMemberSet
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vlanId,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 isMember,
    IN  GT_BOOL                 isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
)
{
    GT_U32      hwData;             /* buffer to build part entry          */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT currTagCmd;
    GT_U32      hwDataTagCmd = 0;       /* hw format for currTagCmd */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */
    CPSS_PORTS_BMP_STC portsMembersInHemisphere;    /* VLAN port members in current hemisphere
                                                       before adding the additional port */
    GT_U32      portGroupInOtherHemishpere;         /* port group which resides in the other hemisphere */
    GT_BOOL     needToModifyOtherHemisphere;        /* weather port 12 of other hemisphere need to be modified */
    GT_U32      value;                              /* value to write to hardware */
    GT_U32      cfgOrderArr[3];                     /* order of configuration steps */
    GT_U32      cfgStepIdx;                         /* configuration steps index    */
    union
    {
        GT_U32 egrVlanEntryBufArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS];
        GT_U32 ingrVlanEntryBufArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS];
    } hwTabEntryBufArr;                             /* buffer for table entry       */
    GT_U32      fieldsAmount;                       /* amount of fields             */
    GT_U32      fieldOffsetArr[2];                  /* array of field offsets       */
    GT_U32      fieldLengthArr[2];                  /* array of field lengths       */
    GT_U32      fieldValueArr[2];                   /* array of field values        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    if (tr101Supported == GT_FALSE)
    {
        currTagCmd = (isTagged == GT_TRUE) ? CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E :
            CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }
    else
    {
        currTagCmd = portTaggingCmd;
    }

    if (portNum != CPSS_CPU_PORT_NUM_CNS)
    {
        PRV_CPSS_DXCH_CONVERT_VLAN_TAG_MODE_TO_HW_VAL_MAC(devNum, hwDataTagCmd,currTagCmd);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

        if (isMember == GT_FALSE)
        {
            /* removing port */
            cfgOrderArr[0] = 0;  /* ingress VLAN membership */
            cfgOrderArr[1] = 1;  /* egress VLAN membership  */
            cfgOrderArr[2] = 2;  /* VLAN command            */
        }
        else
        {
            /* adding port */
            cfgOrderArr[0] = 2;  /* VLAN command            */
            cfgOrderArr[1] = 1;  /* egress VLAN membership  */
            cfgOrderArr[2] = 0;  /* ingress VLAN membership */
        }

        for (cfgStepIdx = 0; (cfgStepIdx < 3); cfgStepIdx++)
        {
            switch (cfgOrderArr[cfgStepIdx])
            {
                case 0:
                    /* the ingress vlan entry looks the same on all port groups */
                    /* set the 'port members' in the Ingress Bridge Port Membership table */
                    rc = prvCpssDxChWriteTableEntryField(
                        devNum,
                        PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E,
                        vlanId,
                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,/* global start bit */
                        portNum, /* bit index */
                        1,/* single bit */
                        BOOL2BIT_MAC(isMember));
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                    break;

                case 1:
                    /* the SHT egress vlan entry looks the same on all port groups */

                    /* set the 'sub field' <port Member> in the field of <PORT_X_MEMBER>*/
                    rc = prvCpssDxChWriteTableEntryField(
                        devNum,
                        PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                        vlanId,
                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_SUB_FIELD_IN_FIELD_NAME_CNS,/* use of sub field ! */
                        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_PORT_X_MEMBER_E, /* field name */
                        PRV_CPSS_DXCH_TABLE_SUB_FIELD_OFFSET_AND_LENGTH_MAC(portNum,1),/* sub field info {offset,length}*/
                        BOOL2BIT_MAC(isMember));
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                    break;

                case 2:
                    /* the QAG egress vlan entry looks the same on all hemispheres */

                    /* set the <egress tag mode> for the proper port */
                    rc = prvCpssDxChWriteTableEntryField(
                        devNum,
                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E,
                        vlanId,
                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,/* global bit */
                        3*portNum, /* start bit */
                        3,/* number of bits*/
                        hwDataTagCmd);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }

        return GT_OK;
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if (portNum == CPSS_CPU_PORT_NUM_CNS)
        {
            portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            fieldsAmount      = 1;
            fieldOffsetArr[0] = 2;
            fieldLengthArr[0] = 1;
            fieldValueArr[0]  = BOOL2BIT_MAC(isMember);
        }
        else
        {
            PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
            /* calculate the word number and the bit offset to read */
            if (localPort < 27)
            {
                /* calculate first field offset in bits */
                fieldOffsetArr[0] = 24 + (2 * localPort);   /* 24 first byte */
            }
            else
            {
                if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
                {
                    /* port 27 data is placed in word 3 bits[23:24] */
                    fieldOffsetArr[0] = 119;
                }
                else
                {
                    /* port 27 data is placed in word 3 bits[8:9] */
                    fieldOffsetArr[0] = 104;
                }
            }

            /* build the data */
            hwData = BOOL2BIT_MAC(isMember);
            if (tr101Supported != GT_FALSE)
            {
                hwData |= ((hwDataTagCmd & 1) << 1) ;
            }
            else
            {
                hwData |= ((BOOL2BIT_MAC(isTagged)) << 1);
            }
            fieldLengthArr[0] = 2;
            fieldValueArr[0]  = hwData;
            fieldsAmount      = 1;

            if (tr101Supported != GT_FALSE)
            {
                /* build the data */
                hwData = (hwDataTagCmd >> 1);

                /* calculate first field offset in bits */
                fieldOffsetArr[1] = 121 + (2 * localPort);
                fieldLengthArr[1] = 2;
                fieldValueArr[1]  = hwData;
                fieldsAmount      = 2;
            }
        }

        return prvCpssDxChPortGroupWriteTableEntryFieldList(
            devNum, portGroupId,
            PRV_CPSS_DXCH_TABLE_VLAN_E,
            (GT_U32)vlanId, (GT_U32*)&hwTabEntryBufArr,
            fieldsAmount, fieldOffsetArr, fieldLengthArr, fieldValueArr);
    }
    else
    {
        if (CPSS_CPU_PORT_NUM_CNS == portNum)
        {
            /* CPU port conversion - set actual CPU port */

            /* CPU port group id  */
            portGroupId = (PRV_CPSS_PP_MAC(devNum)->cpuPortMode !=
                          CPSS_NET_CPU_PORT_MODE_SDMA_E) ?
                          PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(devNum) :
                          PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

            portNum = (portGroupId * 16 + 15);
        }
        else
        {
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portNum);
        }


        portNum = PRV_CPSS_DXCH_GLOBAL_TO_HEM_LOCAL_PORT(devNum,portNum);

        CPSS_TBD_BOOKMARK_EARCH
        if(portNum > 63 /*PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(devNum)*/)
        {
            /* eArch not defined vlan entry format that supports 128 ports*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* set port as member in ingress table */
        rc = prvCpssDxChPortGroupWriteTableEntryField(
            devNum, portGroupId,
            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
            (GT_U32)vlanId,
            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
            2 + portNum,
            1, /* 1 bit */
            BOOL2BIT_MAC(isMember));
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set port as member in egress table */
        fieldOffsetArr[0] = (1 + portNum);
        fieldLengthArr[0] = 1;
        fieldValueArr[0]  = BOOL2BIT_MAC(isMember);
        fieldsAmount      = 1;
        if(portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            /* set port tagCmp */
            fieldOffsetArr[1] = (65 + (3 * portNum));
            fieldLengthArr[1] = 3;
            fieldValueArr[1]  = hwDataTagCmd;
            fieldsAmount      = 2;
        }
        rc = prvCpssDxChPortGroupWriteTableEntryFieldList(
            devNum, portGroupId,
            PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
            (GT_U32)vlanId, (GT_U32*)&hwTabEntryBufArr,
            fieldsAmount, fieldOffsetArr, fieldLengthArr, fieldValueArr);
        if(rc != GT_OK)
        {
            return rc;
        }

        if (GT_FALSE != PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                info_PRV_CPSS_DXCH_LION2_MC_BANDWIDTH_RESTRICTION_WA_E.enabled)
        {
            /* read current VLAN members in the current hemishpere */
            rc = vlanEntrySplitTableMembersInHemisphereGet(
                devNum, vlanId,
                (portGroupId / PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS), /* current hemisphere */
                &portsMembersInHemisphere);
            if (rc != GT_OK)
                return rc;
            /* don't take into account port 12 which is used to send traffic to other hemisphere */
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portsMembersInHemisphere,12);

            needToModifyOtherHemisphere = GT_FALSE;
            if (isMember == GT_TRUE) /* adding member */
            {
                /* check if was is the first member added to this hemisphere */
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portsMembersInHemisphere,portNum); /* remove it from the list */
                if ((portsMembersInHemisphere.ports[0] == 0) && (portsMembersInHemisphere.ports[1] == 0))
                {
                    needToModifyOtherHemisphere = GT_TRUE;
                }
            }
            else /* deleting member */
            {
                /* check if deleted member was the last in current hemisphere */
                if ((portsMembersInHemisphere.ports[0] == 0) && (portsMembersInHemisphere.ports[1] == 0))
                {
                    needToModifyOtherHemisphere = GT_TRUE;
                }
            }

            /* update port 12 of the other hemisphere if needed */
            if (needToModifyOtherHemisphere == GT_TRUE)
            {
                /* any portGroupId in the same hemisphere adresses the same VLAN tables */
                portGroupInOtherHemishpere =
                    (portGroupId >= PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS)
                        ? (portGroupId - PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS)
                        : (portGroupId + PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS);
                /* to make port 12 of other hemisphere VLAN member "if end only if" */
                /* the VLAN contains members in the current hemisphere              */
                value = (isMember == GT_TRUE) ? 0 : 1; /* port 12 is with reverse logic */

                /* set port as member in ingress table */
                rc = prvCpssDxChPortGroupWriteTableEntryField(devNum, portGroupInOtherHemishpere,
                                                      PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                                      (GT_U32)vlanId,
                                                      PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                      2 + 12,
                                                      1, /* 1 bit */
                                                      value);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* set port as member in egress table */
                rc = prvCpssDxChPortGroupWriteTableEntryField(devNum, portGroupInOtherHemishpere,
                                                      PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                                      (GT_U32)vlanId,
                                                      PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                      1 + 12,
                                                      1, /* 1 bit */
                                                      value);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanMemberAdd
*
* DESCRIPTION:
*       Add new port member to vlan entry. This function can be called only for
*       add port belongs to device that already member of the vlan.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*                         CPU port supported
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       portTaggingCmd  - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vid or portTaggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case that added port belongs to device that is new device for vlan
*       other function is used - cpssDxChBrgVlanEntryWrite.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanMemberAdd
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vlanId,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
)
{
    return vlanMemberSet(devNum,vlanId,portNum,GT_TRUE,isTagged,portTaggingCmd);
}

/*******************************************************************************
* cpssDxChBrgVlanMemberAdd
*
* DESCRIPTION:
*       Add new port member to vlan entry. This function can be called only for
*       add port belongs to device that already member of the vlan.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*                         CPU port supported
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       portTaggingCmd  - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vid or portTaggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case that added port belongs to device that is new device for vlan
*       other function is used - cpssDxChBrgVlanEntryWrite.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMemberAdd
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vlanId,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanMemberAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, portNum, isTagged, portTaggingCmd));

    rc = internal_cpssDxChBrgVlanMemberAdd(devNum, vlanId, portNum, isTagged, portTaggingCmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, portNum, isTagged, portTaggingCmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanPortDelete
*
* DESCRIPTION:
*       Delete port member from vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN Id
*       portNum     - port number
*                     CPU port supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortDelete
(
    IN GT_U8                    devNum,
    IN GT_U16                   vlanId,
    IN GT_PHYSICAL_PORT_NUM     portNum
)
{
    return vlanMemberSet(devNum,vlanId,portNum,GT_FALSE,GT_FALSE,
                        CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);

}

/*******************************************************************************
* cpssDxChBrgVlanPortDelete
*
* DESCRIPTION:
*       Delete port member from vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN Id
*       portNum     - port number
*                     CPU port supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortDelete
(
    IN GT_U8                    devNum,
    IN GT_U16                   vlanId,
    IN GT_PHYSICAL_PORT_NUM     portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, portNum));

    rc = internal_cpssDxChBrgVlanPortDelete(devNum, vlanId, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanMemberSet
*
* DESCRIPTION:
*       Set specific member at VLAN entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*                         CPU port supported
*       isMember        - GT_TRUE,  port would set as member
*                         GT_FALSE, port would set as not-member
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       taggingCmd      - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or taggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanMemberSet
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vlanId,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 isMember,
    IN  GT_BOOL                 isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd
)
{
    return vlanMemberSet(devNum,vlanId,portNum,isMember,isTagged,taggingCmd);
}

/*******************************************************************************
* cpssDxChBrgVlanMemberSet
*
* DESCRIPTION:
*       Set specific member at VLAN entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*                         CPU port supported
*       isMember        - GT_TRUE,  port would set as member
*                         GT_FALSE, port would set as not-member
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         The parameter is relevant for DxCh1, DxCh2
*                         and DxCh3 devices.
*                         The parameter is relevant for xCat and
*                         above devices without TR101 feature support
*                         The parameter is not relevant and ignored for
*                         xCat and above devices with TR101 feature
*                         support.
*       taggingCmd      - port tagging command
*                         The parameter is relevant only for xCat and above
*                         with TR101 feature support.
*                         The parameter is not relevant and ignored for
*                         other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or taggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMemberSet
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vlanId,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 isMember,
    IN  GT_BOOL                 isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanMemberSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, portNum, isMember, isTagged, taggingCmd));

    rc = internal_cpssDxChBrgVlanMemberSet(devNum, vlanId, portNum, isMember, isTagged, taggingCmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, portNum, isMember, isTagged, taggingCmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* ingressPortVidGet
*
* DESCRIPTION:
*       Get port's default VLAN Id. - ingress.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*
* OUTPUTS:
*       vidPtr   - default VLAN ID.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ingressPortVidGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U16          *vidPtr
)
{
    GT_STATUS   rc;
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_U32      hwData;    /* data to write to Hw */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_EVLAN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 15;
        }
        else
        {
            startWord = 1;
            startBit  = 0;
        }

        /* read from port-vlan-qos table  */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        portNum,/* port */
                                        startWord,
                                        startBit,
                                        12, /* 12 bits */
                                        &hwData);
    }

    *vidPtr = (GT_U16)hwData;

    return rc;

}

/*******************************************************************************
* egressPortVidSet
*
* DESCRIPTION:
*       Set VID0 assigned to the egress packet if <VID0 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       vid0    - VID0 value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS egressPortVidSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U16           vid0
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum, vid0);

    /* set EGF_QAG unit */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EVLAN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        vid0);
    return rc;

}

/*******************************************************************************
* egressPortVidGet
*
* DESCRIPTION:
*       Return the VID0 assigned to the egress packet if <VID0 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       vid0Ptr - (pointer to) VID0 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS egressPortVidGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U16          *vid0Ptr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EVLAN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    *vid0Ptr = (GT_U16)value;

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanPortVidGet
*
* DESCRIPTION:
*       For ingress direction : Get port's default VID0.
*       For egress direction  : Get port's VID0 when egress port <VID0 Command>=Enabled
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       direction - ingress/egress direction
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* OUTPUTS:
*       vidPtr   - default VLAN ID.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or port or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVidGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN CPSS_DIRECTION_ENT          direction,
    OUT  GT_U16          *vidPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(vidPtr);

    if(GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
    {
        direction = CPSS_DIRECTION_INGRESS_E;
    }

    switch(direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            return ingressPortVidGet(devNum,portNum,vidPtr);

        case CPSS_DIRECTION_EGRESS_E:
            return egressPortVidGet(devNum,portNum,vidPtr);
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidGet
*
* DESCRIPTION:
*       For ingress direction : Get port's default VID0.
*       For egress direction  : Get port's VID0 when egress port <VID0 Command>=Enabled
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       direction - ingress/egress direction
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* OUTPUTS:
*       vidPtr   - default VLAN ID.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or port or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN CPSS_DIRECTION_ENT          direction,
    OUT  GT_U16          *vidPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVidGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, vidPtr));

    rc = internal_cpssDxChBrgVlanPortVidGet(devNum, portNum, direction, vidPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, vidPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* multiPortGroupDebugBrgVlanPortVidSet
*
* DESCRIPTION:
*       Set port's default VLAN Id.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       vlanId   - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum port, or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS multiPortGroupDebugBrgVlanPortVidSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_U16          vlanId
)
{
    GT_STATUS   rc;
    GT_U32  ii,jj; /*iterators*/
    GT_U32  globalPort,tmpPort;/* uplink global port , tmp interconnection port */


    /* in this function we assume that application will set all the uplinks with
       the same PVID , so for a particular uplink we set only it's interconnection
       ports , and not all interconnection ports (when multi-uplinks) */


    /* check if this port is one of the uplink ports ,if so */
    /* loop on all it's interconnection ports , and set them with the same PVID */
    for(ii = 0 ;
        ii < PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->numOfUplinkPorts;
        ii++)
    {
        globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].uplinkPortGroupPortInfo.portGroupId,
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].uplinkPortGroupPortInfo.localPort);
        if(globalPort != portNum)
        {
            continue;
        }

        /* the PVID on this port need to be set on other ports as well */
        for(jj = 0 ;
            jj < PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].numberOfInternalConnections;
            jj++)
        {
            /*convert localPort, portGroupId to global port */
            tmpPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,
                PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].internalConnectionsArr[jj].portGroupId,
                PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].internalConnectionsArr[jj].localPort);

            rc = cpssDxChBrgVlanPortVidSet(devNum,(GT_U8)tmpPort,CPSS_DIRECTION_INGRESS_E,vlanId);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* we handled the 'Interconnection ports' of this uplink ,
           so no more needed */
        break;
    }

    return GT_OK;
}

/*******************************************************************************
* ingressPortVidSet
*
* DESCRIPTION:
*       Set port's default VLAN Id. (ingress)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       vlanId   - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum port, or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ingressPortVidSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_U16          vlanId
)
{
    GT_STATUS   rc;
    GT_U32      startWord; /* the table word at which the field starts */
    GT_U32      startBit;  /* the word's bit at which the field starts */
    GT_U32      hwData;    /* data to write to Hw */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum, vlanId);

    hwData = vlanId;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
         rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_EVLAN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 15;
        }
        else
        {
            startWord = 1;
            startBit  = 0;
        }

        /* write to port-vlan-qos table  */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         portNum,/* port */
                                         startWord,
                                         startBit,
                                         12, /* 12 bits */
                                         hwData);

        if(rc == GT_OK &&
           PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->debugInfoValid == GT_TRUE)
        {
            rc = multiPortGroupDebugBrgVlanPortVidSet(devNum,portNum,vlanId);
        }
    }
    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortVidSet
*
* DESCRIPTION:
*       For ingress direction : Set port's default VID0.
*       For egress direction  : Set port's VID0 when egress port <VID0 Command>=Enabled
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       direction - ingress/egress direction
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*       vlanId   - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum port, or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVidSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN CPSS_DIRECTION_ENT          direction,
    IN  GT_U16          vlanId
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if(GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
    {
        direction = CPSS_DIRECTION_INGRESS_E;
    }

    switch(direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            return ingressPortVidSet(devNum,portNum,vlanId);

        case CPSS_DIRECTION_EGRESS_E:
            return egressPortVidSet(devNum,portNum,vlanId);
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidSet
*
* DESCRIPTION:
*       For ingress direction : Set port's default VID0.
*       For egress direction  : Set port's VID0 when egress port <VID0 Command>=Enabled
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       direction - ingress/egress direction
*                   APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*       vlanId   - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum port, or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN CPSS_DIRECTION_ENT          direction,
    IN  GT_U16          vlanId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVidSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, vlanId));

    rc = internal_cpssDxChBrgVlanPortVidSet(devNum, portNum, direction, vlanId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, vlanId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanPortIngFltEnable
*
* DESCRIPTION:
*       Enable/disable Ingress Filtering for specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       enable    - GT_TRUE, enable ingress filtering
*                   GT_FALSE, disable ingress filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortIngFltEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS   rc;
    GT_U32      val;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    val = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {

        /* set the <VLAN_INGRESS_FILTERING>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_VLAN_INGRESS_FILTERING_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        val);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                  PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                                  PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum),
                                                  21,
                                                  1,
                                                  val);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngFltEnable
*
* DESCRIPTION:
*       Enable/disable Ingress Filtering for specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       enable    - GT_TRUE, enable ingress filtering
*                   GT_FALSE, disable ingress filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngFltEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortIngFltEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortIngFltEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortIngFltEnableGet
*
* DESCRIPTION:
*       Get status of Ingress Filtering for specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       enablePtr    - Pointer to Ingress Filtering status
*                      - GT_TRUE, ingress filtering is enabled
*                      - GT_FALSE, ingress filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortIngFltEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      val;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {

        /* get the <VLAN_INGRESS_FILTERING>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_VLAN_INGRESS_FILTERING_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &val);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        rc = prvCpssHwPpPortGroupGetRegField(devNum,
                                                  PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                                  PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum),
                                                  21,
                                                  1,
                                                  &val);
    }

    *enablePtr = BIT2BOOL_MAC(val);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngFltEnableGet
*
* DESCRIPTION:
*       Get status of Ingress Filtering for specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       enablePtr    - Pointer to Ingress Filtering status
*                      - GT_TRUE, ingress filtering is enabled
*                      - GT_FALSE, ingress filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngFltEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortIngFltEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortIngFltEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortProtoClassVlanEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort.
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortProtoClassVlanEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_U32      hwData;     /* data to write into register */
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    hwData = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
         rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PROT_BASED_VLAN_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 30;
        }
        else
        {
            startWord = 0;
            startBit  = 18;
        }

        /* write to port-vlan-qos table  */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,/* port */
                                             startWord,
                                             startBit,
                                             1, /* 1 bit */
                                             hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassVlanEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort.
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassVlanEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortProtoClassVlanEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortProtoClassVlanEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortProtoClassVlanEnableGet
*
* DESCRIPTION:
*       Get status of Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*                    In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr  - GT_TRUE for enable, otherwise GT_FALSE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortProtoClassVlanEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_U32      hwData;     /* register data  */
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from pre-tti-lookup-ingress-eport table */
         rc = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                             LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PROT_BASED_VLAN_EN_E, /* field name */
                                             PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &hwData);
         if(GT_OK != rc)
         {
             return rc;
         }
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 30;
        }
        else
        {
            startWord = 0;
            startBit  = 18;
        }

        /* write to port-vlan-qos table  */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,/* port */
                                             startWord,
                                             startBit,
                                             1, /* 1 bit */
                                             &hwData);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassVlanEnableGet
*
* DESCRIPTION:
*       Get status of Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*                    In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr  - GT_TRUE for enable, otherwise GT_FALSE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassVlanEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortProtoClassVlanEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortProtoClassVlanEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortProtoClassQosEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort.
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortProtoClassQosEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_U32      hwData;     /* data to write into register */
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    hwData = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
         rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PROT_BASED_QOS_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 1;
            startBit  = 11;
        }
        else
        {
            startWord = 0;
            startBit  = 19;
        }

        /* write to port-vlan-qos table  */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,/* port */
                                             startWord,
                                             startBit,
                                             1, /* 1 bit */
                                             hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassQosEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort.
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassQosEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortProtoClassQosEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortProtoClassQosEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortProtoClassQosEnableGet
*
* DESCRIPTION:
*       Get status of Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr - GT_TRUE for enable, otherwise GT_FALSE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortProtoClassQosEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_U32      hwData;     /* register data */
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from pre-tti-lookup-ingress-eport table */
         rc = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                             LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PROT_BASED_QOS_EN_E, /* field name */
                                             PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &hwData);
        if(GT_OK != rc)
        {
            return rc;
        }
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 1;
            startBit  = 11;
        }
        else
        {
            startWord = 0;
            startBit  = 19;
        }

        /* write to port-vlan-qos table  */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,/* port */
                                             startWord,
                                             startBit,
                                             1, /* 1 bit */
                                             &hwData);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassQosEnableGet
*
* DESCRIPTION:
*       Get status of Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr - GT_TRUE for enable, otherwise GT_FALSE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoClassQosEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_PORT_NUM  portNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortProtoClassQosEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortProtoClassQosEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanProtoClassSet
*
* DESCRIPTION:
*       Write etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       entryNum    - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*       etherType   - Ether Type or DSAP/SSAP
*       encListPtr  - encapsulation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanProtoClassSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    IN GT_U16                           etherType,
    IN CPSS_PROT_CLASS_ENCAP_STC        *encListPtr
)
{
    GT_U32      hwData;              /* value to write into register    */
    GT_U32      regAddr;             /* register address                */
    GT_U32      wordOffset;          /* offset of register              */
    GT_U32      offset;              /* offset in a register            */
    GT_STATUS   rc ;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(encListPtr);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);

    /* build encapsulation bitmap */
    hwData = (BOOL2BIT_MAC(encListPtr->ethV2) |
              ((BOOL2BIT_MAC(encListPtr->nonLlcSnap)) << 1) |
              ((BOOL2BIT_MAC(encListPtr->llcSnap)) << 2));

    if (entryNum > 7)
    {
        /* get address of Protocols Encapsulation Register1 */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).protocolMatch.protocolsEncapsulationConfig1;
        }
        else
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased1;
        }

        /* write encapsulation */
        offset = ((entryNum - 8) * 3);
    }
    else
    {
        /* get address of Protocols Encapsulation Register */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).protocolMatch.protocolsEncapsulationConfig0;
        }
        else
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased;
        }

        /* write encapsulation */
        offset = (entryNum * 3);
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 3, hwData);
    if (rc != GT_OK)
        return rc;

    if (entryNum > 7)
    {
        /* write validity bit */
        offset = (entryNum - 8) + 12;
    }
    else
    {
        /* write validity bit */
        offset = entryNum + 24;
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 1, 1);
    if (rc != GT_OK)
        return rc;

    /* get address of Protocols Configuration Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).protocolMatch.protocolsConfig[0];
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoTypeBased;
    }

    /* write Ether Type */
    hwData = etherType;
    wordOffset = entryNum >> 1; /* /2 */
    offset = (entryNum & 0x1) ? 16 : 0;

    regAddr += (wordOffset * 4);
    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 16, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanProtoClassSet
*
* DESCRIPTION:
*       Write etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       entryNum    - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*       etherType   - Ether Type or DSAP/SSAP
*       encListPtr  - encapsulation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    IN GT_U16                           etherType,
    IN CPSS_PROT_CLASS_ENCAP_STC        *encListPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanProtoClassSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryNum, etherType, encListPtr));

    rc = internal_cpssDxChBrgVlanProtoClassSet(devNum, entryNum, etherType, encListPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryNum, etherType, encListPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanProtoClassGet
*
* DESCRIPTION:
*       Read etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Ether Type or DSAP/SSAP
*       encListPtr   - (pointer to) encapsulation.
*       validEntryPtr - (pointer to) validity bit
*                                   GT_TRUE - the entry is valid
*                                   GT_FALSE - the entry is invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanProtoClassGet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    OUT GT_U16                          *etherTypePtr,
    OUT CPSS_PROT_CLASS_ENCAP_STC       *encListPtr,
    OUT GT_BOOL                         *validEntryPtr
)
{
    GT_U32      hwData;              /* value to write into register    */
    GT_U32      regAddr;             /* register address                */
    GT_U32      wordOffset;          /* offset of register              */
    GT_U32      offset;              /* offset in a register            */
    GT_STATUS   rc ;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);
    CPSS_NULL_PTR_CHECK_MAC(encListPtr);
    CPSS_NULL_PTR_CHECK_MAC(validEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    if (entryNum > 7)
    {
        /* get address of Protocols Encapsulation Register1 */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).protocolMatch.protocolsEncapsulationConfig1;
        }
        else
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased1;
        }

        /* read validity bit */
        offset = (entryNum - 8) + 12;
    }
    else
    {
        /* get address of Protocols Encapsulation Register */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).protocolMatch.protocolsEncapsulationConfig0;
        }
        else
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased;
        }

        /* read validity bit */
        offset = entryNum + 24;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 1, &hwData);
    if (rc != GT_OK)
        return rc;
    *validEntryPtr = BIT2BOOL_MAC(hwData & 0x1);

    if (entryNum > 7)
    {
        /* read encapsulation */
        offset = ((entryNum - 8) * 3);
    }
    else
    {
        /* read encapsulation */
        offset = (entryNum * 3);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 3, &hwData);
    if (rc != GT_OK)
        return rc;

    /* build encapsulation */
    encListPtr->ethV2 = BIT2BOOL_MAC(hwData & 0x1);
    encListPtr->nonLlcSnap = BIT2BOOL_MAC((hwData >> 1) & 0x1);
    encListPtr->llcSnap = BIT2BOOL_MAC((hwData >> 2) & 0x1);

    /* get address of Protocols Configuration Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).protocolMatch.protocolsConfig[0];
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.vlanRegs.protoTypeBased;
    }

    /* get Ether Type */
    wordOffset = entryNum >> 1; /* /2 */
    offset = (entryNum & 0x1) ? 16 : 0;

    regAddr += (wordOffset * 4);
    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 16, &hwData);
    if (rc != GT_OK)
        return rc;

    *etherTypePtr = (GT_U16)hwData;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanProtoClassGet
*
* DESCRIPTION:
*       Read etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Ether Type or DSAP/SSAP
*       encListPtr   - (pointer to) encapsulation.
*       validEntryPtr - (pointer to) validity bit
*                                   GT_TRUE - the entry is valid
*                                   GT_FALSE - the entry is invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassGet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    OUT GT_U16                          *etherTypePtr,
    OUT CPSS_PROT_CLASS_ENCAP_STC       *encListPtr,
    OUT GT_BOOL                         *validEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanProtoClassGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryNum, etherTypePtr, encListPtr, validEntryPtr));

    rc = internal_cpssDxChBrgVlanProtoClassGet(devNum, entryNum, etherTypePtr, encListPtr, validEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryNum, etherTypePtr, encListPtr, validEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanProtoClassInvalidate
*
* DESCRIPTION:
*       Invalidates entry of Protocol based classification for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanProtoClassInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum
)
{
    GT_U32      hwData;              /* value to write into register    */
    GT_U32      regAddr;             /* register address                */
    GT_U32      offset;              /* offset in a register            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);

    if (entryNum > 7)
    {
        /* get address of Protocols Encapsulation Register1 */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).protocolMatch.protocolsEncapsulationConfig1;
        }
        else
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased1;
        }

        /* write validity bit */
        offset = (entryNum - 8) + 12;
    }
    else
    {
        /* get address of Protocols Encapsulation Register */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).protocolMatch.protocolsEncapsulationConfig0;
        }
        else
        {
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlanRegs.protoEncBased;
        }

        /* write validity bit */
        offset = entryNum + 24;
    }

    /* invalidate validity bit */
    hwData = 0;
    return prvCpssHwPpSetRegField(devNum, regAddr, offset, 1, hwData);
}

/*******************************************************************************
* cpssDxChBrgVlanProtoClassInvalidate
*
* DESCRIPTION:
*       Invalidates entry of Protocol based classification for specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanProtoClassInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanProtoClassInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryNum));

    rc = internal_cpssDxChBrgVlanProtoClassInvalidate(devNum, entryNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortProtoVlanQosSet
*
* DESCRIPTION:
*       Write Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       entryNum            - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*       vlanCfgPtr          - pointer to VID parameters
*       qosCfgPtr           - pointer to QoS parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It is possible to update only VLAN or QOS parameters, when one
*       of those input pointers is NULL.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortProtoVlanQosSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN GT_U32                                   entryNum,
    IN CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC    *vlanCfgPtr,
    IN CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC     *qosCfgPtr
)
{
    GT_U32      hwData;         /* data to read/write from/to register */
    GT_U32      value;          /* value to update */
    GT_U32      tableEntryIndex;/* index in VLAN_PORT_PROTOCOL table */
    GT_U32      wordNumber;     /* word number */
    GT_U32      validBitNum;    /* position of the Valid bit */
    GT_U32      cmdFirstBit;    /* VLAN/QoS command field first bit */
    GT_U32      precedenceFirstBit; /* VLAN/QoS precedence field first bit */
    GT_U32      qosProfileFirstBit; /* QoS profile field first bit */
    GT_U32      modifyDscpFirstBit; /* QoS enable modify DSCP field first bit */
    GT_U32      modifyUpFirstBit;   /* QoS enable modify UP field first bit */
    GT_STATUS   rc;
    GT_U32      fieldLength;/* total bits used in the word */
    GT_U32      maskForKeepVlan;/*mask for reset all QoS related bits and don't change VLAN related bits */
    GT_U32      maskForKeepQos;/*mask for reset all QoS related bits and don't change VLAN related bits */
    GT_U32      length;/*length of field*/
    PRV_CPSS_DXCH_PP_TABLE_FORMAT_INFO_STC * initTablesFormatPtr =
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        fieldLength = 0;/* NOT used */

        maskForKeepVlan = 0x0000FFFF;
        maskForKeepQos  = 0xFFFE0000;
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        fieldLength = 28;

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            maskForKeepVlan = 0x00007FFF;
            maskForKeepQos  = 0x0FFF0000;
        }
        else
        {
            maskForKeepVlan = 0x01600FFF;
            maskForKeepQos  = 0x069FF000;
        }

    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* table is actually a matrix of Port (0..63) and Protocol (0..11).
           for every port are allocated 16 protocol entries (12 valid) */
        tableEntryIndex = portNum * 16 + entryNum;
        /* the entry has only one word */
        wordNumber = 0;

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            /* Valid bit number */
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_VALID_E,
                validBitNum,length);
        }
        else
        {
            /* Valid bit number */
            validBitNum = 15;
        }
    }
    else
    {
        /* table entry includes all protocol info per port */
        tableEntryIndex = portNum;
        /* wordNumber is protocol index */
        wordNumber = entryNum;

        /* Valid bit number */
        validBitNum = 27;
    }

    if ((vlanCfgPtr == NULL) && (qosCfgPtr == NULL))
    {
        /* at least one configuration need to be defined */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* Read entry from Port Protocol VID and QOS table */
        rc = prvCpssDxChReadTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                        tableEntryIndex, /* table index */
                                        &hwData);
    }
    else
    {
        /* Read entry from Port Protocol VID and QOS Register */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                            tableEntryIndex, /* table index */
                                            wordNumber,  /* word number */
                                            0,/* start at bit 0*/
                                            fieldLength, /* 28 or 32 bits */
                                            &hwData);
    }
    if (rc != GT_OK)
    {
        return rc;
    }

    /* check if both VID and QOS parameters should be set,
       if not mask the appropriate bits */
    if (vlanCfgPtr == NULL)
    {
        /* reset all QoS related bits and don't change VLAN related bits */
        hwData &= maskForKeepVlan;
    }
    else if (qosCfgPtr == NULL)
    {
        /* reset all VLAN related bits and don't change QOS related bits */
        hwData &= maskForKeepQos;
    }
    else
    {
        /* reset all fields */
        hwData = 0;
    }

    if (vlanCfgPtr != NULL)
    {
        /* check validity VLAN id */
        PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum,vlanCfgPtr->vlanId);

        /* setting 12 bits of <ProtocolVID> in bits [0:11]*/
        hwData |= vlanCfgPtr->vlanId;

        /* define field positions */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_EVLAN_COMMAND_E,
                cmdFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_EVLAN_PRECEDENCE_E,
                precedenceFirstBit,length);
        }
        else
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {   /* xCat and above */
            cmdFirstBit = 12;
            precedenceFirstBit = 14;
        }
        else
        {
            cmdFirstBit = 21;
            precedenceFirstBit = 24;
        }

        /* setting <VLANCmd> */
        PRV_CPSS_CONVERT_ATTR_ASSIGN_CMD_TO_HW_VAL_MAC(
                                           value, vlanCfgPtr->vlanIdAssignCmd);
        hwData |= (value << cmdFirstBit);

        /* <VIDPrecedence> */
        PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(
                                     value, vlanCfgPtr->vlanIdAssignPrecedence);
        hwData |= (value << precedenceFirstBit);
    }

    if (qosCfgPtr != NULL)
    {
        /* check profile ID */
        PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, qosCfgPtr->qosProfileId);

        /* define field positions */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_QOS_PROFILE_E,
                cmdFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_QOS_PRECEDENCE_E,
                precedenceFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_QOS_PROFILE_E,
                qosProfileFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_DSCP_E,
                modifyDscpFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_UP_E,
                modifyUpFirstBit,length);
        }
        else
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {  /* xCat and above */
            cmdFirstBit         = 18;
            precedenceFirstBit  = 27;
            qosProfileFirstBit  = 20;
            modifyDscpFirstBit  = 17;
            modifyUpFirstBit    = 16;
        }
        else
        {
            cmdFirstBit         = 25;
            precedenceFirstBit  = 23;
            qosProfileFirstBit  = 12;
            modifyDscpFirstBit  = 19;
            modifyUpFirstBit    = 20;
        }

        /* setting <ProtocolQosProfile> */
        hwData |= (qosCfgPtr->qosProfileId << qosProfileFirstBit);

        /* setting <ModifyDSCP> */
        value = BOOL2BIT_MAC(qosCfgPtr->enableModifyDscp);
        hwData |= (value  << modifyDscpFirstBit);

        /* setting <ModifyUP> */
        value = BOOL2BIT_MAC(qosCfgPtr->enableModifyUp);
        hwData |= (value  << modifyUpFirstBit);

        /* setting <ProtocolQoSPresence> */
        PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(value,
                                                  qosCfgPtr->qosAssignPrecedence);
        hwData |= (value << precedenceFirstBit);

        /* setting the Protocol <QoSProfile Cmd> */
        PRV_CPSS_CONVERT_ATTR_ASSIGN_CMD_TO_HW_VAL_MAC(value,
                                                       qosCfgPtr->qosAssignCmd);
        hwData |= (value << cmdFirstBit);
    }

    /* set validity bit */
    hwData |= (0x1 << validBitNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* Write entry to Port Protocol VID and QOS table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                         tableEntryIndex, /* table index */
                                         &hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                             tableEntryIndex, /* table index */
                                             wordNumber,  /* word number */
                                             0,/* start at bit 0*/
                                             fieldLength, /* 28 or 32 bits */
                                             hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosSet
*
* DESCRIPTION:
*       Write Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       entryNum            - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*       vlanCfgPtr          - pointer to VID parameters
*       qosCfgPtr           - pointer to QoS parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It is possible to update only VLAN or QOS parameters, when one
*       of those input pointers is NULL.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN GT_U32                                   entryNum,
    IN CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC    *vlanCfgPtr,
    IN CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC     *qosCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortProtoVlanQosSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, entryNum, vlanCfgPtr, qosCfgPtr));

    rc = internal_cpssDxChBrgVlanPortProtoVlanQosSet(devNum, portNum, entryNum, vlanCfgPtr, qosCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, entryNum, vlanCfgPtr, qosCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortProtoVlanQosGet
*
* DESCRIPTION:
*       Get Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       entryNum            - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       vlanCfgPtr          - pointer to VID parameters (can be NULL)
*       qosCfgPtr           - pointer to QoS parameters (can be NULL)
*       validEntryPtr       - pointer to validity bit
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      It is possible to get only one of VLAN or QOS parameters by receiving
*      one of those pointers as NULL pointer or get only validity bit.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortProtoVlanQosGet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN GT_U32                                   entryNum,
    OUT CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC   *vlanCfgPtr,
    OUT CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC    *qosCfgPtr,
    OUT GT_BOOL                                 *validEntryPtr
)
{
    GT_U32      hwData;         /* data to read from register */
    GT_U32      value;          /* temporary data */
    GT_U32      tableEntryIndex;/* index in VLAN_PORT_PROTOCOL table */
    GT_U32      wordNumber;     /* word number */
    GT_U32      validBitNum;    /* position of the Valid bit */
    GT_U32      cmdFirstBit;    /* VLAN/QoS command field first bit */
    GT_U32      precedenceFirstBit; /* VLAN/QoS precedence field first bit */
    GT_U32      qosProfileFirstBit; /* QoS profile field first bit */
    GT_U32      modifyDscpFirstBit; /* QoS enable modify DSCP field first bit */
    GT_U32      modifyUpFirstBit;   /* QoS enable modify UP field first bit */
    GT_STATUS   rc;
    GT_U32      fieldLength;/* total bits used in the word */
    GT_U32      length;/*length of field*/
    GT_U32      bit,qosProfileIdMask = 0x7f;
    PRV_CPSS_DXCH_PP_TABLE_FORMAT_INFO_STC * initTablesFormatPtr =
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);
    CPSS_NULL_PTR_CHECK_MAC(validEntryPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
        fieldLength = 0;/* NOT used */
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
        fieldLength = 28;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* table is actually a matrix of Port (0..63) and Protocol (0..11).
           for every port are allocated 16 protocol entries (12 valid) */
        tableEntryIndex = portNum * 16 + entryNum;
        /* the entry has only one word */
        wordNumber = 0;

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            /* Valid bit number */
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_VALID_E,
                validBitNum,length);
        }
        else
        {
            /* Valid bit number */
            validBitNum = 15;
        }
    }
    else
    {
        /* table entry includes all protocol info per port */
        tableEntryIndex = portNum;
        /* wordNumber is protocol index */
        wordNumber = entryNum;

        /* Valid bit number */
        validBitNum = 27;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* Read entry from Port Protocol VID and QOS table */
        rc = prvCpssDxChReadTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                        tableEntryIndex, /* table index */
                                        &hwData);
    }
    else
    {
        /* Read entry from Port Protocol VID and QOS Register */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                            tableEntryIndex, /* table index */
                                            wordNumber,  /* word number */
                                            0,/* start at bit 0*/
                                            fieldLength, /* 28 or 32 bits */
                                            &hwData);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    /* get validity bit */
    value = (hwData >> validBitNum) & 0x1;
    *validEntryPtr = BIT2BOOL_MAC(value);

    /* check if VLAN is not NULL pointer */
    if (vlanCfgPtr != NULL)
    {
        /* define field positions */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_EVLAN_COMMAND_E,
                cmdFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_EVLAN_PRECEDENCE_E,
                precedenceFirstBit,length);
        }
        else
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {   /* xCat and above */
            cmdFirstBit = 12;
            precedenceFirstBit = 14;
        }
        else
        {
            cmdFirstBit = 21;
            precedenceFirstBit = 24;
        }

        /* get VLAN entry parameters */

        /* get Protocol VID */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
        {
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_EVLAN_E,
                bit,length);
            vlanCfgPtr->vlanId = (GT_U16)(hwData & BIT_MASK_MAC(length));
        }
        else
        {
            vlanCfgPtr->vlanId = (GT_U16)(hwData & 0xFFF);
        }

        /* get (VLANCmd> */
        value = (hwData >> cmdFirstBit) & 0x3;
        PRV_CPSS_CONVERT_HW_VAL_TO_ATTR_ASSIGN_CMD_MAC(vlanCfgPtr->vlanIdAssignCmd,
                                                       value);
        /* get <VIDPrecedence> */
        value = (hwData >> precedenceFirstBit) & 0x1;
        PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(vlanCfgPtr->vlanIdAssignPrecedence,
                                                  value);
    }

    /* check if QOS pointer is not NULL*/
    if (qosCfgPtr != NULL)
    {
        /* define field positions */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_QOS_PROFILE_E,
                cmdFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_QOS_PRECEDENCE_E,
                precedenceFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_QOS_PROFILE_E,
                qosProfileFirstBit,length);
            qosProfileIdMask = BIT_MASK_MAC(length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_DSCP_E,
                modifyDscpFirstBit,length);
            GET_FIELD_INFO_BY_ID_MAC(
                initTablesFormatPtr[PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E].fieldsInfoPtr,
                LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_UP_E,
                modifyUpFirstBit,length);
        }
        else
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {   /* xCat and above */
            cmdFirstBit         = 18;
            precedenceFirstBit  = 27;
            qosProfileFirstBit  = 20;
            modifyDscpFirstBit  = 17;
            modifyUpFirstBit    = 16;
        }
        else
        {
            cmdFirstBit         = 25;
            precedenceFirstBit  = 23;
            qosProfileFirstBit  = 12;
            modifyDscpFirstBit  = 19;
            modifyUpFirstBit    = 20;
        }

        /* get QOS entry parameters */

        /* get <ProtocolQosProfile> */
        qosCfgPtr->qosProfileId = (hwData >> qosProfileFirstBit) & qosProfileIdMask;

        /* get <ModifyDSCP> */
        qosCfgPtr->enableModifyDscp = BIT2BOOL_MAC((hwData >> modifyDscpFirstBit) & 0x1);

        /* get <ModifyUP> */
        qosCfgPtr->enableModifyUp = BIT2BOOL_MAC((hwData >> modifyUpFirstBit) & 0x1);

        /* get <ProtocolQoSPresence> */
        value = (hwData >> precedenceFirstBit) & 0x1;
        PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(qosCfgPtr->qosAssignPrecedence,
                                                  value);
        /* get Protocol <QoSProfile Cmd> */
        value = (hwData >> cmdFirstBit) & 0x3;
        PRV_CPSS_CONVERT_HW_VAL_TO_ATTR_ASSIGN_CMD_MAC(qosCfgPtr->qosAssignCmd,
                                                       value);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosGet
*
* DESCRIPTION:
*       Get Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       entryNum            - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       vlanCfgPtr          - pointer to VID parameters (can be NULL)
*       qosCfgPtr           - pointer to QoS parameters (can be NULL)
*       validEntryPtr       - pointer to validity bit
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or vlanId
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      It is possible to get only one of VLAN or QOS parameters by receiving
*      one of those pointers as NULL pointer or get only validity bit.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosGet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN GT_U32                                   entryNum,
    OUT CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC   *vlanCfgPtr,
    OUT CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC    *qosCfgPtr,
    OUT GT_BOOL                                 *validEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortProtoVlanQosGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, entryNum, vlanCfgPtr, qosCfgPtr, validEntryPtr));

    rc = internal_cpssDxChBrgVlanPortProtoVlanQosGet(devNum, portNum, entryNum, vlanCfgPtr, qosCfgPtr, validEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, entryNum, vlanCfgPtr, qosCfgPtr, validEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortProtoVlanQosInvalidate
*
* DESCRIPTION:
*       Invalidate protocol based QoS and VLAN assignment entry for
*       specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portNum      - port number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortProtoVlanQosInvalidate
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_U32                   entryNum
)
{
    GT_STATUS   rc;
    GT_U32      hwData;          /* value to write into HW   */
    GT_U32      tableEntryIndex; /* index in VLAN_PORT_PROTOCOL table */
    GT_U32      wordNumber;      /* word number */
    GT_U32      bitNum;          /* isValid bit number - not relevant to E_ARCH_ENABLED*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_BRG_PORT_PROTOCOL_CHECK_MAC(devNum, entryNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* table is actually a matrix of Port (0..63) and Protocol (0..11).
           for every port are allocated 16 protocol entries (12 valid) */
        tableEntryIndex = portNum * 16 + entryNum;
        /* the entry has only one word */
        wordNumber = 0;
        bitNum = 15;
    }
    else
    {
        /* table entry includes all protocol info per port */
        tableEntryIndex = portNum;
        /* wordNumber is protocol index */
        wordNumber = entryNum;
        bitNum = 27;
    }

    hwData = 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* Write entry to Port Protocol VID and QOS table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                             tableEntryIndex, /* table index */
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                             LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_VALID_E, /* field name */
                                             PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                             hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
                                             tableEntryIndex, /* table index */
                                             wordNumber,  /* word number */
                                             bitNum,/* first bit */
                                             1, /* 1 bit */
                                             hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosInvalidate
*
* DESCRIPTION:
*       Invalidate protocol based QoS and VLAN assignment entry for
*       specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portNum      - port number
*       entryNum     - entry number
*                    (APPLICABLE RANGES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3 0..7;
*                                        xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3 0..11)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosInvalidate
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_U32                   entryNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortProtoVlanQosInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, entryNum));

    rc = internal_cpssDxChBrgVlanPortProtoVlanQosInvalidate(devNum, portNum, entryNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, entryNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       vlanId    - vlan Id
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanLearningStateSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_BOOL  status
)
{
    GT_STATUS   rc;      /* return code*/
    GT_U32      hwData;  /* value to write to VLAN entry*/

    /* check for device type */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    /* if the user want to enable learning on a vlan then we :
    set Automatic Learning Disable - bit 0 in word 3 */
    hwData = (status == GT_TRUE) ? 0 : 1;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_E,
                                             vlanId,/* vid */
                                             3,/* word 3 */
                                             0,/* start at bit 0*/
                                             1, /* one bit */
                                             hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <AUTO_LEARN_DIS>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_AUTO_LEARN_DIS_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            97,/* start at bit 97*/
                                            1, /* one bit */
                                            hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       vlanId    - vlan Id
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanLearningStateSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_BOOL  status
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanLearningStateSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, status));

    rc = internal_cpssDxChBrgVlanLearningStateSet(devNum, vlanId, status);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, status));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanIsDevMember
*
* DESCRIPTION:
*       Checks if specified device has members belong to specified vlan
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - if port is member of vlan
*       GT_FALSE  - if port does not member of vlan or other status error
*
* COMMENTS:
*
*******************************************************************************/
static GT_BOOL internal_cpssDxChBrgVlanIsDevMember
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
{
    GT_STATUS   rc;
    GT_U32      hwData[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS]; /* word to read
                                                            /write entry info */
    GT_U32      hwDataMask[PRV_CPSS_DXCH_VLAN_ENTRY_MAX_WORDS_COUNT_CNS]; /* word Mask
                                                     to read/write entry info */
    GT_BOOL     result = GT_FALSE;     /* device membership result */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      i;                /* iterator */
    CPSS_PORTS_BMP_STC   portsMembers;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the bridge port members */
        rc = prvCpssDxChReadTableEntry(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E,
                                       vlanId,/* vid */
                                       &portsMembers.ports[0]);
        if(rc != GT_OK)
        {
            return GT_FALSE;
        }

        /* adjusted physical ports of BMP to hold only bmp of existing ports*/
        rc = prvCpssDxChTablePortsBmpAdjustToDevice(devNum , &portsMembers , &portsMembers);
        if(rc != GT_OK)
        {
            return rc;
        }


        return CPSS_PORTS_BMP_IS_ZERO_MAC(&portsMembers) ?
                GT_FALSE : /* no members */
                GT_TRUE;   /* at least one member */
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* mask the vlan ports membership only --- without the tagging */
        hwDataMask[0] = 0x55000000; /* 4 ports -- 8 bits */
        hwDataMask[1] = 0x55555555; /* 16 ports -- 32 bits */
        hwDataMask[2] = 0x00001555; /* 7 ports -- 14 bits */

        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* port 27 data is placed in word 3 bits[23:24] */
                hwDataMask[3] = 0x00800000; /* 1 port -- 2 bits */
            }
            else
            {
                /* port 27 data is placed in word 3 bits[8:9] */
                hwDataMask[3] = 0x00000100; /* 1 port -- 2 bits */
            }
        }
        else
        {
            hwDataMask[3] = 0;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* the port group of lion supports 12 ports */
            hwDataMask[1] = 0x00005555; /* 8 ports -- 16 bits */
            hwDataMask[2] = 0;
            hwDataMask[3] = 0;
        }

        /* get the entry from all port groups , each port group get different ports */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            /* check if we have local port in the vlan */
            rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,/* vlanId */
                                           hwData);
            if(rc != GT_OK)
            {
                return GT_FALSE;
            }

            result = ((hwData[0] & hwDataMask[0]) ||
                      (hwData[1] & hwDataMask[1]) ||
                      (hwData[2] & hwDataMask[2]) ||
                      (hwData[3] & hwDataMask[3]) ) ?
                    GT_TRUE : GT_FALSE;

            if(result == GT_TRUE)
            {
                /* no need to continue loop port groups , this port group already has port member */
                return GT_TRUE;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }
    else
    {
        /* mask the vlan ports membership only --- without the tagging */
        /* members are in bits 2..65 */
        hwDataMask[0] = 0xFFFFFFFC; /* 30 ports */
        hwDataMask[1] = 0xFFFFFFFF; /* 32 ports */
        hwDataMask[2] = 0x00000003; /* 2 ports  */

        if (GT_FALSE != PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                info_PRV_CPSS_DXCH_LION2_MC_BANDWIDTH_RESTRICTION_WA_E.enabled)
        {
            /* mask relay port 12 - bit 14 */
            U32_SET_FIELD_MAC(hwDataMask[0], 14, 1, 0);
        }

        /* loop on all TXQ units */
        for (i =0; i < PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(devNum); i++)
        {
            /* check if we have local port in the vlan */
            rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                            i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                                       PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                       (GT_U32)vlanId,/* vlanId */
                                       &hwData[0]);
            if(rc != GT_OK)
            {
                return GT_FALSE;
            }

            result |= ((hwData[0] & hwDataMask[0]) ||
                  (hwData[1] & hwDataMask[1]) ||
                  (hwData[2] & hwDataMask[2]) ) ?
                GT_TRUE : GT_FALSE;
    }

    }

    return result;
}

/*******************************************************************************
* cpssDxChBrgVlanIsDevMember
*
* DESCRIPTION:
*       Checks if specified device has members belong to specified vlan
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - if port is member of vlan
*       GT_FALSE  - if port does not member of vlan or other status error
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL cpssDxChBrgVlanIsDevMember
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
{
    GT_STATUS rc;
    GT_BOOL boolRc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIsDevMember);

    CPSS_API_LOCK_MAC(0, 0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId));

    boolRc = internal_cpssDxChBrgVlanIsDevMember(devNum, vlanId);

    if( GT_TRUE == boolRc)
    {
        rc = GT_OK;
    }
    else
    {
        rc = GT_FAIL;
    }

    /* To avoid compilation warning until LOG MACROs implementation */
    rc = rc;

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId));
    CPSS_API_UNLOCK_MAC(0, 0);

    return boolRc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanToStpIdBind
*
* DESCRIPTION:
*       Bind VLAN to STP Id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*       stpId   - STP Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_OUT_OF_RANGE          - out of range stpId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanToStpIdBind
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   stpId
)
{
    GT_STATUS   rc;
    GT_U32      hwData;     /* word to read/write entry info   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if(stpId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_STG_ID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwData = stpId;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* read entry from HW ingress Span State Group Index table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E,
                                   vlanId,/* vid */
                                   &hwData);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* egress : set the <SPAN_STATE_GROUP_INDEX>*/
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_SPANNING_E,
                                        vlanId,/* vid */
                                        &hwData);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_E,
                                             (GT_U32)vlanId,/* vid */
                                             2,/* start at word 2 */
                                             24,/* start at bit 24 */
                                             8, /* 8 bits to write */
                                             hwData);
    }
    else
    {
        /* write to ingress vlan */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            130, /* start at bit 130 */
                                            8, /* 8 bits */
                                            hwData);

        if(rc != GT_OK)
        {
            return rc;
        }

        /* write to egress vlan */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            254, /* start at bit 254 */
                                            8, /* 8 bits */
                                            hwData);

        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanToStpIdBind
*
* DESCRIPTION:
*       Bind VLAN to STP Id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*       stpId   - STP Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_OUT_OF_RANGE          - out of range stpId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanToStpIdBind
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   stpId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanToStpIdBind);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, stpId));

    rc = internal_cpssDxChBrgVlanToStpIdBind(devNum, vlanId, stpId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, stpId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanStpIdGet
*
* DESCRIPTION:
*       Read STP Id that bind to specified VLAN.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*
* OUTPUTS:
*       stpIdPtr   - pointer to STP Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanStpIdGet
(
    IN  GT_U8    devNum,
    IN  GT_U16   vlanId,
    OUT GT_U16   *stpIdPtr
)
{
    GT_U32      hwData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    CPSS_NULL_PTR_CHECK_MAC(stpIdPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* read entry from HW ingress Span State Group Index table */
        rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E,
                                   vlanId,/* vid */
                                   &hwData);
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         2,/* start at word 2 */
                                         24,/* start at bit 24 */
                                         8, /* 8 bits to read */
                                         &hwData);
    }
    else
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              130,/* start at bit 130 */
                                              8, /* 8 bits */
                                              &hwData);
    }

    *stpIdPtr = (GT_U16)hwData;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanStpIdGet
*
* DESCRIPTION:
*       Read STP Id that bind to specified VLAN.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*
* OUTPUTS:
*       stpIdPtr   - pointer to STP Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanStpIdGet
(
    IN  GT_U8    devNum,
    IN  GT_U16   vlanId,
    OUT GT_U16   *stpIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanStpIdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, stpIdPtr));

    rc = internal_cpssDxChBrgVlanStpIdGet(devNum, vlanId, stpIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, stpIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEtherTypeSet
*
* DESCRIPTION:
*       Sets vlan Ether Type mode and vlan range.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*       etherType     - the EtherType to be recognized tagged packets. See
*                       comment.
*       vidRange      - the maximum vlan range to accept for the specified
*                       etheType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal vidRange value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the vid of the incoming packet is greater or equal (>=) than the vlan
*       range value, the packet is filtered.
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType,
    IN  GT_U16                  vidRange
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;         /* register address */
    GT_U32      value;           /* value to write into register */
    GT_U32      offset;          /* offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum, vidRange);
    PRV_CPSS_ETHER_TYPE_MODE_CHECK_MAC(etherTypeMode);

    /* set ether type */
    value = etherType;
    offset = (etherTypeMode == CPSS_VLAN_ETHERTYPE0_E) ? 0 : 16;

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan1;

    rc = prvCpssHwPpSetRegField(devNum, regAddr ,offset, 16, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* for vlan etherType coherency between ingress and egress vlan etherType
    configuration register must be configured to match the ingressVlanSel */
    regAddr =
      PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.egressVlanEtherTypeConfig;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 16, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set vid range */
    offset = (etherTypeMode == CPSS_VLAN_ETHERTYPE0_E) ? 0 : 12;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    VLANRangesConfig.ingrVLANRangeConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan0;
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset,
        PRV_CPSS_DXCH_PP_HW_NUM_BITS_VID_MAC(devNum), vidRange);

    return rc ;
}

/*******************************************************************************
* cpssDxChBrgVlanEtherTypeSet
*
* DESCRIPTION:
*       Sets vlan Ether Type mode and vlan range.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*       etherType     - the EtherType to be recognized tagged packets. See
*                       comment.
*       vidRange      - the maximum vlan range to accept for the specified
*                       etheType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal vidRange value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the vid of the incoming packet is greater or equal (>=) than the vlan
*       range value, the packet is filtered.
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType,
    IN  GT_U16                  vidRange
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEtherTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherTypeMode, etherType, vidRange));

    rc = internal_cpssDxChBrgVlanEtherTypeSet(devNum, etherTypeMode, etherType, vidRange);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherTypeMode, etherType, vidRange));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEtherTypeGet
*
* DESCRIPTION:
*       Gets vlan Ether Type mode and vlan range.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*
* OUTPUTS:
*       etherTypePtr  - (pointer to)the EtherType to be recognized tagged
*                        packets. See comment.
*       vidRangePtr   - (pointer to)the maximum vlan range to accept for
*                        the specified etheType.

*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal vidRange value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEtherTypeGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    OUT  GT_U16                 *etherTypePtr,
    OUT  GT_U16                 *vidRangePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;         /* register address */
    GT_U32      value;           /* register value */
    GT_U32      offset;          /* offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_ETHER_TYPE_MODE_CHECK_MAC(etherTypeMode);
    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(vidRangePtr);

    /* get EtherType */
    offset = (etherTypeMode == CPSS_VLAN_ETHERTYPE0_E) ? 0 : 16;

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan1;

    rc = prvCpssHwPpGetRegField(devNum, regAddr ,offset, 16, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *etherTypePtr = (GT_U16)value;

    /* get vid range */
    offset = (etherTypeMode == CPSS_VLAN_ETHERTYPE0_E) ? 0 : 12;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan0;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 12, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *vidRangePtr =  (GT_U16)value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanEtherTypeGet
*
* DESCRIPTION:
*       Gets vlan Ether Type mode and vlan range.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*
* OUTPUTS:
*       etherTypePtr  - (pointer to)the EtherType to be recognized tagged
*                        packets. See comment.
*       vidRangePtr   - (pointer to)the maximum vlan range to accept for
*                        the specified etheType.

*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal vidRange value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEtherTypeGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    OUT  GT_U16                 *etherTypePtr,
    OUT  GT_U16                 *vidRangePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEtherTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherTypeMode, etherTypePtr, vidRangePtr));

    rc = internal_cpssDxChBrgVlanEtherTypeGet(devNum, etherTypeMode, etherTypePtr, vidRangePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherTypeMode, etherTypePtr, vidRangePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTableInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN Table entries by writing 0 to the
*       first word.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTableInvalidate
(
    IN GT_U8 devNum
)
{
    GT_STATUS   rc;
    GT_U16      vlanId;
    GT_U32      maxVid;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    maxVid = PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_VLAN_MAC(devNum);

    /* reset all valid entries in vlan table */
    for(vlanId = 0 ; vlanId <= maxVid ; vlanId ++)
    {
        /* write to word 0 bit 0 in HW */
        rc = cpssDxChBrgVlanEntryInvalidate(devNum, vlanId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Disable ingress mirroring of packets to the Ingress Analyzer */
        rc = cpssDxChBrgVlanIngressMirrorEnable(devNum, vlanId, GT_FALSE,0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanTableInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN Table entries by writing 0 to the
*       first word.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTableInvalidate
(
    IN GT_U8 devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTableInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChBrgVlanTableInvalidate(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       vlanId     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
{
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* ingress : set the <VALID>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VALID_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        0);
        if(rc != GT_OK)
        {
            return rc;
        }
        if (!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
        {
            /* egress : set the <VALID>*/
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                            vlanId,/* vid */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_VALID_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            0);
        }
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* write to word 0 bit 0 in HW */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         vlanId,/* vid */
                                         0,/* start at word 0 */
                                         0,/* start at bit 0 */
                                         1, /* 1 bit */
                                         0);
    }
    else
    {
        /* invalidate the ingress vlan table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                              vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              0,
                                              1,
                                              0);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* invalidate the egress vlan table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                              vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              0,
                                              1,
                                              0);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       vlanId     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEntryInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId));

    rc = internal_cpssDxChBrgVlanEntryInvalidate(devNum, vlanId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanUnkUnregFilterSet
*
* DESCRIPTION:
*       Set per VLAN filtering command for specified Unknown or Unregistered
*       packet type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       vlanId      - VLAN ID
*       packetType  - packet type
*       cmd         - command for the specified packet type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanUnkUnregFilterSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT   packetType,
    IN CPSS_PACKET_CMD_ENT                  cmd

)
{
    GT_STATUS   rc;
    GT_U32      hwValue;             /* hardware value of the command */
    GT_U32      wordOffset;          /* the word number in the VLAN Table */
    GT_U32      bitOffset;           /* bit offset in the word */
    GT_U32      globalOffset;/* relevant only to splitTablesSupported */
    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_ENT fieldName;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    /* check the type of the packet */
    switch (packetType)
    {
        case CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E:
             /* set unknown unicast - word 0, bits 12-14 */
            wordOffset = 0;
            bitOffset = 12;

            globalOffset = 75;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKNOWN_UNICAST_CMD_E;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E:
            /* set unregistered non IP multicast command - word 0, bits 3-5 */
            wordOffset = 0;
            bitOffset = 3;

            globalOffset = 66;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IP_MULTICAST_CMD_E;

            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E:
            /* set unregistered IPv4 multicast command - word 0, bits 6-8 */
            wordOffset = 0;
            bitOffset = 6;

            globalOffset = 69;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_MULTICAST_CMD_E;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E:
            /* set unregistered IPv6 multicast command - word 0, bits 9-11 */
            wordOffset = 0;
            bitOffset = 9;

            globalOffset = 72;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV6_MULTICAST_CMD_E;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E:
            /* set unregistered non IPv4 broadcast command - word 2,
                                                            bits 21-23 */
            wordOffset = 2;
            bitOffset = 21;

            globalOffset = 89;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IPV4_BC_CMD_E;
            break;

        case CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E:
            /* set unregistered IPv4 broadcast command - word 2, bits 18-20 */
            wordOffset = 2;
            bitOffset = 18;

            globalOffset = 86;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_BC_CMD_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(hwValue, cmd)

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the relevant field */
        return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        fieldName, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwValue);

    }
    else
    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             globalOffset,
                                             3, /* 3 bits */
                                             hwValue);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_E,
                                             vlanId,/* vid */
                                             wordOffset,    /* word # */
                                             bitOffset,     /* start at bit */
                                             3, /* 3 bits */
                                             hwValue);
        return rc;
    }

}

/*******************************************************************************
* cpssDxChBrgVlanUnkUnregFilterSet
*
* DESCRIPTION:
*       Set per VLAN filtering command for specified Unknown or Unregistered
*       packet type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       vlanId      - VLAN ID
*       packetType  - packet type
*       cmd         - command for the specified packet type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUnkUnregFilterSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT   packetType,
    IN CPSS_PACKET_CMD_ENT                  cmd

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanUnkUnregFilterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, packetType, cmd));

    rc = internal_cpssDxChBrgVlanUnkUnregFilterSet(devNum, vlanId, packetType, cmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, packetType, cmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanForcePvidEnable
*
* DESCRIPTION:
*       Set Port VID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE  - PVID is assigned to all packets.
*                  GT_FALSE - PVID is assigned to untagged or priority tagged
*                             packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanForcePvidEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS   rc;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_U32      hwData;     /* data to write to Hw */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    hwData = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
         rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_EVLAN_MODE_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 27;
        }
        else
        {
            startWord = 1;
            startBit  = 15;
        }

        /* write the value "forceEn" to <PVIDMode> field of port-vlan-qos table  */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,/* port */
                                             startWord,
                                             startBit,
                                             1, /* set 1 bits */
                                             hwData);
    }
     return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanForcePvidEnable
*
* DESCRIPTION:
*       Set Port VID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE  - PVID is assigned to all packets.
*                  GT_FALSE - PVID is assigned to untagged or priority tagged
*                             packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForcePvidEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanForcePvidEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanForcePvidEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanForcePvidEnableGet
*
* DESCRIPTION:
*       Get Port VID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*                  In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr   - GT_TRUE  - PVID is assigned to all packets.
*                     GT_FALSE - PVID is assigned to untagged or priority tagged
*                                packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanForcePvidEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_U32      hwData;     /* data to write to Hw */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read the value "forceEn" from <PVIDMode> field of pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_EVLAN_MODE_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &hwData);


    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 27;
        }
        else
        {
            startWord = 1;
            startBit  = 15;
        }

        /* read the value "forceEn" from <PVIDMode> field of port-vlan-qos table  */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            portNum,/* port */
                                            startWord,
                                            startBit,
                                            1, /* set 1 bits */
                                            &hwData);
    }
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

     return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanForcePvidEnableGet
*
* DESCRIPTION:
*       Get Port VID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*                  In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr   - GT_TRUE  - PVID is assigned to all packets.
*                     GT_FALSE - PVID is assigned to untagged or priority tagged
*                                packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForcePvidEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanForcePvidEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanForcePvidEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortVidPrecedenceSet
*
* DESCRIPTION:
*       Set Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*                     In eArch devices portNum is default ePort.
*       precedence  - precedence type - soft or hard
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVidPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence
)
{
    GT_STATUS   rc;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_U32      hwData;     /* data to write to Hw */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(hwData, precedence);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
         rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_VID_PRECEDENCE_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 29;
        }
        else
        {
            startWord = 1;
            startBit  = 16;
        }

        /* write the value to <PVIDPrecedence> field of port-vlan-qos table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,/* port */
                                             startWord,
                                             startBit,
                                             1, /* 1 bit */
                                             hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidPrecedenceSet
*
* DESCRIPTION:
*       Set Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*                     In eArch devices portNum is default ePort.
*       precedence  - precedence type - soft or hard
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVidPrecedenceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, precedence));

    rc = internal_cpssDxChBrgVlanPortVidPrecedenceSet(devNum, portNum, precedence);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, precedence));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortVidPrecedenceGet
*
* DESCRIPTION:
*       Get Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*                     In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       precedencePtr - (pointer to) precedence type - soft or hard
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVidPrecedenceGet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    OUT CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT *precedencePtr
)
{
    GT_STATUS   rc;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_U32      hwData;     /* data to read from Hw */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(precedencePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read the value to <PVIDPrecedence> field of pre-tti-lookup-ingress-eport table  */
         rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_VID_PRECEDENCE_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat and above */
            startWord = 0;
            startBit  = 29;
        }
        else
        {
            startWord = 1;
            startBit  = 16;
        }

        /* read the value to <PVIDPrecedence> field of port-vlan-qos table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            portNum,/* port */
                                            startWord,
                                            startBit,
                                            1, /* 1 bit */
                                            &hwData);
    }
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(*precedencePtr, hwData)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidPrecedenceGet
*
* DESCRIPTION:
*       Get Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*                     In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       precedencePtr - (pointer to) precedence type - soft or hard
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVidPrecedenceGet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    OUT CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT *precedencePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVidPrecedenceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, precedencePtr));

    rc = internal_cpssDxChBrgVlanPortVidPrecedenceGet(devNum, portNum, precedencePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, precedencePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpUcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Unicast Routing on Vlan
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       enable      - GT_TRUE - enable ip unicast routing;
*                     GT_FALSE -  disable ip unicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpUcRouteEnable
(
    IN GT_U8                                    devNum,
    IN GT_U16                                   vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocol,
    IN GT_BOOL                                  enable
)
{
    GT_U32      globalOffset;/* relevant only to splitTablesSupported */
    GT_U32      bitOffset;      /* bit offset in the VLAN Entry word */
    GT_U32      hwData;         /* data to write to register */
    GT_STATUS   rc;             /* return status */
    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_ENT fieldName;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    /* when the user want to Ip routing or disable */
    hwData = BOOL2BIT_MAC(enable);
    rc = GT_OK;

    switch(protocol)
    {
        case CPSS_IP_PROTOCOL_IPV4_E :
            bitOffset = 14;
            globalOffset = 92;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_UNICAST_ROUTE_EN_E;
            break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            /* ipv4 */
            rc = cpssDxChBrgVlanIpUcRouteEnable(devNum,vlanId,
                                                 CPSS_IP_PROTOCOL_IPV4_E,
                                                 enable);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* ipv6 */
            return cpssDxChBrgVlanIpUcRouteEnable(devNum,vlanId,
                                                 CPSS_IP_PROTOCOL_IPV6_E,
                                                 enable);
        case CPSS_IP_PROTOCOL_IPV6_E :
            bitOffset = 16;
            globalOffset = 94;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_UNICAST_ROUTE_EN_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           vlanId,/* vid */
                                           2,             /* word 2 */
                                           bitOffset,     /* offset */
                                           1,             /* 1 bit */
                                           hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the relevant field */
        return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        fieldName, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               vlanId,/* vid */
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               globalOffset,  /* offset */
                                               1,             /* 1 bit */
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanIpUcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Unicast Routing on Vlan
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       enable      - GT_TRUE - enable ip unicast routing;
*                     GT_FALSE -  disable ip unicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpUcRouteEnable
(
    IN GT_U8                                    devNum,
    IN GT_U16                                   vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocol,
    IN GT_BOOL                                  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpUcRouteEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, protocol, enable));

    rc = internal_cpssDxChBrgVlanIpUcRouteEnable(devNum, vlanId, protocol, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, protocol, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpMcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Multicast Routing on Vlan
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       enable      - GT_TRUE - enable ip multicast routing;
*                     GT_FALSE -  disable ip multicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpMcRouteEnable
(
    IN GT_U8                            devNum,
    IN GT_U16                           vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN GT_BOOL                          enable
)
{
    GT_U32      globalOffset;/* relevant only to splitTablesSupported */
    GT_U32      bitOffset;    /* bit offset in the VLAN Entry word */
    GT_U32      hwData;       /* data to write to register */
    GT_STATUS   rc;           /* return status */
    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_ENT fieldName;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    /* when the user want to Ip routing or disable */
    hwData = BOOL2BIT_MAC(enable);
    rc = GT_OK;

    switch(protocol)
    {
        case CPSS_IP_PROTOCOL_IPV4_E :
            bitOffset = 15;
            globalOffset = 93;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MULTICAST_ROUTE_EN_E;
            break;

        case CPSS_IP_PROTOCOL_IPV4V6_E:
            /* ipv4 */
            rc = cpssDxChBrgVlanIpMcRouteEnable(devNum,vlanId,
                                                 CPSS_IP_PROTOCOL_IPV4_E,
                                                 enable);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* ipv6 */
            return cpssDxChBrgVlanIpMcRouteEnable(devNum,vlanId,
                                                 CPSS_IP_PROTOCOL_IPV6_E,
                                                 enable);
        case CPSS_IP_PROTOCOL_IPV6_E :
            bitOffset = 17;
            globalOffset = 95;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MULTICAST_ROUTE_EN_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           vlanId,/* vid */
                                           2,             /* word 2 */
                                           bitOffset,     /* offset */
                                           1,             /* 1 bit */
                                           hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the relevant field */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        fieldName, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               vlanId,/* vid */
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               globalOffset,  /* offset */
                                               1,             /* 1 bit */
                                               hwData);
    }

    if(rc != GT_OK)
        return rc;

    /* If Lion2 errata is initialized set port 14 (hemisphere 0) and
       port 64+14 (hemisphere 1) as a member in the vlan */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_LION2_IPM_BRIDGE_COPY_GET_DROPPED_WA_E.enabled == GT_TRUE)
    {
        /* add port 14 to vlan */
        rc = cpssDxChBrgVlanMemberAdd(devNum, vlanId, 14, GT_FALSE, CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);

        if(rc != GT_OK)
            return rc;

        if (PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(devNum) >= 2)
        {
            /* add port 64+14to vlan */
            rc = cpssDxChBrgVlanMemberAdd(devNum, vlanId, 78, GT_FALSE, CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);

            if(rc != GT_OK)
                return rc;
        }

    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpMcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Multicast Routing on Vlan
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       enable      - GT_TRUE - enable ip multicast routing;
*                     GT_FALSE -  disable ip multicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpMcRouteEnable
(
    IN GT_U8                            devNum,
    IN GT_U16                           vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN GT_BOOL                          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpMcRouteEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, protocol, enable));

    rc = internal_cpssDxChBrgVlanIpMcRouteEnable(devNum, vlanId, protocol, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, protocol, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanNASecurEnable
*
* DESCRIPTION:
*      This function enables/disables per VLAN the generation of
*      security breach event for packets with unknown Source MAC addresses.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN Id
*       enable  - GT_TRUE - enable generation of security breach event
*                 GT_FALSE- disable generation of security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanNASecurEnable
(
    IN GT_U8           devNum,
    IN GT_U16          vlanId,
    IN GT_BOOL         enable
)
{
    GT_U32  hwData;         /* for write entry word */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    /* when the user want to enable NA security breach on the vlan we
       clear the bit of NA NOT security breach */
    hwData = (enable == GT_TRUE) ? 0 : 1;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* set bit 1 in word 0 accordingly */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         vlanId,/* vid */
                                         0,/* word 0 */
                                         1,/* start at bit 1*/
                                         1, /* one bit */
                                         hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <NEW_SRC_ADDR_IS_NOT_SECURITY_BREACH> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NEW_SRC_ADDR_IS_NOT_SECURITY_BREACH_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             1,
                                             1, /* 1 bit */
                                             hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanNASecurEnable
*
* DESCRIPTION:
*      This function enables/disables per VLAN the generation of
*      security breach event for packets with unknown Source MAC addresses.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN Id
*       enable  - GT_TRUE - enable generation of security breach event
*                 GT_FALSE- disable generation of security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanNASecurEnable
(
    IN GT_U8           devNum,
    IN GT_U16          vlanId,
    IN GT_BOOL         enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanNASecurEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable));

    rc = internal_cpssDxChBrgVlanNASecurEnable(devNum, vlanId, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChBrgVlanIgmpSnoopingEnable
*
* DESCRIPTION:
*       Enable/Disable IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*       enable  - GT_TRUE - enable
*                 GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIgmpSnoopingEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable
)
{
    GT_U32      value;  /* value to write into register */
    GT_STATUS   rc;     /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    value = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv4IGMPToCPUEn> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_TABLE_VLAN_E,
                                    vlanId,
                                    0,
                                    15,
                                    1,
                                    value);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <IPV4_IGMP_TO_CPU_EN> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IGMP_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        78,
                                        1,
                                        value);
    }


    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIgmpSnoopingEnable
*
* DESCRIPTION:
*       Enable/Disable IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*       enable  - GT_TRUE - enable
*                 GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIgmpSnoopingEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIgmpSnoopingEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable));

    rc = internal_cpssDxChBrgVlanIgmpSnoopingEnable(devNum, vlanId, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIgmpSnoopingEnableGet
*
* DESCRIPTION:
*       Gets the status of IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable
*                    GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIgmpSnoopingEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;  /* value to write into register */
    GT_STATUS   rc;     /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv4IGMPToCPUEn> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_TABLE_VLAN_E,
                                    vlanId,
                                    0,
                                    15,
                                    1,
                                    &value);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <IPV4_IGMP_TO_CPU_EN> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IGMP_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
    }
    else
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        78,
                                        1,
                                        &value);
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIgmpSnoopingEnableGet
*
* DESCRIPTION:
*       Gets the status of IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable
*                    GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIgmpSnoopingEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIgmpSnoopingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enablePtr));

    rc = internal_cpssDxChBrgVlanIgmpSnoopingEnableGet(devNum, vlanId, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*****************************************************************************
* internal_cpssDxChBrgVlanIpCntlToCpuSet
*
* DESCRIPTION:
*       Enable/disable IP control traffic trapping/mirroring to CPU.
*       When set, this enables the following control traffic to be eligible
*       for mirroring/trapping to the CPU:
*        - ARP
*        - IPv6 Neighbor Solicitation
*        - IPv4/v6 Control Protocols Running Over Link-Local Multicast
*        - RIPv1 MAC Broadcast
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*       ipCntrlType - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpCntlToCpuSet
(
    IN GT_U8                               devNum,
    IN GT_U16                              vlanId,
    IN CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      ipCntrlType
)
{
    GT_U32      value;  /* value to write into register */
    GT_U32      value1; /* value to write into register */
    GT_STATUS   rc;     /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    switch(ipCntrlType)
    {

        case CPSS_DXCH_BRG_IP_CTRL_NONE_E:
            value = 0;
            value1 = 0;
            break;

        case CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E:
            value = 1;
            value1 = 1;
            break;

        case CPSS_DXCH_BRG_IP_CTRL_IPV4_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            value = 1;
            value1 = 0;
            break;

        case CPSS_DXCH_BRG_IP_CTRL_IPV6_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            value = 0;
            value1 = 1;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv4ControlToCPUEnable> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_VLAN_E,
                                            (GT_U32)vlanId,
                                            0,
                                            18,
                                            1,
                                            value);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /* VLAN<vlanId> Entry Word<3>, field <IPv6ControlToCPUEnable> */
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                PRV_CPSS_DXCH_TABLE_VLAN_E,
                                                (GT_U32)vlanId,
                                                3,
                                                7,
                                                1,
                                                value1);

        }
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <IPV4_CONTROL_TO_CPU_EN> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_CONTROL_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set the <IPV6_CONTROL_TO_CPU_EN> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_CONTROL_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value1);
    }
    else
    {
        /*Ipv4ControlToCpuEn*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            83,
                                            1,
                                            value);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*Ipv6ControlToCpuEn*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            103,
                                            1,
                                            value1);
    }

    return rc;
}

/*****************************************************************************
* cpssDxChBrgVlanIpCntlToCpuSet
*
* DESCRIPTION:
*       Enable/disable IP control traffic trapping/mirroring to CPU.
*       When set, this enables the following control traffic to be eligible
*       for mirroring/trapping to the CPU:
*        - ARP
*        - IPv6 Neighbor Solicitation
*        - IPv4/v6 Control Protocols Running Over Link-Local Multicast
*        - RIPv1 MAC Broadcast
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*       ipCntrlType - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpCntlToCpuSet
(
    IN GT_U8                               devNum,
    IN GT_U16                              vlanId,
    IN CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      ipCntrlType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpCntlToCpuSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, ipCntrlType));

    rc = internal_cpssDxChBrgVlanIpCntlToCpuSet(devNum, vlanId, ipCntrlType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, ipCntrlType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*****************************************************************************
* internal_cpssDxChBrgVlanIpCntlToCpuGet
*
* DESCRIPTION:
*       Gets IP control traffic trapping/mirroring to CPU status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*
* OUTPUTS:
*       ipCntrlTypePtr - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpCntlToCpuGet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    OUT CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      *ipCntrlTypePtr
)
{
    GT_U32      value=0;  /* value to read from register */
    GT_U32      value1=0; /* value to read from register */
    GT_STATUS   rc;      /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    CPSS_NULL_PTR_CHECK_MAC(ipCntrlTypePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv4ControlToCPUEnable> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_VLAN_E,
                                            (GT_U32)vlanId,
                                            0,
                                            18,
                                            1,
                                            &value);

        if(rc != GT_OK)
            return rc;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /* VLAN<vlanId> Entry Word<3>, field <IPv6ControlToCPUEnable> */
            rc = prvCpssDxChReadTableEntryField(devNum,
                                                PRV_CPSS_DXCH_TABLE_VLAN_E,
                                                (GT_U32)vlanId,
                                                3,
                                                7,
                                                1,
                                                &value1);
            if(rc != GT_OK)
                return rc;

        }
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <IPV4_CONTROL_TO_CPU_EN> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_CONTROL_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

        /* get the <IPV6_CONTROL_TO_CPU_EN> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_CONTROL_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value1);
    }
    else
    {
        /*Ipv4ControlToCpuEn*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            83,
                                            1,
                                            &value);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*Ipv6ControlToCpuEn*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            103,
                                            1,
                                            &value1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* cheetah devices have only two options */
        *ipCntrlTypePtr = (value) ? CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E : CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }
    else
    {
        if((value == 0)&&(value1 == 0))
        {
            *ipCntrlTypePtr = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
        }
        else
        {
            if((value == 1)&&(value1 == 1))
            {
                *ipCntrlTypePtr = CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E;
            }
            else
            {
                if((value == 1)&&(value1 == 0))
                {
                    *ipCntrlTypePtr = CPSS_DXCH_BRG_IP_CTRL_IPV4_E;
                }
                else/* ((value == 0)&&(value1 == 1)) */
                {
                     *ipCntrlTypePtr = CPSS_DXCH_BRG_IP_CTRL_IPV6_E;
                }
            }
        }
    }

    return rc;
}

/*****************************************************************************
* cpssDxChBrgVlanIpCntlToCpuGet
*
* DESCRIPTION:
*       Gets IP control traffic trapping/mirroring to CPU status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*
* OUTPUTS:
*       ipCntrlTypePtr - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpCntlToCpuGet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    OUT CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      *ipCntrlTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpCntlToCpuGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, ipCntrlTypePtr));

    rc = internal_cpssDxChBrgVlanIpCntlToCpuGet(devNum, vlanId, ipCntrlTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, ipCntrlTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpV6IcmpToCpuEnable
*
* DESCRIPTION:
*       Enable/Disable ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*        enable   - GT_TRUE - enable
*                   GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpV6IcmpToCpuEnable
(
    IN  GT_U8      devNum,
    IN  GT_U16     vlanId,
    IN  GT_BOOL    enable
)
{
    GT_U32      hwData;     /* data to write into register */
    GT_STATUS   rc;         /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    hwData = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv6ICMPToCPUEn> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (GT_U32)vlanId,
                                        0,  /* word0 */
                                        17,
                                        1,
                                        hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <IPV6_ICMP_TO_CPU_EN>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_ICMP_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        /* <Ipv6IcmpToCpuEn> */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            82,
                                            1,
                                            hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpV6IcmpToCpuEnable
*
* DESCRIPTION:
*       Enable/Disable ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*        enable   - GT_TRUE - enable
*                   GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpV6IcmpToCpuEnable
(
    IN  GT_U8      devNum,
    IN  GT_U16     vlanId,
    IN  GT_BOOL    enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpV6IcmpToCpuEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable));

    rc = internal_cpssDxChBrgVlanIpV6IcmpToCpuEnable(devNum, vlanId, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
*
* DESCRIPTION:
*       Gets status of ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*
* OUTPUTS:
*        enablePtr   - GT_TRUE - enable
*                      GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U16     vlanId,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      hwData;     /* data to write into register */
    GT_STATUS   rc;         /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* VLAN<vlanId> Entry Word<0>, field <IPv6ICMPToCPUEn> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        (GT_U32)vlanId,
                                        0,  /* word0 */
                                        17,
                                        1,
                                        &hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <IPV6_ICMP_TO_CPU_EN>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_ICMP_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwData);
    }
    else
    {
        /* <Ipv6IcmpToCpuEn> */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            82,
                                            1,
                                            &hwData);
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
*
* DESCRIPTION:
*       Gets status of ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*
* OUTPUTS:
*        enablePtr   - GT_TRUE - enable
*                      GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U16     vlanId,
    OUT GT_BOOL    *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpV6IcmpToCpuEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enablePtr));

    rc = internal_cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(devNum, vlanId, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
* DESCRIPTION:
*     Enable or disable per Vlan the Broadcast UDP packets Mirror/Trap to the
*     CPU based on their destination UDP port.  Destination UDP port, command
*     (trap or mirror) and CPU code configured by
* internal_cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*       enable - Enable/disable UDP Trap Mirror.
*                GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanUdpBcPktsToCpuEnable
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    IN  GT_BOOL     enable
)
{
    GT_U32      hwData;     /* data to write into register */

    /* only for Cheetah2 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    hwData = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Enable/disable trapping or mirroring BC UDP packets based on the
        destination UDP port for UDP relay support */
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,
                                           3,   /* word3 */
                                           6,
                                           1,
                                           hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <BC_UDP_TRAP_MIRROR_EN>*/
        return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_BC_UDP_TRAP_MIRROR_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               102,
                                               1,
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
* DESCRIPTION:
*     Enable or disable per Vlan the Broadcast UDP packets Mirror/Trap to the
*     CPU based on their destination UDP port.  Destination UDP port, command
*     (trap or mirror) and CPU code configured by
*     cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*       enable - Enable/disable UDP Trap Mirror.
*                GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUdpBcPktsToCpuEnable
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanUdpBcPktsToCpuEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable));

    rc = internal_cpssDxChBrgVlanUdpBcPktsToCpuEnable(devNum, vlanId, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
*
* DESCRIPTION:
*     Gets per Vlan if the Broadcast UDP packets are Mirror/Trap to the
*     CPU based on their destination UDP port.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*
* OUTPUTS:
*       enablePtr - UDP Trap Mirror status
*                   GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;     /* data to write into register */

    /* only for Cheetah2 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,
                                           3,   /* word3 */
                                           6,
                                           1,
                                           &hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <BC_UDP_TRAP_MIRROR_EN>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_BC_UDP_TRAP_MIRROR_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwData);
    }
    else
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            (GT_U32)vlanId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            102,
                                            1,
                                            &hwData);
    }


    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
*
* DESCRIPTION:
*     Gets per Vlan if the Broadcast UDP packets are Mirror/Trap to the
*     CPU based on their destination UDP port.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*
* OUTPUTS:
*       enablePtr - UDP Trap Mirror status
*                   GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanUdpBcPktsToCpuEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enablePtr));

    rc = internal_cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(devNum, vlanId, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpv6SourceSiteIdSet
*
* DESCRIPTION:
*       Sets a vlan ipv6 site id
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan.
*       siteId          - the site id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpv6SourceSiteIdSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_IP_SITE_ID_ENT                  siteId
)
{
    GT_U32      hwData;         /* data to write to register */
    GT_STATUS   rc;     /* status to return */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if(siteId != CPSS_IP_SITE_ID_INTERNAL_E &&
       siteId != CPSS_IP_SITE_ID_EXTERNAL_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* when the user want to Ip routing or disable */
    hwData = (siteId == CPSS_IP_SITE_ID_INTERNAL_E) ? 0 : 1;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         0,             /* word 0 */
                                         23,            /* bit 23*/
                                         1,             /* one bit */
                                         hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <IPV6_SITEID>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_SITEID_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             96,
                                             1,             /* one bit */
                                             hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpv6SourceSiteIdSet
*
* DESCRIPTION:
*       Sets a vlan ipv6 site id
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan.
*       siteId          - the site id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv6SourceSiteIdSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_IP_SITE_ID_ENT                  siteId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpv6SourceSiteIdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, siteId));

    rc = internal_cpssDxChBrgVlanIpv6SourceSiteIdSet(devNum, vlanId, siteId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, siteId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpmBridgingEnable
*
* DESCRIPTION:
*       IPv4/Ipv6 Multicast Bridging Enable
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device id
*       vlanId  - Vlan Id
*       ipVer   - IPv4 or Ipv6
*       enable  - GT_TRUE - IPv4 multicast packets are bridged
*                   according to ipv4IpmBrgMode
*                 GT_FALSE - IPv4 multicast packets are bridged
*                   according to MAC DA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpmBridgingEnable
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  GT_BOOL                     enable
)
{
    GT_U32  bitOffset;      /* bit offset in the VLAN Entry word */
    GT_U32  dataLength;     /* length of write data */
    GT_U32  hwData;         /* data to write to register */
    GT_U32  globalOffset;/* relevant only to splitTablesSupported */
    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_ENT fieldName;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    hwData = BOOL2BIT_MAC(enable);

    switch (ipVer)
    {
        case CPSS_IP_PROTOCOL_IPV6_E:
            bitOffset = 22;
            dataLength = 1;
            globalOffset = 85;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_EN_E;
            break;
        case CPSS_IP_PROTOCOL_IPV4_E:
            bitOffset = 21;
            dataLength = 1;
            globalOffset = 84;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_EN_E;
            break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            bitOffset = 21;
            dataLength = 2;
            hwData = hwData | (hwData << 1);
            globalOffset = 84;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_EN_E;/*2 fields from this one */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId, /* vid    */
                                           0,              /* word 0 */
                                           bitOffset,      /* offset */
                                           dataLength,
                                           hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the relevant field(s)*/
        return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        fieldName, /* field name */
                                        dataLength,/* indication for 1 or 2 fields */
                                        hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               globalOffset,
                                               dataLength,
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanIpmBridgingEnable
*
* DESCRIPTION:
*       IPv4/Ipv6 Multicast Bridging Enable
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device id
*       vlanId  - Vlan Id
*       ipVer   - IPv4 or Ipv6
*       enable  - GT_TRUE - IPv4 multicast packets are bridged
*                   according to ipv4IpmBrgMode
*                 GT_FALSE - IPv4 multicast packets are bridged
*                   according to MAC DA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpmBridgingEnable
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  GT_BOOL                     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpmBridgingEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, ipVer, enable));

    rc = internal_cpssDxChBrgVlanIpmBridgingEnable(devNum, vlanId, ipVer, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, ipVer, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpmBridgingModeSet
*
* DESCRIPTION:
*       Sets the IPM bridging mode of Vlan. Relevant when IPM Bridging enabled.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device id
*       vlanId      - Vlan Id
*       ipVer       - IPv4 or Ipv6
*       ipmMode    - IPM bridging mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpmBridgingModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  CPSS_BRG_IPM_MODE_ENT       ipmMode
)
{
    GT_U32  bitOffset;      /* bit offset in the VLAN Entry word */
    GT_U32  dataLength;     /* length of write data */
    GT_U32  hwData;         /* data to write to register */
    GT_U32  globalOffset;/* relevant only to splitTablesSupported */
    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_ENT fieldName;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    switch (ipmMode)
    {
        case CPSS_BRG_IPM_SGV_E:
            hwData = 0;
            break;
        case CPSS_BRG_IPM_GV_E:
            hwData = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (ipVer)
    {
        case CPSS_IP_PROTOCOL_IPV6_E:
            bitOffset = 20;
            dataLength = 1;
            globalOffset = 80;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_MODE_E;
            break;
        case CPSS_IP_PROTOCOL_IPV4_E:
            bitOffset = 19;
            dataLength = 1;
            globalOffset = 79;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_MODE_E;
            break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            bitOffset = 19;
            dataLength = 2;
            hwData = hwData | (hwData << 1);
            globalOffset = 79;
            fieldName = LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_MODE_E;/*2 fields from this one */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,  /* vid    */
                                           0,               /* word 0 */
                                           bitOffset,       /* offset */
                                           dataLength,
                                           hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the relevant field(s)*/
        return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        fieldName, /* field name */
                                        dataLength,/* indication for 1 or 2 fields */
                                        hwData);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               globalOffset,
                                               dataLength,
                                               hwData);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanIpmBridgingModeSet
*
* DESCRIPTION:
*       Sets the IPM bridging mode of Vlan. Relevant when IPM Bridging enabled.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device id
*       vlanId      - Vlan Id
*       ipVer       - IPv4 or Ipv6
*       ipmMode    - IPM bridging mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpmBridgingModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  CPSS_BRG_IPM_MODE_ENT       ipmMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpmBridgingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, ipVer, ipmMode));

    rc = internal_cpssDxChBrgVlanIpmBridgingModeSet(devNum, vlanId, ipVer, ipmMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, ipVer, ipmMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIngressMirrorEnable
*
* DESCRIPTION:
*       Enable/Disable ingress mirroring of packets to the Ingress Analyzer
*       port for packets assigned the given VLAN-ID.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device id
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - Ingress mirrored traffic assigned to
*                          this Vlan to the analyzer port
*                 GT_FALSE - Don't ingress mirrored traffic assigned
*                          to this Vlan to the analyzer port
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIngressMirrorEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_U32      hwData;   /* data to write to Hw */
    GT_STATUS   rc;      /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        hwData = BOOL2BIT_MAC(enable);

        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,
                                         0,
                                         16,
                                         1,
                                         hwData);
    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if(enable == GT_FALSE) /* ignore index and set index 0 */
            {
                hwData = 0;
            }
            else
            {
                if(index >  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }
                /* the index in the API is 0 based , but in HW it is 1 based
                   because in HW 0 means 'no mirror' */
                hwData = index + 1;
            }

            /* set the <MIRROR_TO_INGRESS_ANALYZER>*/
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                            vlanId,/* vid */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MIRROR_TO_INGRESS_ANALYZER_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            hwData);
        }
        else
        {
            hwData = BOOL2BIT_MAC(enable);

            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                 PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                                 (GT_U32)vlanId,
                                                 PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                 81,
                                                 1,
                                                 hwData);
        }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIngressMirrorEnable
*
* DESCRIPTION:
*       Enable/Disable ingress mirroring of packets to the Ingress Analyzer
*       port for packets assigned the given VLAN-ID.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device id
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - Ingress mirrored traffic assigned to
*                          this Vlan to the analyzer port
*                 GT_FALSE - Don't ingress mirrored traffic assigned
*                          to this Vlan to the analyzer port
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIngressMirrorEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIngressMirrorEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable, index));

    rc = internal_cpssDxChBrgVlanIngressMirrorEnable(devNum, vlanId, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortAccFrameTypeSet
*
* DESCRIPTION:
*       Set port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       frameType -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortAccFrameTypeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;    /* register address */
    GT_U32      hwData;     /* data to write to register */
    GT_U32      portGroupId;/* the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    switch (frameType)
    {
        case CPSS_PORT_ACCEPT_FRAME_ALL_E:
            hwData = 0;
            break;
        case CPSS_PORT_ACCEPT_FRAME_TAGGED_E:
            hwData = 1;
            break;
        case CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E:
            hwData = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <ACCEPT_FRAME_TYPE>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_ACCEPT_FRAME_TYPE_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

        /* get address of Protocols Encapsulation Register */
        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_1_REGADDR_PTR(devNum,portNum);

        /* Set port access frame type. */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 21, 2, hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortAccFrameTypeSet
*
* DESCRIPTION:
*       Set port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       frameType -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortAccFrameTypeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortAccFrameTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, frameType));

    rc = internal_cpssDxChBrgVlanPortAccFrameTypeSet(devNum, portNum, frameType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, frameType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortAccFrameTypeGet
*
* DESCRIPTION:
*       Get port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       frameTypePtr -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortAccFrameTypeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     *frameTypePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      hwData;    /* data to write to register */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS   rc;     /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(frameTypePtr);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <ACCEPT_FRAME_TYPE>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_ACCEPT_FRAME_TYPE_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwData);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

        /* get address of Protocols Encapsulation Register */
        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_1_REGADDR_PTR(devNum,portNum);

        /* Set port access frame type. */
        rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 21, 2, &hwData);
    }

    if(rc != GT_OK)
        return rc;

    switch (hwData)
    {
        case 0:
            *frameTypePtr = CPSS_PORT_ACCEPT_FRAME_ALL_E;
            break;
        case 1:
            *frameTypePtr = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;
            break;
        case 2:
            *frameTypePtr = CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgVlanPortAccFrameTypeGet
*
* DESCRIPTION:
*       Get port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       frameTypePtr -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortAccFrameTypeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     *frameTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortAccFrameTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, frameTypePtr));

    rc = internal_cpssDxChBrgVlanPortAccFrameTypeGet(devNum, portNum, frameTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, frameTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanMruProfileIdxSet
*
* DESCRIPTION:
*     Set Maximum Receive Unit MRU profile index for a VLAN.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     Value of MRU for profile is set by cpssDxChBrgVlanMruProfileValueSet.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanMruProfileIdxSet
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_U32    mruIndex
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(mruIndex);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Set Maximum Receive Unit MRU profile index for the VLAN */
        return prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,/* vid */
                                         3,/* start at word 3 */
                                         2,/* start at bit 2*/
                                         3, /* 3 bits */
                                         mruIndex);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <MRU_INDEX>*/
        return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MRU_INDEX_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        mruIndex);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             99,
                                             3, /* 3 bits */
                                             mruIndex);
    }

}

/*******************************************************************************
* cpssDxChBrgVlanMruProfileIdxSet
*
* DESCRIPTION:
*     Set Maximum Receive Unit MRU profile index for a VLAN.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     Value of MRU for profile is set by cpssDxChBrgVlanMruProfileValueSet.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMruProfileIdxSet
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_U32    mruIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanMruProfileIdxSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, mruIndex));

    rc = internal_cpssDxChBrgVlanMruProfileIdxSet(devNum, vlanId, mruIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, mruIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanMruProfileValueSet
*
* DESCRIPTION:
*     Set MRU value for a VLAN MRU profile.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     cpssDxChBrgVlanMruProfileIdxSet set index of profile for a specific VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
*       mruValue - MRU value in bytes
*                  (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 0..0xFFFF;
*                                      Bobcat2, Caelum, Bobcat3  0..0x3FFF)
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanMruProfileValueSet
(
    IN GT_U8     devNum,
    IN GT_U32    mruIndex,
    IN GT_U32    mruValue
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldOffset;    /* field offset of the data in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(mruIndex);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if(mruValue >= BIT_14)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    VLANMRUProfilesConfig.VLANMRUProfileConfig[mruIndex / 2];
    }
    else
    {
        if(mruValue >= BIT_16)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.vlanMruProfilesConfigReg;

        regAddr = regAddr + (mruIndex / 2) * 0x4;
    }
    fieldOffset = (mruIndex % 2) * 16;

    /* Set Maximum Receive Unit MRU profile value for the index */
    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset,
                                     16, mruValue);

}

/*******************************************************************************
* cpssDxChBrgVlanMruProfileValueSet
*
* DESCRIPTION:
*     Set MRU value for a VLAN MRU profile.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     cpssDxChBrgVlanMruProfileIdxSet set index of profile for a specific VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
*       mruValue - MRU value in bytes
*                  (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 0..0xFFFF;
*                                      Bobcat2, Caelum, Bobcat3  0..0x3FFF)
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMruProfileValueSet
(
    IN GT_U8     devNum,
    IN GT_U32    mruIndex,
    IN GT_U32    mruValue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanMruProfileValueSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mruIndex, mruValue));

    rc = internal_cpssDxChBrgVlanMruProfileValueSet(devNum, mruIndex, mruValue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mruIndex, mruValue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanMruProfileValueGet
*
* DESCRIPTION:
*     Get MRU value for a VLAN MRU profile.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
* OUTPUTS:
*       mruValuePtr - (pointer to) MRU value in bytes
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanMruProfileValueGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    mruIndex,
    OUT GT_U32    *mruValuePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldOffset;    /* field offset of the data in the register */
    GT_U32      value;          /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(mruIndex);
    CPSS_NULL_PTR_CHECK_MAC(mruValuePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    VLANMRUProfilesConfig.VLANMRUProfileConfig[mruIndex / 2];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.vlanMruProfilesConfigReg;

        regAddr = regAddr + (mruIndex / 2) * 0x4;
    }

    fieldOffset = (mruIndex % 2) * 16;

    /* Set Maximum Receive Unit MRU profile value for the index */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset,
                                     16, &value);
    if(GT_OK != rc)
    {
        return rc;
    }

    *mruValuePtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanMruProfileValueGet
*
* DESCRIPTION:
*     Get MRU value for a VLAN MRU profile.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       mruIndex - MRU profile index (APPLICABLE RANGES: 0..7)
* OUTPUTS:
*       mruValuePtr - (pointer to) MRU value in bytes
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMruProfileValueGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    mruIndex,
    OUT GT_U32    *mruValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanMruProfileValueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mruIndex, mruValuePtr));

    rc = internal_cpssDxChBrgVlanMruProfileValueGet(devNum, mruIndex, mruValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mruIndex, mruValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanNaToCpuEnable
*
* DESCRIPTION:
*     Enable/Disable New Address (NA) Message Sending to CPU per VLAN  .
*     To send NA to CPU both VLAN and port must be set to send NA to CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       enable   - GT_TRUE  - enable New Address Message Sending to CPU
*                  GT_FALSE - disable New Address Message Sending to CPU
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanNaToCpuEnable
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_BOOL   enable
)
{
    GT_U32      hwData; /* data to write to register */
    GT_STATUS   rc;     /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    hwData = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_VLAN_E,
                                         (GT_U32)vlanId,
                                         3, /* word 3 */
                                         1, /* start at bit 1 */
                                         1, /* 1 bit */
                                         hwData);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <NA_MSG_TO_CPU_EN>*/
        return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             98,
                                             1, /* 1 bit */
                                             hwData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanNaToCpuEnable
*
* DESCRIPTION:
*     Enable/Disable New Address (NA) Message Sending to CPU per VLAN  .
*     To send NA to CPU both VLAN and port must be set to send NA to CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       enable   - GT_TRUE  - enable New Address Message Sending to CPU
*                  GT_FALSE - disable New Address Message Sending to CPU
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanNaToCpuEnable
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanNaToCpuEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable));

    rc = internal_cpssDxChBrgVlanNaToCpuEnable(devNum, vlanId, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanVrfIdSet
*
* DESCRIPTION:
*       Sets vlan Virtual Router ID
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan id.
*       vrfId           - the virtual router id (APPLICABLE RANGES: 0..4095).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_OUT_OF_RANGE          - when vrfId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanVrfIdSet
(
    IN GT_U8                   devNum,
    IN GT_U16                  vlanId,
    IN GT_U32                  vrfId
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    /* check vrf ID */
    if (vrfId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                          PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
                                          (GT_U32)vlanId,
                                          0, /* word 0 */
                                          0,
                                          12, /* 12 bits */
                                          vrfId);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <VRF_ID>*/
        return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VRF_ID_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        vrfId);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                              (GT_U32)vlanId,
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                              116,
                                              12, /* 12 bits */
                                              vrfId);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanVrfIdSet
*
* DESCRIPTION:
*       Sets vlan Virtual Router ID
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan id.
*       vrfId           - the virtual router id (APPLICABLE RANGES: 0..4095).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_OUT_OF_RANGE          - when vrfId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanVrfIdSet
(
    IN GT_U8                   devNum,
    IN GT_U16                  vlanId,
    IN GT_U32                  vrfId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanVrfIdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, vrfId));

    rc = internal_cpssDxChBrgVlanVrfIdSet(devNum, vlanId, vrfId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, vrfId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanBridgingModeSet
*
* DESCRIPTION:
*     Set bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       brgMode  - bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanBridgingModeSet
(
    IN GT_U8                devNum,
    IN CPSS_BRG_MODE_ENT    brgMode
)
{
    GT_U32      hwData;     /* data to write to register */
    GT_U32      regAddr;    /* hw register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    switch (brgMode)
    {
        case CPSS_BRG_MODE_802_1Q_E:
            hwData = 0;
            break;
        case CPSS_BRG_MODE_802_1D_E:
            hwData = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    /* get address of Global Control Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;

    /* set Vlan - unaware mode in Global Control Register */
    return prvCpssHwPpSetRegField(devNum, regAddr, 3, 1, hwData);
}

/*******************************************************************************
* cpssDxChBrgVlanBridgingModeSet
*
* DESCRIPTION:
*     Set bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       brgMode  - bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBridgingModeSet
(
    IN GT_U8                devNum,
    IN CPSS_BRG_MODE_ENT    brgMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanBridgingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, brgMode));

    rc = internal_cpssDxChBrgVlanBridgingModeSet(devNum, brgMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, brgMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanBridgingModeGet
*
* DESCRIPTION:
*     Get bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
* OUTPUTS:
*       brgModePtr  - (pointer to) bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanBridgingModeGet
(
    IN  GT_U8                devNum,
    OUT CPSS_BRG_MODE_ENT    *brgModePtr
)
{
    GT_U32      rc;
    GT_U32      hwData;     /* data to write to register */
    GT_U32      regAddr;    /* hw register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(brgModePtr);


    /* get address of Global Control Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;

    /* set Vlan - unaware mode in Global Control Register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 3, 1, &hwData);
    if(GT_OK != rc)
    {
        return rc;
    }

    *brgModePtr = (hwData) ? CPSS_BRG_MODE_802_1D_E : CPSS_BRG_MODE_802_1Q_E;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanBridgingModeGet
*
* DESCRIPTION:
*     Get bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
* OUTPUTS:
*       brgModePtr  - (pointer to) bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBridgingModeGet
(
    IN  GT_U8                devNum,
    OUT CPSS_BRG_MODE_ENT    *brgModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanBridgingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, brgModePtr));

    rc = internal_cpssDxChBrgVlanBridgingModeGet(devNum, brgModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, brgModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortTranslationEnableSet
*
* DESCRIPTION:
*       Enable/Disable Vlan Translation per ingress or egress port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - port number or CPU port number for egress direction.
*                         In eArch devices portNum is default ePort for ingress direction.
*       direction       - ingress or egress
*       enable          - enable/disable ingress/egress Vlan Translation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortTranslationEnableSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_NUM                        portNum,
    IN CPSS_DIRECTION_ENT                 direction,
    IN CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT enable
)
{

    GT_U32      regAddr;
    GT_U32      hwValue;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc;
    GT_U32      portGroupId;/* the port group Id - support multi-port-groups device */
    GT_U32       localPort;  /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if (GT_TRUE != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.vlanTranslationSupported)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }


    switch (enable)
    {
        case CPSS_DXCH_BRG_VLAN_TRANSLATION_DISABLE_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_BRG_VLAN_TRANSLATION_VID0_E:
            hwValue = 1;
            break;
        case CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_E:
            hwValue = 2;
            break;
        case CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_VID1_E:
            hwValue = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (hwValue > 1 && (direction != CPSS_DIRECTION_EGRESS_E ||
                        !PRV_CPSS_SIP_5_10_CHECK_MAC(devNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

                /* write to pre-tti-lookup-ingress-eport table */
                rc = prvCpssDxChWriteTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                                portNum,
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_VLAN_TRANSLATION_E, /* field name */
                                                PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                hwValue);
            }
            else
            {
                PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

                if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
                {
                    /* xCat and above */
                    startWord = 0;
                    startBit  = 31;
                }
                else
                {
                    startWord = 0;
                    startBit  = 15;
                }

                rc = prvCpssDxChWriteTableEntryField(devNum,
                                                     PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                                     (GT_U32)portNum,
                                                     startWord,
                                                     startBit,
                                                     1, /* 1 bit */
                                                     hwValue);
            }
            break;

        case CPSS_DIRECTION_EGRESS_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
                rc = prvCpssDxChWriteTableEntryField(devNum,
                                                    PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                                   portNum,
                                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                    LION3_HA_EPORT_TABLE_1_FIELDS_VLAN_TRANS_EN_E, /* field name */
                                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                   hwValue);

            }
            else
            {
                PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

                /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

                /* get the start bit 0..63 (in the bmp of registers) */
                startBit = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                         PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                         localPort;

                /* get address of Header Alteration Vlan Translation Enable register */
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                        haVlanTransEnReg[OFFSET_TO_WORD_MAC(startBit)];

                rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, OFFSET_TO_BIT_MAC(startBit), 1, hwValue);
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortTranslationEnableSet
*
* DESCRIPTION:
*       Enable/Disable Vlan Translation per ingress or egress port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - port number or CPU port number for egress direction.
*                         In eArch devices portNum is default ePort for ingress direction.
*       direction       - ingress or egress
*       enable          - enable/disable ingress/egress Vlan Translation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortTranslationEnableSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_NUM                        portNum,
    IN CPSS_DIRECTION_ENT                 direction,
    IN CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortTranslationEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, enable));

    rc = internal_cpssDxChBrgVlanPortTranslationEnableSet(devNum, portNum, direction, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortTranslationEnableGet
*
* DESCRIPTION:
*       Get the status of Vlan Translation (Enable/Disable) per ingress or
*       egress port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       direction       - ingress or egress
*       portNum         - port number or CPU port number for egress direction.
*                         In eArch devices portNum is default ePort for ingress direction.
*
* OUTPUTS:
*       enablePtr       - (pointer to) ingress/egress Vlan Translation status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortTranslationEnableGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    IN  CPSS_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      hwValue=0;
    GT_U32      startWord;  /* the table word at which the field starts */
    GT_U32      startBit;   /* the word's bit at which the field starts */
    GT_STATUS   rc=GT_OK;
    GT_U32      portGroupId;/* the port group Id - support multi-port-groups device */
    GT_U32       localPort;  /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (GT_TRUE != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.vlanTranslationSupported)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
    case CPSS_DIRECTION_INGRESS_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

            /* read the value from pre-tti-lookup-ingress-eport table  */
            rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_VLAN_TRANSLATION_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &hwValue);
        }
        else
        {
            PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat and above */
                startWord = 0;
                startBit  = 31;
            }
            else
            {
                startWord = 0;
                startBit  = 15;
            }

            /* read the value from port-vlan-qos table  */
            rc = prvCpssDxChReadTableEntryField(devNum,
                                                PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                                (GT_U32)portNum,
                                                startWord,
                                                startBit,
                                                1, /* 1 bit */
                                                &hwValue);
        }
        break;

    case CPSS_DIRECTION_EGRESS_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
            rc = prvCpssDxChReadTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                               portNum,
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                LION3_HA_EPORT_TABLE_1_FIELDS_VLAN_TRANS_EN_E, /* field name */
                                                PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                               &hwValue);
        }
        else
        {
            PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

            /* get the start bit 0..63 (in the bmp of registers) */
            startBit = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                     PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                     localPort;

            /* get address of Header Alteration Vlan Translation Enable register */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    haVlanTransEnReg[OFFSET_TO_WORD_MAC(startBit)];

            rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, OFFSET_TO_BIT_MAC(startBit), 1, &hwValue);
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (hwValue > 1 && (direction != CPSS_DIRECTION_EGRESS_E ||
                        !PRV_CPSS_SIP_5_10_CHECK_MAC(devNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    switch (hwValue)
    {
        case 0:
            *enablePtr = CPSS_DXCH_BRG_VLAN_TRANSLATION_DISABLE_E;
            break;
        case 1:
            *enablePtr = CPSS_DXCH_BRG_VLAN_TRANSLATION_VID0_E;
            break;
        case 2:
            *enablePtr = CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_E;
            break;
        case 3:
            *enablePtr = CPSS_DXCH_BRG_VLAN_TRANSLATION_SID_VID0_VID1_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortTranslationEnableGet
*
* DESCRIPTION:
*       Get the status of Vlan Translation (Enable/Disable) per ingress or
*       egress port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       direction       - ingress or egress
*       portNum         - port number or CPU port number for egress direction.
*                         In eArch devices portNum is default ePort for ingress direction.
*
* OUTPUTS:
*       enablePtr       - (pointer to) ingress/egress Vlan Translation status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortTranslationEnableGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    IN  CPSS_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_BRG_VLAN_TRANSLATION_ENT *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortTranslationEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, enablePtr));

    rc = internal_cpssDxChBrgVlanPortTranslationEnableGet(devNum, portNum, direction, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTranslationEntryWrite
*
* DESCRIPTION:
*       Write an entry into Vlan Translation Table Ingress or Egress
*       This mapping avoids collisions between a VLAN-ID
*       used for a logical interface and a VLAN-ID used by the local network.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Translation Table
*                         (APPLICABLE RANGES: 0..4095).
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*       direction       - ingress or egress
*       transVlanId     - Translated Vlan ID, use as value in the
*                         Translation Table (APPLICABLE RANGES: 0..4095).
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_OUT_OF_RANGE          - when transVlanId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTranslationEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_U16                       transVlanId
)
{
    PRV_CPSS_DXCH_TABLE_ENT     tableType;
    GT_U32                      numOfBits = 12;/*number of bit to write*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if (GT_TRUE != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.vlanTranslationSupported)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);
            PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum, transVlanId);

            tableType = PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E;

            numOfBits = PRV_CPSS_DXCH_PP_HW_NUM_BITS_VID_MAC(devNum);

            break;

        case CPSS_DIRECTION_EGRESS_E:
            PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(transVlanId);

            tableType = PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    return prvCpssDxChWriteTableEntryField(devNum,
                                           tableType,
                                           vlanId,
                                           0,   /* word */
                                           0,   /* start bit */
                                           numOfBits,
                                           transVlanId);
}

/*******************************************************************************
* cpssDxChBrgVlanTranslationEntryWrite
*
* DESCRIPTION:
*       Write an entry into Vlan Translation Table Ingress or Egress
*       This mapping avoids collisions between a VLAN-ID
*       used for a logical interface and a VLAN-ID used by the local network.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Translation Table
*                         (APPLICABLE RANGES: 0..4095).
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*       direction       - ingress or egress
*       transVlanId     - Translated Vlan ID, use as value in the
*                         Translation Table (APPLICABLE RANGES: 0..4095).
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_OUT_OF_RANGE          - when transVlanId is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTranslationEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_U16                       transVlanId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTranslationEntryWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, direction, transVlanId));

    rc = internal_cpssDxChBrgVlanTranslationEntryWrite(devNum, vlanId, direction, transVlanId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, direction, transVlanId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTranslationEntryRead
*
* DESCRIPTION:
*       Read an entry from Vlan Translation Table, Ingress or Egress.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Translation Table, 0..4095.
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*       direction       - ingress or egress
*
* OUTPUTS:
*       transVlanIdPtr  - (pointer to) Translated Vlan ID, used as value in the
*                         Translation Table.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTranslationEntryRead
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    OUT GT_U16                      *transVlanIdPtr
)
{
    PRV_CPSS_DXCH_TABLE_ENT     tableType;  /* table type               */
    GT_U32                      hwValue;    /* value to read from hw    */
    GT_STATUS                   rc;         /* return code              */
    GT_U32                      numOfBits = 12;/*number of bit to read*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(transVlanIdPtr);

    if (GT_TRUE != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.vlanTranslationSupported)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

            tableType = PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E;

            numOfBits = PRV_CPSS_DXCH_PP_HW_NUM_BITS_VID_MAC(devNum);
            break;

        case CPSS_DIRECTION_EGRESS_E:
            PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

            tableType = PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChReadTableEntryField(devNum,
                                        tableType,
                                        vlanId,
                                        0,   /* word */
                                        0,   /* start bit */
                                        numOfBits,
                                        &hwValue);

    *transVlanIdPtr = (GT_U16)hwValue;

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanTranslationEntryRead
*
* DESCRIPTION:
*       Read an entry from Vlan Translation Table, Ingress or Egress.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Translation Table, 0..4095.
*                         for ingress : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..4095)
*                         for egress  : (APPLICABLE RANGES: Bobcat2; Caelum; Bobcat3 0..8191)
*       direction       - ingress or egress
*
* OUTPUTS:
*       transVlanIdPtr  - (pointer to) Translated Vlan ID, used as value in the
*                         Translation Table.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTranslationEntryRead
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    OUT GT_U16                      *transVlanIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTranslationEntryRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, direction, transVlanIdPtr));

    rc = internal_cpssDxChBrgVlanTranslationEntryRead(devNum, vlanId, direction, transVlanIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, direction, transVlanIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanValidCheckEnableSet
*
* DESCRIPTION:
*       Enable/disable check of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - physical device number
*       enable    - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanValidCheckEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_U32 fieldOffset; /* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig0;
        fieldOffset = 2;
    }
    else
    {
        /* Bridge Global Configuration Register0, field <VLAN Valid Check Enable> */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bridgeRegs.bridgeGlobalConfigRegArray[0];
        fieldOffset = 4;
    }
    value = BOOL2BIT_MAC(enable);

    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);

}

/*******************************************************************************
* cpssDxChBrgVlanValidCheckEnableSet
*
* DESCRIPTION:
*       Enable/disable check of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - physical device number
*       enable    - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanValidCheckEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanValidCheckEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgVlanValidCheckEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanValidCheckEnableGet
*
* DESCRIPTION:
*       Get check status  of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - pointer to check status  of "Valid" field in the VLAN entry.
*                 - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to read from hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanValidCheckEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    value;       /* value to read from register */
    GT_STATUS rc;          /* return status */
    GT_U32    fieldOffset; /* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig0;
        fieldOffset = 2;
    }
    else
    {
        /* Bridge Global Configuration Register0, field <VLAN Valid Check Enable> */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bridgeRegs.bridgeGlobalConfigRegArray[0];
        fieldOffset = 4;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanValidCheckEnableGet
*
* DESCRIPTION:
*       Get check status  of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - pointer to check status  of "Valid" field in the VLAN entry.
*                 - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to read from hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanValidCheckEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanValidCheckEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgVlanValidCheckEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTpidEntrySet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       tableType   - etherType table selection
*       entryIndex  - entry index for TPID table (APPLICABLE RANGES: 0..7)
*       etherType   - Tag Protocol ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, tableType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E, TPID0 use entry indexes (0,2,4,6)
*       CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E, TPID1 use entry indexes (1,3,5,7)
*
*   Egress Logical Port TPID table each entry defines a Tag0 TPID and Tag1 TPID;
*   4 entries of {16b Tag0 TPID, 16b Tag1 TPID})
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTpidEntrySet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_ETHERTYPE_TABLE_ENT   tableType,
    IN  GT_U32                          entryIndex,
    IN  GT_U16                          etherType
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    startBit;/*start bit of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* table index validity check */
    if (entryIndex > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    startBit = 16 * (entryIndex % 2);

    /* choose the appropriate table */
    switch (tableType)
    {
        case CPSS_DXCH_ETHERTYPE_TABLE_INGRESS_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                                    TTI.VLANAssignment.ingrTPIDConfig[entryIndex / 2];
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeIngressVlanEtherTypeTable[entryIndex / 2];
            }
            break;

        case CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).TPIDConfigReg[entryIndex];
                startBit = 0;/*each TPID in different register (because there is <Tag Type> also in each register)*/
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        haRegs.bridgeEgressVlanEtherTypeTable[entryIndex / 2];
            }
            break;

         case CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.passengerEgressVlanEtherTypeTable[entryIndex / 2];
            break;

        case CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_TS_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.tunnelStartEgressVlanEtherTypeTable[entryIndex / 2];
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, startBit , 16, etherType);
}

/*******************************************************************************
* cpssDxChBrgVlanTpidEntrySet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       tableType   - etherType table selection
*       entryIndex  - entry index for TPID table (APPLICABLE RANGES: 0..7)
*       etherType   - Tag Protocol ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, tableType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E, TPID0 use entry indexes (0,2,4,6)
*       CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E, TPID1 use entry indexes (1,3,5,7)
*
*   Egress Logical Port TPID table each entry defines a Tag0 TPID and Tag1 TPID;
*   4 entries of {16b Tag0 TPID, 16b Tag1 TPID})
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidEntrySet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_ETHERTYPE_TABLE_ENT   tableType,
    IN  GT_U32                          entryIndex,
    IN  GT_U16                          etherType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTpidEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tableType, entryIndex, etherType));

    rc = internal_cpssDxChBrgVlanTpidEntrySet(devNum, tableType, entryIndex, etherType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tableType, entryIndex, etherType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTpidEntryGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       tableType   - etherType table selection
*       entryIndex  - entry index for TPID table (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Tag Protocol ID value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, tableType
*       GT_BAD_PTR               - etherTypePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTpidEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_ETHERTYPE_TABLE_ENT   tableType,
    IN  GT_U32                          entryIndex,
    OUT GT_U16                         *etherTypePtr
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    regValue; /* register value */
    GT_STATUS rc;       /* returned status */
    GT_U32    startBit;/*start bit of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    /* table index validity check */
    if (entryIndex > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    startBit = 16 * (entryIndex % 2);

    /* choose the appropriate table */
    switch (tableType)
    {
         case CPSS_DXCH_ETHERTYPE_TABLE_INGRESS_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                                    TTI.VLANAssignment.ingrTPIDConfig[entryIndex / 2];
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.bridgeIngressVlanEtherTypeTable[entryIndex / 2];
            }
            break;

         case CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).TPIDConfigReg[entryIndex];
                startBit = 0;/*each TPID in different register (because there is <Tag Type> also in each register)*/
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        haRegs.bridgeEgressVlanEtherTypeTable[entryIndex / 2];
            }
            break;

        case CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.passengerEgressVlanEtherTypeTable[entryIndex / 2];
            break;

        case CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_TS_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    haRegs.tunnelStartEgressVlanEtherTypeTable[entryIndex / 2];
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, startBit, 16, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *etherTypePtr = (GT_U16) regValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanTpidEntryGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       tableType   - etherType table selection
*       entryIndex  - entry index for TPID table (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Tag Protocol ID value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, tableType
*       GT_BAD_PTR               - etherTypePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_ETHERTYPE_TABLE_ENT   tableType,
    IN  GT_U32                          entryIndex,
    OUT GT_U16                         *etherTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTpidEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tableType, entryIndex, etherTypePtr));

    rc = internal_cpssDxChBrgVlanTpidEntryGet(devNum, tableType, entryIndex, etherTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tableType, entryIndex, etherTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIngressTpidProfileSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) per profile .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - TPID profile. (APPLICABLE RANGES: 0..7)
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent 8 entries at the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, profile, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For the ingress direction multiple TPID may be used to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*       2. bind 'port' to 'profile' using function cpssDxChBrgVlanPortIngressTpidProfileSet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIngressTpidProfileSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               profile,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
{
    GT_STATUS   rc;      /*return code*/
    GT_U32      regAddr; /* register address */
    GT_U32      bit;/* start bit */
    GT_U32      tag;/* tag 0 or 1*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if(profile >= 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            tag = 0;
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            tag = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (tpidBmp >= BIT_8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* each tag hold  2 registers , with 4 profiles in register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                        TTI.VLANAssignment.ingrTagProfileTPIDSelect[tag][profile/4];

    bit = (profile % 4) * 8;/* 8 bits per profile */

    rc = prvCpssHwPpSetRegField(devNum, regAddr, bit, 8, tpidBmp);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIngressTpidProfileSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) per profile .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - TPID profile. (APPLICABLE RANGES: 0..7)
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent 8 entries at the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, profile, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For the ingress direction multiple TPID may be used to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*       2. bind 'port' to 'profile' using function cpssDxChBrgVlanPortIngressTpidProfileSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIngressTpidProfileSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               profile,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIngressTpidProfileSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profile, ethMode, tpidBmp));

    rc = internal_cpssDxChBrgVlanIngressTpidProfileSet(devNum, profile, ethMode, tpidBmp);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profile, ethMode, tpidBmp));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIngressTpidProfileGet
*
* DESCRIPTION:
*       Function gts bitmap of TPID (Tag Protocol ID) per profile .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - TPID profile. (APPLICABLE RANGES: 0..7)
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidBmpPtr    - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, profile, ethMode
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIngressTpidProfileGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               profile,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
)
{
    GT_STATUS   rc;      /*return code*/
    GT_U32      regAddr; /* register address */
    GT_U32      bit;/* start bit */
    GT_U32      tag;/* tag 0 or 1*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(tpidBmpPtr);

    if(profile >= 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            tag = 0;
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            tag = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* each tag hold  2 registers , with 4 profiles in register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                        TTI.VLANAssignment.ingrTagProfileTPIDSelect[tag][profile/4];

    bit = (profile % 4) * 8;/* 8 bits per profile */

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bit, 8, tpidBmpPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIngressTpidProfileGet
*
* DESCRIPTION:
*       Function gts bitmap of TPID (Tag Protocol ID) per profile .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - TPID profile. (APPLICABLE RANGES: 0..7)
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidBmpPtr    - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, profile, ethMode
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIngressTpidProfileGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               profile,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIngressTpidProfileGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profile, ethMode, tpidBmpPtr));

    rc = internal_cpssDxChBrgVlanIngressTpidProfileGet(devNum, profile, ethMode, tpidBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profile, ethMode, tpidBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTpidTagTypeSet
*
* DESCRIPTION:
*       Set TPID tag type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       index       - TPID index (APPLICABLE RANGES: 0..7)
*       type        - the TPID type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong input parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTpidTagTypeSet
(
    IN GT_U8                        devNum,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_U32                       index,
    IN CPSS_BRG_TPID_SIZE_TYPE_ENT  type
)
{
    GT_U32 regAddr;     /* the register address */
    GT_U32 offset;      /* the offset in the register */
    GT_U32 value;       /* the value to write */
    GT_U32 length;      /* number of bits to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (index > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).VLANAssignment.ingrTPIDTagType;
            if(0 == PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                offset = index;
                length = 1;
            }
            else
            {
                offset = 2*index;
                length = 2;
            }
            break;

        case CPSS_DIRECTION_EGRESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).TPIDConfigReg[index];
            offset = 16;
            length = 2;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (type)
    {
        case CPSS_BRG_TPID_SIZE_TYPE_4_BYTES_E:
            value = 0;
            break;

        case CPSS_BRG_TPID_SIZE_TYPE_8_BYTES_E:
            value = 1;
            break;

        case CPSS_BRG_TPID_SIZE_TYPE_6_BYTES_E:
            if(0 == PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                /* the device not supports the value */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            value = 2;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, offset, length, value);
}

/*******************************************************************************
* cpssDxChBrgVlanTpidTagTypeSet
*
* DESCRIPTION:
*       Set TPID tag type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       index       - TPID index (APPLICABLE RANGES: 0..7)
*       type        - the TPID type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong input parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidTagTypeSet
(
    IN GT_U8                        devNum,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_U32                       index,
    IN CPSS_BRG_TPID_SIZE_TYPE_ENT  type
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTpidTagTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, index, type));

    rc = internal_cpssDxChBrgVlanTpidTagTypeSet(devNum, direction, index, type);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, index, type));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTpidTagTypeGet
*
* DESCRIPTION:
*       Get TPID tag type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       index       - TPID index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       typePtr     - (pointer to) the TPID type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong input parameter
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTpidTagTypeGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DIRECTION_ENT          direction,
    IN  GT_U32                      index,
    OUT CPSS_BRG_TPID_SIZE_TYPE_ENT *typePtr
)
{
    GT_U32      regAddr, offset, value, length;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(typePtr);

    if (index > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).VLANAssignment.ingrTPIDTagType;
            if(0 == PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                offset = index;
                length = 1;
            }
            else
            {
                offset = 2*index;
                length = 2;
            }
            break;

        case CPSS_DIRECTION_EGRESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).TPIDConfigReg[index];
            offset = 16;
            length = 2;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, length, &value);
    if (rc == GT_OK)
    {
        switch (value)
        {
            case 0:
                *typePtr = CPSS_BRG_TPID_SIZE_TYPE_4_BYTES_E;
                break;

            case 1:
                *typePtr = CPSS_BRG_TPID_SIZE_TYPE_8_BYTES_E;
                break;

            case 2:
                if(0 == PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
                {
                    /* the device not supports the value */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                }
                *typePtr = CPSS_BRG_TPID_SIZE_TYPE_6_BYTES_E;
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanTpidTagTypeGet
*
* DESCRIPTION:
*       Get TPID tag type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       index       - TPID index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       typePtr     - (pointer to) the TPID type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong input parameter
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTpidTagTypeGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DIRECTION_ENT          direction,
    IN  GT_U32                      index,
    OUT CPSS_BRG_TPID_SIZE_TYPE_ENT *typePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTpidTagTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, index, typePtr));

    rc = internal_cpssDxChBrgVlanTpidTagTypeGet(devNum, direction, index, typePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, index, typePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortIngressTpidProfileSet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) profile per ingress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number or CPU port.
*                    In eArch devices portNum is default ePort for default profile.
*       ethMode    - TAG0/TAG1 selector
*       isDefaultProfile - indication that the profile is the default profile or
*                          used for re-parse after TTI lookup:
*                            1. Parse inner Layer2 after TT
*                            2. Re-parse Layer2 after popping EVB/BPE tag
*                            3. Parsing of passenger packet
*                        GT_TRUE  - used as default profile
*                        GT_FALSE - used for re-parse after TTI lookup.
*       profile    - TPID profile. (APPLICABLE RANGE: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - profile > 7
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For setting TPID bmp per profile use function cpssDxChBrgVlanIngressTpidProfileSet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortIngressTpidProfileSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN GT_BOOL               isDefaultProfile,
    IN  GT_U32               profile
)
{
    GT_STATUS   rc;      /*return code*/
    GT_U32      fieldName_defEprot;/* the field name in default eport entry */
    GT_U32      fieldName_EportAttributes; /* the field name in eport attributes entry */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if(profile >= BIT_3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            /* field name in pre TTI  */
            fieldName_defEprot = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_INGRESS_TAG0_TPID_PROFILE_E;
            /* field name in post TTI */
            fieldName_EportAttributes = LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_TAG0_TPID_PROFILE_E;
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            /* field name in pre TTI  */
            fieldName_defEprot = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_INGRESS_TAG1_TPID_PROFILE_E;
            /* field name in post TTI */
            fieldName_EportAttributes = LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_TAG1_TPID_PROFILE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(isDefaultProfile == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
        /* write to pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,/*table type*/
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            fieldName_defEprot, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            profile);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
       /* write to post-tti-lookup-ingress-eport table */

        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,/*table type*/
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            fieldName_EportAttributes,    /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            profile);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidProfileSet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) profile per ingress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number or CPU port.
*                    In eArch devices portNum is default ePort for default profile.
*       ethMode    - TAG0/TAG1 selector
*       isDefaultProfile - indication that the profile is the default profile or
*                          used for re-parse after TTI lookup:
*                            1. Parse inner Layer2 after TT
*                            2. Re-parse Layer2 after popping EVB/BPE tag
*                            3. Parsing of passenger packet
*                        GT_TRUE  - used as default profile
*                        GT_FALSE - used for re-parse after TTI lookup.
*       profile    - TPID profile. (APPLICABLE RANGE: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - profile > 7
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For setting TPID bmp per profile use function cpssDxChBrgVlanIngressTpidProfileSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidProfileSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN GT_BOOL               isDefaultProfile,
    IN  GT_U32               profile
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortIngressTpidProfileSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ethMode, isDefaultProfile, profile));

    rc = internal_cpssDxChBrgVlanPortIngressTpidProfileSet(devNum, portNum, ethMode, isDefaultProfile, profile);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ethMode, isDefaultProfile, profile));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortIngressTpidProfileGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) profile per ingress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number or CPU port.
*                    In eArch devices portNum is default ePort for default profile.
*       ethMode    - TAG0/TAG1 selector
*       isDefaultProfile - indication that the profile is the default profile or
*                          used for re-parse after TTI lookup:
*                            1. Parse inner Layer2 after TT
*                            2. Re-parse Layer2 after popping EVB/BPE tag
*                            3. Parsing of passenger packet
*                        GT_TRUE  - used as default profile
*                        GT_FALSE - used for re-parse after TTI lookup.
*
* OUTPUTS:
*       profilePtr    - (pointer to)TPID profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortIngressTpidProfileGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN GT_BOOL               isDefaultProfile,
    OUT GT_U32               *profilePtr
)
{
    GT_STATUS   rc;      /*return code*/
    GT_U32      fieldName_defEprot;/* the field name in default eport entry */
    GT_U32      fieldName_EportAttributes; /* the field name in eport attributes entry */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(profilePtr);

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            /* field name in pre TTI  */
            fieldName_defEprot = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_INGRESS_TAG0_TPID_PROFILE_E;
            /* field name in post TTI */
            fieldName_EportAttributes = LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_TAG0_TPID_PROFILE_E;
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            /* field name in pre TTI  */
            fieldName_defEprot = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_INGRESS_TAG1_TPID_PROFILE_E;
            /* field name in post TTI */
            fieldName_EportAttributes = LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_TAG1_TPID_PROFILE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(isDefaultProfile == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
        /* read from pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,/*table type*/
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            fieldName_defEprot, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            profilePtr);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
       /* read from post-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,/*table type*/
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            fieldName_EportAttributes, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            profilePtr);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidProfileGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) profile per ingress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number or CPU port.
*                    In eArch devices portNum is default ePort for default profile.
*       ethMode    - TAG0/TAG1 selector
*       isDefaultProfile - indication that the profile is the default profile or
*                          used for re-parse after TTI lookup:
*                            1. Parse inner Layer2 after TT
*                            2. Re-parse Layer2 after popping EVB/BPE tag
*                            3. Parsing of passenger packet
*                        GT_TRUE  - used as default profile
*                        GT_FALSE - used for re-parse after TTI lookup.
*
* OUTPUTS:
*       profilePtr    - (pointer to)TPID profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidProfileGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN GT_BOOL               isDefaultProfile,
    OUT GT_U32               *profilePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortIngressTpidProfileGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ethMode, isDefaultProfile, profilePtr));

    rc = internal_cpssDxChBrgVlanPortIngressTpidProfileGet(devNum, portNum, ethMode, isDefaultProfile, profilePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ethMode, isDefaultProfile, profilePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - etherType mode selector; valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E
*       tpidBmp    - bitmap represent 8 entries at the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;    /* register address */
    GT_U32      vlanOffset;
    GT_U32       localPort;  /* local port - support multi-port-groups device */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    if (tpidBmp >= BIT_8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    switch (ethMode)
    {
         case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E:
            vlanOffset = 0 + 16 * (portNum % 2);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeIngressVlanSelect[localPort / 2];
            break;

         case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E:
            vlanOffset = 8 + 16 * (portNum % 2);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeIngressVlanSelect[localPort / 2];
            break;

         case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            vlanOffset = 0 + 16 * (portNum % 2);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttIngressTpidSelect[localPort / 2];
            break;

        case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            vlanOffset = 8 + 16 * (portNum % 2);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttIngressTpidSelect[localPort / 2];
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, vlanOffset, 8, tpidBmp);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - etherType mode selector; valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E
*       tpidBmp    - bitmap represent 8 entries at the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortIngressTpidSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ethMode, tpidBmp));

    rc = internal_cpssDxChBrgVlanPortIngressTpidSet(devNum, portNum, ethMode, tpidBmp);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ethMode, tpidBmp));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes  per
*       ingress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - etherType mode selector. valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E
*
* OUTPUTS:
*       tpidBmpPtr - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;    /* register address */
    GT_U32      vlanOffset;
    GT_U32       localPort;  /* local port - support multi-port-groups device */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(tpidBmpPtr);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

      /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    switch (ethMode)
    {
        case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E:
            vlanOffset = 0 + 16 * (portNum % 2);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeIngressVlanSelect[localPort / 2];
            break;

        case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E:
            vlanOffset = 8 + 16 * (portNum % 2);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeIngressVlanSelect[localPort / 2];
            break;

        case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            vlanOffset = 0 + 16 * (portNum % 2);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttIngressTpidSelect[localPort / 2];
            break;

        case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            vlanOffset = 8 + 16 * (portNum % 2);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttIngressTpidSelect[localPort / 2];
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, vlanOffset, 8, tpidBmpPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes  per
*       ingress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - etherType mode selector. valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_INGRESS_PASSENGER_ETHERTYPE1_E
*
* OUTPUTS:
*       tpidBmpPtr - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represents one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortIngressTpidGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ethMode, tpidBmpPtr));

    rc = internal_cpssDxChBrgVlanPortIngressTpidGet(devNum, portNum, ethMode, tpidBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ethMode, tpidBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortEgressTpidSet
*
* DESCRIPTION:
*       Function sets index of TPID (Tag protocol ID) table per egress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       ethMode        - ethertype mode selector; valid values:
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E
*       tpidEntryIndex - TPID table entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortEgressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    IN  GT_U32               tpidEntryIndex
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      vlanOffset;
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32       localPort;  /* local port - support multi-port-groups device */
    GT_U32      fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    if (tpidEntryIndex >= 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch (ethMode)
        {
            case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E:
                fieldOffset = LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG0_TPID_INDEX_E;
                break;

             case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E:
                fieldOffset = LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG1_TPID_INDEX_E;
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                           portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            fieldOffset, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                           tpidEntryIndex);
    }
    else
    {

        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            localPort = (GT_U8)PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum);
        }

        vlanOffset = 3 * (localPort % 8);

        switch (ethMode)
        {
            case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        haRegs.bridgeEgressPortTag0TpidSelect[localPort / 8];
                break;

             case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            haRegs.bridgeEgressPortTag1TpidSelect[localPort / 8];
                break;

             case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E:
                PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            haRegs.tunnelStartEgressPortTagTpidSelect[localPort / 8];
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, vlanOffset, 3, tpidEntryIndex);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanPortEgressTpidSet
*
* DESCRIPTION:
*       Function sets index of TPID (Tag protocol ID) table per egress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       ethMode        - ethertype mode selector; valid values:
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                           CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E
*       tpidEntryIndex - TPID table entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortEgressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    IN  GT_U32               tpidEntryIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortEgressTpidSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ethMode, tpidEntryIndex));

    rc = internal_cpssDxChBrgVlanPortEgressTpidSet(devNum, portNum, ethMode, tpidEntryIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ethMode, tpidEntryIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortEgressTpidGet
*
* DESCRIPTION:
*       Function gets index of TPID (Tag Protocol ID) table per egress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - ethertype mode selector; valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortEgressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    OUT GT_U32               *tpidEntryIndexPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;  /* register address */
    GT_U32    vlanOffset;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    GT_U32  hwValue;
    GT_U32  fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(tpidEntryIndexPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch (ethMode)
        {
             case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E:
                fieldOffset = LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG0_TPID_INDEX_E;
                break;

            case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E:
                fieldOffset = LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG1_TPID_INDEX_E;
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                               portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            fieldOffset, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                               &hwValue);
    }
    else
    {

        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            localPort = (GT_U8)PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum);
        }

        vlanOffset = 3 * (localPort % 8);

        switch (ethMode)
        {
             case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        haRegs.bridgeEgressPortTag0TpidSelect[localPort / 8];
                break;

             case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        haRegs.bridgeEgressPortTag1TpidSelect[localPort / 8];
                break;

             case CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E:
                PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            haRegs.tunnelStartEgressPortTagTpidSelect[localPort / 8];
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, vlanOffset, 3, &hwValue);
    }

    if(rc != GT_OK)
        return rc;

    *tpidEntryIndexPtr = hwValue;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortEgressTpidGet
*
* DESCRIPTION:
*       Function gets index of TPID (Tag Protocol ID) table per egress port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*       ethMode    - ethertype mode selector; valid values:
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE0_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ETHERTYPE1_E
*                       CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_EGRESS_TS_ETHERTYPE_E
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortEgressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_NUM          portNum,
    IN  CPSS_DXCH_BRG_VLAN_ETHERTYPE_TABLE_ENTRY_SELECT_MODE_ENT  ethMode,
    OUT GT_U32               *tpidEntryIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortEgressTpidGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ethMode, tpidEntryIndexPtr));

    rc = internal_cpssDxChBrgVlanPortEgressTpidGet(devNum, portNum, ethMode, tpidEntryIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ethMode, tpidEntryIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEgressTagTpidSelectModeSet
*
* DESCRIPTION:
*       Set Tag 0/1 TPID (Tag Protocol Identifies) Select Mode
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ethMode     - TAG0/TAG1 selector
*       mode        - 0: TPID of Tag0/1 is selected according to
*                          <Egress Tag 0/1 TPID select>
*                     1: TPID of Tag0/1 is selected according to
*                           VID0/1 to TPID select table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEgressTagTpidSelectModeSet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    IN CPSS_ETHER_MODE_ENT                          ethMode,
    IN CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT  mode
)
{
    GT_U32   hwData;
    GT_U32   bitOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_DXCH_BRG_VLAN_EGRESS_TAG_TPID_SELECT_MODE_E:
            hwData = 0;
            break;
        case CPSS_DXCH_BRG_VLAN_VID_TO_TPID_SELECT_TABLE_MODE_E:
            hwData = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            bitOffset = LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG0_TPID_E;
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            bitOffset = LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG1_TPID_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    bitOffset, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   hwData);
}

/*******************************************************************************
* cpssDxChBrgVlanEgressTagTpidSelectModeSet
*
* DESCRIPTION:
*       Set Tag 0/1 TPID (Tag Protocol Identifies) Select Mode
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ethMode     - TAG0/TAG1 selector
*       mode        - 0: TPID of Tag0/1 is selected according to
*                          <Egress Tag 0/1 TPID select>
*                     1: TPID of Tag0/1 is selected according to
*                           VID0/1 to TPID select table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressTagTpidSelectModeSet
(
    IN GT_U8                                        devNum,
    IN GT_PORT_NUM                                  portNum,
    IN CPSS_ETHER_MODE_ENT                          ethMode,
    IN CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEgressTagTpidSelectModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ethMode, mode));

    rc = internal_cpssDxChBrgVlanEgressTagTpidSelectModeSet(devNum, portNum, ethMode, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ethMode, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEgressTagTpidSelectModeGet
*
* DESCRIPTION:
*       Returns Tag 0/1 TPID (Tag Protocol Identifies) Select Mode
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ethMode     - TAG0/TAG1 selector
*
* OUTPUTS:
*       modePtr   - (pointer to)
*                     0: TPID of Tag0/1 is selected according to
*                          <Egress Tag 0/1 TPID select>
*                     1: TPID of Tag0/1 is selected according to
*                           VID0/1 to TPID select table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEgressTagTpidSelectModeGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    IN  CPSS_ETHER_MODE_ENT                                 ethMode,
    OUT CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT         *modePtr
)
{
    GT_U32      hwValue;    /* hardware value */
    GT_STATUS   rc;
    GT_U32      bitOffset;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (ethMode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            bitOffset = LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG0_TPID_E;
            break;

        case CPSS_VLAN_ETHERTYPE1_E:
            bitOffset = LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG1_TPID_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                   portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    bitOffset, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch(hwValue)
    {
        case 0:
            *modePtr  = CPSS_DXCH_BRG_VLAN_EGRESS_TAG_TPID_SELECT_MODE_E;
            break;
        case 1:
            *modePtr  = CPSS_DXCH_BRG_VLAN_VID_TO_TPID_SELECT_TABLE_MODE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

   return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanEgressTagTpidSelectModeGet
*
* DESCRIPTION:
*       Returns Tag 0/1 TPID (Tag Protocol Identifies) Select Mode
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ethMode     - TAG0/TAG1 selector
*
* OUTPUTS:
*       modePtr   - (pointer to)
*                     0: TPID of Tag0/1 is selected according to
*                          <Egress Tag 0/1 TPID select>
*                     1: TPID of Tag0/1 is selected according to
*                           VID0/1 to TPID select table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressTagTpidSelectModeGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    IN  CPSS_ETHER_MODE_ENT                                 ethMode,
    OUT CPSS_DXCH_BRG_VLAN_TAG_TPID_SELECT_MODE_ENT         *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEgressTagTpidSelectModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ethMode, modePtr));

    rc = internal_cpssDxChBrgVlanEgressTagTpidSelectModeGet(devNum, portNum, ethMode, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ethMode, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanRangeSet
*
* DESCRIPTION:
*       Function configures the valid VLAN Range.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       vidRange  - VID range for VLAN filtering (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanRangeSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vidRange
)
{
    GT_U32    regAddr;  /* register address */
    GT_STATUS rc;       /* returned status  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum, vidRange);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    VLANRangesConfig.ingrVLANRangeConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan0;
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0,
        PRV_CPSS_DXCH_PP_HW_NUM_BITS_VID_MAC(devNum), vidRange);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanRangeSet
*
* DESCRIPTION:
*       Function configures the valid VLAN Range.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       vidRange  - VID range for VLAN filtering (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRangeSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vidRange
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanRangeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vidRange));

    rc = internal_cpssDxChBrgVlanRangeSet(devNum, vidRange);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vidRange));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanRangeGet
*
* DESCRIPTION:
*       Function gets the valid VLAN Range.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vidRangePtr - (pointer to) VID range for VLAN filtering
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - vidRangePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanRangeGet
(
    IN  GT_U8   devNum,
    OUT GT_U16  *vidRangePtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value   */
    GT_STATUS rc;       /* returned status  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    CPSS_NULL_PTR_CHECK_MAC(vidRangePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    VLANRangesConfig.ingrVLANRangeConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vlan0;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 0 ,
            PRV_CPSS_DXCH_PP_HW_NUM_BITS_VID_MAC(devNum),&regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *vidRangePtr = (GT_U16)regValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanRangeGet
*
* DESCRIPTION:
*       Function gets the valid VLAN Range.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vidRangePtr - (pointer to) VID range for VLAN filtering
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - vidRangePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRangeGet
(
    IN  GT_U8   devNum,
    OUT GT_U16  *vidRangePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanRangeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vidRangePtr));

    rc = internal_cpssDxChBrgVlanRangeGet(devNum, vidRangePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vidRangePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortIsolationCmdSet
*
* DESCRIPTION:
*       Function sets port isolation command per egress VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN id
*       cmd     - port isolation command for given VID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or cmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortIsolationCmdSet
(
    IN  GT_U8                                      devNum,
    IN  GT_U16                                     vlanId,
    IN  CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT  cmd
)
{
    GT_U32  value;  /* HW value for L2/L3 Port Isolation command */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(value, cmd);

    /* write 2 bit for Port Isolation command */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChWriteTableEntryField(
            devNum,
            PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E,
            vlanId,/* vid */
            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
            LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_PORT_ISOLATION_VLAN_CMD_E, /* field name */
            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
            value);
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_VLAN_E,
                                           (GT_U32)vlanId,/* vid */
                                           3,
                                           21,
                                           2,
                                           value);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               263,
                                               2,
                                               value);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanPortIsolationCmdSet
*
* DESCRIPTION:
*       Function sets port isolation command per egress VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN id
*       cmd     - port isolation command for given VID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or cmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortIsolationCmdSet
(
    IN  GT_U8                                      devNum,
    IN  GT_U16                                     vlanId,
    IN  CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT  cmd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortIsolationCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, cmd));

    rc = internal_cpssDxChBrgVlanPortIsolationCmdSet(devNum, vlanId, cmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, cmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanLocalSwitchingEnableSet
*
* DESCRIPTION:
*       Function sets local switching of Multicast, known and unknown Unicast,
*       and Broadcast traffic per VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN id
*       trafficType - local switching traffic type
*       enable      - Enable/disable of local switching
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To enable local switching of Multicast, unknown Unicast, and
*       Broadcast traffic, both this field in the VLAN entry and the
*       egress port configuration must be enabled for Multicast local switching
*       in function cpssDxChBrgPortEgressMcastLocalEnable.
*
*       2. To enable  local switching of known Unicast traffic, both this
*       field in the VLAN entry and the ingress port configuration must
*       be enabled for Unicast local switching in function
*       cpssDxChBrgGenUcLocalSwitchingEnable.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanLocalSwitchingEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    IN  CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT  trafficType,
    IN  GT_BOOL enable
)
{
    GT_U32  wordOffset;  /* word offset */
    GT_U32  fieldOffset; /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        switch (trafficType)
        {
            case CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E:
                wordOffset = 5;
                fieldOffset = 17;
                break;

            case CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E:
                wordOffset = 3;
                fieldOffset = 20;
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

            /* write 1 bit local switching command */
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_TABLE_VLAN_E,
                                               (GT_U32)vlanId,/* vid */
                                               wordOffset,
                                               fieldOffset,
                                               1,
                                               BOOL2BIT_MAC(enable));
    }
    else
    {
        switch (trafficType)
        {
            case CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E:
                if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                {
                    /* set the <UC_LOCAL_EN>*/
                    return prvCpssDxChWriteTableEntryField(devNum,
                                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                                    vlanId,/* vid */
                                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UC_LOCAL_EN_E, /* field name */
                                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                    BOOL2BIT_MAC(enable));
                }
                else
                {
                    /* write to ingress table */
                    return prvCpssDxChWriteTableEntryField(devNum,
                                                           PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                                           (GT_U32)vlanId,
                                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                           128,
                                                           1,
                                                           BOOL2BIT_MAC(enable));
                }

            case CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E:
                if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                {
                    return prvCpssDxChWriteTableEntryField(
                        devNum,
                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E,
                        vlanId,/* vid */
                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_LOCAL_SWITHING_EN_E, /* field name */
                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                        BOOL2BIT_MAC(enable));
                }
                else
                {
                    /* write to egress table */
                    return prvCpssDxChWriteTableEntryField(devNum,
                                                           PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                                           (GT_U32)vlanId,
                                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                           262,
                                                           1,
                                                           BOOL2BIT_MAC(enable));

                }
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
}

/*******************************************************************************
* cpssDxChBrgVlanLocalSwitchingEnableSet
*
* DESCRIPTION:
*       Function sets local switching of Multicast, known and unknown Unicast,
*       and Broadcast traffic per VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN id
*       trafficType - local switching traffic type
*       enable      - Enable/disable of local switching
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To enable local switching of Multicast, unknown Unicast, and
*       Broadcast traffic, both this field in the VLAN entry and the
*       egress port configuration must be enabled for Multicast local switching
*       in function cpssDxChBrgPortEgressMcastLocalEnable.
*
*       2. To enable  local switching of known Unicast traffic, both this
*       field in the VLAN entry and the ingress port configuration must
*       be enabled for Unicast local switching in function
*       cpssDxChBrgGenUcLocalSwitchingEnable.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanLocalSwitchingEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    IN  CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT  trafficType,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanLocalSwitchingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, trafficType, enable));

    rc = internal_cpssDxChBrgVlanLocalSwitchingEnableSet(devNum, vlanId, trafficType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, trafficType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanMirrorToTxAnalyzerSet
*
* DESCRIPTION:
*       Function sets Tx Analyzer port index to given VLAN.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       vlanId                   - VLAN id
*       enable                   - Enable/disable Tx Mirroring
*                                  GT_TRUE - enable
*                                  GT_FALSE - disable
*       txAnalyzerIndex          - Tx Analyzer index
*                                  (APPLICABLE RANGES: 0..6)
*                                  relevant only if mirrToTxAnalyzerEn == GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanMirrorToTxAnalyzerSet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    IN  GT_BOOL     enable,
    IN  GT_U32      txAnalyzerIndex
)
{
    GT_U32  hwValue;  /* HW value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    if (enable == GT_FALSE)
    {
        hwValue = 0;
    }
    else
    {
        if (txAnalyzerIndex > 6)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        hwValue = (txAnalyzerIndex + 1);
    }

    return prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E,
        vlanId,/* vid */
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_EGRESS_MIRROR_TO_ANALYZER_INDEX_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        hwValue);
}

/*******************************************************************************
* cpssDxChBrgVlanMirrorToTxAnalyzerSet
*
* DESCRIPTION:
*       Function sets Tx Analyzer port index to given VLAN.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       vlanId                   - VLAN id
*       enable                   - Enable/disable Tx Mirroring
*                                  GT_TRUE - enable
*                                  GT_FALSE - disable
*       txAnalyzerIndex          - Tx Analyzer index
*                                  (APPLICABLE RANGES: 0..6)
*                                  relevant only if mirrToTxAnalyzerEn == GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMirrorToTxAnalyzerSet
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    IN  GT_BOOL     enable,
    IN  GT_U32      txAnalyzerIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanMirrorToTxAnalyzerSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable, txAnalyzerIndex));

    rc = internal_cpssDxChBrgVlanMirrorToTxAnalyzerSet(devNum, vlanId, enable, txAnalyzerIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable, txAnalyzerIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanFloodVidxModeSet
*
* DESCRIPTION:
*       Function sets Flood VIDX and Flood VIDX Mode per VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id
*       floodVidx     - VIDX value (12bit) - VIDX value applied to Broadcast,
*                       Unregistered Multicast and unknown Unicast packets,
*                       depending on the VLAN entry field <Flood VIDX Mode>.
*                       (APPLICABLE RANGES: xCat2 0..1023, 4095)
*       floodVidxMode - Flood VIDX Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or floodVidxMode
*       GT_OUT_OF_RANGE          - illegal floodVidx
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanFloodVidxModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U16                                  vlanId,
    IN  GT_U16                                  floodVidx,
    IN  CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT  floodVidxMode
)
{
    GT_U32  hwValue;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 support constant 4095 (0xFFF) and values in range 0..1023 */
        if ((floodVidx != 0xFFF) &&
            (floodVidx >= BIT_10))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if (floodVidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }

    PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(hwValue, floodVidxMode);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_VLAN_E,
                                             (GT_U32)vlanId,/* vid */
                                             3,
                                             8,
                                             12,
                                             (GT_U32)floodVidx);
        if (rc != GT_OK)
        {
            return rc;
        }

            /* write 1 bit floodVidxMode */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_TABLE_VLAN_E,
                                               (GT_U32)vlanId,/* vid */
                                               5,
                                               18,
                                               1,
                                               hwValue);
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <FLOOD_EVIDX>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_EVIDX_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        floodVidx);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* set the <FLOOD_VIDX_MODE>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        vlanId,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_VIDX_MODE_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwValue);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                             (GT_U32)vlanId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             104,
                                             12,
                                             (GT_U32)floodVidx);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write 1 bit floodVidxMode */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                               (GT_U32)vlanId,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               129,
                                               1,
                                               hwValue);

        if (rc != GT_OK)
        {
            return rc;
        }

    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanFloodVidxModeSet
*
* DESCRIPTION:
*       Function sets Flood VIDX and Flood VIDX Mode per VLAN.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id
*       floodVidx     - VIDX value (12bit) - VIDX value applied to Broadcast,
*                       Unregistered Multicast and unknown Unicast packets,
*                       depending on the VLAN entry field <Flood VIDX Mode>.
*                       (APPLICABLE RANGES: xCat2 0..1023, 4095)
*       floodVidxMode - Flood VIDX Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or floodVidxMode
*       GT_OUT_OF_RANGE          - illegal floodVidx
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanFloodVidxModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U16                                  vlanId,
    IN  GT_U16                                  floodVidx,
    IN  CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT  floodVidxMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanFloodVidxModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, floodVidx, floodVidxMode));

    rc = internal_cpssDxChBrgVlanFloodVidxModeSet(devNum, vlanId, floodVidx, floodVidxMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, floodVidx, floodVidxMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* lion3VlanHwEntryBuildIngress
*
* DESCRIPTION:
*       Bobcat2; Caelum; Bobcat3 : Builds Ingress vlan entry to buffer.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - device number
*       vlanInfoPtr         - (pointer to) VLAN specific information
*
* OUTPUTS:
*       hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS]
*                           - pointer to hw VLAN entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*                         or vlanInfoPtr->stgId
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3VlanHwEntryBuildIngress
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr,
    OUT GT_U32                      hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS]
)
{
    GT_U32      value;          /* value in HW format */

    devNum = devNum; /* avoid compiler warning */

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0, VLAN_INGRESS_ENTRY_WORDS_NUM_CNS * sizeof(GT_U32));

    /* valid bit */
    value = 1;
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VALID_E);

    /* NewSrcAddrIsNotSecurityBreach - revert the bit */
    value = 1 - (BOOL2BIT_MAC(vlanInfoPtr->unkSrcAddrSecBreach));
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NEW_SRC_ADDR_IS_NOT_SECURITY_BREACH_E);

    /* UnregisteredNonIPMulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregNonIpMcastCmd))
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IP_MULTICAST_CMD_E);

    /* UnregisteredIpv4MulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv4McastCmd))
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_MULTICAST_CMD_E);


    /* UnregisteredIpv6MulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv6McastCmd))
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV6_MULTICAST_CMD_E);

    /* UnknownUnicastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unkUcastCmd))
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKNOWN_UNICAST_CMD_E);

    /* Ipv4IgmpToCpuEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IgmpToCpuEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IGMP_TO_CPU_EN_E);

    if(vlanInfoPtr->mirrToRxAnalyzerEn == GT_FALSE) /* ignore index and set 0 to all bits */
    {
        value = 0;
    }
    else
    {
        if(vlanInfoPtr->mirrToRxAnalyzerIndex >  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        value = vlanInfoPtr->mirrToRxAnalyzerIndex + 1;
    }
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MIRROR_TO_INGRESS_ANALYZER_E);

    /* Ipv6IcmpToCpuEn*/
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IcmpToCpuEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_ICMP_TO_CPU_EN_E);

    /* check ipCtrlToCpuEn for ipv4 */
    switch(vlanInfoPtr->ipCtrlToCpuEn)
    {
        case CPSS_DXCH_BRG_IP_CTRL_NONE_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV6_E     :
            value = 0;
            break;
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Ipv4ControlToCpuEn*/
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_CONTROL_TO_CPU_EN_E);

    /* Ipv4IpmBridgingEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IpmBrgEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_EN_E);

    /* Ipv6IpmBridgingEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IpmBrgEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_EN_E);

    /* IPv4 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv4IpmBrgEn)
    {
        /* Ipv4IpmBridgingMode */
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv4IpmBrgMode, value);
        LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_MODE_E);
    }

    /* IPv6 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv6IpmBrgEn)
    {
        /* Ipv6IpmBridgingMode */
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv6IpmBrgMode, value);
        LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_MODE_E);
    }

    /* UnregisteredIpv4BcCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregIpv4BcastCmd);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_BC_CMD_E);

    /* UnregisteredNonIpv4BcCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregNonIpv4BcastCmd);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IPV4_BC_CMD_E);

    /* Ipv4UnicastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4UcastRouteEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_UNICAST_ROUTE_EN_E);

    /* Ipv4MulticastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4McastRouteEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MULTICAST_ROUTE_EN_E);

    /* Ipv6UnicastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6UcastRouteEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_UNICAST_ROUTE_EN_E);

    /* Ipv6MulticastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6McastRouteEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MULTICAST_ROUTE_EN_E);

    switch(vlanInfoPtr->ipv6SiteIdMode)
    {
        case CPSS_IP_SITE_ID_INTERNAL_E:
            value = 0;
            break;
        case CPSS_IP_SITE_ID_EXTERNAL_E:
            value = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* Ipv6SiteId */
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_SITEID_E);

    /* AutoLearnDis */
    value = BOOL2BIT_MAC(vlanInfoPtr->autoLearnDisable);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_AUTO_LEARN_DIS_E);

    /* NaMsgToCpuEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->naMsgToCpuEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E);

    value = vlanInfoPtr->mruIdx;
    /* check MRU index */
    PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(value);
    /* MRU Index */
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MRU_INDEX_E);

    /* BcUdpTrapMirrorEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->bcastUdpTrapMirrEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_BC_UDP_TRAP_MIRROR_EN_E);

    /* check ipCtrlToCpuEn for ipv6 */
    switch(vlanInfoPtr->ipCtrlToCpuEn)
    {
        case CPSS_DXCH_BRG_IP_CTRL_NONE_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_E     :
            value = 0;
            break;
        case CPSS_DXCH_BRG_IP_CTRL_IPV6_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Ipv6ControlToCpuEn */
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_CONTROL_TO_CPU_EN_E);

    if(vlanInfoPtr->floodVidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    value = vlanInfoPtr->floodVidx;
    /* Vidx */
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_EVIDX_E);

    if(vlanInfoPtr->vrfId >= BIT_12 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    /* VrfId */
    value = vlanInfoPtr->vrfId;
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VRF_ID_E);

    /* UcLocalEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ucastLocalSwitchingEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UC_LOCAL_EN_E);

    /* FloodVidxMode */
    PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->floodVidxMode);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_VIDX_MODE_E);

    if(vlanInfoPtr->fidValue > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* fid value  */
    value = vlanInfoPtr->fidValue;
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FID_E);

    /* unknown Mac Sa Command  */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value, vlanInfoPtr->unknownMacSaCmd);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKOWN_MAC_SA_CMD_E);

    /* IPv4 MC/BC Mirror to Analyzer Index */
    if(vlanInfoPtr->ipv4McBcMirrToAnalyzerEn == GT_FALSE) /* ignore index and set 0 to all bits */
    {
        value = 0;
    }
    else
    {
        if(vlanInfoPtr->ipv4McBcMirrToAnalyzerIndex >  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        value = vlanInfoPtr->ipv4McBcMirrToAnalyzerIndex + 1;
    }

    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MC_BC_TO_MIRROR_ANLYZER_IDX_E);

    /* IPv6 MC Mirror to Analyzer Index */
    if(vlanInfoPtr->ipv6McMirrToAnalyzerEn == GT_FALSE) /* ignore index and set 0 to all bits */
    {
        value = 0;
    }
    else
    {
        if(vlanInfoPtr->ipv6McMirrToAnalyzerIndex >  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        value = vlanInfoPtr->ipv6McMirrToAnalyzerIndex + 1;
    }

    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MC_TO_MIRROR_ANALYZER_IDX_E);

    value = BOOL2BIT_MAC(vlanInfoPtr->fcoeForwardingEn);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(
        devNum,value, LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FCOE_FORWARDING_EN_E);

    PRV_CPSS_DXCH_CONVERT_UNREG_IPM_EVIDX_MODE_TO_HW_VAL_MAC(
        value, vlanInfoPtr->unregIpmEVidxMode);
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(
        devNum,value, LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_MODE_E);

    if (vlanInfoPtr->unregIpmEVidx >= BIT_16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    value = vlanInfoPtr->unregIpmEVidx;
    LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(
        devNum,value, LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_E);

    /* FDB Lookup Key mode */
    if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        PRV_CPSS_DXCH_CONVERT_FDB_LOOKUP_KEY_MODE_TO_HW_VAL_MAC(value,
                                                 vlanInfoPtr->fdbLookupKeyMode);
        LION3_SET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,
                     LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FDB_LOOKUP_KEY_MODE_E);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryBuildIngress
*
* DESCRIPTION:
*       Builds Ingress vlan entry to buffer.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information
*
* OUTPUTS:
*       hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS]
*                           - pointer to hw VLAN entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*                         or vlanInfoPtr->stgId
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryBuildIngress
(
    IN GT_U8                        devNum,
    IN GT_U32                       *portsMembersPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr,
    OUT GT_U32                      hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS]
)
{
    GT_U32      value;          /* value in HW format */

    devNum = devNum; /* avoid compiler warning */

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0, VLAN_INGRESS_ENTRY_WORDS_NUM_CNS * sizeof(GT_U32));

    /* bit 0 - valid bit */
    value = 1;
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,0,1,value);

    /* bit 1 - NewSrcAddrIsNotSecurityBreach - revert the bit */
    value = 1 - (BOOL2BIT_MAC(vlanInfoPtr->unkSrcAddrSecBreach));
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,1,1,value);

    /* bit 2..65 port members */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr, 2,32,portsMembersPtr[0]);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,34,32,portsMembersPtr[1]);

    /* bits 66..68 - UnregisteredNonIPMulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregNonIpMcastCmd))
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,66,3,value);

    /* bits 69..71 - UnregisteredIpv4MulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv4McastCmd))
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,69,3,value);


    /* bits 72..74 - UnregisteredIpv6MulticastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unregIpv6McastCmd))
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,72,3,value);

    /* bits 75..77 - UnknownUnicastCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                             (vlanInfoPtr->unkUcastCmd))
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,75,3,value);

    /* bit 78 - Ipv4IgmpToCpuEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IgmpToCpuEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,78,1,value);

    /* bit 81 - mirrToRxAnalyzerEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->mirrToRxAnalyzerEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,81,1,value);


    /* bit 82 - Ipv6IcmpToCpuEn*/
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IcmpToCpuEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,82,1,value);

    /* check ipCtrlToCpuEn for ipv4 */
    switch(vlanInfoPtr->ipCtrlToCpuEn)
    {
        case CPSS_DXCH_BRG_IP_CTRL_NONE_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV6_E     :
            value = 0;
            break;
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bit 83 - Ipv4ControlToCpuEn*/
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,83,1,value);

    /* bit 84 - Ipv4IpmBridgingEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4IpmBrgEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,84,1,value);

    /* bit 85 - Ipv6IpmBridgingEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6IpmBrgEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,85,1,value);

    /* IPv4 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv4IpmBrgEn)
    {
        /* bit 79 - Ipv4IpmBridgingMode */
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv4IpmBrgMode, value);
        U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,79,1,value);
    }

    /* IPv6 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv6IpmBrgEn)
    {
        /* bit 80 - Ipv6IpmBridgingMode */
        PRV_DXCH_BRG_VLAN_IPM_MODE_CONVERT_MAC(vlanInfoPtr->ipv6IpmBrgMode, value);
        U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,80,1,value);
    }

    /* bits 86..88 - UnregisteredIpv4BcCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregIpv4BcastCmd);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,86,3,value);

    /* bits 89..91 - UnregisteredNonIpv4BcCmd */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,
                                            vlanInfoPtr->unregNonIpv4BcastCmd);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,89,3,value);

    /* bit 92 - Ipv4UnicastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4UcastRouteEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,92,1,value);

    /* bit 93 - Ipv4MulticastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv4McastRouteEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,93,1,value);

    /* bit 94 - Ipv6UnicastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6UcastRouteEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,94,1,value);

    /* bit 95 - Ipv6MulticastRouteEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ipv6McastRouteEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,95,1,value);

    switch(vlanInfoPtr->ipv6SiteIdMode)
    {
        case CPSS_IP_SITE_ID_INTERNAL_E:
            value = 0;
            break;
        case CPSS_IP_SITE_ID_EXTERNAL_E:
            value = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* bit 96 - Ipv6SiteId */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,96,1,value);

    /* bit 97 - AutoLearnDis */
    value = BOOL2BIT_MAC(vlanInfoPtr->autoLearnDisable);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,97,1,value);

    /* bit 98 - NaMsgToCpuEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->naMsgToCpuEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,98,1,value);

    value = vlanInfoPtr->mruIdx;
    /* check MRU index */
    PRV_CPSS_DXCH_BRG_MRU_INDEX_CHECK_MAC(value);
    /* bits 99..101 MRU Index */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,99,3,value);

    /* bit 102 - BcUdpTrapMirrorEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->bcastUdpTrapMirrEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,102,1,value);

    /* check ipCtrlToCpuEn for ipv6 */
    switch(vlanInfoPtr->ipCtrlToCpuEn)
    {
        case CPSS_DXCH_BRG_IP_CTRL_NONE_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_E     :
            value = 0;
            break;
        case CPSS_DXCH_BRG_IP_CTRL_IPV6_E     :
        case CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bit 103 - Ipv6ControlToCpuEn */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,103,1,value);

    if(vlanInfoPtr->floodVidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    value = vlanInfoPtr->floodVidx;
    /* bit 104..115 - Vidx */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,104,12,value);

    if(vlanInfoPtr->vrfId >= BIT_12 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    /* bit 116..127 - VrfId */
    value = vlanInfoPtr->vrfId;
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,116,12,value);

    /* bit 128 - UcLocalEn */
    value = BOOL2BIT_MAC(vlanInfoPtr->ucastLocalSwitchingEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,128,1,value);

    /* bit 129 - FloodVidxMode */
    PRV_CPSS_DXCH_CONVERT_FLOOD_VIDX_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->floodVidxMode);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,129,1,value);

    if(vlanInfoPtr->stgId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_STG_ID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    value = vlanInfoPtr->stgId;
    /* bit 130..137 - SpanStateGroupIndex */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,130,8,value);

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryBuildEgress
*
* DESCRIPTION:
*       Builds Egress vlan entry to buffer.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* OUTPUTS:
*       hwDataArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS]
*                           - pointer to hw VLAN entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*                         or vlanInfoPtr->stgId
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryBuildEgress
(
    IN GT_U8                        devNum,
    IN GT_U32                       *portsMembersPtr,
    IN GT_U32                       *portsTaggingPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr,
    IN CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT  *portsTaggingCmdPtr,
    OUT GT_U32                      hwDataArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS]
)
{
    GT_U32      value;          /* value in HW format */
    GT_U32      ii;             /* iterator */
    GT_U32      tagCmdMaxPorts = 63;  /* 63 ports with tagCmd -- NOT 64 !!! */
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT currTagCmd;
    GT_U32      startBit;/* start bit in entry */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0, VLAN_EGRESS_ENTRY_WORDS_NUM_CNS * sizeof(GT_U32));

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    /* bit 0 - valid bit */
    value = 1;
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,0,1,value);

    /* bit 1..64 port members */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr, 1,32,portsMembersPtr[0]);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,33,32,portsMembersPtr[1]);

    startBit = 65;
    /* tagCmd info for all ports */
    for (ii = 0; ii < tagCmdMaxPorts; ii++ , startBit += 3)
    {
        if (tr101Supported == GT_FALSE)
        {
            /*U32_GET_FIELD_IN_ENTRY_MAC*/
            currTagCmd = ((portsTaggingPtr[(ii)>>5] & (1 << ((ii)& 0x1f)))? 1 : 0) ?
                CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E :
                CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
        }
        else
        {
            currTagCmd = portsTaggingCmdPtr[ii];
        }

        PRV_CPSS_DXCH_CONVERT_VLAN_TAG_MODE_TO_HW_VAL_MAC(devNum, value,currTagCmd);

        /* 65..253 - tagCmd of ports */
        U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,startBit, 3 ,value);
    }

    value = vlanInfoPtr->stgId;
    /* bit 254..261 - SpanStateGroupIndex */
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,254,8,value);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        value = (vlanInfoPtr->stgId >> 8);
        /* bit 265..268 - extra 4 bits from SpanStateGroupIndex */
        U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,265,4,value);
    }

    /* bit 262 - LocalSwithingEn*/
    value = BOOL2BIT_MAC(vlanInfoPtr->mcastLocalSwitchingEn);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,262,1,value);

    /* bits 263..264 - PortIsolationVlanCmd */
    PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->portIsolationMode);
    U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,263,2,value);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Mirror to egress analyzer port- new field bits [269:271] */

        if(vlanInfoPtr->mirrToTxAnalyzerEn == GT_FALSE) /* ignore index and set 0 to all bits */
        {
            value = 0;
        }
        else
        {
            if(vlanInfoPtr->mirrToTxAnalyzerIndex >  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            value = vlanInfoPtr->mirrToTxAnalyzerIndex + 1;
        }

        U32_SET_FIELD_IN_ENTRY_MAC(hwDataArr,269,3,value);
    }


    return GT_OK;
}

/*******************************************************************************
* lion3VlanHwEntryBuildEgress
*
* DESCRIPTION:
*       Builds Egress vlan entry to buffer - Bobcat2; Caelum; Bobcat3
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; .
*
* INPUTS:
*       devNum              - device number
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* OUTPUTS:
*       egfSht_hwDataArr - pointer to hw VLAN entry - EGF_SHT entry
*       egfShtVlanAttrib_hwDataArr - pointer to hw VLAN Attributes entry
*       egfShtVlanSpanId_hwDataArr - pointer to hw VLAN SpanTree Id entry
*       egfQag_hwDataArr - pointer to hw VLAN entry - EGF_QAG entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*                         or vlanInfoPtr->stgId
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3VlanHwEntryBuildEgress
(
    IN GT_U8                        devNum,
    IN GT_U32                       *portsMembersPtr,
    IN GT_U32                       *portsTaggingPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr,
    IN CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT  *portsTaggingCmdPtr,
    OUT GT_U32                       egfSht_hwDataArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS],
    OUT GT_U32                       egfShtVlanAttrib_hwDataArr[1],
    OUT GT_U32                       egfShtVlanSpanId_hwDataArr[1],
    OUT GT_U32                       egfQag_hwDataArr[EGF_QAG_EGRESS_VLAN_ENTRY_WORDS_NUM_CNS]
)
{
    GT_U32      value;          /* value in HW format */
    GT_U32      ii;             /* iterator */
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT currTagCmd;
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */
    GT_U32      mirrorIndex;      /* mirror index in HW format */
    CPSS_PORTS_BMP_STC          existingPorts;

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &egfSht_hwDataArr[0], 0, VLAN_EGRESS_ENTRY_WORDS_NUM_CNS * sizeof(GT_U32));
    cpssOsMemSet((char *) &egfQag_hwDataArr[0], 0, EGF_QAG_EGRESS_VLAN_ENTRY_WORDS_NUM_CNS * sizeof(GT_U32));

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    /* Mirror to egress analyzer port */
    if(vlanInfoPtr->mirrToTxAnalyzerEn == GT_FALSE) /* ignore index and set 0 to all bits */
    {
        mirrorIndex = 0;
    }
    else
    {
        if(vlanInfoPtr->mirrToTxAnalyzerIndex >  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        mirrorIndex = vlanInfoPtr->mirrToTxAnalyzerIndex + 1;
    }

/**************** EGF_SHT section *********************************************/

    if (!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        /* valid bit */
        value = 1;
        LION3_SET_FIELD_IN_EGF_SHT_EGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_VALID_E);
    }

    /* port members */
    U_ANY_SET_FIELD_BY_ID_MAC(egfSht_hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EVLAN_E].fieldsInfoPtr,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_PORT_X_MEMBER_E,
        portsMembersPtr);/* pointer to the array of 256 ports */

    /* SpanStateGroupIndex */
    if (vlanInfoPtr->stgId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_STG_ID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    egfShtVlanSpanId_hwDataArr[0] = vlanInfoPtr->stgId;

    egfShtVlanAttrib_hwDataArr[0] = 0;

    /* LocalSwithingEn*/
    value = BOOL2BIT_MAC(vlanInfoPtr->mcastLocalSwitchingEn);

    U32_SET_FIELD_BY_ID_MAC(
        egfShtVlanAttrib_hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_VLAN_ATTRIBUTES_E].fieldsInfoPtr,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_LOCAL_SWITHING_EN_E,
        value);

    /* PortIsolationVlanCmd */
    PRV_CPSS_DXCH_CONVERT_PORT_ISOLATION_MODE_TO_HW_VAL_MAC(value, vlanInfoPtr->portIsolationMode);

    U32_SET_FIELD_BY_ID_MAC(
        egfShtVlanAttrib_hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_VLAN_ATTRIBUTES_E].fieldsInfoPtr,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_PORT_ISOLATION_VLAN_CMD_E,
        value);

    U32_SET_FIELD_BY_ID_MAC(
        egfShtVlanAttrib_hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_VLAN_ATTRIBUTES_E].fieldsInfoPtr,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_EGRESS_MIRROR_TO_ANALYZER_INDEX_E,
        mirrorIndex);

/**************** EGF_QAG section *********************************************/

    /* get the bmp of supported port on this device */
    prvCpssDrvPortsFullMaskGet(
        PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum),
            &existingPorts);

    /* tagCmd info for all ports */
    for (ii = 0; ii < CPSS_MAX_PORTS_NUM_CNS ; ii++)
    {
        if (tr101Supported == GT_FALSE)
        {
            /*U32_GET_FIELD_IN_ENTRY_MAC*/
            currTagCmd = ((portsTaggingPtr[(ii)>>5] & (1 << ((ii)& 0x1f)))? 1 : 0) ?
                CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E :
                CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
        }
        else
        {
            currTagCmd = portsTaggingCmdPtr[ii];
        }

        PRV_CPSS_DXCH_CONVERT_VLAN_TAG_MODE_TO_HW_VAL_MAC(devNum, value,currTagCmd);

        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&existingPorts,ii))
        {
            /* make sure that caller 'ignores' non exists ports */
            if(currTagCmd != CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            continue;
        }

        /* tagCmd of ports */
        U32_SET_FIELD_IN_ENTRY_MAC(egfQag_hwDataArr,(ii*3), 3 ,value);
    }

    return GT_OK;
}



/*******************************************************************************
* vlanEntrySplitTableWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW - for split table (ingress,egress)
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_OUT_OF_RANGE          - length of portsMembersPtr, portsTaggingPtr
*                                  or vlanInfoPtr->stgId is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS vlanEntrySplitTableWrite
(
    IN  GT_U8                                 devNum,
    IN  GT_U16                                vlanId,
    IN  CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    IN  CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    IN  CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_STATUS   rc;
    GT_U32      i;                /* iterator */
    /* ingress HW format */
    GT_U32      ingressHwData[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS];
    /* egress HW format */
    GT_U32      egressHwData[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS];
    GT_U32      efgQag_egressHwData[EGF_QAG_EGRESS_VLAN_ENTRY_WORDS_NUM_CNS];/*Bobcat2: EGF_QAG info*/

    CPSS_PORTS_BMP_STC portsMembersBmp; /* local variable to avoid portsMembersPtr change */
    GT_U32      egfShtVlanAttrib_hwDataArr[1]; /* HW entry of Ergess Vlan Attributes Table  */
    GT_U32      egfShtVlanSpanId_hwDataArr[1]; /* HW entry of Ergess Vlan Span Tree Id Table */

    /* CPU port conversion */
    cpssOsMemCpy(&portsMembersBmp, portsMembersPtr, sizeof(CPSS_PORTS_BMP_STC));

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* the ingress table looks the same on all port groups */

        /* check that the device supports all the physical ports that defined in the BMP */
        rc = prvCpssDxChTablePortsBmpCheck(devNum , portsMembersPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* build ingress VLAN entry */
        rc = lion3VlanHwEntryBuildIngress(devNum, vlanInfoPtr, &ingressHwData[0]);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write ingress VLAN entry */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                    vlanId,/* vid */
                                    &ingressHwData[0]);
        if (rc != GT_OK)
        {
            return rc;
        }
        PRV_CPSS_DXCH_FDB_LOOKUP_KEY_MODE_PER_FID_SET_MAC(
            devNum, vlanInfoPtr->fidValue, vlanInfoPtr->fdbLookupKeyMode);

        /* write ingress bridge port members entry */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E,
                                    vlanId,/* vid */
                                    &portsMembersPtr->ports[0]);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write ingress spanning state group index */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E,
                                    vlanId,/* vid */
                                    &vlanInfoPtr->stgId);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* build egress VLAN entry */
        rc = lion3VlanHwEntryBuildEgress(devNum,
                  &(portsMembersBmp.ports[0]),
                  &(portsTaggingPtr->ports[0]),
                  vlanInfoPtr,
                  &(portsTaggingCmdPtr->portsCmd[0]),
                  &egressHwData[0],
                  &egfShtVlanAttrib_hwDataArr[0],
                  &egfShtVlanSpanId_hwDataArr[0],
                  &efgQag_egressHwData[0]);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* the ingress table looks the same on all port groups */

        /* write entry to HW EGF_SHT egress table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                    vlanId,/* vid */
                                    &egressHwData[0]);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write entry to HW EGF_SHT egress Vlan Attributes table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E,
                                    vlanId,/* vid */
                                    &egfShtVlanAttrib_hwDataArr[0]);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write entry to HW EGF_SHT egress Vlan Span Tree Id table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_SPANNING_E,
                                    vlanId,/* vid */
                                    &egfShtVlanSpanId_hwDataArr[0]);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write entry to HW EGF_QAG egress table */
        rc = prvCpssDxChWriteTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E,
                                   vlanId,/* vid */
                                   &efgQag_egressHwData[0]);
        return rc;
    }
    else
    {

        /* Update port bitmap according to physical CPU port connection */
        rc = prvCpssDxChBrgCpuPortBmpConvert(devNum,
                                             GT_TRUE,
                                             &portsMembersBmp);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (GT_FALSE != PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                info_PRV_CPSS_DXCH_LION2_MC_BANDWIDTH_RESTRICTION_WA_E.enabled)
        {
            /* port 12 on each hemisphere is used to duplicate the packet to the other hemisphere;
               this port should be member of the VLAN only if there are additional members on the
               other hemisphere; note that port 12 is reversed logic (0: member, 1: not a member) */

            /* check if the VLAN contains members on the 1st hemisphere (other than port 12) */
            if (((portsMembersBmp.ports[0] & 0xffffefff) == 0) && (portsMembersBmp.ports[1] == 0))
            {
                /* internally set port 12 on 2nd hemisphere to indicate it is NOT a member */
                CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembersBmp,64+12);
            }
            else
            {
                /* internally clear port 12 on 2nd hemisphere to indicate it IS a member */
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portsMembersBmp,64+12);
            }

            /* check if the VLAN contains members on the 2nd hemisphere (other than port 12) */
            if (((portsMembersBmp.ports[2] & 0xffffefff) == 0) && (portsMembersBmp.ports[3] == 0))
            {
                /* internally set port 12 on 1st hemisphere to indicate it is NOT a member */
                CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembersBmp,12);
            }
            else
            {
                /* internally clear port 12 on 1st hemisphere to indicate it IS a member */
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portsMembersBmp,12);
            }
        }



        /* loop on all TXQ units */
        for (i =0; i < PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(devNum); i++)
        {
            /* build ingress VLAN entry */
            rc = prvCpssBrgVlanHwEntryBuildIngress(devNum,
                      &(portsMembersBmp.ports[i*PRV_CPSS_DXCH_PORT_WITH_CPU_BMP_NUM_IN_HEMISPHERE_CNS]),
                                            vlanInfoPtr,
                                            &ingressHwData[0]);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* write ingress VLAN entry to the ingress VLAN Table */
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        (GT_U32)vlanId,/* vid */
                                        &ingressHwData[0]);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* build egress VLAN entry */
            rc = prvCpssBrgVlanHwEntryBuildEgress(devNum,
                      &(portsMembersBmp.ports[i*PRV_CPSS_DXCH_PORT_WITH_CPU_BMP_NUM_IN_HEMISPHERE_CNS]),
                      &(portsTaggingPtr->ports[i*PRV_CPSS_DXCH_PORT_WITH_CPU_BMP_NUM_IN_HEMISPHERE_CNS]),
                                            vlanInfoPtr,
                      &(portsTaggingCmdPtr->portsCmd[i*PRV_CPSS_DXCH_PORT_WITH_CPU_NUM_IN_HEMISPHERE_CNS]),
                                            &egressHwData[0]);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* write egress VLAN entry to the egress VLAN Table */
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                        (GT_U32)vlanId,/* vid */
                                        &egressHwData[0]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }
}
/*******************************************************************************
* lion3VlanHwEntryParseIngress
*
* DESCRIPTION:
*       Bobcat2; Caelum; Bobcat3 : parse vlan entry from buffer. - ingress vlan
*
*       NOTE: for multi-port-groups device , this function called per port group , to get the
*             per port group info
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       hwDataArr - pointer to hw VLAN entry.
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3VlanHwEntryParseIngress
(
    IN  GT_U8                       devNum,
    IN GT_U16                       vlanId,
    IN GT_U32                       hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS],
    OUT GT_U32                      *portsMembersPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC *vlanInfoPtr,
    OUT GT_BOOL                     *isValidPtr
)
{
    GT_STATUS   rc;
    GT_U32      value;

    /* valid bit */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VALID_E);
    *isValidPtr = BIT2BOOL_MAC(value);

    /* NewSrcAddrIsNotSecurityBreach - revert the bit */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NEW_SRC_ADDR_IS_NOT_SECURITY_BREACH_E);
    vlanInfoPtr->unkSrcAddrSecBreach = BIT2BOOL_MAC(value - 1);

    /* UnregisteredNonIPMulticastCmd */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IP_MULTICAST_CMD_E);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpMcastCmd,
                                              value)

    /* UnregisteredIpv4MulticastCmd */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_MULTICAST_CMD_E);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4McastCmd,
                                              value)


    /* UnregisteredIpv6MulticastCmd */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV6_MULTICAST_CMD_E);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv6McastCmd,
                                              value)

    /* UnknownUnicastCmd */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKNOWN_UNICAST_CMD_E);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unkUcastCmd,
                                              value)

    /* Ipv4IgmpToCpuEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IGMP_TO_CPU_EN_E);
    vlanInfoPtr->ipv4IgmpToCpuEn = BIT2BOOL_MAC(value);

    /* Mirror to ingress analyzer port*/
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MIRROR_TO_INGRESS_ANALYZER_E);
    if(value == 0)
    {
        /* ignore index */
        vlanInfoPtr->mirrToRxAnalyzerEn = GT_FALSE;
    }
    else
    {
        vlanInfoPtr->mirrToRxAnalyzerEn = GT_TRUE;
        vlanInfoPtr->mirrToRxAnalyzerIndex = value - 1;
    }

    /* Ipv6IcmpToCpuEn*/
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_ICMP_TO_CPU_EN_E);
    vlanInfoPtr->ipv6IcmpToCpuEn = BIT2BOOL_MAC(value);

    /* parse the Ipv4ControlToCpuEn,
       (the Ipv6ControlToCpuEn will be read later , and the ipCtrlToCpuEn will
       be changed accordingly */

    /* Ipv4ControlToCpuEn*/
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_CONTROL_TO_CPU_EN_E);
    vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_E :
                                    CPSS_DXCH_BRG_IP_CTRL_NONE_E;

    /* Ipv4IpmBridgingEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_EN_E);
    vlanInfoPtr->ipv4IpmBrgEn = BIT2BOOL_MAC(value);

    /* Ipv6IpmBridgingEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_EN_E);
    vlanInfoPtr->ipv6IpmBrgEn = BIT2BOOL_MAC(value);

    /* IPv4 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv4IpmBrgEn)
    {
        /* Ipv4IpmBridgingMode */
        LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_MODE_E);
        vlanInfoPtr->ipv4IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                     CPSS_BRG_IPM_GV_E;
    }

    /* IPv6 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv6IpmBrgEn)
    {
        /* Ipv6IpmBridgingMode */
        LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_MODE_E);
        vlanInfoPtr->ipv6IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                     CPSS_BRG_IPM_GV_E;
    }

    /* UnregisteredIpv4BcCmd */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_BC_CMD_E);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4BcastCmd,
                                              value)

    /* UnregisteredNonIpv4BcCmd */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IPV4_BC_CMD_E);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpv4BcastCmd,
                                              value)

    /* Ipv4UnicastRouteEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_UNICAST_ROUTE_EN_E);
    vlanInfoPtr->ipv4UcastRouteEn = BIT2BOOL_MAC(value);

    /* Ipv4MulticastRouteEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MULTICAST_ROUTE_EN_E);
    vlanInfoPtr->ipv4McastRouteEn = BIT2BOOL_MAC(value);

    /* Ipv6UnicastRouteEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_UNICAST_ROUTE_EN_E);
    vlanInfoPtr->ipv6UcastRouteEn = BIT2BOOL_MAC(value);

    /* Ipv6MulticastRouteEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MULTICAST_ROUTE_EN_E);
    vlanInfoPtr->ipv6McastRouteEn = BIT2BOOL_MAC(value);

    /* Ipv6SiteId */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_SITEID_E);
    vlanInfoPtr->ipv6SiteIdMode = (value == 0x0) ?
        CPSS_IP_SITE_ID_INTERNAL_E :
        CPSS_IP_SITE_ID_EXTERNAL_E;

    /* AutoLearnDis */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_AUTO_LEARN_DIS_E);
    vlanInfoPtr->autoLearnDisable = BIT2BOOL_MAC(value);

    /* NaMsgToCpuEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E);
    vlanInfoPtr->naMsgToCpuEn = BIT2BOOL_MAC(value);

    /* MRU Index */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MRU_INDEX_E);
    vlanInfoPtr->mruIdx = value;

    /* BcUdpTrapMirrorEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_BC_UDP_TRAP_MIRROR_EN_E);
    vlanInfoPtr->bcastUdpTrapMirrEn = BIT2BOOL_MAC(value);

    /* Ipv6ControlToCpuEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_CONTROL_TO_CPU_EN_E);

    if(vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_NONE_E)
    {
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV6_E :
                                    CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }
    else /*CPSS_DXCH_BRG_IP_CTRL_IPV4_E*/
    {
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E :
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_E;
    }

    /* Vidx */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_EVIDX_E);
    vlanInfoPtr->floodVidx = (GT_U16)value;

    /* VrfId */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VRF_ID_E);
    vlanInfoPtr->vrfId = value;

    /* UcLocalEn */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UC_LOCAL_EN_E);
    vlanInfoPtr->ucastLocalSwitchingEn = BIT2BOOL_MAC(value);

    /* FloodVidxMode */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_VIDX_MODE_E);
    vlanInfoPtr->floodVidxMode = (value == 0) ? CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E
                                              : CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;

    /* read entry from HW ingress Span State Group Index table */
    rc = prvCpssDxChReadTableEntry(devNum,
                               PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E,
                               vlanId,/* vid */
                               &vlanInfoPtr->stgId);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* fid value */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FID_E);
    vlanInfoPtr->fidValue = value;

    /* unknown Mac Sa Command  */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKOWN_MAC_SA_CMD_E);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unknownMacSaCmd, value);

    /* IPv4 MC/BC Mirror to Analyzer Index */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MC_BC_TO_MIRROR_ANLYZER_IDX_E);
    if(value == 0)
    {
        /* ignore index */
        vlanInfoPtr->ipv4McBcMirrToAnalyzerEn = GT_FALSE;
    }
    else
    {
        vlanInfoPtr->ipv4McBcMirrToAnalyzerEn = GT_TRUE;
        vlanInfoPtr->ipv4McBcMirrToAnalyzerIndex = value - 1;
    }

    /* IPv6 MC Mirror to Analyzer Index */
    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MC_TO_MIRROR_ANALYZER_IDX_E);
    if(value == 0)
    {
        /* ignore index */
        vlanInfoPtr->ipv6McMirrToAnalyzerEn = GT_FALSE;
    }
    else
    {
        vlanInfoPtr->ipv6McMirrToAnalyzerEn = GT_TRUE;
        vlanInfoPtr->ipv6McMirrToAnalyzerIndex = value - 1;
    }


    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(
        devNum,value, LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FCOE_FORWARDING_EN_E);
    vlanInfoPtr->fcoeForwardingEn = BIT2BOOL_MAC(value);

    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(
        devNum,value, LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_MODE_E);
    PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_UNREG_IPM_EVIDX_MODE_MAC(
        value, vlanInfoPtr->unregIpmEVidxMode);

    LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(
        devNum,value, LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_E);
    vlanInfoPtr->unregIpmEVidx = value;

    /* FDB Lookup Key mode */
    if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        LION3_GET_FIELD_IN_INGRESS_VLAN_ENTRY_MAC(devNum,value,
                     LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FDB_LOOKUP_KEY_MODE_E);
        PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_FDB_LOOKUP_KEY_MODE_MAC(value,
                                                 vlanInfoPtr->fdbLookupKeyMode);
    }
    else
    {
        vlanInfoPtr->fdbLookupKeyMode = CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_E;
    }

    /* read entry from HW ingress port members table */
    rc = prvCpssDxChReadTableEntry(devNum,
                               PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E,
                               vlanId,/* vid */
                               portsMembersPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryParseIngress
*
* DESCRIPTION:
*       parse vlan entry from buffer. - ingress vlan
*
*       NOTE: for multi-port-groups device , this function called per port group , to get the
*             per port group info
*
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       hwDataArr - pointer to hw VLAN entry.
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryParseIngress
(
    IN GT_U32                       hwDataArr[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS],
    OUT GT_U32                      *portsMembersPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC *vlanInfoPtr,
    OUT GT_BOOL                     *isValidPtr
)
{
    GT_U32      value;

    /* bit 0 - valid bit */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,0,1,value);
    *isValidPtr = BIT2BOOL_MAC(value);

    /* bit 1 - NewSrcAddrIsNotSecurityBreach - revert the bit */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,1,1,value);
    vlanInfoPtr->unkSrcAddrSecBreach = BIT2BOOL_MAC(value - 1);

    /* bit 2..65 port members */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr, 2,32,portsMembersPtr[0]);
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,34,32,portsMembersPtr[1]);

    /* bits 66..68 - UnregisteredNonIPMulticastCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,66,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpMcastCmd,
                                              value)

    /* bits 69..71 - UnregisteredIpv4MulticastCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,69,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4McastCmd,
                                              value)


    /* bits 72..74 - UnregisteredIpv6MulticastCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,72,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv6McastCmd,
                                              value)

    /* bits 75..77 - UnknownUnicastCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,75,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unkUcastCmd,
                                              value)

    /* bit 78 - Ipv4IgmpToCpuEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,78,1,value);
    vlanInfoPtr->ipv4IgmpToCpuEn = BIT2BOOL_MAC(value);

   /* bit 81 - mirrToRxAnalyzerEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,81,1,value);
    vlanInfoPtr->mirrToRxAnalyzerEn = BIT2BOOL_MAC(value);

    /* bit 82 - Ipv6IcmpToCpuEn*/
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,82,1,value);
    vlanInfoPtr->ipv6IcmpToCpuEn = BIT2BOOL_MAC(value);

    /* parse the Ipv4ControlToCpuEn,
       (the Ipv6ControlToCpuEn will be read later , and the ipCtrlToCpuEn will
       be changed accordingly */

    /* bit 83 - Ipv4ControlToCpuEn*/
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,83,1,value);
    vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_E :
                                    CPSS_DXCH_BRG_IP_CTRL_NONE_E;

    /* bit 84 - Ipv4IpmBridgingEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,84,1,value);
    vlanInfoPtr->ipv4IpmBrgEn = BIT2BOOL_MAC(value);

    /* bit 85 - Ipv6IpmBridgingEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,85,1,value);
    vlanInfoPtr->ipv6IpmBrgEn = BIT2BOOL_MAC(value);

    /* IPv4 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv4IpmBrgEn)
    {
        /* bit 79 - Ipv4IpmBridgingMode */
        U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,79,1,value);
        vlanInfoPtr->ipv4IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                     CPSS_BRG_IPM_GV_E;
    }

    /* IPv6 IPM Bridging mode */
    if(GT_TRUE == vlanInfoPtr->ipv6IpmBrgEn)
    {
        /* bit 80 - Ipv6IpmBridgingMode */
        U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,80,1,value);
        vlanInfoPtr->ipv6IpmBrgMode = (value == 0x0) ? CPSS_BRG_IPM_SGV_E :
                                                     CPSS_BRG_IPM_GV_E;
    }

    /* bits 86..88 - UnregisteredIpv4BcCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,86,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregIpv4BcastCmd,
                                              value)

    /* bits 89..91 - UnregisteredNonIpv4BcCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,89,3,value);
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(vlanInfoPtr->unregNonIpv4BcastCmd,
                                              value)

    /* bit 92 - Ipv4UnicastRouteEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,92,1,value);
    vlanInfoPtr->ipv4UcastRouteEn = BIT2BOOL_MAC(value);

    /* bit 93 - Ipv4MulticastRouteEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,93,1,value);
    vlanInfoPtr->ipv4McastRouteEn = BIT2BOOL_MAC(value);

    /* bit 94 - Ipv6UnicastRouteEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,94,1,value);
    vlanInfoPtr->ipv6UcastRouteEn = BIT2BOOL_MAC(value);

    /* bit 95 - Ipv6MulticastRouteEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,95,1,value);
    vlanInfoPtr->ipv6McastRouteEn = BIT2BOOL_MAC(value);

    /* bit 96 - Ipv6SiteId */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,96,1,value);
    vlanInfoPtr->ipv6SiteIdMode = (value == 0x0) ?
        CPSS_IP_SITE_ID_INTERNAL_E :
        CPSS_IP_SITE_ID_EXTERNAL_E;

    /* bit 97 - AutoLearnDis */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,97,1,value);
    vlanInfoPtr->autoLearnDisable = BIT2BOOL_MAC(value);

    /* bit 98 - NaMsgToCpuEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,98,1,value);
    vlanInfoPtr->naMsgToCpuEn = BIT2BOOL_MAC(value);

    /* bits 99..101 MRU Index */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,99,3,value);
    vlanInfoPtr->mruIdx = value;

    /* bit 102 - BcUdpTrapMirrorEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,102,1,value);
    vlanInfoPtr->bcastUdpTrapMirrEn = BIT2BOOL_MAC(value);

    /* bit 103 - Ipv6ControlToCpuEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,103,1,value);

    if(vlanInfoPtr->ipCtrlToCpuEn == CPSS_DXCH_BRG_IP_CTRL_NONE_E)
    {
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV6_E :
                                    CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    }
    else /*CPSS_DXCH_BRG_IP_CTRL_IPV4_E*/
    {
        vlanInfoPtr->ipCtrlToCpuEn = (value == 0x1) ?
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E :
                                    CPSS_DXCH_BRG_IP_CTRL_IPV4_E;
    }

    /* bit 104..115 - Vidx */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,104,12,value);
    vlanInfoPtr->floodVidx = (GT_U16)value;

    /* bit 116..127 - VrfId */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,116,12,value);
    vlanInfoPtr->vrfId = value;

    /* bit 128 - UcLocalEn */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,128,1,value);
    vlanInfoPtr->ucastLocalSwitchingEn = BIT2BOOL_MAC(value);

    /* bit 129 - FloodVidxMode */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,129,1,value);
    vlanInfoPtr->floodVidxMode = (value == 0) ? CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E
                                              : CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;

    /* bit 130..137 - SpanStateGroupIndex */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,130,8,value);

    vlanInfoPtr->stgId = value;

    return GT_OK;
}

/*******************************************************************************
* prvCpssBrgVlanHwEntryParseEgress
*
* DESCRIPTION:
*       parse vlan entry from buffer. -- egress vlan
*
*       NOTE: for multi-port-groups device , this function called per port group , to get the
*             per port group info
*
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - device number
*       hwDataArr - pointer to hw VLAN entry.
*       isValidPtr          - (pointer to) INGRESS VLAN entry status
*       portsMembersPtr     - (pointer to) bmp of ports members in INGRESS vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information -
*                              field of : PortIsolationVlanCmd is taken from stgId
*
* OUTPUTS:
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             the relevant parameter for DxCh3 and beyond and
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) egress VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - the ingress and egress vlan hold 'same fields' but with
*                         different values : valid,members,stgId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssBrgVlanHwEntryParseEgress
(
    IN GT_U8                        devNum,
    IN GT_U32                       hwDataArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS],
    IN GT_U32                       *portsMembersPtr,
    OUT GT_U32                      *portsTaggingPtr,
    INOUT CPSS_DXCH_BRG_VLAN_INFO_STC *vlanInfoPtr,
    IN GT_BOOL                      *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT  *portsTaggingCmdPtr
)
{
    GT_U32      value;          /* value in HW format */
    GT_U32      value1;          /* value in HW format */
    GT_U32      ii;             /* iterator */
    GT_U32      tagCmdMaxPorts = 63;  /* 63 ports with tagCmd -- NOT 64 !!! */
    GT_U32      startBit;/* start bit in entry */
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    /* bit 0 - valid bit */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,0,1,value);
    if(value != (GT_U32)(BOOL2BIT_MAC(*isValidPtr)))
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* bit 1..64 port members */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr, 1,32,value);
    if(portsMembersPtr[0] != value)
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,33,32,value);
    if(portsMembersPtr[1] != value)
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    startBit = 65;
    /* tagCmd info for all ports */
    for (ii = 0; ii < tagCmdMaxPorts; ii++ , startBit += 3)
    {
        /* 65..253 - tagCmd of ports */
        U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,startBit, 3 ,value);

        if (tr101Supported == GT_FALSE)
        {
            if(value != 0)
            {
                portsTaggingPtr[(ii)>>5] |= 1 << ((ii)& 0x1f);
            }
        }
        else
        {
            PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_VLAN_TAG_MODE_MAC(devNum, value,
                    portsTaggingCmdPtr[ii]);
        }
    }

    /* bit 254..261 - SpanStateGroupIndex */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,254,8,value);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* bit 265..268 - extra 4 bits from SpanStateGroupIndex */
        U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,265,4,value1);
        value |= (value1 << 8);
    }

    if(value != vlanInfoPtr->stgId)
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* bit 262 - LocalSwithingEn*/
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,262,1,value);
    vlanInfoPtr->mcastLocalSwitchingEn = BIT2BOOL_MAC (value);

    /* bits 263..264 - PortIsolationVlanCmd */
    U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,263,2,value);
    switch (value)
    {
        case 0:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
            break;
        case 1:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E;
            break;
        case 2:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E;
            break;
        default:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Mirror to egress analyzer port-new fiels bits [269:271] */

        U32_GET_FIELD_IN_ENTRY_MAC(hwDataArr,269,3,value);

        if(value == 0)
        {
            /* ignore index */
            vlanInfoPtr->mirrToTxAnalyzerEn = GT_FALSE;
        }
        else
        {
            vlanInfoPtr->mirrToTxAnalyzerEn = GT_TRUE;

             U32_SET_FIELD_IN_ENTRY_MAC(&vlanInfoPtr->mirrToTxAnalyzerIndex,0,3,value);

             vlanInfoPtr->mirrToTxAnalyzerIndex -= 1;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* lion3VlanHwEntryParseEgress
*
* DESCRIPTION:
*       parse vlan entry from buffer. -- egress vlan - Bobcat2; Caelum; Bobcat3
*
*       NOTE: all port groups hold the same info
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       egfSht_hwDataArr - pointer to hw VLAN entry - EGF_SHT entry
*       egfShtVlanAttrib_hwDataArr - pointer to hw VLAN Attributes entry
*       egfShtVlanSpanId_hwDataArr - pointer to hw VLAN SpanTree Id entry
*       egfQag_hwDataArr - pointer to hw VLAN entry - EGF_QAG entry
*       isValidPtr          - (pointer to) INGRESS VLAN entry status
*       vlanInfoPtr         - (pointer to) VLAN specific information -
*                              field of : stgId is taken from INGRESS vlan
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in INGRESS vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             not relevant for TR101 supported devices.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) egress VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             the relevant parameter for xCat and above
*                             with TR101 feature support.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - the ingress and egress vlan hold 'same fields' but with
*                         different values : valid,members,stgId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3VlanHwEntryParseEgress
(
    IN GT_U8                        devNum,
    IN GT_U32                       egfSht_hwDataArr[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS],
    IN GT_U32                       egfShtVlanAttrib_hwDataArr[1],
    IN GT_U32                       egfShtVlanSpanId_hwDataArr[1],
    IN GT_U32                       egfQag_hwDataArr[EGF_QAG_EGRESS_VLAN_ENTRY_WORDS_NUM_CNS],
    OUT CPSS_PORTS_BMP_STC          *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC          *portsTaggingPtr,
    INOUT CPSS_DXCH_BRG_VLAN_INFO_STC *vlanInfoPtr,
    IN GT_BOOL                      *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
)
{
    GT_STATUS   rc;
    GT_U32      ii;                     /* iterator */
    GT_U32      value;                  /* value in HW format */
    GT_U32      hwMirrorIndex;          /* Mirror Index in HW format      */
    GT_U32      hwPortIsolationMode;    /* PortIsolationMode in HW format */
    GT_BOOL     tr101Supported;         /* TR101 Feature Support */
    CPSS_PORTS_BMP_STC  portsBmpMembers;/*ports members in vlan*/
    CPSS_PORTS_BMP_STC  adjusted_portsBmpMembers;/* adjusted ports bmp of only existing ports */
    CPSS_PORTS_BMP_STC  adjusted_IN_portsBmpMembers;/* adjusted ports bmp of only existing ports */
    CPSS_PORTS_BMP_STC  existingPorts;

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

/**************** EGF_SHT section *********************************************/

    /* valid bit */
    if (!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        LION3_GET_FIELD_IN_EGF_SHT_EGRESS_VLAN_ENTRY_MAC(devNum,value,LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_VALID_E);
        *isValidPtr = BIT2BOOL_MAC(value);

        if(value != (GT_U32)(BOOL2BIT_MAC(*isValidPtr)))
        {
            /* the ingress vlan and the egress vlan not synchronized ?! */
            /* we have not API to allow this situation */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    /* port members */
    U_ANY_GET_FIELD_BY_ID_MAC(egfSht_hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EVLAN_E].fieldsInfoPtr,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_PORT_X_MEMBER_E,
        &portsBmpMembers.ports[0]);/* pointer to the array of 256 ports */

    /* adjusted physical ports of BMP to hold only bmp of existing ports*/
    rc = prvCpssDxChTablePortsBmpAdjustToDevice(devNum , &portsBmpMembers , &adjusted_portsBmpMembers);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* adjusted physical ports of BMP to hold only bmp of existing ports*/
    rc = prvCpssDxChTablePortsBmpAdjustToDevice(devNum , portsMembersPtr , &adjusted_IN_portsBmpMembers);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(0 == CPSS_PORTS_BMP_ARE_EQUAL_MAC(&adjusted_portsBmpMembers,&adjusted_IN_portsBmpMembers))
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have no API to allow this situation */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* copy value into the OUT parameter */
    *portsMembersPtr = adjusted_IN_portsBmpMembers;

    /* SpanStateGroupIndex */
    if(egfShtVlanSpanId_hwDataArr[0] != (GT_U32)vlanInfoPtr->stgId)
    {
        /* the ingress vlan and the egress vlan not synchronized ?! */
        /* we have not API to allow this situation */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* LocalSwithingEn*/
    U32_GET_FIELD_BY_ID_MAC(egfShtVlanAttrib_hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_VLAN_ATTRIBUTES_E].fieldsInfoPtr,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_LOCAL_SWITHING_EN_E,
        value);
    vlanInfoPtr->mcastLocalSwitchingEn = BIT2BOOL_MAC(value);

    /* PortIsolationVlanCmd */
    U32_GET_FIELD_BY_ID_MAC(egfShtVlanAttrib_hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_VLAN_ATTRIBUTES_E].fieldsInfoPtr,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_PORT_ISOLATION_VLAN_CMD_E,
        hwPortIsolationMode);

    /* Mirror to egress analyzer port */
    U32_GET_FIELD_BY_ID_MAC(egfShtVlanAttrib_hwDataArr,
        PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_VLAN_ATTRIBUTES_E].fieldsInfoPtr,
        LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_EGRESS_MIRROR_TO_ANALYZER_INDEX_E,
        hwMirrorIndex);
    switch (hwPortIsolationMode)
    {
        case 0:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
            break;
        case 1:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E;
            break;
        case 2:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E;
            break;
        default:
            vlanInfoPtr->portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E;
    }

    if(hwMirrorIndex == 0)
    {
        /* ignore index */
        vlanInfoPtr->mirrToTxAnalyzerEn = GT_FALSE;
    }
    else
    {
        vlanInfoPtr->mirrToTxAnalyzerEn = GT_TRUE;
        vlanInfoPtr->mirrToTxAnalyzerIndex = hwMirrorIndex - 1;
    }
/**************** EGF_QAG section *********************************************/
    /* get the bmp of supported port on this device */
    prvCpssDrvPortsFullMaskGet(
        PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum),
            &existingPorts);

    /* tagCmd info for all ports */
    for (ii = 0; ii < CPSS_MAX_PORTS_NUM_CNS; ii++)
    {
        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&existingPorts,ii))
        {
            /* make sure to 'ignores' non exists ports */
            continue;
        }

        U32_GET_FIELD_IN_ENTRY_MAC(egfQag_hwDataArr,(ii*3), 3 ,value);

        if (tr101Supported == GT_FALSE)
        {
            if(value)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(portsTaggingPtr,ii);
            }
        }
        else
        {
            PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_VLAN_TAG_MODE_MAC(devNum, value,
                    portsTaggingCmdPtr->portsCmd[ii]);
        }
    }

    return GT_OK;
}



/*******************************************************************************
* vlanEntrySplitTableRead
*
* DESCRIPTION:
*       Read vlan entry. - for split table (ingress,egress)
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan -
*                             The parameter is relevant for DxCh1, DxCh2
*                             and DxCh3 devices.
*                             The parameter is relevant for xCat and
*                             above devices without TR101 feature support
*                             The parameter is not relevant and ignored for
*                             xCat and above devices with TR101 feature
*                             support.
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                                 GT_TRUE = Vlan is valid
*                                 GT_FALSE = Vlan is not Valid
*       portsTaggingCmdPtr  - (pointer to) ports tagging commands in the vlan -
*                             The parameter is relevant only for xCat and
*                             above with TR101 feature support.
*                             The parameter is not relevant and ignored for
*                             other devices.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT               - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS vlanEntrySplitTableRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
)
{
    GT_STATUS   rc;
    /* ingress HW format */
    GT_U32      ingressHwData[VLAN_INGRESS_ENTRY_WORDS_NUM_CNS];
    /* egress HW format */
    GT_U32      egressHwData[VLAN_EGRESS_ENTRY_WORDS_NUM_CNS];
    GT_U32      efgQag_egressHwData[EGF_QAG_EGRESS_VLAN_ENTRY_WORDS_NUM_CNS];/*Bobcat2: EGF_QAG info*/
    GT_BOOL     tr101Supported;   /* TR101 Feature Support */
    GT_U32      i;                /* iterator */
    GT_U32      egfShtVlanAttrib_hwDataArr[1]; /* SHT Egress Vlan Attribute table HW entry   */
    GT_U32      egfShtVlanSpanId_hwDataArr[1]; /* SHT Egress Vlan Span Tree Id table HW entry*/

    /* get TR101 feature support */
    tr101Supported = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    cpssOsMemSet(vlanInfoPtr,0,sizeof(*vlanInfoPtr));
    cpssOsMemSet(portsMembersPtr,0,sizeof(*portsMembersPtr));

    /* get TR101 feature support */
    if (tr101Supported != GT_FALSE)
    {
        cpssOsMemSet(portsTaggingCmdPtr,0,sizeof(*portsTaggingCmdPtr));
    }
    else
    {
        cpssOsMemSet(portsMembersPtr,0,sizeof(*portsTaggingPtr));
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* the ingress table looks the same on all port groups */

        /* read entry from HW ingress table */
        rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                   vlanId,/* vid */
                                   &ingressHwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* convert HW format to SW format */
        rc = lion3VlanHwEntryParseIngress(
                devNum,
                vlanId,
                &ingressHwData[0],
                &(portsMembersPtr->ports[0]),
                vlanInfoPtr,
                isValidPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read entry from HW EGF_SHT egress table */
        rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                   vlanId,/* vid */
                                   &egressHwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read entry from HW EGF_SHT egress Vlan Attributes table */
        rc = prvCpssDxChReadTableEntry(
            devNum,
            PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E,
            vlanId,/* vid */
            &egfShtVlanAttrib_hwDataArr[0]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read entry from HW EGF_SHT egress Vlan Span Tree Id table */
        rc = prvCpssDxChReadTableEntry(
            devNum,
            PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_SPANNING_E,
            vlanId,/* vid */
            &egfShtVlanSpanId_hwDataArr[0]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read entry from HW EGF_QAG egress table */
        rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E,
                                   vlanId,/* vid */
                                   &efgQag_egressHwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }


        /* convert HW format to SW format */
        rc = lion3VlanHwEntryParseEgress(devNum,
                  &egressHwData[0],         /*EGF_SHT info*/
                  &egfShtVlanAttrib_hwDataArr[0],
                  &egfShtVlanSpanId_hwDataArr[0],
                  &efgQag_egressHwData[0],  /*EGF_QAG info*/
                  portsMembersPtr,
                  portsTaggingPtr,
                  vlanInfoPtr,
                  isValidPtr,
                  portsTaggingCmdPtr);

        return rc;
    }
    else
    {
        /* loop on all TXQ units */
        for (i = 0; i < PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(devNum); i++)
        {
            if(GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
            {
                /* read entry from HW ingress table */
                rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                    i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                                           PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                           (GT_U32)vlanId,/* vid */
                                           &ingressHwData[0]);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* convert HW format to SW format */
                rc = prvCpssBrgVlanHwEntryParseIngress(
                          &ingressHwData[0],
                          &(portsMembersPtr->ports[i*PRV_CPSS_DXCH_PORT_WITH_CPU_BMP_NUM_IN_HEMISPHERE_CNS]),
                          vlanInfoPtr,
                          isValidPtr);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            /* read entry from HW egress table */
            rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                                       PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                       (GT_U32)vlanId,/* vid */
                                       &egressHwData[0]);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* convert HW format to SW format */
            rc = prvCpssBrgVlanHwEntryParseEgress(devNum,
                      &egressHwData[0],
                      &(portsMembersPtr->ports[i*PRV_CPSS_DXCH_PORT_WITH_CPU_BMP_NUM_IN_HEMISPHERE_CNS]),
                      &(portsTaggingPtr->ports[i*PRV_CPSS_DXCH_PORT_WITH_CPU_BMP_NUM_IN_HEMISPHERE_CNS]),
                      vlanInfoPtr,
                      isValidPtr,
                      &(portsTaggingCmdPtr->portsCmd[i*PRV_CPSS_DXCH_PORT_WITH_CPU_NUM_IN_HEMISPHERE_CNS]));

            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if (GT_FALSE != PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                info_PRV_CPSS_DXCH_LION2_MC_BANDWIDTH_RESTRICTION_WA_E.enabled)
        {
            /* port 12 on each hemisphere is used to duplicate the packet to the other hemisphere;
               this port is set internally so it should be cleared upon reading the entry */
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(portsMembersPtr,12);      /* 1st hemisphere -> port 12 */
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(portsMembersPtr,(64+12)); /* 2nd hemisphere -> port 76 */
        }

        return GT_OK;
    }
}


/*******************************************************************************
* internal_cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*
* DESCRIPTION:
*       Set Tag1 removal mode from the egress port Tag State if Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       mode          - Vlan Tag1 Removal mode when Tag1 VID=0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     mode
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value   */
    GT_U32  regValueB1; /* register value for Lion revision 3 and above */
    GT_U32  regValEArch;/* register for Bobcat2, Caelum, Bobcat3 */
    GT_U32  regOffset;  /* offset in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        if( PRV_CPSS_PP_MAC(devNum)->revision <= 2 )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
        }
    }

    switch (mode)
    {
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E:
            regValue = regValueB1 = 0;
            regValEArch = 0;
            break;
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_AND_INGRESS_WITHOUT_TAG1_E:
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E:
            /* NOTE: as the functionality was slightly changed in e_ARCH devices
               the enum of  CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_AND_INGRESS_WITHOUT_TAG1_E
               should be used for those devices instead of
               CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E */
            if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE) &&
                (mode == CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_AND_INGRESS_WITHOUT_TAG1_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            else
            if ((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) &&
                (mode == CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            regValue = 1;
            regValueB1 = 2;
            regValEArch = 2;
            break;
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E:
            regValue = regValueB1 = 3;
            regValEArch = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* NOTE: PRV_CPSS_PP_MAC(devNum)->revision > 2 */

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.global.debug.metalFix;
        regOffset = 22;

        regValue = regValueB1;
    }
    else if (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.global.debug.metalFix;
        regOffset = 22;
    }
    else if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_EGF_QAG_MAC(devNum).
                distributor.distributorGeneralConfigs;
        regOffset = 13;
        regValue = regValEArch;
    }
    else if ((PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE) && (CPSS_PP_FAMILY_DXCH_XCAT3_E != PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        regOffset = 17;
    }

    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
        regOffset = 6;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, regOffset, 2, regValue);
}

/*******************************************************************************
* cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*
* DESCRIPTION:
*       Set Tag1 removal mode from the egress port Tag State if Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       mode          - Vlan Tag1 Removal mode when Tag1 VID=0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
*
* DESCRIPTION:
*       Get Tag1 removal mode from the egress port Tag State when Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - (pointer to) Vlan Tag1 Removal mode when Tag1 VID=0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_BAD_STATE             - on bad state of register
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     *modePtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regOffset;  /* offset in register */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        if( PRV_CPSS_PP_MAC(devNum)->revision > 2 )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.dq.global.debug.metalFix;
            regOffset = 22;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
        }
    }
    else if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.dq.global.debug.metalFix;
        regOffset = 22;
    }
    else if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_EGF_QAG_MAC(devNum).
                distributor.distributorGeneralConfigs;
        regOffset = 13;
    }
    else if ((PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE) && (CPSS_PP_FAMILY_DXCH_XCAT3_E != PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        regOffset = 17;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
        regOffset = 6;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, regOffset, 2, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        switch (regValue)
        {
            case 0:
                *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E;
                break;
            case 1:
                *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E;
                break;
            case 2:
                *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_AND_INGRESS_WITHOUT_TAG1_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        switch (regValue)
        {
            case 0:
                *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E;
                break;
            case 1:
            case 2:
                if(((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily) &&
                    (1 == regValue)) ||
                   ((CPSS_PP_FAMILY_DXCH_LION_E != PRV_CPSS_PP_MAC(devNum)->devFamily) &&
                    (2 == regValue )))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
                *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E;
                break;
            case 3:
                *modePtr = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E;
                break;
            default:
                /* no chance getting here */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
*
* DESCRIPTION:
*       Get Tag1 removal mode from the egress port Tag State when Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - (pointer to) Vlan Tag1 Removal mode when Tag1 VID=0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_BAD_STATE             - on bad state of register
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanForceNewDsaToCpuEnableSet
*
* DESCRIPTION:
*       Enable / Disable preserving the original VLAN tag
*       and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
*       enable        - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanForceNewDsaToCpuEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    data = (enable == GT_TRUE) ? 1 : 0;

    /*Note:in Bobcat2; Caelum; Bobcat3 the configuration is 'per physical port'*/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
                                              CPSS_CPU_PORT_NUM_CNS,/*global port*/
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                              LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_FORCE_NEW_DSA_TO_CPU_E, /* field name */
                                              PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                              data);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

        /* Enable / Disable preserving the original VLAN tag  */
        return prvCpssHwPpSetRegField(devNum, regAddr, 17, 1, data);
    }

}

/*******************************************************************************
* cpssDxChBrgVlanForceNewDsaToCpuEnableSet
*
* DESCRIPTION:
*       Enable / Disable preserving the original VLAN tag
*       and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
*       enable        - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForceNewDsaToCpuEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanForceNewDsaToCpuEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgVlanForceNewDsaToCpuEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanForceNewDsaToCpuEnableGet
*
* DESCRIPTION:
*       Get Force New DSA To Cpu mode for enabling / disabling
*       preserving the original VLAN tag and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
* OUTPUTS:
*       enablePtr     - pointer to Force New DSA To Cpu mode.
*                     - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanForceNewDsaToCpuEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    data;        /* reg sub field data */
    GT_STATUS rc;          /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /*Note:in Bobcat2; Caelum; Bobcat3 the configuration is 'per physical port'*/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
                                              CPSS_CPU_PORT_NUM_CNS,/*global port*/
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                              LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_FORCE_NEW_DSA_TO_CPU_E, /* field name */
                                              PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                              &data);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

        /* Get Force New DSA To Cpu mode for enabling / disabling
           preserving the original VLAN tag */
        rc =  prvCpssHwPpGetRegField(devNum, regAddr, 17, 1, &data);
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanForceNewDsaToCpuEnableGet
*
* DESCRIPTION:
*       Get Force New DSA To Cpu mode for enabling / disabling
*       preserving the original VLAN tag and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
* OUTPUTS:
*       enablePtr     - pointer to Force New DSA To Cpu mode.
*                     - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForceNewDsaToCpuEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanForceNewDsaToCpuEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgVlanForceNewDsaToCpuEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanKeepVlan1EnableSet
*
* DESCRIPTION:
*       Enable/Disable keeping VLAN1 in the packet, for packets received with VLAN1,
*       even-though the tag-state of this egress-port is configured in the
*       VLAN-table to "untagged" or "VLAN0".
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority (APPLICABLE RANGES: 0..7)
*       enable          - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanKeepVlan1EnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U8                       up,
    IN  GT_BOOL                     enable
)
{
    GT_U32    regAddr;     /* register address                              */
    GT_U32    data;        /* reg sub field data                            */
    GT_U32    regOffset;   /* register offset                               */
    GT_U32    fieldOffset; /* field offset inside register                  */
    GT_U32    portGroupId; /* the port group Id - support
                              multi-port-groups device                      */
    GT_U32     localPort;  /* local port - support multi-port-groups device  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);

    data = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

        return prvCpssDxChWriteTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                                              portNum,/*global port*/
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_SUB_FIELD_IN_FIELD_NAME_CNS,
                                              LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PER_UP0_KEEP_VLAN1_ENABLE_E, /* field name */
                                              PRV_CPSS_DXCH_TABLE_SUB_FIELD_OFFSET_AND_LENGTH_MAC(up,1),/* sub field info {offset,length}*/
                                              data);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        regOffset = (localPort / 4);
        fieldOffset = ((localPort % 4) * 8) + up;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.keepVlan1Reg[regOffset];

        return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                              fieldOffset, 1, data);
    }
}

/*******************************************************************************
* cpssDxChBrgVlanKeepVlan1EnableSet
*
* DESCRIPTION:
*       Enable/Disable keeping VLAN1 in the packet, for packets received with VLAN1,
*       even-though the tag-state of this egress-port is configured in the
*       VLAN-table to "untagged" or "VLAN0".
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority (APPLICABLE RANGES: 0..7)
*       enable          - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanKeepVlan1EnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U8                       up,
    IN  GT_BOOL                     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanKeepVlan1EnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, up, enable));

    rc = internal_cpssDxChBrgVlanKeepVlan1EnableSet(devNum, portNum, up, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, up, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanKeepVlan1EnableGet
*
* DESCRIPTION:
*       Get status of keeping VLAN1 in the packet, for packets received with VLAN1.
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       enablePtr       - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanKeepVlan1EnableGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U8                       up,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_U32    regAddr;     /* register address                              */
    GT_U32    data;        /* reg sub field data                            */
    GT_U32    regOffset;   /* register offset                               */
    GT_U32    fieldOffset; /* field offset inside register                  */
    GT_U32    portGroupId; /* the port group Id - support
                              multi-port-groups device                      */
    GT_U32     localPort;   /* local port - support multi-port-groups device */
    GT_STATUS rc;          /* return status                                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(up);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

        rc = prvCpssDxChReadTableEntryField(devNum,
                                              PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                                              portNum,/*global port*/
                                              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_SUB_FIELD_IN_FIELD_NAME_CNS,
                                              LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PER_UP0_KEEP_VLAN1_ENABLE_E, /* field name */
                                              PRV_CPSS_DXCH_TABLE_SUB_FIELD_OFFSET_AND_LENGTH_MAC(up,1),/* sub field info {offset,length}*/
                                              &data);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        regOffset = (localPort / 4);
        fieldOffset = ((localPort % 4) * 8) + up;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.keepVlan1Reg[regOffset];

        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                fieldOffset, 1, &data);
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanKeepVlan1EnableGet
*
* DESCRIPTION:
*       Get status of keeping VLAN1 in the packet, for packets received with VLAN1.
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       enablePtr       - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanKeepVlan1EnableGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U8                       up,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanKeepVlan1EnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, up, enablePtr));

    rc = internal_cpssDxChBrgVlanKeepVlan1EnableGet(devNum, portNum, up, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, up, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet
*
* DESCRIPTION:
*       Enable Learning Of Original Tag1 VID Per Port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - enable learning
*                 GT_FALSE - disable learning
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_U32    hwData;      /* value to write */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    hwData = BOOL2BIT_MAC(enable);

    /* set the <EN_LEARN_ORIG_TAG1_VID>*/
    return prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_EPORT_TABLE_FIELDS_EN_LEARN_ORIG_TAG1_VID_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    hwData);
}

/*******************************************************************************
* cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet
*
* DESCRIPTION:
*       Enable Learning Of Original Tag1 VID Per Port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - enable learning
*                 GT_FALSE - disable learning
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet
*
* DESCRIPTION:
*       Get Learning mode Of Original Tag1 VID Per Port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                 GT_TRUE  - enable learning
*                 GT_FALSE - disable learning
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    GT_U32    hwData;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* get the <EN_LEARN_ORIG_TAG1_VID>*/
    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_EPORT_TABLE_FIELDS_EN_LEARN_ORIG_TAG1_VID_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &hwData);
    if(rc!=GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet
*
* DESCRIPTION:
*       Get Learning mode Of Original Tag1 VID Per Port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                 GT_TRUE  - enable learning
*                 GT_FALSE - disable learning
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortEnableLearningOfOriginalTag1VidGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet
*
* DESCRIPTION:
*       When enabled, IPv4 MC/BC packets in this eVLAN are mirrored to the
*       analyzer specified in this field. This is independent of the analyzer
*       configuration of other traffic in this eVLAN.
*       0x0 is a reserved value that means Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - IPv4 MC/BC packets in this eVLAN are mirrored to the
*                 analyzer specified in this field
*                 GT_FALSE - IPv4 MC/BC packets in this eVLAN are NOT mirrored
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_STATUS rc;
    GT_U32    hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(enable == GT_FALSE) /* ignore index and set 0 */
    {
        hwData = 0;
    }
    else
    {
        if(index >  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        hwData = index + 1;
    }

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                       (GT_U32)vlanId,
                                       PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                       LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MC_BC_TO_MIRROR_ANLYZER_IDX_E,
                                       PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                       hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet
*
* DESCRIPTION:
*       When enabled, IPv4 MC/BC packets in this eVLAN are mirrored to the
*       analyzer specified in this field. This is independent of the analyzer
*       configuration of other traffic in this eVLAN.
*       0x0 is a reserved value that means Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - IPv4 MC/BC packets in this eVLAN are mirrored to the
*                 analyzer specified in this field
*                 GT_FALSE - IPv4 MC/BC packets in this eVLAN are NOT mirrored
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable, index));

    rc = internal_cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexSet(devNum, vlanId, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet
*
* DESCRIPTION:
*       Get Ipv4 MC/BC Mirror To Analyzer Index
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                   GT_TRUE - IPv4 MC/BC packets in this eVLAN are mirrored to the
*                   analyzer specified in this field
*                   GT_FALSE - IPv4 MC/BC packets in this eVLAN are NOT mirrored
*       indexPtr   - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{
    GT_STATUS rc;
    GT_U32    hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

     rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                       (GT_U32)vlanId,
                                       PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                       LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MC_BC_TO_MIRROR_ANLYZER_IDX_E,
                                       PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                       &hwData);

     if(rc != GT_OK)
         return rc;

    if(hwData == 0) /* ignore index and set 0 */
    {
        *enablePtr = GT_FALSE;
        *indexPtr  = 0;
    }
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = hwData-1;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet
*
* DESCRIPTION:
*       Get Ipv4 MC/BC Mirror To Analyzer Index
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                   GT_TRUE - IPv4 MC/BC packets in this eVLAN are mirrored to the
*                   analyzer specified in this field
*                   GT_FALSE - IPv4 MC/BC packets in this eVLAN are NOT mirrored
*       indexPtr   - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enablePtr, indexPtr));

    rc = internal_cpssDxChBrgVlanIpv4McBcMirrorToAnalyzerIndexGet(devNum, vlanId, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet
*
* DESCRIPTION:
*       When enabled, IPv6 MC packets in this eVLAN are mirrored to the
*       analyzer. This is independent of the analyzer configuration of other
*       traffic in this eVLAN.
*       0x0 is a reserved value that means Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - IPv6 MC packets in this eVLAN are mirrored to the
*                 analyzer specified in this field
*                 GT_FALSE - IPv6 MC packets in this eVLAN are NOT mirrored to the
*                 analyzer specified in this field
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_STATUS rc;
    GT_U32    hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(enable == GT_FALSE) /* ignore index and set 0 */
    {
        hwData = 0;
    }
    else
    {
        if(index >  PRV_CPSS_DXCH_MIRROR_TO_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        hwData = index + 1;
    }

    /* set the <IPV6_MC_TO_MIRROR_ANALYZER_IDX>*/
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                    vlanId,/* vid */
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MC_TO_MIRROR_ANALYZER_IDX_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet
*
* DESCRIPTION:
*       When enabled, IPv6 MC packets in this eVLAN are mirrored to the
*       analyzer. This is independent of the analyzer configuration of other
*       traffic in this eVLAN.
*       0x0 is a reserved value that means Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - IPv6 MC packets in this eVLAN are mirrored to the
*                 analyzer specified in this field
*                 GT_FALSE - IPv6 MC packets in this eVLAN are NOT mirrored to the
*                 analyzer specified in this field
*       index   - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable,
    IN GT_U32  index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable, index));

    rc = internal_cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexSet(devNum, vlanId, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet
*
* DESCRIPTION:
*       Get Ipv6 MC Mirror To Analyzer Index
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                   GT_TRUE - IPv6 MC packets in this eVLAN are mirrored to the
*                   analyzer specified in this field
*                   GT_FALSE - IPv6 MC packets in this eVLAN are NOT mirrored to the
*                   analyzer specified in this field
*       indexPtr   - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{
    GT_STATUS rc;
    GT_U32    hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* get the <IPV6_MC_TO_MIRROR_ANALYZER_IDX>*/
    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                    vlanId,/* vid */
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MC_TO_MIRROR_ANALYZER_IDX_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &hwData);

     if(rc != GT_OK)
         return rc;

    if(hwData == 0) /* ignore index and set 0 */
    {
        *enablePtr = GT_FALSE;
        *indexPtr  = 0;
    }
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = hwData-1;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet
*
* DESCRIPTION:
*       Get Ipv6 MC Mirror To Analyzer Index
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                   GT_TRUE - IPv6 MC packets in this eVLAN are mirrored to the
*                   analyzer specified in this field
*                   GT_FALSE - IPv6 MC packets in this eVLAN are NOT mirrored to the
*                   analyzer specified in this field
*       indexPtr   - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enablePtr, indexPtr));

    rc = internal_cpssDxChBrgVlanIpv6McMirrorToAnalyzerIndexGet(devNum, vlanId, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanForwardingIdSet
*
* DESCRIPTION:
*       Set Forwarding ID value.
*       Used for virtual birdges per eVLAN.
*       Used by the bridge engine for entry lookup and entry match (replaces VID
*       in bridge entry)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       fidValue - Forwarding ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanForwardingIdSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_U32  fidValue
)
{
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(fidValue > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FID_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* set the <FID>*/
    return prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                    vlanId,/* vid */
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FID_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    fidValue);
}

/*******************************************************************************
* cpssDxChBrgVlanForwardingIdSet
*
* DESCRIPTION:
*       Set Forwarding ID value.
*       Used for virtual birdges per eVLAN.
*       Used by the bridge engine for entry lookup and entry match (replaces VID
*       in bridge entry)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       fidValue - Forwarding ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForwardingIdSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_U32  fidValue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanForwardingIdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, fidValue));

    rc = internal_cpssDxChBrgVlanForwardingIdSet(devNum, vlanId, fidValue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, fidValue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChBrgVlanForwardingIdGet
*
* DESCRIPTION:
*       Get Forwarding ID value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       fidValuePtr - (pointer to) Forwarding ID value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanForwardingIdGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_U32  *fidValuePtr
)
{
    GT_STATUS rc;
    GT_U32    hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(fidValuePtr);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* get the <FID>*/
    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                    vlanId,/* vid */
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FID_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &hwData);
    if(rc != GT_OK)
        return rc;

    *fidValuePtr = hwData;

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanForwardingIdGet
*
* DESCRIPTION:
*       Get Forwarding ID value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*
* OUTPUTS:
*       fidValuePtr - (pointer to) Forwarding ID value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanForwardingIdGet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vlanId,
    OUT GT_U32  *fidValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanForwardingIdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, fidValuePtr));

    rc = internal_cpssDxChBrgVlanForwardingIdGet(devNum, vlanId, fidValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, fidValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanUnknownMacSaCommandSet
*
* DESCRIPTION:
*       Set bridge forwarding decision for packets with unknown Source Address.
*       Only relevant in controlled address learning mode, when <Auto-learning
*       enable> = Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID
*       cmd          - supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_HARD_E
*                      CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanUnknownMacSaCommandSet
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN CPSS_PACKET_CMD_ENT  cmd
)
{
    GT_U32 hwData=0;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(hwData, cmd);

    /* set the <UNKOWN_MAC_SA>*/
    return prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                    vlanId,/* vid */
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKOWN_MAC_SA_CMD_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    hwData);

}

/*******************************************************************************
* cpssDxChBrgVlanUnknownMacSaCommandSet
*
* DESCRIPTION:
*       Set bridge forwarding decision for packets with unknown Source Address.
*       Only relevant in controlled address learning mode, when <Auto-learning
*       enable> = Disabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID
*       cmd          - supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_HARD_E
*                      CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUnknownMacSaCommandSet
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN CPSS_PACKET_CMD_ENT  cmd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanUnknownMacSaCommandSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, cmd));

    rc = internal_cpssDxChBrgVlanUnknownMacSaCommandSet(devNum, vlanId, cmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, cmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChBrgVlanUnknownMacSaCommandGet
*
* DESCRIPTION:
*       Get bridge forwarding decision for packets with unknown Source Address.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID
*
* OUTPUTS:
*       cmdPtr   - (pointer to) supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_HARD_E
*                      CPSS_PACKET_CMD_DROP_SOFT_E
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanUnknownMacSaCommandGet
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    OUT CPSS_PACKET_CMD_ENT *cmdPtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(cmdPtr);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* get the <UNKOWN_MAC_SA>*/
    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                    vlanId,/* vid */
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKOWN_MAC_SA_CMD_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &hwData);
    if(rc != GT_OK)
        return rc;

    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*cmdPtr, hwData);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanUnknownMacSaCommandGet
*
* DESCRIPTION:
*       Get bridge forwarding decision for packets with unknown Source Address.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN ID
*
* OUTPUTS:
*       cmdPtr   - (pointer to) supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_HARD_E
*                      CPSS_PACKET_CMD_DROP_SOFT_E
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUnknownMacSaCommandGet
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    OUT CPSS_PACKET_CMD_ENT *cmdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanUnknownMacSaCommandGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, cmdPtr));

    rc = internal_cpssDxChBrgVlanUnknownMacSaCommandGet(devNum, vlanId, cmdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, cmdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChBrgVlanEgressPortTagStateModeSet
*
* DESCRIPTION:
*       Set the egress port tag state mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       stateMode   - state mode: eVLAN or ePort
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEgressPortTagStateModeSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT      stateMode
)
{
    GT_U32      value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (stateMode)
    {
        case CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EVLAN_MODE_E:
            value = 0;
            break;
        case CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EPORT_MODE_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EPORT_TAG_STATE_MODE_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   value);
}

/*******************************************************************************
* cpssDxChBrgVlanEgressPortTagStateModeSet
*
* DESCRIPTION:
*       Set the egress port tag state mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       stateMode   - state mode: eVLAN or ePort
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressPortTagStateModeSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT      stateMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEgressPortTagStateModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, stateMode));

    rc = internal_cpssDxChBrgVlanEgressPortTagStateModeSet(devNum, portNum, stateMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, stateMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEgressPortTagStateModeGet
*
* DESCRIPTION:
*       Get the egress port tag state mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       stateModePtr   - (pointer to) state mode: eVLAN or ePort
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEgressPortTagStateModeGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    OUT CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT         *stateModePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(stateModePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EPORT_TAG_STATE_MODE_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &value);

    switch (value)
    {
        case 0:
            *stateModePtr = CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EVLAN_MODE_E;
            break;
        case 1:
            *stateModePtr = CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_EPORT_MODE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanEgressPortTagStateModeGet
*
* DESCRIPTION:
*       Get the egress port tag state mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       stateModePtr   - (pointer to) state mode: eVLAN or ePort
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressPortTagStateModeGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    OUT CPSS_DXCH_BRG_VLAN_EPORT_TAG_STATE_MODE_ENT         *stateModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEgressPortTagStateModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, stateModePtr));

    rc = internal_cpssDxChBrgVlanEgressPortTagStateModeGet(devNum, portNum, stateModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, stateModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanEgressPortTagStateSet
*
* DESCRIPTION:
*       Determines the egress port state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tagCmd      - Port tag state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEgressPortTagStateSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT              tagCmd
)
{
    GT_U32      hwDataTagCmd;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_CONVERT_VLAN_TAG_MODE_TO_HW_VAL_MAC(devNum, hwDataTagCmd,tagCmd);

    return prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EPORT_TAG_STATE_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   hwDataTagCmd);
}

/*******************************************************************************
* cpssDxChBrgVlanEgressPortTagStateSet
*
* DESCRIPTION:
*       Determines the egress port state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tagCmd      - Port tag state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressPortTagStateSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT              tagCmd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEgressPortTagStateSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tagCmd));

    rc = internal_cpssDxChBrgVlanEgressPortTagStateSet(devNum, portNum, tagCmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tagCmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEgressPortTagStateGet
*
* DESCRIPTION:
*       Returns the egress port state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       tagCmdPtr   - (pointer to) Port tag state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEgressPortTagStateGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    OUT CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT                 *tagCmdPtr
)
{
    GT_U32      value;
    GT_U32      hwValue;    /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(tagCmdPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EPORT_TAG_STATE_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &hwValue);

   PRV_CPSS_DXCH_CONVERT_HW_VAL_TO_VLAN_TAG_MODE_MAC(devNum, hwValue, value);
   *tagCmdPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanEgressPortTagStateGet
*
* DESCRIPTION:
*       Returns the egress port state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       tagCmdPtr   - (pointer to) Port tag state
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressPortTagStateGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PORT_NUM                                         portNum,
    OUT CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT                 *tagCmdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEgressPortTagStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tagCmdPtr));

    rc = internal_cpssDxChBrgVlanEgressPortTagStateGet(devNum, portNum, tagCmdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tagCmdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortPushVlanCommandSet
*
* DESCRIPTION:
*       Set Push Vlan Command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       vlanCmd - vlan command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortPushVlanCommandSet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_NUM                                 portNum,
    IN  CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT    vlanCmd
)
{
    GT_STATUS   rc;
    GT_U32      value;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(vlanCmd)
    {
        case CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_NONE_E:
            value = 0;
            break;
        case CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_TUNNEL_E:
            value = 1;
            break;
        case CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_PASSENGER_E:
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
                value = 2;
            else
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_PUSH_VLAN_COMMAND_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortPushVlanCommandSet
*
* DESCRIPTION:
*       Set Push Vlan Command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       vlanCmd - vlan command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushVlanCommandSet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_NUM                                 portNum,
    IN  CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT    vlanCmd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortPushVlanCommandSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, vlanCmd));

    rc = internal_cpssDxChBrgVlanPortPushVlanCommandSet(devNum, portNum, vlanCmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, vlanCmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortPushVlanCommandGet
*
* DESCRIPTION:
*       Get Push Vlan Command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       vlanCmdPtr - (pointer to)  push vlan command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortPushVlanCommandGet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_NUM                                 portNum,
    OUT CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT    *vlanCmdPtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(vlanCmdPtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_PUSH_VLAN_COMMAND_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    if(rc != GT_OK)
        return rc;

    switch(value)
    {
    case 0:
        *vlanCmdPtr = CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_NONE_E;
        break;
    case 1:
        *vlanCmdPtr = CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_TUNNEL_E;
        break;
    case 2:
        *vlanCmdPtr = CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_PUSH_SINGLE_TAG_ON_PASSENGER_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortPushVlanCommandGet
*
* DESCRIPTION:
*       Get Push Vlan Command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       vlanCmdPtr - (pointer to)  push vlan command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushVlanCommandGet
(
    IN  GT_U8                                       devNum,
    IN  GT_PORT_NUM                                 portNum,
    OUT CPSS_DXCH_BRG_VLAN_PUSH_VLAN_COMMAND_ENT    *vlanCmdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortPushVlanCommandGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, vlanCmdPtr));

    rc = internal_cpssDxChBrgVlanPortPushVlanCommandGet(devNum, portNum, vlanCmdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, vlanCmdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet
*
* DESCRIPTION:
*       Set TPID select table index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       tpidEntryIndex      - TPID select table index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           tpidEntryIndex
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(tpidEntryIndex >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_PUSHED_TAG_TPID_SELECT_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        tpidEntryIndex);
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet
*
* DESCRIPTION:
*       Set TPID select table index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       tpidEntryIndex      - TPID select table index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           tpidEntryIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tpidEntryIndex));

    rc = internal_cpssDxChBrgVlanPortPushedTagTpidSelectEgressSet(devNum, portNum, tpidEntryIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tpidEntryIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet
*
* DESCRIPTION:
*       Get TPID select table index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       tpidEntryIndexPtr       - (pointer to) TPID select table index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *tpidEntryIndexPtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(tpidEntryIndexPtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_PUSHED_TAG_TPID_SELECT_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    if(rc != GT_OK)
        return rc;

    *tpidEntryIndexPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet
*
* DESCRIPTION:
*       Get TPID select table index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       tpidEntryIndexPtr       - (pointer to) TPID select table index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *tpidEntryIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tpidEntryIndexPtr));

    rc = internal_cpssDxChBrgVlanPortPushedTagTpidSelectEgressGet(devNum, portNum, tpidEntryIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tpidEntryIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortPushedTagValueSet
*
* DESCRIPTION:
*       Set the tag value to push
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tagValue    - tag value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortPushedTagValueSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U16           tagValue
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(tagValue >= BIT_12)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_PUSHED_TAG_VALUE_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        tagValue);
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortPushedTagValueSet
*
* DESCRIPTION:
*       Set the tag value to push
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       tagValue    - tag value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushedTagValueSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U16           tagValue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortPushedTagValueSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tagValue));

    rc = internal_cpssDxChBrgVlanPortPushedTagValueSet(devNum, portNum, tagValue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tagValue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortPushedTagValueGet
*
* DESCRIPTION:
*       Get the tag value to push
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       tagValuePtr   - (pointer to) tag value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortPushedTagValueGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U16          *tagValuePtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(tagValuePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_PUSHED_TAG_VALUE_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    if(rc != GT_OK)
        return rc;

    *tagValuePtr = (GT_U16)value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortPushedTagValueGet
*
* DESCRIPTION:
*       Get the tag value to push
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       tagValuePtr   - (pointer to) tag value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortPushedTagValueGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U16          *tagValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortPushedTagValueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tagValuePtr));

    rc = internal_cpssDxChBrgVlanPortPushedTagValueGet(devNum, portNum, tagValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tagValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet
*
* DESCRIPTION:
*       Determines if UP and CFI values will be assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - Enable/Disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_UP_CFI_ASSIGNMENT_COMMAND_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        BOOL2BIT_MAC(enable));
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet
*
* DESCRIPTION:
*       Determines if UP and CFI values will be assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - Enable/Disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortUpAndCfiAssignmentCommandSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet
*
* DESCRIPTION:
*       Get if UP and CFI values will be assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) Enable/Disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_UP_CFI_ASSIGNMENT_COMMAND_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet
*
* DESCRIPTION:
*       Get if UP and CFI values will be assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) Enable/Disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortUpAndCfiAssignmentCommandGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUpSet
*
* DESCRIPTION:
*       Set UP value to assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       up          - UP value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUpSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    if(up >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_UP_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        up);
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortUpSet
*
* DESCRIPTION:
*       Set UP value to assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       up          - UP value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUpSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUpSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, up));

    rc = internal_cpssDxChBrgVlanPortUpSet(devNum, portNum, up);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, up));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUpGet
*
* DESCRIPTION:
*       Get UP value to assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       upPtr         - (pointer to)  UP value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUpGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *upPtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(upPtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_UP_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    if(rc != GT_OK)
        return rc;

    *upPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortUpGet
*
* DESCRIPTION:
*       Get UP value to assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       upPtr         - (pointer to)  UP value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUpGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *upPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUpGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, upPtr));

    rc = internal_cpssDxChBrgVlanPortUpGet(devNum, portNum, upPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, upPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortCfiEnableSet
*
* DESCRIPTION:
*       Set CFI value to assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - CFI value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortCfiEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_CFI_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        BOOL2BIT_MAC(enable));
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortCfiEnableSet
*
* DESCRIPTION:
*       Set CFI value to assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - CFI value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortCfiEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortCfiEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortCfiEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortCfiEnableGet
*
* DESCRIPTION:
*       Get CFI value to assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) CFI value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortCfiEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_HA_EPORT_TABLE_2_CFI_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortCfiEnableGet
*
* DESCRIPTION:
*       Get CFI value to assign
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) CFI value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortCfiEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortCfiEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortCfiEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortNumOfTagWordsToPopSet
*
* DESCRIPTION:
*       Set the number of bytes to pop for traffic that ingress from the port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number
*       numBytesToPop     - enumeration to set the number of bytes to pop.
*                           (for popping 0/4/8/6 bytes )
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortNumOfTagWordsToPopSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT numBytesToPop
)
{
    GT_STATUS   rc;
    GT_U32      hwValue;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    switch(numBytesToPop)
    {
        case GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_NONE_E:
            hwValue = 0;
            break;
        case GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_4_E:
            hwValue = 1;
            break;
        case GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_8_E:
            hwValue = 2;
            break;
        case GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_6_E:
            if (!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            hwValue = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
                                         portNum,
                                         PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                         LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_NUM_OF_TAGS_TO_POP_E, /* field name */
                                         PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                         hwValue);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortNumOfTagWordsToPopSet
*
* DESCRIPTION:
*       Set the number of bytes to pop for traffic that ingress from the port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number
*       numBytesToPop     - enumeration to set the number of bytes to pop.
*                           (for popping 0/4/8/6 bytes )
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortNumOfTagWordsToPopSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT numBytesToPop
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortNumOfTagWordsToPopSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, numBytesToPop));

    rc = internal_cpssDxChBrgVlanPortNumOfTagWordsToPopSet(devNum, portNum, numBytesToPop);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, numBytesToPop));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortNumOfTagWordsToPopGet
*
* DESCRIPTION:
*       Get the number of bytes to pop for traffic that ingress from the port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       numBytesToPopPtr  - (pointer to) enumeration to set the number of bytes to pop.
*                           (for popping 0/4/8/6 bytes )
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on not supported value read from the HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortNumOfTagWordsToPopGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT  GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT *numBytesToPopPtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      hwValue;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(numBytesToPopPtr);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_NUM_OF_TAGS_TO_POP_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(hwValue)
    {
        case 0:
            *numBytesToPopPtr = GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_NONE_E;
            break;
        case 1:
            *numBytesToPopPtr = GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_4_E;
            break;
        case 2:
            *numBytesToPopPtr = GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_8_E;
            break;
        case 3:
            if (!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
            *numBytesToPopPtr = GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_6_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortNumOfTagWordsToPopGet
*
* DESCRIPTION:
*       Get the number of bytes to pop for traffic that ingress from the port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       numBytesToPopPtr  - (pointer to) enumeration to set the number of bytes to pop.
*                           (for popping 0/4/8/6 bytes )
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on not supported value read from the HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortNumOfTagWordsToPopGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT  GT_DXCH_BRG_VLAN_NUM_BYTES_TO_POP_ENT *numBytesToPopPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortNumOfTagWordsToPopGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, numBytesToPopPtr));

    rc = internal_cpssDxChBrgVlanPortNumOfTagWordsToPopGet(devNum, portNum, numBytesToPopPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, numBytesToPopPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortVid0CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing VID0 of the egress packet
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE: Packet VID0 is not changed.
*                 GT_TRUE:  Assign new VID0 to the egress packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVid0CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EVLAN_COMMAND_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    BOOL2BIT_MAC(enable));
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortVid0CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing VID0 of the egress packet
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE: Packet VID0 is not changed.
*                 GT_TRUE:  Assign new VID0 to the egress packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid0CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVid0CommandEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortVid0CommandEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortVid0CommandEnableGet
*
* DESCRIPTION:
*       Return if changing VID0 of the egress packet is enabled/disabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet VID0 is not changed.
*                   GT_TRUE:  Assign new VID0 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVid0CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EVLAN_COMMAND_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVid0CommandEnableGet
*
* DESCRIPTION:
*       Return if changing VID0 of the egress packet is enabled/disabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet VID0 is not changed.
*                   GT_TRUE:  Assign new VID0 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid0CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVid0CommandEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortVid0CommandEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortVid1CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing VID1 of the egress packet
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE: Packet VID1 is not changed.
*                 GT_TRUE:  Assign new VID1 to the egress packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVid1CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_VID1_COMMAND_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    BOOL2BIT_MAC(enable));
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortVid1CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing VID1 of the egress packet
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE: Packet VID1 is not changed.
*                 GT_TRUE:  Assign new VID1 to the egress packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid1CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVid1CommandEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortVid1CommandEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortVid1CommandEnableGet
*
* DESCRIPTION:
*       Return if changing VID1 of the egress packet is enabled/disabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet VID1 is not changed.
*                   GT_TRUE:  Assign new VID1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVid1CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_VID1_COMMAND_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVid1CommandEnableGet
*
* DESCRIPTION:
*       Return if changing VID1 of the egress packet is enabled/disabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet VID1 is not changed.
*                   GT_TRUE:  Assign new VID1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid1CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVid1CommandEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortVid1CommandEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUp0CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing UP1 of the egress packet
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE: Packet UP1 is not changed.
*                 GT_TRUE:  Assign new UP1 to the egress packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUp0CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP0_COMMAND_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    BOOL2BIT_MAC(enable));
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortUp0CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing UP1 of the egress packet
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE: Packet UP1 is not changed.
*                 GT_TRUE:  Assign new UP1 to the egress packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp0CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUp0CommandEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortUp0CommandEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUp0CommandEnableGet
*
* DESCRIPTION:
*       Return if changing UP0 of the egress packet is enabled/disabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet UP1 is not changed.
*                   GT_TRUE:  Assign new UP1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUp0CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP0_COMMAND_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &value);

    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortUp0CommandEnableGet
*
* DESCRIPTION:
*       Return if changing UP0 of the egress packet is enabled/disabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet UP1 is not changed.
*                   GT_TRUE:  Assign new UP1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp0CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUp0CommandEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortUp0CommandEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUp1CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing UP1 of the egress packet
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE: Packet UP1 is not changed.
*                 GT_TRUE:  Assign new UP1 to the egress packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUp1CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP1_COMMAND_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    BOOL2BIT_MAC(enable));
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortUp1CommandEnableSet
*
* DESCRIPTION:
*       Enable/Disable changing UP1 of the egress packet
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE: Packet UP1 is not changed.
*                 GT_TRUE:  Assign new UP1 to the egress packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp1CommandEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUp1CommandEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortUp1CommandEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUp1CommandEnableGet
*
* DESCRIPTION:
*       Return if changing UP1 of the egress packet is enabled/disabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet UP1 is not changed.
*                   GT_TRUE:  Assign new UP1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUp1CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP1_COMMAND_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortUp1CommandEnableGet
*
* DESCRIPTION:
*       Return if changing UP1 of the egress packet is enabled/disabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: Packet UP1 is not changed.
*                   GT_TRUE:  Assign new UP1 to the egress packet
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp1CommandEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUp1CommandEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortUp1CommandEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgVlanPortVid1Set
*
* DESCRIPTION:
*       For ingress direction : Set port's default Vid1.
*       For egress direction  : Set port's Vid1 when egress port <VID1 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort for ingress direction.
*       direction - ingress/egress direction
*       vid1    - VID1 value (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVid1Set
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    IN CPSS_DIRECTION_ENT   direction,
    IN  GT_U16              vid1
)
{
    GT_STATUS   rc;


    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(vid1);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

            /* write to pre-tti-lookup-ingress-eport table */
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                                portNum,
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_DEF_TAG1_VLAN_ID_E, /* field name */
                                                PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                vid1);
            return rc;

        case CPSS_DIRECTION_EGRESS_E:
            PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                                portNum,
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_VID1_E, /* field name */
                                                PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                vid1);
            return rc;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

}

/*******************************************************************************
* cpssDxChBrgVlanPortVid1Set
*
* DESCRIPTION:
*       For ingress direction : Set port's default Vid1.
*       For egress direction  : Set port's Vid1 when egress port <VID1 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort for ingress direction.
*       direction - ingress/egress direction
*       vid1    - VID1 value (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid1Set
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    IN CPSS_DIRECTION_ENT   direction,
    IN  GT_U16              vid1
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVid1Set);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, vid1));

    rc = internal_cpssDxChBrgVlanPortVid1Set(devNum, portNum, direction, vid1);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, vid1));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortVid1Get
*
* DESCRIPTION:
*       For ingress direction : Get port's default Vid1.
*       For egress direction  : Get port's Vid1 when egress port <VID1 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort for ingress direction.
*       direction - ingress/egress direction
*
* OUTPUTS:
*       vid1Ptr    - (pointer to)VID1 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortVid1Get
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    IN CPSS_DIRECTION_ENT   direction,
    OUT  GT_U16             *vid1Ptr
)
{
    GT_STATUS   rc;
    GT_U32      value;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(vid1Ptr);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

            /* read from pre-tti-lookup-ingress-eport table */
            rc = prvCpssDxChReadTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                                portNum,
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_DEF_TAG1_VLAN_ID_E, /* field name */
                                                PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                &value);
            break;
        case CPSS_DIRECTION_EGRESS_E:
            PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

            rc = prvCpssDxChReadTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                                portNum,
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_VID1_E, /* field name */
                                                PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                &value);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    *vid1Ptr = (GT_U16)value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVid1Get
*
* DESCRIPTION:
*       For ingress direction : Get port's default Vid1.
*       For egress direction  : Get port's Vid1 when egress port <VID1 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort for ingress direction.
*       direction - ingress/egress direction
*
* OUTPUTS:
*       vid1Ptr    - (pointer to)VID1 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortVid1Get
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    IN CPSS_DIRECTION_ENT   direction,
    OUT  GT_U16             *vid1Ptr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortVid1Get);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, vid1Ptr));

    rc = internal_cpssDxChBrgVlanPortVid1Get(devNum, portNum, direction, vid1Ptr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, vid1Ptr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUp0Set
*
* DESCRIPTION:
*       Set Up0 assigned to the egress packet if <UP0 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       up0     - UP0 value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUp0Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up0
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(up0 >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP0_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        up0);
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortUp0Set
*
* DESCRIPTION:
*       Set Up0 assigned to the egress packet if <UP0 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       up0     - UP0 value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp0Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up0
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUp0Set);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, up0));

    rc = internal_cpssDxChBrgVlanPortUp0Set(devNum, portNum, up0);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, up0));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUp0Get
*
* DESCRIPTION:
*       Return the UP0 assigned to the egress packet if <UP0 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       up0Ptr - (pointer to) UP0 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUp0Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *up0Ptr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(up0Ptr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP0_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    *up0Ptr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortUp0Get
*
* DESCRIPTION:
*       Return the UP0 assigned to the egress packet if <UP0 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       up0Ptr - (pointer to) UP0 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp0Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *up0Ptr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUp0Get);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, up0Ptr));

    rc = internal_cpssDxChBrgVlanPortUp0Get(devNum, portNum, up0Ptr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, up0Ptr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUp1Set
*
* DESCRIPTION:
*       Set Up1 assigned to the egress packet if <UP1 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       up1     - UP1 value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUp1Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up1
)
{
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(up1 >= BIT_3)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP1_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        up1);
    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanPortUp1Set
*
* DESCRIPTION:
*       Set Up1 assigned to the egress packet if <UP1 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       up1     - UP1 value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp1Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U32           up1
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUp1Set);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, up1));

    rc = internal_cpssDxChBrgVlanPortUp1Set(devNum, portNum, up1);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, up1));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortUp1Get
*
* DESCRIPTION:
*       Return the UP1 assigned to the egress packet if <UP1 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       up1Ptr - (pointer to) UP1 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortUp1Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *up1Ptr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(up1Ptr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP1_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    *up1Ptr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortUp1Get
*
* DESCRIPTION:
*       Return the UP1 assigned to the egress packet if <UP1 Command>=Enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       up1Ptr - (pointer to) UP1 value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortUp1Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *up1Ptr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortUp1Get);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, up1Ptr));

    rc = internal_cpssDxChBrgVlanPortUp1Get(devNum, portNum, up1Ptr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, up1Ptr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN Tag1 removing from packets arrived without VLAN Tag1.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       enable  - GT_FALSE - according to global configuration
*                            see cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*                 GT_TRUE  - Tag1 removed if original packet arrived without Tag1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS   rc;   /* return code */
    GT_U32      data; /* HW value */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    data = BOOL2BIT_MAC(enable);

    rc = prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
        portNum,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_REMOVE_VLAN_TAG_1_IF_RX_WITHOUT_TAG_1_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN Tag1 removing from packets arrived without VLAN Tag1.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       enable  - GT_FALSE - according to global configuration
*                            see cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*                 GT_TRUE  - Tag1 removed if original packet arrived without Tag1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of VLAN Tag1 removing from packets arrived without VLAN Tag1.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                 GT_FALSE - according to global configuration
*                            see cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*                 GT_TRUE  - Tag1 removed if original packet arrived without Tag1
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_STATUS   rc;   /* return code */
    GT_U32      data; /* HW value */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChReadTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
        portNum,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_REMOVE_VLAN_TAG_1_IF_RX_WITHOUT_TAG_1_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of VLAN Tag1 removing from packets arrived without VLAN Tag1.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                 GT_FALSE - according to global configuration
*                            see cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*                 GT_TRUE  - Tag1 removed if original packet arrived without Tag1
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgVlanPortRemoveTag1IfRxWithoutTag1EnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIndependentNonFloodVidxEnableSet
*
* DESCRIPTION:
*       Enable/Disable Independent non-flood VIDX
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_FALSE: VIDX portlist is always AND'ed with VID portlist
*                 GT_TRUE:  If packet has VIDX target and VIDX != 0xFFF
*                           (i.e. not flooded to all ports in the VLAN),
*                           then the VIDX portlist is the distribution portlist,
*                           regardless of the VID portlist.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIndependentNonFloodVidxEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_BOOL          enable
)
{
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssHwPpSetRegField(devNum,
        PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(devNum).global.SHTGlobalConfigs,
        4,1,BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChBrgVlanIndependentNonFloodVidxEnableSet
*
* DESCRIPTION:
*       Enable/Disable Independent non-flood VIDX
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_FALSE: VIDX portlist is always AND'ed with VID portlist
*                 GT_TRUE:  If packet has VIDX target and VIDX != 0xFFF
*                           (i.e. not flooded to all ports in the VLAN),
*                           then the VIDX portlist is the distribution portlist,
*                           regardless of the VID portlist.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIndependentNonFloodVidxEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIndependentNonFloodVidxEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgVlanIndependentNonFloodVidxEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanIndependentNonFloodVidxEnableGet
*
* DESCRIPTION:
*       Get Independent non-flood VIDX status.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: VIDX portlist is always AND'ed with VID portlist
*                   GT_TRUE:  If packet has VIDX target and VIDX != 0xFFF
*                           (i.e. not flooded to all ports in the VLAN),
*                           then the VIDX portlist is the distribution portlist,
*                           regardless of the VID portlist.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanIndependentNonFloodVidxEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssHwPpGetRegField(devNum,
        PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(devNum).global.SHTGlobalConfigs,
        4,1,&value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanIndependentNonFloodVidxEnableGet
*
* DESCRIPTION:
*       Get Independent non-flood VIDX status.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_FALSE: VIDX portlist is always AND'ed with VID portlist
*                   GT_TRUE:  If packet has VIDX target and VIDX != 0xFFF
*                           (i.e. not flooded to all ports in the VLAN),
*                           then the VIDX portlist is the distribution portlist,
*                           regardless of the VID portlist.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanIndependentNonFloodVidxEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanIndependentNonFloodVidxEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgVlanIndependentNonFloodVidxEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanFcoeForwardingEnableSet
*
* DESCRIPTION:
*       Set FCoE Forwarding Enable.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which FCoE Forwarding enabled/disabled..
*       enable  - FCoE Forwarding Enable
*                 GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanFcoeForwardingEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_U16           vlanId,
    IN  GT_BOOL          enable
)
{
    GT_STATUS   rc;      /* return code */
    GT_U32      hwData;  /* HW data     */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    hwData = BOOL2BIT_MAC(enable);

    rc = prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
        vlanId,/* vid */
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FCOE_FORWARDING_EN_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanFcoeForwardingEnableSet
*
* DESCRIPTION:
*       Set FCoE Forwarding Enable.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN ID which FCoE Forwarding enabled/disabled..
*       enable  - FCoE Forwarding Enable
*                 GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanFcoeForwardingEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_U16           vlanId,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanFcoeForwardingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, enable));

    rc = internal_cpssDxChBrgVlanFcoeForwardingEnableSet(devNum, vlanId, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanUnregisteredIpmEVidxSet
*
* DESCRIPTION:
*       Set Unregistered IPM eVidx Assignment Mode and Value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanId             - VLAN ID.
*       unregIpmEVidxMode  - Unregistered IPM eVidx Assignment Mode.
*       unregIpmEVidx      - Unregistered IPM eVidx Assignment Value.
*                            (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanUnregisteredIpmEVidxSet
(
    IN GT_U8                                        devNum,
    IN GT_U16                                       vlanId,
    IN CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT  unregIpmEVidxMode,
    IN GT_U32                                       unregIpmEVidx
)
{
    GT_STATUS   rc;      /* return code */
    GT_U32      hwData;  /* HW data     */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (unregIpmEVidx >= BIT_16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    PRV_CPSS_DXCH_CONVERT_UNREG_IPM_EVIDX_MODE_TO_HW_VAL_MAC(
        hwData, unregIpmEVidxMode);

    rc = prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
        vlanId,/* vid */
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_MODE_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
        vlanId,/* vid */
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        unregIpmEVidx);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanUnregisteredIpmEVidxSet
*
* DESCRIPTION:
*       Set Unregistered IPM eVidx Assignment Mode and Value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanId             - VLAN ID.
*       unregIpmEVidxMode  - Unregistered IPM eVidx Assignment Mode.
*       unregIpmEVidx      - Unregistered IPM eVidx Assignment Value.
*                            (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanUnregisteredIpmEVidxSet
(
    IN GT_U8                                        devNum,
    IN GT_U16                                       vlanId,
    IN CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_ENT  unregIpmEVidxMode,
    IN GT_U32                                       unregIpmEVidx
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanUnregisteredIpmEVidxSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, unregIpmEVidxMode, unregIpmEVidx));

    rc = internal_cpssDxChBrgVlanUnregisteredIpmEVidxSet(devNum, vlanId, unregIpmEVidxMode, unregIpmEVidx);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, unregIpmEVidxMode, unregIpmEVidx));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanMembersTableIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN Members table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       mode        - table access indexing mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanMembersTableIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
)
{
    GT_STATUS   rc = GT_BAD_PARAM;  /* return code */
    GT_U32      fieldValue;         /* register field value  */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E:
            fieldValue = 0;
            break;

        case CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E:
            fieldValue = 1;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if( (CPSS_DIRECTION_INGRESS_E == direction) ||
        (CPSS_DIRECTION_BOTH_E == direction) )
    {
        rc = prvCpssHwPpSetRegField(devNum,
                    PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                         bridgeEngineConfig.bridgeGlobalConfig1,
                                          26, 1, fieldValue);
        if( GT_OK != rc )
        {
            return rc;
        }
    }

    if( (CPSS_DIRECTION_EGRESS_E == direction) ||
        (CPSS_DIRECTION_BOTH_E == direction) )
    {
        rc = prvCpssHwPpSetRegField(devNum,
                    PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(devNum).
                                          global.SHTGlobalConfigs,
                                           7, 1, fieldValue);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanMembersTableIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN Members table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       mode        - table access indexing mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMembersTableIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanMembersTableIndexingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, mode));

    rc = internal_cpssDxChBrgVlanMembersTableIndexingModeSet(devNum, direction, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanMembersTableIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN Members table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanMembersTableIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      fieldValue; /* register field value  */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            rc = prvCpssHwPpGetRegField(devNum,
                  PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                         bridgeEngineConfig.bridgeGlobalConfig1,
                                          26, 1, &fieldValue);
            break;

        case CPSS_DIRECTION_EGRESS_E:
            rc = prvCpssHwPpGetRegField(devNum,
                  PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(devNum).
                                          global.SHTGlobalConfigs,
                                           7, 1, &fieldValue);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (fieldValue == 0x0) ?
                           CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E :
                           CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanMembersTableIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN Members table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanMembersTableIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanMembersTableIndexingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, modePtr));

    rc = internal_cpssDxChBrgVlanMembersTableIndexingModeGet(devNum, direction, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanStgIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN table to fetch span state
*       group.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       mode        - table access indexing mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanStgIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
)
{
    GT_STATUS   rc = GT_BAD_PARAM;  /* return code */
    GT_U32      fieldValue;         /* register field value  */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E:
            fieldValue = 0;
            break;

        case CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E:
            fieldValue = 1;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if( (CPSS_DIRECTION_INGRESS_E == direction) ||
        (CPSS_DIRECTION_BOTH_E == direction) )
    {
        rc = prvCpssHwPpSetRegField(devNum,
                    PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                         bridgeEngineConfig.bridgeGlobalConfig1,
                                          27, 1, fieldValue);
        if( GT_OK != rc )
        {
            return rc;
        }
    }

    if( (CPSS_DIRECTION_EGRESS_E == direction) ||
        (CPSS_DIRECTION_BOTH_E == direction) )
    {
        rc = prvCpssHwPpSetRegField(devNum,
                    PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(devNum).
                                          global.SHTGlobalConfigs,
                                           6, 1, fieldValue);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanStgIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN table to fetch span state
*       group.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       mode        - table access indexing mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanStgIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanStgIndexingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, mode));

    rc = internal_cpssDxChBrgVlanStgIndexingModeSet(devNum, direction, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanStgIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN table to fetch span state
*       group.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanStgIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      fieldValue; /* register field value  */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
            rc = prvCpssHwPpGetRegField(devNum,
                  PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                         bridgeEngineConfig.bridgeGlobalConfig1,
                                          27, 1, &fieldValue);
            break;

        case CPSS_DIRECTION_EGRESS_E:
            rc = prvCpssHwPpGetRegField(devNum,
                  PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(devNum).
                                          global.SHTGlobalConfigs,
                                           6, 1, &fieldValue);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (fieldValue == 0x0) ?
                           CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E :
                           CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanStgIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN table to fetch span state
*       group.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanStgIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      direction,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanStgIndexingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, modePtr));

    rc = internal_cpssDxChBrgVlanStgIndexingModeGet(devNum, direction, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTagStateIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN table to fetch tag state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number (CPU port supported)
*       mode        - table access indexing mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTagStateIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
)
{
    GT_U32      fieldValue;     /* register field value  */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E:
            fieldValue = 0;
            break;

        case CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E:
            fieldValue = 1;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                    PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E,
                    portNum,
                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                    SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_EUSE_VLAN_TAG_1_FOR_TAG_STATE_E, /* field name */
                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                    fieldValue);
    }

    return prvCpssHwPpSetRegField(devNum,
                    PRV_DXCH_REG1_UNIT_EGF_QAG_MAC(devNum).
                     distributor.useVlanTag1ForTagStateReg[(portNum/32)],
                                                (portNum%32), 1, fieldValue);
}

/*******************************************************************************
* cpssDxChBrgVlanTagStateIndexingModeSet
*
* DESCRIPTION:
*       Set the indexing mode for accessing the VLAN table to fetch tag state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number (CPU port supported)
*       mode        - table access indexing mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTagStateIndexingModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTagStateIndexingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode));

    rc = internal_cpssDxChBrgVlanTagStateIndexingModeSet(devNum, portNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanTagStateIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN table to fetch tag state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number (CPU port supported)
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanTagStateIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      fieldValue; /* register field value  */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                    PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E,
                    portNum,
                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                    SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_EUSE_VLAN_TAG_1_FOR_TAG_STATE_E, /* field name */
                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                    &fieldValue);
    }
    else
    {
        rc = prvCpssHwPpGetRegField(devNum,
                    PRV_DXCH_REG1_UNIT_EGF_QAG_MAC(devNum).
                     distributor.useVlanTag1ForTagStateReg[(portNum/32)],
                                                (portNum%32), 1, &fieldValue);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (fieldValue == 0x0) ?
                           CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VLAN_E :
                           CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_VID1_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanTagStateIndexingModeGet
*
* DESCRIPTION:
*       Get the indexing mode for accessing the VLAN table to fetch tag state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number (CPU port supported)
*
* OUTPUTS:
*       modePtr     - (pointer to) table access indexing mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanTagStateIndexingModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    OUT CPSS_DXCH_BRG_VLAN_TBL_INDEX_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanTagStateIndexingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modePtr));

    rc = internal_cpssDxChBrgVlanTagStateIndexingModeGet(devNum, portNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*
* DESCRIPTION:
*       Set the BPE E-Tag Ingress_E-CID_base calculation algorithm
*       for Looped Back Forwarded Bridged Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       forceSrcCidDefault  - GT_TRUE - Ingress_E-CID_base field of E-TAG forced
*                             to get default value configured by
*                             cpssDxChBrgVlanBpeTagMcCfgSet (defaultSrcECid).
*                             GT_FALSE - Ingress_E-CID_base field of E-TAG is pushed VID.
*                             configured by cpssDxChBrgVlanPortPushedTagValueSet
*                             (tagValue) per source ePort.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For regular multicast packets E-Tag Ingress_E-CID_base is always S_VID.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          forceSrcCidDefault
)
{
    GT_STATUS   rc; /* return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    rc = prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
        portNum,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_HA_EPORT_TABLE_2_FORCE_E_TAG_IE_PID_TO_DEFAULT_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        BOOL2BIT_MAC(forceSrcCidDefault));

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*
* DESCRIPTION:
*       Set the BPE E-Tag Ingress_E-CID_base calculation algorithm
*       for Looped Back Forwarded Bridged Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       forceSrcCidDefault  - GT_TRUE - Ingress_E-CID_base field of E-TAG forced
*                             to get default value configured by
*                             cpssDxChBrgVlanBpeTagMcCfgSet (defaultSrcECid).
*                             GT_FALSE - Ingress_E-CID_base field of E-TAG is pushed VID.
*                             configured by cpssDxChBrgVlanPortPushedTagValueSet
*                             (tagValue) per source ePort.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For regular multicast packets E-Tag Ingress_E-CID_base is always S_VID.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          forceSrcCidDefault
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, forceSrcCidDefault));

    rc = internal_cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet(devNum, portNum, forceSrcCidDefault);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, forceSrcCidDefault));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet
*
* DESCRIPTION:
*       Get the BPE E-Tag Ingress_E-CID_base calculation algorithm
*       for Looped Back Forwarded Bridged Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*
* OUTPUTS:
*       forceSrcCidDefaultPtr - (pointer to)
*                             GT_TRUE - Ingress_E-CID_base field of E-TAG forced
*                             to get default value configured by
*                             cpssDxChBrgVlanBpeTagMcCfgSet (defaultSrcECid).
*                             GT_FALSE - Ingress_E-CID_base field of E-TAG is pushed VID.
*                             configured by cpssDxChBrgVlanPortPushedTagValueSet
*                             (tagValue) per source ePort.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For regular multicast packets E-Tag Ingress_E-CID_base is always S_VID.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *forceSrcCidDefaultPtr
)
{
    GT_STATUS   rc;     /* function return code              */
    GT_U32      value;  /* value to write                    */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(forceSrcCidDefaultPtr);

    rc = prvCpssDxChReadTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
        portNum,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
        LION3_HA_EPORT_TABLE_2_FORCE_E_TAG_IE_PID_TO_DEFAULT_E, /* field name */
        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
        &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *forceSrcCidDefaultPtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet
*
* DESCRIPTION:
*       Get the BPE E-Tag Ingress_E-CID_base calculation algorithm
*       for Looped Back Forwarded Bridged Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*
* OUTPUTS:
*       forceSrcCidDefaultPtr - (pointer to)
*                             GT_TRUE - Ingress_E-CID_base field of E-TAG forced
*                             to get default value configured by
*                             cpssDxChBrgVlanBpeTagMcCfgSet (defaultSrcECid).
*                             GT_FALSE - Ingress_E-CID_base field of E-TAG is pushed VID.
*                             configured by cpssDxChBrgVlanPortPushedTagValueSet
*                             (tagValue) per source ePort.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For regular multicast packets E-Tag Ingress_E-CID_base is always S_VID.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *forceSrcCidDefaultPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, forceSrcCidDefaultPtr));

    rc = internal_cpssDxChBrgVlanBpeTagSourceCidDefaultForceGet(devNum, portNum, forceSrcCidDefaultPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, forceSrcCidDefaultPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanBpeTagMcCfgSet
*
* DESCRIPTION:
*       Set the BPE E-Tag Configuration for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       eCidOffsetNegative  - GT_TRUE - negative, GT_FALSE - positive
*       eCidOffset          - E-CID Offset negative or positive
*                             for Multicast packets, field
*                             {GRP,E-CID_base} calculated as (eVIDX + eCidOffset)
*                             (APPLICABLE RANGES: 0..0x3FFF)
*       defaultSrcECid      - default value for Ingress_E-CID_base field of E-TAG  -
*                             the value that not filtered by source VM filtering.
*                             Used for all Unicast and some looped back Multicast packets.
*                             About looped back Multicast packets see
*                             cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*                             (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - when one of parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanBpeTagMcCfgSet
(
    IN  GT_U8             devNum,
    IN  GT_BOOL           eCidOffsetNegative,
    IN  GT_U32            eCidOffset,
    IN  GT_U32            defaultSrcECid
)
{
    GT_U32 regAddr;     /* the register address */
    GT_U32 value;       /* the value to write   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (eCidOffset > 0x3FFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (defaultSrcECid > 0xFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).BPEConfigReg1;
    value =
        ((BOOL2BIT_MAC(eCidOffsetNegative) << 26)
         | (eCidOffset << 12) | defaultSrcECid);

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 0 /*offset*/, 27 /*length*/, value);
}

/*******************************************************************************
* cpssDxChBrgVlanBpeTagMcCfgSet
*
* DESCRIPTION:
*       Set the BPE E-Tag Configuration for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       eCidOffsetNegative  - GT_TRUE - negative, GT_FALSE - positive
*       eCidOffset          - E-CID Offset negative or positive
*                             for Multicast packets, field
*                             {GRP,E-CID_base} calculated as (eVIDX + eCidOffset)
*                             (APPLICABLE RANGES: 0..0x3FFF)
*       defaultSrcECid      - default value for Ingress_E-CID_base field of E-TAG  -
*                             the value that not filtered by source VM filtering.
*                             Used for all Unicast and some looped back Multicast packets.
*                             About looped back Multicast packets see
*                             cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*                             (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - when one of parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagMcCfgSet
(
    IN  GT_U8             devNum,
    IN  GT_BOOL           eCidOffsetNegative,
    IN  GT_U32            eCidOffset,
    IN  GT_U32            defaultSrcECid
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanBpeTagMcCfgSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, eCidOffsetNegative, eCidOffset, defaultSrcECid));

    rc = internal_cpssDxChBrgVlanBpeTagMcCfgSet(devNum, eCidOffsetNegative, eCidOffset, defaultSrcECid);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, eCidOffsetNegative, eCidOffset, defaultSrcECid));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanBpeTagMcCfgGet
*
* DESCRIPTION:
*       Get the BPE E-Tag Configuration for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       eCidOffsetNegativePtr  - (pointer to)GT_TRUE - negative, GT_FALSE - positive
*       eCidOffsetPtr          - (pointer to)E-CID Offset negative or positive
*                                for Multicast packets, field
*                                {GRP,E-CID_base} calculated as (eVIDX + eCidOffset)
*       defaultSrcECidPtr      - (pointer to)default value for Ingress_E-CID_base field of E-TAG  -
*                                the value that not filtered by source VM filtering.
*                                Used for all Unicast and some looped back Multicast packets.
*                                About looped back Multicast packets see
*                                cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanBpeTagMcCfgGet
(
    IN  GT_U8             devNum,
    OUT GT_BOOL           *eCidOffsetNegativePtr,
    OUT GT_U32            *eCidOffsetPtr,
    OUT GT_U32            *defaultSrcECidPtr
)
{
    GT_STATUS   rc;     /* return code            */
    GT_U32 regAddr;     /* the register address   */
    GT_U32 value;       /* the value from regiter */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(eCidOffsetNegativePtr);
    CPSS_NULL_PTR_CHECK_MAC(eCidOffsetPtr);
    CPSS_NULL_PTR_CHECK_MAC(defaultSrcECidPtr);

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).BPEConfigReg1;

    rc = prvCpssHwPpReadRegister(devNum, regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *eCidOffsetNegativePtr = BIT2BOOL_MAC(((value >> 26) & 1));
    *eCidOffsetPtr         = ((value >> 12) & 0x3FFF);
    *defaultSrcECidPtr     = (value & 0xFFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanBpeTagMcCfgGet
*
* DESCRIPTION:
*       Get the BPE E-Tag Configuration for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       eCidOffsetNegativePtr  - (pointer to)GT_TRUE - negative, GT_FALSE - positive
*       eCidOffsetPtr          - (pointer to)E-CID Offset negative or positive
*                                for Multicast packets, field
*                                {GRP,E-CID_base} calculated as (eVIDX + eCidOffset)
*       defaultSrcECidPtr      - (pointer to)default value for Ingress_E-CID_base field of E-TAG  -
*                                the value that not filtered by source VM filtering.
*                                Used for all Unicast and some looped back Multicast packets.
*                                About looped back Multicast packets see
*                                cpssDxChBrgVlanBpeTagSourceCidDefaultForceSet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagMcCfgGet
(
    IN  GT_U8             devNum,
    OUT GT_BOOL           *eCidOffsetNegativePtr,
    OUT GT_U32            *eCidOffsetPtr,
    OUT GT_U32            *defaultSrcECidPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanBpeTagMcCfgGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, eCidOffsetNegativePtr, eCidOffsetPtr, defaultSrcECidPtr));

    rc = internal_cpssDxChBrgVlanBpeTagMcCfgGet(devNum, eCidOffsetNegativePtr, eCidOffsetPtr, defaultSrcECidPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, eCidOffsetNegativePtr, eCidOffsetPtr, defaultSrcECidPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanBpeTagReservedFieldsSet
*
* DESCRIPTION:
*       Set the values for BPE E-Tag Reserved Fields of all packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       reValue             - value for reserved field called "re-"
*                             (APPLICABLE RANGES: 0..3)
*       ingressECidExtValue - value for field "ingress_E-CID_ext"
*                             (APPLICABLE RANGES: 0..0xFF)
*       eCidExtValue        - value for field "E-CID_ext"
*                             (APPLICABLE RANGES: 0..0xFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - when one of parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanBpeTagReservedFieldsSet
(
    IN  GT_U8             devNum,
    IN  GT_U32            reValue,
    IN  GT_U32            ingressECidExtValue,
    IN  GT_U32            eCidExtValue
)
{
    GT_U32 regAddr;     /* the register address */
    GT_U32 value;       /* the value to write   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (reValue > 0x3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ingressECidExtValue > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (eCidExtValue > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).BPEConfigReg2;
    value =
        ((reValue << 16) | (ingressECidExtValue << 8) | eCidExtValue);

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 0 /*offset*/, 18 /*length*/, value);
}

/*******************************************************************************
* cpssDxChBrgVlanBpeTagReservedFieldsSet
*
* DESCRIPTION:
*       Set the values for BPE E-Tag Reserved Fields of all packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       reValue             - value for reserved field called "re-"
*                             (APPLICABLE RANGES: 0..3)
*       ingressECidExtValue - value for field "ingress_E-CID_ext"
*                             (APPLICABLE RANGES: 0..0xFF)
*       eCidExtValue        - value for field "E-CID_ext"
*                             (APPLICABLE RANGES: 0..0xFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - when one of parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagReservedFieldsSet
(
    IN  GT_U8             devNum,
    IN  GT_U32            reValue,
    IN  GT_U32            ingressECidExtValue,
    IN  GT_U32            eCidExtValue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanBpeTagReservedFieldsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, reValue, ingressECidExtValue, eCidExtValue));

    rc = internal_cpssDxChBrgVlanBpeTagReservedFieldsSet(devNum, reValue, ingressECidExtValue, eCidExtValue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, reValue, ingressECidExtValue, eCidExtValue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanBpeTagReservedFieldsGet
*
* DESCRIPTION:
*       Get the values for BPE E-Tag Reserved Fields of all packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       reValuePtr             - (pointer to)value for reserved field called "re-"
*       ingressECidExtValuePtr - (pointer to)value for field "ingress_E-CID_ext"
*       eCidExtValuePtr        - (pointer to)value for field "E-CID_ext"
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanBpeTagReservedFieldsGet
(
    IN  GT_U8             devNum,
    OUT GT_U32            *reValuePtr,
    OUT GT_U32            *ingressECidExtValuePtr,
    OUT GT_U32            *eCidExtValuePtr
)
{
    GT_STATUS   rc;     /* return code            */
    GT_U32 regAddr;     /* the register address   */
    GT_U32 value;       /* the value from regiter */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(reValuePtr);
    CPSS_NULL_PTR_CHECK_MAC(ingressECidExtValuePtr);
    CPSS_NULL_PTR_CHECK_MAC(eCidExtValuePtr);

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).BPEConfigReg2;

    rc = prvCpssHwPpReadRegister(devNum, regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *reValuePtr              = ((value >> 16) & 3);
    *ingressECidExtValuePtr  = ((value >> 8) & 0xFF);
    *eCidExtValuePtr         = (value & 0xFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanBpeTagReservedFieldsGet
*
* DESCRIPTION:
*       Get the values for BPE E-Tag Reserved Fields of all packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       reValuePtr             - (pointer to)value for reserved field called "re-"
*       ingressECidExtValuePtr - (pointer to)value for field "ingress_E-CID_ext"
*       eCidExtValuePtr        - (pointer to)value for field "E-CID_ext"
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanBpeTagReservedFieldsGet
(
    IN  GT_U8             devNum,
    OUT GT_U32            *reValuePtr,
    OUT GT_U32            *ingressECidExtValuePtr,
    OUT GT_U32            *eCidExtValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanBpeTagReservedFieldsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, reValuePtr, ingressECidExtValuePtr, eCidExtValuePtr));

    rc = internal_cpssDxChBrgVlanBpeTagReservedFieldsGet(devNum, reValuePtr, ingressECidExtValuePtr, eCidExtValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, reValuePtr, ingressECidExtValuePtr, eCidExtValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanFdbLookupKeyModeSet
*
* DESCRIPTION:
*       Set an FDB Lookup key mode
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       vlanId                 - VLAN ID
*       mode                   - FDB Lookup key mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanFdbLookupKeyModeSet
(
    IN GT_U8                                 devNum,
    IN GT_U16                                vlanId,
    IN CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT mode
)
{
    GT_STATUS   rc;
    GT_U32      hwData;
    GT_U32      fid;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vlanId);

    if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_CONVERT_FDB_LOOKUP_KEY_MODE_TO_HW_VAL_MAC(hwData, mode);

    rc = prvCpssDxChWriteTableEntryField(devNum,
                      PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                      vlanId,
                      PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                      LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FDB_LOOKUP_KEY_MODE_E,
                      PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                      hwData);

    /* store FDB Lookup key mode of the vlan into DB according to a vlan's FID */
    if (GT_OK == rc)
    {
        rc = cpssDxChBrgVlanForwardingIdGet(devNum, vlanId, &fid);
        if (rc != GT_OK)
        {
            return rc;
        }
        PRV_CPSS_DXCH_FDB_LOOKUP_KEY_MODE_PER_FID_SET_MAC(devNum, fid, mode);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgVlanFdbLookupKeyModeSet
*
* DESCRIPTION:
*       Set an FDB Lookup key mode
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       vlanId                 - VLAN ID
*       mode                   - FDB Lookup key mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanFdbLookupKeyModeSet
(
    IN GT_U8                                 devNum,
    IN GT_U16                                vlanId,
    IN CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanFdbLookupKeyModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanId, mode));

    rc = internal_cpssDxChBrgVlanFdbLookupKeyModeSet(devNum, vlanId, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanId, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlan6BytesTagConfigSet
*
* DESCRIPTION:
*       Set global configurations for 6 bytes v-tag.
*       NOTE: not relevant to standard 8 bytes BPE 802.1BR.
*
* APPLICABLE DEVICES:
*       Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum              - device number
*       isPortExtender   - is this 'port Extender' or 'Control bridge'
*                          GT_TRUE - 'port Extender'
*                          GT_FALSE - 'Control bridge'
*      lBitInSrcId - the bit index of 'L bit' in the srcId field passed from ingress pipe to egress pipe.
*                    (APPLICABLE RANGES: 0..15)
*      dBitInSrcId - the bit index of 'D bit' in the srcId field passed from ingress pipe to egress pipe.
*                    (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter devNum
*       GT_OUT_OF_RANGE          - out of range in value of lBitInSrcId or dBitInSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlan6BytesTagConfigSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  isPortExtender,
    IN GT_U32   lBitInSrcId,
    IN GT_U32   dBitInSrcId
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;             /* register address                */
    GT_U32      startBit; /* start bit in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
          CPSS_BOBCAT2_E);

    if(lBitInSrcId >= BIT_4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(dBitInSrcId >= BIT_4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    startBit = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 26 : 25;

    regAddr =
        PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
    /* set isPortExtender */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, startBit + 4, 1, BOOL2BIT_MAC(isPortExtender));
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set lBitInSrcId */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, startBit, 4, lBitInSrcId);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr =
        PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig1;

    /* set dBitInSrcId */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 26, 4, dBitInSrcId);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlan6BytesTagConfigSet
*
* DESCRIPTION:
*       Set global configurations for 6 bytes v-tag.
*       NOTE: not relevant to standard 8 bytes BPE 802.1BR.
*
* APPLICABLE DEVICES:
*       Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum              - device number
*       isPortExtender   - is this 'port Extender' or 'Control bridge'
*                          GT_TRUE - 'port Extender'
*                          GT_FALSE - 'Control bridge'
*      lBitInSrcId - the bit index of 'L bit' in the srcId field passed from ingress pipe to egress pipe.
*                    (APPLICABLE RANGES: 0..15)
*      dBitInSrcId - the bit index of 'D bit' in the srcId field passed from ingress pipe to egress pipe.
*                    (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter devNum
*       GT_OUT_OF_RANGE          - out of range in value of lBitInSrcId or dBitInSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlan6BytesTagConfigSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  isPortExtender,
    IN GT_U32   lBitInSrcId,
    IN GT_U32   dBitInSrcId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlan6BytesTagConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, isPortExtender, lBitInSrcId , dBitInSrcId));

    rc = internal_cpssDxChBrgVlan6BytesTagConfigSet(devNum, isPortExtender, lBitInSrcId , dBitInSrcId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, isPortExtender, lBitInSrcId , dBitInSrcId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlan6BytesTagConfigGet
*
* DESCRIPTION:
*       Get global configurations for 6 bytes v-tag.
*       NOTE: not relevant to standard 8 bytes BPE 802.1BR.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       isPortExtenderPtr   - (pointer to) is this 'port Extender' or 'Control bridge'
*                          GT_TRUE - 'port Extender'
*                          GT_FALSE - 'Control bridge'
*      lBitInSrcIdPtr - (pointer to) the bit index of 'L bit' in the srcId field passed from ingress pipe to egress pipe.
*      dBitInSrcIdPtr - (pointer to) the bit index of 'D bit' in the srcId field passed from ingress pipe to egress pipe.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlan6BytesTagConfigGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *isPortExtenderPtr,
    OUT GT_U32   *lBitInSrcIdPtr,
    OUT GT_U32   *dBitInSrcIdPtr
)
{
    GT_STATUS   rc;     /* return code            */
    GT_U32 regAddr;     /* the register address   */
    GT_U32 value;       /* the value from register */
    GT_U32      startBit; /* start bit in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(isPortExtenderPtr);
    CPSS_NULL_PTR_CHECK_MAC(lBitInSrcIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(dBitInSrcIdPtr);

    startBit = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 26 : 25;

    regAddr =
        PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
    /* get isPortExtender */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, startBit + 4, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *isPortExtenderPtr = BIT2BOOL_MAC(value);

    /* get lBitInSrcId */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, startBit , 4, lBitInSrcIdPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr =
        PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig1;

    /* get dBitInSrcId */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 26, 4, dBitInSrcIdPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgVlan6BytesTagConfigGet
*
* DESCRIPTION:
*       Get global configurations for 6 bytes v-tag.
*       NOTE: not relevant to standard 8 bytes BPE 802.1BR.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       isPortExtenderPtr   - (pointer to) is this 'port Extender' or 'Control bridge'
*                          GT_TRUE - 'port Extender'
*                          GT_FALSE - 'Control bridge'
*      lBitInSrcIdPtr - (pointer to) the bit index of 'L bit' in the srcId field passed from ingress pipe to egress pipe.
*      dBitInSrcIdPtr - (pointer to) the bit index of 'D bit' in the srcId field passed from ingress pipe to egress pipe.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlan6BytesTagConfigGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *isPortExtenderPtr,
    OUT GT_U32   *lBitInSrcIdPtr,
    OUT GT_U32   *dBitInSrcIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlan6BytesTagConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, isPortExtenderPtr, lBitInSrcIdPtr , dBitInSrcIdPtr));

    rc = internal_cpssDxChBrgVlan6BytesTagConfigGet(devNum, isPortExtenderPtr, lBitInSrcIdPtr , dBitInSrcIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, isPortExtenderPtr, lBitInSrcIdPtr , dBitInSrcIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

