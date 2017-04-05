/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoOam.c
*
* DESCRIPTION:
*       OAM AppDemo.
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h> 
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOam.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOamSrv.h>
#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>
/*******************************************************************************
 * External Functions
 ******************************************************************************/
#define DEBUG_APPDEMO_OAM 0

#ifdef ASIC_SIMULATION 
typedef GT_STATUS (*RX_PACKET_RECEIVE_CB_FUN)
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    IN GT_U32                               numOfBuff,
    IN GT_U8                               *packetBuffs[],
    IN GT_U32                               buffLen[],
    IN void                                 *rxParamsPtr
);

extern GT_STATUS appDemoDxChNetRxPacketCbRegister(
    IN  RX_PACKET_RECEIVE_CB_FUN  (rxPktReceiveCbFun)
);

GT_STATUS prvGtAppDemoOamCbFunc(
    GT_U8      devNum,
    GT_U8      queueIdx,
    GT_U32     numOfBuff,
    GT_U8      *packetBuffs[],
    GT_U32     buffLen[],
    void       *rxParamsPtr);

GT_BOOL gtAppDemoOamCbRegistered = GT_FALSE;
#endif

/*******************************************************************************
 * External variables
 ******************************************************************************/

extern GT_U32   appDemoPpConfigDevAmount;/* number of device PP, from cpssInitSystem*/

/*******************************************************************************
 * Local usage variables
 ******************************************************************************/
#define GT_APP_DEMO_OAM_RX_QUEUE_NUM                7 /* CCM, LB */
#define GT_APP_DEMO_OAM_RX_QUEUE_NUM_LM_DM          6 
#define GT_APP_DEMO_OAM_RX_QUEUE_NUM_LMR_DMR        6 
#define GT_APP_DEMO_OAM_TX_QUEUE_NUM                0

/* cpssInitSystem defines the start of TTI rules in TCAM from 18K.
   This index is used for the TTI rule that classifies the BFD over
   IPv4 over MPLS packets */
#define GT_APP_DEMO_OAM_TTI_FIRST_INDEX          18432 /* 18K*/
/* This index is used for the PCL rule that classifies the BFD over
   IPv4 packets */
#define GT_APP_DEMO_OAM_PCL_BFD_IPV4_INDEX       0

/* OAM profile index */
#define GT_APP_DEMO_OAM_PROFILE_1731_CNS         0
#define GT_APP_DEMO_OAM_PROFILE_UP_MEP_CNS       1
/* OAM base flow ID */
#define GT_APP_DEMO_OAM_BASEFLOW_ID_CNS          0
/* OAM ethertype*/
#define GT_APP_DEMO_OAM_ETHERTYPE                0x8902

/* CPU code that will be assigned to packets when trapped to CPU */
#define GT_APP_DEMO_OAM_PCL_CPU_CODE             CPSS_NET_FIRST_USER_DEFINED_E/* HW code 192 */
#define GT_APP_DEMO_OAM_LMR_DMR_CPU_CODE         GT_APP_DEMO_OAM_PCL_CPU_CODE + 0 /*??*/
#define GT_APP_DEMO_OAM_LM_DM_CPU_CODE           GT_APP_DEMO_OAM_PCL_CPU_CODE + 1
#define GT_APP_DEMO_OAM_CCM_CPU_CODE             GT_APP_DEMO_OAM_PCL_CPU_CODE + 2
#define GT_APP_DEMO_OAM_LBR_CPU_CODE             GT_APP_DEMO_OAM_PCL_CPU_CODE + 3

/* number of ports in BC2 */
#define GT_APP_DEMO_OAM_BC2_PORTS_NUM_CNS        72 

/* Tx SDMA queue used as generator mode */
#define GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS  3 
/* Flow ID based counting, Minimum flow ID
   used for index to billing */
#define GT_APP_DEMO_OAM_BILLING_MIN_FLOW_ID      1
/* base index for ingress offset table */
#define GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX 0
/* base index for egress offset table */
#define GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX  4
/* index for ingress dual-ended LM offset table */
#define GT_APP_DEMO_OAM_INGRESS_PTP_DE_LM_OFFSET_INDEX 8
/* index for egress dual-ended LM offset table */
#define GT_APP_DEMO_OAM_EGRESS_PTP_DE_LM_OFFSET_INDEX  9
/* PCL rule index base for Egress CCM Up MEP  */
#define GT_APP_DEMO_OAM_UP_MEP_PCL_EGRESS_INDEX_BASE   100
/* PCL rule index base for Rx LM data */
#define GT_APP_DEMO_OAM_LM_DATA_PCL_INDEX_BASE   200
/* PCL rule index base for Tx LM  */
#define GT_APP_DEMO_OAM_LM_PCL_EGRESS_INDEX_BASE 300
/* PCL rule index base for Tx LM data */
#define GT_APP_DEMO_OAM_LM_DATA_PCL_EGRESS_INDEX_BASE   400

#define GT_APP_DEMO_OAM_UP_MEP_AGING_PERIOD_ENTRY_INDEX 1

GT_BOOL gtAppDemoOAmSrvCpuInited = GT_FALSE;

/*******************************************************************************
 * Private functions implementation
 ******************************************************************************/


/*******************************************************************************
* gtAppDemoOamTtiInitBfdMpls
*
* DESCRIPTION:
*       This function does specific initialization of TTI engine needed for BFD
*       IPv4 MPLS packet classification.
*       TTI lookup is enabled for UDB MPLS packets, key size is defined 
*       to 20 UDB, initialization of TTI UDB needed for BFD IPv4 MPLS
*       packet classification according to: PCL-ID, MPLS Label0, S1 field,
*       MPLS payload protocol, IPv4 TTL, IP Protocol and Destination UDP port.   
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamTtiInitBfdMpls(GT_VOID)
{
    GT_STATUS   rc; /* function return code */
    GT_PORT_NUM port; /* port number */
    GT_U8       devNum; /* device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        for(port = 0; port < GT_APP_DEMO_OAM_BC2_PORTS_NUM_CNS; port++)
        {
            rc = cpssDxChTtiPortLookupEnableSet(devNum,
                                                port,
                                                CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                                GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPortLookupEnableSet",rc);
            if(rc != GT_OK)
            {            
                return rc;
            }

        }
        /* set UDB key size to 20B */
        rc =  cpssDxChTtiPacketTypeKeySizeSet(devNum,
                                              CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                              CPSS_DXCH_TTI_KEY_SIZE_20_B_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPacketTypeKeySizeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* PCL -ID */
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           0,
                                           CPSS_DXCH_TTI_OFFSET_METADATA_E,
                                           22);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           1,
                                           CPSS_DXCH_TTI_OFFSET_METADATA_E,
                                           23);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Label0*/
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           2,
                                           CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E,
                                           2);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           3,
                                           CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E,
                                           3);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           4,
                                           CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E,
                                           4);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
         /* S1 */
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           5,
                                           CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E,
                                           8);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* MPLS payload protocol */
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           6,
                                           CPSS_DXCH_TTI_OFFSET_METADATA_E,
                                           9);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* IPv4 TTL */
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           7,
                                           CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E,
                                           10);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* IP Protocol */
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           8,
                                           CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E,
                                           11); 
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Destination UDP port */
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           9,
                                           CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E,
                                           24);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChTtiUserDefinedByteSet(devNum,
                                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E,
                                           10,
                                           CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E,
                                           25);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamTtiInitBfdMplsTp
*
* DESCRIPTION:
*       This function does specific initialization of TTI engine needed for BFD
*       MPLS-TP packet classification.
*       TTI lookup is enabled for MPLS packets, control word channel type profile
*       table is configured with channel type 0x22 for MPLS-TP BFD.   
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamTtiInitBfdMplsTp(GT_VOID)
{
    GT_STATUS   rc; /* function return code */
    GT_PORT_NUM port; /* port number */
    GT_U8       devNum; /* device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        for(port = 0; port < GT_APP_DEMO_OAM_BC2_PORTS_NUM_CNS; port++)
        {
            rc = cpssDxChTtiPortLookupEnableSet(devNum,
                                                port,
                                                CPSS_DXCH_TTI_KEY_MPLS_E,
                                                GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPortLookupEnableSet",rc);
            if(rc != GT_OK)
            {            
                return rc;
            }

        }
        rc = cpssDxChOamMplsCwChannelTypeProfileSet(devNum, 0x2, 0x22, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamMplsCwChannelTypeProfileSet",rc);
        if(rc != GT_OK)
        {            
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamTtiInitCfmMplsTp
*
* DESCRIPTION:
*       This function does specific initialization of TTI engine needed for OAM
*       CFM MPLS-TP packet classification.
*       TTI lookup is enabled for MPLS packets, control word channel type profile
*       table is configured with channel type 0x8902 for MPLS-TP OAM CFM.   
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamTtiInitCfmMplsTp(GT_VOID)
{
    GT_STATUS   rc; /* function return code */
    GT_PORT_NUM port; /* port number */
    GT_U8       devNum; /* device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        for(port = 0; port < GT_APP_DEMO_OAM_BC2_PORTS_NUM_CNS; port++)
        {
            rc = cpssDxChTtiPortLookupEnableSet(devNum,
                                                port,
                                                CPSS_DXCH_TTI_KEY_MPLS_E,
                                                GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPortLookupEnableSet",rc);
            if(rc != GT_OK)
            {            
                return rc;
            }

        }
        rc = cpssDxChOamMplsCwChannelTypeProfileSet(devNum, 0x1, 0x8902, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamMplsCwChannelTypeProfileSet",rc);
        if(rc != GT_OK)
        {            
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclInitGen
*
* DESCRIPTION:
*       This function does generic initialization of PCL engine. 
*       It is enabled per port and access to the PCL configuration table 
*       is defined by port, also PCL configuration table is configured for 
*       lookup 0 when accessed by port type.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclInitGen(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    CPSS_DXCH_PCL_LOOKUP_CFG_STC        lookupCfg; /* lookup configuration */
    CPSS_INTERFACE_INFO_STC             interfaceInfo; /* interface data: port, VLAN, or index */
    GT_PORT_NUM                         port; /* port number */
    GT_HW_DEV_NUM                       hwDevNum; /* HW device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        /* PCL per port configurations */
        /* enable IPCL on all ports */
        for(port = 0; port < GT_APP_DEMO_OAM_BC2_PORTS_NUM_CNS; port++)
        {
            /* ingress PCL configuration*/
            /* enable pcl on ingress port */
            rc = cpssDxChPclPortIngressPolicyEnable(devNum, port, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclPortIngressPolicyEnable",rc);
            if(rc != GT_OK)
            {
                return rc;
            }    
            /* enable lookup 0_0 on port */
            /* define that the accesss to PCL configuration table index done via port */
            rc = cpssDxChPclPortLookupCfgTabAccessModeSet(devNum,
                              port,
                              CPSS_PCL_DIRECTION_INGRESS_E,
                              CPSS_PCL_LOOKUP_0_E,
                              0, /* subLookupNum */
                              CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclPortLookupCfgTabAccessModeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }  
            /* set lookup 0_0 parameters 
               IPv4 packets that will arrive to this port will use 
               key of type CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E */
            cpssOsMemSet(&lookupCfg, '\0', sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));
            cpssOsMemSet(&interfaceInfo, '\0', sizeof(CPSS_INTERFACE_INFO_STC));
        
            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.dualLookup = GT_FALSE;
            lookupCfg.pclId = 0x000;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            lookupCfg.udbKeyBitmapEnable = GT_FALSE;

            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
			rc = cpssDxChCfgHwDevNumGet(devNum,&hwDevNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            interfaceInfo.devPort.hwDevNum = hwDevNum;
            interfaceInfo.devPort.portNum = port;
    
            rc = cpssDxChPclCfgTblSet(devNum,
                                      &interfaceInfo,
                                      CPSS_PCL_DIRECTION_INGRESS_E,
                                      CPSS_PCL_LOOKUP_NUMBER_0_E,
                                      &lookupCfg);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclCfgTblSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }         
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclInitGenDualEndedLm
*
* DESCRIPTION:
*       This function does generic initialization of PCL engine. 
*       It is enabled per port and access to the PCL configuration table 
*       is defined by port, also PCL configuration table is configured for 
*       lookup 0 when accessed by port type.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclInitGenDualEndedLm(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    CPSS_DXCH_PCL_LOOKUP_CFG_STC        lookupCfg; /* lookup configuration */
    CPSS_INTERFACE_INFO_STC             interfaceInfo; /* interface data: port, VLAN, or index */
    GT_PORT_NUM                         port; /* port number */
    GT_HW_DEV_NUM                       hwDevNum; /* HW device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        /* PCL per port configurations */
        /* enable IPCL on all ports */
        for(port = 0; port < GT_APP_DEMO_OAM_BC2_PORTS_NUM_CNS; port++)
        {
            /* ingress PCL configuration*/
            /* enable pcl on ingress port */
            rc = cpssDxChPclPortIngressPolicyEnable(devNum, port, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclPortIngressPolicyEnable",rc);
            if(rc != GT_OK)
            {
                return rc;
            }    
            /* enable lookup 0_0 on port */
            /* define that the accesss to PCL configuration table index done via port */
            rc = cpssDxChPclPortLookupCfgTabAccessModeSet(devNum,
                              port,
                              CPSS_PCL_DIRECTION_INGRESS_E,
                              CPSS_PCL_LOOKUP_0_E,
                              0, /* subLookupNum */
                              CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclPortLookupCfgTabAccessModeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }  
            /* set lookup 0_0 parameters 
               IPv4 packets that will arrive to this port will use 
               key of type CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E */
            cpssOsMemSet(&lookupCfg, '\0', sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));
            cpssOsMemSet(&interfaceInfo, '\0', sizeof(CPSS_INTERFACE_INFO_STC));
        
            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.dualLookup = GT_FALSE;
            lookupCfg.pclId = 0x000;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            lookupCfg.udbKeyBitmapEnable = GT_FALSE;

            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            rc = cpssDxChCfgHwDevNumGet(devNum,&hwDevNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            interfaceInfo.devPort.hwDevNum = hwDevNum;
            interfaceInfo.devPort.portNum = port;
    
            rc = cpssDxChPclCfgTblSet(devNum,
                                      &interfaceInfo,
                                      CPSS_PCL_DIRECTION_INGRESS_E,
                                      CPSS_PCL_LOOKUP_NUMBER_0_E,
                                      &lookupCfg);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclCfgTblSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }         
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamEPclInitGen
*
* DESCRIPTION:
*       This function does generic initialization of Egress PCL engine. 
*       It is enabled per port and access to the PCL configuration table 
*       is defined by port and packet type, also PCL configuration table
*       is configured for lookup 0 when accessed by port type.
*       lookup rule format for non IP pavkets is configured to 20 UDBs.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEPclInitGen(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    CPSS_DXCH_PCL_LOOKUP_CFG_STC        lookupCfg; /* lookup configuration */
    CPSS_INTERFACE_INFO_STC             interfaceInfo; /* interface data: port, VLAN, or index */
    GT_PORT_NUM                         port; /* port number */
    GT_HW_DEV_NUM                       hwDevNum; /* HW device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        /* PCL per port configurations */
        /* enable pcl on egress port */
        for(port = 0; port < GT_APP_DEMO_OAM_BC2_PORTS_NUM_CNS; port++)
        {   
            /* enable EPCL on all ports for packet type FROM CPU */
            rc = cpssDxCh2EgressPclPacketTypesSet(devNum,
                                                   port,
                                                   CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E,
                                                   GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh2EgressPclPacketTypesSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }  
            /* enable EPCL on all ports for packet type FORWARD Non-TS */
            rc = cpssDxCh2EgressPclPacketTypesSet(devNum,
                                                   port,
                                                   CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E,
                                                   GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh2EgressPclPacketTypesSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }  
            /* enable lookup 0_0 on port */
            /* define that the accesss to PCL configuration table index done via port */
            rc = cpssDxChPclPortLookupCfgTabAccessModeSet(devNum,
                              port,
                              CPSS_PCL_DIRECTION_EGRESS_E,
                              CPSS_PCL_LOOKUP_0_E,
                              0, /* subLookupNum */
                              CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclPortLookupCfgTabAccessModeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }  
            /* set lookup 0_0 parameters */
            cpssOsMemSet(&lookupCfg, '\0', sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));
            cpssOsMemSet(&interfaceInfo, '\0', sizeof(CPSS_INTERFACE_INFO_STC));
        
            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.dualLookup = GT_FALSE;
            lookupCfg.pclId = 0x003;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;
            lookupCfg.udbKeyBitmapEnable = GT_FALSE;

            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            rc = cpssDxChCfgHwDevNumGet(devNum,&hwDevNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            interfaceInfo.devPort.hwDevNum = hwDevNum;
            interfaceInfo.devPort.portNum = port;
    
            rc = cpssDxChPclCfgTblSet(devNum,
                                      &interfaceInfo,
                                      CPSS_PCL_DIRECTION_EGRESS_E,
                                      CPSS_PCL_LOOKUP_NUMBER_0_E,
                                      &lookupCfg);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclCfgTblSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }         
        }
    }
    return GT_OK;
}
/*******************************************************************************
* gtAppDemoOamPclInitBfdIpv4
*
* DESCRIPTION:
*       This function does specific initialization of PCL UDB needed for BFD IPv4 
*       packet per flow classification according to: PCL-ID, IPv4 SIP & DIP and 
*       BFD My Discriminator & Your Discriminator, also configure PCL 
*       configuration table for lookup 1. 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclInitBfdIpv4(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    CPSS_DXCH_PCL_LOOKUP_CFG_STC        lookupCfg; /* lookup configuration */
    CPSS_INTERFACE_INFO_STC             interfaceInfo; /* interface data: port, VLAN, or index */
    GT_U8                               offset; /* UDB offset from anchor */
    GT_U32                              udbIndex; /* UDB index */
    CPSS_DXCH_PCL_UDB_SELECT_STC        udbSelect; /* UDB Selection configuration */
   /* CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC crcMaskEntry;*/

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&udbSelect, '\0', sizeof(CPSS_DXCH_PCL_UDB_SELECT_STC));
        /* UDB for per flow PCL rule, configure UDB[2-9]
           UDB[0-1] are replaced by PCL-ID */
        for(udbIndex = 2, offset = 14 ; udbIndex < 10; udbIndex++,offset++)
        {
            udbSelect.udbSelectArr[udbIndex] = udbIndex;
            /* UDB[2-9] offset for IPv4 SIP & DIP */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E, 
                                               CPSS_PCL_DIRECTION_INGRESS_E,
                                               udbIndex,
                                               CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        /* UDB for per flow PCL rule, configure UDB[10-17] */
        for(offset = 12 ; udbIndex < 18; udbIndex++,offset++)
        {
            udbSelect.udbSelectArr[udbIndex] = udbIndex;
            /* UDB[10-17] offset for BFD My Discriminator & Your Discriminator */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E, 
                                               CPSS_PCL_DIRECTION_INGRESS_E,
                                               udbIndex,
                                               CPSS_DXCH_PCL_OFFSET_L4_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        /* UDB18 - IPv4 TTL */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           0/* Not relevant */, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           18,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           10);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return GT_FAIL;
        }
        udbSelect.ingrUdbReplaceArr[0] = GT_TRUE;
        udbSelect.ingrUdbReplaceArr[1] = GT_TRUE;
        rc = cpssDxChPclUserDefinedBytesSelectSet(devNum,
                                                  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E,
                                                  CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E,
                                                  CPSS_PCL_LOOKUP_NUMBER_1_E,
                                                  &udbSelect);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }        
                 
         /* set lookup 0_1 parameters for second lookup as type index
            IPv4 packets that will arrive to this port will use 
            key of type CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E */
        cpssOsMemSet(&lookupCfg, '\0', sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));
        cpssOsMemSet(&interfaceInfo, '\0', sizeof(CPSS_INTERFACE_INFO_STC));
    
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.dualLookup = GT_FALSE;
        lookupCfg.pclId = 0x001;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E;
        lookupCfg.udbKeyBitmapEnable = GT_FALSE;

        interfaceInfo.type = CPSS_INTERFACE_INDEX_E;
        interfaceInfo.index = 0x0;

        rc = cpssDxChPclCfgTblSet(devNum,
                                  &interfaceInfo,
                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                  CPSS_PCL_LOOKUP_NUMBER_1_E,
                                  &lookupCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclInitBfdMpls
*
* DESCRIPTION:
*       This function does specific initialization of PCL UDB needed for BFD IPv4 
*       MPLS packet per flow classification according to: PCL-ID, IPv4 SIP & DIP
*       MPLS application labelnd FD My Discriminator & Your Discriminator
*       also configure PCL configuration table for lookup 0. 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclInitBfdMpls(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    CPSS_DXCH_PCL_LOOKUP_CFG_STC        lookupCfg; /* lookup configuration */
    CPSS_INTERFACE_INFO_STC             interfaceInfo; /* interface data: port, VLAN, or index */
    GT_U8                               offset; /* UDB offset from anchor */
    GT_U32                              udbIndex; /* UDB index */
    GT_U32                              mapUdbIndex; /* mapped UDB index */
    CPSS_DXCH_PCL_UDB_SELECT_STC        udbSelect; /* UDB Selection configuration */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&udbSelect, '\0', sizeof(CPSS_DXCH_PCL_UDB_SELECT_STC));
        /* UDB for per flow PCL rule, configure UDB[2-9]
           UDB[0-1] are replaced by PCL-ID */
        for(udbIndex = 2, offset = 14 ; udbIndex < 10; udbIndex++,offset++)
        {
            udbSelect.udbSelectArr[udbIndex] = udbIndex;

            /* UDB[2-9] offset for BFD over IPv4 over MPLS SIP & DIP */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E, 
                                               CPSS_PCL_DIRECTION_INGRESS_E,
                                               udbIndex,
                                               CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        for(offset = 6 ; udbIndex < 13; udbIndex++,offset++)
        {
            udbSelect.udbSelectArr[udbIndex] = udbIndex;

            /* UDB[10-12] offset for BFD over IPv4 over MPLS application label */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E, 
                                               CPSS_PCL_DIRECTION_INGRESS_E,
                                               udbIndex,
                                               CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        for(offset = 12, mapUdbIndex = 16  ; udbIndex < 21; udbIndex++, offset++, mapUdbIndex++)
        {
            /* map UDB[13-20] -> UDB[16-23] because UDB[14-15] are needed for CRC hash calculations */
            udbSelect.udbSelectArr[udbIndex] = mapUdbIndex;

            /* UDB[13-20] offset for BFD My Discriminator & Your Discriminator */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E, 
                                               CPSS_PCL_DIRECTION_INGRESS_E,
                                               mapUdbIndex,
                                               CPSS_DXCH_PCL_OFFSET_L4_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        udbSelect.ingrUdbReplaceArr[0] = GT_TRUE;
        udbSelect.ingrUdbReplaceArr[1] = GT_TRUE;
        rc = cpssDxChPclUserDefinedBytesSelectSet(devNum,
                                                  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E,
                                                  CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E,
                                                  CPSS_PCL_LOOKUP_NUMBER_0_E,
                                                  &udbSelect);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set lookup 0_0 parameters as type index for TTI override
            MPLS packets that will arrive to this port will use 
            key of type CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E */
        cpssOsMemSet(&lookupCfg, '\0', sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));
        cpssOsMemSet(&interfaceInfo, '\0', sizeof(CPSS_INTERFACE_INFO_STC));
    
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.dualLookup = GT_FALSE;
        lookupCfg.pclId = 0x001;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        lookupCfg.udbKeyBitmapEnable = GT_FALSE;

        interfaceInfo.type = CPSS_INTERFACE_INDEX_E;
        interfaceInfo.index = 0x0;

        rc = cpssDxChPclCfgTblSet(devNum,
                                  &interfaceInfo,
                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                  CPSS_PCL_LOOKUP_NUMBER_0_E,
                                  &lookupCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclCfgTblSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclInitBfdMplsTp
*
* DESCRIPTION:
*       This function does specific initialization of PCL UDB needed for BFD  
*       MPLS-TP packet per flow classification according to: My Discriminator
*       & Your Discriminator also configure PCL configuration table for lookup 0.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclInitBfdMplsTp(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    CPSS_DXCH_PCL_LOOKUP_CFG_STC        lookupCfg; /* lookup configuration */
    CPSS_INTERFACE_INFO_STC             interfaceInfo; /* interface data: port, VLAN, or index */
    GT_U8                               offset; /* UDB offset from anchor */
    GT_U32                              udbIndex; /* UDB index */
    CPSS_DXCH_PCL_UDB_SELECT_STC        udbSelect; /* UDB Selection configuration */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&udbSelect, '\0', sizeof(CPSS_DXCH_PCL_UDB_SELECT_STC));
        /* UDB for per flow PCL rule, configure UDB[2-9]
           UDB[0-1] are replaced by PCL-ID */
        for(udbIndex = 2, offset = 10 ; udbIndex < 10; udbIndex++,offset++)
        {
            udbSelect.udbSelectArr[udbIndex] = udbIndex;

            /* UDB[2-9] offset for BFD My Discriminator & Your Discriminator */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E, 
                                               CPSS_PCL_DIRECTION_INGRESS_E,
                                               udbIndex,
                                               CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        udbSelect.ingrUdbReplaceArr[0] = GT_TRUE;
        udbSelect.ingrUdbReplaceArr[1] = GT_TRUE;
        rc = cpssDxChPclUserDefinedBytesSelectSet(devNum,
                                                  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E,
                                                  CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E,
                                                  CPSS_PCL_LOOKUP_NUMBER_0_E,
                                                  &udbSelect);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set lookup 0_0 parameters as type index for TTI override
            MPLS packets that will arrive to this port will use 
            key of type CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E */
        cpssOsMemSet(&lookupCfg, '\0', sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));
        cpssOsMemSet(&interfaceInfo, '\0', sizeof(CPSS_INTERFACE_INFO_STC));
    
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.dualLookup = GT_FALSE;
        lookupCfg.pclId = 0x001;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        lookupCfg.udbKeyBitmapEnable = GT_FALSE;

        interfaceInfo.type = CPSS_INTERFACE_INDEX_E;
        interfaceInfo.index = 0x0;

        rc = cpssDxChPclCfgTblSet(devNum,
                                  &interfaceInfo,
                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                  CPSS_PCL_LOOKUP_NUMBER_0_E,
                                  &lookupCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclCfgTblSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclInitCfm
*
* DESCRIPTION:
*       This function does specific initialization of PCL UDB needed for CFM packet
*       classification according to: PCL-ID, CFM  Source MAC, Orig VID,
*       Local Dev Src Port and Ethertype.
*       configure OAM engine profile 0 UDBs: OAM opcode, MEG Level, RDI
*       and Tx Period, also sets UDE0 as OAM ether type 0x8902.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclInitCfm(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    GT_U8                               offset; /* UDB offset from anchor */
    GT_U32                              udbIndex; /* UDB index */
    GT_U32                              mapUdbIndex; /* mapped UDB index */
    GT_U32                              metaUdbIndex; /* metadata UDB index */
    CPSS_DXCH_PCL_UDB_SELECT_STC        udbSelect; /* UDB Selection configuration */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&udbSelect, '\0', sizeof(CPSS_DXCH_PCL_UDB_SELECT_STC));

        /* Configure relevant UDBs for OAM engine profile 0 */
        /* OAM opcode UDB[0] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           0x0 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           3/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* MEG Level UDB[2] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           0x2 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           2/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* RDI UDB[4] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           0x4 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           4/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Tx Period UDB[6] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           0x6 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           4/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* UDB for per flow CFM PCL rule, configure UDB[2-13]
           UDB[0-1] are replaced by PCL-ID 
           map UDB[2-13] -> UDB[16-27] in order to avoid OAM profile 0/1 UDB and Hash configuration */
        for(udbIndex = 2, mapUdbIndex=16, offset = 0 ; udbIndex < 14; udbIndex++,offset++,mapUdbIndex++)
        {
            udbSelect.udbSelectArr[udbIndex] = mapUdbIndex;

            /* UDB[2-7] offset for CFM  Destination MAC */
            /* UDB[8-13] offset for CFM  Source MAC */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                               CPSS_PCL_DIRECTION_INGRESS_E,
                                               mapUdbIndex,
                                               CPSS_DXCH_PCL_OFFSET_L2_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }            
        }
        /* MetaData anchor type is only applicable to UDBs 30-49.
           map udb[14-18] -> udb[30-34] */       
        metaUdbIndex = 30;
        /* Orig VID, UDB[14-15]*/ 
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           27);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        udbIndex++;
        metaUdbIndex++;
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           28);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Local Dev Src Port, UDB[16] */
        metaUdbIndex++;
        udbIndex++;
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           10);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Ethertype CFM packet, UDB[17-18] */
        metaUdbIndex++;
        udbIndex++;
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           24);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        metaUdbIndex++;
        udbIndex++;
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           25);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        udbSelect.ingrUdbReplaceArr[0] = GT_TRUE;
        udbSelect.ingrUdbReplaceArr[1] = GT_TRUE;
        rc = cpssDxChPclUserDefinedBytesSelectSet(devNum,
                                                  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E,
                                                  CPSS_DXCH_PCL_PACKET_TYPE_UDE_E,
                                                  CPSS_PCL_LOOKUP_NUMBER_0_E,
                                                  &udbSelect);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }     
        /* Set UDE 0 for OAM ether type*/
        rc = cpssDxChPclUdeEtherTypeSet(devNum, 0, GT_APP_DEMO_OAM_ETHERTYPE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUdeEtherTypeSet",rc);
        if(rc != GT_OK)
        {
            return GT_FAIL;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclInitCfmMplsTp
*
* DESCRIPTION:
*       This function does specific initialization of PCL UDB needed for MPLS-TP 
*       CFM packet classification according to: MPLS lable 0.
*       configure OAM engine profile 0 UDBs: OAM opcode, MEG Level, RDI
*       and Tx Period also configure PCL configuration table for lookup 0.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclInitCfmMplsTp(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    CPSS_DXCH_PCL_LOOKUP_CFG_STC        lookupCfg; /* lookup configuration */
    CPSS_INTERFACE_INFO_STC             interfaceInfo; /* interface data: port, VLAN, or index */
    GT_U8                               offset; /* UDB offset from anchor */
    GT_U32                              udbIndex; /* UDB index */
    CPSS_DXCH_PCL_UDB_SELECT_STC        udbSelect; /* UDB Selection configuration */
    GT_U32                              mapUdbIndex; /* mapped UDB index */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&udbSelect, '\0', sizeof(CPSS_DXCH_PCL_UDB_SELECT_STC));
        /* Configure relevant UDBs for OAM engine profile 0 */
        /* OAM opcode UDB[0] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           0x0 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           7/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* MEG Level UDB[2] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           0x2 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           6/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* RDI UDB[4] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           0x4 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           8/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Tx Period UDB[6] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           0x6 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           8/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }  
       
        /* UDB for per flow PCL rule, configure UDB[7-9]
           UDB[0-1] are replaced by PCL-ID 
           map UDB[2-4] -> UDB[10-12] in order to avoid OAM profile 0/1 UDB configuration */
        for(udbIndex = 2, offset = 2, mapUdbIndex = 10; udbIndex < 5; udbIndex++,offset++,mapUdbIndex++)
        {
            udbSelect.udbSelectArr[udbIndex] = mapUdbIndex;

            /* UDB[2-4] offset for MPLS Transport Label */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E, 
                                               CPSS_PCL_DIRECTION_INGRESS_E,
                                               mapUdbIndex,
                                               CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        udbSelect.ingrUdbReplaceArr[0] = GT_TRUE;
        udbSelect.ingrUdbReplaceArr[1] = GT_TRUE;
        rc = cpssDxChPclUserDefinedBytesSelectSet(devNum,
                                                  CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E,
                                                  CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E,
                                                  CPSS_PCL_LOOKUP_NUMBER_0_E,
                                                  &udbSelect);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set lookup 0_0 parameters as type index for TTI override
            MPLS packets that will arrive to this port will use 
            key of type CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E */
        cpssOsMemSet(&lookupCfg, '\0', sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));
        cpssOsMemSet(&interfaceInfo, '\0', sizeof(CPSS_INTERFACE_INFO_STC));
    
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.dualLookup = GT_FALSE;
        lookupCfg.pclId = 0x001;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        lookupCfg.udbKeyBitmapEnable = GT_FALSE;

        interfaceInfo.type = CPSS_INTERFACE_INDEX_E;
        interfaceInfo.index = 0x0;

        rc = cpssDxChPclCfgTblSet(devNum,
                                  &interfaceInfo,
                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                  CPSS_PCL_LOOKUP_NUMBER_0_E,
                                  &lookupCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclCfgTblSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }         
    }
    return GT_OK;
}
/*******************************************************************************
* gtAppDemoOamEPclInitCfm
*
* DESCRIPTION:
*       This function does specific initialization of Egress PCL UDB needed for
*       CFM packet classification according to: PCL-ID, CFM Destination MAC, 
*       CFM  Source MAC, Orig VID, Egress DSA Tag Cmd and Ethertype.
*       configure OAM engine profile 0 UDBs: OAM opcode, MEG Level, RDI
*       and Tx Period.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEPclInitCfm(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    GT_U8                               offset; /* UDB offset from anchor */
    GT_U32                              udbIndex; /* UDB index */
    GT_U32                              mapUdbIndex; /* mapped UDB index */
    GT_U32                              metaUdbIndex; /* metadata UDB index */
    CPSS_DXCH_PCL_UDB_SELECT_STC        udbSelect; /* UDB Selection configuration */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&udbSelect, '\0', sizeof(CPSS_DXCH_PCL_UDB_SELECT_STC));

        /* Configure relevant UDBs for OAM engine profile 0 */
        /* OAM opcode UDB[0] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           0x0 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           3/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* MEG Level UDB[2] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           0x2 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           2/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* RDI UDB[4] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           0x4 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           4/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Tx Period UDB[6] */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           0x6 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           4/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* UDB for per flow CFM PCL rule, configure UDB[2-13]
           UDB[0-1] are replaced by PCL-ID 
           map UDB[2-13] -> UDB[12-23] in order to avoid OAM profile 0/1 UDB */
        for(udbIndex = 2, mapUdbIndex=12, offset = 0 ; udbIndex < 14; udbIndex++,offset++,mapUdbIndex++)
        {
            udbSelect.udbSelectArr[udbIndex] = mapUdbIndex;

            /* UDB[2-7] offset for CFM  Destination MAC */
            /* UDB[8-13] offset for CFM  Source MAC */
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                               CPSS_PCL_DIRECTION_EGRESS_E,
                                               mapUdbIndex,
                                               CPSS_DXCH_PCL_OFFSET_L2_E,
                                               offset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }            
        }
        /* MetaData anchor type is only applicable to UDBs 30-49.
           map udb[14-18] -> udb[30-34] */       
        metaUdbIndex = 30;
        /* Orig VID, UDB[14-15]*/ 
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           24);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        udbIndex++;
        metaUdbIndex++;
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           25);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Src Port, UDB[16] */
        metaUdbIndex++;
        udbIndex++;
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           16);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Ethertype CFM packet, UDB[17-18] */
        metaUdbIndex++;
        udbIndex++;
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           22);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        metaUdbIndex++;
        udbIndex++;
        udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_EGRESS_E,
                                           metaUdbIndex,
                                           CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                           23);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Set UDB mapping and replace UDB[0-1] with PCL-ID */
        udbSelect.egrUdb01Replace = GT_TRUE;
        rc = cpssDxChPclUserDefinedBytesSelectSet(devNum,
                                                  CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E,
                                                  CPSS_DXCH_PCL_PACKET_TYPE_UDE_E,
                                                  CPSS_PCL_LOOKUP_NUMBER_0_E,
                                                  &udbSelect);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        } 

        rc = cpssDxChPclUdeEtherTypeSet(devNum, 0, GT_APP_DEMO_OAM_ETHERTYPE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUdeEtherTypeSet",rc);
        if(rc != GT_OK)
        {
            return GT_FAIL;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclInitLmData
*
* DESCRIPTION:
*       This function does specific initialization of Ingress and Egress PCL UDB 
*       needed for OAM LM counting per flow classification according to: PCL-ID,
*       CFM  Source MAC, Orig VID and Local Dev Src Port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclInitLmData(GT_VOID)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    GT_U8                               offset; /* UDB offset from anchor */
    GT_U32                              udbIndex; /* UDB index */
    GT_U32                              metaUdbIndex; /* metadata UDB index */
    GT_U8                               metaUdbOffset; /* metadata UDB offset */
    CPSS_DXCH_PCL_UDB_SELECT_STC        udbSelect; /* UDB Selection configuration */
    CPSS_PCL_DIRECTION_ENT              direction;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        for(direction = CPSS_PCL_DIRECTION_INGRESS_E;
            direction <= CPSS_PCL_DIRECTION_EGRESS_E; direction++)
        {
            cpssOsMemSet(&udbSelect, '\0', sizeof(CPSS_DXCH_PCL_UDB_SELECT_STC));    
            if(direction == CPSS_PCL_DIRECTION_INGRESS_E)
                ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E;
            else
                ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E;
            /* UDB for per flow CFM PCL rule, configure UDB[2-7]
               UDB[0-1] are replaced by PCL-ID */
            for(udbIndex = 2, offset = 6 ; udbIndex < 8; udbIndex++,offset++)
            {
                udbSelect.udbSelectArr[udbIndex] = udbIndex;    
                /* UDB[2-7] offset for CFM  Source MAC */
                rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                                   ruleFormat, 
                                                   CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, 
                                                   direction,
                                                   udbIndex,
                                                   CPSS_DXCH_PCL_OFFSET_L2_E,
                                                   offset);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
                if(rc != GT_OK)
                {
                    return rc;
                } 
            } 
            /* MetaData anchor type is only applicable to UDBs 30-49.
               map udb[8-10] -> udb[30-34] */       
            metaUdbIndex = 30;
            /* Orig VID, UDB[8-9]*/ 
            udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
            if(direction == CPSS_PCL_DIRECTION_INGRESS_E)
                metaUdbOffset = 27;
            else
                metaUdbOffset = 24;
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               ruleFormat, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, 
                                               direction,
                                               metaUdbIndex,
                                               CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                               metaUdbOffset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            udbIndex++;
            metaUdbIndex++;
            metaUdbOffset++;
            udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
            rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                               ruleFormat, 
                                               CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, 
                                               direction,
                                               metaUdbIndex,
                                               CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                               metaUdbOffset);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* Local Dev Src Port, UDB[10] */
            if(direction == CPSS_PCL_DIRECTION_INGRESS_E)
            {            
                metaUdbIndex++;
                udbIndex++;
                udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
                rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                                   ruleFormat, 
                                                   CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, 
                                                   direction,
                                                   metaUdbIndex,
                                                   CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                                   10);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            else
            {
                metaUdbIndex++;
                udbIndex++;
                udbSelect.udbSelectArr[udbIndex] = metaUdbIndex;
                rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                                   ruleFormat, 
                                                   CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, 
                                                   direction,
                                                   metaUdbIndex,
                                                   CPSS_DXCH_PCL_OFFSET_METADATA_E,
                                                   16);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            /* Set UDB mapping and replace UDB[0-1] with PCL-ID */
            if(direction == CPSS_PCL_DIRECTION_INGRESS_E)
            {
                udbSelect.ingrUdbReplaceArr[0] = GT_TRUE;
                udbSelect.ingrUdbReplaceArr[1] = GT_TRUE;
            }
            else
            {
                udbSelect.egrUdb01Replace = GT_TRUE;
            }                
            rc = cpssDxChPclUserDefinedBytesSelectSet(devNum,
                                                      ruleFormat,
                                                      CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
                                                      CPSS_PCL_LOOKUP_NUMBER_0_E,
                                                      &udbSelect);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }            
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHwInitGen
*
* DESCRIPTION:
*       This function does generic initialization of OAM engine for all OAM packets. 
*       OAM engine is enabled for Ingress, OAM Table Base Flow ID is set,
*       Aging Bitmap Update Mode is set to all, CPU Code Base for all OAM PDU's
*       is set, OAM Hash Bit Selection is set for hash validation and according 
*       to agingDaemonEnable the aging daemon is enabled for ingress and the
*       aging period is defined to 5/6 ms. 
*
* INPUTS:
*       stage               - OAM stage type.
*       agingDaemonEnable   - enable aging daemon.
*       upMep               - Up MEP flow type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       when the aging period is set to 5/6 ms and the aging Threshold is set to 14
*       it is possible to have an exception every 3.5 times for a transmission 
*       period of 3.3 ms,   5/6*14 = 11 2/3 = 3.3*3.5
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHwInitGen
(
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT stage,
    IN GT_BOOL                      agingDaemonEnable,
    IN GT_BOOL                      upMep
)
{
    GT_U8                               devNum; /* device number */
    GT_STATUS                           rc; /* function return code */
    GT_U64                              agingPeriodValue; /* expected time interval */
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
         /* Enable OAM Functionality*/
        rc = cpssDxChOamEnableSet(devNum,
                                  stage,
                                  GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEnableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Set OAM Table Base Flow ID */
        rc = cpssDxChOamTableBaseFlowIdSet(devNum,
                                           stage,
                                           GT_APP_DEMO_OAM_BASEFLOW_ID_CNS);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamTableBaseFlowIdSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Set CPU Code Base for all OAM PDU's */
        rc = cpssDxChOamCpuCodeBaseSet(devNum,
                                       stage,
                                       GT_APP_DEMO_OAM_PCL_CPU_CODE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamCpuCodeBaseSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }   
        
        /* Aging status is updated for fail and for okay */
        rc = cpssDxChOamAgingBitmapUpdateModeSet(devNum,
                                                 stage,
                                                 CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ALL_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamAgingBitmapUpdateModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }   
        /* select which bits will be used for hash detection form the 32bit hash */
        rc = cpssDxChOamHashBitSelectionSet(devNum, stage, 0, 11);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamHashBitSelectionSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* configure aging daemon */
        if(agingDaemonEnable)
        {
            GT_U32  entryIndex;

            if(upMep &&
               stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
            {
                /* configure the largest value possible, 34 bits 
                   ~10 minutes */
                agingPeriodValue.l[0] = 0xFFFFFFFF;
                agingPeriodValue.l[1] = 0x3;
                entryIndex = GT_APP_DEMO_OAM_UP_MEP_AGING_PERIOD_ENTRY_INDEX;
            }
            else
            {            
                agingPeriodValue.l[0] = 20833;/* 82500- the value is in 40 ns units = 3.3 ms 
                                                 20833- 5/6 ms agingPeriod and threshold = 14,
                                                 5/6*14 = 11 2/3 = 3.3*3.5
                                                 in ASIC_SIMULATION this value has no meaning */
                agingPeriodValue.l[1] = 0;
                entryIndex = 0;
            }
            
            rc = cpssDxChOamAgingPeriodEntrySet(devNum,stage,
                                                entryIndex,
                                                agingPeriodValue);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamAgingPeriodEntrySet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }  
            rc = cpssDxChOamAgingDaemonEnableSet(devNum, stage, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamAgingDaemonEnableSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        } 
        /* Currently there isn't an API for this field, this setting is temporary
           Enable keepalive for soft drop packets, Bugs00149516 */
        if(!upMep &&
           stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
        {
            CPSS_TBD_BOOKMARK /* Should use API */
            rc = prvCpssHwPpSetRegField(devNum, 0x1C000020, 13, 1, 1);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssHwPpSetRegField",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHwInitCcm
*
* DESCRIPTION:
*       This function does specific initialization of OAM engine for CCM packets.
*       CCM opcode packet command is set to trap to CPU and statistical rate 
*       limit is set for the CCM CPU code to drop all trapped CCM packets.
*       
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHwInitCcm(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;/* entry information */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           0x1/*CCM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                           2); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 2) == GT_APP_DEMO_OAM_CCM_CPU_CODE */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* configure statistical rate limit - all CCM packets are dropped */   
        cpssOsMemSet(&entryInfo, '\0', sizeof(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC));    
        entryInfo.tc = GT_APP_DEMO_OAM_RX_QUEUE_NUM; /* CCM queue number */
        entryInfo.dp = CPSS_DP_GREEN_E;
        entryInfo.truncate = GT_FALSE;
        entryInfo.cpuRateLimitMode = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
        entryInfo.cpuCodeRateLimiterIndex = 0;
        entryInfo.cpuCodeStatRateLimitIndex = 1; /*index*/      
        entryInfo.designatedDevNumIndex = 0;

        rc = cpssDxChNetIfCpuCodeTableSet(devNum,
                                          GT_APP_DEMO_OAM_CCM_CPU_CODE,
                                          &entryInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(devNum, 1 /*index*/, 0 /* drop*/);

        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHwInitUpMepCcm
*
* DESCRIPTION:
*       This function does specific initialization of OAM engine for CCM Up MEPs.
*       CCM opcode packet command is set to forward in Ingress and to drop 
*       in Egress.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHwInitUpMepCcm(GT_VOID)
{
    GT_STATUS   rc; /* function return code */
    GT_U8       devNum; /* device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           0x1/*CCM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_UP_MEP_CNS,
                                                           CPSS_PACKET_CMD_FORWARD_E,
                                                           0);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                                           0x1/*CCM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_UP_MEP_CNS,
                                                           CPSS_PACKET_CMD_DROP_HARD_E,
                                                           0);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHwInitLm
*
* DESCRIPTION:
*       This function does specific initialization of Ingress and Egress OAM
*       engines for LM packets.
*       LMM/LMR opcode packet command is set to TRAP TO CPU for Ingress and to 
*       FORWARD for Egress.
*       CCM opcode is configured to be counted as part of the LM counting.
*       LM single ended opcodes is configured to LMM and LMR opcodes.
*       The offset for LM counter to be inserted into the packet is configured 
*       appropriately for TX/RX LMM and TX/RX LMR. 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHwInitLm(GT_VOID)
{
    GT_STATUS   rc; /* function return code */
    GT_U8       devNum; /* device number */
    CPSS_DXCH_OAM_STAGE_TYPE_ENT stage; /* OAM stage type */
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;/* entry information */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {        
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           43/*LMM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                           1); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 1) == GT_APP_DEMO_OAM_LM_DM_CPU_CODE*/
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           42/*LMR*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                           0); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 0) == GT_APP_DEMO_OAM_LMR_DMR_CPU_CODE */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           1/*CCM for dual-ended LM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                           0); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 0) == GT_APP_DEMO_OAM_LMR_DMR_CPU_CODE */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamDualEndedLmPacketCommandSet(devNum,
                                                    CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                    CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                    0);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamDualEndedLmPacketCommandSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                                           43/*LMM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_FORWARD_E,
                                                           0); /* not relevant for egress */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                                           42/*LMR*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_FORWARD_E,
                                                           0); /* not relevant for egress */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                                           1/*CCM for dual-ended LM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_FORWARD_E,
                                                           0); /* not relevant for egress */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamDualEndedLmPacketCommandSet(devNum,
                                                    CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                                    CPSS_PACKET_CMD_FORWARD_E,
                                                    0);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamDualEndedLmPacketCommandSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        for(stage = CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E; stage <= CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E; stage++)
        {
            /* count CCM frames */
            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_LM_COUNTED_E,
                                       0,
                                       1/* CCM */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
#if 0
            /* count LMM frames */
            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_LM_COUNTED_E,
                                       1,
                                       43/* LMM */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
			/* count LMR frames */
            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_LM_COUNTED_E,
                                       2,
                                       42/* LMR */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
#endif
            /* configure single ended LM opcode */
            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_LM_SINGLE_ENDED_E,
                                       0/*n*/,/* used in cpssDxChOamLmOffsetTableSet() */
                                       43/* LMM */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* configure single ended LM opcode */
            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_LM_SINGLE_ENDED_E,
                                       1/*n*/,/* used in cpssDxChOamLmOffsetTableSet() */
                                       42/* LMR */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* configure single ended LM opcode */
            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_LM_DUAL_ENDED_E,
                                       0,/* must be 0 for dual ended table */
                                       1/* CCM for dual-ended LM */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* the index to the LM Table is <OAM PTP Offset Index>+n 
           where n is the index from the single ended LM opcode configuration */
        /* offset in bytes from the start of the OAM header to insert
           the Rx counter value in the received LMM frame */
        rc = cpssDxChOamLmOffsetTableSet(devNum,
                                         /* GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + n */
                                         GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + 0,
                                         8); /* byte offset in oam header. start count after oam ethertype. */ 
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamLmOffsetTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* offset to insert for the Rx counter value in the received LMR frame */
        rc = cpssDxChOamLmOffsetTableSet(devNum,
                                         /* GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + n */
                                         GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + 1,
                                         16); /* byte offset in oam header. start count after oam ethertype. */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamLmOffsetTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* offset to insert for the Rx counter value in the transmited dual-ended LM frame */
        rc = cpssDxChOamLmOffsetTableSet(devNum,
                                         GT_APP_DEMO_OAM_INGRESS_PTP_DE_LM_OFFSET_INDEX,
                                         70); /* byte offset in oam header. start count after oam ethertype. */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamLmOffsetTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* <OAM PTP Offset Index> = GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX */
        /* offset to insert for the Tx counter value in the transmited LMM frame */
        rc = cpssDxChOamLmOffsetTableSet(devNum,
                                         /* GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + n */
                                         GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX + 0,
                                         4); /* byte offset in oam header. start count after oam ethertype. */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamLmOffsetTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* offset to insert for the Tx counter value in the transmited LMR frame */
        rc = cpssDxChOamLmOffsetTableSet(devNum,
                                         /* GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + n */
                                         GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX + 1,
                                         12); /* byte offset in oam header. start count after oam ethertype. */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamLmOffsetTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* offset to insert for the Tx counter value in the transmited dual-ended LM frame */
        rc = cpssDxChOamLmOffsetTableSet(devNum,
                                         GT_APP_DEMO_OAM_EGRESS_PTP_DE_LM_OFFSET_INDEX,
                                         58); /* byte offset in oam header. start count after oam ethertype. */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamLmOffsetTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* configure statistical rate limit - all LM packets are trapped */   
        cpssOsMemSet(&entryInfo, '\0', sizeof(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC));    
        entryInfo.tc = GT_APP_DEMO_OAM_RX_QUEUE_NUM_LM_DM; 
        entryInfo.dp = CPSS_DP_GREEN_E;
        entryInfo.truncate = GT_FALSE;
        entryInfo.cpuRateLimitMode = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
        entryInfo.cpuCodeRateLimiterIndex = 0;
        entryInfo.cpuCodeStatRateLimitIndex = 2; /*index*/
        entryInfo.designatedDevNumIndex = 0;

        rc = cpssDxChNetIfCpuCodeTableSet(devNum,
                                          GT_APP_DEMO_OAM_LM_DM_CPU_CODE,
                                          &entryInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        cpssOsMemSet(&entryInfo, '\0', sizeof(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC));    
        entryInfo.tc = GT_APP_DEMO_OAM_RX_QUEUE_NUM_LMR_DMR; 
        entryInfo.dp = CPSS_DP_GREEN_E;
        entryInfo.truncate = GT_FALSE;
        entryInfo.cpuRateLimitMode = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
        entryInfo.cpuCodeRateLimiterIndex = 0;
        entryInfo.cpuCodeStatRateLimitIndex = 2; /*index*/ 
        entryInfo.designatedDevNumIndex = 0;

        rc = cpssDxChNetIfCpuCodeTableSet(devNum,
                                          GT_APP_DEMO_OAM_LMR_DMR_CPU_CODE,
                                          &entryInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(devNum, 2 /*index*/, 0xFFFFFFFF /* forward*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHwInitDm
*
* DESCRIPTION:
*       This function does specific initialization of Ingress and Egress OAM
*       engines for DM packets.
*       DMM/DMR/1DM opcode packet command is set to TRAP TO CPU for Ingress and to 
*       FORWARD for Egress.
*       The offset for DM counter to be inserted into the packet is configured 
*       appropriately for TX/RX DMM and TX/RX DMR. 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHwInitDm(GT_VOID)
{
    GT_STATUS   rc; /* function return code */
    GT_U8       devNum; /* device number */
    CPSS_DXCH_OAM_STAGE_TYPE_ENT stage; /* OAM stage type */
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;/* entry information */
    CPSS_DXCH_PTP_TS_CFG_ENTRY_STC entry;

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {        


        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           47/*DMM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                           1); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 1) == GT_APP_DEMO_OAM_LM_DM_CPU_CODE */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           46/*DMR*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                           0); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 0) == GT_APP_DEMO_OAM_LMR_DMR_CPU_CODE */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }


        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           45/*1DM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                           0); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 0) == GT_APP_DEMO_OAM_LMR_DMR_CPU_CODE */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                                           47/*DMM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_FORWARD_E,
                                                           0); /*not relevant for egress*/
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                                           46/*DMR*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_FORWARD_E,
                                                           0); /*not relevant for egress*/
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                                           45/*1DM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_FORWARD_E,
                                                           0); /*not relevant for egress*/
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        for(stage = CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E; stage <= CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E; stage++)
        {
            /* configure single ended DM opcode */
            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_DM_E,
                                       0/*n*/,
                                       47/* DMM */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            
            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_DM_E,
                                       1/*n*/,
                                       46/* DMR */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            rc =  cpssDxChOamOpcodeSet(devNum,
                                       stage,
                                       CPSS_DXCH_OAM_OPCODE_TYPE_DM_E,
                                       2/*n*/,
                                       45/* 1DM */);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeSet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }

        }

        /* the index to the PTP Table is <OAM PTP Offset Index>+n 
           where n is the index from the cpssDxChOamOpcodeSet configuration */
        cpssOsMemSet(&entry, '\0', sizeof(CPSS_DXCH_PTP_TS_CFG_ENTRY_STC));
        /* offset in bytes from the start of the OAM header to insert
           the Rx counter value in the received DMM frame */
        entry.tsMode = CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E;
        entry.tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E;
        entry.packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
        entry.ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E;
        entry.offset = 12; /* byte offset in oam header. start count after oam ethertype. */
        rc = cpssDxChPtpTsCfgTableSet(devNum, 
                                      /* GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + n */
                                      GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + 0,
                                      &entry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPtpTsCfgTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* offset in bytes from the start of the OAM header to insert
           the Rx counter value in the received DMR frame */
        entry.tsMode = CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E;
        entry.tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E;
        entry.packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
        entry.ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E;
        entry.offset = 28; /* byte offset in oam header. start count after oam ethertype. */
        rc = cpssDxChPtpTsCfgTableSet(devNum, 
                                      /* GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + n */
                                      GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + 1,
                                      &entry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPtpTsCfgTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* offset in bytes from the start of the OAM header to insert
           the Rx counter value in the received 1DM frame */
        entry.tsMode = CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E;
        entry.tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E;
        entry.packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
        entry.ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E;
        entry.offset = 12; /* byte offset in oam header. start count after oam ethertype. */
        rc = cpssDxChPtpTsCfgTableSet(devNum,
                                      /* GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + n */ 
                                      GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX + 2,
                                      &entry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPtpTsCfgTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* <OAM PTP Offset Index> = GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX */
        /* offset to insert for the Tx counter value in the transmit DMM frame */
        entry.tsMode = CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E;
        entry.tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E;
        entry.packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
        entry.ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E;
        entry.offset = 4; /* byte offset in oam header. start count after oam ethertype. */
        rc = cpssDxChPtpTsCfgTableSet(devNum,
                                      /* GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX + n */ 
                                      GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX + 0,
                                      &entry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPtpTsCfgTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* offset to insert for the Tx counter value in the transmit DMR frame */
        entry.tsMode = CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E;
        entry.tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E;
        entry.packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
        entry.ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E;
        entry.offset = 20; /* byte offset in oam header. start count after oam ethertype. */
        rc = cpssDxChPtpTsCfgTableSet(devNum,
                                      /* GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX + n */ 
                                      GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX + 1,
                                      &entry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPtpTsCfgTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* offset to insert for the Tx counter value in the transmit 1DM frame */
        entry.tsMode = CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E;
        entry.tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E;
        entry.packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
        entry.ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E;
        entry.offset = 4; /* byte offset in oam header. start count after oam ethertype. */
        rc = cpssDxChPtpTsCfgTableSet(devNum, 
                                      /* GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX + n */
                                      GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX + 2,
                                      &entry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPtpTsCfgTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* configure statistical rate limit - all DM packets are trapped */   
        cpssOsMemSet(&entryInfo, '\0', sizeof(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC));    
        entryInfo.tc = GT_APP_DEMO_OAM_RX_QUEUE_NUM_LM_DM; 
        entryInfo.dp = CPSS_DP_GREEN_E;
        entryInfo.truncate = GT_FALSE;
        entryInfo.cpuRateLimitMode = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
        entryInfo.cpuCodeRateLimiterIndex = 0;
        entryInfo.cpuCodeStatRateLimitIndex = 2; /*index*/ 
        entryInfo.designatedDevNumIndex = 0;

        rc = cpssDxChNetIfCpuCodeTableSet(devNum,
                                          GT_APP_DEMO_OAM_LM_DM_CPU_CODE,
                                          &entryInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        
        cpssOsMemSet(&entryInfo, '\0', sizeof(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC));    
        entryInfo.tc = GT_APP_DEMO_OAM_RX_QUEUE_NUM_LMR_DMR; 
        entryInfo.dp = CPSS_DP_GREEN_E;
        entryInfo.truncate = GT_FALSE;
        entryInfo.cpuRateLimitMode = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
        entryInfo.cpuCodeRateLimiterIndex = 0;
        entryInfo.cpuCodeStatRateLimitIndex = 2; /*index*/ 
        entryInfo.designatedDevNumIndex = 0;

        rc = cpssDxChNetIfCpuCodeTableSet(devNum,
                                          GT_APP_DEMO_OAM_LMR_DMR_CPU_CODE,
                                          &entryInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(devNum, 2 /*index*/, 0xFFFFFFFF /* forward*/);

        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHwInitLb
*
* DESCRIPTION:
*       This function does specific initialization of OAM engine for CCM packets.
*       CCM opcode packet command is set to trap to CPU and statistical rate 
*       limit is set for the CCM CPU code to drop all trapped CCM packets.
*       
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHwInitLb(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;/* entry information */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           0x2/*LBR*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
                                                           3); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 3) == GT_APP_DEMO_OAM_LBR_CPU_CODE */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,
                                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                                           0x3/*LBM*/,
                                                           GT_APP_DEMO_OAM_PROFILE_1731_CNS,
                                                           CPSS_PACKET_CMD_LOOPBACK_E,
                                                           0); /* (GT_APP_DEMO_OAM_PCL_CPU_CODE + 0) == GT_APP_DEMO_OAM_LBR_CPU_CODE */
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamOpcodeProfilePacketCommandEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* configure statistical rate limit - all LB packets are trapped */   
        cpssOsMemSet(&entryInfo, '\0', sizeof(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC));    
        entryInfo.tc = GT_APP_DEMO_OAM_RX_QUEUE_NUM; /* service CPU queue number */
        entryInfo.dp = CPSS_DP_GREEN_E;
        entryInfo.truncate = GT_FALSE;
        entryInfo.cpuRateLimitMode = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
        entryInfo.cpuCodeRateLimiterIndex = 0;
        entryInfo.cpuCodeStatRateLimitIndex = 0; /*index*/      
        entryInfo.designatedDevNumIndex = 0;

        rc = cpssDxChNetIfCpuCodeTableSet(devNum,
                                          GT_APP_DEMO_OAM_LBR_CPU_CODE,
                                          &entryInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(devNum,
                                                               0 /*index*/,
                                                               0xFFFFFFFF /* forward*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamExceptionConfigInit
*
* DESCRIPTION:
*       This function does generic initialization of OAM Exceptions.
*       all exceptions are set to trap packet to CPU.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamExceptionConfigInit(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC      exceptionConfig; /* OAM exception configuration */

    exceptionConfig.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    exceptionConfig.cpuCode = GT_APP_DEMO_OAM_PCL_CPU_CODE;
    exceptionConfig.summaryBitEnable = GT_TRUE;
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamExceptionConfigSet(devNum,
                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                           CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E, 
                                           &exceptionConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionConfigSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamExceptionConfigSet(devNum,
                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                           CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E, 
                                           &exceptionConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionConfigSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamExceptionConfigSet(devNum,
                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                           CPSS_DXCH_OAM_EXCEPTION_TYPE_TX_PERIOD_E, 
                                           &exceptionConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionConfigSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamExceptionConfigSet(devNum,
                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                           CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E, 
                                           &exceptionConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionConfigSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChOamExceptionConfigSet(devNum,
                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                           CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E, 
                                           &exceptionConfig);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionConfigSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamEntrySetCcm
*
* DESCRIPTION:
*       Set OAM Entry configuration for CCM flows.
*       The functions sets keepalive processing 
*       meg Level Check, period Check, rdi Check, source Interface Check 
*       and opcode Parsing according to enableEntry.
*       aging Threshold is set to 14 (See comment).
* 
* INPUTS:
*       flowId          - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                         0 is reserverd.
*       enableEntry     - enable or disable keepalive processing
*       megLevel        - Determines the MEG level of the current MEP
*                         (APPLICABLE RANGES: 0..7)
*       portNum         - The Source port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       when the aging period is set to 5/6 ms and the aging Threshold is set to 14
*       it is possible to have an exception every 3.5 times for a transmission 
*       period of 3.3 ms,   5/6*14 = 11 2/3 = 3.3*3.5
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEntrySetCcm
(
    IN GT_U32 flowId, 
    IN GT_BOOL enableEntry,
    IN GT_U32 megLevel, 
    IN GT_PORT_NUM portNum
)
{
    GT_STATUS               rc; /* function return code */
    GT_U8                   devNum; /* device number */
    CPSS_DXCH_OAM_ENTRY_STC oamTableEntry; /* OAM entry configuration */
    GT_HW_DEV_NUM           hwDevNum; /* HW device number */

    cpssOsMemSet(&oamTableEntry, '\0', sizeof(CPSS_DXCH_OAM_ENTRY_STC));
    oamTableEntry.keepaliveAgingEnable = enableEntry;    
#ifdef ASIC_SIMULATION
    oamTableEntry.agingThreshold = 1;
#else
    oamTableEntry.agingThreshold = 3;
#endif
    oamTableEntry.agingPeriodIndex = 0;
    oamTableEntry.ageState = 0;    
    oamTableEntry.megLevelCheckEnable = enableEntry;
    oamTableEntry.megLevel = megLevel;
    oamTableEntry.periodCheckEnable = enableEntry;
    oamTableEntry.keepaliveTxPeriod = 1;
    oamTableEntry.rdiCheckEnable = enableEntry;
    oamTableEntry.rdiStatus = 0;
    oamTableEntry.excessKeepaliveDetectionEnable = GT_FALSE;
    oamTableEntry.hashVerifyEnable = enableEntry;
    oamTableEntry.flowHash = 0x00;
    oamTableEntry.lockHashValueEnable = GT_FALSE;
    oamTableEntry.protectionLocUpdateEnable = GT_FALSE;
    oamTableEntry.sourceInterface.type = CPSS_INTERFACE_PORT_E;
    oamTableEntry.sourceInterface.devPort.portNum = portNum;
    oamTableEntry.sourceInterfaceCheckEnable = enableEntry;
    oamTableEntry.sourceInterfaceCheckMode = CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E;
    oamTableEntry.opcodeParsingEnable = enableEntry;
    oamTableEntry.packetCommandProfile = GT_APP_DEMO_OAM_PROFILE_1731_CNS;
    oamTableEntry.lmCountingMode = CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E;

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChCfgHwDevNumGet(devNum,&hwDevNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        oamTableEntry.sourceInterface.devPort.hwDevNum = hwDevNum;
        /* The entry index in the PP is calculated from the descriptor flow id - the OAM base flow id */
        rc = cpssDxChOamEntrySet(devNum, CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                 flowId,&oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamEntrySetUpMepCcm
*
* DESCRIPTION:
*       Set OAM Entry configuration for Up MEP CCM flows.
*       The functions sets keepalive processing for Ingress and Egress both.
*       meg Level Check, period Check, rdi Check, source Interface Check 
*       and opcode Parsing according to enableEntry and stage.
*       aging Threshold is set to 14 (Egress) or 15 (Ingress) (See comment).
* 
* INPUTS:
*       flowId          - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                         0 is reserverd.
*       stage           - OAM stage type.
*       enableEntry     - enable or disable keepalive processing
*       megLevel        - Determines the MEG level of the current MEP
*                         Only relevant for Ingress.
*                         (APPLICABLE RANGES: 0..7)
*       portNum         - The Source port number
*                         Only relevant for Ingress.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       when the aging period is set to 5/6 ms and the aging Threshold is set to 
*       14 it is possible to have an exception every 3.5 times for a transmission 
*       period of 3.3 ms,   5/6*14 = 11 2/3 = 3.3*3.5
*       This is used for Egress, for Ingress we want the largest possible 
*       threshold which is 15.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEntrySetUpMepCcm
(
    IN GT_U32                       flowId, 
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT stage,
    IN GT_BOOL                      enableEntry,
    IN GT_U32                       megLevel, 
    IN GT_PORT_NUM                  portNum
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_OAM_ENTRY_STC            oamTableEntry; /* OAM entry configuration */

    cpssOsMemSet(&oamTableEntry, '\0', sizeof(CPSS_DXCH_OAM_ENTRY_STC));   
    switch(stage)
    {    
        case CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E:
        {
            oamTableEntry.agingThreshold = 15;
            oamTableEntry.agingPeriodIndex = GT_APP_DEMO_OAM_UP_MEP_AGING_PERIOD_ENTRY_INDEX;
            oamTableEntry.sourceInterface.type = CPSS_INTERFACE_PORT_E;
            oamTableEntry.sourceInterface.devPort.portNum = portNum;
            oamTableEntry.sourceInterfaceCheckEnable = enableEntry;
            oamTableEntry.sourceInterfaceCheckMode = CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E;
            oamTableEntry.megLevelCheckEnable = enableEntry;
            oamTableEntry.megLevel = megLevel;
            oamTableEntry.periodCheckEnable = enableEntry;
            oamTableEntry.keepaliveTxPeriod = 1;
            oamTableEntry.rdiCheckEnable = enableEntry;
            oamTableEntry.rdiStatus = 0;
            oamTableEntry.hashVerifyEnable = enableEntry;
            oamTableEntry.flowHash = 0x00;
            break;
        }
        case CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E:
        {
#ifdef ASIC_SIMULATION
            oamTableEntry.agingThreshold = 1;
#else
            oamTableEntry.agingThreshold = 14;
#endif        
            oamTableEntry.agingPeriodIndex = 0;
            break;
        }
        default:
            return GT_BAD_PARAM;
    }
    oamTableEntry.keepaliveAgingEnable = enableEntry; 
    oamTableEntry.ageState = 0;   
    oamTableEntry.excessKeepaliveDetectionEnable = GT_FALSE;
    oamTableEntry.lockHashValueEnable = GT_FALSE;
    oamTableEntry.protectionLocUpdateEnable = GT_FALSE; 
    oamTableEntry.opcodeParsingEnable = enableEntry;
    oamTableEntry.packetCommandProfile = GT_APP_DEMO_OAM_PROFILE_UP_MEP_CNS;

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        oamTableEntry.sourceInterface.devPort.hwDevNum = devNum;
        /* The entry index in the PP is calculated from the descriptor
           flow id - the OAM base flow id */
        rc = cpssDxChOamEntrySet(devNum, stage, flowId, &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamEntrySetLm
*
* DESCRIPTION:
*       Set OAM Entry configuration for LM flows.
*       The functions updates the entry with meg Level Check, source Interface
*       Check, opcode Parsing, LM counting enable and LM capture enable.
* 
* INPUTS:
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       portNum             - The Source port number
*       stage               - OAM stage type.
*       oamPtpOffsetIndex   - An index to the OAM Offset Table.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEntrySetLm
(
    IN GT_U32                       flowId, 
    IN GT_U32                       megLevel, 
    IN GT_PORT_NUM                  portNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT stage,
    IN GT_U32                       oamPtpOffsetIndex
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_OAM_ENTRY_STC            oamTableEntry; /* OAM entry configuration */

    cpssOsMemSet(&oamTableEntry, '\0', sizeof(CPSS_DXCH_OAM_ENTRY_STC));

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamEntryGet(devNum,
                                 stage,
                                 flowId,
                                 &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntryGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        oamTableEntry.megLevelCheckEnable = GT_TRUE;
        oamTableEntry.megLevel = megLevel;
        if(stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
        {
            oamTableEntry.sourceInterface.type = CPSS_INTERFACE_PORT_E;
            oamTableEntry.sourceInterface.devPort.portNum = portNum; /* eport */
            oamTableEntry.sourceInterface.devPort.hwDevNum = 0; /* not used */
            oamTableEntry.sourceInterfaceCheckMode = CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E;
            oamTableEntry.sourceInterfaceCheckEnable = GT_TRUE;		
        }
        else
        {
            oamTableEntry.sourceInterfaceCheckEnable = GT_FALSE;
        }        
        oamTableEntry.opcodeParsingEnable = GT_TRUE;
        oamTableEntry.cpuCodeOffset = 0;
        oamTableEntry.packetCommandProfile = GT_APP_DEMO_OAM_PROFILE_1731_CNS;
        oamTableEntry.lmCounterCaptureEnable = GT_TRUE;
        oamTableEntry.lmCountingMode = CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E;
        oamTableEntry.oamPtpOffsetIndex = oamPtpOffsetIndex;
        oamTableEntry.dualEndedLmEnable = GT_FALSE;
        /* The entry index in the PP is calculated from the descriptor flow id - the OAM base flow id */
        rc = cpssDxChOamEntrySet(devNum,
                                 stage,
                                 flowId,
                                 &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

GT_STATUS gtAppDemoOamEntrySetDualEndedLm
(
    IN GT_U32                       flowId, 
    IN GT_U32                       megLevel, 
    IN GT_PORT_NUM                  portNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT stage,
    IN GT_U32                       oamPtpOffsetIndex
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_OAM_ENTRY_STC            oamTableEntry; /* OAM entry configuration */

    cpssOsMemSet(&oamTableEntry, '\0', sizeof(CPSS_DXCH_OAM_ENTRY_STC));

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamEntryGet(devNum,
                                 stage,
                                 flowId,
                                 &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntryGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        oamTableEntry.megLevelCheckEnable = GT_TRUE;
        oamTableEntry.megLevel = megLevel;
        if(stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
        {
            oamTableEntry.sourceInterface.type = CPSS_INTERFACE_PORT_E;
            oamTableEntry.sourceInterface.devPort.portNum = portNum; /* eport */
            oamTableEntry.sourceInterface.devPort.hwDevNum = 0; /* not used */
            oamTableEntry.sourceInterfaceCheckMode = CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E;
            oamTableEntry.sourceInterfaceCheckEnable = GT_TRUE;		
        }
        else
        {
            oamTableEntry.sourceInterfaceCheckEnable = GT_FALSE;
        }        
        oamTableEntry.opcodeParsingEnable = GT_TRUE;
        oamTableEntry.cpuCodeOffset = 0;
        oamTableEntry.packetCommandProfile = GT_APP_DEMO_OAM_PROFILE_1731_CNS;
        oamTableEntry.lmCounterCaptureEnable = GT_TRUE;
        oamTableEntry.lmCountingMode = CPSS_DXCH_OAM_LM_COUNTING_MODE_ENABLE_E;
        oamTableEntry.oamPtpOffsetIndex = oamPtpOffsetIndex;
        oamTableEntry.dualEndedLmEnable = GT_TRUE;
        /* The entry index in the PP is calculated from the descriptor flow id - the OAM base flow id */
        rc = cpssDxChOamEntrySet(devNum,
                                 stage,
                                 flowId,
                                 &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamEntrySetDm
*
* DESCRIPTION:
*       Set OAM Entry configuration for DM flows.
*       The functions updates the entry with meg Level Check, source Interface
*       Check, opcode Parsing and DM capture enable.
* 
* INPUTS:
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       portNum             - The Source port number
*       stage               - OAM stage type.
*       oamPtpOffsetIndex   - An index to the OAM Offset Table.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEntrySetDm
(
    IN GT_U32                       flowId, 
    IN GT_U32                       megLevel, 
    IN GT_PORT_NUM                  portNum,
    IN CPSS_DXCH_OAM_STAGE_TYPE_ENT stage,
    IN GT_U32                       oamPtpOffsetIndex
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_OAM_ENTRY_STC            oamTableEntry; /* OAM entry configuration */

    cpssOsMemSet(&oamTableEntry, '\0', sizeof(CPSS_DXCH_OAM_ENTRY_STC));

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamEntryGet(devNum,
                                 stage,
                                 flowId,
                                 &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntryGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        oamTableEntry.megLevelCheckEnable = GT_TRUE;
        oamTableEntry.megLevel = megLevel;
        if(stage == CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
        {
            oamTableEntry.sourceInterface.type = CPSS_INTERFACE_PORT_E;
            oamTableEntry.sourceInterface.devPort.portNum = portNum; /* eport */
            oamTableEntry.sourceInterface.devPort.hwDevNum = 0; /* not used */
            oamTableEntry.sourceInterfaceCheckMode = CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E;
            oamTableEntry.sourceInterfaceCheckEnable = GT_TRUE;		
        }
        else
        {
            oamTableEntry.sourceInterfaceCheckEnable = GT_FALSE;
        }        
        oamTableEntry.opcodeParsingEnable = GT_TRUE;
        oamTableEntry.cpuCodeOffset = 0;
        oamTableEntry.packetCommandProfile = GT_APP_DEMO_OAM_PROFILE_1731_CNS;
        oamTableEntry.oamPtpOffsetIndex = oamPtpOffsetIndex;
        oamTableEntry.timestampEnable = GT_TRUE;
        /* The entry index in the PP is calculated from the descriptor flow id - the OAM base flow id */
        rc = cpssDxChOamEntrySet(devNum,
                                 stage,
                                 flowId,
                                 &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamEntrySetLb
*
* DESCRIPTION:
*       Set OAM Entry configuration for LM flows.
*       The functions updates the entry with meg Level Check, source Interface
*       Check, opcode Parsing, LM counting enable and LM capture enable.
* 
* INPUTS:
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       portNum             - The Source port number
*       stage               - OAM stage type.
*       oamPtpOffsetIndex   - An index to the OAM Offset Table.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEntrySetLb
(
    IN GT_U32                       flowId, 
    IN GT_U32                       megLevel, 
    IN GT_PORT_NUM                  portNum
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_OAM_ENTRY_STC            oamTableEntry; /* OAM entry configuration */

    cpssOsMemSet(&oamTableEntry, '\0', sizeof(CPSS_DXCH_OAM_ENTRY_STC));

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamEntryGet(devNum,
                                 CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                 flowId,
                                 &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntryGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        oamTableEntry.megLevelCheckEnable = GT_TRUE;
        oamTableEntry.megLevel = megLevel;
        oamTableEntry.sourceInterface.type = CPSS_INTERFACE_PORT_E;
        oamTableEntry.sourceInterface.devPort.portNum = portNum;
        oamTableEntry.sourceInterface.devPort.hwDevNum = devNum;
        oamTableEntry.sourceInterfaceCheckMode = CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E;
        oamTableEntry.sourceInterfaceCheckEnable = GT_TRUE;
        oamTableEntry.opcodeParsingEnable = GT_TRUE;
        oamTableEntry.packetCommandProfile = GT_APP_DEMO_OAM_PROFILE_1731_CNS;

        /* The entry index in the PP is calculated from the descriptor flow id - the OAM base flow id */
        rc = cpssDxChOamEntrySet(devNum,
                                 CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                 flowId,
                                 &oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamEntrySetBfd
*
* DESCRIPTION:
*       Set OAM Entry configuration for BFD flows.
*       The functions sets keepalive processing according to enable.
*       meg Level Check, period Check, rdi Check, source Interface Check 
*       and opcode Parsing are disabled.
*       aging Threshold is set to 14 (See comment).
*
* 
* INPUTS:
*       flowId      - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                     0 is reserverd.
*       enableEntry      - enable or disable keepalive processing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       when the aging period is set to 5/6 ms and the aging Threshold is set to 14
*       it is possible to have an exception every 3.5 times for a transmission 
*       period of 3.3 ms,   5/6*14 = 11 2/3 = 3.3*3.5
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEntrySetBfd
(
    IN GT_U32 flowId,
    IN GT_BOOL enableEntry
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_OAM_ENTRY_STC            oamTableEntry; /* OAM entry configuration */

    if(flowId < 1)
        return GT_OUT_OF_RANGE;
    cpssOsMemSet(&oamTableEntry, '\0', sizeof(CPSS_DXCH_OAM_ENTRY_STC));

    oamTableEntry.keepaliveAgingEnable = enableEntry;  
#ifdef ASIC_SIMULATION
    oamTableEntry.agingThreshold = 1;
#else
    oamTableEntry.agingThreshold = 14;
#endif
    oamTableEntry.agingPeriodIndex = 0;
    oamTableEntry.megLevelCheckEnable = GT_FALSE;
    oamTableEntry.periodCheckEnable = GT_FALSE;
    oamTableEntry.rdiCheckEnable = GT_FALSE;
    oamTableEntry.hashVerifyEnable = enableEntry;
    oamTableEntry.flowHash = 0xFF;
    oamTableEntry.lockHashValueEnable = GT_FALSE;
    oamTableEntry.protectionLocUpdateEnable = GT_TRUE;
    oamTableEntry.sourceInterfaceCheckEnable = GT_FALSE;
    oamTableEntry.sourceInterfaceCheckMode = CPSS_DXCH_OAM_SOURCE_INTERFACE_CHECK_MODE_MATCH_E;
    oamTableEntry.opcodeParsingEnable = GT_FALSE;
    oamTableEntry.packetCommandProfile = GT_APP_DEMO_OAM_PROFILE_1731_CNS;  

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        /* The entry index in the PP is calculated from the descriptor flow id - the OAM base flow id */
        rc = cpssDxChOamEntrySet(devNum, CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                flowId,&oamTableEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclRuleCfmFlow
*
* DESCRIPTION:
*       Set OAM CFM (L2) per flow Ingress PCL rule.
*       The functions sets a 20B UDB rule format with the following key:
*       PCL-ID                  = 0x0, UDB[0-1]
*       Source MAC              = 00:00:00:00:00:01, UDB[8-13]
*       VLAN ID                 = flowId,  UDB[14-14]
*       Source Port             = portNum, UDB[16]
*       Ethertype               = 0x8902,  UDB[17-18]
*
*       The action sets OAM processing and flow ID.
* 
* INPUTS:
*       direction           - PCL direction.
*       ruleIndex           - index of the rule in the TCAM.
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       srcPortNum          - The interface source port. relevant only 
*                             for Ingress.
*                             for Egress it is CPU port.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclRuleCfmFlow
(
    IN GT_U32                   ruleIndex,
    IN GT_U32                   flowId, 
    IN GT_U8                    srcPortNum,
    IN GT_U8                    srcMacAddr,
    IN GT_U32                   vlanId,
    IN GT_BOOL                  drop
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat; 
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */

    if (flowId < 1) 
        return GT_OUT_OF_RANGE;

    /* set new rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E;
    /* Set Mask */
    mask.ruleIngrUdbOnly.replacedFld.pclId = 0x3FF;
    /* UDB[2-7] are counfigured for destination MAC */
    mask.ruleIngrUdbOnly.udb[2] = 0x00; 
    mask.ruleIngrUdbOnly.udb[3] = 0x00;
    mask.ruleIngrUdbOnly.udb[4] = 0x00;
    mask.ruleIngrUdbOnly.udb[5] = 0x00;
    mask.ruleIngrUdbOnly.udb[6] = 0x00;
    mask.ruleIngrUdbOnly.udb[7] = 0x00;
    mask.ruleIngrUdbOnly.udb[8] = 0xFF;/* Source MAC */ 
    mask.ruleIngrUdbOnly.udb[9] = 0xFF;  
    mask.ruleIngrUdbOnly.udb[10] = 0xFF; 
    mask.ruleIngrUdbOnly.udb[11] = 0xFF; 
    mask.ruleIngrUdbOnly.udb[12] = 0xFF;
    mask.ruleIngrUdbOnly.udb[13] = 0xFF;
    mask.ruleIngrUdbOnly.udb[14] = 0xFF;/* VLAN ID */
    mask.ruleIngrUdbOnly.udb[15] = 0x0F;
    if(srcPortNum == 255) 
        mask.ruleIngrUdbOnly.udb[16] = 0x00;/* Source port */
    else
        mask.ruleIngrUdbOnly.udb[16] = 0xFF;/* Source port */
    mask.ruleIngrUdbOnly.udb[17] = 0xFF;/* Ethertype */
    mask.ruleIngrUdbOnly.udb[18] = 0xFF;
    /* Set Pattern */
    pattern.ruleIngrUdbOnly.replacedFld.pclId = 0x000;
    pattern.ruleIngrUdbOnly.udb[8] = 0x00; /* Source MAC */
    pattern.ruleIngrUdbOnly.udb[9] = 0x00;
    pattern.ruleIngrUdbOnly.udb[10] = 0x00;
    pattern.ruleIngrUdbOnly.udb[11] = 0x00;
    pattern.ruleIngrUdbOnly.udb[12] = 0x00;
    pattern.ruleIngrUdbOnly.udb[13]  = srcMacAddr; /* Source MAC */
    pattern.ruleIngrUdbOnly.udb[14]  = (GT_U8)vlanId; /* VLAN ID */
    pattern.ruleIngrUdbOnly.udb[15]  = (GT_U8)((vlanId >> 8)& 0x0F);
    pattern.ruleIngrUdbOnly.udb[16] = srcPortNum; /* Source port*/
    pattern.ruleIngrUdbOnly.udb[17] = 0x02; /* Ethertype */
    pattern.ruleIngrUdbOnly.udb[18] = 0x89;
    /* Set Action */                            
    action.oam.oamProcessEnable = GT_TRUE;
    action.flowId = flowId;
	if(drop)
	{
    	action.pktCmd			= CPSS_PACKET_CMD_DROP_HARD_E;
		action.mirror.cpuCode	= CPSS_NET_FIRST_USER_DEFINED_E; /* must for non-forward*/
	}
	else
    	action.pktCmd = CPSS_PACKET_CMD_FORWARD_E;

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclRuleUpMepCfmFlow
*
* DESCRIPTION:
*       Set OAM CFM (L2) per Up MEP flow Ingress and Egress PCL rule.
*       The functions sets a 20B UDB rule format with the following key:
*       PCL-ID                  = 0x0, UDB[0-1]
*       Destination MAC         = 00:00:00:00:00:01, UDB[2-7]
*       Source MAC              = 00:00:00:00:00:01, UDB[8-13]
*       VLAN ID                 = flowId,  UDB[14-14]
*       Source Port             = portNum, UDB[16]
*       Ethertype               = 0x8902,  UDB[17-18]
*
*       The action sets OAM processing and flow ID.
* 
* INPUTS:
*       direction           - PCL direction.
*       ruleIndex           - index of the rule in the TCAM.
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       srcPortNum          - The interface source port. relevant only 
*                             for Ingress.
*                             for Egress it is CPU port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclRuleUpMepCfmFlow
(
    IN GT_U32                   ruleIndex,
    IN GT_U32                   flowId, 
    IN GT_U8                    srcPortNum
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat; 
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */

    if (flowId < 1) 
        return GT_OUT_OF_RANGE;

    /* set new rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E;
    /* Set Mask */
    mask.ruleIngrUdbOnly.replacedFld.pclId = 0x3FF;
    mask.ruleIngrUdbOnly.udb[2] = 0xFF; /* destination MAC */
    mask.ruleIngrUdbOnly.udb[3] = 0xFF;
    mask.ruleIngrUdbOnly.udb[4] = 0xFF;
    mask.ruleIngrUdbOnly.udb[5] = 0xFF;
    mask.ruleIngrUdbOnly.udb[6] = 0xFF;
    mask.ruleIngrUdbOnly.udb[7] = 0xFF;
    mask.ruleIngrUdbOnly.udb[8] = 0xFF;/* Source MAC */ 
    mask.ruleIngrUdbOnly.udb[9] = 0xFF;  
    mask.ruleIngrUdbOnly.udb[10] = 0xFF; 
    mask.ruleIngrUdbOnly.udb[11] = 0xFF; 
    mask.ruleIngrUdbOnly.udb[12] = 0xFF;
    mask.ruleIngrUdbOnly.udb[13] = 0xFF;
    mask.ruleIngrUdbOnly.udb[14] = 0xFF;/* VLAN ID */
    mask.ruleIngrUdbOnly.udb[15] = 0x0F;
    mask.ruleIngrUdbOnly.udb[16] = 0xFF;/* Source port */
    mask.ruleIngrUdbOnly.udb[17] = 0xFF;/* Ethertype */
    mask.ruleIngrUdbOnly.udb[18] = 0xFF;
    /* Set Pattern */
    pattern.ruleIngrUdbOnly.replacedFld.pclId = 0x000;
    pattern.ruleIngrUdbOnly.udb[2] = 0x00; /* destination MAC */
    pattern.ruleIngrUdbOnly.udb[3] = 0x00;
    pattern.ruleIngrUdbOnly.udb[4] = 0x00;
    pattern.ruleIngrUdbOnly.udb[5] = 0x00;
    pattern.ruleIngrUdbOnly.udb[6] = 0x00;
    pattern.ruleIngrUdbOnly.udb[7] = 0x02;
    pattern.ruleIngrUdbOnly.udb[8] = 0x00; /* Source MAC */
    pattern.ruleIngrUdbOnly.udb[9] = 0x00;
    pattern.ruleIngrUdbOnly.udb[10] = 0x00;
    pattern.ruleIngrUdbOnly.udb[11] = 0x00;
    pattern.ruleIngrUdbOnly.udb[12] = 0x00;
    pattern.ruleIngrUdbOnly.udb[13]  = 0x01; /* Source MAC */
    pattern.ruleIngrUdbOnly.udb[14]  = (GT_U8)flowId; /* VLAN ID */
    pattern.ruleIngrUdbOnly.udb[15]  = (GT_U8)((flowId >> 8)& 0x0F);
    pattern.ruleIngrUdbOnly.udb[16] = srcPortNum; /* Source port*/
    pattern.ruleIngrUdbOnly.udb[17] = 0x02; /* Ethertype */
    pattern.ruleIngrUdbOnly.udb[18] = 0x89;

    /* Set Action */                            
    action.oam.oamProcessEnable = GT_TRUE;
    action.flowId = flowId;

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* Set the same rule for Egress stage */
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E;
    cpssOsMemCpy(mask.ruleEgrUdbOnly.udb, mask.ruleIngrUdbOnly.udb, 19); 
    cpssOsMemCpy(pattern.ruleEgrUdbOnly.udb, pattern.ruleIngrUdbOnly.udb, 19); 
    mask.ruleEgrUdbOnly.udb[14] = 0xF0; /* VLAN ID */
    mask.ruleEgrUdbOnly.udb[15] = 0xFF;
    pattern.ruleEgrUdbOnly.udb[14] = (GT_U8)((flowId << 4) & 0xF0); /* VLAN ID */
    pattern.ruleEgrUdbOnly.udb[15] = (GT_U8)((flowId >> 4)& 0xFF);
    pattern.ruleEgrUdbOnly.replacedFld.pclId = 0x003;
    ruleIndex += GT_APP_DEMO_OAM_UP_MEP_PCL_EGRESS_INDEX_BASE;
    action.egressPolicy = GT_TRUE;
    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamEPclRuleCfmFlow
*
* DESCRIPTION:
*       Set OAM CFM (L2) per flow Egress PCL rule.
*       The functions sets a 20B UDB rule format with the following key:
*       PCL-ID                  = 0x0, UDB[0-1]
*       Destination MAC         = 00:00:00:00:00:01, UDB[2-7]
*       Source MAC              = 00:00:00:00:00:02, UDB[8-13]
*       VLAN ID                 = flowId, UDB[14-15]
*       Source Port             = 63,     UDB[16] 
*       Ethertype               = 0x8902, UDB[17-18]

*
*       The action sets OAM processing and flow ID.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       dstMacAddr          - the end of destination mac: 00:00:00:00:00:XX.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamEPclRuleCfmFlow
(
    IN GT_U32                   ruleIndex,
    IN GT_U32                   flowId,
    IN GT_U8                    dstMacAddr,
    IN GT_U8                    srcMacAddr,
    IN GT_U32                   vlanId,
	IN GT_U8                    srcPortNum	
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat; 
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */

    if (flowId < 1) 
        return GT_OUT_OF_RANGE;
    /* set new rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E;
        /* Set Mask */
    mask.ruleEgrUdbOnly.replacedFld.pclId = 0x3FF;
    mask.ruleIngrUdbOnly.udb[2] = 0xFF; /* Destination MAC */
    mask.ruleIngrUdbOnly.udb[3] = 0xFF;
    mask.ruleIngrUdbOnly.udb[4] = 0xFF;
    mask.ruleIngrUdbOnly.udb[5] = 0xFF;
    mask.ruleIngrUdbOnly.udb[6] = 0xFF;
    mask.ruleIngrUdbOnly.udb[7] = 0xFF;
    mask.ruleIngrUdbOnly.udb[8] = 0xFF; /* Source MAC */
    mask.ruleIngrUdbOnly.udb[9] = 0xFF;
    mask.ruleIngrUdbOnly.udb[10] = 0xFF;
    mask.ruleIngrUdbOnly.udb[11] = 0xFF;
    mask.ruleIngrUdbOnly.udb[12] = 0xFF;
    mask.ruleIngrUdbOnly.udb[13] = 0xFF;
    mask.ruleIngrUdbOnly.udb[14] = 0xF0; /* VLAN ID */
    mask.ruleIngrUdbOnly.udb[15] = 0xFF;
	if(srcPortNum == 255)
		mask.ruleIngrUdbOnly.udb[16] = 0x00; /* Source port */
	else
		mask.ruleIngrUdbOnly.udb[16] = 0xFF; /* Source port */
    mask.ruleIngrUdbOnly.udb[17] = 0xFF; /* Ethertype */
    mask.ruleIngrUdbOnly.udb[18] = 0xFF;
    /* Set Pattern */
    pattern.ruleEgrUdbOnly.replacedFld.pclId = 0x003;  
    pattern.ruleIngrUdbOnly.udb[2] = 0x00; /* Destination MAC */
    pattern.ruleIngrUdbOnly.udb[3] = 0x00;
    pattern.ruleIngrUdbOnly.udb[4] = 0x00;
    pattern.ruleIngrUdbOnly.udb[5] = 0x00;
    pattern.ruleIngrUdbOnly.udb[6] = 0x00;
    pattern.ruleIngrUdbOnly.udb[7] = dstMacAddr; 
    pattern.ruleIngrUdbOnly.udb[8] = 0x00;/* Source MAC */
    pattern.ruleIngrUdbOnly.udb[9] = 0x00;
    pattern.ruleIngrUdbOnly.udb[10] = 0x00;
    pattern.ruleIngrUdbOnly.udb[11] = 0x00;
    pattern.ruleIngrUdbOnly.udb[12] = 0x00;
    pattern.ruleIngrUdbOnly.udb[13] = srcMacAddr; 
    pattern.ruleIngrUdbOnly.udb[14] = (GT_U8)((vlanId << 4) & 0xF0); /* VLAN ID */
    pattern.ruleIngrUdbOnly.udb[15] = (GT_U8)((vlanId >> 4)& 0xFF);
    pattern.ruleIngrUdbOnly.udb[16] = srcPortNum; /* Source port - CPU port */
    pattern.ruleIngrUdbOnly.udb[17] = 0x02; /* Ethertype */
    pattern.ruleIngrUdbOnly.udb[18] = 0x89;
  
    /* Set Action */     
    action.egressPolicy = GT_TRUE;
    action.oam.oamProcessEnable = GT_TRUE;
    action.flowId = flowId;

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclRuleCfmMplsTpFlow
*
* DESCRIPTION:
*       Set OAM CFM MPLS-TP per flow Ingress PCL rule.
*       The functions sets a 10B UDB rule format with the following key:
*       PCL-ID                  = 0x0, UDB[0-1]
*       MPLS Lable 0            = mplsLable0, UDB[2-4]
*
*       The action sets OAM processing and flow ID.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd. 
*       mplsLable0          - The MPLS lable 0 (transport lable). 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclRuleCfmMplsTpFlow
(
    IN GT_U32                   ruleIndex,
    IN GT_U32                   flowId, 
    IN GT_U32                   mplsLable0
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat; 
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */

    if (flowId < 1) 
        return GT_OUT_OF_RANGE;

    /* set new rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E;
    /* Set Mask */
    mask.ruleIngrUdbOnly.replacedFld.pclId = 0x3FF;
    /* UDB[2-4] are counfigured for MPLS Label 0 */
    mask.ruleIngrUdbOnly.udb[2] = 0xFF; 
    mask.ruleIngrUdbOnly.udb[3] = 0xFF;
    mask.ruleIngrUdbOnly.udb[4] = 0xF0;
    /* Set Pattern */
    pattern.ruleIngrUdbOnly.replacedFld.pclId = 0x001;
    pattern.ruleIngrUdbOnly.udb[2] = (GT_U8)((mplsLable0 & 0xFF000) >> 12); 
    pattern.ruleIngrUdbOnly.udb[3] = (GT_U8)((mplsLable0 & 0x00FF0) >> 4);
    pattern.ruleIngrUdbOnly.udb[4] = (GT_U8)((mplsLable0 & 0xF) << 4);

    /* Set Action */                            
    action.oam.oamProcessEnable = GT_TRUE;
    action.flowId = flowId;

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclRuleLmData
*
* DESCRIPTION:
*       Set OAM LM DATA flow PCL rule according to direction, for policer
*       billing counting.
*       The functions sets a 20B UDB rule format with the following key:
*       PCL-ID                  = 0x0 (Ingress), UDB[0-1]
*                               = 0x3 (Egress)
*       source MAC              = 00:00:00:00:00:01, UDB[2-7]
*       VLAN ID                 = flowId, UDB[8-9]
*       Source Port             = portNum, UDB[10] (for Ingress)
*
*       The action sets policer enable, FORWARD action and flow ID.
*       Counting is done based on flow ID.
*
* INPUTS:
*       direction           - PCL direction.
*       ruleIndex           - index of the rule in the TCAM.
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       srcPortNum          - The interface source port. relevant only 
*                             for Ingress.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclRuleLmData
(
    IN CPSS_PCL_DIRECTION_ENT           direction,
    IN GT_U32                           ruleIndex,
    IN GT_U32                           flowId,
    IN GT_U8                            sourcePortNum,
    IN GT_U8                            srcMacAddr,
    IN GT_U32                           vlanId
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat; 
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */
    GT_U8                              udbMask[11]; /* mask UDB value */
    GT_U8                              udbPattern[11]; /* pattern UDB value */

    /* set new rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));
    cpssOsMemSet(udbMask, '\0', 11);
    cpssOsMemSet(udbPattern, '\0', 11);

     /* Set Mask */   
    udbMask[2] = 0xFF; /* Source MAC */
    udbMask[3] = 0xFF;
    udbMask[4] = 0xFF;
    udbMask[5] = 0xFF;
    udbMask[6] = 0xFF;
    udbMask[7] = 0xFF;
    
    /* Set Pattern */    
    udbPattern[2] = 0x00; /* Source MAC */
    udbPattern[3] = 0x00;
    udbPattern[4] = 0x00;
    udbPattern[5] = 0x00;
    udbPattern[6] = 0x00;
    udbPattern[7] = srcMacAddr;/* Source MAC */
    if(direction == CPSS_PCL_DIRECTION_INGRESS_E) /* ingress pcl */
    {
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E;
        udbMask[8] = 0xFF; /* VLAN ID */
        udbMask[9] = 0x0F;
        udbMask[10] = 0xFF; /* Source port*/
        cpssOsMemCpy(mask.ruleIngrUdbOnly.udb, udbMask, 11);       
        mask.ruleIngrUdbOnly.replacedFld.pclId = 0x3FF;  
        udbPattern[8]  = (GT_U8)vlanId; /* VLAN ID */
        udbPattern[9]  = (GT_U8)((vlanId >> 8)& 0x0F);
        udbPattern[10] = sourcePortNum; /* Source port*/
        cpssOsMemCpy(pattern.ruleIngrUdbOnly.udb, udbPattern, 11);
        pattern.ruleIngrUdbOnly.replacedFld.pclId = 0x000;
    }
    else /* egress pcl */
    {
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E;
        udbMask[8] = 0xF0; /* VLAN ID */
        udbMask[9] = 0xFF;
        udbMask[10] = 0x00; /* Source port*/
        cpssOsMemCpy(mask.ruleEgrUdbOnly.udb, udbMask, 11);       
        mask.ruleEgrUdbOnly.replacedFld.pclId = 0x3FF;
        udbPattern[8] = (GT_U8)((vlanId << 4) & 0xF0); /* VLAN ID */
        udbPattern[9] = (GT_U8)((vlanId >> 4)& 0xFF);
        udbPattern[10] = sourcePortNum;
        cpssOsMemCpy(pattern.ruleEgrUdbOnly.udb, udbPattern, 11);
        pattern.ruleEgrUdbOnly.replacedFld.pclId = 0x003;  
        action.egressPolicy = GT_TRUE;
    }

    /* Set Action */
    action.pktCmd = CPSS_PACKET_CMD_FORWARD_E;
    action.policer.policerEnable = CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E;
    action.flowId = flowId;
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    { 
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
                                0, &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}
/*******************************************************************************
* gtAppDemoOamPclRuleBfdIpv4MplsLspFlow
*
* DESCRIPTION:
*       Set OAM BFD IPv4 MPLS per flow PCL rule.
*       The functions sets a 30B UDB ingress rule format with the following 
*       key:
*       PCL-ID                  = 0x1, UDB[0-1]
*       source IP               = 192.168.ipAddrThirdByte.ipAddrLastByte, UDB[2-5]
*       destnation IP           = 192.168.1.100, UDB[6-9]
*       MPLS application label  = 0x00066, UDB[10-12]
*       BFD My Discriminator    = 0x0000AB01, UDB[13-16]
*       BFD Your Discriminator  = 0x0000AB02, UDB[17-20]
*
*       The action sets OAM processing and flow ID,
*       also packet command is soft drop.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       ipAddrThirdByte     - The 3rd byte source IP address (192.168.XXX.001).
*       ipAddrLastByte      - The last byte source IP address (192.168.001.XXX).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclRuleBfdIpv4MplsLspFlow
(
    IN GT_U32 ruleIndex,
    IN GT_U32 flowId,
    IN GT_U8 ipAddrThirdByte,
    IN GT_U8 ipAddrLastByte
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */

    if(flowId < 1)
        return GT_OUT_OF_RANGE;

    /* set new IPCL rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E;
    /* Set Mask */
    mask.ruleIngrUdbOnly.replacedFld.pclId = 0x3FF;
    mask.ruleIngrUdbOnly.udb[2] = 0xFF; /* src IP */
    mask.ruleIngrUdbOnly.udb[3] = 0xFF;
    mask.ruleIngrUdbOnly.udb[4] = 0xFF;
    mask.ruleIngrUdbOnly.udb[5] = 0xFF;
    mask.ruleIngrUdbOnly.udb[6] = 0xFF; /* dst IP */
    mask.ruleIngrUdbOnly.udb[7] = 0xFF;
    mask.ruleIngrUdbOnly.udb[8] = 0xFF;
    mask.ruleIngrUdbOnly.udb[9] = 0xFF;
    mask.ruleIngrUdbOnly.udb[10] = 0xFF; /* MPLS application label */
    mask.ruleIngrUdbOnly.udb[11] = 0xFF;
    mask.ruleIngrUdbOnly.udb[12] = 0xF0;

    mask.ruleIngrUdbOnly.udb[13] = 0xFF; /* My Discriminator */
    mask.ruleIngrUdbOnly.udb[14] = 0xFF;
    mask.ruleIngrUdbOnly.udb[15] = 0xFF;
    mask.ruleIngrUdbOnly.udb[16] = 0xFF;
    mask.ruleIngrUdbOnly.udb[17] = 0xFF; /* Your Discriminator*/
    mask.ruleIngrUdbOnly.udb[18] = 0xFF;
    mask.ruleIngrUdbOnly.udb[19] = 0xFF;
    mask.ruleIngrUdbOnly.udb[20] = 0xFF;

    /* Set Pattern */
    pattern.ruleIngrUdbOnly.replacedFld.pclId = 0x001;
    pattern.ruleIngrUdbOnly.udb[2] = 0xC0; /* src IP */
    pattern.ruleIngrUdbOnly.udb[3] = 0xA8;
    pattern.ruleIngrUdbOnly.udb[4] = ipAddrThirdByte;
    pattern.ruleIngrUdbOnly.udb[5] = ipAddrLastByte;
    pattern.ruleIngrUdbOnly.udb[6] = 0xC0; /* dst IP */
    pattern.ruleIngrUdbOnly.udb[7] = 0xA8;
    pattern.ruleIngrUdbOnly.udb[8] = 0x01;
    pattern.ruleIngrUdbOnly.udb[9] = 0x64;
    pattern.ruleIngrUdbOnly.udb[10] = 0x00; /* MPLS application label */
    pattern.ruleIngrUdbOnly.udb[11] = 0x06;
    pattern.ruleIngrUdbOnly.udb[12] = 0x60;
    pattern.ruleIngrUdbOnly.udb[13] = 0x00; /* My Discriminator */
    pattern.ruleIngrUdbOnly.udb[14] = 0x00;
    pattern.ruleIngrUdbOnly.udb[15] = 0xAB;
    pattern.ruleIngrUdbOnly.udb[16] = 0x01;
    pattern.ruleIngrUdbOnly.udb[17] = 0x00; /* Your Discriminator */
    pattern.ruleIngrUdbOnly.udb[18] = 0x00;
    pattern.ruleIngrUdbOnly.udb[19] = 0xAB;
    pattern.ruleIngrUdbOnly.udb[20] = 0x02;
    /* Set Action */   

    action.oam.oamProcessEnable = GT_TRUE;
    action.flowId = flowId;
    /* need to enable keepalive for soft dropped packets for this to work */
    action.pktCmd = CPSS_PACKET_CMD_DROP_SOFT_E ; 
    /* must be set in SIP5 when assigning packet command */
    action.mirror.cpuCode = GT_APP_DEMO_OAM_PCL_CPU_CODE;


    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {    
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclRuleBfdMplsTpFlow
*
* DESCRIPTION:
*       Set OAM BFD MPLS-TP per flow PCL rule.
*       The functions sets a 10B UDB ingress rule format with the following 
*       key:
*       PCL-ID                  = 0x1, UDB[0-1]
*       BFD My Discriminator    = 0x0000AB01, UDB[2-5]
*       BFD Your Discriminator  = 0x0000AB02, UDB[6-9]
*
*       The action sets OAM processing and flow ID,
*       also packet command is soft drop.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclRuleBfdMplsTpFlow
(
    IN GT_U32 ruleIndex,
    IN GT_U32 flowId
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */

    if(flowId < 1)
        return GT_OUT_OF_RANGE;

    /* set new IPCL rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E;
    /* Set Mask */
    mask.ruleIngrUdbOnly.replacedFld.pclId = 0x3FF;
    mask.ruleIngrUdbOnly.udb[2] = 0xFF; /* My Discriminator */
    mask.ruleIngrUdbOnly.udb[3] = 0xFF;
    mask.ruleIngrUdbOnly.udb[4] = 0xFF;
    mask.ruleIngrUdbOnly.udb[5] = 0xFF;
    mask.ruleIngrUdbOnly.udb[6] = 0xFF; /* Your Discriminator*/
    mask.ruleIngrUdbOnly.udb[7] = 0xFF;
    mask.ruleIngrUdbOnly.udb[8] = 0xFF;
    mask.ruleIngrUdbOnly.udb[9] = 0xFF;

    /* Set Pattern */
    pattern.ruleIngrUdbOnly.replacedFld.pclId = 0x001;
    pattern.ruleIngrUdbOnly.udb[2] = 0x00; /* My Discriminator */
    pattern.ruleIngrUdbOnly.udb[3] = 0x00;
    pattern.ruleIngrUdbOnly.udb[4] = 0xAB;
    pattern.ruleIngrUdbOnly.udb[5] = 0x01;
    pattern.ruleIngrUdbOnly.udb[6] = 0x00; /* Your Discriminator */
    pattern.ruleIngrUdbOnly.udb[7] = 0x00;
    pattern.ruleIngrUdbOnly.udb[8] = 0xAB;
    pattern.ruleIngrUdbOnly.udb[9] = 0x02;

    /* Set Action */ 
    action.oam.oamProcessEnable = GT_TRUE;
    action.flowId = flowId;
    /* need to enable keepalive for soft dropped packets for this to work */
    action.pktCmd = CPSS_PACKET_CMD_DROP_SOFT_E ; 
    /* must be set in SIP5 when assigning packet command */
    action.mirror.cpuCode = GT_APP_DEMO_OAM_PCL_CPU_CODE;


    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {    
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclRuleBfdIpv4Flow
*
* DESCRIPTION:
*       Set OAM BFD IPv4 per flow PCL rule.
*       The functions sets a 20B UDB ingress rule format with the following 
*       key:
*       PCL-ID                  = 0x1, UDB[0-1]
*       source IP               = 192.168.ipAddrThirdByte.ipAddrLastByte, UDB[2-5]
*       destnation IP           = 192.168.1.100, UDB[6-9]
*       BFD My Discriminator    = 0x0000AB01, UDB[10-13]
*       BFD Your Discriminator  = 0x0000AB02, UDB[14-17]
*
*       The action sets OAM processing and flow ID,
*       also packet command is soft drop.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       srcIpAddrThirdByte  - The 3rd byte source IP address (192.168.XXX.001).
*       srcIpAddrLastByte   - The last byte source IP address (192.168.001.XXX).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPclRuleBfdIpv4Flow
(
    IN GT_U32 ruleIndex,
    IN GT_U32 flowId,
    IN GT_U8 srcIpAddrThirdByte,
    IN GT_U8 srcIpAddrLastByte
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */

    if(flowId < 1)
        return GT_OUT_OF_RANGE;
    /* set new IPCL rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E;
    /* Set Mask */
    mask.ruleIngrUdbOnly.replacedFld.pclId = 0x3FF;
    mask.ruleIngrUdbOnly.udb[2] = 0xFF;
    mask.ruleIngrUdbOnly.udb[3] = 0xFF;
    mask.ruleIngrUdbOnly.udb[4] = 0xFF;
    mask.ruleIngrUdbOnly.udb[5] = 0xFF;
    mask.ruleIngrUdbOnly.udb[6] = 0xFF;
    mask.ruleIngrUdbOnly.udb[7] = 0xFF;
    mask.ruleIngrUdbOnly.udb[8] = 0xFF;
    mask.ruleIngrUdbOnly.udb[9] = 0xFF;
    mask.ruleIngrUdbOnly.udb[10] = 0xFF;
    mask.ruleIngrUdbOnly.udb[11] = 0xFF;
    mask.ruleIngrUdbOnly.udb[12] = 0xFF;
    mask.ruleIngrUdbOnly.udb[13] = 0xFF;
    mask.ruleIngrUdbOnly.udb[14] = 0xFF;
    mask.ruleIngrUdbOnly.udb[15] = 0xFF;
    mask.ruleIngrUdbOnly.udb[16] = 0xFF;
    mask.ruleIngrUdbOnly.udb[17] = 0xFF;
    /* Set Pattern */
    pattern.ruleIngrUdbOnly.replacedFld.pclId = 0x001;
    pattern.ruleIngrUdbOnly.udb[2] = 0xC0; /* src IP */
    pattern.ruleIngrUdbOnly.udb[3] = 0xA8;
    pattern.ruleIngrUdbOnly.udb[4] = srcIpAddrThirdByte;
    pattern.ruleIngrUdbOnly.udb[5] = srcIpAddrLastByte;
    pattern.ruleIngrUdbOnly.udb[6] = 0xC0; /* dst IP */
    pattern.ruleIngrUdbOnly.udb[7] = 0xA8;
    pattern.ruleIngrUdbOnly.udb[8] = 0x01;
    pattern.ruleIngrUdbOnly.udb[9] = 0x64;
    pattern.ruleIngrUdbOnly.udb[10] = 0x00; /* My Discriminator */
    pattern.ruleIngrUdbOnly.udb[11] = 0x00;
    pattern.ruleIngrUdbOnly.udb[12] = 0xAB;
    pattern.ruleIngrUdbOnly.udb[13] = 0x01;
    pattern.ruleIngrUdbOnly.udb[14] = 0x00; /* Your Discriminator */
    pattern.ruleIngrUdbOnly.udb[15] = 0x00;
    pattern.ruleIngrUdbOnly.udb[16] = 0xAB;
    pattern.ruleIngrUdbOnly.udb[17] = 0x02;
    /* Set Action */                            
    action.oam.oamProcessEnable = GT_TRUE;
    action.flowId = flowId;
    /* The action should be CPSS_PACKET_CMD_DROP_SOFT_E but
       there is a Bugs00149492 */
    action.pktCmd = CPSS_PACKET_CMD_FORWARD_E;
    /* must be set in SIP5 when assigning packet command */
    action.mirror.cpuCode = GT_APP_DEMO_OAM_PCL_CPU_CODE;


    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {    
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPclRuleBfdIpv4
*
* DESCRIPTION:
*       Set PCL rule to indentify OAM BFD IPv4 UDP packets according to RFC 5881.
*       The functions sets a IP L2 QOS ingress rule format with the following 
*       key:
*       PCL-ID                = 0x0
*       is IP                 = 0x1
*       is IPv4               = 0x1
*       IP protocol           = 17 (UDP)
*       destanation UDP port  = 3784 (BFD)
*       IPv4 TTL              = 255
*
*       The action is set to override PCL lookup 1 configuration to index 0.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
int gtAppDemoOamPclRuleBfdIpv4
(
    IN GT_U32 ruleIndex
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask; /* rule mask */
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern; /* rule pattern */
    CPSS_DXCH_PCL_ACTION_STC           action; /* rule action */

    /* set new IPCL rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_PCL_ACTION_STC));

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    /* Set Mask */
    mask.ruleStdIpL2Qos.common.pclId = 0x3FF;
    mask.ruleStdIpL2Qos.common.isIp = 0x1;
    mask.ruleStdIpL2Qos.commonStdIp.isIpv4  = 0x01;
    mask.ruleStdIpL2Qos.commonStdIp.ipProtocol  = 0xFF;
    mask.ruleStdIpL2Qos.commonStdIp.l4Byte2 = 0xFF;
    mask.ruleStdIpL2Qos.commonStdIp.l4Byte3 = 0xFF;
    mask.ruleStdIpL2Qos.udb18_19[0] = 0xFF; /* UDB18*/
    /* Set Pattern */
    pattern.ruleStdIpL2Qos.common.pclId = 0x000;
    pattern.ruleStdIpL2Qos.common.isIp = 0x1;
    pattern.ruleStdIpL2Qos.commonStdIp.isIpv4  = 0x01;
    pattern.ruleStdIpL2Qos.commonStdIp.ipProtocol  = 17;
    pattern.ruleStdIpL2Qos.commonStdIp.l4Byte2 = 0x0E;
    pattern.ruleStdIpL2Qos.commonStdIp.l4Byte3 = 0xC8;
    pattern.ruleStdIpL2Qos.udb18_19[0] = 255; /* IPv4 TTL*/
    /* Set Action */
    action.lookupConfig.ipclConfigIndex = 0x0;
    action.lookupConfig.pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;

    for(devNum=0; devNum<appDemoPpConfigDevAmount; devNum++)
    {    
        rc = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, 0,
                                &mask, &pattern, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamTtiRuleBfdIpv4MplsLsp
*
* DESCRIPTION:
*       Set TTI rule to indentify OAM BFD IPv4 MPLS packets according to RFC 5884.
*       The functions sets a 20B UDB ingress rule format with the following 
*       key:
*       PCL-ID                = the value that was initially set by appDemo, UDB[0-1]
*       MPLS Transport Label  = 0x00055, UDB[2-4]
*       MPLS S0               = 0x0, UDB[4]
*       MPLS S1               = 0x1, UDB[5]
*       Protocol After MPLS   = 0x0 (IPv4), UDB[6]   
*       IPv4 TTL              = 0x1, UDB[7]
*       IP protocol           = 17 (UDP), UDB[8]   
*       UDP destanation port  = 3784 (BFD), UDB[9-10]

*
*       The action is set to override PCL lookup 0 configuration to index 0, 
*       override UDB configuration index to UDE1 and parse passenger as IP.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamTtiRuleBfdIpv4MplsLsp
(
    IN GT_U32 ruleIndex
)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    GT_U32                              mplsPclId;
    CPSS_DXCH_TTI_RULE_UNT              pattern; /* rule pattern */
    CPSS_DXCH_TTI_RULE_UNT              mask; /* rule mask */
    CPSS_DXCH_TTI_ACTION_UNT            action; /* rule action */
    
    /* set new rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_TTI_ACTION_UNT));
    /*Set Pattern */
    pattern.udbArray[2] = 0x00; /* mpls.label0 */
    pattern.udbArray[3] = 0x05;
    pattern.udbArray[4] = 0x50; /* S0 = 0 */
    pattern.udbArray[5] = 0x01; /* S1 = 1 */
    pattern.udbArray[6] = 0x0;  /* 0 = IPv4 (ms nibble = 0x4) */
    pattern.udbArray[7] = 0x1;  /* IPv4 TTL */
    pattern.udbArray[8] = 0x11; /* IP Protocol = 17 (UDP) */
    pattern.udbArray[9] = 0x0E; /* Destination UDP port*/
    pattern.udbArray[10] = 0xC8;
    /* Set Mask */
    mask.udbArray[0] = 0x1F;    /* PCL-ID [4:0] in metadata */
    mask.udbArray[1] = 0x1F;    /* PCL-ID [9:5] in metadata */
    mask.udbArray[2] = 0xFF;    /* Label0 */
    mask.udbArray[3] = 0xFF;    /* Label0 */
    mask.udbArray[4] = 0xF1;    /* Label0 , S0*/
    mask.udbArray[5] = 0x01;    /* S1 */
    mask.udbArray[6] = 0x03;    /* Protocol after mpls in metadata*/
    mask.udbArray[7] = 0xFF;    /* IPv4 TTL */
    mask.udbArray[8] = 0xFF;    /* IP Protocol */
    mask.udbArray[9] = 0xFF;    /* Destination UDP port*/
    mask.udbArray[10] = 0xFF;   /* Destination UDP port*/
    /* Set Action */
    /* This field must be set with a value different from default */
    action.type2.tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
    /* Override lookup0 default configuration */
    action.type2.pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
    action.type2.iPclConfigIndex = 0x000;
    action.type2.iPclUdbConfigTableEnable = GT_TRUE;
    /* override PCL UDB configuration */
    action.type2.iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E;
    /* parsing with override PCL UDB is not currently possible, see Bugs00148646 */
    /* Parse Passenger as IP Only  
    action.type2.passengerParsingOfTransitMplsTunnelMode = CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_IP_E; */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    { 
        rc = cpssDxChTtiPclIdGet(devNum, CPSS_DXCH_TTI_KEY_UDB_MPLS_E, &mplsPclId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        pattern.udbArray[0] = mplsPclId & 0xFF;
        pattern.udbArray[1] = (mplsPclId & 0x300) >> 0x8; 
        rc = cpssDxChTtiRuleSet(devNum, ruleIndex, CPSS_DXCH_TTI_RULE_UDB_20_E,
                                &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    } 
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamTtiRuleBfdMplsTp
*
* DESCRIPTION:
*       Set TTI rule to indentify BFD MPLS-TP packets.
*       The functions sets a TTI MPLS ingress rule format with the following 
*       key:
*       PCL-ID                = the value that was initially set by appDemo.
*       label0                = 0x55
*       reservedLabelExist    = GT_TRUE
*       reservedLabelValue    = 13 (OAM GAL)
*       channelTypeProfile    = 0x2, CW channel type profile table index. 
*
*       The action is set to override PCL lookup 0 configuration to index 0 and 
*       override UDB configuration index to UDE2.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamTtiRuleBfdMplsTp
(
    IN GT_U32 ruleIndex
)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    GT_U32                              mplsPclId;
    CPSS_DXCH_TTI_RULE_UNT              pattern; /* rule pattern */
    CPSS_DXCH_TTI_RULE_UNT              mask; /* rule mask */
    CPSS_DXCH_TTI_ACTION_UNT            action; /* rule action */
    
    /* set new rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_TTI_ACTION_UNT));

    /*Set Pattern */
    pattern.mpls.label0 = 0x55;
    pattern.mpls.reservedLabelExist = GT_TRUE;
    pattern.mpls.reservedLabelValue = 13;
    /* There is a bug in simulation and the channel type profile
       is not taken from the right place, it doesn't look at all at the
       Channel Type Profile Table */
#ifdef ASIC_SIMULATION
    pattern.mpls.channelTypeProfile = 0x1;
#else
    pattern.mpls.channelTypeProfile = 0x2;
#endif
    /* Set Mask */
    mask.mpls.common.pclId = 0x3ff;
    mask.mpls.label0 = 0xFFFFF;
    mask.mpls.reservedLabelExist = 0xFF;
    mask.mpls.reservedLabelValue = 0xF;
    mask.mpls.channelTypeProfile = 0xF;

    /* Set Action */
    /* This field must be set with a value different from default */
    action.type2.tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
    /* Override lookup0 default configuration */
    action.type2.pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
    action.type2.iPclConfigIndex = 0x000;
    action.type2.iPclUdbConfigTableEnable = GT_TRUE;
    /* override PCL UDB configuration */
    action.type2.iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E;

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    { 
        rc = cpssDxChTtiPclIdGet(devNum, CPSS_DXCH_TTI_KEY_MPLS_E, &mplsPclId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        pattern.mpls.common.pclId = mplsPclId; 
        rc = cpssDxChTtiRuleSet(devNum, ruleIndex, CPSS_DXCH_TTI_RULE_MPLS_E,
                                &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    } 
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamTtiRuleCfmMplsTp
*
* DESCRIPTION:
*       Set TTI rule to indentify OAM CFM MPLS-TP packets.
*       The functions sets a TTI MPLS ingress rule format with the following 
*       key:
*       PCL-ID                = the value that was initially set by appDemo.
*       reservedLabelExist    = GT_TRUE.
*       reservedLabelValue    = 13 (OAM GAL)
*       channelTypeProfile    = 0x1, CW channel type profile table index. 
*
*       The action is set to override PCL lookup 0 configuration to index 0 and 
*       override UDB configuration index to UDE3.
* 
* INPUTS:
*       ruleIndex           - index of the rule in the TCAM.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamTtiRuleCfmMplsTp
(
    IN GT_U32 ruleIndex
)
{
    GT_STATUS                           rc; /* function return code */
    GT_U8                               devNum; /* device number */
    GT_U32                              mplsPclId;
    CPSS_DXCH_TTI_RULE_UNT              pattern; /* rule pattern */
    CPSS_DXCH_TTI_RULE_UNT              mask; /* rule mask */
    CPSS_DXCH_TTI_ACTION_UNT            action; /* rule action */
    
    /* set new rule */
    cpssOsMemSet(&mask, '\0', sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cpssOsMemSet(&pattern, '\0', sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cpssOsMemSet(&action, '\0', sizeof(CPSS_DXCH_TTI_ACTION_UNT));

    /*Set Pattern */
    pattern.mpls.reservedLabelExist = GT_TRUE;
    pattern.mpls.reservedLabelValue = 13;
    pattern.mpls.channelTypeProfile = 0x1;

    /* Set Mask */
    mask.mpls.common.pclId = 0x3ff;
    mask.mpls.reservedLabelExist = 0xFF;
    mask.mpls.reservedLabelValue = 0xF;
    mask.mpls.channelTypeProfile = 0xF;

       /* Set Action */
    /* This field must be set with a value different from default */
    action.type2.tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
    /* Override lookup0 default configuration */
    action.type2.pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
    action.type2.iPclConfigIndex = 0x000;
    action.type2.iPclUdbConfigTableEnable = GT_TRUE;
    /* override PCL UDB configuration */
    action.type2.iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E;

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    { 
        rc = cpssDxChTtiPclIdGet(devNum, CPSS_DXCH_TTI_KEY_MPLS_E, &mplsPclId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        pattern.mpls.common.pclId = mplsPclId;

        rc = cpssDxChTtiRuleSet(devNum, ruleIndex, CPSS_DXCH_TTI_RULE_MPLS_E,
                                &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiRuleSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    } 
    return GT_OK;
}
/*******************************************************************************
* gtAppDemoOamPclDisableRule
*
* DESCRIPTION:  
*               This function disables PCL rule.
*
* INPUTS:       ruleIndex - the PCL rule index which be disabled
*
* OUTPUTS:      none.       
*
* RETURNS:     
*       GT_OK           - on success
*       GT_FAIL         - otherwise. 
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS gtAppDemoOamPclDisableRule
(
    IN GT_U32 ruleIndex
)
{
    GT_STATUS                          rc; /* function return code */
    GT_U8                              devNum; /* device number */
    GT_U32                             ruleSize;

    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChPclRuleInvalidate(devNum, ruleSize, ruleIndex);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclRuleInvalidate",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHashInitGen
*
* DESCRIPTION:
*       This function does generic initialization of trunk hash engine. 
*       Set CRC-Based Hash Mode, set Enhanced CRC-Based Hash Mode (32 bit)
*       with seed = 0xFF.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHashInitGen(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    GT_U32                                  crcSeed; /* The seed used by the CRC computation */
   
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        /* Set CRC-Based Hash Mode */
        rc = cpssDxChTrunkHashGlobalModeSet(devNum,CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkHashGlobalModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        crcSeed = 0xFF;
        /* Set Enhanced CRC-Based Hash Mode (32bit) */
        rc = cpssDxChTrunkHashCrcParametersSet(devNum,
                                               CPSS_DXCH_TRUNK_LBH_CRC_32_MODE_E,
                                               crcSeed);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkHashCrcParametersSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHashInitBfdIpv4
*
* DESCRIPTION:
*       This function does specific initialization of trunk hash engine 
*       and PCL UDB for BFD Ipv4 packets.
*       The UDB CRC mask is set to include UDBs[19-20] for IPv4 UDP packets.
*       IP data and MAC data are not added to the CRC hash. 
*       UDBs[19-20] are configured to the start of the BFD header.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHashInitBfdIpv4(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC  crcMaskEntry;
   
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&crcMaskEntry, '\0', sizeof(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC));
        /* UDBs 19-20 */
        crcMaskEntry.udbsMaskBmp = 0x180000;
        rc = cpssDxChTrunkHashMaskCrcEntrySet(devNum,
                                              16 +  CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E,
                                              &crcMaskEntry);
        /* IP data is not added to the trunk load balancing hash */
        rc = cpssDxChTrunkHashIpModeSet(devNum,GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkHashIpModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* If the packet is an IP packet MAC data is not
           added to the Trunk load balancing hash */
        rc = cpssDxChTrunkHashIpAddMacModeSet(devNum,GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkHashIpAddMacModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                          0/* Not relevant */, 
                          CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E, 
                          CPSS_PCL_DIRECTION_INGRESS_E,
                          19 /*udbIndex*/,
                          CPSS_DXCH_PCL_OFFSET_L4_E,
                          8/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                          0/* Not relevant */, 
                          CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E, 
                          CPSS_PCL_DIRECTION_INGRESS_E,
                          20/*udbIndex*/,
                          CPSS_DXCH_PCL_OFFSET_L4_E,
                          9/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHashInitBfdIpv4MplsLsp
*
* DESCRIPTION:
*       This function does specific initialization of trunk hash engine 
*       and PCL UDB for BFD Ipv4 MPLS packets.
*       The UDB CRC mask is set to include UDBs[14-15] for MPLS packets.
*       MPLS data is not added to the CRC hash. 
*       UDBs[14-15] are configured to the start of the BFD header.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHashInitBfdIpv4MplsLsp(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC  crcMaskEntry;
   
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        /* MPLS parameter are not used in trunk hash index */
        rc = cpssDxChTrunkHashMplsModeEnableSet(devNum,GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkHashMplsModeEnableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsMemSet(&crcMaskEntry, '\0', sizeof(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC));
        /* UDBs 14-15 */
        crcMaskEntry.udbsMaskBmp = 0xC000;
        rc = cpssDxChTrunkHashMaskCrcEntrySet(devNum,
                                              16 +  10 /*CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E*/,
                                              &crcMaskEntry);

        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                          0/* Not relevant */, 
                          CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E, 
                          CPSS_PCL_DIRECTION_INGRESS_E,
                          14 /*udbIndex*/,
                          CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E/*CPSS_DXCH_PCL_OFFSET_L4_E*/,
                          30/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                          0/* Not relevant */, 
                          CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E, 
                          CPSS_PCL_DIRECTION_INGRESS_E,
                          15/*udbIndex*/,
                          CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E/*CPSS_DXCH_PCL_OFFSET_L4_E*/,
                          31/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHashInitBfdMplsTp
*
* DESCRIPTION:
*       This function does specific initialization of trunk hash engine 
*       and PCL UDB for BFD MPLS-TP packets.
*       The UDB CRC mask is set to include UDBs[14-15] for UDE2 packets.
*       MPLS data is not added to the CRC hash. 
*       UDBs[14-15] are configured to the start of the BFD header.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHashInitBfdMplsTp(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC  crcMaskEntry;
   
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        /* MPLS parameter are not used in trunk hash index */
        rc = cpssDxChTrunkHashMplsModeEnableSet(devNum,GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkHashMplsModeEnableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsMemSet(&crcMaskEntry, '\0', sizeof(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC));
        /* UDBs 14-15 */
        crcMaskEntry.udbsMaskBmp = 0xC000;
        rc = cpssDxChTrunkHashMaskCrcEntrySet(devNum,
                                              16 +  11 /*CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E*/,
                                              &crcMaskEntry);

        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                          0/* Not relevant */, 
                          CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E, 
                          CPSS_PCL_DIRECTION_INGRESS_E,
                          14 /*udbIndex*/,
                          CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                          6/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                          0/* Not relevant */, 
                          CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E, 
                          CPSS_PCL_DIRECTION_INGRESS_E,
                          15/*udbIndex*/,
                          CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                          7/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}
/*******************************************************************************
* gtAppDemoOamHashInitCfm
*
* DESCRIPTION:
*       This function does specific initialization of trunk hash engine 
*       and PCL UDB for CFM packets.
*       The UDB CRC mask is set to include UDBs[14-15] for UDE0 packets.
*       UDBs[14-15] are configured to the CFM MEP ID in the OAM header.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHashInitCfm(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC  crcMaskEntry;
   
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&crcMaskEntry, '\0', sizeof(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC));
        /* UDBs 14-15 */
        crcMaskEntry.udbsMaskBmp = 0xC000;
        rc = cpssDxChTrunkHashMaskCrcEntrySet(devNum,
                                              16 + 9 /*CPSS_DXCH_PCL_PACKET_TYPE_UDE_E*/,
                                              &crcMaskEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkHashMaskCrcEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* CFM MEP ID */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           0/* Not relevant */, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           14 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           10/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           0/* Not relevant */, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           15/*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           11/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHashInitCfmMplsTp
*
* DESCRIPTION:
*       This function does specific initialization of trunk hash engine 
*       and PCL UDB for MPLS-TP CFM packets.
*       The UDB CRC mask is set to include UDBs[14-15] for UDE3 packets.
*       UDBs[14-15] are configured to the CFM MEP ID in the OAM header.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHashInitCfmMplsTp(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC  crcMaskEntry;
   
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        cpssOsMemSet(&crcMaskEntry, '\0', sizeof(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC));
        /* UDBs 14-15 */
        crcMaskEntry.udbsMaskBmp = 0xC000;
        rc = cpssDxChTrunkHashMaskCrcEntrySet(devNum,
                                              16 + 12 /*CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E*/,
                                              &crcMaskEntry);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkHashMaskCrcEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* CFM MEP ID */
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           0/* Not relevant */, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           14 /*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           14/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPclUserDefinedByteSet(devNum, 
                                           0/* Not relevant */, 
                                           CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E, 
                                           CPSS_PCL_DIRECTION_INGRESS_E,
                                           15/*udbIndex*/,
                                           CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
                                           15/*offset*/);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclUserDefinedByteSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}
#ifdef ASIC_SIMULATION
/*******************************************************************************
* prvAppDemoOamCbFunc
*
* DESCRIPTION:
*       This function is a call back for trapped RX packets.
*       It checks the packet flow ID and checks which exceptions occured for 
*       this flow. THis function is of type RX_PACKET_RECEIVE_CB_FUN.
*
* INPUTS:
*       devNum              - device number.
*       queueIdx            - received Rx SDMA queue.
*       numOfBuff           - number of received buffers.     
*       packetBuffs[]       - Array of packet buffers.
*       buffLen[]           - Array of buffers length.
*       rxParamsPtr         - net RX parameters (DSA tag).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvAppDemoOamCbFunc
(
    IN GT_U8      devNum,
    IN GT_U8      queueIdx,
    IN GT_U32     numOfBuff,
    IN GT_U8      *packetBuffs[],
    IN GT_U32     buffLen[],
    IN void       *rxParamsPtr
)
{
    GT_STATUS                           rc;
    GT_U32                              i, j; /* for loop indexes */
    GT_U32                              entryIndex; /* index of OAM exception table entry*/
    GT_U32                              bmpIndex; /* flow id mapped to bitmap */
    GT_U32                              entryBmp; /* OAM exception table entry */
    GT_U32                              flowId; /* OAM flow ID */
    GT_U8                               *pkt_ptr; /* pointer to the current buffer */
    CPSS_DXCH_NET_RX_PARAMS_STC         *dxChPcktParamsPtr; /* network Rx DSA paramaters */
    CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT    exceptionType; /* OAM exception type */
    CPSS_NET_RX_CPU_CODE_ENT            cpuCode;
    static char* exceptionStr[7]=
    {
        "MEG_LEVEL",
        "SOURCE_INTERFACE",
        "RDI_STATUS_CHANGED",
        "KEEPALIVE_AGING",
        "KEEPALIVE_EXCESS",
        "KEEPALIVE_INVALID_HASH",
        "TX_PERIOD"
    };
    cpssOsPrintf("devNum: %d\n",devNum);
    cpssOsPrintf("queueIdx: %d\n",queueIdx);
    dxChPcktParamsPtr = (CPSS_DXCH_NET_RX_PARAMS_STC*)rxParamsPtr;   
    for(i=0; i < numOfBuff; i++)
    {
        pkt_ptr =  packetBuffs[i];
        for (j=0; j < buffLen[i]; j++) 
        {
            cpssOsPrintf("%02lx ",pkt_ptr[j]);
            if (((j+1)%16)==0) 
                cpssOsPrintf("\n");
        }
        cpssOsPrintf("\n");

        flowId = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.flowIdTtOffset.flowId;
        cpssOsPrintf("Flow ID = %d\n", flowId);        
        entryIndex = flowId/32;
        for (exceptionType = 0; exceptionType < CPSS_DXCH_OAM_EXCEPTION_TYPE_SUMMARY_E; exceptionType++) 
        {
            rc = cpssDxChOamExceptionStatusGet(devNum,
                                               CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                               exceptionType,
                                               entryIndex,
                                               &entryBmp);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionStatusGet",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
            bmpIndex = flowId % 32;
            bmpIndex = 0x1 << bmpIndex;
            if(entryBmp & bmpIndex)
            {
                cpssOsPrintf("Exception %-22s occurred\n",exceptionStr[exceptionType]);
            }
        }
        cpuCode = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode;
        if(cpuCode == GT_APP_DEMO_OAM_LM_DM_CPU_CODE)
        {
            CPSS_DXCH_NET_TX_PARAMS_STC pcktParams;
            GT_U8                       temp;
            GT_U8                       *buffList[1];
            GT_U32                      buffLenList[1];
            for(j=0; j<6; j++)
            {
                temp = pkt_ptr[j];
                pkt_ptr[j] = pkt_ptr[j+6];
                pkt_ptr[j+6] = temp;
            }
            pkt_ptr[19]--;
            cpssOsMemSet(&pcktParams, '\0', sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
            pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
            pcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT;
            pcktParams.dsaParam.commonParams.vid = (GT_U16)flowId;
            pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.hwDevNum;;
            pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.interface.portNum;;
            pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
            pcktParams.dsaParam.dsaInfo.fromCpu.tc = 0;
            pcktParams.dsaParam.dsaInfo.fromCpu.dp =  CPSS_DP_GREEN_E;
                
            pcktParams.cookie = NULL;
            pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
            pcktParams.sdmaInfo.txQueue = GT_APP_DEMO_OAM_TX_QUEUE_NUM;
            pcktParams.sdmaInfo.evReqHndl = 0;
            buffLenList[0] = buffLen[i];
            buffList[0] = pkt_ptr;
                    /*cpssDxChNetIfSdmaTxPacketSend*/
            rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&pcktParams,buffList,buffLenList,1);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfSdmaSyncTxPacketSend",rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }        
    }     
    return GT_OK;
}
#endif
/*******************************************************************************
* gtAppDemoOamSdmaTxGeneratorEnable
*
* DESCRIPTION:
*       This function enables the Tx queue defined by 
*       GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS in generator mode and continuous 
*       or burst mode according to the input.       
*
* INPUTS:
*       burstEnable     - GT_TRUE for packets burst generation.
*                       - GT_FALSE for continuous packets generation.
*       burstPacketsNumber - Number of packets in burst.
*                            Relevant only if burstEnable == GT_TRUE.                           
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamSdmaTxGeneratorEnable
(
    IN  GT_BOOL                         burstEnable,
    IN  GT_U32                          burstPacketsNumber
)
{
    GT_STATUS                               rc;/* function return code */
    GT_U8                                   devNum;/* device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {

        rc = cpssDxChNetIfSdmaTxGeneratorEnable(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS,
                                                burstEnable,
                                                burstPacketsNumber);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfSdmaTxGeneratorEnable",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamSdmaTxGeneratorDisable
*
* DESCRIPTION:
*       This function disables the Tx queue defined by 
*       GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS.
*
* INPUTS:
*       None.                        
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamSdmaTxGeneratorDisable(GT_VOID)
{
    GT_STATUS                               rc;/* function return code */
    GT_U8                                   devNum;/* device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChNetIfSdmaTxGeneratorDisable(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfSdmaTxGeneratorDisable",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamSdmaTxGeneratorRateSet
*
* DESCRIPTION:
*       This function configures the Tx queue defined by
*       GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS to work in a rate of 
*       "per flow interval" with a rate of 3.3ms.
*
* INPUTS:
*       rate	- for 3.3 milisec set rate to 3300000.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       in ASIC_SIMULATION the rate is 1s
*
*******************************************************************************/
GT_STATUS gtAppDemoOamSdmaTxGeneratorRateSet(GT_U32 rate)
{
    GT_STATUS                                   rc;/* function return code */
    GT_U8                                       devNum;/* device number */
    GT_U64                                      actualRateValue;/* the actual configured rate value */
    GT_U64                                      rateValue;/* the rate in nanoseconds */

#ifdef ASIC_SIMULATION
    rateValue.l[0] = 1000000000/* 1 sec*/;
#else
    rateValue.l[0] = rate;/* 3.3ms */
#endif
    rateValue.l[1] = 0;
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChNetIfSdmaTxGeneratorRateSet(devNum,
                                                 CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                 GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS,
                                                 CPSS_DXCH_NET_TX_GENERATOR_RATE_MODE_PER_FLOW_INTERVAL_E,
                                                 rateValue,
                                                 &actualRateValue);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfSdmaTxGeneratorRateSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        if((actualRateValue.l[0] != rateValue.l[0]) &&
           (actualRateValue.l[1] != rateValue.l[1]))
            cpssOsPrintf("cpssDxChNetIfSdmaTxGeneratorRateSet, actualRateValue = 0x%x\n", actualRateValue);
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamSdmaTxGeneratorPacketAdd
*
* DESCRIPTION:
*       This function Adds a packet to the Tx queue defined by 
*       GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS, the packet is sent through
*       interface port type to port given by dstPortNum.
*
* INPUTS:
*       packetDataPtr       - pointer to the packet data.
*       packetDataLength    - the packet data lenght in bytes.  
*       dstPortNum          - Destination port number.                      
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamSdmaTxGeneratorPacketAdd
(
    IN GT_U8    vid,
    IN GT_U8    *packetDataPtr,
    IN GT_U32   packetDataLength,
    IN GT_U32   dstPortNum
)
{
    GT_STATUS                               rc;/* function return code */
    GT_U8                                   devNum;/* device number */
    CPSS_DXCH_NET_TX_PARAMS_STC             packetParams;/* sdma and dsa info */
    GT_U32                                  packetId; /* returned packet identification number */
    GT_HW_DEV_NUM                           hwDevNum; /* HW device number */

    cpssOsMemSet(&packetParams, '\0', sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
    packetParams.packetIsTagged = GT_TRUE;

    packetParams.sdmaInfo.txQueue = GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS;
    packetParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;
    packetParams.sdmaInfo.recalcCrc = GT_TRUE;

    packetParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E; 
    packetParams.dsaParam.commonParams.dsaTagType  = CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT;
    packetParams.dsaParam.commonParams.vid = (GT_U16)vid;
    packetParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;    
    packetParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = dstPortNum;

    packetParams.dsaParam.dsaInfo.fromCpu.tc = GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS;
    packetParams.dsaParam.dsaInfo.fromCpu.dp =  CPSS_DP_GREEN_E;
    
    packetParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = GT_TRUE;
    packetParams.dsaParam.dsaInfo.fromCpu.isTrgPhyPortValid = GT_TRUE;
    packetParams.dsaParam.dsaInfo.fromCpu.dstEport = dstPortNum;
    
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChCfgHwDevNumGet(devNum,&hwDevNum);
        packetParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum = hwDevNum;
        packetParams.dsaParam.dsaInfo.fromCpu.srcHwDev = hwDevNum;
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChNetIfSdmaTxGeneratorPacketAdd(devNum,
                                                   CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                   &packetParams,
                                                   packetDataPtr,
                                                   packetDataLength,
                                                   &packetId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfSdmaTxGeneratorPacketAdd",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("cpssDxChNetIfSdmaTxGeneratorPacketAdd, packetId = %d\n", packetId);
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamSdmaTxGeneratorPacketRemove
*
* DESCRIPTION:
*       This function removes a packet from the Tx queue defined by 
*       GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS, the packet is removed
*       according to packetId.
*
* INPUTS:
*       packetId        - packet identification number.                     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamSdmaTxGeneratorPacketRemove
(
    IN GT_U32   packetId
)
{
    GT_STATUS                               rc;/* function return code */
    GT_U8                                   devNum;/* device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChNetIfSdmaTxGeneratorPacketRemove(devNum,
                                                      CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                      GT_APP_DEMO_OAM_SDMA_TX_GENERATOR_Q_CNS,
                                                      packetId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfSdmaTxGeneratorPacketRemove",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamSdmaTxPacketSend
*
* DESCRIPTION:
*       This function sends a single packet.
*       The packet is sent through interface port type to port given by 
*       dstPortNum with VLAN vid. 
*
* INPUTS:
*       vid                 - VLAN ID.
*       packetDataPtr       - pointer to the packet data.
*       packetDataLength    - the packet data lenght in bytes.   
*       dstPortNum          - Destination port number.                           
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamSdmaTxPacketSend
(
    IN GT_U8    vid,
    IN GT_U8    *packetDataPtr,
    IN GT_U32   packetDataLength,   
    IN GT_U32   dstPortNum
)
{
    GT_STATUS                   rc;
    CPSS_DXCH_NET_TX_PARAMS_STC pcktParams;
    GT_U8                       *buffList[1];
    GT_U32                      buffLenList[1];
    GT_HW_DEV_NUM               hwDevNum; /* HW device number */

    cpssOsMemSet(&pcktParams, '\0', sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
    pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
    pcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT;
    pcktParams.dsaParam.commonParams.vid = (GT_U16)vid;
    pcktParams.packetIsTagged = GT_TRUE;
    rc = cpssDxChCfgHwDevNumGet(0,&hwDevNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet",rc);
    if(rc != GT_OK)
    {
        return rc;
    }
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum = hwDevNum;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = dstPortNum;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
    pcktParams.dsaParam.dsaInfo.fromCpu.tc = GT_APP_DEMO_OAM_TX_QUEUE_NUM;
    pcktParams.dsaParam.dsaInfo.fromCpu.dp =  CPSS_DP_GREEN_E;
    pcktParams.dsaParam.dsaInfo.fromCpu.srcHwDev = hwDevNum;
    pcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = GT_TRUE;
    pcktParams.dsaParam.dsaInfo.fromCpu.isTrgPhyPortValid = GT_TRUE;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstEport = dstPortNum;
    pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    pcktParams.sdmaInfo.txQueue = GT_APP_DEMO_OAM_TX_QUEUE_NUM;

    buffLenList[0] = packetDataLength;
    buffList[0] = cpssOsCacheDmaMalloc(buffLenList[0]);
    cpssOsMemCpy(buffList[0],packetDataPtr,packetDataLength);
    
    rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&pcktParams,buffList,buffLenList,1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfSdmaSyncTxPacketSend",rc);
    cpssOsCacheDmaFree(buffList[0]);
    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS gtAppDemoOamSdmaDualEndedLmTxPacketSend
(
    IN GT_U8    vid,
    IN GT_U8    *packetDataPtr,
    IN GT_U32   packetDataLength,   
    IN GT_U32   dstPortNum
)
{
    GT_STATUS                   rc;
    CPSS_DXCH_NET_TX_PARAMS_STC pcktParams;
    GT_U8                       *buffList[1];
    GT_U32                      buffLenList[1];
    GT_HW_DEV_NUM               hwDevNum; /* HW device number */

    cpssOsMemSet(&pcktParams, '\0', sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
    pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FORWARD_E;
    pcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT;
    pcktParams.dsaParam.commonParams.vid = (GT_U16)vid;
    pcktParams.packetIsTagged = GT_TRUE;
    rc = cpssDxChCfgHwDevNumGet(0,&hwDevNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet",rc);
    if(rc != GT_OK)
    {
        return rc;
    }
    pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.hwDevNum = hwDevNum;
    pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = dstPortNum;
    pcktParams.dsaParam.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_PORT_E;
    pcktParams.dsaParam.dsaInfo.forward.srcHwDev = hwDevNum+1;/* not our device */
    pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.forward.source.portNum = 63;
    pcktParams.dsaParam.dsaInfo.forward.isTrgPhyPortValid = GT_TRUE;
    pcktParams.dsaParam.dsaInfo.forward.dstEport = dstPortNum;
    pcktParams.dsaParam.dsaInfo.forward.origSrcPhyIsTrunk = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.forward.origSrcPhy.portNum = 0;
    pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    pcktParams.sdmaInfo.txQueue = GT_APP_DEMO_OAM_TX_QUEUE_NUM;

    buffLenList[0] = packetDataLength;
    buffList[0] = cpssOsCacheDmaMalloc(buffLenList[0]);
    cpssOsMemCpy(buffList[0],packetDataPtr,packetDataLength);
    
    rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&pcktParams,buffList,buffLenList,1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfSdmaSyncTxPacketSend",rc);
    cpssOsCacheDmaFree(buffList[0]);
    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamBillingCountingModeSet
*
* DESCRIPTION:  Set policers memory control mode, init each of the stages to 
*               billing count mode and flow ID based counting .
*               Set ingress 0 and egress stages and disable stage 1.
*
* INPUTS:      
*       None.
*                     
* OUTPUTS:      
*       None.    
*                                   
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*******************************************************************************/
GT_STATUS gtAppDemoOamBillingCountingModeSet(GT_VOID)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
                                            /* billing index configuration */
    CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC billingIndexCfg;                                            

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {    
        /* Set Policer 0 to use all three memories */
        rc = cpssDxChPolicerMemorySizeModeSet(devNum, CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_1_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerMemorySizeModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Set stage 0 */
        /* Set Counter Color Mode bit on Policer Control0 Register */
        rc = cpssDxCh3PolicerCountingColorModeSet(devNum,
                                                  CPSS_DXCH_POLICER_STAGE_INGRESS_0_E,
                                                  CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E);       
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh3PolicerCountingColorModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Set counting mode to billing */
        rc = cpssDxChPolicerCountingModeSet(devNum,
                                            CPSS_DXCH_POLICER_STAGE_INGRESS_0_E,
                                            CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Enable LM capture on this stage */
        rc = cpssDxChPolicerLossMeasurementCounterCaptureEnableSet(devNum,
                                                                   CPSS_DXCH_POLICER_STAGE_INGRESS_0_E,
                                                                   GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerLossMeasurementCounterCaptureEnableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Enable billing for trapped packets */
        rc = cpssDxChPolicerTrappedPacketsBillingEnableSet(devNum,
                                                           CPSS_DXCH_POLICER_STAGE_INGRESS_0_E,
                                                           GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerTrappedPacketsBillingEnableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Set stage to flow ID based counting */
        billingIndexCfg.billingFlowIdIndexBase = 0x0;
        billingIndexCfg.billingIndexMode = CPSS_DXCH_POLICER_BILLING_INDEX_MODE_FLOW_ID_E;
        billingIndexCfg.billingMaxFlowId = 2047;
        billingIndexCfg.billingMinFlowId = GT_APP_DEMO_OAM_BILLING_MIN_FLOW_ID;
        rc = cpssDxChPolicerFlowIdCountingCfgSet(devNum,
                                                 CPSS_DXCH_POLICER_STAGE_INGRESS_0_E,
                                                 &billingIndexCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerFlowIdCountingCfgSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Disable stage 1 */
        rc = cpssDxChPolicerCountingModeSet(devNum,
                                            CPSS_DXCH_POLICER_STAGE_INGRESS_1_E,
                                            CPSS_DXCH_POLICER_COUNTING_DISABLE_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Set egress stage */
        rc = cpssDxCh3PolicerCountingColorModeSet(devNum,
                                                  CPSS_DXCH_POLICER_STAGE_EGRESS_E,
                                                  CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E);       
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh3PolicerCountingColorModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Set counting mode to billing */
        rc = cpssDxChPolicerCountingModeSet(devNum,
                                            CPSS_DXCH_POLICER_STAGE_EGRESS_E,
                                            CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingModeSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Enable LM capture on this stage */
        rc = cpssDxChPolicerLossMeasurementCounterCaptureEnableSet(devNum,
                                                                   CPSS_DXCH_POLICER_STAGE_EGRESS_E,
                                                                   GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerLossMeasurementCounterCaptureEnableSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        } 
        /* Set stage to flow ID based counting */
        rc = cpssDxChPolicerFlowIdCountingCfgSet(devNum,
                                                 CPSS_DXCH_POLICER_STAGE_EGRESS_E,
                                                 &billingIndexCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerFlowIdCountingCfgSet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamBillingEntrySet
*
* DESCRIPTION:
*       Set Policer Billing Entry configuration for LM counting.
*       The entry is set to count packets and to insert mode for LM
*       counter capture
*       
* 
* INPUTS:
*       flowId              - The OAM flow ID (APPLICABLE RANGES: 1..2047)
*                             0 is reserverd.
*       stage               - policer stage type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBillingEntrySet
(
    IN GT_U32                           flowId,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage   
)
{
    GT_STATUS                               rc; /* function return code */
    GT_U8                                   devNum; /* device number */
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    billingCntr; /* Billing Counters Entry */

    cpssOsMemSet(&billingCntr, '\0', sizeof(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC));
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {  
        /* Flush internal Write Back Cache */
        rc = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, stage);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingWriteBackCacheFlush",rc);
        if(rc != GT_OK)
        {
            return rc;
        } 
        /* Set the billing counter to count packets and to insert mode
           for LM counter capture */
        billingCntr.billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E;
        billingCntr.lmCntrCaptureMode = CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_INSERT_E;
        billingCntr.packetSizeMode = CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E;
        rc = cpssDxCh3PolicerBillingEntrySet(devNum,
                                             stage,
                                             flowId - GT_APP_DEMO_OAM_BILLING_MIN_FLOW_ID,
                                             &billingCntr);   
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh3PolicerBillingEntrySet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }          
        /* Flush internal Write Back Cache */
        rc = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, stage);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingWriteBackCacheFlush",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}



/*******************************************************************************
 * Public API implementation
 ******************************************************************************/

/*******************************************************************************
 * OAM BFD AppDemo
 ******************************************************************************/

/*******************************************************************************
* gtAppDemoOamBfdInit
*
* DESCRIPTION:
*       This function does general initialization of PCL, TTI and OAM engines.
*       cpssInitSystem() must be called before calling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdInit
(
    GT_VOID
)
{
    GT_STATUS    rc; /* function return code */

    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclInitBfdIpv4();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclInitBfdMpls();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclRuleBfdIpv4(GT_APP_DEMO_OAM_PCL_BFD_IPV4_INDEX);   
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_TRUE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamExceptionConfigInit();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamTtiInitBfdMpls();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamTtiRuleBfdIpv4MplsLsp(GT_APP_DEMO_OAM_TTI_FIRST_INDEX); /* 18K */
    if(rc != GT_OK)   
         return rc;    
    rc =  gtAppDemoOamHashInitGen();
    if(rc != GT_OK)   
         return rc; 
    rc =  gtAppDemoOamHashInitBfdIpv4();
    if(rc != GT_OK)   
         return rc; 
    rc =  gtAppDemoOamHashInitBfdIpv4MplsLsp();
    if(rc != GT_OK)   
         return rc;
#ifdef ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    }
#endif 
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamBfdIpv4RxFlowSet
*
* DESCRIPTION:
*       Set OAM BFD IPv4 Rx flow.
*       The function configs a PCL rule and OAM entry accroding to the flow ID
*       and two last bytes of source IP address.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       srcIpAddrThirdByte  - The 3rd byte source IP address (192.168.XXX.001).
*       srcIpAddrLastByte   - The last byte source IP address (192.168.001.XXX).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdIpv4RxFlowSet
(
    IN GT_U32 flowId,
    IN GT_U8 srcIpAddrThirdByte,
    IN GT_U8 srcIpAddrLastByte
)
{
    GT_STATUS rc; /* function return code */

    rc = gtAppDemoOamPclRuleBfdIpv4Flow(flowId*2+2,flowId, srcIpAddrThirdByte, srcIpAddrLastByte);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetBfd(flowId, GT_TRUE);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamBfdIpv4MplsLspRxFlowSet
*
* DESCRIPTION:
*       Set OAM BFD IPv4 MPLS Rx flow.
*       The function configs a PCL rule and OAM entry accroding to the flow ID
*       and two last bytes of source IP address.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       srcIpAddrThirdByte  - The 3rd byte source IP address (192.168.XXX.001).
*       srcIpAddrLastByte   - The last byte source IP address (192.168.001.XXX).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdIpv4MplsLspRxFlowSet
(
    IN GT_U32 flowId,
    IN GT_U8 srcIpAddrThirdByte,
    IN GT_U8 srcIpAddrLastByte
)
{
    GT_STATUS rc; /* function return code */
    rc = gtAppDemoOamPclRuleBfdIpv4MplsLspFlow(flowId*3,flowId, srcIpAddrThirdByte, srcIpAddrLastByte);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetBfd(flowId, GT_TRUE);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamBfdIpv4RxFlowDel
*
* DESCRIPTION:
*       Disable OAM BFD IPv4 Rx flow.
*       The function diasbles a PCL rule and OAM entry accroding to the flow ID.
*
* INPUTS:
*       flowId          - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdIpv4RxFlowDel
(
    IN GT_U32 flowId
)
{
    GT_STATUS rc; /* function return code */
    rc = gtAppDemoOamPclDisableRule(flowId*2+2);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetBfd(flowId, GT_FALSE);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamBfdIpv4MplsLspRxFlowDel
*
* DESCRIPTION:
*       Disable OAM BFD IPv4 MPLS Rx flow.
*       The function diasbles a PCL rule and OAM entry accroding to the flow ID.
*
* INPUTS:
*       flowId          - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdIpv4MplsLspRxFlowDel
(
    IN GT_U32 flowId
)
{
    GT_STATUS rc; /* function return code */
    rc = gtAppDemoOamPclDisableRule(flowId*3);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetBfd(flowId, GT_FALSE);
    return rc;
}
/*******************************************************************************
* gtAppDemoOamBfdIpv4TxFlowSet
*
* DESCRIPTION:
*       Set OAM BFD IPv4 Tx flow.
*       The function adds a BFD IPv4 packet with the input source IP address 
*       third and fourth bytes with the following paramaters:
*       dst MAC             = 00:00:00:00:00:02
*       src MAC             = 00:00:00:00:00:01
*       TTL                 = 255
*       dst IP              = 192.168.1.100
*       src IP              = 192.168.xxx.xxx
*       dst port            = 0x0EC8
*       src port            = 0xC000
*       My Discriminator    = 0x0000AB01
*       Your Discriminator  = 0x0000AB02
*       Desired Min TX Interval         = 0x00000CE4 (3.3ms)
*       Required Min RX Interval        = 0x00000CE4 (3.3ms)
*       Required Min Echo RX Interval   = 0x00000CE4 (3.3ms)
*
* INPUTS:
*       srcIpAddrThirdByte     - The 3rd byte source IP address (192.168.XXX.001).
*       srcIpAddrLastByte      - The last byte source IP address (192.168.001.XXX).
*       dstPortNum             - Destination port number. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdIpv4TxFlowSet
(
    IN GT_U8    srcIpAddrThirdByte,
    IN GT_U8    srcIpAddrLastByte,
    IN GT_U32   dstPortNum
)
{
    GT_STATUS rc; /* function return code */
    GT_U8                                   packetData[66] = 
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x08, 0x00, 0x45, 0x00, 
    0x00, 0x2E, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x11,
    0x38, 0x09, 0xC0, 0xA8, 0x00, 0x00, 0xC0, 0xA8,
    0x01, 0x64, 0xC0, 0x00, 0x0E, 0xC8, 0x00, 0x08,
    0x36, 0x80, 0x20, 0xC0, 0x00, 0x08, 0x00, 0x00,
    0xAB, 0x01, 0x00, 0x00, 0xAB, 0x02, 0x00, 0x00,
    0x0C, 0xE4, 0x00, 0x00, 0x0C, 0xE4, 0x00, 0x00,
    0x0C, 0xE4}; /*,0x00, 0x00, 0x00, 0x00};*/

    packetData[28] = srcIpAddrThirdByte;
    packetData[29] = srcIpAddrLastByte;
    rc = gtAppDemoOamSdmaTxGeneratorPacketAdd(1,packetData, 66, dstPortNum);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamBfdIpv4TxFlowDel
*
* DESCRIPTION:
*       Remove OAM BFD IPv4 Tx flow.
*       The function removes the specific flow, given by the packet ID from the 
*       Tx queue.
*
* INPUTS:
*       packetId        - packet identification number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdIpv4TxFlowDel
(
    IN GT_U32 packetId
)
{
    GT_STATUS rc; /* function return code */
    rc = gtAppDemoOamSdmaTxGeneratorPacketRemove(packetId);
    return rc;
}

/*******************************************************************************
 * OAM BFD over MPLS-TP AppDemo
 ******************************************************************************/
/*******************************************************************************
* gtAppDemoOamBfdMplsTpInit
*
* DESCRIPTION:
*       This function does general initialization of PCL, TTI and OAM engines.
*       cpssInitSystem() must be called before calling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdMplsTpInit
(
    GT_VOID
)
{
    GT_STATUS    rc; /* function return code */

    rc = gtAppDemoOamTtiInitBfdMplsTp();
    if(rc != GT_OK)   
         return rc;

    rc = gtAppDemoOamTtiRuleBfdMplsTp(GT_APP_DEMO_OAM_TTI_FIRST_INDEX); /* 18K */
    if(rc != GT_OK)   
         return rc; 

    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;

    rc = gtAppDemoOamPclInitBfdMplsTp();
    if(rc != GT_OK)   
         return rc;

    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_TRUE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamExceptionConfigInit();
    if(rc != GT_OK)   
         return rc;
   
    rc =  gtAppDemoOamHashInitGen();
    if(rc != GT_OK)   
         return rc; 
    rc =  gtAppDemoOamHashInitBfdMplsTp();
    if(rc != GT_OK)   
         return rc;
#ifdef ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    }
#endif 
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamBfdMplsTpRxFlowSet
*
* DESCRIPTION:
*       Set OAM BFD MPLS-TP Rx flow.
*       The function configs a PCL rule and OAM entry accroding to the flow ID.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamBfdMplsTpRxFlowSet
(
    IN GT_U32 flowId
)
{
    GT_STATUS rc; /* function return code */
    rc = gtAppDemoOamPclRuleBfdMplsTpFlow(flowId - 1, flowId);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetBfd(flowId, GT_TRUE);
    return rc;
}



/*******************************************************************************
* gtAppDemoOamKeepAliveExceptionGet
*
* DESCRIPTION:
*       Display OAM keepalive exception status and counter.
*       The status is of a specific flow.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamKeepAliveExceptionGet
(
    IN  GT_U32                          flowId,
    IN  CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage
)
{
    GT_U32          counterValue; /* OAM exception counter value and bitmap status */
    GT_STATUS       rc; /* function return code */
    GT_U8           devNum; /* device number */

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamExceptionCounterGet(devNum,
                                            stage,
                                            CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E,
                                            &counterValue);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionCounterGet",rc);
        if(rc != GT_OK)
        {    
             return rc;
        }
        cpssOsPrintf("counterValue = 0x%x\n",counterValue);
    
        flowId = flowId/32;
        rc = cpssDxChOamExceptionStatusGet(devNum,
                                           stage,
                                           CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_AGING_E,
                                           flowId,
                                           &counterValue );
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionStatusGet",rc);
        if(rc != GT_OK)
        {    
             return rc;
        }
        cpssOsPrintf("bitMap = 0x%08x\n",counterValue);
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamHashExceptionGet
*
* DESCRIPTION:
*       Display OAM hash exception status and counter.
*       The status is of a specific flow.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamHashExceptionGet
(
    IN GT_U32 flowId
)
{
    GT_U32          counterValue; /* OAM exception counter value and bitmap status */
    GT_STATUS       rc; /* function return code */
    GT_U8           devNum; /* device number */
    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        rc = cpssDxChOamExceptionCounterGet(devNum,
                                            CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                            CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E,
                                            &counterValue);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionCounterGet",rc);
        if(rc != GT_OK)
        {    
             return rc;
        }
        cpssOsPrintf("counterValue = 0x%x\n",counterValue);
    
        flowId = flowId/32;
        rc = cpssDxChOamExceptionStatusGet(devNum,
                                           CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                           CPSS_DXCH_OAM_EXCEPTION_TYPE_KEEPALIVE_INVALID_HASH_E,
                                           flowId,
                                           &counterValue );
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionStatusGet",rc);
        if(rc != GT_OK)
        {    
             return rc;
        }
        cpssOsPrintf("bitMap = 0x%08x\n",counterValue);
    }
    return GT_OK;
}

/*******************************************************************************
 * OAM CCM AppDemo
 ******************************************************************************/

/*******************************************************************************
* gtAppDemoOamCcmInit
*
* DESCRIPTION:
*       This function does general initialization of PCL and OAM engines.
*       cpssInitSystem() must be called before caling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamCcmInit
(
    GT_VOID
)
{
    GT_STATUS    rc; /* function return code */

    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclInitCfm();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_TRUE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamExceptionConfigInit();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitCcm();
    if(rc != GT_OK)   
         return rc;
    rc =  gtAppDemoOamHashInitGen();
    if(rc != GT_OK)   
         return rc; 
    rc =  gtAppDemoOamHashInitCfm();
    if(rc != GT_OK)   
         return rc;
#ifdef ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    }
#endif    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamCcmRxFlowSet
*
* DESCRIPTION:
*       Set OAM CCM Rx flow.
*       The function configs a PCL rule and OAM entry accroding to the flow ID,
*       meg level and source port number.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       portNum             - The Source port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamCcmRxFlowSet
(
    IN GT_U32 flowId,
    IN GT_U32 megLevel, 
    IN GT_U8  portNum,
    IN GT_BOOL drop
)
{
    GT_STATUS rc; /* function return code */

    rc = gtAppDemoOamPclRuleCfmFlow(flowId*2-2,
                                    flowId,
                                    portNum,
                                    0x01,
                                    flowId,
                                    drop);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetCcm(flowId, GT_TRUE, megLevel, portNum);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamCcmFlowDel
*
* DESCRIPTION:
*       Disable OAM CCM flow.
*       The function diasbles a PCL rule and OAM entry accroding to the flow ID.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamCcmFlowDel
(
    IN GT_U32 flowId
)
{
    GT_STATUS rc; /* function return code */
    rc = gtAppDemoOamPclDisableRule(flowId*2-2);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetCcm(flowId, GT_FALSE, 0x0, 0x0);
    return rc;
}


/*******************************************************************************
 * OAM Up MEP CCM AppDemo
 ******************************************************************************/

/*******************************************************************************
* gtAppDemoOamUpMepCcmInit
*
* DESCRIPTION:
*       This function does general initialization of PCL and OAM engines.
*       cpssInitSystem() must be called before caling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamUpMepCcmInit
(
    GT_VOID
)
{
    GT_STATUS    rc; /* function return code */

    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamEPclInitGen();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclInitCfm();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamEPclInitCfm();
    if(rc != GT_OK)   
         return rc;  
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_TRUE,
                               GT_TRUE);
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                               GT_TRUE,
                               GT_TRUE);
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamExceptionConfigInit();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitUpMepCcm();
    if(rc != GT_OK)   
         return rc;
    rc =  gtAppDemoOamHashInitGen();
    if(rc != GT_OK)   
         return rc; 
    rc =  gtAppDemoOamHashInitCfm();
    if(rc != GT_OK)   
         return rc;
#ifdef ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    }
#endif    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamUpMepCcmRxFlowSet
*
* DESCRIPTION:
*       Set OAM Up MEP CCM Rx flow.
*       The function configs a PCL rule and OAM entry accroding to the flow ID,
*       meg level and source port number.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       portNum             - The Source port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamUpMepCcmRxFlowSet
(
    IN GT_U32 flowId,
    IN GT_U32 megLevel, 
    IN GT_U8 portNum
)
{
    GT_STATUS rc; /* function return code */

    rc = gtAppDemoOamPclRuleUpMepCfmFlow(flowId*2-2,
                                         flowId,
                                         portNum);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetUpMepCcm(flowId,
                                      CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                      GT_TRUE,
                                      megLevel,
                                      portNum);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetUpMepCcm(flowId,
                                      CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                      GT_TRUE,
                                      0 /* not relevant */,
                                      0 /* not relevant */);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamUpMepCcmFlowDel
*
* DESCRIPTION:
*       Disable OAM CCM flow.
*       The function diasbles a PCL rule and OAM entry accroding to the flow ID.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamUpMepCcmFlowDel
(
    IN GT_U32 flowId
)
{
    GT_STATUS rc; /* function return code */
    rc = gtAppDemoOamPclDisableRule(flowId*2-2);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamPclDisableRule(flowId*2-2 + GT_APP_DEMO_OAM_UP_MEP_PCL_EGRESS_INDEX_BASE);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetUpMepCcm(flowId,
                                      CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                      GT_FALSE,
                                      0 /* not relevant */,
                                      0 /* not relevant */);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetUpMepCcm(flowId,
                                      CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                      GT_FALSE,
                                      0 /* not relevant */,
                                      0 /* not relevant */);
    return rc;
}
/*******************************************************************************
 * OAM CCM over MPLS-TP AppDemo
 ******************************************************************************/
/*******************************************************************************
* gtAppDemoOamCcmMplsTpInit
*
* DESCRIPTION:
*       This function does general initialization of PCL, TTI and OAM engines.
*       cpssInitSystem() must be called before calling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamCcmMplsTpInit
(
    GT_VOID
)
{
    GT_STATUS    rc; /* function return code */

    rc = gtAppDemoOamTtiInitCfmMplsTp();
    if(rc != GT_OK)   
         return rc;   
    rc = gtAppDemoOamTtiRuleCfmMplsTp(GT_APP_DEMO_OAM_TTI_FIRST_INDEX);
    if (rc != GT_OK) 
        return rc;

    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclInitCfmMplsTp();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_TRUE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamExceptionConfigInit();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitCcm();
    if(rc != GT_OK)   
         return rc;

    rc =  gtAppDemoOamHashInitGen();
    if(rc != GT_OK)   
         return rc; 
    rc =  gtAppDemoOamHashInitCfmMplsTp();
    if(rc != GT_OK)   
         return rc;
#ifdef ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    } 
#endif
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamCcmMplsTpRxFlowSet
*
* DESCRIPTION:
*       Set OAM CCM MPLS-TP Rx flow.
*       The function configs a PCL rule and OAM entry accroding to the flow ID,
*       meg level and source port number.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       portNum             - The Source port number
*       mplsLable0          - The MPLS lable 0 (transport lable).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamCcmMplsTpRxFlowSet
(
    IN GT_U32   flowId,
    IN GT_U32   megLevel, 
    IN GT_U32   portNum,
    IN GT_U32   mplsLable0
)
{
    GT_STATUS rc; /* function return code */
    rc = gtAppDemoOamPclRuleCfmMplsTpFlow(flowId - 1, flowId, mplsLable0);
    if(rc != GT_OK)
          return rc;
    rc = gtAppDemoOamEntrySetCcm(flowId, GT_TRUE, megLevel, portNum);
    return rc;
}

/*******************************************************************************
 * OAM LM AppDemo
 ******************************************************************************/
/*******************************************************************************
* gtAppDemoOamLmInit
*
* DESCRIPTION:
*       This function does general initialization of PCL, Policer and OAM engines.
*       cpssInitSystem() must be called before caling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmInit
(
    GT_VOID
)
{
    GT_STATUS    rc; /* function return code */

    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamEPclInitGen();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclInitCfm();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamEPclInitCfm();
    if(rc != GT_OK)   
         return rc;    
    rc = gtAppDemoOamPclInitLmData();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_FALSE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;    
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                               GT_FALSE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamExceptionConfigInit();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitLm();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamBillingCountingModeSet();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamSdmaTxGeneratorRateSet(3300000);
    if(rc != GT_OK)   
         return rc;
    /* Enable SDMA generator in continuous mode */
    rc = gtAppDemoOamSdmaTxGeneratorEnable(GT_FALSE, 0);
    if(rc != GT_OK)   
         return rc;
#ifdef ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    }
#else
    if(gtAppDemoOAmSrvCpuInited == GT_FALSE)
    {
		CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC    sysParams;/* OAM Service CPU system paramaters*/
	
        cpssOsMemSet(&sysParams, '\0', sizeof(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC));
        rc =  cpssDxChOamSrvSystemInit(0, &sysParams);
        if(rc != GT_OK)   
             return rc;

        gtAppDemoOAmSrvCpuInited = GT_TRUE;
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamLmRxFlowSet
*
* DESCRIPTION:
*       Set OAM LM Rx flow support.
*       The function configs a PCL rule and OAM entry accroding to the flow ID
*       meg level and source port number to classify LMM/LMR frames in Ingress.
*       It also configs a PCL rule to classify and count the flows data, that is
*       used for the lost measurment test, and a billing counter according to 
*       the flow ID. 
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       cfmPortNum          - The Source port number for the LMM/LMR frames.
*       dataPortNum         - The Source port number the data frames.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*       dataSrcMacAddr      - the end of src mac: 00:00:00:00:00:XX for data pcl rule.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmRxFlowSet
(
    IN GT_U32   flowId,
    IN GT_U32   megLevel, 
    IN GT_U8    cfmPortNum,
    IN GT_U8    dataPortNum,
    IN GT_U8    srcMacAddr,
    IN GT_U8    dataSrcMacAddr,
    IN GT_U32   vlanId
)
{
    GT_STATUS rc; /* function return code */
    CPSS_DXCH_OAM_SRV_LM_CONFIG_STC lmConfig;

    /* set PCL rule */
    rc = gtAppDemoOamPclRuleCfmFlow(flowId*2-2,
                                    flowId, 
                                    cfmPortNum,
                                    srcMacAddr,
                                    vlanId,
                                    GT_FALSE);
    if (rc != GT_OK) 
        return rc;
    /* set OAM entry */
    rc = gtAppDemoOamEntrySetLm(flowId,
                                megLevel,
                                cfmPortNum,
                                CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX);
    if (rc != GT_OK) 
        return rc;
    /* set PCL rule */
    rc = gtAppDemoOamPclRuleLmData(CPSS_PCL_DIRECTION_INGRESS_E,
                                   flowId*2-2 + GT_APP_DEMO_OAM_LM_DATA_PCL_INDEX_BASE,
                                   flowId,
                                   dataPortNum,
                                   dataSrcMacAddr,
                                   vlanId);
    if (rc != GT_OK) 
        return rc;
    /* set billing counter */
    rc = gtAppDemoOamBillingEntrySet(flowId,CPSS_DXCH_POLICER_STAGE_INGRESS_0_E); 
    if (rc != GT_OK) 
        return rc;

    lmConfig.countersEnable = GT_TRUE;
    rc = cpssDxChOamSrvLmConfigSet(0,flowId, &lmConfig);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLmConfigSet",rc);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLmTxFlowSet
*
* DESCRIPTION:
*       Set OAM LM Tx flow support.
*       The function configs a PCL rule and OAM entry accroding to the flow ID
*       and meg level to classify LMM/LMR frames in Egress.
*       It also configs a PCL rule to classify and count the flows data, that is
*       used for the lost measurment test, and a billing counter according to 
*       the flow ID. 
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       dataPortNum         - port number for data packets
*       dstMacAddr          - the end of destination mac: 00:00:00:00:00:XX.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*       dataSrcMacAddr      - the end of src mac: 00:00:00:00:00:XX for data pcl rule.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmTxFlowSet
(
    IN GT_U32   flowId,
    IN GT_U32   megLevel,
    IN GT_U8    dataPortNum,
    IN GT_U8    dstMacAddr,
    IN GT_U8    srcMacAddr,
    IN GT_U8    dataSrcMacAddr,
    IN GT_U32   vlanId,
	IN GT_U8	cfmPortNum
)
{
    GT_STATUS   rc; /* function return code */ 
    /* Set PCL rule */
    rc = gtAppDemoOamEPclRuleCfmFlow(GT_APP_DEMO_OAM_LM_PCL_EGRESS_INDEX_BASE + flowId*2-2,
                                     flowId, 
                                     dstMacAddr, 
                                     srcMacAddr,
                                     vlanId,
									 cfmPortNum);
    if (rc != GT_OK) 
        return rc;
    /* set OAM entry */
    rc = gtAppDemoOamEntrySetLm(flowId,
                                megLevel,
                                63 /* CPU port */,
                                CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX);
    if (rc != GT_OK) 
        return rc;
    /* set PCL rule */
    rc = gtAppDemoOamPclRuleLmData(CPSS_PCL_DIRECTION_EGRESS_E,
                                   flowId*2-2 + GT_APP_DEMO_OAM_LM_DATA_PCL_EGRESS_INDEX_BASE,
                                   flowId,
                                   dataPortNum,
                                   dataSrcMacAddr,
                                   vlanId);
    if (rc != GT_OK) 
        return rc;
    /* set billing counter */
    rc = gtAppDemoOamBillingEntrySet(flowId,CPSS_DXCH_POLICER_STAGE_EGRESS_E); 
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLmTxFlowSend
*
* DESCRIPTION:
*       This function sends a single LMM or starts tramsmiting continuously,
*       according to generatorEnable.
*       The packets are sent with the following paramaters:
*       dst MAC             = 00:00:00:00:00:01
*       src MAC             = 00:00:00:00:00:02
*       VLAN                = vlanId
*       meg level           = 0x2
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       generatorEnable     - enable Tx SDMA generator mode.
*       dstPortNum          - Destination port number.      
*       dstMacAddr          - the end of destination mac: 00:00:00:00:00:XX.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmTxFlowSend
(
    IN GT_U8    vlanId,
    IN GT_BOOL  generatorEnable,
    IN GT_U32   dstPortNum,
    IN GT_U8    dstMacAddr,
    IN GT_U8    srcMacAddr
)
{
    GT_STATUS   rc; /* function return code */
    GT_U8       packetData[64] = 
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 
    0x89, 0x02, 0x40, 0x2B, 0x00, 0x0C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    packetData[5] = dstMacAddr;
    packetData[11] = srcMacAddr;
    packetData[15] = vlanId;
    if(generatorEnable)
    {
        rc = gtAppDemoOamSdmaTxGeneratorPacketAdd(vlanId, packetData, 64, dstPortNum);
    }
    else
    {
        rc = gtAppDemoOamSdmaTxPacketSend(vlanId, packetData, 64, dstPortNum);        
    }
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLmDualEndedInit
*
* DESCRIPTION:
*       This function does general initialization of PCL, Policer and OAM engines.
*       cpssInitSystem() must be called before caling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
#if 0
GT_STATUS gtAppDemoOamLmDualEndedInit
(
    GT_VOID
)
{
    GT_STATUS    rc; /* function return code */

    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamEPclInitGen();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclInitCfm();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamEPclInitCfm();
    if(rc != GT_OK)   
         return rc;    
    rc = gtAppDemoOamPclInitLmData();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_FALSE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;    
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                               GT_FALSE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamExceptionConfigInit();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitLm();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamBillingCountingModeSet();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamSdmaTxGeneratorRateSet(3300000);
    if(rc != GT_OK)   
         return rc;
    /* Enable SDMA generator in continuous mode */
    rc = gtAppDemoOamSdmaTxGeneratorEnable(GT_FALSE, 0);
    if(rc != GT_OK)   
         return rc;
#ifdef ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    }
#else
    if(gtAppDemoOAmSrvCpuInited == GT_FALSE)
    {
		CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC    sysParams;/* OAM Service CPU system paramaters*/
	
        cpssOsMemSet(&sysParams, '\0', sizeof(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC));
        rc =  cpssDxChOamSrvSystemInit(0, &sysParams);
        if(rc != GT_OK)   
             return rc;

        gtAppDemoOAmSrvCpuInited = GT_TRUE;
    }
#endif
    return GT_OK;
}
#endif

/*******************************************************************************
* gtAppDemoOamLmDualEndedRxFlowSet
*
* DESCRIPTION:
*       Set OAM LM Rx flow support.
*       The function configs a PCL rule and OAM entry accroding to the flow ID
*       meg level and source port number to classify LMM/LMR frames in Ingress.
*       It also configs a PCL rule to classify and count the flows data, that is
*       used for the lost measurment test, and a billing counter according to 
*       the flow ID. 
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       cfmPortNum          - The Source port number for the LMM/LMR frames.
*       dataPortNum         - The Source port number the data frames.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*       dataSrcMacAddr      - the end of src mac: 00:00:00:00:00:XX for data pcl rule.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmDualEndedRxFlowSet
(
    IN GT_U32   flowId,
    IN GT_U32   megLevel, 
    IN GT_U8    cfmPortNum,
    IN GT_U8    dataPortNum,
    IN GT_U8    srcMacAddr,
    IN GT_U8    dataSrcMacAddr,
    IN GT_U32   vlanId
)
{
    GT_STATUS rc; /* function return code */
    CPSS_DXCH_OAM_SRV_LM_CONFIG_STC lmConfig;

    /* set PCL rule */
    rc = gtAppDemoOamPclRuleCfmFlow(flowId*2-2,
                                    flowId, 
                                    cfmPortNum,
                                    srcMacAddr,
                                    vlanId,
                                    GT_FALSE);
    if (rc != GT_OK) 
        return rc;
    /* set OAM entry */
    rc = gtAppDemoOamEntrySetDualEndedLm(flowId,
                                megLevel,
                                cfmPortNum,
                                CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                GT_APP_DEMO_OAM_INGRESS_PTP_DE_LM_OFFSET_INDEX);
    if (rc != GT_OK) 
        return rc;
    /* set PCL rule */
    rc = gtAppDemoOamPclRuleLmData(CPSS_PCL_DIRECTION_INGRESS_E,
                                   flowId*2-2 + GT_APP_DEMO_OAM_LM_DATA_PCL_INDEX_BASE,
                                   flowId,
                                   dataPortNum,
                                   dataSrcMacAddr,
                                   vlanId);
    if (rc != GT_OK) 
        return rc;
    /* set billing counter */
    rc = gtAppDemoOamBillingEntrySet(flowId,CPSS_DXCH_POLICER_STAGE_INGRESS_0_E); 
    if (rc != GT_OK) 
        return rc;

    lmConfig.countersEnable = GT_TRUE;
    rc = cpssDxChOamSrvLmConfigSet(0,flowId, &lmConfig);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLmConfigSet",rc);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLmDualEndedTxFlowSet
*
* DESCRIPTION:
*       Set OAM LM Tx flow support.
*       The function configs a PCL rule and OAM entry accroding to the flow ID
*       and meg level to classify LMM/LMR frames in Egress.
*       It also configs a PCL rule to classify and count the flows data, that is
*       used for the lost measurment test, and a billing counter according to 
*       the flow ID. 
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       dataPortNum         - port number for data packets
*       dstMacAddr          - the end of destination mac: 00:00:00:00:00:XX.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*       dataSrcMacAddr      - the end of src mac: 00:00:00:00:00:XX for data pcl rule.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmDualEndedTxFlowSet
(
    IN GT_U32   flowId,
    IN GT_U32   megLevel,
    IN GT_U8    dataPortNum,
    IN GT_U8    dstMacAddr,
    IN GT_U8    srcMacAddr,
    IN GT_U8    dataSrcMacAddr,
    IN GT_U32   vlanId,
	IN GT_U8	cfmPortNum
)
{
    GT_STATUS   rc; /* function return code */ 
    
    /* Set PCL rule */
    rc = gtAppDemoOamEPclRuleCfmFlow(GT_APP_DEMO_OAM_LM_PCL_EGRESS_INDEX_BASE + flowId*2-2,
                                     flowId, 
                                     dstMacAddr, 
                                     srcMacAddr,
                                     vlanId,
									 cfmPortNum);
    if (rc != GT_OK) 
        return rc;
    /* set OAM entry */
    rc = gtAppDemoOamEntrySetDualEndedLm(flowId,
                                megLevel,
                                63 /* CPU port */,
                                CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                GT_APP_DEMO_OAM_EGRESS_PTP_DE_LM_OFFSET_INDEX);
    if (rc != GT_OK) 
        return rc;
    /* set PCL rule */
    rc = gtAppDemoOamPclRuleLmData(CPSS_PCL_DIRECTION_EGRESS_E,
                                   flowId*2-2 + GT_APP_DEMO_OAM_LM_DATA_PCL_EGRESS_INDEX_BASE,
                                   flowId,
                                   dataPortNum,
                                   dataSrcMacAddr,
                                   vlanId);
    if (rc != GT_OK) 
        return rc;
    
    /* set billing counter */
    rc = gtAppDemoOamBillingEntrySet(flowId,CPSS_DXCH_POLICER_STAGE_EGRESS_E); 
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLmDualEndedTxFlowSend
*
* DESCRIPTION:
*       This function sends a single LMM or starts tramsmiting continuously,
*       according to generatorEnable.
*       The packets are sent with the following paramaters:
*       dst MAC             = 00:00:00:00:00:01
*       src MAC             = 00:00:00:00:00:02
*       VLAN                = vlanId
*       meg level           = 0x2
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       generatorEnable     - enable Tx SDMA generator mode.
*       dstPortNum          - Destination port number.      
*       dstMacAddr          - the end of destination mac: 00:00:00:00:00:XX.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*       rxFcb               - the first counter from 4 LM counters (offset 80)
*       txFcb               - the second counter from 4 LM counters (offset 84)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmDualEndedTxFlowSend
(
    IN GT_U8    vlanId,
    IN GT_BOOL  generatorEnable,
    IN GT_U32   dstPortNum,
    IN GT_U8    dstMacAddr,
    IN GT_U8    srcMacAddr,
    IN GT_U32   rxFcb,
    IN GT_U32   txFcb
)
{

    GT_STATUS   rc; /* function return code */
    GT_U8       packetData[112] = 
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 
    0x89, 0x02, 
    0x40, 0x01, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
    /*meg id-48B*/ 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    /*4 LM counters*/
    0x99, 0x99, 0x99, 0x99, 0x88, 0x88, 0x88, 0x88,
    0x77, 0x77, 0x77, 0x77, 0x66, 0x66, 0x66, 0x66,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
    };

    packetData[5] = dstMacAddr;
    packetData[11] = srcMacAddr;
    packetData[15] = vlanId;
    packetData[80] = (GT_U8)((rxFcb >> 24) & 0xFF);
    packetData[81] = (GT_U8)((rxFcb >> 16) & 0xFF);
    packetData[82] = (GT_U8)((rxFcb >> 8) & 0xFF);
    packetData[83] = (GT_U8)(rxFcb & 0xFF);
    packetData[84] = (GT_U8)((txFcb >> 24) & 0xFF);
    packetData[85] = (GT_U8)((txFcb >> 16) & 0xFF);
    packetData[86] = (GT_U8)((txFcb >> 8) & 0xFF);
    packetData[87] = (GT_U8)(txFcb & 0xFF);
    if(generatorEnable)
    {
        rc = gtAppDemoOamSdmaTxGeneratorPacketAdd(vlanId, packetData, 64, dstPortNum);
    }
    else
    {
        rc = gtAppDemoOamSdmaTxPacketSend(vlanId, packetData, 112, dstPortNum);
    }
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLmRxFlowGet
*
* DESCRIPTION:
*       Get OAM LM rx flow configuration from the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmRxFlowGet
(
    IN GT_U32   flowId
)
{
    GT_STATUS                       rc;
    CPSS_DXCH_OAM_SRV_LM_CONFIG_STC lmConfig;

    rc = cpssDxChOamSrvLmConfigGet(0, flowId, &lmConfig);
    if (rc != GT_OK) 
        return rc;
    cpssOsPrintf("countersEnable = %s\n",(lmConfig.countersEnable?"TRUE":"FALSE"));
    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamLmStatCntrGet
*
* DESCRIPTION:
*       Get OAM LM Rx flow statistical counters from the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       clrOnRead           - enable counter clear
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmStatCntrGet 
(
    IN GT_U32   flowId,
    IN GT_BOOL  clrOnRead
)
{
    GT_STATUS                           rc;
    CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC   lmCounters;

    cpssOsMemSet(&lmCounters, '\0', sizeof(CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC));
    rc = cpssDxChOamSrvLmStatisticalCountersGet(0, flowId, clrOnRead, &lmCounters); 
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLmStatisticalCountersGet",rc);
    if(rc != GT_OK)
     return rc;       
    
    cpssOsPrintf("txFrameCntForward = %u = 0x%08x\n",lmCounters.basicCounters.txFrameCntForward, lmCounters.basicCounters.txFrameCntForward);
    cpssOsPrintf("rxFrameCntForward = %u = 0x%08x\n",lmCounters.basicCounters.rxFrameCntForward, lmCounters.basicCounters.rxFrameCntForward);
    cpssOsPrintf("txFrameCntBackward = %u = 0x%08x\n",lmCounters.basicCounters.txFrameCntBackward, lmCounters.basicCounters.txFrameCntBackward);
    cpssOsPrintf("rxFrameCntBackward = %u = 0x%08x\n",lmCounters.basicCounters.rxFrameCntBackward, lmCounters.basicCounters.rxFrameCntBackward);
    cpssOsPrintf("farEndFrameLoss  = %u = 0x%08x\n",lmCounters.basicCounters.farEndFrameLoss, lmCounters.basicCounters.farEndFrameLoss);
    cpssOsPrintf("nearEndFrameLoss = %u = 0x%08x\n",lmCounters.basicCounters.nearEndFrameLoss, lmCounters.basicCounters.nearEndFrameLoss);
    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamLmStatCntrClr
*
* DESCRIPTION:
*       Clear OAM LM Rx flow statistical counters at the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLmStatCntrClr
(
    IN GT_U32 flowId
)
{
    GT_STATUS   rc;/* function return code */
    rc = cpssDxChOamSrvLmStatisticalCountersClear(0, flowId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLbStatisticalCountersClr",rc);
    return rc;  
}

/*******************************************************************************
 * OAM DM AppDemo
 ******************************************************************************/
/*******************************************************************************
* gtAppDemoOamDmInit
*
* DESCRIPTION:
*       This function does general initialization of PCL, Policer and OAM engines.
*       cpssInitSystem() must be called before caling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamDmInit
(
    GT_VOID
)
{
    GT_STATUS    rc; /* function return code */


    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;

    rc = gtAppDemoOamEPclInitGen();
    if(rc != GT_OK)   
         return rc;

    rc = gtAppDemoOamPclInitCfm();
    if(rc != GT_OK)   
         return rc;

    rc = gtAppDemoOamEPclInitCfm();
    if(rc != GT_OK)   
         return rc;    

    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_FALSE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;    

    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                               GT_FALSE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc;

    rc = gtAppDemoOamExceptionConfigInit();
    if(rc != GT_OK)   
         return rc;

    rc = gtAppDemoOamHwInitDm();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamBillingCountingModeSet();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamSdmaTxGeneratorRateSet(3300000);
    if(rc != GT_OK)   
         return rc;
    /* Enable SDMA generator in continuous mode */
    rc = gtAppDemoOamSdmaTxGeneratorEnable(GT_FALSE, 0);
    if(rc != GT_OK)   
         return rc;
#ifdef ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    }
#else
    if(gtAppDemoOAmSrvCpuInited == GT_FALSE)
    {
		CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC    sysParams;/* OAM Service CPU system paramaters*/
	
        cpssOsMemSet(&sysParams, '\0', sizeof(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC));
        rc =  cpssDxChOamSrvSystemInit(0, &sysParams);
        if(rc != GT_OK)   
             return rc;

        gtAppDemoOAmSrvCpuInited = GT_TRUE;
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamDmRxFlowSet
*
* DESCRIPTION:
*       Set OAM DM Rx flow support.
*       The function configs a PCL rule and OAM entry accroding to the flow ID
*       meg level and source port number to classify DMM/DMR/1DM frames in Ingress.
*       It also configs a PCL rule to classify and count the flows data, that is
*       used for the lost measurment test, and a billing counter according to 
*       the flow ID. 
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       cfmPortNum          - The Source port number for the LMM/LMR frames.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*       vlanId              - vlan
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamDmRxFlowSet
(
    IN GT_U32   flowId,
    IN GT_U32   megLevel,
    IN GT_U8    cfmPortNum,
    IN GT_U8    srcMacAddr,
    IN GT_U32   vlanId
)
{
    GT_STATUS rc; /* function return code */
    CPSS_DXCH_OAM_SRV_DM_CONFIG_STC dmConfig;

    /* set PCL rule */
    rc = gtAppDemoOamPclRuleCfmFlow(flowId*2-2,flowId, 
                                    cfmPortNum,
                                    srcMacAddr,
                                    vlanId,
                                    GT_FALSE);
    if (rc != GT_OK) 
        return rc;
    /* set OAM entry */
    rc = gtAppDemoOamEntrySetDm(flowId,
                                megLevel,
                                cfmPortNum,
                                CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                                GT_APP_DEMO_OAM_INGRESS_PTP_OFFSET_INDEX);
    if (rc != GT_OK) 
        return rc;

    dmConfig.countersEnable = GT_TRUE;
    rc = cpssDxChOamSrvDmConfigSet(0,flowId, &dmConfig);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvDmConfigSet",rc);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamDmTxFlowSet
*
* DESCRIPTION:
*       Set OAM DM Tx flow support.
*       The function configs a PCL rule and OAM entry accroding to the flow ID
*       and meg level to classify DMM/DMR/1DM frames in Egress.
*       It also configs a PCL rule to classify and count the flows data, that is
*       used for the lost measurment test, and a billing counter according to 
*       the flow ID. 
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       dstMacAddr          - the end of destination mac: 00:00:00:00:00:XX.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*       vlanId              - vlan
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamDmTxFlowSet
(
    IN GT_U32   flowId,
    IN GT_U32   megLevel,
    IN GT_U8    dstMacAddr,
    IN GT_U8    srcMacAddr,
    IN GT_U32   vlanId	
)
{
    GT_STATUS   rc; /* function return code */ 
    /* Set PCL rule */
    rc = gtAppDemoOamEPclRuleCfmFlow(GT_APP_DEMO_OAM_LM_PCL_EGRESS_INDEX_BASE + flowId*2-2,
    /*rc = gtAppDemoOamEPclRuleCfmFlow(0 + flowId*2-2,*/
                                     flowId, 
                                     dstMacAddr, 
                                     srcMacAddr,
                                     vlanId,
									 255);
    if (rc != GT_OK) 
        return rc;
    /* set OAM entry */
    rc = gtAppDemoOamEntrySetDm(flowId,
                                megLevel,
                                63 /* CPU port */,
                                CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E,
                                GT_APP_DEMO_OAM_EGRESS_PTP_OFFSET_INDEX);
    if (rc != GT_OK) 
        return rc;    

    return rc;
}

/*******************************************************************************
* gtAppDemoOamDmTxFlowSend
*
* DESCRIPTION:
*       This function sends a single DWMM or starts tramsmiting continuously,
*       according to generatorEnable.
*       The packets are sent with the following paramaters:
*       dst MAC             = 00:00:00:00:00:01
*       src MAC             = 00:00:00:00:00:02
*       VLAN                = vlan
*       meg level           = 0x2
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       generatorEnable     - enable Tx SDMA generator mode.
*       dstPortNum          - Destination port number.      
*       dstMacAddr          - the end of destination mac: 00:00:00:00:00:XX.
*       srcMacAddr          - the end of source mac: 00:00:00:00:00:XX.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamDmTxFlowSend
(
    IN GT_U8    vlanId,
    IN GT_BOOL  generatorEnable,
    IN GT_U32   dstPortNum,
    IN GT_U8    dstMacAddr,
    IN GT_U8    srcMacAddr
)
{
    GT_STATUS   rc; /* function return code */
    GT_U8       packetData[64] = 
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 
    0x89, 0x02, 0x40, 0x2F, 0x00, 0x20, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    packetData[5] = dstMacAddr;
    packetData[11] = srcMacAddr;
    packetData[15] = vlanId;
    if(generatorEnable)
    {
        rc = gtAppDemoOamSdmaTxGeneratorPacketAdd(vlanId, packetData, 64, dstPortNum);
    }
    else
    {
        rc = gtAppDemoOamSdmaTxPacketSend(vlanId, packetData, 64, dstPortNum);
    }
    return rc;
}

/*******************************************************************************
* gtAppDemoOamDmRxFlowGet
*
* DESCRIPTION:
*       Get OAM DM rx flow configuration from the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamDmRxFlowGet
(
    IN GT_U32   flowId
)
{
    GT_STATUS                       rc;
    CPSS_DXCH_OAM_SRV_DM_CONFIG_STC dmConfig;

    rc = cpssDxChOamSrvDmConfigGet(0, flowId, &dmConfig);
    if (rc != GT_OK) 
        return rc;
    cpssOsPrintf("countersEnable = %s\n",(dmConfig.countersEnable?"TRUE":"FALSE"));
    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamDmStatCntrGet
*
* DESCRIPTION:
*       Get OAM DM Rx flow statistical counters from the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       clrOnRead           - enable counter clear
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamDmStatCntrGet 
(
    IN GT_U32   flowId,
    IN GT_BOOL  clrOnRead
)
{
    GT_STATUS                           rc;
    CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC   dmCounters;

    cpssOsMemSet(&dmCounters, '\0', sizeof(CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC));
    rc = cpssDxChOamSrvDmStatisticalCountersGet(0, flowId, clrOnRead, &dmCounters); 
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvDmStatisticalCountersGet",rc);
    if(rc != GT_OK)
     return rc;       
    
    cpssOsPrintf("twoWayFrameDelay   = %u = 0x%08x\n",dmCounters.basicCounters.twoWayFrameDelay, 
                                                      dmCounters.basicCounters.twoWayFrameDelay);
    cpssOsPrintf("oneWayFrameDelayForward = %u = 0x%08x\n",dmCounters.basicCounters.oneWayFrameDelayForward, 
                                                           dmCounters.basicCounters.oneWayFrameDelayForward);
    cpssOsPrintf("oneWayFrameDelayBackward = %u = 0x%08x\n",dmCounters.basicCounters.oneWayFrameDelayBackward, 
                                                            dmCounters.basicCounters.oneWayFrameDelayBackward);
    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamDmStatCntrClr
*
* DESCRIPTION:
*       Clear OAM DM Rx flow statistical counters at the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamDmStatCntrClr
(
    IN GT_U32 flowId
)
{
    GT_STATUS   rc;/* function return code */
    rc = cpssDxChOamSrvDmStatisticalCountersClear(0, flowId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLbStatisticalCountersClr",rc);
    return rc;  
}

/*******************************************************************************
 * OAM LB AppDemo
 ******************************************************************************/
/*******************************************************************************
* gtAppDemoOamLbInit
*
* DESCRIPTION:
*       This function does general initialization of PCL, OAM engines and OAM service CPU.
*       cpssInitSystem() must be called before caling this function for CPSS
*       initialization for BOBCAT2 device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLbInit(void)
{
    GT_STATUS                           rc;/* function return code */

    rc = gtAppDemoOamPclInitGen();   /* Generic Init for all OAM demo PCL */
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamPclInitCfm();
    if(rc != GT_OK)   
         return rc;
    rc = gtAppDemoOamHwInitGen(CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E,
                               GT_FALSE,
                               GT_FALSE);
    if(rc != GT_OK)   
         return rc; 
    rc = gtAppDemoOamHwInitLb();
    if(rc != GT_OK)   
         return rc;
#if ASIC_SIMULATION
    if(gtAppDemoOamCbRegistered == GT_FALSE)
    {
        gtAppDemoOamCbRegistered = GT_TRUE;
        /* Register call back for Rx packets trapped to CPU  */
        rc = appDemoDxChNetRxPacketCbRegister(prvAppDemoOamCbFunc);    
        if(rc != GT_OK)
              return rc;   
    }
#else
    if(gtAppDemoOAmSrvCpuInited == GT_FALSE)
    {
        CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC    sysParams;/* OAM Service CPU system paramaters*/

        cpssOsMemSet(&sysParams, '\0', sizeof(CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC));
#if DEBUG_APPDEMO_OAM
		osPrintf("Init OAM Firmware\n");
        rc =  cpssDxChOamSrvSystemInit(0, &sysParams);
        if(rc != GT_OK)   
             return rc;
#endif

        gtAppDemoOAmSrvCpuInited = GT_TRUE;
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamLbRxFlowSet
*
* DESCRIPTION:
*       Set OAM Loopback Rx flow.
*       The function configs a PCL rule and OAM entry accroding to the flow ID,
*       meg level and source port number.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       megLevel            - Determines the MEG level of the current MEP
*                             (APPLICABLE RANGES: 0..7)
*       portNum             - The Source port number
*       outOfSequenceEnable - enable out of sequence check
*       invalidTlvEnable    - enable invalid TLV check
*       tlvDataCrc32Val     - data TLV CRC32 value, relevant only if 
*                             invalidTlvEnable is TRUE  
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLbRxFlowSet
(
    IN GT_U32   flowId,
    IN GT_U32   megLevel, 
    IN GT_U8    portNum,
    IN GT_BOOL  outOfSequenceEnable,
    IN GT_BOOL  invalidTlvEnable,
    IN GT_U32   tlvDataCrc32Val
)
{
    GT_STATUS                       rc;/* function return code */
    CPSS_DXCH_OAM_SRV_LB_CONFIG_STC config;/*OAM Service CPU Loopback configuration*/    

    rc = gtAppDemoOamPclRuleCfmFlow(flowId*2-2,
                                    flowId,
                                    portNum,
                                    0x01,
                                    flowId,
                                    GT_FALSE);
    if (rc != GT_OK) 
        return rc;
    rc = gtAppDemoOamEntrySetLb(flowId, megLevel, portNum);
    if (rc != GT_OK) 
        return rc;
    config.invalidTlvEnable = invalidTlvEnable;
    config.outOfSequenceEnable = outOfSequenceEnable;
    config.tlvDataCrc32Val = tlvDataCrc32Val;

    rc = cpssDxChOamSrvLoopbackConfigSet(0,flowId, &config);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLbConfigSet",rc);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLbRxFlowGet
*
* DESCRIPTION:
*       Get OAM Loopback Rx flow configuration from the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLbRxFlowGet
(
    IN GT_U32   flowId
)
{
    GT_STATUS                       rc;/* function return code */
    CPSS_DXCH_OAM_SRV_LB_CONFIG_STC config;/*OAM Service CPU Loopback configuration*/    

    rc = cpssDxChOamSrvLoopbackConfigGet(0, flowId, &config);
    if (rc != GT_OK) 
        return rc;
    cpssOsPrintf("outOfSequenceEnable = %s\n",(config.outOfSequenceEnable?"TRUE":"FALSE"));
    cpssOsPrintf("invalidTlvEnable    = %s\n",(config.invalidTlvEnable?"TRUE":"FALSE"));
    cpssOsPrintf("tlvDataCrc32Val     = 0x%x\n",config.tlvDataCrc32Val);

    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamLbStatCntrGet
*
* DESCRIPTION:
*       Get OAM Loopback Rx flow statistical counters from the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       clrOnRead           - enable counter clear
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLbStatCntrGet 
(
    IN GT_U32   flowId,
    IN GT_BOOL  clrOnRead
)
{
    GT_STATUS                           rc;/* function return code */
    CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC   lbCounters;

    cpssOsMemSet(&lbCounters, '\0', sizeof(CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC));
    rc = cpssDxChOamSrvLoopbackStatisticalCountersGet(0, flowId, clrOnRead, 0, &lbCounters); 
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLbStatisticalCountersGet",rc);
    if(rc != GT_OK)
     return rc;       
    
    cpssOsPrintf("outOfOrderCount      = %d\n",lbCounters.outOfSequenceCount);
    cpssOsPrintf("totalCheckedDataTlv  = %d\n",lbCounters.totalCheckedDataTlv);
    cpssOsPrintf("totalInvalidDataTlv  = %d\n",lbCounters.totalInvalidDataTlv);
    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamLbStatCntrClr
*
* DESCRIPTION:
*       Clear OAM Loopback Rx flow statistical counters at the OAM service CPU.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLbStatCntrClr
(
    IN GT_U32 flowId
)
{
    GT_STATUS   rc;/* function return code */
    rc = cpssDxChOamSrvLoopbackStatisticalCountersClear(0, flowId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLbStatisticalCountersClr",rc);
    return rc;  
}

/*******************************************************************************
* gtAppDemoOamExceptionGet
*
* DESCRIPTION:
*       Display OAM exceptions status and counter.
*       The status is of a specific flow.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamExceptionGet
(
    IN  GT_U32 flowId
)
{
    GT_U32                          counterValue; /* OAM exception counter value and bitmap status */
    GT_STATUS                       rc; /* function return code */
    GT_U8                           devNum; /* device number */
    CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage;

    for(devNum=0; devNum < appDemoPpConfigDevAmount; devNum++)
    {
        for(stage = CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E; stage <= CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E; stage++)
        {
            rc = cpssDxChOamExceptionCounterGet(devNum,
                                                stage,
                                                CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E,
                                                &counterValue);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionCounterGet",rc);
            if(rc != GT_OK)
            {    
                 return rc;
            }
            cpssOsPrintf("megLevelCntr = 0x%x ",counterValue);
        
            flowId = flowId/32;
            rc = cpssDxChOamExceptionStatusGet(devNum,
                                               stage,
                                               CPSS_DXCH_OAM_EXCEPTION_TYPE_MEG_LEVEL_E,
                                               flowId,
                                               &counterValue );
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionStatusGet",rc);
            if(rc != GT_OK)
            {    
                 return rc;
            }
            cpssOsPrintf("bitMap = 0x%08x\n",counterValue);
    
            rc = cpssDxChOamExceptionCounterGet(devNum,
                                                stage,
                                                CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E,
                                                &counterValue);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionCounterGet",rc);
            if(rc != GT_OK)
            {    
                 return rc;
            }
            cpssOsPrintf("srcInterfaceCntr = 0x%x ",counterValue);
        
            flowId = flowId/32;
            rc = cpssDxChOamExceptionStatusGet(devNum,
                                               stage,
                                               CPSS_DXCH_OAM_EXCEPTION_TYPE_SOURCE_INTERFACE_E,
                                               flowId,
                                               &counterValue );
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionStatusGet",rc);
            if(rc != GT_OK)
            {    
                 return rc;
            }
            cpssOsPrintf("bitMap = 0x%08x\n",counterValue);
    
            rc = cpssDxChOamExceptionCounterGet(devNum,
                                                stage,
                                                CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E,
                                                &counterValue);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionCounterGet",rc);
            if(rc != GT_OK)
            {    
                 return rc;
            }
            cpssOsPrintf("rdiStatusCntr = 0x%x ",counterValue);
        
            flowId = flowId/32;
            rc = cpssDxChOamExceptionStatusGet(devNum,
                                               stage,
                                               CPSS_DXCH_OAM_EXCEPTION_TYPE_RDI_STATUS_CHANGED_E,
                                               flowId,
                                               &counterValue );
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamExceptionStatusGet",rc);
            if(rc != GT_OK)
            {    
                 return rc;
            }
            cpssOsPrintf("bitMap = 0x%08x\n",counterValue);
        }
    }
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamPolicerGet
*
* DESCRIPTION:
*       Display policer green counter.
*       The status is of a specific flow.
*
* INPUTS:
*       flowId              - The flow ID (APPLICABLE RANGES: 1..2047)
*       reset               - reset the counter values
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamPolicerGet
(
    IN  GT_U32 flowId,
    IN  GT_BOOL reset
)
{
    GT_STATUS rc;
    GT_U8 devNum = 0;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    billingCntr;

    stage = CPSS_DXCH_POLICER_STAGE_INGRESS_0_E;
    /* Flush internal Write Back Cache */
    rc = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, stage);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingWriteBackCacheFlush",rc);
    if(rc != GT_OK)
    {
        return rc;
    } 

    cpssOsMemSet(&billingCntr, 0, sizeof(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC));
    rc = cpssDxCh3PolicerBillingEntryGet(devNum,
                                         stage,
                                         flowId - GT_APP_DEMO_OAM_BILLING_MIN_FLOW_ID,
                                         reset,
                                         &billingCntr);   
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh3PolicerBillingEntryGet",rc);
    if(rc != GT_OK)
    {
        return rc;
    }          

    cpssOsPrintf("ingress green = %d\n",billingCntr.greenCntr);

    /* Flush internal Write Back Cache */
    rc = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, stage);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingWriteBackCacheFlush",rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    stage = CPSS_DXCH_POLICER_STAGE_EGRESS_E;
    /* Flush internal Write Back Cache */
    rc = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, stage);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingWriteBackCacheFlush",rc);
    if(rc != GT_OK)
    {
        return rc;
    } 

    cpssOsMemSet(&billingCntr, 0, sizeof(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC));
    rc = cpssDxCh3PolicerBillingEntryGet(devNum,
                                         stage,
                                         flowId - GT_APP_DEMO_OAM_BILLING_MIN_FLOW_ID,
                                         reset,
                                         &billingCntr);   
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh3PolicerBillingEntryGet",rc);
    if(rc != GT_OK)
    {
        return rc;
    }          

    cpssOsPrintf("egress green = %d\n",billingCntr.greenCntr);

    /* Flush internal Write Back Cache */
    rc = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, stage);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingWriteBackCacheFlush",rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamLocalMacAddressSet
*
* DESCRIPTION:
*       Globally configure the MAC address table in the firmware.
*
* INPUTS:
*       Index 		   - Local MAC Address Entry (Range: 0..127)
*       mac  		   - the end of mac address: 00:00:00:00:00:XX
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLocalMacAddressSet
(
    IN  GT_U32 index,
    IN  GT_U8  mac
)
{
    GT_STATUS rc;
    GT_U8 devNum = 0;
    GT_ETHERADDR macAddr;

    macAddr.arEther[0] = 0x0;
    macAddr.arEther[1] = 0x0;
    macAddr.arEther[2] = 0x0;
    macAddr.arEther[3] = 0x0;
    macAddr.arEther[4] = 0x0;
    macAddr.arEther[5] = mac;

    rc = cpssDxChOamSrvLocalMacAddressSet(devNum, index, macAddr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLocalMacAddressSet",rc);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLocalMacAddressGet
*
* DESCRIPTION:
*       Get the MAC address table from the firmware.
*
* INPUTS:
*       Index 		   - Local MAC Address Entry (Range: 0..127)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLocalMacAddressGet
(
    IN  GT_U32 indexStart,
    IN  GT_U32 indexEnd
)
{
    GT_STATUS rc;
    GT_U8 devNum = 0;
    GT_ETHERADDR macAddr;
    GT_U32 index;

    for(index=indexStart; index<=indexEnd; index++)
    {
        rc = cpssDxChOamSrvLocalMacAddressGet(devNum, index, &macAddr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLocalMacAddressGet",rc);
        if(rc != GT_OK)
         return rc;       
        
        cpssOsPrintf("index=%d mac=%02x %02x %02x %02x %02x %02x\n",index,
                     macAddr.arEther[0],macAddr.arEther[1],macAddr.arEther[2],
                     macAddr.arEther[3],macAddr.arEther[4],macAddr.arEther[5]);
        
    }
    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamLocalInterfaceMacIndexSet
*
* DESCRIPTION:
*       Configures the MAC address index for some local interface
*
* INPUTS:
*       portNum 	    - 0-27 or (for eport) 0-8k
*       isPhysicalPort  - for eport ==0. for regular port ==1.
*       macAddIndex     - 0-127 (index to table configured here- gtAppDemoOamLocalMacAddressSet)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLocalInterfaceMacIndexSet
(
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             isPhysicalPort,
    IN  GT_U32	     		   	            macAddIndex
)
{
    GT_STATUS rc;
    GT_U8 devNum = 0;
    
    rc = cpssDxChOamSrvLocalInterfaceMacIndexSet(devNum, portNum, isPhysicalPort, macAddIndex);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLocalMacAddressSet",rc);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamLocalInterfaceMacIndexGet
*
* DESCRIPTION:
*       Get the MAC address index for some local interface
*
* INPUTS:
*       portNum 	    - 0-27 or (for eport) 0-8k
*       isPhysicalPort  - for eport ==0. for regular port ==1.      
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamLocalInterfaceMacIndexGet
(
    IN  GT_PORT_NUM                         portNumStart,
    IN  GT_PORT_NUM                         portNumEnd,
    IN  GT_BOOL                             isPhysicalPort
)
{
    GT_STATUS rc;
    GT_U8 devNum = 0;
    GT_U32 port;
    GT_U32 macAddIndex;

    for(port=portNumStart; port<=portNumEnd; port++)
    {
        rc = cpssDxChOamSrvLocalInterfaceMacIndexGet(devNum, 
                                                     port, 
                                                     isPhysicalPort,
                                                     &macAddIndex);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvLocalInterfaceMacIndexGet",rc);
        if(rc != GT_OK)
         return rc;       
        
        cpssOsPrintf("interface=%d macAddIndex=%d\n",port, macAddIndex);
        
    }
    
    return GT_OK;
}

/*******************************************************************************
* gtAppDemoOamRxFlowEntryParamsSet
*
* DESCRIPTION:
*       Configures parameters for rx flow
*
* INPUTS:
*       flowId 		   - flowId
*       mpType         - 0-upMep, 1-downMep
*       cfmOffset  	   - Represent the beginning of CFM payload in the received OAM frame (0-127)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamRxFlowEntryParamsSet
(
    IN GT_U32      flowId,
    IN GT_U32      mpType,
    IN GT_U32      cfmOffset,
    IN GT_U32      dsaValid,
    IN GT_U32      vid,
    IN GT_U32      dstPortNum,
    IN GT_U32      egressQueue
)
{
    GT_STATUS rc;
    GT_U8 devNum = 0;
    CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC  frameParams;
    CPSS_DXCH_NET_DSA_PARAMS_STC                pcktParams;
    GT_HW_DEV_NUM                               hwDevNum;

    cpssOsMemSet(&frameParams, '\0', sizeof(CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC));

    frameParams.frameType = CPSS_DXCH_OAM_SRV_FRAME_TYPE_1731_E;
    if(mpType == 0)
        frameParams.frame.frame1731.mpType = CPSS_DXCH_OAM_SRV_MP_TYPE_UP_MEP_E;
    else
        frameParams.frame.frame1731.mpType = CPSS_DXCH_OAM_SRV_MP_TYPE_DOWN_MEP_E;    
    frameParams.frame.frame1731.cfmOffset = cfmOffset;
    frameParams.frame.frame1731.transmitParams.dsaParamValid = 0;

    if(dsaValid)
    {
        frameParams.frame.frame1731.transmitParams.dsaParamValid = 1;

        cpssOsMemSet(&pcktParams, '\0', sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
        pcktParams.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
        pcktParams.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT;
        pcktParams.commonParams.vid = (GT_U16)vid;
        rc = cpssDxChCfgHwDevNumGet(0,&hwDevNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet",rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        pcktParams.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum = hwDevNum;
        pcktParams.dsaInfo.fromCpu.dstInterface.devPort.portNum = dstPortNum;
        pcktParams.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaInfo.fromCpu.tc = (GT_U8)egressQueue;
        pcktParams.dsaInfo.fromCpu.dp =  CPSS_DP_GREEN_E;
        pcktParams.dsaInfo.fromCpu.srcHwDev = hwDevNum;
        pcktParams.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = GT_TRUE;
        pcktParams.dsaInfo.fromCpu.isTrgPhyPortValid = GT_TRUE;
        pcktParams.dsaInfo.fromCpu.dstEport = dstPortNum;        

        cpssOsMemCpy(&frameParams.frame.frame1731.transmitParams.dsaParam, 
                     &pcktParams, sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
    }

    rc = cpssDxChOamSrvRxFlowEntrySet(devNum, flowId, NULL, &frameParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvRxFlowEntrySet",rc);
    return rc;
}

/*******************************************************************************
* gtAppDemoOamRxFlowEntryParamsGet
*
* DESCRIPTION:
*       Get the rx flow generic parameters
*
* INPUTS:
*       flowId
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtAppDemoOamRxFlowEntryParamsGet
(
    IN GT_U32      flowId
)
{
    GT_STATUS rc;
    GT_U8 devNum = 0;
    CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC  frameParams;
    CPSS_DXCH_NET_DSA_PARAMS_STC                dsaParam;
    
    cpssOsMemSet(&frameParams, '\0', sizeof(CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC));

    rc = cpssDxChOamSrvRxFlowEntryGet(devNum, flowId, NULL, &frameParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChOamSrvRxFlowEntryGet",rc);
    if(rc != GT_OK)
     return rc;       
    
    cpssOsPrintf("frameType=%d\n",frameParams.frameType);
    cpssOsPrintf("mpType=%d\n",frameParams.frame.frame1731.mpType);
    cpssOsPrintf("cfmOffset=%d\n",frameParams.frame.frame1731.cfmOffset);
    cpssOsPrintf("dsaParamValid=%d\n",frameParams.frame.frame1731.transmitParams.dsaParamValid);

    cpssOsMemCpy(&dsaParam, &frameParams.frame.frame1731.transmitParams.dsaParam, sizeof(CPSS_DXCH_NET_DSA_PARAMS_STC));

    cpssOsPrintf("dsaType=%d\n",dsaParam.dsaType);
    cpssOsPrintf("dsaTagType=%d\n",dsaParam.commonParams.dsaTagType);
    cpssOsPrintf("vid=%d\n",dsaParam.commonParams.vid);
    cpssOsPrintf("dstEport=%d\n",dsaParam.dsaInfo.fromCpu.dstEport);
    cpssOsPrintf("dstInterface.type=%d\n",dsaParam.dsaInfo.fromCpu.dstInterface.type);
    cpssOsPrintf("dstInterface.hwDevNum=%d\n",dsaParam.dsaInfo.fromCpu.dstInterface.hwDevNum);
    cpssOsPrintf("dstInterface.devPort.hwDevNum=%d\n",dsaParam.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum);
    cpssOsPrintf("dstInterface.devPort.portNum=%d\n",dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum);
    cpssOsPrintf("isTrgPhyPortValid=%d\n",dsaParam.dsaInfo.fromCpu.isTrgPhyPortValid);
    cpssOsPrintf("srcHwDev=%d\n",dsaParam.dsaInfo.fromCpu.srcHwDev);
    cpssOsPrintf("tc=%d\n",dsaParam.dsaInfo.fromCpu.tc);
    cpssOsPrintf("dp=%d\n",dsaParam.dsaInfo.fromCpu.dp);
    
    
    return GT_OK;
}
