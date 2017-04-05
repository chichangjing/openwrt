/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdCpssDxChIncrEntry.c
*
* DESCRIPTION:
*       CPSS Utils for create incremented Entries sequences
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>
#include <galtisAgent/wrapUtil/dxCh/utils/cmdCpssDxChIncrEntry.h>

extern GT_U32          wrCpssDxChVirtualTcam_useVTcamApi;
extern GT_U32          wrCpssDxChVirtualTcam_vTcamMngId;
extern GT_U32          wrCpssDxChVirtualTcam_vTcamId;
extern GT_U32          wrCpssDxChVirtualTcam_vTcamRulePrio;

/*******************************************************************************
* cmdCpssDxChGenIntrfaceInfoIncrement
*
* DESCRIPTION:
*       Inrement the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChGenIntrfaceInfoIncrement
(
    INOUT CPSS_INTERFACE_INFO_STC *basePtr,
    IN    CPSS_INTERFACE_INFO_STC *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    if (basePtr->type != incrPtr->type)
    {
        /* different types means that structure not incremented */
        /* this behavior is legitime                            */
        return GT_OK;
    }
    switch (basePtr->type)
    {
        case CPSS_INTERFACE_DEVICE_E:
            _INCR(hwDevNum);
            break;
        case CPSS_INTERFACE_FABRIC_VIDX_E:
            _INCR(fabricVidx);
            break;
        case CPSS_INTERFACE_INDEX_E:
            _INCR(index);
            break;
        case CPSS_INTERFACE_PORT_E:
            _INCR(devPort.portNum);
            break;
        case CPSS_INTERFACE_TRUNK_E:
            _INCR(trunkId);
            break;
        case CPSS_INTERFACE_VID_E:
            _INCR(vlanId);
            break;
        case CPSS_INTERFACE_VIDX_E:
            _INCR(vidx);
            break;
        default: return GT_FAIL;
    }
    return GT_OK;
    #undef _INCR
}

/*******************************************************************************
* cmdCpssDxChOamEntryIncrement
*
* DESCRIPTION:
*       Inrement the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChOamEntryIncrement
(
    INOUT CPSS_DXCH_OAM_ENTRY_STC *basePtr,
    IN    CPSS_DXCH_OAM_ENTRY_STC *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    GT_STATUS rc = GT_OK;

    _INCR(megLevel);
    _INCR(packetCommandProfile);
    _INCR(cpuCodeOffset);
    if (GT_OK != cmdCpssDxChGenIntrfaceInfoIncrement(
        &(basePtr->sourceInterface), &(incrPtr->sourceInterface)))
    {
        rc = GT_FAIL;
    }
    /*CPSS_INTERFACE_INFO_STC  sourceInterface;*/
    _INCR(lmCounterCaptureEnable);
    _INCR(dualEndedLmEnable);
    _INCR(oamPtpOffsetIndex);
    _INCR(timestampEnable);
    _INCR(keepaliveAgingEnable);
    _INCR(ageState);
    _INCR(agingPeriodIndex);
    _INCR(agingThreshold);
    _INCR(flowHash);
    _INCR(excessKeepalivePeriodCounter);
    _INCR(excessKeepalivePeriodThreshold);
    _INCR(excessKeepaliveMessageCounter);
    _INCR(excessKeepaliveMessageThreshold);
    _INCR(rdiStatus);
    _INCR(keepaliveTxPeriod);
    return rc;
    #undef _INCR
}

/*******************************************************************************
* cmdCpssDxChOamEntrySequenceWrite
*
* DESCRIPTION:
*       Write sequence of incremented OAM Entries.
*
* INPUTS:
*       devNum            - device number
*       stage             - OAM stage type.
*       baseEntryIndex  - index of entry to be retrieved and used as base values.
*                           this entry also will be start entry of the sequence
*       incrEntryIndex  - index of entry to be retrieved and used as increment values.
*                           retieved at the beginnig and can be overriden by sequence.
*       amountToWrite     - amount of enries to write
*       writeIndexIncr    - signed increment of sequence entry index
*
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChOamEntrySequenceWrite
(
    IN   GT_U8                              devNum,
    IN   CPSS_DXCH_OAM_STAGE_TYPE_ENT       stage,
    IN   GT_U32                             baseEntryIndex,
    IN   GT_U32                             incrEntryIndex,
    IN   GT_U32                             amountToWrite,
    IN   GT_32                              writeIndexIncr
)
{
    GT_STATUS rc;                      /* return code */
    CPSS_DXCH_OAM_ENTRY_STC baseEntry; /* base entry */
    CPSS_DXCH_OAM_ENTRY_STC incrEntry; /* entry with incrementing values */
    GT_U32                  index;     /* current entry index */
    GT_U32                  i;         /* loop index */

    rc = cpssDxChOamEntryGet(
        devNum, stage, baseEntryIndex, &baseEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChOamEntryGet(
        devNum, stage, incrEntryIndex, &incrEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 1; (i < amountToWrite); i++)
    {
        rc = cmdCpssDxChOamEntryIncrement(&baseEntry, &incrEntry);
        if (rc != GT_OK)
        {
            return rc;
        }
        index = (GT_U32)(baseEntryIndex + (writeIndexIncr * i));
        rc = cpssDxChOamEntrySet(
            devNum, stage, index, &baseEntry);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChPclRuleClearEgrCommonOvelappingFields
*
* DESCRIPTION:
*       Clear overlapping fields in the given egress common structure.
*
* INPUTS:
*
*
* OUTPUTS:
*       dataPtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       none.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleClearEgrCommonOvelappingFields
(
    OUT   CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC                    *dataPtr
)
{
    cpssOsMemSet(&(dataPtr->fromCpu), 0, sizeof(dataPtr->fromCpu));
    cpssOsMemSet(&(dataPtr->fwdData), 0, sizeof(dataPtr->fwdData));
    cpssOsMemSet(&(dataPtr->toAnalyzer), 0, sizeof(dataPtr->toAnalyzer));
    cpssOsMemSet(&(dataPtr->toCpu), 0, sizeof(dataPtr->toCpu));
    cpssOsMemSet(&(dataPtr->portListBmp), 0, sizeof(dataPtr->portListBmp));
}

/*******************************************************************************
* cmdCpssDxChPclRuleClearOvelappingFields
*
* DESCRIPTION:
*       Clear overlapping fields in the given structure.
*
* INPUTS:
*       ruleFormat      - Rule Format
*
* OUTPUTS:
*       dataPtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS cmdCpssDxChPclRuleClearOvelappingFields
(
    IN    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    OUT   CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *dataPtr
)
{
    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            cpssOsMemSet(
                &(dataPtr->ruleStdNotIp.common.portListBmp),
                0, sizeof(dataPtr->ruleStdNotIp.common.portListBmp));
            dataPtr->ruleStdNotIp.vrfId = 0;
            dataPtr->ruleStdNotIp.trunkHash = 0;
            dataPtr->ruleStdNotIp.tag1Exist = 0;
            dataPtr->ruleStdNotIp.vid1 = 0;
            dataPtr->ruleStdNotIp.up1 = 0;
            dataPtr->ruleStdNotIp.cfi1 = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            cpssOsMemSet(
                &(dataPtr->ruleStdIpL2Qos.common.portListBmp),
                0, sizeof(dataPtr->ruleStdIpL2Qos.common.portListBmp));
            dataPtr->ruleStdIpL2Qos.vrfId = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            cpssOsMemSet(
                &(dataPtr->ruleStdIpv4L4.common.portListBmp),
                0, sizeof(dataPtr->ruleStdIpv4L4.common.portListBmp));
            dataPtr->ruleStdIpv4L4.vrfId = 0;
            dataPtr->ruleStdIpv4L4.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            cpssOsMemSet(
                &(dataPtr->ruleStdIpv6Dip.common.portListBmp),
                0, sizeof(dataPtr->ruleStdIpv6Dip.common.portListBmp));
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            cpssOsMemSet(
                &(dataPtr->ruleExtNotIpv6.common.portListBmp),
                0, sizeof(dataPtr->ruleExtNotIpv6.common.portListBmp));
            dataPtr->ruleExtNotIpv6.vrfId = 0;
            dataPtr->ruleExtNotIpv6.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            cpssOsMemSet(
                &(dataPtr->ruleExtIpv6L2.common.portListBmp),
                0, sizeof(dataPtr->ruleExtIpv6L2.common.portListBmp));
            dataPtr->ruleExtIpv6L2.vrfId = 0;
            dataPtr->ruleExtIpv6L2.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            cpssOsMemSet(
                &(dataPtr->ruleExtIpv6L4.common.portListBmp),
                0, sizeof(dataPtr->ruleExtIpv6L4.common.portListBmp));
            dataPtr->ruleExtIpv6L4.vrfId = 0;
            dataPtr->ruleExtIpv6L4.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            cmdCpssDxChPclRuleClearEgrCommonOvelappingFields(
                &(dataPtr->ruleEgrStdNotIp.common));
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            cmdCpssDxChPclRuleClearEgrCommonOvelappingFields(
                &(dataPtr->ruleEgrStdIpL2Qos.common));
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            cmdCpssDxChPclRuleClearEgrCommonOvelappingFields(
                &(dataPtr->ruleEgrStdIpv4L4.common));
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            cmdCpssDxChPclRuleClearEgrCommonOvelappingFields(
                &(dataPtr->ruleEgrExtNotIpv6.common));
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            cmdCpssDxChPclRuleClearEgrCommonOvelappingFields(
                &(dataPtr->ruleEgrExtIpv6L2.common));
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            cmdCpssDxChPclRuleClearEgrCommonOvelappingFields(
                &(dataPtr->ruleEgrExtIpv6L4.common));
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            cpssOsMemSet(
                &(dataPtr->ruleIngrStdUdb.commonIngrUdb.portListBmp),
                0, sizeof(dataPtr->ruleIngrStdUdb.commonIngrUdb.portListBmp));
            dataPtr->ruleIngrStdUdb.vrfId = 0;
            dataPtr->ruleIngrStdUdb.qosProfile = 0;
            dataPtr->ruleIngrStdUdb.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            cpssOsMemSet(
                &(dataPtr->ruleIngrExtUdb.commonIngrUdb.portListBmp),
                0, sizeof(dataPtr->ruleIngrExtUdb.commonIngrUdb.portListBmp));
            dataPtr->ruleIngrExtUdb.vrfId = 0;
            dataPtr->ruleIngrExtUdb.qosProfile = 0;
            dataPtr->ruleIngrExtUdb.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E:
            dataPtr->ruleStdIpv4RoutedAclQos.vrfId =0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E:
            dataPtr->ruleExtIpv4PortVlanQos.vrfId = 0;
            dataPtr->ruleExtIpv4PortVlanQos.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E:
            dataPtr->ruleUltraIpv6PortVlanQos.vrfId = 0;
            dataPtr->ruleUltraIpv6PortVlanQos.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E:
            dataPtr->ruleUltraIpv6RoutedAclQos.vrfId = 0;
            dataPtr->ruleUltraIpv6RoutedAclQos.trunkHash = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E:
            dataPtr->ruleEgrExtIpv4RaclVacl.egressIpCommon.srcHwDev = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E:
            dataPtr->ruleEgrUltraIpv6RaclVacl.egressIpCommon.srcHwDev = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E:
            cpssOsMemSet(
                &(dataPtr->ruleIngrUdbOnly.replacedFld),
                0, sizeof(dataPtr->ruleIngrUdbOnly.replacedFld));
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E:
            cpssOsMemSet(
                &(dataPtr->ruleEgrUdbOnly.replacedFld),
                0, sizeof(dataPtr->ruleEgrUdbOnly.replacedFld));
            break;
        default: return GT_FAIL;
    }
    return GT_OK;
}

/*******************************************************************************
* cmdCpssDxChByteArrayIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the byte array with base values.
*       incrPtr         - (pointer to)the byte array with increment values.
*       length          - amount of byte in array
*       carry           - GT_TRUE  - to add carry bit to the next byte
*                         GT_FALSE - to loose carry bit
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChByteArrayIncrement
(
    INOUT GT_U8      *basePtr,
    IN    GT_U8      *incrPtr,
    IN    GT_U32     length,
    IN    GT_BOOL    carry
)
{
    GT_U32 i;   /* loop index             */
    GT_U8  *bP; /* work base pointer      */
    GT_U8  *iP; /* work increment pointer */
    GT_U32 cF;  /* carry flag             */
    GT_U32 res; /* result                 */

    bP = &(basePtr[length - 1]);
    iP = &(incrPtr[length - 1]);
    if (carry == GT_FALSE)
    {
        for (i = 0; (i < length); i++, bP--, iP--)
        {
            *bP += *iP;
        }
    }
    else
    {
        cF = 0;
        for (i = 0; (i < length); i++, bP--, iP--)
        {
            res = (GT_U32)*bP + (GT_U32)*iP + cF;
            cF = (res >> 8) & 1;
            *bP = (GT_U8)res;
        }
    }
}

/*******************************************************************************
* cmdCpssDxChPclRuleIngrCommonIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleIngrCommonIncrement
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(pclId);
    _INCR(macToMe);
    _INCR(sourcePort);
    _INCR(sourceDevice);
    _INCR(isTagged);
    _INCR(vid);
    _INCR(up);
    _INCR(qosProfile);
    _INCR(isIp);
    _INCR(isL2Valid);
    _INCR(isUdbValid);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleIngrCommonStdIpIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleIngrCommonStdIpIncrement
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(isIpv4);
    _INCR(ipProtocol);
    _INCR(dscp);
    _INCR(isL4Valid);
    _INCR(l4Byte2);
    _INCR(l4Byte3);
    _INCR(ipHeaderOk);
    _INCR(ipv4Fragmented);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleIngrCommonExtIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleIngrCommonExtIncrement
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(isIpv6);
    _INCR(ipProtocol);
    _INCR(dscp);
    _INCR(isL4Valid);
    _INCR(l4Byte0);
    _INCR(l4Byte1);
    _INCR(l4Byte2);
    _INCR(l4Byte3);
    _INCR(l4Byte13);
    _INCR(ipHeaderOk);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleEgrCommonIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleEgrCommonIncrement
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(valid);
    _INCR(pclId);
    _INCR(sourcePort);
    _INCR(isTagged);
    _INCR(vid);
    _INCR(up);
    _INCR(isIp);
    _INCR(isL2Valid);
    _INCR(egrPacketType);
    _INCR(srcHwDev);
    _INCR(sourceId);
    _INCR(isVidx);
    _INCR(tag1Exist);
    _INCR(isUdbValid);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleEgrCommonStdIpIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleEgrCommonStdIpIncrement
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(isIpv4);
    _INCR(ipProtocol);
    _INCR(dscp);
    _INCR(isL4Valid);
    _INCR(l4Byte2);
    _INCR(l4Byte3);
    _INCR(ipv4Fragmented);
    _INCR(egrTcpUdpPortComparator);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleEgrCommonExtIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleEgrCommonExtIncrement
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(isIpv6);
    _INCR(ipProtocol);
    _INCR(dscp);
    _INCR(isL4Valid);
    _INCR(l4Byte0);
    _INCR(l4Byte1);
    _INCR(l4Byte2);
    _INCR(l4Byte3);
    _INCR(l4Byte13);
    _INCR(egrTcpUdpPortComparator);
    _INCR(isUdbValid);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleIngrCommonUdbIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleIngrCommonUdbIncrement
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(pclId);
    _INCR(macToMe);
    _INCR(sourcePort);
    _INCR(sourceDevice);
    _INCR(vid);
    _INCR(up);
    _INCR(isIp);
    _INCR(dscpOrExp);
    _INCR(isL2Valid);
    _INCR(isUdbValid);
    _INCR(pktTagging);
    _INCR(l3OffsetInvalid);
    _INCR(l4ProtocolType);
    _INCR(pktType);
    _INCR(ipHeaderOk);
    _INCR(macDaType);
    _INCR(l4OffsetInvalid);
    _INCR(l2Encapsulation);
    _INCR(isIpv6Eh);
    _INCR(isIpv6HopByHop);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleIngrCommonEnh1Increment
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleIngrCommonEnh1Increment
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(pclId);
    _INCR(macToMe);
    _INCR(sourcePort);
    _INCR(sourceDevice);
    _INCR(isTagged);
    _INCR(vid);
    _INCR(up);
    _INCR(tos);
    _INCR(ipProtocol);
    _INCR(isL4Valid);
    _INCR(l4Byte0);
    _INCR(l4Byte1);
    _INCR(l4Byte2);
    _INCR(l4Byte3);
    _INCR(l4Byte13);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleEgrCommonEnh1Increment
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID cmdCpssDxChPclRuleEgrCommonEnh1Increment
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC      *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC      *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR(pclId);
    _INCR(sourcePort);
    _INCR(qosProfile);
    _INCR(isL2Valid);
    _INCR(originalVid);
    _INCR(isSrcTrunk);
    _INCR(srcDevOrTrunkId);
    _INCR(isIp);
    _INCR(isArp);
    cmdCpssDxChByteArrayIncrement(
        &(basePtr->macDa.arEther[0]),
        &(incrPtr->macDa.arEther[0]),
        sizeof(basePtr->macDa.arEther),
        GT_TRUE);
    cmdCpssDxChByteArrayIncrement(
        &(basePtr->macSa.arEther[0]),
        &(incrPtr->macSa.arEther[0]),
        sizeof(basePtr->macSa.arEther),
        GT_TRUE);
    _INCR(tag1Exist);
    _INCR(sourceId);
    _INCR(tos);
    _INCR(ipProtocol);
    _INCR(ttl);
    _INCR(isL4Valid);
    _INCR(l4Byte0);
    _INCR(l4Byte1);
    _INCR(l4Byte2);
    _INCR(l4Byte3);
    _INCR(l4Byte13);
    _INCR(tcpUdpPortComparators);
    _INCR(tc);
    _INCR(dp);
    _INCR(egrPacketType);
    _INCR(srcTrgOrTxMirror);
    _INCR(assignedUp);
    _INCR(trgPhysicalPort);
    _INCR(rxSniff);
    _INCR(isRouted);
    _INCR(isIpv6);
    _INCR(cpuCode);
    _INCR(srcTrg);
    _INCR(egrFilterEnable);
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclRuleIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       ruleFormat      - Rule Format
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChPclRuleIncrement
(
    IN    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT        ruleFormat,
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_UNT             *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_UNT             *incrPtr
)
{
    #define _INCR(field) basePtr->field += incrPtr->field
    GT_U32 i; /* loop index */
    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            cmdCpssDxChPclRuleIngrCommonIncrement(
                &(basePtr->ruleStdNotIp.common),
                &(incrPtr->ruleStdNotIp.common));
            _INCR(ruleStdNotIp.isIpv4);
            _INCR(ruleStdNotIp.etherType);
            _INCR(ruleStdNotIp.isArp);
            _INCR(ruleStdNotIp.l2Encap);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdNotIp.macDa.arEther[0]),
                &(incrPtr->ruleStdNotIp.macDa.arEther[0]),
                sizeof(basePtr->ruleStdNotIp.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdNotIp.macSa.arEther[0]),
                &(incrPtr->ruleStdNotIp.macSa.arEther[0]),
                sizeof(basePtr->ruleStdNotIp.macSa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdNotIp.udb15_17[0]),
                &(incrPtr->ruleStdNotIp.udb15_17[0]),
                sizeof(basePtr->ruleStdNotIp.udb15_17),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdNotIp.udb23_26[0]),
                &(incrPtr->ruleStdNotIp.udb23_26[0]),
                sizeof(basePtr->ruleStdNotIp.udb23_26),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            cmdCpssDxChPclRuleIngrCommonIncrement(
                &(basePtr->ruleStdIpL2Qos.common),
                &(incrPtr->ruleStdIpL2Qos.common));
            cmdCpssDxChPclRuleIngrCommonStdIpIncrement(
                &(basePtr->ruleStdIpL2Qos.commonStdIp),
                &(incrPtr->ruleStdIpL2Qos.commonStdIp));
            _INCR(ruleStdIpL2Qos.isArp);
            _INCR(ruleStdIpL2Qos.isIpv6ExtHdrExist);
            _INCR(ruleStdIpL2Qos.isIpv6HopByHop);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpL2Qos.macDa.arEther[0]),
                &(incrPtr->ruleStdIpL2Qos.macDa.arEther[0]),
                sizeof(basePtr->ruleStdIpL2Qos.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpL2Qos.macSa.arEther[0]),
                &(incrPtr->ruleStdIpL2Qos.macSa.arEther[0]),
                sizeof(basePtr->ruleStdIpL2Qos.macSa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpL2Qos.udb18_19[0]),
                &(incrPtr->ruleStdIpL2Qos.udb18_19[0]),
                sizeof(basePtr->ruleStdIpL2Qos.udb18_19),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpL2Qos.udb27_30[0]),
                &(incrPtr->ruleStdIpL2Qos.udb27_30[0]),
                sizeof(basePtr->ruleStdIpL2Qos.udb27_30),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            cmdCpssDxChPclRuleIngrCommonIncrement(
                &(basePtr->ruleStdIpv4L4.common),
                &(incrPtr->ruleStdIpv4L4.common));
            cmdCpssDxChPclRuleIngrCommonStdIpIncrement(
                &(basePtr->ruleStdIpv4L4.commonStdIp),
                &(incrPtr->ruleStdIpv4L4.commonStdIp));
            _INCR(ruleStdIpv4L4.isArp);
            _INCR(ruleStdIpv4L4.isBc);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv4L4.sip.arIP[0]),
                &(incrPtr->ruleStdIpv4L4.sip.arIP[0]),
                sizeof(basePtr->ruleStdIpv4L4.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv4L4.dip.arIP[0]),
                &(incrPtr->ruleStdIpv4L4.dip.arIP[0]),
                sizeof(basePtr->ruleStdIpv4L4.dip.arIP),
                GT_TRUE);
            _INCR(ruleStdIpv4L4.l4Byte0);
            _INCR(ruleStdIpv4L4.l4Byte1);
            _INCR(ruleStdIpv4L4.l4Byte13);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv4L4.udb20_22[0]),
                &(incrPtr->ruleStdIpv4L4.udb20_22[0]),
                sizeof(basePtr->ruleStdIpv4L4.udb20_22),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv4L4.udb31_34[0]),
                &(incrPtr->ruleStdIpv4L4.udb31_34[0]),
                sizeof(basePtr->ruleStdIpv4L4.udb31_34),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            cmdCpssDxChPclRuleIngrCommonIncrement(
                &(basePtr->ruleStdIpv6Dip.common),
                &(incrPtr->ruleStdIpv6Dip.common));
            cmdCpssDxChPclRuleIngrCommonStdIpIncrement(
                &(basePtr->ruleStdIpv6Dip.commonStdIp),
                &(incrPtr->ruleStdIpv6Dip.commonStdIp));
            _INCR(ruleStdIpv6Dip.isArp);
            _INCR(ruleStdIpv6Dip.isIpv6ExtHdrExist);
            _INCR(ruleStdIpv6Dip.isIpv6HopByHop);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv6Dip.dip.arIP[0]),
                &(incrPtr->ruleStdIpv6Dip.dip.arIP[0]),
                sizeof(basePtr->ruleStdIpv6Dip.dip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv6Dip.udb47_49[0]),
                &(incrPtr->ruleStdIpv6Dip.udb47_49[0]),
                sizeof(basePtr->ruleStdIpv6Dip.udb47_49),
                GT_FALSE);
            _INCR(ruleStdIpv6Dip.udb0);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            cmdCpssDxChPclRuleIngrCommonIncrement(
                &(basePtr->ruleExtNotIpv6.common),
                &(incrPtr->ruleExtNotIpv6.common));
            cmdCpssDxChPclRuleIngrCommonExtIncrement(
                &(basePtr->ruleExtNotIpv6.commonExt),
                &(incrPtr->ruleExtNotIpv6.commonExt));
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtNotIpv6.sip.arIP[0]),
                &(incrPtr->ruleExtNotIpv6.sip.arIP[0]),
                sizeof(basePtr->ruleExtNotIpv6.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtNotIpv6.dip.arIP[0]),
                &(incrPtr->ruleExtNotIpv6.dip.arIP[0]),
                sizeof(basePtr->ruleExtNotIpv6.dip.arIP),
                GT_TRUE);
            _INCR(ruleExtNotIpv6.etherType);
            _INCR(ruleExtNotIpv6.l2Encap);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtNotIpv6.macDa.arEther[0]),
                &(incrPtr->ruleExtNotIpv6.macDa.arEther[0]),
                sizeof(basePtr->ruleExtNotIpv6.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtNotIpv6.macSa.arEther[0]),
                &(incrPtr->ruleExtNotIpv6.macSa.arEther[0]),
                sizeof(basePtr->ruleExtNotIpv6.macSa.arEther),
                GT_TRUE);
            _INCR(ruleExtNotIpv6.ipv4Fragmented);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtNotIpv6.udb0_5[0]),
                &(incrPtr->ruleExtNotIpv6.udb0_5[0]),
                sizeof(basePtr->ruleExtNotIpv6.udb0_5),
                GT_FALSE);
            _INCR(ruleExtNotIpv6.tag1Exist);
            _INCR(ruleExtNotIpv6.vid1);
            _INCR(ruleExtNotIpv6.up1);
            _INCR(ruleExtNotIpv6.cfi1);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtNotIpv6.udb39_46[0]),
                &(incrPtr->ruleExtNotIpv6.udb39_46[0]),
                sizeof(basePtr->ruleExtNotIpv6.udb39_46),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            cmdCpssDxChPclRuleIngrCommonIncrement(
                &(basePtr->ruleExtIpv6L2.common),
                &(incrPtr->ruleExtIpv6L2.common));
            cmdCpssDxChPclRuleIngrCommonExtIncrement(
                &(basePtr->ruleExtIpv6L2.commonExt),
                &(incrPtr->ruleExtIpv6L2.commonExt));
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L2.sip.arIP[0]),
                &(incrPtr->ruleExtIpv6L2.sip.arIP[0]),
                sizeof(basePtr->ruleExtIpv6L2.sip.arIP),
                GT_TRUE);
            _INCR(ruleExtIpv6L2.dipBits127to120);
            _INCR(ruleExtIpv6L2.isIpv6ExtHdrExist);
            _INCR(ruleExtIpv6L2.isIpv6HopByHop);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L2.macDa.arEther[0]),
                &(incrPtr->ruleExtIpv6L2.macDa.arEther[0]),
                sizeof(basePtr->ruleExtIpv6L2.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L2.macSa.arEther[0]),
                &(incrPtr->ruleExtIpv6L2.macSa.arEther[0]),
                sizeof(basePtr->ruleExtIpv6L2.macSa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L2.udb6_11[0]),
                &(incrPtr->ruleExtIpv6L2.udb6_11[0]),
                sizeof(basePtr->ruleExtIpv6L2.udb6_11),
                GT_FALSE);
            _INCR(ruleExtIpv6L2.tag1Exist);
            _INCR(ruleExtIpv6L2.vid1);
            _INCR(ruleExtIpv6L2.up1);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L2.udb47_49[0]),
                &(incrPtr->ruleExtIpv6L2.udb47_49[0]),
                sizeof(basePtr->ruleExtIpv6L2.udb47_49),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L2.udb0_4[0]),
                &(incrPtr->ruleExtIpv6L2.udb0_4[0]),
                sizeof(basePtr->ruleExtIpv6L2.udb0_4),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            cmdCpssDxChPclRuleIngrCommonIncrement(
                &(basePtr->ruleExtIpv6L4.common),
                &(incrPtr->ruleExtIpv6L4.common));
            cmdCpssDxChPclRuleIngrCommonExtIncrement(
                &(basePtr->ruleExtIpv6L4.commonExt),
                &(incrPtr->ruleExtIpv6L4.commonExt));
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L4.sip.arIP[0]),
                &(incrPtr->ruleExtIpv6L4.sip.arIP[0]),
                sizeof(basePtr->ruleExtIpv6L4.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L4.dip.arIP[0]),
                &(incrPtr->ruleExtIpv6L4.dip.arIP[0]),
                sizeof(basePtr->ruleExtIpv6L4.dip.arIP),
                GT_TRUE);
            _INCR(ruleExtIpv6L4.isIpv6ExtHdrExist);
            _INCR(ruleExtIpv6L4.isIpv6HopByHop);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L4.udb12_14[0]),
                &(incrPtr->ruleExtIpv6L4.udb12_14[0]),
                sizeof(basePtr->ruleExtIpv6L4.udb12_14),
                GT_FALSE);
            _INCR(ruleExtIpv6L4.tag1Exist);
            _INCR(ruleExtIpv6L4.vid1);
            _INCR(ruleExtIpv6L4.up1);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv6L4.udb15_22[0]),
                &(incrPtr->ruleExtIpv6L4.udb15_22[0]),
                sizeof(basePtr->ruleExtIpv6L4.udb15_22),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            cmdCpssDxChPclRuleEgrCommonIncrement(
                &(basePtr->ruleEgrStdNotIp.common),
                &(incrPtr->ruleEgrStdNotIp.common));
            _INCR(ruleEgrStdNotIp.isIpv4);
            _INCR(ruleEgrStdNotIp.etherType);
            _INCR(ruleEgrStdNotIp.isArp);
            _INCR(ruleEgrStdNotIp.l2Encap);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdNotIp.macDa.arEther[0]),
                &(incrPtr->ruleEgrStdNotIp.macDa.arEther[0]),
                sizeof(basePtr->ruleEgrStdNotIp.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdNotIp.macSa.arEther[0]),
                &(incrPtr->ruleEgrStdNotIp.macSa.arEther[0]),
                sizeof(basePtr->ruleEgrStdNotIp.macSa.arEther),
                GT_TRUE);
            _INCR(ruleEgrStdNotIp.vid1);
            _INCR(ruleEgrStdNotIp.up1);
            _INCR(ruleEgrStdNotIp.cfi1);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdNotIp.udb0_3[0]),
                &(incrPtr->ruleEgrStdNotIp.udb0_3[0]),
                sizeof(basePtr->ruleEgrStdNotIp.udb0_3),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            cmdCpssDxChPclRuleEgrCommonIncrement(
                &(basePtr->ruleEgrStdIpL2Qos.common),
                &(incrPtr->ruleEgrStdIpL2Qos.common));
            cmdCpssDxChPclRuleEgrCommonStdIpIncrement(
                &(basePtr->ruleEgrStdIpL2Qos.commonStdIp),
                &(incrPtr->ruleEgrStdIpL2Qos.commonStdIp));
            _INCR(ruleEgrStdIpL2Qos.isArp);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdIpL2Qos.dipBits0to31[0]),
                &(incrPtr->ruleEgrStdIpL2Qos.dipBits0to31[0]),
                sizeof(basePtr->ruleEgrStdIpL2Qos.dipBits0to31),
                GT_TRUE);
            _INCR(ruleEgrStdIpL2Qos.l4Byte13);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdIpL2Qos.macDa.arEther[0]),
                &(incrPtr->ruleEgrStdIpL2Qos.macDa.arEther[0]),
                sizeof(basePtr->ruleEgrStdIpL2Qos.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdIpL2Qos.udb4_7[0]),
                &(incrPtr->ruleEgrStdIpL2Qos.udb4_7[0]),
                sizeof(basePtr->ruleEgrStdIpL2Qos.udb4_7),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            cmdCpssDxChPclRuleEgrCommonIncrement(
                &(basePtr->ruleEgrStdIpv4L4.common),
                &(incrPtr->ruleEgrStdIpv4L4.common));
            cmdCpssDxChPclRuleEgrCommonStdIpIncrement(
                &(basePtr->ruleEgrStdIpv4L4.commonStdIp),
                &(incrPtr->ruleEgrStdIpv4L4.commonStdIp));
            _INCR(ruleEgrStdIpv4L4.isArp);
            _INCR(ruleEgrStdIpv4L4.isBc);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdIpv4L4.sip.arIP[0]),
                &(incrPtr->ruleEgrStdIpv4L4.sip.arIP[0]),
                sizeof(basePtr->ruleEgrStdIpv4L4.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdIpv4L4.dip.arIP[0]),
                &(incrPtr->ruleEgrStdIpv4L4.dip.arIP[0]),
                sizeof(basePtr->ruleEgrStdIpv4L4.dip.arIP),
                GT_TRUE);
            _INCR(ruleEgrStdIpv4L4.l4Byte0);
            _INCR(ruleEgrStdIpv4L4.l4Byte1);
            _INCR(ruleEgrStdIpv4L4.l4Byte13);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrStdIpv4L4.udb8_11[0]),
                &(incrPtr->ruleEgrStdIpv4L4.udb8_11[0]),
                sizeof(basePtr->ruleEgrStdIpv4L4.udb8_11),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            cmdCpssDxChPclRuleEgrCommonIncrement(
                &(basePtr->ruleEgrExtNotIpv6.common),
                &(incrPtr->ruleEgrExtNotIpv6.common));
            cmdCpssDxChPclRuleEgrCommonExtIncrement(
                &(basePtr->ruleEgrExtNotIpv6.commonExt),
                &(incrPtr->ruleEgrExtNotIpv6.commonExt));
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtNotIpv6.sip.arIP[0]),
                &(incrPtr->ruleEgrExtNotIpv6.sip.arIP[0]),
                sizeof(basePtr->ruleEgrExtNotIpv6.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtNotIpv6.dip.arIP[0]),
                &(incrPtr->ruleEgrExtNotIpv6.dip.arIP[0]),
                sizeof(basePtr->ruleEgrExtNotIpv6.dip.arIP),
                GT_TRUE);
            _INCR(ruleEgrExtNotIpv6.etherType);
            _INCR(ruleEgrExtNotIpv6.l2Encap);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtNotIpv6.macDa.arEther[0]),
                &(incrPtr->ruleEgrExtNotIpv6.macDa.arEther[0]),
                sizeof(basePtr->ruleEgrExtNotIpv6.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtNotIpv6.macSa.arEther[0]),
                &(incrPtr->ruleEgrExtNotIpv6.macSa.arEther[0]),
                sizeof(basePtr->ruleEgrExtNotIpv6.macSa.arEther),
                GT_TRUE);
            _INCR(ruleEgrExtNotIpv6.ipv4Fragmented);
            _INCR(ruleEgrExtNotIpv6.vid1);
            _INCR(ruleEgrExtNotIpv6.up1);
            _INCR(ruleEgrExtNotIpv6.cfi1);
            _INCR(ruleEgrExtNotIpv6.isMpls);
            _INCR(ruleEgrExtNotIpv6.numOfMplsLabels);
            _INCR(ruleEgrExtNotIpv6.protocolTypeAfterMpls);
            _INCR(ruleEgrExtNotIpv6.mplsLabel0);
            _INCR(ruleEgrExtNotIpv6.mplsExp0);
            _INCR(ruleEgrExtNotIpv6.mplsLabel1);
            _INCR(ruleEgrExtNotIpv6.mplsExp1);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtNotIpv6.udb12_19[0]),
                &(incrPtr->ruleEgrExtNotIpv6.udb12_19[0]),
                sizeof(basePtr->ruleEgrExtNotIpv6.udb12_19),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            cmdCpssDxChPclRuleEgrCommonIncrement(
                &(basePtr->ruleEgrExtIpv6L2.common),
                &(incrPtr->ruleEgrExtIpv6L2.common));
            cmdCpssDxChPclRuleEgrCommonExtIncrement(
                &(basePtr->ruleEgrExtIpv6L2.commonExt),
                &(incrPtr->ruleEgrExtIpv6L2.commonExt));
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv6L2.sip.arIP[0]),
                &(incrPtr->ruleEgrExtIpv6L2.sip.arIP[0]),
                sizeof(basePtr->ruleEgrExtIpv6L2.sip.arIP),
                GT_TRUE);
            _INCR(ruleEgrExtIpv6L2.dipBits127to120);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv6L2.macDa.arEther[0]),
                &(incrPtr->ruleEgrExtIpv6L2.macDa.arEther[0]),
                sizeof(basePtr->ruleEgrExtIpv6L2.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv6L2.macSa.arEther[0]),
                &(incrPtr->ruleEgrExtIpv6L2.macSa.arEther[0]),
                sizeof(basePtr->ruleEgrExtIpv6L2.macSa.arEther),
                GT_TRUE);
            _INCR(ruleEgrExtIpv6L2.vid1);
            _INCR(ruleEgrExtIpv6L2.up1);
            _INCR(ruleEgrExtIpv6L2.cfi1);
            _INCR(ruleEgrExtIpv6L2.isIpv6ExtHdrExist);
            _INCR(ruleEgrExtIpv6L2.isIpv6HopByHop);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv6L2.udb20_27[0]),
                &(incrPtr->ruleEgrExtIpv6L2.udb20_27[0]),
                sizeof(basePtr->ruleEgrExtIpv6L2.udb20_27),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            cmdCpssDxChPclRuleEgrCommonIncrement(
                &(basePtr->ruleEgrExtIpv6L4.common),
                &(incrPtr->ruleEgrExtIpv6L4.common));
            cmdCpssDxChPclRuleEgrCommonExtIncrement(
                &(basePtr->ruleEgrExtIpv6L4.commonExt),
                &(incrPtr->ruleEgrExtIpv6L4.commonExt));
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv6L4.sip.arIP[0]),
                &(incrPtr->ruleEgrExtIpv6L4.sip.arIP[0]),
                sizeof(basePtr->ruleEgrExtIpv6L4.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv6L4.dip.arIP[0]),
                &(incrPtr->ruleEgrExtIpv6L4.dip.arIP[0]),
                sizeof(basePtr->ruleEgrExtIpv6L4.dip.arIP),
                GT_TRUE);
            _INCR(ruleEgrExtIpv6L4.isIpv6ExtHdrExist);
            _INCR(ruleEgrExtIpv6L4.isIpv6HopByHop);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv6L4.udb28_35[0]),
                &(incrPtr->ruleEgrExtIpv6L4.udb28_35[0]),
                sizeof(basePtr->ruleEgrExtIpv6L4.udb28_35),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            cmdCpssDxChPclRuleIngrCommonUdbIncrement(
                &(basePtr->ruleIngrStdUdb.commonIngrUdb),
                &(incrPtr->ruleIngrStdUdb.commonIngrUdb));
            _INCR(ruleIngrStdUdb.isIpv4);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrStdUdb.udb0_15[0]),
                &(incrPtr->ruleIngrStdUdb.udb0_15[0]),
                sizeof(basePtr->ruleIngrStdUdb.udb0_15),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrStdUdb.udb35_38[0]),
                &(incrPtr->ruleIngrStdUdb.udb35_38[0]),
                sizeof(basePtr->ruleIngrStdUdb.udb35_38),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            cmdCpssDxChPclRuleIngrCommonUdbIncrement(
                &(basePtr->ruleIngrExtUdb.commonIngrUdb),
                &(incrPtr->ruleIngrExtUdb.commonIngrUdb));
            _INCR(ruleIngrExtUdb.isIpv6);
            _INCR(ruleIngrExtUdb.ipProtocol);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrExtUdb.sipBits31_0[0]),
                &(incrPtr->ruleIngrExtUdb.sipBits31_0[0]),
                sizeof(basePtr->ruleIngrExtUdb.sipBits31_0),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrExtUdb.sipBits79_32orMacSa[0]),
                &(incrPtr->ruleIngrExtUdb.sipBits79_32orMacSa[0]),
                sizeof(basePtr->ruleIngrExtUdb.sipBits79_32orMacSa),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrExtUdb.sipBits127_80orMacDa[0]),
                &(incrPtr->ruleIngrExtUdb.sipBits127_80orMacDa[0]),
                sizeof(basePtr->ruleIngrExtUdb.sipBits127_80orMacDa),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrExtUdb.dipBits127_112[0]),
                &(incrPtr->ruleIngrExtUdb.dipBits127_112[0]),
                sizeof(basePtr->ruleIngrExtUdb.dipBits127_112),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrExtUdb.dipBits31_0[0]),
                &(incrPtr->ruleIngrExtUdb.dipBits31_0[0]),
                sizeof(basePtr->ruleIngrExtUdb.dipBits31_0),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrExtUdb.udb0_15[0]),
                &(incrPtr->ruleIngrExtUdb.udb0_15[0]),
                sizeof(basePtr->ruleIngrExtUdb.udb0_15),
                GT_FALSE);
            _INCR(ruleIngrExtUdb.tag1Exist);
            _INCR(ruleIngrExtUdb.vid1);
            _INCR(ruleIngrExtUdb.up1);
            _INCR(ruleIngrExtUdb.cfi1);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleIngrExtUdb.udb23_30[0]),
                &(incrPtr->ruleIngrExtUdb.udb23_30[0]),
                sizeof(basePtr->ruleIngrExtUdb.udb23_30),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E:
            cmdCpssDxChPclRuleIngrCommonEnh1Increment(
                &(basePtr->ruleStdIpv4RoutedAclQos.ingressIpCommon),
                &(incrPtr->ruleStdIpv4RoutedAclQos.ingressIpCommon));
            _INCR(ruleStdIpv4RoutedAclQos.pktType);
            _INCR(ruleStdIpv4RoutedAclQos.ipFragmented);
            _INCR(ruleStdIpv4RoutedAclQos.ipHeaderInfo);
            _INCR(ruleStdIpv4RoutedAclQos.ipPacketLength);
            _INCR(ruleStdIpv4RoutedAclQos.ttl);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv4RoutedAclQos.sip.arIP[0]),
                &(incrPtr->ruleStdIpv4RoutedAclQos.sip.arIP[0]),
                sizeof(basePtr->ruleStdIpv4RoutedAclQos.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv4RoutedAclQos.dip.arIP[0]),
                &(incrPtr->ruleStdIpv4RoutedAclQos.dip.arIP[0]),
                sizeof(basePtr->ruleStdIpv4RoutedAclQos.dip.arIP),
                GT_TRUE);
            _INCR(ruleStdIpv4RoutedAclQos.tcpUdpPortComparators);
            _INCR(ruleStdIpv4RoutedAclQos.isUdbValid);
            _INCR(ruleStdIpv4RoutedAclQos.udb5);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleStdIpv4RoutedAclQos.udb41_44[0]),
                &(incrPtr->ruleStdIpv4RoutedAclQos.udb41_44[0]),
                sizeof(basePtr->ruleStdIpv4RoutedAclQos.udb41_44),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E:
            cmdCpssDxChPclRuleIngrCommonEnh1Increment(
                &(basePtr->ruleExtIpv4PortVlanQos.ingressIpCommon),
                &(incrPtr->ruleExtIpv4PortVlanQos.ingressIpCommon));
            _INCR(ruleExtIpv4PortVlanQos.isL2Valid);
            _INCR(ruleExtIpv4PortVlanQos.isBc);
            _INCR(ruleExtIpv4PortVlanQos.isIp);
            _INCR(ruleExtIpv4PortVlanQos.isArp);
            _INCR(ruleExtIpv4PortVlanQos.l2Encap);
            _INCR(ruleExtIpv4PortVlanQos.etherType);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.macDa.arEther[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.macDa.arEther[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.macSa.arEther[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.macSa.arEther[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.macSa.arEther),
                GT_TRUE);
            _INCR(ruleExtIpv4PortVlanQos.tag1Exist);
            _INCR(ruleExtIpv4PortVlanQos.vid1);
            _INCR(ruleExtIpv4PortVlanQos.up1);
            _INCR(ruleExtIpv4PortVlanQos.cfi1);
            _INCR(ruleExtIpv4PortVlanQos.ipFragmented);
            _INCR(ruleExtIpv4PortVlanQos.ipHeaderInfo);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.sip.arIP[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.sip.arIP[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.dip.arIP[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.dip.arIP[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.dip.arIP),
                GT_TRUE);
            _INCR(ruleExtIpv4PortVlanQos.isUdbValid);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.UdbStdIpL2Qos[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.UdbStdIpL2Qos[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.UdbStdIpL2Qos),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.UdbStdIpV4L4[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.UdbStdIpV4L4[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.UdbStdIpV4L4),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.UdbExtIpv6L2[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.UdbExtIpv6L2[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.UdbExtIpv6L2),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.UdbExtIpv6L4[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.UdbExtIpv6L4[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.UdbExtIpv6L4),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.udb5_16[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.udb5_16[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.udb5_16),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleExtIpv4PortVlanQos.udb31_38[0]),
                &(incrPtr->ruleExtIpv4PortVlanQos.udb31_38[0]),
                sizeof(basePtr->ruleExtIpv4PortVlanQos.udb31_38),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E:
            cmdCpssDxChPclRuleIngrCommonEnh1Increment(
                &(basePtr->ruleUltraIpv6PortVlanQos.ingressIpCommon),
                &(incrPtr->ruleUltraIpv6PortVlanQos.ingressIpCommon));
            _INCR(ruleUltraIpv6PortVlanQos.isL2Valid);
            _INCR(ruleUltraIpv6PortVlanQos.isNd);
            _INCR(ruleUltraIpv6PortVlanQos.isBc);
            _INCR(ruleUltraIpv6PortVlanQos.isIp);
            _INCR(ruleUltraIpv6PortVlanQos.l2Encap);
            _INCR(ruleUltraIpv6PortVlanQos.etherType);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.macDa.arEther[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.macDa.arEther[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.macDa.arEther),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.macSa.arEther[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.macSa.arEther[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.macSa.arEther),
                GT_TRUE);
            _INCR(ruleUltraIpv6PortVlanQos.tag1Exist);
            _INCR(ruleUltraIpv6PortVlanQos.vid1);
            _INCR(ruleUltraIpv6PortVlanQos.up1);
            _INCR(ruleUltraIpv6PortVlanQos.cfi1);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.sip.arIP[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.sip.arIP[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.dip.arIP[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.dip.arIP[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.dip.arIP),
                GT_TRUE);
            _INCR(ruleUltraIpv6PortVlanQos.isIpv6ExtHdrExist);
            _INCR(ruleUltraIpv6PortVlanQos.isIpv6HopByHop);
            _INCR(ruleUltraIpv6PortVlanQos.ipHeaderOk);
            _INCR(ruleUltraIpv6PortVlanQos.isUdbValid);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.UdbExtNotIpv6[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.UdbExtNotIpv6[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.UdbExtNotIpv6),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.UdbExtIpv6L2[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.UdbExtIpv6L2[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.UdbExtIpv6L2),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.UdbExtIpv6L4[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.UdbExtIpv6L4[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.UdbExtIpv6L4),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.udb0_11[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.udb0_11[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.udb0_11),
                GT_FALSE);
            _INCR(ruleUltraIpv6PortVlanQos.udb12);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6PortVlanQos.udb39_40[0]),
                &(incrPtr->ruleUltraIpv6PortVlanQos.udb39_40[0]),
                sizeof(basePtr->ruleUltraIpv6PortVlanQos.udb39_40),
                GT_FALSE);
            _INCR(ruleUltraIpv6PortVlanQos.srcPortOrTrunk);
            _INCR(ruleUltraIpv6PortVlanQos.srcIsTrunk);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E:
            cmdCpssDxChPclRuleIngrCommonEnh1Increment(
                &(basePtr->ruleUltraIpv6RoutedAclQos.ingressIpCommon),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.ingressIpCommon));
            _INCR(ruleUltraIpv6RoutedAclQos.pktType);
            _INCR(ruleUltraIpv6RoutedAclQos.isNd);
            _INCR(ruleUltraIpv6RoutedAclQos.tag1Exist);
            _INCR(ruleUltraIpv6RoutedAclQos.vid1);
            _INCR(ruleUltraIpv6RoutedAclQos.up1);
            _INCR(ruleUltraIpv6RoutedAclQos.cfi1);
            _INCR(ruleUltraIpv6RoutedAclQos.mplsOuterLabel);
            _INCR(ruleUltraIpv6RoutedAclQos.mplsOuterLabExp);
            _INCR(ruleUltraIpv6RoutedAclQos.mplsOuterLabSBit);
            _INCR(ruleUltraIpv6RoutedAclQos.ipPacketLength);
            _INCR(ruleUltraIpv6RoutedAclQos.ipv6HdrFlowLabel);
            _INCR(ruleUltraIpv6RoutedAclQos.ttl);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.sip.arIP[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.sip.arIP[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.dip.arIP[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.dip.arIP[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.dip.arIP),
                GT_TRUE);
            _INCR(ruleUltraIpv6RoutedAclQos.isIpv6ExtHdrExist);
            _INCR(ruleUltraIpv6RoutedAclQos.isIpv6HopByHop);
            _INCR(ruleUltraIpv6RoutedAclQos.isIpv6LinkLocal);
            _INCR(ruleUltraIpv6RoutedAclQos.isIpv6Mld);
            _INCR(ruleUltraIpv6RoutedAclQos.ipHeaderOk);
            _INCR(ruleUltraIpv6RoutedAclQos.isUdbValid);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.UdbStdNotIp[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.UdbStdNotIp[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.UdbStdNotIp),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.UdbStdIpL2Qos[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.UdbStdIpL2Qos[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.UdbStdIpL2Qos),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.UdbStdIpV4L4[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.UdbStdIpV4L4[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.UdbStdIpV4L4),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.UdbExtNotIpv6[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.UdbExtNotIpv6[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.UdbExtNotIpv6),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.UdbExtIpv6L2[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.UdbExtIpv6L2[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.UdbExtIpv6L2),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.UdbExtIpv6L4[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.UdbExtIpv6L4[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.UdbExtIpv6L4),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.udb0_11[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.udb0_11[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.udb0_11),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.udb17_22[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.udb17_22[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.udb17_22),
                GT_FALSE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleUltraIpv6RoutedAclQos.udb45_46[0]),
                &(incrPtr->ruleUltraIpv6RoutedAclQos.udb45_46[0]),
                sizeof(basePtr->ruleUltraIpv6RoutedAclQos.udb45_46),
                GT_FALSE);
            _INCR(ruleUltraIpv6RoutedAclQos.srcPortOrTrunk);
            _INCR(ruleUltraIpv6RoutedAclQos.srcIsTrunk);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E:
            cmdCpssDxChPclRuleEgrCommonEnh1Increment(
                &(basePtr->ruleEgrExtIpv4RaclVacl.egressIpCommon),
                &(incrPtr->ruleEgrExtIpv4RaclVacl.egressIpCommon));
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv4RaclVacl.sip.arIP[0]),
                &(incrPtr->ruleEgrExtIpv4RaclVacl.sip.arIP[0]),
                sizeof(basePtr->ruleEgrExtIpv4RaclVacl.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv4RaclVacl.dip.arIP[0]),
                &(incrPtr->ruleEgrExtIpv4RaclVacl.dip.arIP[0]),
                sizeof(basePtr->ruleEgrExtIpv4RaclVacl.dip.arIP),
                GT_TRUE);
            _INCR(ruleEgrExtIpv4RaclVacl.ipv4Options);
            _INCR(ruleEgrExtIpv4RaclVacl.isVidx);
            _INCR(ruleEgrExtIpv4RaclVacl.vid1);
            _INCR(ruleEgrExtIpv4RaclVacl.up1);
            _INCR(ruleEgrExtIpv4RaclVacl.cfi1);
            _INCR(ruleEgrExtIpv4RaclVacl.isUdbValid);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrExtIpv4RaclVacl.udb36_49[0]),
                &(incrPtr->ruleEgrExtIpv4RaclVacl.udb36_49[0]),
                sizeof(basePtr->ruleEgrExtIpv4RaclVacl.udb36_49),
                GT_FALSE);
            _INCR(ruleEgrExtIpv4RaclVacl.udb0);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E:
            cmdCpssDxChPclRuleEgrCommonEnh1Increment(
                &(basePtr->ruleEgrUltraIpv6RaclVacl.egressIpCommon),
                &(incrPtr->ruleEgrUltraIpv6RaclVacl.egressIpCommon));
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrUltraIpv6RaclVacl.sip.arIP[0]),
                &(incrPtr->ruleEgrUltraIpv6RaclVacl.sip.arIP[0]),
                sizeof(basePtr->ruleEgrUltraIpv6RaclVacl.sip.arIP),
                GT_TRUE);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrUltraIpv6RaclVacl.dip.arIP[0]),
                &(incrPtr->ruleEgrUltraIpv6RaclVacl.dip.arIP[0]),
                sizeof(basePtr->ruleEgrUltraIpv6RaclVacl.dip.arIP),
                GT_TRUE);
            _INCR(ruleEgrUltraIpv6RaclVacl.isNd);
            _INCR(ruleEgrUltraIpv6RaclVacl.isIpv6ExtHdrExist);
            _INCR(ruleEgrUltraIpv6RaclVacl.isIpv6HopByHop);
            _INCR(ruleEgrUltraIpv6RaclVacl.isVidx);
            _INCR(ruleEgrUltraIpv6RaclVacl.vid1);
            _INCR(ruleEgrUltraIpv6RaclVacl.up1);
            _INCR(ruleEgrUltraIpv6RaclVacl.cfi1);
            _INCR(ruleEgrUltraIpv6RaclVacl.srcPort);
            _INCR(ruleEgrUltraIpv6RaclVacl.trgPort);
            _INCR(ruleEgrUltraIpv6RaclVacl.isUdbValid);
            cmdCpssDxChByteArrayIncrement(
                &(basePtr->ruleEgrUltraIpv6RaclVacl.udb1_4[0]),
                &(incrPtr->ruleEgrUltraIpv6RaclVacl.udb1_4[0]),
                sizeof(basePtr->ruleEgrUltraIpv6RaclVacl.udb1_4),
                GT_FALSE);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E:
            for (i = 0; (i < CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS); i++)
            {
                _INCR(ruleIngrUdbOnly.udb[i]);
            }
            _INCR(ruleIngrUdbOnly.udb60FixedFld.isUdbValid);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.pclId);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.vid);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.srcPort);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.srcDevIsOwn);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.vid1);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.up1);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.macToMe);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.qosProfile);
            _INCR(ruleIngrUdbOnly.udb60FixedFld.flowId);
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E:
            for (i = 0; (i < CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS); i++)
            {
                _INCR(ruleEgrUdbOnly.udb[i]);
            }
            _INCR(ruleEgrUdbOnly.udb60FixedFld.pclId);
            _INCR(ruleEgrUdbOnly.udb60FixedFld.isUdbValid);
            _INCR(ruleEgrUdbOnly.udb60FixedFld.vid);
            _INCR(ruleEgrUdbOnly.udb60FixedFld.srcPort);
            _INCR(ruleEgrUdbOnly.udb60FixedFld.trgPort);
            _INCR(ruleEgrUdbOnly.udb60FixedFld.srcDev);
            _INCR(ruleEgrUdbOnly.udb60FixedFld.trgDev);
            _INCR(ruleEgrUdbOnly.udb60FixedFld.localDevTrgPhyPort);
            break;
        default: return GT_FAIL;
    }
    cmdCpssDxChPclRuleClearOvelappingFields(
        ruleFormat, basePtr);
    return GT_OK;
    #undef  _INCR
}

/*******************************************************************************
* cmdCpssDxChPclActionIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       ruleFormat      - Rule Format
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChPclActionIncrement
(
    INOUT CPSS_DXCH_PCL_ACTION_STC             *basePtr,
    IN    CPSS_DXCH_PCL_ACTION_STC             *incrPtr
)
{
    GT_STATUS rc = GT_OK; /* return code */
    #define _INCR(field) basePtr->field += incrPtr->field
    _INCR( flowId);
    _INCR(tmQueueId);
    _INCR(lookupConfig.ipclConfigIndex);
    if ((incrPtr->egressPolicy != GT_FALSE)
        && (incrPtr->mirror.mirrorToRxAnalyzerPort != GT_FALSE))
    {
        _INCR(mirror.ingressMirrorToAnalyzerIndex);
    }
    if (incrPtr->matchCounter.enableMatchCount != GT_FALSE)
    {
        _INCR(matchCounter.matchCounterIndex);
    }
    if (incrPtr->egressPolicy == GT_FALSE)
    {
        /* ingress */
        if (incrPtr->qos.ingress.profileAssignIndex != GT_FALSE)
        {
            _INCR(qos.ingress.profileIndex);
        }
        if (incrPtr->qos.ingress.up1Cmd !=
            CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_DO_NOT_MODIFY_E)
        {
            _INCR(qos.ingress.up1);
        }
    }
    else
    {
        /* egress */
        if (incrPtr->qos.egress.modifyDscp !=
            CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
        {
            _INCR(qos.egress.dscp);
        }
        if (incrPtr->qos.egress.modifyUp !=
            CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
        {
            _INCR(qos.egress.up);
        }
        if (incrPtr->qos.egress.up1ModifyEnable != GT_FALSE)
        {
            _INCR(qos.egress.up1);
        }
    }
    if (incrPtr->egressPolicy == GT_FALSE)
    {
        /* ingress */
        switch (incrPtr->redirect.redirectCmd)
        {
            case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E:
                if (GT_OK != cmdCpssDxChGenIntrfaceInfoIncrement(
                    &(basePtr->redirect.data.outIf.outInterface),
                    &(incrPtr->redirect.data.outIf.outInterface)))
                {
                    rc = GT_FAIL;
                }
                if (incrPtr->redirect.data.outIf.tunnelStart != GT_FALSE)
                {
                    _INCR(redirect.data.outIf.tunnelPtr);
                }
                else
                {
                    _INCR(redirect.data.outIf.arpPtr);
                }

                break;
            case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E:
                _INCR(redirect.data.routerLttIndex);
                break;
            case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E:
                _INCR(redirect.data.vrfId);
                break;
            case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_REPLACE_MAC_SA_E:
                _INCR(redirect.data.modifyMacSa.arpPtr);
                cmdCpssDxChByteArrayIncrement(
                    &(basePtr->redirect.data.modifyMacSa.macSa.arEther[0]),
                    &(incrPtr->redirect.data.modifyMacSa.macSa.arEther[0]),
                    sizeof(basePtr->redirect.data.modifyMacSa.macSa.arEther),
                    GT_TRUE);
                break;
            case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_LOGICAL_PORT_ASSIGN_E:
                if (GT_OK != cmdCpssDxChGenIntrfaceInfoIncrement(
                    &(basePtr->redirect.data.logicalSourceInterface.logicalInterface),
                    &(incrPtr->redirect.data.logicalSourceInterface.logicalInterface)))
                {
                    rc = GT_FAIL;
                }
                if (incrPtr->redirect.data.logicalSourceInterface.sourceMeshIdSetEnable
                    != GT_FALSE)
                {
                    _INCR(redirect.data.logicalSourceInterface.sourceMeshId);
                }
                break;
            case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E:
                if (incrPtr->redirect.data.noRedirect.modifyMacDa
                    != GT_FALSE)
                {
                    _INCR(redirect.data.noRedirect.arpPtr);
                }
                break;
            default:
                break;
        }
    }
    if (incrPtr->policer.policerEnable !=
        CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E)
    {
        _INCR(policer.policerId);
    }
    if (incrPtr->egressPolicy == GT_FALSE)
    {
        /* ingress */
        if (incrPtr->vlan.ingress.modifyVlan !=
            CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
        {
            _INCR(vlan.ingress.vlanId);
        }
        if (incrPtr->vlan.ingress.vlanId1Cmd !=
            CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_DO_NOT_MODIFY_E)
        {
            _INCR(vlan.ingress.vlanId1);
        }
    }
    else
    {
        /* egress */
        if (incrPtr->vlan.egress.vlanCmd !=
            CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
        {
            _INCR(vlan.egress.vlanId);
        }
        if (incrPtr->vlan.egress.vlanId1ModifyEnable !=
            GT_FALSE)
        {
            _INCR(vlan.egress.vlanId1);
        }
    }
    if (incrPtr->egressPolicy == GT_FALSE)
    {
        if (incrPtr->ipUcRoute.doIpUcRoute != GT_FALSE)
        {
            _INCR(ipUcRoute.arpDaIndex);
        }
    }
    if (incrPtr->egressPolicy == GT_FALSE)
    {
        if (incrPtr->sourceId.assignSourceId != GT_FALSE)
        {
            _INCR(sourceId.sourceIdValue);
        }
    }
    if (incrPtr->oam.timeStampEnable != GT_FALSE)
    {
        _INCR(oam.offsetIndex);
    }
    if (incrPtr->oam.oamProcessEnable != GT_FALSE)
    {
        _INCR(oam.oamProfile);
    }
    if (incrPtr->sourcePort.assignSourcePortEnable != GT_FALSE)
    {
        _INCR(sourcePort.sourcePortValue);
    }
    #undef  _INCR
    return rc;
}

/*******************************************************************************
* cmdCpssDxChPclRuleSequenceWrite
*
* DESCRIPTION:
*       Write sequence of incremented PclRules.
*       The rule mask copied unchanged, pattern and action incremented.
*
* INPUTS:
*       devNum            - device number
*       ruleFormat        - format of all related rules.
*       baseRuleIndex     - index of rule to be retrieved and used as base values.
*                           this entry also will be start rule of the sequence
*       incrRuleIndex     - index of rule to be retrieved and used as increment values.
*                           retieved at the beginnig and can be overriden by sequence.
*       amountToWrite     - amount of rules to write
*       writeIndexIncr    - signed increment of sequence rule index
*
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, other on error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChPclRuleSequenceWrite
(
    IN   GT_U8                              devNum,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN   GT_U32                             baseRuleIndex,
    IN   GT_U32                             incrRuleIndex,
    IN   GT_U32                             amountToWrite,
    IN   GT_32                              writeIndexIncr
)
{
    GT_STATUS rc;                              /* return code */
    CPSS_DXCH_PCL_ACTION_STC baseAction;       /* base action */
    CPSS_DXCH_PCL_ACTION_STC incrAction;       /* action with incrementing values */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT baseMask;    /* base rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT incrMask;    /* rule mask of incrementing rule */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT basePattern; /* base rule pattern */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT incrPattern; /* rule pattern with incrementing values */
    GT_U32                  index;             /* current entry index */
    GT_U32                  i;                 /* loop index */
    GT_BOOL                 isRuleValid;       /* is rule valid*/
    CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        ruleType;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        ruleData;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC  ruleAttributes;

    ruleType.ruleType = CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E;
    ruleType.rule.pcl.ruleFormat = ruleFormat;

    if (wrCpssDxChVirtualTcam_useVTcamApi)
    {
        ruleData.rule.pcl.actionPtr = &baseAction;
        ruleData.rule.pcl.patternPtr = &basePattern;
        ruleData.rule.pcl.maskPtr = &baseMask;

        rc = cpssDxChVirtualTcamRuleRead(wrCpssDxChVirtualTcam_vTcamMngId, 
                                            wrCpssDxChVirtualTcam_vTcamId,
                                            baseRuleIndex,
                                            &ruleType,
                                            &ruleData);
        if (rc != GT_OK)
        {
            return rc;
        }

        ruleData.rule.pcl.actionPtr = &incrAction;
        ruleData.rule.pcl.patternPtr = &incrPattern;
        ruleData.rule.pcl.maskPtr = &incrMask;

        rc = cpssDxChVirtualTcamRuleRead(wrCpssDxChVirtualTcam_vTcamMngId, 
                                            wrCpssDxChVirtualTcam_vTcamId,
                                            incrRuleIndex,
                                            &ruleType,
                                            &ruleData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {

        rc = cpssDxChPclRuleParsedGet(
            devNum, ruleFormat, baseRuleIndex, 0 /*ruleOptionsBmp*/,
            &isRuleValid, &baseMask, &basePattern, &baseAction);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPclRuleParsedGet(
            devNum, ruleFormat, incrRuleIndex, 0 /*ruleOptionsBmp*/,
            &isRuleValid, &incrMask, &incrPattern, &incrAction);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    ruleAttributes.priority = wrCpssDxChVirtualTcam_vTcamRulePrio;
    ruleData.valid = GT_TRUE;
    ruleData.rule.pcl.actionPtr = &baseAction;
    ruleData.rule.pcl.patternPtr = &basePattern;
    ruleData.rule.pcl.maskPtr = &baseMask;

    for (i = 1; (i < amountToWrite); i++)
    {
        rc = cmdCpssDxChPclRuleIncrement(
            ruleFormat, &basePattern, &incrPattern);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cmdCpssDxChPclActionIncrement(
            &baseAction, &incrAction);
        if (rc != GT_OK)
        {
            return rc;
        }
        index = (GT_U32)(baseRuleIndex + (writeIndexIncr * i));

        if (wrCpssDxChVirtualTcam_useVTcamApi)
        {            
            rc = cpssDxChVirtualTcamRuleWrite(wrCpssDxChVirtualTcam_vTcamMngId, 
                                                wrCpssDxChVirtualTcam_vTcamId,
                                                index,
                                                &ruleAttributes,
                                                &ruleType,
                                                &ruleData);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            rc = cpssDxChPclRuleSet(
                devNum, ruleFormat, index, 0 /*ruleOptionsBmp*/,
                &baseMask, &basePattern, &baseAction);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}


