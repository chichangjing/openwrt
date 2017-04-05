/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* dxChVbMuxConfig.c
*
* DESCRIPTION:
*       hold generic configurations for DXCH device that is part of VB
*       (value blade) in ExMx / ExMxPm system
*
*       NOTE: code ported from PSS --> coreCpssCheetahVbMuxInit
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

#ifndef CHX_FAMILY
#error "this file must be compiled only when VB needed , and the CHX_FAMILY is defined"
#endif /*CHX_FAMILY*/


#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgPrvEdgeVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>


/* default STG id */
#define DEF_STG_ID  0

#define RC_CHECK(rc)    \
    {                                                                      \
        GT_STATUS __rc = (rc);                                             \
        if(__rc != GT_OK)                                                  \
        {                                                                  \
            osPrintf(" [%s] failed with code[%d] in line[%d] \n",#rc,__rc,__LINE__);\
            return __rc;                                                     \
        }                                                                  \
    }

/*******************************************************************************
 * External variables
 ******************************************************************************/


/*******************************************************************************
* appDemoDxChVbMuxInit
*
* DESCRIPTION:
*         Initialize DXCH device as VB multiplexer
*
* INPUTS:
*       ppIndex       - index of the device in array of appDemoPpConfigList[]
*
*       networkPortsNum - network Ports amount
*       networkPortsArr - array of network Ports (refer to networkPortsNum)
*       networkPortCscdArr - array of cascade Ports that match as egress private
*                           edge vlan to the network ports (refer to networkPortsNum)
*
*       cascadePortsNum - cascade Ports amount
*       cascadePortsArr - the cascade Ports (refer to cascadePortsNum)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChVbMuxInit
(
    IN  GT_U8                       ppIndex,

    IN  GT_U32                      networkPortsNum,
    IN  GT_U8                       networkPortsArr[],
    IN  GT_U8                       networkPortCscdArr[],

    IN  GT_U32                      cascadePortsNum,
    IN  GT_U8                       cascadePortsArr[]

)
{
    GT_U8   devNum;
    GT_U8   port;
    CPSS_PORTS_BMP_STC           portsTaggingBmp;
    CPSS_DXCH_BRG_VLAN_INFO_STC  vlanInfo;
    GT_U32  hwStpEntry[CPSS_DXCH_STG_ENTRY_SIZE_CNS];
    GT_U32  ii;
    CPSS_CSCD_LINK_TYPE_STC      nullPortCascadeLink={CPSS_NULL_PORT_NUM_CNS,
                            CPSS_CSCD_LINK_TYPE_PORT_E};

    devNum = appDemoPpConfigList[ppIndex].devNum;

    /* check that this is DXCH device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    osMemSet(&vlanInfo,0,sizeof(vlanInfo));
    osMemSet(&hwStpEntry,0,sizeof(hwStpEntry));

    /* init the vlan lib in CPSS */
    RC_CHECK(cpssDxChBrgVlanInit(devNum))

    /* init the stp lib in CPSS */
    RC_CHECK(cpssDxChBrgStpInit(devNum))

    /* VLAN table */
    /* Create all VLANs with all ports included*/
    /* Learning disabled */
    /* valid (bit1), New SA is Security Breach - No Learning (bit2),
       no-cpu-port(bit4), ... */
     /* Default STG 0 */

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsTaggingBmp);

    vlanInfo.unkSrcAddrSecBreach = GT_FALSE;
    vlanInfo.unregNonIpMcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4McastCmd  = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv6McastCmd  = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unkUcastCmd        = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4BcastCmd  = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.ipv4IgmpToCpuEn = GT_FALSE;
    vlanInfo.mirrToRxAnalyzerEn = GT_FALSE;
    vlanInfo.ipv6IcmpToCpuEn = GT_FALSE;
    vlanInfo.ipCtrlToCpuEn = GT_FALSE;
    vlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv4IpmBrgEn = GT_FALSE;
    vlanInfo.ipv6IpmBrgEn = GT_FALSE;
    vlanInfo.ipv6SiteIdMode = CPSS_IP_SITE_ID_INTERNAL_E;
    vlanInfo.ipv4UcastRouteEn = GT_FALSE;
    vlanInfo.ipv4McastRouteEn = GT_FALSE;
    vlanInfo.ipv6UcastRouteEn = GT_FALSE;
    vlanInfo.ipv6McastRouteEn = GT_FALSE;
    vlanInfo.stgId = DEF_STG_ID;
    vlanInfo.autoLearnDisable = GT_TRUE;
    vlanInfo.naMsgToCpuEn = GT_FALSE;
    vlanInfo.mruIdx = 0;
    vlanInfo.bcastUdpTrapMirrEn = GT_FALSE;

    /* set all Vlans from 0..4095 */
    RC_CHECK(cpssDxChBrgVlanEntriesRangeWrite(devNum,    /* devNum*/
                                          0,    /* vlanId*/
                                          4096,    /* numOfEntries*/
                                          &PRV_CPSS_PP_MAC(devNum)->existingPorts,    /* portsMembersPtr*/
                                          &portsTaggingBmp,    /* portsTaggingPtr*/
                                          &vlanInfo,/* vlanInfoPtr*/
                                          NULL))/*portsTaggingCmdPtr*/

    /* STG table */
    /* All ports are set to disabled state in STG 0 */
    /* two bits per port, 0 means "STP_DISALBED" */
    RC_CHECK(cpssDxChBrgStpEntryWrite(devNum,DEF_STG_ID,&hwStpEntry[0]))


    /* write HW device number*/
    RC_CHECK(cpssDxChCfgHwDevNumSet(devNum,appDemoPpConfigList[ppIndex].hwDevNum))

    /* enable private VLAN edge */
    RC_CHECK(cpssDxChBrgPrvEdgeVlanEnable(devNum,GT_TRUE))

    /* noDropInvalidSa */
    RC_CHECK(cpssDxChBrgGenDropInvalidSaEnable(devNum,GT_FALSE))
    /* noDropNonIpMc, noDropIpMc */
    RC_CHECK(cpssDxChBrgGenDropIpMcEnable(devNum,GT_FALSE))
    RC_CHECK(cpssDxChBrgGenDropNonIpMcEnable(devNum,GT_FALSE))

    /* disable dropping looped packets generated by the device and received back
    on one of Cascading ports ("added value chassis") */
    RC_CHECK(cpssDxChCscdDsaSrcDevFilterSet(devNum,GT_FALSE))

    /* loop on all network ports */
    for (ii = 0; (ii < networkPortsNum); ii++)
    {
        port = networkPortsArr[ii];

        /* PVE */
        /* Enable */
        /* target port is set according to VB init parameters */

        RC_CHECK(cpssDxChBrgPrvEdgeVlanPortEnable(devNum,                  /*devNum,  */
                                port,                                      /*portNum, */
                                GT_TRUE,                                   /*enable,  */
                                networkPortCscdArr[ii],                    /*dstPort, */
                                appDemoPpConfigList[ppIndex].hwDevNum,     /*dstDev,  */
                                GT_FALSE))                                 /*dstTrunk */

        /* VLAN Ingress filter */
        /* Disable */
        RC_CHECK(cpssDxChBrgVlanPortIngFltEnable(devNum,port,GT_FALSE))

        /* PVID */
        /* Set default vid = 0 */
        RC_CHECK(cpssDxChBrgVlanPortVidSet(devNum,port,CPSS_DIRECTION_INGRESS_E,0))

    }

    /* VLAN Egress filter */
    /* Disabled on all ports */
    for (port = 0; port < 32; port++)
    {
        if(PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum,port) == 0)
        {
            continue;
        }

        /* allow local switching -- otherwise the cheetah may drop packets on
           the ingress (not passing to the 'Switch device')*/
        RC_CHECK(cpssDxChBrgGenUcLocalSwitchingEnable(devNum,port,GT_TRUE))

        /* set all ports to be "locked port" */
        RC_CHECK(cpssDxChBrgFdbPortLearnStatusSet(devNum,port,GT_FALSE,CPSS_LOCK_FRWRD_E))

        /* disable Unknown UC egress filtering for all ports */
        /* forward Unknown Uc and Unreg Mc and send nothing to CPU */
        RC_CHECK(cpssDxChBrgPortEgrFltUnkEnable(devNum,port,GT_FALSE))
        RC_CHECK(cpssDxChBrgPortEgrFltUregMcastEnable(devNum,port,GT_FALSE))
        RC_CHECK(cpssDxChBrgPortEgressMcastLocalEnable(devNum,port,GT_FALSE))

        if(appDemoPpConfigList[ppIndex].devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            RC_CHECK(cpssDxChBrgPortEgrFltUregBcEnable(devNum,port,GT_FALSE))
        }
    }

    RC_CHECK(cpssDxChBrgVlanEgressFilteringEnable(devNum,GT_FALSE))
    RC_CHECK(cpssDxChBrgRoutedUnicastEgressFilteringEnable(devNum,GT_FALSE))
    RC_CHECK(cpssDxChBrgRoutedSpanEgressFilteringEnable(devNum,GT_FALSE))


    if(appDemoPpConfigList[ppIndex].devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* VLAN Range filter */
        /* Disabled on a device: range contains all VIDs  */
        RC_CHECK(cpssDxChBrgVlanEtherTypeSet(devNum,CPSS_VLAN_ETHERTYPE0_E,0x8100,4095))
        RC_CHECK(cpssDxChBrgVlanEtherTypeSet(devNum,CPSS_VLAN_ETHERTYPE1_E,0x8100,4095))
    }
    else
    {
        RC_CHECK(cpssDxChBrgVlanRangeSet(devNum, 4095))
    }


    /* Disable various of FDB msgs to CPU. */
    RC_CHECK(cpssDxChBrgFdbSpAaMsgToCpuSet(devNum, GT_FALSE))

    /* Address update messages to CPU */
    /* All types are disabled */
    RC_CHECK(cpssDxChBrgFdbAAandTAToCpuSet(devNum,GT_FALSE))
    for (port = 0; port < 32; port++)
    {
        if(PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum,port) == 0)
        {
            continue;
        }
        RC_CHECK(cpssDxChBrgFdbNaToCpuPerPortSet(devNum,port,GT_FALSE))
    }

    /* no NA msg to CPU if chain too long */
    /* NOT need API -- HW default */
    RC_CHECK(cpssDxChBrgFdbNaMsgOnChainTooLongSet(devNum,GT_FALSE))

    /* Cascade port configuration */
    /* add DSA tag */
    for (ii = 0; (ii < cascadePortsNum); ii++)
    {
        port = cascadePortsArr[ii];
        /* set cascading bit for port */
        /* set relevant bit of Egress DSA tag type Configuration register
           value 0=non-extended DSA Tag */

        if((appDemoPpConfigList[ppIndex].devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (appDemoPpConfigList[ppIndex].devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
        {
            /* Lion has 48 port so extended DSA should be used */
            RC_CHECK(cpssDxChCscdPortTypeSet(devNum,port,CPSS_PORT_DIRECTION_BOTH_E,CPSS_CSCD_PORT_DSA_MODE_EXTEND_E))
        }
        else
        {
            RC_CHECK(cpssDxChCscdPortTypeSet(devNum,port,CPSS_PORT_DIRECTION_BOTH_E,CPSS_CSCD_PORT_DSA_MODE_REGULAR_E))
        }
    }


    /* Egress DSA Tag Type Configuration */
    /* value 0 = Packets sent to the host CPU contain a non-extended DSA Tag.
      (bit 27 - for Cheetah, bit 31 - for Cheetah2)*/
    RC_CHECK(cpssDxChCscdPortTypeSet(devNum,CPSS_CPU_PORT_NUM_CNS,CPSS_PORT_DIRECTION_BOTH_E,CPSS_CSCD_PORT_DSA_MODE_REGULAR_E))

    for (ii = 0; (ii < cascadePortsNum); ii++)
    {
        port = cascadePortsArr[ii];

        if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType == PRV_CPSS_PORT_XG_E)
        {
            /* Configure short (4 bytes) preamble transmitted by for
               XG cascading port to compensate 4 bytes of DSA tag */
            RC_CHECK(cpssDxChPortPreambleLengthSet(devNum,port,
                                        CPSS_PORT_DIRECTION_TX_E,
                                        4))/* 4 bytes */

        }

    }

    /* set the local device to 'discard' the traffic destined to other devices */
    for(ii = 0 ; ii < 31 ; ii++)
    {
        if(ii == appDemoPpConfigList[ppIndex].hwDevNum)
        {
            continue;
        }

        RC_CHECK(cpssDxChCscdDevMapTableSet(devNum,
                        (GT_U8)ii,/*targetDevNum*/
                        0,/*sourceDevNum -- don't care*/
                        0,/*portNum*/
                        &nullPortCascadeLink, /*cascadeLinkPtr*/
                        CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E, /*srcPortTrunkHashEn*/
                        GT_FALSE))/*egressAttributesLocallyEn*/
    }

    return GT_OK;
}
