/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfBridgeGen.c
*
* DESCRIPTION:
*       Generic API implementation for Bridge
*
* FILE REVISION NUMBER:
*       $Revision: 146 $
*
*******************************************************************************/
#ifdef COW_INTERNAL_COMPILE
    /* those 3 are defined , so remove them */
    #undef EXMXPM_FAMILY
    #undef EX_FAMILY
    #undef CHX_FAMILY

    /* define only the tested one */
    #define CHX_FAMILY

#endif /*COW_INTERNAL_COMPILE*/

#define UNUSED(x)        ((void)(x))        /* to avoid warnings */

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <extUtils/tgf/tgfBridgeGen.h>
#include <extUtils/tgf/tgfCommonList.h>
#include <extUtils/tgf/prvUtfExtras.h>
#include <extUtils/auEventHandler/auFdbUpdateLock.h>
#include <extUtils/auEventHandler/auEventHandler.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgGen.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdbHash.h>
#endif /*EXMXPM_FAMILY*/

#include <extUtils/trafficEngine/tgfTrafficEngine.h>
#include <extUtils/trafficEngine/private/prvTgfTrafficParser.h>
#include <extUtils/trafficEngine/tgfTrafficGenerator.h>
#include <extUtils/trafficEngine/prvTgfLog.h>

/* list of valid messages - for NA from the Appdemo for auto/controlled learning */
PRV_TGF_LIST_HEAD_STC brgFdbAuqMessagesPart1 ={
    NULL, NULL,
    sizeof(PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC),
    0, _2K
};

/* indication that the FDB shadow used */
GT_BOOL prvTgfFdbShadowUsed = GT_FALSE;

#ifdef CHX_FAMILY

GT_U32   prvTgfBrgFdbDoNaShadow_part2 = 0;

/* list of valid messages - for entries set by the test */
PRV_TGF_LIST_HEAD_STC brgFdbAuqMessagesPart2 ={
    NULL, NULL,
    sizeof(PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC),
    0, _2K
};

extern GT_STATUS appDemoBrgFdbIpv6UcFreeAddrDataLinkedList
(
    IN GT_U8    devNum
);

extern GT_STATUS prvAppDemoBrgFdbIpv6UcFindPairIndexToDelete
(
    IN  GT_U8                           devNum,
    IN  CPSS_MAC_ENTRY_EXT_TYPE_ENT     entryType,
    IN  GT_U32                          index,
    IN  GT_BOOL                         deleteElement,
    OUT GT_U32                          *pairIndexPtr
);

extern GT_BOOL prvUtfIsGmCompilation
(
    void
);

#endif /*CHX_FAMILY*/

/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/
GT_STATUS prvTgfConvertCpssToGenericMacEntry
(
    IN  CPSS_MAC_ENTRY_EXT_STC       *cpssMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChMacEntryKey
*
* DESCRIPTION:
*       Convert generic MAC entry key into device specific MAC entry key
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) MAC entry key parameters
*
* OUTPUTS:
*       dxChMacEntryKeyPtr - (pointer to) DxCh MAC entry key parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToDxChMacEntryKey
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT CPSS_MAC_ENTRY_EXT_KEY_STC   *dxChMacEntryKeyPtr
)
{
    cpssOsMemSet(dxChMacEntryKeyPtr, 0, sizeof(*dxChMacEntryKeyPtr));

    /* convert entry type into device specific format */
    switch (macEntryKeyPtr->entryType)
    {
        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
            break;
        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E;
            break;


        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_FID_VID1_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E;
            break;
        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_FID_VID1_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E;
            break;
        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_FID_VID1_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E;
            break;


        default:
            return GT_BAD_PARAM;
    }


    /* convert key data into device specific format */
    if (PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E == macEntryKeyPtr->entryType ||
        PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_FID_VID1_E == macEntryKeyPtr->entryType)
    {
        cpssOsMemCpy(dxChMacEntryKeyPtr->key.macVlan.macAddr.arEther,
                     macEntryKeyPtr->key.macVlan.macAddr.arEther,
                     sizeof(dxChMacEntryKeyPtr->key.macVlan.macAddr));
        dxChMacEntryKeyPtr->key.macVlan.vlanId = macEntryKeyPtr->key.macVlan.vlanId;
    }
    else
    {
        if (PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E == macEntryKeyPtr->entryType)
        {
            cpssOsMemCpy(&dxChMacEntryKeyPtr->key.ipv4Unicast,
                         &macEntryKeyPtr->key.ipv4Unicast,
                         sizeof(dxChMacEntryKeyPtr->key.ipv4Unicast));
        }
        else
        {
            if ( (PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E == macEntryKeyPtr->entryType) ||
                 (PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E == macEntryKeyPtr->entryType) )
            {
                cpssOsMemCpy(&dxChMacEntryKeyPtr->key.ipv6Unicast,
                             &macEntryKeyPtr->key.ipv6Unicast,
                             sizeof(dxChMacEntryKeyPtr->key.ipv6Unicast));
            }
            else
            {
                cpssOsMemCpy(dxChMacEntryKeyPtr->key.ipMcast.sip,
                             macEntryKeyPtr->key.ipMcast.sip,
                             sizeof(dxChMacEntryKeyPtr->key.ipMcast.sip));
                cpssOsMemCpy(dxChMacEntryKeyPtr->key.ipMcast.dip,
                             macEntryKeyPtr->key.ipMcast.dip,
                             sizeof(dxChMacEntryKeyPtr->key.ipMcast.dip));
                dxChMacEntryKeyPtr->key.ipMcast.vlanId = macEntryKeyPtr->key.ipMcast.vlanId;
            }
        }
    }

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryKeyPtr, macEntryKeyPtr, vid1);


    return GT_OK;
}


/*******************************************************************************
* prvTgfConvertCpssToGenericMacKey
*
* DESCRIPTION:
*       Convert DX MAC entry key into device generic MAC entry key
*
* INPUTS:
*       dxChMacEntryKeyPtr - (pointer to) DxCh MAC entry key parameters
*
* OUTPUTS:
*       macEntryKeyPtr - (pointer to) MAC entry key parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertCpssToGenericMacKey
(
    IN CPSS_MAC_ENTRY_EXT_KEY_STC       *dxChMacEntryKeyPtr,
    OUT  PRV_TGF_MAC_ENTRY_KEY_STC      *macEntryKeyPtr
)
{
    cpssOsMemSet(macEntryKeyPtr, 0, sizeof(*macEntryKeyPtr));

    macEntryKeyPtr->vid1 =  dxChMacEntryKeyPtr->vid1;

    /* convert entry type into device specific format */
    switch (dxChMacEntryKeyPtr->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            macEntryKeyPtr->entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
            macEntryKeyPtr->entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            macEntryKeyPtr->entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E;
            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
            macEntryKeyPtr->entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            macEntryKeyPtr->entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            macEntryKeyPtr->entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
            macEntryKeyPtr->entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_FID_VID1_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
            macEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            macEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E;
            break;
        default:
            return GT_BAD_PARAM;
    }


    /* convert key data into device specific format */
    if (PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E == macEntryKeyPtr->entryType ||
        PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_FID_VID1_E == macEntryKeyPtr->entryType)
    {
        cpssOsMemCpy(macEntryKeyPtr->key.macVlan.macAddr.arEther,
                     dxChMacEntryKeyPtr->key.macVlan.macAddr.arEther,
                     sizeof(macEntryKeyPtr->key.macVlan.macAddr));
        macEntryKeyPtr->key.macVlan.vlanId = dxChMacEntryKeyPtr->key.macVlan.vlanId;
    }
    else
    {
        if (PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E == macEntryKeyPtr->entryType)
        {
            cpssOsMemCpy(&macEntryKeyPtr->key.ipv4Unicast,
                         &dxChMacEntryKeyPtr->key.ipv4Unicast,
                         sizeof(macEntryKeyPtr->key.ipv4Unicast));

        }
        else
        {
            if ( (PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E == macEntryKeyPtr->entryType) ||
                 (PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E == macEntryKeyPtr->entryType) )
            {
                cpssOsMemCpy(&macEntryKeyPtr->key.ipv6Unicast,
                             &dxChMacEntryKeyPtr->key.ipv6Unicast,
                             sizeof(macEntryKeyPtr->key.ipv6Unicast));
            }
            else
            {
                cpssOsMemCpy(macEntryKeyPtr->key.ipMcast.sip,
                             dxChMacEntryKeyPtr->key.ipMcast.sip,
                             sizeof(macEntryKeyPtr->key.ipMcast.sip));
                cpssOsMemCpy(macEntryKeyPtr->key.ipMcast.dip,
                             dxChMacEntryKeyPtr->key.ipMcast.dip,
                             sizeof(macEntryKeyPtr->key.ipMcast.dip));
                macEntryKeyPtr->key.ipMcast.vlanId = dxChMacEntryKeyPtr->key.ipMcast.vlanId;
            }
        }
    }


    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChMacEntry
*
* DESCRIPTION:
*       Convert generic MAC entry into device specific MAC entry
*
* INPUTS:
*       devNum      - device number
*       macEntryPtr - (pointer to) MAC entry parameters
*
* OUTPUTS:
*       dxChMacEntryPtr - (pointer to) DxCh MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToDxChMacEntry
(
    IN  GT_U8                         devNum,
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC       *dxChMacEntryPtr
)
{
    GT_STATUS rc; /* return code */

    cpssOsMemSet(dxChMacEntryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));

    /* convert entry type into device specific format */
    switch (macEntryPtr->key.entryType)
    {
        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
            break;
        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E;
            break;
        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E;
            break;
        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E;
            break;
        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_FID_VID1_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_FID_VID1_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_FID_VID1_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E;
            break;

    default:
        return GT_BAD_PARAM;
    }

    /* convert key data into device specific format */
    if (PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E == macEntryPtr->key.entryType)
    {
        cpssOsMemCpy(dxChMacEntryPtr->key.key.macVlan.macAddr.arEther,
                     macEntryPtr->key.key.macVlan.macAddr.arEther,
                     sizeof(dxChMacEntryPtr->key.key.macVlan.macAddr));
        dxChMacEntryPtr->key.key.macVlan.vlanId = macEntryPtr->key.key.macVlan.vlanId;
    }
    else
    {
        if (PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E == macEntryPtr->key.entryType)
        {
            cpssOsMemCpy(&dxChMacEntryPtr->key.key.ipv4Unicast,
                         &macEntryPtr->key.key.ipv4Unicast,
                         sizeof(dxChMacEntryPtr->key.key.ipv4Unicast));

        }
        else
        {
            if ( (PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E == macEntryPtr->key.entryType) ||
                 (PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E == macEntryPtr->key.entryType) )
            {
                cpssOsMemCpy(&dxChMacEntryPtr->key.key.ipv6Unicast,
                             &macEntryPtr->key.key.ipv6Unicast,
                             sizeof(dxChMacEntryPtr->key.key.ipv6Unicast));
            }
            else
            {
                cpssOsMemCpy(dxChMacEntryPtr->key.key.ipMcast.sip,
                             macEntryPtr->key.key.ipMcast.sip,
                             sizeof(dxChMacEntryPtr->key.key.ipMcast.sip));
                cpssOsMemCpy(dxChMacEntryPtr->key.key.ipMcast.dip,
                             macEntryPtr->key.key.ipMcast.dip,
                             sizeof(dxChMacEntryPtr->key.key.ipMcast.dip));
                dxChMacEntryPtr->key.key.ipMcast.vlanId = macEntryPtr->key.key.ipMcast.vlanId;
            }
        }
    }

    /* convert interface info into device specific format */
    dxChMacEntryPtr->dstInterface.type = macEntryPtr->dstInterface.type;
    switch (dxChMacEntryPtr->dstInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            rc = prvUtfHwDeviceNumberGet(macEntryPtr->dstInterface.devPort.hwDevNum,
                                         &dxChMacEntryPtr->dstInterface.devPort.hwDevNum);
            if(GT_OK != rc)
                return rc;
            dxChMacEntryPtr->dstInterface.devPort.portNum = macEntryPtr->dstInterface.devPort.portNum;
            break;

        case CPSS_INTERFACE_TRUNK_E:
            dxChMacEntryPtr->dstInterface.trunkId = macEntryPtr->dstInterface.trunkId;
            rc = prvUtfHwDeviceNumberGet(devNum,
                                         &dxChMacEntryPtr->dstInterface.hwDevNum);
            if(GT_OK != rc)
                return rc;
            break;

        case CPSS_INTERFACE_VIDX_E:
            dxChMacEntryPtr->dstInterface.vidx = macEntryPtr->dstInterface.vidx;
            break;

        case CPSS_INTERFACE_VID_E:
            dxChMacEntryPtr->dstInterface.vlanId = macEntryPtr->dstInterface.vlanId;
            break;

        case CPSS_INTERFACE_DEVICE_E:
            dxChMacEntryPtr->dstInterface.hwDevNum = macEntryPtr->dstInterface.hwDevNum;
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            dxChMacEntryPtr->dstInterface.fabricVidx = macEntryPtr->dstInterface.fabricVidx;
            break;

        default:
            return GT_BAD_PARAM;
    }

    if ( (PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E == macEntryPtr->key.entryType) ||
         (PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E == macEntryPtr->key.entryType) ||
         (PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E == macEntryPtr->key.entryType) )
    {
        cpssOsMemCpy(&dxChMacEntryPtr->fdbRoutingInfo,
                     &macEntryPtr->fdbRoutingInfo,
                     sizeof(dxChMacEntryPtr->fdbRoutingInfo));

        dxChMacEntryPtr->age = macEntryPtr->age;

        return GT_OK;
    }

    dxChMacEntryPtr->age = GT_TRUE;

    dxChMacEntryPtr->sourceID = macEntryPtr->sourceId;

    /* convert daCommand info into device specific format */
    switch (macEntryPtr->daCommand)
    {
        case PRV_TGF_PACKET_CMD_FORWARD_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_FRWRD_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_HARD_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_DROP_E;
            break;

        case PRV_TGF_PACKET_CMD_INTERV_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_INTERV_E;
            break;

        case PRV_TGF_PACKET_CMD_CNTL_E:
        case PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_CNTL_E;
            break;

        case PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_SOFT_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_SOFT_DROP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert saCommand info into device specific format */
    switch (macEntryPtr->saCommand)
    {
        case PRV_TGF_PACKET_CMD_FORWARD_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_FRWRD_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_HARD_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_DROP_E;
            break;

        case PRV_TGF_PACKET_CMD_INTERV_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_INTERV_E;
            break;

        case PRV_TGF_PACKET_CMD_CNTL_E:
        case PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_CNTL_E;
            break;

        case PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_SOFT_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_SOFT_DROP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }


    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, key.vid1);

    /* convert MAC entry info into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, isStatic);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, daRoute);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, mirrorToRxAnalyzerPortEn);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, userDefined);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, daQosIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, saQosIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, daSecurityLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, saSecurityLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, appSpecificCpuCode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, spUnknown);

    /* need to update TGF API for new eArch fields */
    CPSS_TBD_BOOKMARK_EARCH;
    dxChMacEntryPtr->saMirrorToRxAnalyzerPortEn = dxChMacEntryPtr->mirrorToRxAnalyzerPortEn;
    dxChMacEntryPtr->daMirrorToRxAnalyzerPortEn = dxChMacEntryPtr->mirrorToRxAnalyzerPortEn;
    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       dxChMacEntryPtr - (pointer to) DxCh MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertDxChToGenericMacEntry
(
    IN  CPSS_MAC_ENTRY_EXT_STC       *dxChMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{

    return prvTgfConvertCpssToGenericMacEntry(dxChMacEntryPtr,macEntryPtr);
}

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToExMxPmMacEntry
*
* DESCRIPTION:
*       Convert generic MAC entry into device specific MAC entry
*
* INPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* OUTPUTS:
*       exMxPmMacEntryPtr - (pointer to) ExMxPm MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToExMxPmMacEntry
(
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr,
    OUT CPSS_EXMXPM_FDB_ENTRY_STC    *exMxPmMacEntryPtr
)
{
    cpssOsMemSet(exMxPmMacEntryPtr,0,sizeof(CPSS_EXMXPM_FDB_ENTRY_STC));

    /* convert entry type into device specific format */
    switch (macEntryPtr->key.entryType)
    {
        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E:
            exMxPmMacEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            exMxPmMacEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            exMxPmMacEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_VPLS_E:
            exMxPmMacEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data into device specific format */
    if ((PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E == macEntryPtr->key.entryType) ||
        (PRV_TGF_FDB_ENTRY_TYPE_VPLS_E == macEntryPtr->key.entryType))
    {
        cpssOsMemCpy(exMxPmMacEntryPtr->key.key.macFid.macAddr.arEther,
                     macEntryPtr->key.key.macVlan.macAddr.arEther,
                     sizeof(exMxPmMacEntryPtr->key.key.macFid.macAddr));
        exMxPmMacEntryPtr->key.key.macFid.fId = macEntryPtr->key.key.macVlan.vlanId;
    }
    else
    {
        cpssOsMemCpy(exMxPmMacEntryPtr->key.key.ipMcast.sip,
                     macEntryPtr->key.key.ipMcast.sip,
                     sizeof(exMxPmMacEntryPtr->key.key.ipMcast.sip));
        cpssOsMemCpy(exMxPmMacEntryPtr->key.key.ipMcast.dip,
                     macEntryPtr->key.key.ipMcast.dip,
                     sizeof(exMxPmMacEntryPtr->key.key.ipMcast.dip));
        exMxPmMacEntryPtr->key.key.ipMcast.fId = macEntryPtr->key.key.ipMcast.vlanId;
    }

    /* convert interface info into device specific format */
    exMxPmMacEntryPtr->dstOutlif.interfaceInfo.type = macEntryPtr->dstInterface.type;
    switch (exMxPmMacEntryPtr->dstOutlif.interfaceInfo.type)
    {
        case CPSS_INTERFACE_PORT_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devPort.hwDevNum  = macEntryPtr->dstInterface.devPort.hwDevNum;
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devPort.portNum = macEntryPtr->dstInterface.devPort.portNum;
            break;

        case CPSS_INTERFACE_TRUNK_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.trunkId = macEntryPtr->dstInterface.trunkId;
            break;

        case CPSS_INTERFACE_VIDX_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.vidx = macEntryPtr->dstInterface.vidx;
            break;

        case CPSS_INTERFACE_VID_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.vlanId = macEntryPtr->dstInterface.vlanId;
            break;

        case CPSS_INTERFACE_DEVICE_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.hwDevNum = macEntryPtr->dstInterface.hwDevNum;
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.fabricVidx = macEntryPtr->dstInterface.fabricVidx;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert interface info into device specific format */
    exMxPmMacEntryPtr->dstOutlif.outlifType = macEntryPtr->dstOutlif.outlifType;
    switch (exMxPmMacEntryPtr->dstOutlif.outlifType)
    {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            exMxPmMacEntryPtr->dstOutlif.outlifPointer.arpPtr = macEntryPtr->dstOutlif.outlifPointer.arpPtr;
            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            exMxPmMacEntryPtr->dstOutlif.outlifPointer.ditPtr = macEntryPtr->dstOutlif.outlifPointer.ditPtr;
            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            /* convert passenger type into device specific format */
            switch (macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType)
            {
                case PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E:
                    exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType =
                        CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                    break;

                case PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E:
                    exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType =
                        CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_OTHER_E;
                    break;

                default:
                    return GT_BAD_PARAM;
            }

            exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.ptr =
                macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.ptr;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert daCommand info into device specific format */
    switch (macEntryPtr->daCommand)
    {
        case PRV_TGF_PACKET_CMD_FORWARD_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_FORWARD_E;
            break;

        case PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E:
        case PRV_TGF_PACKET_CMD_CNTL_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_HARD_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_DROP_HARD_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_SOFT_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;

        case PRV_TGF_PACKET_CMD_ROUTE_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_ROUTE_E;
            break;

        case PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;

        case PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;

        case PRV_TGF_PACKET_CMD_BRIDGE_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_BRIDGE_E;
            break;

        case PRV_TGF_PACKET_CMD_NONE_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_NONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert saCommand info into device specific format */
    switch (macEntryPtr->saCommand)
    {
        case PRV_TGF_PACKET_CMD_FORWARD_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_FORWARD_E;
            break;

        case PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E:
        case PRV_TGF_PACKET_CMD_CNTL_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_HARD_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_DROP_HARD_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_SOFT_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;

        case PRV_TGF_PACKET_CMD_ROUTE_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_ROUTE_E;
            break;

        case PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;

        case PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;

        case PRV_TGF_PACKET_CMD_BRIDGE_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_BRIDGE_E;
            break;

        case PRV_TGF_PACKET_CMD_NONE_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_NONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic vlanInfo into device specific vlanInfo */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, isStatic);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daRoute);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, mirrorToRxAnalyzerPortEn);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, sourceId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, userDefined);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daQosIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, saQosIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daSecurityLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, saSecurityLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, appSpecificCpuCode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, pwId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, spUnknown);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       exMxPmMacEntryPtr - (pointer to) ExMxPm MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericMacEntry
(
    IN  CPSS_EXMXPM_FDB_ENTRY_STC    *exMxPmMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    /* convert entry type from device specific format */
    switch (exMxPmMacEntryPtr->key.entryType)
    {
        case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
            break;

        case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E;
            break;

        case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E;
            break;

        case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_VPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data from device specific format */
    if ((CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E == exMxPmMacEntryPtr->key.entryType) ||
        (CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E == exMxPmMacEntryPtr->key.entryType))
    {
        cpssOsMemCpy(macEntryPtr->key.key.macVlan.macAddr.arEther,
                     exMxPmMacEntryPtr->key.key.macFid.macAddr.arEther,
                     sizeof(exMxPmMacEntryPtr->key.key.macFid.macAddr));
        macEntryPtr->key.key.macVlan.vlanId = (GT_U16)exMxPmMacEntryPtr->key.key.macFid.fId;
    }
    else
    {
        cpssOsMemCpy(macEntryPtr->key.key.ipMcast.sip,
                     exMxPmMacEntryPtr->key.key.ipMcast.sip,
                     sizeof(exMxPmMacEntryPtr->key.key.ipMcast.sip));
        cpssOsMemCpy(macEntryPtr->key.key.ipMcast.dip,
                     exMxPmMacEntryPtr->key.key.ipMcast.dip,
                     sizeof(exMxPmMacEntryPtr->key.key.ipMcast.dip));
        macEntryPtr->key.key.ipMcast.vlanId = (GT_U16)exMxPmMacEntryPtr->key.key.ipMcast.fId;
    }

    /* convert interface info from device specific format */
    macEntryPtr->dstInterface.type = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.type;
    switch (exMxPmMacEntryPtr->dstOutlif.interfaceInfo.type)
    {
        case CPSS_INTERFACE_PORT_E:
            macEntryPtr->dstInterface.devPort.hwDevNum  = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devPort.hwDevNum;
            macEntryPtr->dstInterface.devPort.portNum = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devPort.portNum;
            break;

        case CPSS_INTERFACE_TRUNK_E:
            macEntryPtr->dstInterface.trunkId = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.trunkId;
            break;

        case CPSS_INTERFACE_VIDX_E:
            macEntryPtr->dstInterface.vidx = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.vidx;
            break;

        case CPSS_INTERFACE_VID_E:
            macEntryPtr->dstInterface.vlanId = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.vlanId;
            break;

        case CPSS_INTERFACE_DEVICE_E:
            macEntryPtr->dstInterface.hwDevNum = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.hwDevNum;
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            macEntryPtr->dstInterface.fabricVidx = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.fabricVidx;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert interface info from device specific format */
    macEntryPtr->dstOutlif.outlifType = exMxPmMacEntryPtr->dstOutlif.outlifType;
    switch (exMxPmMacEntryPtr->dstOutlif.outlifType)
    {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            macEntryPtr->dstOutlif.outlifPointer.arpPtr = exMxPmMacEntryPtr->dstOutlif.outlifPointer.arpPtr;
            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            macEntryPtr->dstOutlif.outlifPointer.ditPtr = exMxPmMacEntryPtr->dstOutlif.outlifPointer.ditPtr;
            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            /* convert passenger type from device specific format */
            switch (exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType)
            {
                case CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E:
                    macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType =
                        PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                    break;

                case CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_OTHER_E:
                    macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType =
                        PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E;
                    break;

                default:
                    return GT_BAD_PARAM;
            }

            macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.ptr =
                exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.ptr;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* convert daCommand info from device specific format */
    switch (exMxPmMacEntryPtr->daCommand)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
            break;

        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_DROP_HARD_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_DROP_HARD_E;
            break;

        case CPSS_PACKET_CMD_DROP_SOFT_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_DROP_SOFT_E;
            break;

        case CPSS_PACKET_CMD_ROUTE_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_ROUTE_E;
            break;

        case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_BRIDGE_E;
            break;

        case CPSS_PACKET_CMD_NONE_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_NONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert saCommand info from device specific format */
    switch (exMxPmMacEntryPtr->saCommand)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
            break;

        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_DROP_HARD_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_DROP_HARD_E;
            break;

        case CPSS_PACKET_CMD_DROP_SOFT_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_DROP_SOFT_E;
            break;

        case CPSS_PACKET_CMD_ROUTE_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_ROUTE_E;
            break;

        case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_BRIDGE_E;
            break;

        case CPSS_PACKET_CMD_NONE_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_NONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic vlanInfo from device specific vlanInfo */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, isStatic);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daRoute);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, mirrorToRxAnalyzerPortEn);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, sourceId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, userDefined);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daQosIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, saQosIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daSecurityLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, saSecurityLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, appSpecificCpuCode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, pwId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, spUnknown);

    return GT_OK;
}

#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfBrgFdbMacEntryReadPerDev
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       hwDevNumPtr   - (pointer to) is HW device number associated with the entry
*       macEntryPtr - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryReadPerDev
(
    IN  GT_U8                         devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN  GT_U32                        index,
    OUT GT_BOOL                      *validPtr,
    OUT GT_BOOL                      *skipPtr,
    OUT GT_BOOL                      *agedPtr,
    OUT GT_HW_DEV_NUM                *hwDevNumPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_STC      dxChMacEntry;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ENTRY_STC   exMxPmMacEntry;
    GT_U8                       associatedDevNum;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* call device specific API */
    AU_FDB_UPDATE_LOCK();
    rc = cpssDxChBrgFdbPortGroupMacEntryRead(devNum,  portGroupsBmp, index,
                                             validPtr,skipPtr,agedPtr,
                                             hwDevNumPtr,&dxChMacEntry);
    AU_FDB_UPDATE_UNLOCK();
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacEntryRead FAILED, rc = [%d]", rc);

        return rc;
    }

    if (dxChMacEntry.key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
    {
        /* hwDevNumPtr relevant only in case if entry type is CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E */

        /* convert hwDevNumPtr to SW devNum.*/
        rc = prvUtfSwFromHwDeviceNumberGet(*hwDevNumPtr, hwDevNumPtr);
        if(GT_OK != rc)
        {
            PRV_TGF_LOG1_MAC("[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);
            return rc;
        }
    }


    /* convert key data from device specific format */
    return prvTgfConvertDxChToGenericMacEntry(&dxChMacEntry, macEntryPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    rc = cpssExMxPmBrgFdbEntryRead(devNum, index,validPtr, skipPtr,
                                   agedPtr, &associatedDevNum, &exMxPmMacEntry);
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbEntryRead FAILED, rc = [%d]", rc);

        return rc;
    }

    *hwDevNumPtr = associatedDevNum;

    /* convert key data from device specific format */
    return prvTgfConvertExMxPmToGenericMacEntry(&exMxPmMacEntry, macEntryPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryInvalidatePerDev
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index
*       of specified device.
*       the invalidation done by resetting to first word of the entry
*
* INPUTS:
*       devNum - a device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index  - hw mac entry index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryInvalidatePerDev
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               index
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    /* call device specific API */
    AU_FDB_UPDATE_LOCK();
    rc = cpssDxChBrgFdbPortGroupMacEntryInvalidate(devNum, portGroupsBmp, index);
    AU_FDB_UPDATE_UNLOCK();
    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    UNUSED(portGroupsBmp);
    return cpssExMxPmBrgFdbEntryInvalidate(devNum, index);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************

* prvTgfBrgFdbHashCrcMultiResultsCalc
*
* DESCRIPTION:
*       This function calculates the CRC multiple hash results
*       (indexes into the FDB table).
*       NOTE: the function do not access the HW , and do only SW calculations.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - (pointer to) entry key
*       multiHashStartBankIndex  - the first bank for 'multi hash' result.
*                   (APPLICABLE RANGES: 0..15)
*       numOfBanks    - number of banks for 'multi hash' result.
*                   this value indicates the number of elements that will be
*                   retrieved by crcMultiHashArr[]
*                   restriction of (numOfBanks + multiHashStartBankIndex) <= 16 .
*                   (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*      crcMultiHashArr[] - (array of) 'multi hash' CRC results. index in this array is 'bank Id'
*                           (index 0 will hold value relate to bank multiHashStartBankIndex).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or parameters in entryKeyPtr.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbHashCrcMultiResultsCalc
(
    IN  GT_U8                       devNum,
    IN  PRV_TGF_MAC_ENTRY_KEY_STC   *macEntryKeyPtr,
    IN  GT_U32                      multiHashStartBankIndex,
    IN  GT_U32                      numOfBanks,
    OUT GT_U32                      crcMultiHashArr[]
)
{
#ifdef CHX_FAMILY
    GT_STATUS           rc    ;
    CPSS_MAC_ENTRY_EXT_KEY_STC  dxChMacEntryKey;

    numOfBanks = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.numOfBanks;

    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToDxChMacEntryKey(macEntryKeyPtr, &dxChMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntryKey FAILED, rc = [%d]", rc);

        return rc;
    }

    rc = cpssDxChBrgFdbHashCrcMultiResultsCalc(devNum,
                                               &dxChMacEntryKey,
                                               multiHashStartBankIndex,
                                               numOfBanks,
                                               crcMultiHashArr);
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC("[TGF]: cpssDxChBrgFdbHashCrcMultiResultsCalc FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;

#else /*CHX_FAMILY*/
    return GT_NOT_SUPPORTED;
#endif /*!CHX_FAMILY*/
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryDeletePerDev
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table of specified device
*
* INPUTS:
*       devNum         - a device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr - (pointer to) key parameters of the mac entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vlanId
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryDeletePerDev
(
    IN GT_U8                          devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN PRV_TGF_MAC_ENTRY_KEY_STC     *macEntryKeyPtr
)
{
    GT_STATUS   rc, rc1   = GT_OK;
#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_KEY_STC  dxChMacEntryKey;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    GT_BOOL     completed = GT_FALSE;
    GT_BOOL     succeeded = GT_FALSE;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC exMxPmMacEntryKey;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    prvTgfBrgFdbMacEntryDelete_unregister(devNum, macEntryKeyPtr);
#endif /* CHX_FAMILY */

#ifdef CHX_FAMILY

    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToDxChMacEntryKey(macEntryKeyPtr, &dxChMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntryKey FAILED, rc = [%d]", rc);

        return rc;
    }
    /* call device specific API */
    AU_FDB_UPDATE_LOCK();
    rc = cpssDxChBrgFdbPortGroupMacEntryDelete(devNum, portGroupsBmp,
                                               &dxChMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC("[TGF]: cpssDxChBrgFdbPortGroupMacEntryDelete FAILED, rc = [%d]", rc);
        rc1 = rc;
    }
    else
    {
        tgfCpssDxChBrgFdbFromCpuAuMsgStatusPerPortGroupsGet(
            devNum,portGroupsBmp, GT_FALSE); /* fdbEntryByMessageMustSucceed used instead GT_FALSE  before */

    }
    AU_FDB_UPDATE_UNLOCK();
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntryKey(macEntryKeyPtr, &exMxPmMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntryKey FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssExMxPmBrgFdbEntryDelete(devNum, &exMxPmMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbEntryDelete FAILED, rc = [%d]", rc);

        rc1 = rc;
        return rc1;

    }
    /* synchronize call */
    completed = GT_FALSE;
    succeeded = GT_FALSE;
    do {
        cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum, CPSS_QA_E, &completed, &succeeded);
        if (GT_OK != rc)
        {
            PRV_TGF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbFromCpuAuMsgStatusGet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    } while ((GT_FALSE == completed) && (GT_FALSE == succeeded));
#endif /* EXMXPM_FAMILY */

    return rc1;
}

#ifdef CHX_FAMILY

/*******************************************************************************
* prvTgfBrgFdbMacEntryDelete_unregister
*
* DESCRIPTION:
*       Remove the MAC entry from the linked list brgFdbAuqMessagesPart2
*
* INPUTS:
*       devNum         - a device number
*       macEntryKeyPtr - a MAC entry struct pointer
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryDelete_unregister
(
    IN GT_U8                          devNum,
    IN PRV_TGF_MAC_ENTRY_KEY_STC     *macEntryKeyPtr
)
{
    PRV_TGF_LIST_ITEM_PTR itemPtr; /* Is used for moving thru the
                                      brgFdbAuqMessagesPart2 list. */
    PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC *currInfoPtr;/* pointer to current info */

    if(prvTgfBrgFdbDoNaShadow_part2 == 0)
    {
        return GT_OK;
    }

    /* lock the DB */
    AU_FDB_UPDATE_LOCK();
    itemPtr = brgFdbAuqMessagesPart2.firstPtr;
    while (NULL != itemPtr)
    {
        currInfoPtr = (PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC*)itemPtr;
        itemPtr = itemPtr->nextPtr;
        if (GT_TRUE != currInfoPtr->deleted)
        {
            if(currInfoPtr->devNum == devNum &&
               (0 == cpssOsMemCmp(&currInfoPtr->message.key,
                                 macEntryKeyPtr,
                                 sizeof(*macEntryKeyPtr))))
            {
                prvTgfListDelItem(&brgFdbAuqMessagesPart2, &currInfoPtr->item);
                currInfoPtr->deleted = GT_TRUE;
                break;
            }
        }
    }
    AU_FDB_UPDATE_UNLOCK();

    /* even if not found in the list ... it's ok */
    return GT_OK;
}

/*******************************************************************************
* prvTgfBrgFdbMacEntrySet_register
*
* DESCRIPTION:
*       Add the MAC entry to the linked list brgFdbAuqMessagesPart2
*
* INPUTS:
*       devNum         - a device number
*       index          - an FDB entry index
*       macEntryKeyPtr - a MAC entry struct pointer
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntrySet_register
(
    IN GT_U8                         devNum,
    IN GT_U32                        index,
    IN PRV_TGF_BRG_MAC_ENTRY_STC     *macEntryPtr
)
{
    PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC *currInfoPtr;/* pointer to current info */

    if(prvTgfBrgFdbDoNaShadow_part2 == 0)
    {
        return GT_OK;
    }

    /* first remove the entry if already exists from the shadow */
    (void)prvTgfBrgFdbMacEntryDelete_unregister(devNum,&macEntryPtr->key);

    /* lock the DB */
    AU_FDB_UPDATE_LOCK();
    currInfoPtr = (PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC *)
                    prvTgfListAppend(&brgFdbAuqMessagesPart2);
    if (NULL != currInfoPtr) /* new item was  added */
    {
        currInfoPtr->devNum  = devNum;
        currInfoPtr->index   = index;
        currInfoPtr->message = *macEntryPtr;
        currInfoPtr->deleted = GT_FALSE;
    }
    else /* not found empty place in array or a malloc() error occured */
    {
        PRV_TGF_LOG0_MAC("ERROR : prvTgfBrgFdbMacEntrySet_register : queue FULL brgFdbAuqMessagesArrPart2 or malloc() error occured.\n");
    }

    /* unlock the DB */
    AU_FDB_UPDATE_UNLOCK();
    return (NULL == currInfoPtr) ? GT_FULL : GT_OK;
}

/*******************************************************************************
* prvTgfBrgFdbMacEntrySet_register
*
* DESCRIPTION:
*       Add/Remove the MAC entry to/from the linked list brgFdbAuqMessagesPart2
*       depending on the 'skip' parameter.
*
* INPUTS:
*       devNum         - a device number
*       index          - an FDB entry index
*       skip           - whether to delete (GT_TRUE) or add (GT_FASE) a mac
*                        entry to the linked list
*       macEntryKeyPtr - a MAC entry struct pointer
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryWrite_register
(
    IN GT_U8                         devNum,
    IN GT_U32                        index,
    IN GT_BOOL                       skip,
    IN PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    if(skip)
    {
        return prvTgfBrgFdbMacEntryDelete_unregister(devNum, &macEntryPtr->key);
    }

    return prvTgfBrgFdbMacEntrySet_register(devNum, index,macEntryPtr);
}

/*******************************************************************************
* prvTgfBrgFdbFlushByShadowPart1
*
* DESCRIPTION:
*       Go through the linked list brgFdbAuqMessagesPart1 look for NA
*       messages and delete appropriate FDB entries
*
* INPUTS:
*       devNum     - a device number
*       fdbDelMode - FDB deletion mode
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_OUT_OF_RANGE - index out of range

*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbFlushByShadowPart1
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT fdbDelMode
)
{
    GT_STATUS   rc;
    GT_BOOL     delStatic;
    GT_BOOL     delDynamic;
    GT_BOOL     isStatic;
    PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC *currInfoPtr;/* pointer to current info */
    PRV_TGF_LIST_ITEM_PTR itemPtr; /* Is used for moving thru the
                                      brgFdbAuqMessagesPart1 list. */
    PRV_TGF_BRG_MAC_ENTRY_STC macEntry;
    PRV_TGF_FDB_ENTRY_TYPE_ENT entryType;

    delStatic =
        fdbDelMode == CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E
        || fdbDelMode == CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E;

    delDynamic =
        fdbDelMode == CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E
        || fdbDelMode == CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E;

    /* lock the DB */
    AU_FDB_UPDATE_LOCK();

    itemPtr = brgFdbAuqMessagesPart1.firstPtr;
    while (NULL != itemPtr)
    {
        currInfoPtr = (PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC*)itemPtr;
        itemPtr = itemPtr->nextPtr;

        /* check the device number */
        if (currInfoPtr->devNum != devNum)
        {
            continue;
        }
        /* check that this is NA message */
        if(currInfoPtr->message.updType != CPSS_NA_E)
        {
            continue;
        }

        isStatic = currInfoPtr->message.macEntry.isStatic;
        if ((isStatic == GT_TRUE  && delStatic  == GT_FALSE) ||
            (isStatic == GT_FALSE && delDynamic == GT_FALSE))
        {
            /* not really gone get here because this DB is from control learning from AppDemo ... */
            continue;
        }

        /* convert cpss to TGF format */
        rc = prvTgfConvertCpssToGenericMacEntry(&currInfoPtr->message.macEntry,&macEntry);
        /* delete item from list */
        prvTgfListDelItem(&brgFdbAuqMessagesPart1, &currInfoPtr->item);
        if(rc != GT_OK)
        {
            continue;
        }

        /* unlock the DB */
        AU_FDB_UPDATE_UNLOCK();

        /* delete the mac */
        entryType = macEntry.key.entryType;
        if (((entryType==PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E)||
            (entryType==PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E)||
            (entryType==PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E))
#if defined DXCH_CODE
            && (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_FDB_ROUTE_UC_DELETE_BY_MESSAGE_WA_E))
#endif
            )
        {
            prvTgfBrgFdbMacEntryInvalidatePerDev(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                 currInfoPtr->message.macEntryIndex);
        }
        else
        {
            prvTgfBrgFdbMacEntryDeletePerDev(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                             &macEntry.key);
        }

        /* lock the DB */
        AU_FDB_UPDATE_LOCK();
    }

    /* unlock the DB */
    AU_FDB_UPDATE_UNLOCK();

    return GT_OK;

}

/*******************************************************************************
* prvTgfBrgFdbFlushByShadowPart2
*
* DESCRIPTION:
*       Go through the linked list brgFdbAuqMessagesPart2
*       and delete FDB entries that  apporopriate to the list's items
*
* INPUTS:
*       devNum - a device number
*       fdbDelMode - FDB deletion mode
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_OUT_OF_RANGE - index out of range

*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbFlushByShadowPart2
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT fdbDelMode
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    i=0;
    GT_BOOL     delStatic;
    GT_BOOL     delDynamic;
    GT_BOOL     isStatic;

    PRV_TGF_LIST_ITEM_PTR itemPtr; /* Is used for moving thru the
                                      brgFdbAuqMessagesPart2 list. */
    PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC *currInfoPtr;/* pointer to current info */
    PRV_TGF_MAC_ENTRY_KEY_STC       tmpKey;
    PRV_TGF_FDB_ENTRY_TYPE_ENT      entryType;
    GT_U32                          crcMultiHashArr[16];

    GT_BOOL                         valid;
    GT_BOOL                         skip;
    OUT GT_BOOL                     aged;
    OUT GT_HW_DEV_NUM               hwDevNum;
    PRV_TGF_BRG_MAC_ENTRY_STC       macEntry;
    GT_U32                          secondIndex;
    CPSS_MAC_ENTRY_EXT_TYPE_ENT     cpssEntryType;

    if(prvTgfBrgFdbDoNaShadow_part2 == 0)
    {
        return GT_OK;
    }

    /* lock the DB */
    AU_FDB_UPDATE_LOCK();

    delStatic =
        fdbDelMode == CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E
        || fdbDelMode == CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E;

    delDynamic =
        fdbDelMode == CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E
        || fdbDelMode == CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E;

    itemPtr = brgFdbAuqMessagesPart2.firstPtr;
    while (NULL != itemPtr)
    {
        currInfoPtr = (PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC*)itemPtr;
        itemPtr = itemPtr->nextPtr;

        isStatic = currInfoPtr->message.isStatic;
        if(currInfoPtr->deleted == GT_TRUE)
        {
            /* the other info not valid any more */
        }
        else
        if((isStatic == GT_TRUE  && delStatic  == GT_TRUE) ||
           (isStatic == GT_FALSE && delDynamic == GT_TRUE))
        {
            cpssOsMemCpy(&tmpKey,&currInfoPtr->message.key,sizeof(tmpKey));
            currInfoPtr->deleted = GT_TRUE; /*mark as deleted*/
            /* unlock the DB */
            AU_FDB_UPDATE_UNLOCK();
            /* delete the mac */
            entryType = tmpKey.entryType;
            if (((entryType==PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E)||
                (entryType==PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E)||
                (entryType==PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E))
#if defined DXCH_CODE
                && (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_FDB_ROUTE_UC_DELETE_BY_MESSAGE_WA_E))
#endif
                )
            {
                /* if the type is IPv4 UC, need to find all 16 indexes
                   possible for this message and go over all of them to find the appropriate entry.
                   if the entry is valid need to invalidate it by index */
                 if (entryType==PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E)
                 {
                     rc = prvTgfBrgFdbHashCrcMultiResultsCalc(devNum, &tmpKey,0, 16, crcMultiHashArr);
                     if(rc != GT_OK)
                     {
                         return rc;
                     }
                     for(i=0;i<16;i++)
                     {
                         rc = prvTgfBrgFdbMacEntryReadPerDev(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                             crcMultiHashArr[i],
                                                             &valid, &skip, &aged,
                                                             &hwDevNum, &macEntry);
                         if(rc != GT_OK)
                         {
                             return rc;
                         }
                         if(valid==GT_TRUE)
                         {
                             /* if the entry is valid need to check if this is the IP UC entry we want to delete,
                                if the entry is invalid then nothing to do */
                             if(0 == cpssOsMemCmp(&tmpKey,&macEntry.key, sizeof(PRV_TGF_MAC_ENTRY_KEY_STC)))
                             {

                                 rc = prvTgfBrgFdbMacEntryInvalidatePerDev(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                                           crcMultiHashArr[i]);
                                 if(rc != GT_OK)
                                 {
                                     return rc;
                                 }
                                 break;
                             }
                         }
                     }
                 }
                 else
                 {
                    /* if this is an IPv6 UC entry need to invalidate it by index */
                     rc = prvTgfBrgFdbMacEntryInvalidatePerDev(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                               currInfoPtr->index);
                     if(rc != GT_OK)
                     {
                         return rc;
                     }

                     /* need to delete the element form the Linked List holding address to data mapping */
                     switch(entryType)
                     {
                         case PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E:
                             cpssEntryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E;
                             break;
                         case PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E:
                             cpssEntryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E;
                             break;
                         case PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E:
                             cpssEntryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E;
                             break;
                         default:
                             return GT_BAD_PARAM;
                     }
                     rc = prvAppDemoBrgFdbIpv6UcFindPairIndexToDelete(devNum, cpssEntryType,
                                                                      currInfoPtr->index,
                                                                      GT_TRUE,
                                                                      &secondIndex);
                     if((rc != GT_OK)&&(rc!=GT_NOT_FOUND))
                     {
                         return rc;
                     }
                 }
            }
            else
            {
                rc = prvTgfBrgFdbMacEntryDeletePerDev(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                      &tmpKey);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            /* lock the DB */
            AU_FDB_UPDATE_LOCK();

            itemPtr = currInfoPtr->item.nextPtr;
            prvTgfListDelItem(&brgFdbAuqMessagesPart2,
                              &currInfoPtr->item); /*delete physically*/
        }
    }

    /* unlock the DB */
    AU_FDB_UPDATE_UNLOCK();

    return GT_OK;
}

/*******************************************************************************
* tgfCpssDxChBrgFdbFromCpuAuMsgStatusPerPortGroupsGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*
* INPUTS:
*       devNum - a device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       mustSucceed   - expected status of message processing
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    -  on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT               - on timeout expired
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfCpssDxChBrgFdbFromCpuAuMsgStatusPerPortGroupsGet
(
    IN GT_U8              devNum,
    IN GT_PORT_GROUPS_BMP portGroupsBmp,
    IN GT_BOOL            mustSucceed
)
{
    GT_STATUS   st;
    GT_PORT_GROUPS_BMP completedBmp;
    GT_PORT_GROUPS_BMP succeededBmp;
    GT_PORT_GROUPS_BMP completedBmpSummary = 0;
    GT_PORT_GROUPS_BMP succeededBmpSummary = 0;
#ifdef ASIC_SIMULATION
    GT_U32  sleepTime = 0;
#ifdef GM_USED
    /*the GM not switch tasks for 'active' memories. all done in the same contexts */
    /*so if operation not ended ... it will never end ! */
    GT_U32  maxRetry = 0;
    GT_U32  totalMaxRetry = 2;
#else
    GT_U32  maxRetry = 128;
    GT_U32  totalMaxRetry = _10K;
#endif  /* GM_USED */

#endif/*ASIC_SIMULATION*/

    do{
        completedBmp = 0;
        succeededBmp = 0;
#ifdef ASIC_SIMULATION
        if ((sleepTime == 0) && maxRetry)
        {
            /* do max retries with 'busy wait' but with option to other tasks
               to also run --> reduce run time */
            maxRetry--;
        }
        else if (maxRetry == 0)
        {
            sleepTime = 1;
        }

        totalMaxRetry--;
        if(totalMaxRetry == 0)
        {
            PRV_TGF_LOG1_MAC("[TGF]: tgfCpssDxChBrgFdbFromCpuAuMsgStatusPerPortGroupsGet Timed out after [%d] seconds \n", 10);
            /* indicate ERROR */
            PRV_TGF_LOG1_MAC("%d", devNum);
            return GT_TIMEOUT;

        }
        else if((totalMaxRetry % 1000) == 0)
        {
            /* show the wait process , every 1 seconds */
            PRV_TGF_LOG0_MAC("@");
        }

        cpssOsTimerWkAfter(sleepTime);
#endif /*ASIC_SIMULATION*/
        st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum,
                                                          portGroupsBmp,
                                                          &completedBmp,
                                                          &succeededBmp);
        if(st != GT_OK)
        {
            PRV_TGF_LOG1_MAC("[TGF]: tgfCpssDxChBrgFdbFromCpuAuMsgStatusPerPortGroupsGet FAILED, rc = [%d] \n", st);
            PRV_TGF_LOG1_MAC("%d", devNum);
            return st;
        }

        completedBmpSummary |= completedBmp;
        succeededBmpSummary |= succeededBmp;
    }while (
        (completedBmpSummary & PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp) !=
        (portGroupsBmp & PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp));


    if(completedBmpSummary != succeededBmpSummary  && mustSucceed == GT_TRUE)
    {
        /* operation finished but not succeeded on all port groups */
        st = GT_FAIL;
        PRV_TGF_LOG1_MAC("[TGF]: tgfCpssDxChBrgFdbFromCpuAuMsgStatusPerPortGroupsGet FAILED, rc = [%d] \n", st);
        PRV_TGF_LOG1_MAC("%d", devNum);
        return st;
    }
    return GT_OK;
}

#endif/*CHX_FAMILY*/

/*******************************************************************************
* prvTgfConvertCpssToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       cpssMacEntryPtr - (pointer to) cpss MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertCpssToGenericMacEntry
(
    IN  CPSS_MAC_ENTRY_EXT_STC       *cpssMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    GT_STATUS   rc;

    cpssOsMemSet(macEntryPtr,0,sizeof(PRV_TGF_BRG_MAC_ENTRY_STC));

    /* convert entry type from device specific format */
    switch (cpssMacEntryPtr->key.entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_FID_VID1_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_FID_VID1_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_FID_VID1_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data from device specific format */
    if (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E == cpssMacEntryPtr->key.entryType)
    {
        cpssOsMemCpy(macEntryPtr->key.key.macVlan.macAddr.arEther,
                     cpssMacEntryPtr->key.key.macVlan.macAddr.arEther,
                     sizeof(cpssMacEntryPtr->key.key.macVlan.macAddr));
        macEntryPtr->key.key.macVlan.vlanId = cpssMacEntryPtr->key.key.macVlan.vlanId;
    }
    else
        if (CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E == cpssMacEntryPtr->key.entryType)
        {
            cpssOsMemCpy(&macEntryPtr->key.key.ipv4Unicast,
                         &cpssMacEntryPtr->key.key.ipv4Unicast,
                         sizeof(cpssMacEntryPtr->key.key.ipv4Unicast));
        }
        else
            if ( (CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E == cpssMacEntryPtr->key.entryType) ||
                 (CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E == cpssMacEntryPtr->key.entryType) )
            {
                cpssOsMemCpy(&macEntryPtr->key.key.ipv6Unicast,
                             &cpssMacEntryPtr->key.key.ipv6Unicast,
                             sizeof(cpssMacEntryPtr->key.key.ipv6Unicast));
            }
            else
            {
                cpssOsMemCpy(macEntryPtr->key.key.ipMcast.sip,
                             cpssMacEntryPtr->key.key.ipMcast.sip,
                             sizeof(cpssMacEntryPtr->key.key.ipMcast.sip));
                cpssOsMemCpy(macEntryPtr->key.key.ipMcast.dip,
                             cpssMacEntryPtr->key.key.ipMcast.dip,
                             sizeof(cpssMacEntryPtr->key.key.ipMcast.dip));
                macEntryPtr->key.key.ipMcast.vlanId = cpssMacEntryPtr->key.key.ipMcast.vlanId;
            }

    /* convert interface info from device specific format */
    macEntryPtr->dstInterface.type = cpssMacEntryPtr->dstInterface.type;
    switch (cpssMacEntryPtr->dstInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            rc = prvUtfSwFromHwDeviceNumberGet(cpssMacEntryPtr->dstInterface.devPort.hwDevNum,
                                               &(macEntryPtr->dstInterface.devPort.hwDevNum));
            if(GT_OK != rc)
                return rc;
            macEntryPtr->dstInterface.devPort.portNum = cpssMacEntryPtr->dstInterface.devPort.portNum;
            break;

        case CPSS_INTERFACE_TRUNK_E:
            macEntryPtr->dstInterface.trunkId = cpssMacEntryPtr->dstInterface.trunkId;
            break;

        case CPSS_INTERFACE_VIDX_E:
            macEntryPtr->dstInterface.vidx = cpssMacEntryPtr->dstInterface.vidx;
            break;

        case CPSS_INTERFACE_VID_E:
            macEntryPtr->dstInterface.vlanId = cpssMacEntryPtr->dstInterface.vlanId;
            break;

        case CPSS_INTERFACE_DEVICE_E:
            macEntryPtr->dstInterface.hwDevNum = cpssMacEntryPtr->dstInterface.hwDevNum;
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            macEntryPtr->dstInterface.fabricVidx = cpssMacEntryPtr->dstInterface.fabricVidx;
            break;

        default:
            return GT_BAD_PARAM;
    }


    if ( (CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E == cpssMacEntryPtr->key.entryType) ||
         (CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E == cpssMacEntryPtr->key.entryType) ||
         (CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E == cpssMacEntryPtr->key.entryType) )
    {
        cpssOsMemCpy(&macEntryPtr->fdbRoutingInfo,
                     &cpssMacEntryPtr->fdbRoutingInfo,
                     sizeof(cpssMacEntryPtr->fdbRoutingInfo));
        return GT_OK;
    }

    macEntryPtr->sourceId = cpssMacEntryPtr->sourceID;

    /* convert daCommand info from device specific format */
    switch (cpssMacEntryPtr->daCommand)
    {
        case CPSS_MAC_TABLE_FRWRD_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
            break;

        case CPSS_MAC_TABLE_DROP_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_DROP_HARD_E;
            break;

        case CPSS_MAC_TABLE_INTERV_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_INTERV_E;
            break;

        case CPSS_MAC_TABLE_CNTL_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_CNTL_E;
            break;

        case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case CPSS_MAC_TABLE_SOFT_DROP_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_DROP_SOFT_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert saCommand info from device specific format */
    switch (cpssMacEntryPtr->saCommand)
    {
        case CPSS_MAC_TABLE_FRWRD_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
            break;

        case CPSS_MAC_TABLE_DROP_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_DROP_HARD_E;
            break;

        case CPSS_MAC_TABLE_INTERV_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_INTERV_E;
            break;

        case CPSS_MAC_TABLE_CNTL_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_CNTL_E;
            break;

        case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case CPSS_MAC_TABLE_SOFT_DROP_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_DROP_SOFT_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert MAC entry info from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, isStatic);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, daRoute);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, mirrorToRxAnalyzerPortEn);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, userDefined);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, daQosIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, saQosIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, daSecurityLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, saSecurityLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, appSpecificCpuCode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, spUnknown);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, key.vid1);

    return GT_OK;
}

/*******************************************************************************
* brgFdbAuqMessageHandler
*
* DESCRIPTION:
*       the function handle AUQ messages , currently only QR needed.
*       NOTE: the function that is called from the appDemo when appdemo receive
*       FDB AUQ message
*
* INPUTS:
*      cookie       - a cookie passed to auEventHandlerAddCallback()
*      evType       - event type
*      devNum       - Device number.
*      auMessagePtr - pointer to the AUQ message
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FULL         - array full
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS brgFdbAuqMessageHandler
(
  IN  GT_UINTPTR                   cookie,
  IN  AU_EV_HANDLER_EV_TYPE_ENT    evType,
  IN  GT_U8                        devNum,
  IN  CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagePtr
)
{
    GT_STATUS ret;
    PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC *currInfoPtr;/* pointer to current info */

    UNUSED(cookie);
    UNUSED(evType);

    /* lock the DB */
    AU_FDB_UPDATE_LOCK();

    /* add new item to the end of the list brgFdbAuqMessagesPart1 */
    currInfoPtr = (PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC *)
                    prvTgfListAppend(&brgFdbAuqMessagesPart1);

    if (NULL != currInfoPtr) /* new item was  added */
    {
        currInfoPtr->devNum = devNum;
        currInfoPtr->message = *auMessagePtr;
        ret = GT_OK;
    }
    else
    {
        PRV_TGF_LOG0_MAC("ERROR : brgFdbAuqMessageHandler : queue FULL brgFdbAuqMessagesPart1 or malloc error occured.\n");
        ret=GT_FULL;
    };

    /* unlock the DB */
    AU_FDB_UPDATE_UNLOCK();
    return ret;
};

/*******************************************************************************
* prvTgfBrgFdbAuqCbRegister
*
* DESCRIPTION:
*       register a CB (callback) function for a specific AUQ message type.
*       the registered function (cbFuncPtr) will be called for every AUQ message
*       that match the messageType.
*
* INPUTS:
*       messageType - AUQ message type
*       bindOrUnbind    - bind/unbind   :
                        GT_TRUE - bind
                        GT_FALSE - unbind
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS   prvTgfBrgFdbAuqCbRegister
(
    IN CPSS_UPD_MSG_TYPE_ENT       messageType,
    IN GT_BOOL                     bindOrUnbind
)
{
#ifdef CHX_FAMILY
    if(messageType == CPSS_NA_E)
    {
        prvTgfBrgFdbDoNaShadow_part2 = bindOrUnbind == GT_TRUE ? 1 : 0;
    }
#endif /*CHX_FAMILY*/

    if (bindOrUnbind == GT_TRUE)
    {
        /* register callback receive AUQ messages */
        return auEventHandlerAddCallback(AU_EV_HANDLER_DEVNUM_ALL,
                (AU_EV_HANDLER_EV_TYPE_ENT)messageType,
                brgFdbAuqMessageHandler, 0);
    }

    /* unregister callback */
    return auEventHandlerRemoveCallback(AU_EV_HANDLER_DEVNUM_ALL,
            (AU_EV_HANDLER_EV_TYPE_ENT)messageType,
            brgFdbAuqMessageHandler, 0);
}

/*******************************************************************************
* prvTgfBrgFdbInit
*
* DESCRIPTION:
*       Init the TGF FDB lib. needed for GM FDB shadow.
*       (but with flag allowAnyDevice = true , we can use it for any device)
*
* INPUTS:
*       devNum - the device number
*       allowAnyDevice - allow any device to use the shadow.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS   prvTgfBrgFdbInit(IN GT_U8 devNum , IN GT_BOOL allowAnyDevice)
{
    static GT_U32   registerFdbDone = 0;
    GT_STATUS   st;

    if(registerFdbDone == 0 &&
       (prvUtfIsGmCompilation() == GT_TRUE || allowAnyDevice == GT_TRUE) &&
       (PRV_CPSS_SIP_5_CHECK_MAC(devNum) || allowAnyDevice == GT_TRUE))
    {
        /* bind the enh-ut engine to keep track over the FDB entries that we
           add/delete to/from the CPSS by the control learning
           we need it due to poor performance of the GM of flush FDB */
        st = prvTgfBrgFdbAuqCbRegister(CPSS_NA_E,GT_TRUE);
        if(st != GT_OK)
        {
            PRV_TGF_LOG1_MAC("[UTF]: prvTgfBrgFdbAuqCbRegister: error[0x%x]\n",st);
        }

        prvTgfFdbShadowUsed = GT_TRUE;
        registerFdbDone = 1;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart1
*
* DESCRIPTION:
*       get next entry from the FDB shadow 'part 1'.
*
* INPUTS:
*       devNum     - a device number
*       fdbDelMode - FDB deletion mode
*       cookiePtrPtr - (pointer to) cookiePtr. the 'cookie' as IN
*                   (identify the previous entry returned)
*                   when (*cookiePtrPtr)==NULL means to start from first element
* OUTPUTS:
*       cookiePtrPtr - (pointer to) cookiePtr. the 'cookie' as OUT
*                   (identify the current entry returned)
*       OUT_macEntryPtr - (pointer to) the fdb entry found in the shadow.
*
* RETURNS:
*       GT_OK      - on success,
*       GT_NO_MORE - no more entries in this FDB shadow.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart1
(
    IN GT_U8                         devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT mode,
    INOUT GT_U32                   *cookiePtr,
    OUT CPSS_MAC_ENTRY_EXT_STC    **OUT_macEntryPtr
)
{
    GT_BOOL     foundNext = GT_FALSE;
    PRV_TGF_LIST_ITEM_PTR itemPtr; /* Is used for moving thru the
                                      brgFdbAuqMessagesPart2 list. */
    PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC *currInfoPtr;/* pointer to current info */
    GT_U32  currIndex = 0;
    GT_U32  startIndex = *cookiePtr;

    *cookiePtr = 0;
    *OUT_macEntryPtr = NULL;

    if(mode == CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E)
    {
        /* this DB filled with controlled learning ... all are dynamic */
        return GT_NO_MORE;
    }

    /* lock the DB */
    AU_FDB_UPDATE_LOCK();
    itemPtr = brgFdbAuqMessagesPart1.firstPtr;

    while (NULL != itemPtr)
    {
        currInfoPtr = (PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC*)itemPtr;
        itemPtr = itemPtr->nextPtr;
        currIndex ++;
        if((currIndex-1) < startIndex)
        {
            continue;
        }

        *OUT_macEntryPtr = &(currInfoPtr->message.macEntry);
        foundNext = GT_TRUE;
        break;
    }
    AU_FDB_UPDATE_UNLOCK();


    *cookiePtr = currIndex;

    if (foundNext == GT_TRUE)
    {
        return GT_OK;
    }

    return GT_NO_MORE;

}

/*******************************************************************************
* prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart2
*
* DESCRIPTION:
*       get next entry from the FDB shadow 'part 2'.
*
* INPUTS:
*       devNum     - a device number
*       fdbDelMode - FDB deletion mode
*       cookiePtrPtr - (pointer to) cookiePtr. the 'cookie' as IN
*                   (identify the previous entry returned)
*                   when (*cookiePtrPtr)==NULL means to start from first element
* OUTPUTS:
*       cookiePtrPtr - (pointer to) cookiePtr. the 'cookie' as OUT
*                   (identify the current entry returned)
*       OUT_macEntryPtr - (pointer to) the fdb entry found in the shadow.
*                       NOTE: the format is 'TGF'
*
* RETURNS:
*       GT_OK      - on success,
*       GT_NO_MORE - no more entries in this FDB shadow.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart2
(
    IN GT_U8                         devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT mode,
    INOUT GT_U32                     *cookiePtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    **OUT_macEntryPtr
)
{
    GT_BOOL     foundNext = GT_FALSE;
    PRV_TGF_LIST_ITEM_PTR itemPtr; /* Is used for moving thru the
                                      brgFdbAuqMessagesPart2 list. */
    PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC *currInfoPtr;/* pointer to current info */
    GT_U32  currIndex = 0;
    GT_U32  startIndex = *cookiePtr;

    *cookiePtr = 0;
    *OUT_macEntryPtr = NULL;

    if(prvTgfBrgFdbDoNaShadow_part2 == 0)
    {
        return GT_NOT_INITIALIZED;
    }

    /* lock the DB */
    AU_FDB_UPDATE_LOCK();
    itemPtr = brgFdbAuqMessagesPart2.firstPtr;

    while (NULL != itemPtr)
    {
        currInfoPtr = (PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC*)itemPtr;
        itemPtr = itemPtr->nextPtr;
        if (GT_TRUE != currInfoPtr->deleted)
        {
            currIndex ++;
            if((currIndex-1) < startIndex)
            {
                continue;
            }

            if (currInfoPtr->message.isStatic == GT_TRUE &&
                mode == CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E)
            {
                continue;
            }
            else
            if (currInfoPtr->message.isStatic == GT_FALSE &&
                mode == CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E)
            {
                continue;
            }

            *OUT_macEntryPtr = &(currInfoPtr->message);
            foundNext = GT_TRUE;
            break;
        }
    }
    AU_FDB_UPDATE_UNLOCK();


    *cookiePtr = currIndex;

    if (foundNext == GT_TRUE)
    {
        return GT_OK;
    }

    return GT_NO_MORE;

}

