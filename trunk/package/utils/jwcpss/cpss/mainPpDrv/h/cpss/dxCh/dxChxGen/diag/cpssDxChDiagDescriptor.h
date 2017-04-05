/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChDiagDescriptor.h
*
* DESCRIPTION:
*       Diag Descriptor APIs for CPSS.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __cpssDxChDiagDescriptorh
#define __cpssDxChDiagDescriptorh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

typedef enum
{
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_APPSPECIFICCPUCODE_E,          /* AppSpecificCPUCode */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ARPPTR_E,                      /* ARPPtr */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BADIPXBC_E,                    /* BadIPxBC */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BANK_E,                        /* Bank */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BILLINGEN_E,                   /* BillingEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BUFREUSE_E,                    /* BufReuse */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BYPASSBRIDGE_DUP_E,            /* BypassBridge_Dup */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BYPASSBRIDGE_E,                /* BypassBridge */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BYPASSINGRESSPIPE_E,           /* BypassIngressPipe */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BYTECOUNT_E,                   /* ByteCount */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CCFC_E,                        /* CCFC */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CFI0_E,                        /* CFI0 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CFI1_E,                        /* CFI1 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CFI_E,                         /* CFI */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CFMPKT_E,                      /* CFMPkt */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CONGESTEDPORTNUMBER_E,         /* CongestedPortNumber */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CONGESTEDQUEUENUMBER_E,        /* CongestedQueueNumber */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CONTROLTC_E,                   /* ControlTC */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_COPYTTL_E,                     /* CopyTTL */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CPIDMATCH_E,                   /* CPIDMatch */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CPU2CPUMIRROR_E,               /* CPU2CPUMirror */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CPUCODE_E,                     /* CPUCode */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CPUMAILTONEIGBORCPU_E,         /* CPUMailToNeigborCPU */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CPUTRGDEV_E,                   /* CPUTrgDev */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DECTTL_E,                      /* DecTTL */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_0_31_E,                    /* DIP bits 0..31 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_32_63_E,                   /* DIP bits 32..63 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_64_95_E,                   /* DIP bits 64..95 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_96_127_E,                  /* DIP bits 96..127 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_E,                         /* DIP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DISABLEIPCL1_FOR_ROUTEDPACKETS_E,  /* DisableIPCL1_for_RoutedPackets */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DOROUTEHA_E,                   /* DoRouteHa */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DOROUTE_E,                     /* DoRoute */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DP_E,                          /* DP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DROPCODE_E,                    /* DropCode */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DROPONSOURCE_E,                /* DropOnSource */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DROPPKT_E,                     /* DropPkt */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DSCP_E,                        /* DSCP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EGRESSFILTEREN_E,              /* EgressFilterEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EGRESSFILTERREGISTERED_E,      /* EgressFilterRegistered */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EGRESSTRGDEV_E,                /* EgressTrgDev */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EGRESSTRGPORT_DUP_2_0_E,       /* EgressTrgPort_dup_2_0 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EGRESSTRGPORT_E,               /* EgressTrgPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENABLEBRIDGELOGFLOW_E,         /* EnableBridgeLogFlow */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENABLEBRIDGE_E,                /* EnableBridge */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENABLEDSATAGMANIPULATION_E,    /* EnableDsaTagManipulation */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENABLEROUTERLOGFLOW_E,         /* EnableRouterLogFlow */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENABLESOURCELOCALFILTERING_E,  /* EnableSourceLocalFiltering */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENABLE_IPCL0_LOOKUP_E,         /* Enable_IPCL0_lookup */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENABLE_POLICY_E,               /* Enable_Policy */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ETHERTYPE_E,                   /* EtherType */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EXCLUDEDDEVISLOCAL_E,          /* ExcludedDevIsLocal */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EXCLUDEDPORT_E,                /* ExcludedPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EXCLUDEDTRUNK_E,               /* ExcludedTrunk */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EXCLUDEISTRUNK_E,              /* ExcludeIsTrunk */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_EXP_E,                         /* EXP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_FIRSTBUFFER_E,                 /* FirstBuffer */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_FLOODMLLPOINTER_E,             /* FloodMLLPointer */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_FLOWID_E,                      /* FlowID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_FLOWSUBTEMPLATE_E,             /* FlowSubTemplate */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_FORCE_IPCL1_CONFIGURATIONS_E,  /* Force_IPCL1_configurations */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_FRAGMENTED_E,                  /* Fragmented */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_FROMCPUDP_E,                   /* FromCPUDP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_FROMCPUTC_E,                   /* FromCPUTC */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_HASHMASKINDEX_E,               /* HashMaskIndex */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IGMPNONQUERY_E,                /* IGMPNonQuery */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IGMPQUERY_E,                   /* IGMPQuery */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_INLIF_MODE_E,                  /* InLIF_Mode */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_INNERISLLCNONSNAP_E,           /* InnerIsLLCNonSnap */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_INNERL3OFFSET1_DUP_E,          /* InnerL3Offset1_dup */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_INNERL3OFFSET_E,               /* InnerL3Offset */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_INNERPKTTYPE_E,                /* InnerPktType */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPCL_PROFILE_INDEX_E,          /* IPCL_Profile_Index */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPLEGAL_E,                     /* IPLegal */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPMGMNTEN_E,                   /* IPMgmntEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPMODIFYDSCP_E,                /* IPModifyDSCP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPMODIFYUP_E,                  /* IPModifyUP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPM_E,                         /* IPM */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPQOSPROFILE_E,                /* IPQoSProfile */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV4MULTICASTEN_E,             /* Ipv4MulticastEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV4OPTIONFIELD_E,             /* IPv4OptionField */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV4RIPV1_E,                   /* IPv4RIPv1 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV4UNICASTEN_E,               /* Ipv4UnicastEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV4_BAD_GRE_HEADER_E,         /* IPv4_Bad_GRE_Header */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV4_DF_E,                     /* IPv4_DF */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6FLOWLABEL_E,               /* IPv6FlowLabel */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6HBHEXT_E,                  /* IPv6HBHExt */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6HBHOPTIONS_E,              /* IPv6HBHOptions */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6ICMPTYPE_E,                /* Ipv6ICMPType */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6ICMP_E,                    /* IPv6ICMP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6MULTICASTEN_E,             /* Ipv6MulticastEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6NONHBHEXT_E,               /* IPv6NonHBHExt */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6SITEID_E,                  /* IPv6SiteID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6UNICASTEN_E,               /* Ipv6UnicastEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPV6_EH_E,                     /* IPv6_EH */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPXHEADERTOTALLENGTH_E,        /* IPxHeaderTotalLength */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPXMACMCMISMATCH_E,            /* IPxMACMcMismatch */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPXMCLINKLOCALPROT_E,          /* IPxMcLinkLocalProt */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IPXPROTOCOL_E,                 /* IPxProtocol */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IP_FRAGMENTED_E,               /* IP_Fragmented */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_IP_HEADER_INFO_E,              /* IP_Header_Info */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISARPREPLY_E,                  /* IsARPReply */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISARP_E,                       /* IsARP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISBC_E,                        /* IsBC */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISIPV4_E,                      /* IsIPv4 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISIPV4_FORKEY_E,               /* IsIPv4_ForKey */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISIPV6LINKLOCAL_E,             /* IsIPv6LinkLocal */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISIPV6MLD_E,                   /* IsIPv6MLD */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISIPV6_E,                      /* IsIPv6 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISIPV6_FORKEY_E,               /* IsIPv6_ForKey */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISIP_E,                        /* IsIP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISIP_FORKEY_E,                 /* IsIP_ForKey */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISMPLS_E,                      /* IsMPLS */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISND_E,                        /* IsND */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISPTP_E,                       /* IsPTP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISSYN_E,                       /* IsSYN */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L2ECHO_E,                      /* L2Echo */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L2ENCAPSULATION_E,             /* L2Encapsulation */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L2_VALID_E,                    /* L2_Valid */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L3OFFSETINVALID_E,             /* L3OffsetInvalid */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET_E,  /* L3TotalOffset_or_InnerHeaderTotalOffset */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L4BYTE13_E,                    /* L4Byte13 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L4BYTES3_0_E,                  /* L4Bytes3_0 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L4OFFSETINVALID_E,             /* L4OffsetInvalid */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_L4VALID_E,                     /* L4Valid */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LLT_E,                         /* LLT */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LOCALDEVICESRCTRUNKID_E,       /* LocalDeviceSrcTrunkID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LOCALDEVSRCPORT_E,             /* LocalDevSrcPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LOOKUP0_PCL_MODE_E,            /* Lookup0_PCL_mode */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LOOKUP0_USEINDEXFROMDESC_E,    /* Lookup0_UseIndexFromDesc */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LOOKUP1_PCL_MODE_E,            /* Lookup1_PCL_mode */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LOOKUP1_USEINDEXFROMDESC_E,    /* Lookup1_UseIndexFromDesc */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LOOKUP2_PCL_MODE_E,            /* Lookup2_PCL_mode */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LOOKUP2_USEINDEXFROMDESC_E,    /* Lookup2_UseIndexFromDesc */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MAC2ME_E,                      /* mac2me */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MACDATYPE_E,                   /* MacDaType */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MACDA_0_31_E,                  /* MACDA bits 0..31 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MACDA_32_48_E,                 /* MACDA bits 32..48 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MACSAARPSAMISMATCH_E,          /* MacSAArpSAMismatch */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MACSA_0_31_E,                  /* MACSA bits 0..31 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MACSA_32_48_E,                 /* MACSA bits 32..48 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MAILBOXTONEIGHBORCPU_E,        /* MailBoxToNeighborCPU */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MARVELLTAGGEDEXTENDED_E,       /* MarvellTaggedExtended */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MARVELLTAGGED_E,               /* MarvellTagged */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MCRPFFAIL_E,                   /* MCRPFFail */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MDB_E,                         /* MDB */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_METERINGEN_E,                  /* MeteringEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MLLEN_E,                       /* MLLEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MLLLAST_E,                     /* MLLLast */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MLLPTR_E,                      /* MLLPtr */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MLL_E,                         /* MLL */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MLL_POINTER_E,                 /* MLL_Pointer */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MODIFYDP_E,                    /* ModifyDP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MODIFYDSCP_E,                  /* ModifyDSCP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MODIFYDSCP_EXP_E,              /* ModifyDSCP_EXP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MODIFYEXP_E,                   /* ModifyEXP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MODIFYTC_E,                    /* ModifyTC */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MODIFYUP_E,                    /* ModifyUP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLSCMD_E,                     /* MPLSCmd */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLSLABEL_E,                   /* MPLSLabel */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_LABEL0_E,                 /* MPLS_Label0 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_LABEL1_E,                 /* MPLS_Label1 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_LABEL2_E,                 /* MPLS_Label2 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_OUTEREXP_E,               /* MPLS_OuterEXP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_OUTERLABEL_E,             /* MPLS_OuterLabel */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_OUTERS_BIT_E,             /* MPLS_OuterS_Bit */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_NESTEDVLANACCESSPORT_E,        /* NestedVlanAccessPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_NHEMCRPFFAILCMD_E,             /* NHEMCRPFFailCmd */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_NUM_OF_INGRESSTAGS_E,          /* Num_of_IngressTags */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ORIGISTRUNK_E,                 /* OrigIsTrunk */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ORIGQCNPRIO_E,                 /* OrigQCNPrio */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ORIGSRCDEV_E,                  /* OrigSrcDev */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ORIGSRCPORT_E,                 /* OrigSrcPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ORIGSRCTRUNK_E,                /* OrigSrcTrunk */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ORIGVID_E,                     /* OrigVID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_OUTERISLLCNONSNAP_E,           /* OuterIsLLCNonSnap */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_OUTERL3TOTALOFFSET_E,          /* OuterL3TotalOffset */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_OUTERSRCTAG_E,                 /* OuterSrcTag */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_OUTGOINGMTAGCMD_E,             /* OutGoingMtagCmd */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_OUTLIF_TYPE_E,                 /* OutLIF_Type */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETCMD_E,                   /* PacketCmd */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETISCNTAGGED_E,            /* PacketIsCNTagged */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETISLLCSNAP_E,             /* PacketIsLLCSnap */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETREJECT_E,                /* PacketReject */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETTOS_E,                   /* PacketTOS */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETTYPE_E,                  /* PacketType */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETUP_E,                    /* PacketUP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PCL1_IPV4_KEY_TYPE_E,          /* PCL1_IPv4_Key_Type */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PCL1_IPV6_KEY_TYPE_E,          /* PCL1_IPv6_Key_Type */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PCL1_LOOKUP_MODE_E,            /* PCL1_Lookup_mode */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PCL1_LOOKUP_TYPE_E,            /* PCL1_Lookup_Type */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PCL1_NONIP_KEY_TYPE_E,         /* PCL1_NonIP_Key_Type */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PCL1_PCL_ID_E,                 /* PCL1_PCL_ID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PCLASSIGNEDSSTID_E,            /* PCLAssignedSSTID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PKTCMD_E,                      /* PktCmd */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PKTHASH_E,                     /* PktHash */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PKTISLOOPED_E,                 /* PktIsLooped */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PKTTYPEFORKEY_E,               /* PktTypeForKey */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_POLICEREN_E,                   /* PolicerEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_POLICERPTR_E,                  /* PolicerPtr */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_POLICYBASEDROUTED_E,           /* PolicyBasedRouted */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_POLICYRTTINDEX_E,              /* PolicyRTTIndex */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PORTSPEEDINDEX_OR_ORIGQCNPRIO_E,  /* PortSpeedIndex_or_OrigQCNPrio */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PRIOTAGGED_E,                  /* PrioTagged */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PSEUDOWIREID_E,                /* PseudoWireID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PTPTRIGGERTYPE_E,              /* PTPTriggerType */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QCNPACKET_E,                   /* QCNPacket */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QCNQDELTA_E,                   /* QCNqDelta */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QCNQFB_E,                      /* QCNqFb */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QCNQOFFSET_E,                  /* QCNqOffset */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QCNQSATURATED_E,               /* QCNqSaturated */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QCN_E,                         /* QCN */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QINQ_E,                        /* QinQ */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QOSPROFILEPRECEDENCE_E,        /* QosProfilePrecedence */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QOSPROFILE_E,                  /* QosProfile */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RECALCCRC_E,                   /* RecalcCRC */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_REDIRECTCMD_E,                 /* RedirectCmd */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RESERVED_0_31_E,               /* Reserved bits 0..31 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RESERVED_32_63_E,              /* Reserved bits 32..63 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RESERVED_64_65_E,              /* Reserved bits 64..65 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RESERVED_64_90_E,              /* Reserved bits 64..90 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RESERVED_E,                    /* Reserved */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGARPINDEX_E,                  /* RGARPIndex */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGBYPASSTTLANDOPTIONCHECKS_E,  /* RGBypassTTLandOptionChecks */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGDECTTL_E,                    /* RGDecTTL */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGEGRESSINTERFACE_E,           /* RGEgressInterface */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGFWDCMD_E,                    /* RGFwdCmd */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGICMP_REDIRECTCHECKEN_E,      /* RGICMP_RedirectCheckEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGMIRRORTOINGRESSANALYZERPORT_E,  /* RGMirrorToIngressAnalyzerPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGMODIFYDSCP_E,                /* RGModifyDSCP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGMODIFYUP_E,                  /* RGModifyUP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGNHEVID_E,                    /* RGNHEVID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGPCEROUTEENTRY_E,             /* RGPCERouteEntry */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGQOSPROFILE_E,                /* RGQoSProfile */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGQOS_PRECEDENCE_E,            /* RGQoS_Precedence */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RGQOS_PROFILEMARKINGEN_E,      /* RGQoS_ProfileMarkingEn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ROUTED_E,                      /* Routed */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ROUTETOENTRY_E,                /* RouteToEntry */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RSRVDCOPY_E,                   /* RsrvdCopy */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_RXSNIFF_E,                     /* RxSniff */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SIP_0_31_E,                    /* SIP bits 0..31 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SIP_32_63_E,                   /* SIP bits 32..63 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SIP_64_95_E,                   /* SIP bits 64..95 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SIP_96_127_E,                  /* SIP bits 96..127 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SNIFFTRGDEV_E,                 /* SniffTrgDev */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SNIFFTRGPORT_E,                /* SniffTrgPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SNIFFVIDX_E,                   /* SniffVIDX */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SOLICITATIONMULTICASTMESSAGE_E,  /* SolicitationMulticastMessage */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCDEVISOWN_E,                 /* SrcDevIsOwn */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCDEV_E,                      /* SrcDev */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCISLOOPBACK_E,               /* SrcIsLoopback */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCPORT_E,                     /* SrcPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCTAGGED_DUP_E,               /* SrcTagged_dup */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCTAGGED_E,                   /* SrcTagged */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCTCPUDPPORTIS0_E,            /* SrcTCPUDPPortIs0 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCTRG_E,                      /* SrcTrg */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRC_IS_PE_E,                   /* Src_is_PE */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SST_ID_E,                      /* SST_ID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SYNWITHDATA_E,                 /* SYNWithData */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TARGETISTRUNK_E,               /* TargetIsTrunk */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TCPFLAGS_E,                    /* TCPFlags */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TCP_UDPDESTPORT_E,             /* TCP_UDPDestPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TCP_UDP_SRCPORT_E,             /* TCP_UDP_SrcPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TC_E,                          /* TC */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGDEVICE_E,                   /* TrgDevice */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGDEVICE_VIRTUALSRCDEV_E,     /* TrgDevice_VirtualSrcDev */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGDEV_E,                      /* TrgDev */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGISTRUNK_E,                  /* TrgIsTrunk */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGPORT_E,                     /* TrgPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGPORT_VIRTUALSRCPORT_E,      /* TrgPort_VirtualSrcPort */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGTAGGED_DUP_E,               /* TrgTagged_dup */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGTAGGED_E,                   /* TrgTagged */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRGTRUNKID_E,                  /* TrgTrunkID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TRUNCATED_E,                   /* Truncated */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TS_PASSENGER_TYPE_E,           /* TS_Passenger_Type */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TTL_E,                         /* TTL */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TUNNELPTR_E,                   /* TunnelPtr */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TUNNELSTART_E,                 /* TunnelStart */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TUNNELTERMINATED_E,            /* TunnelTerminated */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TUNNELTYPE_E,                  /* TunnelType */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_TXSAMPLED_E,                   /* TxSampled */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB0_E,                        /* UDB0 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB10_E,                       /* UDB10 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB11_E,                       /* UDB11 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB12_E,                       /* UDB12 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB13_E,                       /* UDB13 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB14_E,                       /* UDB14 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB15_E,                       /* UDB15 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB16_E,                       /* UDB16 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB17_E,                       /* UDB17 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB18_E,                       /* UDB18 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB19_E,                       /* UDB19 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB1_E,                        /* UDB1 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB20_E,                       /* UDB20 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB21_E,                       /* UDB21 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB22_E,                       /* UDB22 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB2_E,                        /* UDB2 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB3_E,                        /* UDB3 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB4_E,                        /* UDB4 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB5_E,                        /* UDB5 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB6_E,                        /* UDB6 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB7_E,                        /* UDB7 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB8_E,                        /* UDB8 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB9_E,                        /* UDB9 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB_BYTESVALID_E,              /* UDB_BytesValid */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UP1_E,                         /* UP1 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UP_E,                          /* UP */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_USEINTERFACEBASEDINDEX_E,      /* UseInterfaceBasedIndex */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_USEVIDX_E,                     /* UseVIDX */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VID1_E,                        /* VID1 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VIDMODIFIED_E,                 /* VIDModified */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VIDX_E,                        /* VIDX */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VID_E,                         /* VID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VID_OR_INLIF_ID_E,             /* VID_or_InLIF_ID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VID_PRECEDENCE_E,              /* VID_Precedence */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VIRTUALINTERFACE_E,            /* VirtualInterface */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VLAN1_EXIST_E,                 /* VLAN1_Exist */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VRF_ID_E,                      /* VRF_ID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_VR_ID_E,                       /* VR_ID */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCOWNDEV0_E,                  /* SrcOwnDev0 */
    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CONGESTEDQUEUEFILLLEVEL_E,     /* CongestedQueueFillLevel */

    CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LAST_E

}CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENT;




/*
 * typedef: struct CPSS_DXCH_DIAG_DESCRIPTOR_STC
 *
 * Description: used to describe descriptor information.
 *
 * Fields:
 *      fieldValue - descriptor field values
 *      fieldValueValid - descriptor field validity
 *                        - GT_TRUE  - the field is valid
 *                        - GT_FALSE - the field is invalid
 *
 */
typedef struct
{
    GT_U32   fieldValue[CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LAST_E];
    GT_BOOL  fieldValueValid[CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LAST_E];
}CPSS_DXCH_DIAG_DESCRIPTOR_STC;


/*
 * Typedef: enum CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT
 *
 * Description:
 *      This enum defines descriptor types.
 * Enumerations:
 *     CPSS_DXCH_DIAG_DESCRIPTOR_WR_DMA_TO_TTI_E - Write DMA to TTI descriptor.
 *     CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E - TTI to PCL descriptor.
 *     CPSS_DXCH_DIAG_DESCRIPTOR_PCL_TO_BRIDGE_E - PCL to bridge descriptor.
 *     CPSS_DXCH_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E - Bridge to Router descriptor.
 *     CPSS_DXCH_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E -
 *            Router to ingress policer descriptor.
 *             (APPLICABLE DEVICES: Lion; Lion2)
 *     CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER0_TO_INGRESS_POLICER1_E -
 *            Ingress policer0 to ingress policer1 descriptor.
 *     CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER1_TO_PRE_EGRESS_E -
 *            Ingress policer1 to preegress descriptor.
 *     CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E - Preegress to TxQ descriptor.
 *     CPSS_DXCH_DIAG_DESCRIPTOR_TXQ_TO_HEADER_ALTERATION_E -  TxQ to
 *            Header Alteration descriptor.
 *             (APPLICABLE DEVICES: xCat2; Lion; Lion2)
 *     CPSS_DXCH_DIAG_DESCRIPTOR_EGRESS_POLICER_TO_EGRESS_PCL_E - egress policer
 *            to egress PCL descriptor.
 *     CPSS_DXCH_DIAG_DESCRIPTOR_LAST_E - last descriptor.
 *
 *  Comments:
 *            None.
 *
 */
typedef enum
{
   CPSS_DXCH_DIAG_DESCRIPTOR_WR_DMA_TO_TTI_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_PCL_TO_BRIDGE_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER0_TO_INGRESS_POLICER1_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER1_TO_PRE_EGRESS_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_TXQ_TO_HEADER_ALTERATION_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_EGRESS_POLICER_TO_EGRESS_PCL_E,
   CPSS_DXCH_DIAG_DESCRIPTOR_LAST_E

} CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT;


/*******************************************************************************
* cpssDxChDiagDescriptorRawGet
*
* DESCRIPTION:
*       Get the descriptor information in raw (HW) format
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       descriptorType     - descriptor type
*       rawDataLengthPtr   - pointer to descriptor raw (HW) data length (bytes).
*
* OUTPUTS:
*       rawDataLengthPtr   - pointer to actual descriptor raw (HW) data length (bytes).
*       rawDataPtr         - pointer to descriptor data in raw (HW) format.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number, portGroupsBmp,
*                                  descriptorType.
*       GT_BAD_SIZE              - on insufficient descriptor raw (HW) data size.
*       GT_BAD_PTR               - on NULL pointer value.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDescriptorRawGet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    INOUT GT_U32                            *rawDataLengthPtr,
    OUT GT_U32                              *rawDataPtr
);

/*******************************************************************************
* cpssDxChDiagDescriptorPortGroupRawGet
*
* DESCRIPTION:
*       Get the descriptor information in raw (HW) format
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                          NOTEs:
*                   1. for non multi-port groups device this parameter is IGNORED.
*                   2. for multi-port groups device :
*                      bitmap must be set with at least one bit representing
*                      valid port group(s). If a bit of non valid port group
*                      is set then function returns GT_BAD_PARAM.
*                      Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       descriptorType     - descriptor type
*       rawDataLengthPtr   - pointer to descriptor raw (HW) data length (bytes).
*
* OUTPUTS:
*       rawDataLengthPtr   - pointer to actual descriptor raw (HW) data length (bytes).
*       rawDataPtr         - pointer to descriptor data in raw (HW) format.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number, portGroupsBmp,
*                                  descriptorType.
*       GT_BAD_SIZE              - on insufficient descriptor raw (HW) data size.
*       GT_BAD_PTR               - on NULL pointer value.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDescriptorPortGroupRawGet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    INOUT GT_U32                            *rawDataLengthPtr,
    OUT GT_U32                              *rawDataPtr
);

/*******************************************************************************
* cpssDxChDiagDescriptorRawSizeGet
*
* DESCRIPTION:
*       Get the descriptor raw (HW) size.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       descriptorType     - descriptor type
*
* OUTPUTS:
*       rawDataLengthPtr   - pointer to actual descriptor raw (HW) data length (bytes).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number, portGroupsBmp,
*                                  descriptorType.
*       GT_BAD_PTR               - on NULL pointer value.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDescriptorRawSizeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT  descriptorType,
    OUT GT_U32                              *rawDataLengthPtr
);

/*******************************************************************************
* cpssDxChDiagDescriptorGet
*
* DESCRIPTION:
*       Get the descriptor information.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       descriptorType     - descriptor type
*
* OUTPUTS:
*       descriptorPtr       - pointer to descriptor information.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number, portGroupsBmp,
*                                  descriptorType.
*       GT_BAD_PTR               - on NULL pointer value.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDescriptorGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    OUT CPSS_DXCH_DIAG_DESCRIPTOR_STC        *descriptorPtr
);

/*******************************************************************************
* cpssDxChDiagDescriptorPortGroupGet
*
* DESCRIPTION:
*       Get the descriptor information.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                          NOTEs:
*                   1. for non multi-port groups device this parameter is IGNORED.
*                   2. for multi-port groups device :
*                      bitmap must be set with at least one bit representing
*                      valid port group(s). If a bit of non valid port group
*                      is set then function returns GT_BAD_PARAM.
*                      Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       descriptorType     - descriptor type
*
* OUTPUTS:
*       descriptorPtr       - pointer to descriptor information.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device number, portGroupsBmp,
*                                  descriptorType.
*       GT_BAD_PTR               - on NULL pointer value.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDescriptorPortGroupGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    OUT CPSS_DXCH_DIAG_DESCRIPTOR_STC        *descriptorPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChDiagDescriptor */

