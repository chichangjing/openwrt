/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChDiagDescriptor.c
*
* DESCRIPTION:
*       Diag Descriptor APIs for CPSS.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/diag/private/prvCpssDxChDiagLog.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDescriptor.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* Not condition and not splitted field entry */
 #define FLD_MAC(_fieldName, _fieldOffset, _fieldLength) \
 { CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_##_fieldName##_E, /* field type enum */\
  (_fieldOffset), \
  (_fieldLength), \
    0,   /* single field without split */\
    NULL} /* field without condition*/\

/* full entry */
#define FLD_FULL_MAC(_fieldName, _fieldOffset, _fieldLength, _swFieldOffset, _condPtr) \
 { CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_##_fieldName##_E, /* field type enum */\
  (_fieldOffset), \
  (_fieldLength), \
  (_swFieldOffset),\
  (_condPtr)} \

/* condition entry */
#define COND_MAC(_operType, _startBit, _length, _value,_condType)  \
{PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_##_operType##_E,  (_startBit), (_length),\
(_value), PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_##_condType##_E}

/*
 * Typedef: enum PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_ENT
 *
 * Description:
 *      This enum defines operation types for condition fields.
 * Enumerations:
 *     PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_OR_E - Perform OR with next condition.
 *     PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_OR_SEQ_E - Perform OR and continue sequence.
 *     PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_AND_E - Perform AND with next condition.
 *     PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_LAST_E - No more conditions.
 *
 *  Comments:
 *
 */
typedef enum
{
  PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_OR_E,
    PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_OR_SEQ_E,
  PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_AND_E,
  PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_LAST_E
} PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_ENT
 *
 * Description:
 *      This enum defines condition types for condition fields.
 * Enumerations:
 *     PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_EQUAL_E - The HW value should be
 *                                  eguel to condition field value.
 *     PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_DIFFER_E - The HW value should be
 *                                  different from condition field value.
 *
 *  Comments:
 *
 */
typedef enum
{
  PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_EQUAL_E,
  PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_DIFFER_E
} PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_ENT;

/*
 * typedef: struct PRV_CPSS_DXCH_DIAG_CONDITION_STC
 *
 * Description: used to describe condition field information.
 *
 * Fields:
 *      operation - operation type
 *      fieldOffset - condition field offset
 *      fieldLength - condition field length
 *      conditionValue - condition field value
 *      type - condition type
 *
 */
typedef struct
{
  PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_ENT operation;
  GT_U32   fieldOffset;
  GT_U32   fieldLength;
  GT_U32   conditionValue;
  PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_ENT type;
} PRV_CPSS_DXCH_DIAG_CONDITION_STC;


/*
 * typedef: struct PRV_CPSS_DXCH_DIAG_HW_DESCR_STC
 *
 * Description: used to describe private DB for HW to SW conversion.
 *
 * Fields:
 *      fieldName - field name
 *      fieldOffset - field offset
 *      fieldLength - field length
 *      swFieldOffset - SW field offset for splitted fields.
 *      condDataPtr - pointer to condition data.
 *
 */
typedef struct
{
  CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ENT  fieldName;
  GT_U32   fieldOffset;
  GT_U32   fieldLength;
  GT_U32   swFieldOffset;
  PRV_CPSS_DXCH_DIAG_CONDITION_STC *condDataPtr;

} PRV_CPSS_DXCH_DIAG_HW_DESCR_STC;

/*  WR DMA To TTI */
static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat2wrdma2tti[] =
{
    FLD_MAC(FIRSTBUFFER, 0, 14),
    FLD_MAC(MDB, 14, 1),
    FLD_MAC(BANK, 15, 1),
    FLD_MAC(BYTECOUNT, 16, 14),
    FLD_MAC(RECALCCRC, 30, 1),
    FLD_MAC(MARVELLTAGGED, 31, 1),
    FLD_MAC(LOCALDEVSRCPORT, 32, 6)
};


/* TTI To PCL */
static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_PacketCmd_NOT_FromCPU[] =
{
    COND_MAC(LAST, 179, 3, 7, DIFFER) /* (PacketCmd != FromCPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_PacketCmd_FromCPU[] =
{
    COND_MAC(LAST, 179, 3, 7, EQUAL) /* (PacketCmd == FromCPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_UseVIDX_0[] =
{
    COND_MAC(LAST, 232 + 0, 1, 0, EQUAL) /* (<UseVIDX> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_UseVIDX_0_AND_TrgIsTrunk_0[] =
{
    COND_MAC(AND, 232 + 0, 1, 0, EQUAL), /* (<UseVIDX> == 0) */
    COND_MAC(LAST, 232 + 1, 1, 0, EQUAL) /* (<TrgIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_UseVIDX_0_AND_TrgIsTrunk_1[] =
{
    COND_MAC(AND, 232 + 0, 1, 0, EQUAL), /* (<UseVIDX> == 0) */
    COND_MAC(LAST, 232 + 1, 1, 1, EQUAL) /* (<TrgIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_UseVIDX_1[] =
{
    COND_MAC(LAST, 232 + 0, 1, 1, EQUAL) /* (<UseVIDX> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_OutLIF_Type_MLL[] =
{
    COND_MAC(LAST, 232 + 15, 2, 0, EQUAL) /* (<OutLIF_Type> == MLL) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_OutLIF_Type_Tunnel[] =
{
    COND_MAC(LAST, 232 + 15, 2, 1, EQUAL) /* (<OutLIF_Type> == Tunnel) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_OutLIF_Type_LinkLayer[] =
{
    COND_MAC(LAST, 232 + 15, 2, 2, EQUAL) /* (<OutLIF_Type> == LinkLayer) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_RedirectCmd_4[] =
{
    COND_MAC(LAST, 190, 3, 4, EQUAL) /* (<RedirectCmd> == 4) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_RedirectCmd_2[] =
{
    COND_MAC(LAST, 190, 3, 2, EQUAL) /* (<RedirectCmd> == 2) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_RedirectCmd_3[] =
{
    COND_MAC(LAST, 190, 3, 3, EQUAL) /* (<RedirectCmd> == 3) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_RedirectCmd_5[] =
{
    COND_MAC(LAST, 190, 3, 5, EQUAL) /* (<RedirectCmd> == 5) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_1[] =
{
    COND_MAC(AND, 179, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(LAST, 232 + 0, 1, 1, EQUAL) /* (<UseVIDX> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_1[] =
{
    COND_MAC(AND, 179, 3, 7, DIFFER), /* (<PacketCmd> != FromCPU) */
    COND_MAC(LAST, 304, 1, 1, EQUAL) /* (<OrigIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_0[] =
{
    COND_MAC(AND, 179, 3, 7, DIFFER), /* (<PacketCmd> != FromCPU) */
    COND_MAC(LAST, 304, 1, 0, EQUAL) /* (<OrigIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1[] =
{
    COND_MAC(AND, 179, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(AND, 232 + 0, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 304, 1, 1, EQUAL) /* (<ExcludeIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0[] =
{
    COND_MAC(AND, 179, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(AND, 232 + 0, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 304, 1, 0, EQUAL) /* (<ExcludeIsTrunk> ==0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_0[] =
{
    COND_MAC(AND, 179, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(LAST, 232 + 0, 1, 1, EQUAL) /* (<UseVIDX> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_IsIPv4_ForKey_0[] =
{
    COND_MAC(LAST, 973, 1, 0, EQUAL) /* (<IsIPv4_ForKey> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_IsIPv4_ForKey_1[] =
{
    COND_MAC(LAST, 973, 1, 1, EQUAL) /* (<IsIPv4_ForKey> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_QCNPacket_1[] =
{
    COND_MAC(LAST, 1134, 1, 1, EQUAL) /* (<QCNPacket> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_tti2pcl_QCNPacket_0[] =
{
    COND_MAC(LAST, 1134, 1, 0, EQUAL) /* (<QCNPacket> == 0) */
};

static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat2tti2pcl[] =
{
    FLD_MAC(FIRSTBUFFER, 0, 14),
    FLD_MAC(MDB, 14, 1),
    FLD_MAC(BANK, 15, 1),
    FLD_MAC(ETHERTYPE, 16, 16),
    FLD_MAC(SRCTAGGED, 32, 1),
    FLD_MAC(UP, 33, 3),
    FLD_MAC(VID, 36, 12),
    FLD_MAC(VIDMODIFIED, 48, 1),
    FLD_MAC(VID_PRECEDENCE, 49, 1),
    FLD_MAC(OUTERISLLCNONSNAP, 50, 1),
    FLD_MAC(PRIOTAGGED, 51, 1),
    FLD_MAC(MARVELLTAGGEDEXTENDED, 52, 2),
    FLD_MAC(RECALCCRC, 54, 1),
    FLD_MAC(BYTECOUNT, 55, 14),
    FLD_MAC(LOCALDEVSRCPORT, 69, 6),
    FLD_MAC(MARVELLTAGGED, 75, 1),
    FLD_FULL_MAC(LOCALDEVICESRCTRUNKID, 76, 7, 0, NULL),
    FLD_FULL_MAC(LOCALDEVICESRCTRUNKID, 83, 1, 7, NULL),
    FLD_MAC(RXSNIFF, 84, 1),
    FLD_MAC(ENABLE_POLICY, 85, 1),
    FLD_MAC(LOOKUP0_PCL_MODE, 86, 1),
    FLD_MAC(LOOKUP0_USEINDEXFROMDESC, 87, 1),
    FLD_MAC(LOOKUP1_PCL_MODE, 88, 1),
    FLD_MAC(LOOKUP1_USEINDEXFROMDESC, 89, 1),
    FLD_MAC(LOOKUP2_PCL_MODE, 90, 1),
    FLD_MAC(LOOKUP2_USEINDEXFROMDESC, 91, 1),
    FLD_MAC(PCL1_PCL_ID, 92, 10),
    FLD_MAC(PCL1_NONIP_KEY_TYPE, 102, 3),
    FLD_MAC(PCL1_IPV4_KEY_TYPE, 105, 3),
    FLD_MAC(PCL1_IPV6_KEY_TYPE, 108, 3),
    FLD_MAC(PCL1_LOOKUP_MODE, 111, 1),
    FLD_MAC(PCL1_LOOKUP_TYPE, 112, 2),
    FLD_MAC(FORCE_IPCL1_CONFIGURATIONS, 114, 1),
    FLD_MAC(ENABLE_IPCL0_LOOKUP, 115, 1),
    FLD_MAC(IPCL_PROFILE_INDEX, 116, 13),
    FLD_MAC(DISABLEIPCL1_FOR_ROUTEDPACKETS, 129, 1),
    FLD_MAC(NESTEDVLANACCESSPORT, 130, 1),
    FLD_MAC(QOSPROFILE, 131, 7),
    FLD_FULL_MAC(DSCP, 138, 6, 0, cond_tti2pcl_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(CONTROLTC, 138, 1, 0, cond_tti2pcl_PacketCmd_FromCPU),
    FLD_FULL_MAC(FROMCPUTC, 139, 3, 0, cond_tti2pcl_PacketCmd_FromCPU),
    FLD_FULL_MAC(FROMCPUDP, 142, 2, 0, cond_tti2pcl_PacketCmd_FromCPU),
    FLD_MAC(EXP, 144, 3),
    FLD_MAC(TC, 147, 3),
    FLD_MAC(DP, 150, 2),
    FLD_MAC(QOSPROFILEPRECEDENCE, 152, 1),
    FLD_MAC(MODIFYUP, 153, 1),
    FLD_FULL_MAC(CPU2CPUMIRROR, 154, 1, 0, cond_tti2pcl_PacketCmd_FromCPU),
    FLD_FULL_MAC(MODIFYDSCP, 154, 1, 0, cond_tti2pcl_PacketCmd_NOT_FromCPU),
    FLD_MAC(MODIFYEXP, 155, 1),
    FLD_MAC(TTL, 156, 8),
    FLD_MAC(DECTTL, 164, 1),
    FLD_MAC(COPYTTL, 165, 1),
    FLD_MAC(L3OFFSETINVALID, 166, 1),
    FLD_MAC(INNERL3OFFSET, 167, 3),
    FLD_MAC(L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET, 170, 4),
    FLD_MAC(OUTERL3TOTALOFFSET, 174, 4),
    FLD_MAC(L4OFFSETINVALID, 178, 1),
    FLD_MAC(PACKETCMD, 179, 3),
    FLD_MAC(CPUCODE, 182, 8),
    FLD_MAC(REDIRECTCMD, 190, 3),
    FLD_MAC(BILLINGEN, 193, 1),
    FLD_MAC(METERINGEN, 194, 1),
    FLD_MAC(POLICERPTR, 195, 16),
    FLD_MAC(TUNNELTERMINATED, 211, 2),
    FLD_MAC(INNERPKTTYPE, 213, 2),
    FLD_MAC(INLIF_MODE, 215, 1),
    FLD_MAC(VID_OR_INLIF_ID, 216, 16),
    FLD_MAC(USEVIDX, 232 + 0, 1),
    FLD_FULL_MAC(TRGISTRUNK, 232 + 1, 1, 0, cond_tti2pcl_UseVIDX_0),
    FLD_FULL_MAC(TRGPORT, 232 + 2, 6, 0, cond_tti2pcl_UseVIDX_0_AND_TrgIsTrunk_0),
    FLD_FULL_MAC(TRGDEVICE, 232 + 8, 7, 0, cond_tti2pcl_UseVIDX_0_AND_TrgIsTrunk_0),
    FLD_FULL_MAC(TRGTRUNKID, 232 + 2, 8, 0, cond_tti2pcl_UseVIDX_0_AND_TrgIsTrunk_1),
    FLD_FULL_MAC(VIDX, 232 + 1, 12, 0, cond_tti2pcl_UseVIDX_1),
    FLD_MAC(OUTLIF_TYPE, 232 + 15, 2),
    FLD_FULL_MAC(MLL_POINTER, 232 + 17, 16, 0, cond_tti2pcl_OutLIF_Type_MLL),
    FLD_FULL_MAC(TUNNELPTR, 232 + 17, 14, 0, cond_tti2pcl_OutLIF_Type_Tunnel),
    FLD_FULL_MAC(TS_PASSENGER_TYPE, 232 + 31, 2, 0, cond_tti2pcl_OutLIF_Type_Tunnel),
    FLD_FULL_MAC(ARPPTR, 232 + 17, 16, 0, cond_tti2pcl_OutLIF_Type_LinkLayer),
    FLD_FULL_MAC(TRGDEVICE, 232 + 33, 2, 7, NULL),
    FLD_MAC(SRCISLOOPBACK, 267, 1),
    FLD_FULL_MAC(VR_ID, 268, 12, 0, cond_tti2pcl_RedirectCmd_4),
    FLD_FULL_MAC(POLICYRTTINDEX, 268, 20, 0, cond_tti2pcl_RedirectCmd_2),
    FLD_FULL_MAC(ENABLESOURCELOCALFILTERING, 268, 1, 0, cond_tti2pcl_RedirectCmd_3),
    FLD_FULL_MAC(FLOODMLLPOINTER, 269, 16, 0, cond_tti2pcl_RedirectCmd_3),
    FLD_FULL_MAC(TRGPORT_VIRTUALSRCPORT, 268, 6, 0, cond_tti2pcl_RedirectCmd_5),
    FLD_FULL_MAC(TRGDEVICE_VIRTUALSRCDEV, 274, 5, 0, cond_tti2pcl_RedirectCmd_5),
    FLD_MAC(L2_VALID, 288, 1),
    FLD_MAC(PKTHASH, 289, 6),
    FLD_MAC(SRCDEVISOWN, 295, 1),
    FLD_MAC(EGRESSFILTERREGISTERED, 296, 1),
    FLD_MAC(PCLASSIGNEDSSTID, 297, 1),
    FLD_MAC(SST_ID, 298, 5),
    FLD_FULL_MAC(ROUTED, 303, 1, 0, cond_tti2pcl_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(EGRESSFILTEREN, 303, 1, 0, cond_tti2pcl_PacketCmd_FromCPU),
    FLD_FULL_MAC(ORIGISTRUNK, 304, 1, 0, cond_tti2pcl_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(EXCLUDEISTRUNK, 304, 1, 0, cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_1),
    FLD_FULL_MAC(ORIGSRCPORT, 306, 6, 0, cond_tti2pcl_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_0),
    FLD_FULL_MAC(ORIGSRCTRUNK, 305, 8, 0, cond_tti2pcl_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDTRUNK, 305, 8, 0, cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDDEVISLOCAL, 305, 1, 0, cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(EXCLUDEDPORT, 306, 6, 0, cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(MAILBOXTONEIGHBORCPU, 311, 1, 0, cond_tti2pcl_PacketCmd_FromCPU_AND_UseVIDX_0),
    FLD_MAC(IPXPROTOCOL, 313, 8),
    FLD_MAC(ISARPREPLY, 321, 1),
    FLD_MAC(L4VALID, 322, 1),
    FLD_MAC(SYNWITHDATA, 323, 1),
    FLD_MAC(ISSYN, 324, 1),
    FLD_MAC(IPV6_EH, 325, 1),
    FLD_MAC(IPV6HBHEXT, 326, 1),
    FLD_MAC(SRCDEV, 327, 9),
    FLD_MAC(IPXHEADERTOTALLENGTH, 336, 16),
    FLD_MAC(MACSAARPSAMISMATCH, 352, 1),
    FLD_MAC(MAC2ME, 353, 1),
    FLD_MAC(IPLEGAL, 354, 1),
    FLD_MAC(ISIPV4, 355, 1),
    FLD_MAC(ISIPV6, 356, 1),
    FLD_MAC(ISARP, 357, 1),
    FLD_MAC(L4BYTES3_0, 358, 32),
    FLD_MAC(L4BYTE13, 390, 8),
    FLD_FULL_MAC(SIP_0_31, 398, 32, 0, NULL),
    FLD_FULL_MAC(SIP_32_63, 398+32, 32, 0, NULL),
    FLD_FULL_MAC(SIP_64_95, 398+64, 32, 0, NULL),
    FLD_FULL_MAC(SIP_96_127, 398+96, 32, 0, NULL),
    FLD_FULL_MAC(DIP_0_31, 526, 32, 0, cond_tti2pcl_IsIPv4_ForKey_0),
    FLD_FULL_MAC(DIP_32_63, 526+32, 32, 0, cond_tti2pcl_IsIPv4_ForKey_0),
    FLD_FULL_MAC(DIP_64_95, 526+64, 32, 0, cond_tti2pcl_IsIPv4_ForKey_0),
    FLD_FULL_MAC(DIP_96_127, 526+96, 32, 0, cond_tti2pcl_IsIPv4_ForKey_0),
    FLD_FULL_MAC(DIP, 526, 32, 0, cond_tti2pcl_IsIPv4_ForKey_1),
    FLD_FULL_MAC(IPV4_BAD_GRE_HEADER, 558, 1, 0, cond_tti2pcl_IsIPv4_ForKey_1),
    FLD_FULL_MAC(IPV4OPTIONFIELD, 559, 1, 0, cond_tti2pcl_IsIPv4_ForKey_1),
    FLD_FULL_MAC(FRAGMENTED, 560, 1, 0, cond_tti2pcl_IsIPv4_ForKey_1),
    FLD_FULL_MAC(IPV4_DF, 561, 1, 0, cond_tti2pcl_IsIPv4_ForKey_1),
    FLD_FULL_MAC(IP_FRAGMENTED, 562, 2, 0, cond_tti2pcl_IsIPv4_ForKey_1),
    FLD_FULL_MAC(MACSA_0_31, 654, 32, 0, NULL),
    FLD_FULL_MAC(MACSA_32_48, 654+32, 16, 0, NULL),
    FLD_FULL_MAC(MACDA_0_31, 702, 32, 0, NULL),
    FLD_FULL_MAC(MACDA_32_48, 702+32, 16, 0, NULL),
    FLD_MAC(L2ENCAPSULATION, 750, 2),
    FLD_MAC(ISBC, 752, 1),
    FLD_MAC(TRGTAGGED, 753, 1),
    FLD_MAC(UDB0, 754, 8),
    FLD_MAC(UDB1, 762, 8),
    FLD_MAC(UDB2, 770, 8),
    FLD_MAC(UDB3, 778, 8),
    FLD_MAC(UDB4, 786, 8),
    FLD_MAC(UDB5, 794, 8),
    FLD_MAC(UDB6, 802, 8),
    FLD_MAC(UDB7, 810, 8),
    FLD_MAC(UDB8, 818, 8),
    FLD_MAC(UDB9, 826, 8),
    FLD_MAC(UDB10, 834, 8),
    FLD_MAC(UDB11, 842, 8),
    FLD_MAC(UDB12, 850, 8),
    FLD_MAC(UDB13, 858, 8),
    FLD_MAC(UDB14, 866, 8),
    FLD_MAC(UDB15, 874, 8),
    FLD_MAC(UDB16, 882, 8),
    FLD_MAC(UDB17, 890, 8),
    FLD_MAC(UDB18, 898, 8),
    FLD_MAC(UDB19, 906, 8),
    FLD_MAC(UDB20, 914, 8),
    FLD_MAC(UDB21, 922, 8),
    FLD_MAC(UDB22, 930, 8),
    FLD_MAC(UDB_BYTESVALID, 938, 23),
    FLD_MAC(PACKETTOS, 961, 8),
    FLD_MAC(PACKETUP, 969, 3),
    FLD_MAC(ISIP_FORKEY, 972, 1),
    FLD_MAC(ISIPV4_FORKEY, 973, 1),
    FLD_MAC(ISIPV6_FORKEY, 974, 1),
    FLD_MAC(QINQ, 975, 1),
    FLD_MAC(FLOWSUBTEMPLATE, 976, 8),
    FLD_MAC(IP_HEADER_INFO, 984, 2),
    FLD_MAC(ISND, 986, 1),
    FLD_MAC(PKTTYPEFORKEY, 987, 3),
    FLD_MAC(IPV6FLOWLABEL, 990, 20),
    FLD_MAC(MACDATYPE, 1010, 2),
    FLD_MAC(MPLS_OUTERLABEL, 1012, 20),
    FLD_MAC(MPLS_OUTEREXP, 1032, 3),
    FLD_MAC(MPLS_OUTERS_BIT, 1035, 1),
    FLD_MAC(ISIPV6LINKLOCAL, 1036, 1),
    FLD_MAC(ISIPV6MLD, 1037, 1),
    FLD_MAC(SOLICITATIONMULTICASTMESSAGE, 1038, 1),
    FLD_MAC(ENABLEBRIDGE, 1039, 1),
    FLD_MAC(BYPASSBRIDGE, 1040, 1),
    FLD_MAC(BYPASSINGRESSPIPE, 1041, 1),
    FLD_MAC(L2ECHO, 1042, 1),
    FLD_MAC(ISMPLS, 1043, 1),
    FLD_MAC(OUTERSRCTAG, 1044, 1),
    FLD_MAC(PKTISLOOPED, 1045, 1),
    FLD_MAC(DROPONSOURCE, 1046, 1),
    FLD_MAC(MPLSCMD, 1047, 3),
    FLD_MAC(MPLSLABEL, 1050, 20),
    FLD_MAC(FLOWID, 1070, 20),
    FLD_MAC(PSEUDOWIREID, 1090, 7),
    FLD_MAC(SRC_IS_PE, 1097, 1),
    FLD_MAC(LLT, 1098, 3),
    FLD_MAC(ENABLEBRIDGELOGFLOW, 1101, 1),
    FLD_MAC(ENABLEROUTERLOGFLOW, 1102, 1),
    FLD_MAC(ENABLEDSATAGMANIPULATION, 1103, 1),
    FLD_MAC(CFI0, 1104, 1),
    FLD_MAC(VLAN1_EXIST, 1105, 1),
    FLD_MAC(VID1, 1106, 12),
    FLD_MAC(CFI1, 1118, 1),
    FLD_MAC(UP1, 1119, 3),
    FLD_MAC(ORIGVID, 1122, 12),
    FLD_MAC(QCNPACKET, 1134, 1),
    FLD_FULL_MAC(ORIGQCNPRIO, 1135, 3, 0, cond_tti2pcl_QCNPacket_1),
    FLD_FULL_MAC(PACKETISCNTAGGED, 1135, 1, 0, cond_tti2pcl_QCNPacket_0),
    FLD_MAC(CPIDMATCH, 1136, 1),
    FLD_MAC(DOROUTEHA, 1138, 1),
    FLD_MAC(MPLS_LABEL0, 1139, 20),
    FLD_MAC(MPLS_LABEL1, 1159, 20),
    FLD_MAC(MPLS_LABEL2, 1179, 20),
    FLD_MAC(HASHMASKINDEX, 1199, 4),
    FLD_MAC(USEINTERFACEBASEDINDEX, 1203, 1),
    FLD_MAC(ISPTP, 1204, 1),
    FLD_MAC(PTPTRIGGERTYPE, 1205, 2)
};

/* PCL to Bridge */
static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_TunnelTerminated_TT[] =
{
    COND_MAC(LAST, 40, 1, 1, EQUAL) /* (<TunnelTerminated> == TT) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_TunnelTerminated_NoTT_AND_IsPTP_FALSE[] =
{
    COND_MAC(AND, 40, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT */
    COND_MAC(LAST, 42, 1, 0, EQUAL), /*  <IsPTP>==FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_TunnelTerminated_NoTT_AND_IsPTP_TRUE[] =
{
    COND_MAC(AND, 40, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT */
    COND_MAC(LAST, 42, 1, 1, EQUAL), /*  <IsPTP>==TRUE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_PacketCmd_NOT_FromCPU[] =
{
    COND_MAC(LAST, 89, 3, 7, DIFFER) /* (<PacketCmd> != FromCPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_PacketCmd_FromCPU[] =
{
    COND_MAC(LAST, 89, 3, 7, EQUAL) /* (<PacketCmd> == FromCPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_UseVIDX_0[] =
{
    COND_MAC(LAST, 95, 1, 0, EQUAL) /* (UseVIDX == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_UseVIDX_1[] =
{
    COND_MAC(LAST, 95, 1, 1, EQUAL) /* (<UseVIDX> ==1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_UseVIDX_0_AND_TargetIsTrunk_0[] =
{
    COND_MAC(AND, 95, 1, 0, EQUAL), /* (<UseVIDX> ==0) */
    COND_MAC(LAST, 96, 1, 0, EQUAL) /* (<TargetIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_UseVIDX_0_AND_TargetIsTrunk_1[] =
{
    COND_MAC(AND, 95, 1, 0, EQUAL), /* (<UseVIDX> ==0) */
    COND_MAC(LAST, 96, 1, 1, EQUAL) /* (<TargetIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_1[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(LAST, 95, 1, 1, EQUAL) /* (<UseVIDX> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_0[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(LAST, 95, 1, 0, EQUAL) /* (<UseVIDX> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_0[] =
{
    COND_MAC(AND, 89, 3, 7, DIFFER), /* (<PacketCmd> != FromCPU) */
    COND_MAC(LAST, 117, 1, 0, EQUAL) /* (<OrigIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_1[] =
{
    COND_MAC(AND, 89, 3, 7, DIFFER), /* (<PacketCmd> != FromCPU) */
    COND_MAC(LAST, 117, 1, 1, EQUAL) /* (<OrigIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(AND, 95, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 117, 1, 1, EQUAL) /* (<ExcludeIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(AND, 95, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 117, 1, 0, EQUAL) /* (<ExcludeIsTrunk> ==0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_TunnelStart_1[] =
{
    COND_MAC(LAST, 139, 1, 1, EQUAL) /* (<TunnelStart> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_TunnelStart_NOT_1[] =
{
    COND_MAC(LAST, 139, 1, 1, DIFFER) /* (<TunnelStart> != 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_RouteToEntry_1[] =
{
    COND_MAC(LAST, 206, 1, 1, EQUAL) /* (<RouteToEntry> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_RouteToEntry_NOT_1[] =
{
    COND_MAC(LAST, 206, 1, 1, DIFFER) /* (<RouteToEntry> != 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_IsPTP_FALSE[] =
{
    COND_MAC(LAST, 42, 1, 0, EQUAL) /* (<IsPTP> == FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_pcl2l2i_IsPTP_TRUE[] =
{
    COND_MAC(LAST, 42, 1, 1, EQUAL) /* (<IsPTP> == TRUE) */
};


static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat2pcl2l2i[] =
{
    FLD_MAC(FIRSTBUFFER, 0, 14),
    FLD_MAC(MDB, 14, 1),
    FLD_MAC(BANK, 15, 1),
    FLD_MAC(BYTECOUNT, 16, 14),
    FLD_MAC(RECALCCRC, 30, 1),
    FLD_MAC(MARVELLTAGGED, 31, 1),
    FLD_MAC(LOCALDEVSRCPORT, 32, 6),
    FLD_MAC(SRCTAGGED, 38, 1),
    FLD_MAC(MARVELLTAGGEDEXTENDED, 39, 1),
    FLD_MAC(TUNNELTERMINATED, 40, 1),
    FLD_FULL_MAC(INNERPKTTYPE, 41, 2, 0, cond_pcl2l2i_TunnelTerminated_TT),
    FLD_FULL_MAC(L2ECHO, 41, 1, 0, cond_pcl2l2i_TunnelTerminated_NoTT_AND_IsPTP_FALSE),
    FLD_FULL_MAC(ISPTP, 42, 1, 0, cond_pcl2l2i_TunnelTerminated_NoTT_AND_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 41, 1, 0, cond_pcl2l2i_TunnelTerminated_NoTT_AND_IsPTP_TRUE),
    FLD_FULL_MAC(ISPTP, 42, 1, 0, cond_pcl2l2i_TunnelTerminated_NoTT_AND_IsPTP_TRUE),
    FLD_FULL_MAC(INNERL3OFFSET, 43, 1, 0, NULL),
    FLD_MAC(MODIFYUP, 44, 1),
    FLD_MAC(UP, 45, 3),
    FLD_MAC(MODIFYDSCP_EXP, 48, 1),
    FLD_FULL_MAC(QOSPROFILE, 49, 7, 0, cond_pcl2l2i_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(CPU2CPUMIRROR, 49, 1, 0, cond_pcl2l2i_PacketCmd_FromCPU),
    FLD_FULL_MAC(CONTROLTC, 50, 1, 0, cond_pcl2l2i_PacketCmd_FromCPU),
    FLD_FULL_MAC(FROMCPUTC, 51, 3, 0, cond_pcl2l2i_PacketCmd_FromCPU),
    FLD_FULL_MAC(FROMCPUDP, 54, 2, 0, cond_pcl2l2i_PacketCmd_FromCPU),
    FLD_MAC(TRGTAGGED, 56, 1),
    FLD_MAC(VIDMODIFIED, 57, 1),
    FLD_MAC(VID, 58, 12),
    FLD_MAC(LOCALDEVICESRCTRUNKID, 70, 7),
    FLD_FULL_MAC(PKTHASH, 77, 3, 0, NULL),
    FLD_MAC(SRCDEVISOWN, 80, 1),
    FLD_MAC(EGRESSFILTERREGISTERED, 81, 1),
    FLD_MAC(MACDATYPE, 82, 2),
    FLD_MAC(SST_ID, 84, 5),
    FLD_MAC(PACKETCMD, 89, 3),
    FLD_MAC(RXSNIFF, 92, 1),
    FLD_MAC(ISIP, 93, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 94, 1, 1, NULL),
    FLD_MAC(USEVIDX, 95, 1),
    FLD_FULL_MAC(VIDX, 96, 1, 0, cond_pcl2l2i_UseVIDX_1),
    FLD_FULL_MAC(TARGETISTRUNK, 96, 1, 0, cond_pcl2l2i_UseVIDX_0),
    FLD_FULL_MAC(VIDX, 97, 11, 1, cond_pcl2l2i_UseVIDX_1),
    FLD_FULL_MAC(TRGPORT_VIRTUALSRCPORT, 97, 6, 0, cond_pcl2l2i_UseVIDX_0_AND_TargetIsTrunk_0),
    FLD_FULL_MAC(TRGDEVICE_VIRTUALSRCDEV, 103, 5, 0, cond_pcl2l2i_UseVIDX_0_AND_TargetIsTrunk_0),
    FLD_FULL_MAC(TRGTRUNKID, 101, 7, 0, cond_pcl2l2i_UseVIDX_0_AND_TargetIsTrunk_1),
    FLD_MAC(CPUCODE, 108, 8),
    FLD_FULL_MAC(ROUTED, 116, 1, 0, cond_pcl2l2i_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(EGRESSFILTEREN, 116, 1, 0, cond_pcl2l2i_PacketCmd_FromCPU),
    FLD_FULL_MAC(ORIGISTRUNK, 117, 1, 0, cond_pcl2l2i_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(EXCLUDEISTRUNK, 117, 1, 0, cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_1),
    FLD_FULL_MAC(ORIGSRCPORT, 119, 6, 0, cond_pcl2l2i_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_0),
    FLD_FULL_MAC(ORIGSRCTRUNK, 118, 7, 0, cond_pcl2l2i_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDTRUNK, 118, 7, 0, cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDDEVISLOCAL, 118, 1, 0, cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(EXCLUDEDPORT, 119, 6, 0, cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(MAILBOXTONEIGHBORCPU, 124, 1, 0, cond_pcl2l2i_PacketCmd_FromCPU_AND_UseVIDX_0),
    FLD_MAC(L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET, 125, 4),
    FLD_MAC(OUTERISLLCNONSNAP, 129, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 130, 1, 2, NULL),
    FLD_MAC(TTL, 131, 8),
    FLD_MAC(TUNNELSTART, 139, 1),
    FLD_FULL_MAC(TUNNELPTR, 140, 13, 0, cond_pcl2l2i_TunnelStart_1),
    FLD_FULL_MAC(TUNNELTYPE, 153, 1, 0, cond_pcl2l2i_TunnelStart_1),
    FLD_FULL_MAC(ARPPTR, 140, 14, 0, cond_pcl2l2i_TunnelStart_NOT_1),
    FLD_MAC(IPM, 154, 1),
    FLD_FULL_MAC(PKTHASH, 155, 3, 3, NULL),
    FLD_MAC(IPXPROTOCOL, 158, 8),
    FLD_MAC(ISARPREPLY, 166, 1),
    FLD_MAC(L4VALID, 167, 1),
    FLD_MAC(TCP_UDPDESTPORT, 168, 16),
    FLD_MAC(INNERISLLCNONSNAP, 184, 1),
    FLD_MAC(SYNWITHDATA, 185, 1),
    FLD_MAC(ISIPV4, 186, 1),
    FLD_MAC(APPSPECIFICCPUCODE, 187, 1),
    FLD_MAC(ISSYN, 188, 1),
    FLD_MAC(PCLASSIGNEDSSTID, 196, 1),
    FLD_MAC(IPV4_DF, 197, 1),
    FLD_MAC(IPV4OPTIONFIELD, 198, 1),
    FLD_MAC(IPV6HBHEXT, 199, 1),
    FLD_MAC(IPV6NONHBHEXT, 200, 1),
    FLD_MAC(QOSPROFILEPRECEDENCE, 201, 1),
    FLD_MAC(BYPASSINGRESSPIPE, 202, 1),
    FLD_MAC(IPLEGAL, 203, 1),
    FLD_MAC(IPXMACMCMISMATCH, 204, 1),
    FLD_MAC(ROUTETOENTRY, 206, 1),
    FLD_FULL_MAC(RGEGRESSINTERFACE, 207, 13, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGPCEROUTEENTRY, 221, 1, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGARPINDEX, 222, 10, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGNHEVID, 232, 12, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGFWDCMD, 244, 3, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGMIRRORTOINGRESSANALYZERPORT, 247, 1, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGQOS_PRECEDENCE, 248, 1, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGQOS_PROFILEMARKINGEN, 249, 1, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGQOSPROFILE, 250, 7, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGMODIFYDSCP, 257, 2, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGMODIFYUP, 259, 2, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGICMP_REDIRECTCHECKEN, 261, 1, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGBYPASSTTLANDOPTIONCHECKS, 262, 1, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RGDECTTL, 263, 1, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(RSRVDCOPY, 264, 5, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(IPV6HBHOPTIONS, 269, 1, 0, cond_pcl2l2i_RouteToEntry_1),
    FLD_FULL_MAC(SIP_0_31, 207, 32, 0, cond_pcl2l2i_RouteToEntry_NOT_1),
    FLD_FULL_MAC(SIP_32_63, 207+32, 32, 0, cond_pcl2l2i_RouteToEntry_NOT_1),
    FLD_FULL_MAC(SIP_64_95, 207+64, 32, 0, cond_pcl2l2i_RouteToEntry_NOT_1),
    FLD_FULL_MAC(SIP_96_127, 207+96, 32, 0, cond_pcl2l2i_RouteToEntry_NOT_1),
    FLD_FULL_MAC(DIP_0_31, 335, 32, 0, NULL),
    FLD_FULL_MAC(DIP_32_63, 335+32, 32, 0, NULL),
    FLD_FULL_MAC(DIP_64_95, 335+64, 32, 0, NULL),
    FLD_FULL_MAC(DIP_96_127, 335+96, 32, 0, NULL),
    FLD_MAC(POLICYBASEDROUTED, 463, 1),
    FLD_FULL_MAC(POLICYRTTINDEX, 464, 14, 0, NULL),
    FLD_MAC(BYPASSBRIDGE_DUP, 478, 1),
    FLD_FULL_MAC(POLICYRTTINDEX, 479, 1, 14, NULL),
    FLD_MAC(IPXHEADERTOTALLENGTH, 480, 16),
    FLD_MAC(BYPASSBRIDGE, 496, 1),
    FLD_MAC(MACSAARPSAMISMATCH, 497, 1),
    FLD_MAC(SRCDEV, 498, 5),
    FLD_MAC(IGMPQUERY, 503, 1),
    FLD_MAC(IGMPNONQUERY, 504, 1),
    FLD_FULL_MAC(MACSA_0_31, 506, 32, 0, NULL),
    FLD_FULL_MAC(MACSA_32_48, 506+32, 16, 0, NULL),
    FLD_FULL_MAC(MACDA_0_31, 554, 32, 0, NULL),
    FLD_FULL_MAC(MACDA_32_48, 554+32, 16, 0, NULL),
    FLD_MAC(SOLICITATIONMULTICASTMESSAGE, 602, 1),
    FLD_MAC(IPV6ICMP, 603, 1),
    FLD_MAC(IPV6ICMPTYPE, 604, 8),
    FLD_MAC(IPXMCLINKLOCALPROT, 612, 1),
    FLD_MAC(IPV4RIPV1, 613, 1),
    FLD_FULL_MAC(TCPFLAGS, 614, 6, 0, NULL),
    FLD_MAC(FRAGMENTED, 620, 1),
    FLD_MAC(SRCTCPUDPPORTIS0, 621, 1),
    FLD_MAC(PRIOTAGGED, 622, 1),
    FLD_MAC(CFI, 623, 1),
    FLD_MAC(PKTISLOOPED, 624, 1),
    FLD_MAC(DROPONSOURCE, 625, 1),
    FLD_MAC(ORIGVID, 626, 12),
    FLD_FULL_MAC(CFMPKT, 638, 1, 0, cond_pcl2l2i_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 638, 1, 0, cond_pcl2l2i_IsPTP_TRUE),
    FLD_MAC(POLICEREN, 644, 1),
    FLD_MAC(POLICERPTR, 645, 12),
    FLD_MAC(BILLINGEN, 657, 1),
    FLD_MAC(DOROUTEHA, 658, 1),
    FLD_MAC(VRF_ID, 660, 12),
    FLD_MAC(TCP_UDP_SRCPORT, 672, 16),
    FLD_MAC(ETHERTYPE, 688, 16),
    FLD_MAC(VIRTUALINTERFACE, 704, 1)
};

/* Bridge to Router */
static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_TunnelTerminated_TT[] =
{
    COND_MAC(LAST, 40, 1, 1, EQUAL) /* (<TunnelTerminated> == TT) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_TunnelTerminated_NoTT_IsPTP_FALSE[] =
{
    COND_MAC(AND, 40, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT  */
    COND_MAC(LAST, 42, 1, 0, EQUAL) /* <IsPTP>==FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_TunnelTerminated_NoTT_IsPTP_TRUE[] =
{
    COND_MAC(AND, 40, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT  */
    COND_MAC(LAST, 42, 1, 1, EQUAL) /* <IsPTP>==TRUE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_PacketCmd_NOT_FromCPU[] =
{
    COND_MAC(LAST, 89, 3, 7, DIFFER) /* (<PacketCmd> != FromCPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_PacketCmd_FromCPU[] =
{
    COND_MAC(LAST, 89, 3, 7, EQUAL) /* (<PacketCmd> == FromCPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_UseVIDX_0[] =
{
    COND_MAC(LAST, 95, 1, 0, EQUAL) /* (UseVIDX == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_UseVIDX_1[] =
{
    COND_MAC(LAST, 95, 1, 1, EQUAL) /* (<UseVIDX> ==1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_UseVIDX_0_AND_TargetIsTrunk_0[] =
{
    COND_MAC(AND, 95, 1, 0, EQUAL), /* (<UseVIDX> == 0) */
    COND_MAC(LAST, 96, 1, 0, EQUAL) /* (<TargetIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_UseVIDX_0_AND_TargetIsTrunk_1[] =
{
    COND_MAC(AND, 95, 1, 0, EQUAL), /* (<UseVIDX> == 0) */
    COND_MAC(LAST, 96, 1, 1, EQUAL) /* (<TargetIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_1[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(LAST, 95, 1, 1, EQUAL) /* (<UseVIDX> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_1[] =
{
    COND_MAC(AND, 89, 3, 7, DIFFER), /* (<PacketCmd> != FromCPU) */
    COND_MAC(LAST, 117, 1, 1, EQUAL) /* (<OrigIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_0[] =
{
    COND_MAC(AND, 89, 3, 7, DIFFER), /* (<PacketCmd> != FromCPU) */
    COND_MAC(LAST, 117, 1, 0, EQUAL) /* (<OrigIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(AND, 95, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 117, 1, 1, EQUAL) /* (<ExcludeIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(AND, 95, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 117, 1, 0, EQUAL) /* (<ExcludeIsTrunk> ==0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_0_AND_MLLEn_0[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL),  /* (<PacketCmd> == FromCPU) */
    COND_MAC(LAST, 95, 1, 0, EQUAL)  /* (<UseVIDX> == 0) */
     /* (<MLLEn> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_TunnelStart_1[] =
{
    COND_MAC(LAST, 139, 1, 1, EQUAL) /* (<TunnelStart> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_TunnelStart_NOT_1[] =
{
    COND_MAC(LAST, 139, 1, 1, DIFFER) /* (<TunnelStart> != 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_RouteToEntry_1[] =
{
    COND_MAC(LAST, 206, 1, 1, EQUAL) /* (<RouteToEntry> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_RouteToEntry_NOT_1[] =
{
    COND_MAC(LAST, 206, 1, 1, DIFFER) /* (<RouteToEntry> != 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_IsPTP_FALSE[] =
{
    COND_MAC(LAST, 42, 1, 0, EQUAL) /* (<IsPTP> == FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_l2i2rtr_IsPTP_TRUE[] =
{
    COND_MAC(LAST, 42, 1, 1, EQUAL) /* (<IsPTP> == TRUE) */
};

static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat2l2i2rtr[] =
{
    FLD_MAC(FIRSTBUFFER, 0, 14),
    FLD_MAC(MDB, 14, 1),
    FLD_MAC(BANK, 15, 1),
    FLD_MAC(BYTECOUNT, 16, 14),
    FLD_MAC(RECALCCRC, 30, 1),
    FLD_MAC(MARVELLTAGGED, 31, 1),
    FLD_MAC(LOCALDEVSRCPORT, 32, 6),
    FLD_MAC(SRCTAGGED, 38, 1),
    FLD_MAC(MARVELLTAGGEDEXTENDED, 39, 1),
    FLD_MAC(TUNNELTERMINATED, 40, 1),
    FLD_FULL_MAC(INNERPKTTYPE, 41, 2, 0, cond_l2i2rtr_TunnelTerminated_TT),
    FLD_FULL_MAC(L2ECHO, 41, 1, 0, cond_l2i2rtr_TunnelTerminated_NoTT_IsPTP_FALSE),
    FLD_FULL_MAC(ISPTP, 42, 1, 0, cond_l2i2rtr_TunnelTerminated_NoTT_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 41, 1, 0, cond_l2i2rtr_TunnelTerminated_NoTT_IsPTP_TRUE),
    FLD_FULL_MAC(ISPTP, 42, 1, 0, cond_l2i2rtr_TunnelTerminated_NoTT_IsPTP_TRUE),
    FLD_FULL_MAC(INNERL3OFFSET, 43, 1, 0, NULL),
    FLD_MAC(MODIFYUP, 44, 1),
    FLD_MAC(UP, 45, 3),
    FLD_MAC(MODIFYDSCP_EXP, 48, 1),
    FLD_FULL_MAC(QOSPROFILE, 49, 7, 0, cond_l2i2rtr_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(CPU2CPUMIRROR, 49, 1, 0, cond_l2i2rtr_PacketCmd_FromCPU),
    FLD_FULL_MAC(CONTROLTC, 50, 1, 0, cond_l2i2rtr_PacketCmd_FromCPU),
    FLD_FULL_MAC(FROMCPUTC, 51, 3, 0, cond_l2i2rtr_PacketCmd_FromCPU),
    FLD_FULL_MAC(FROMCPUDP, 54, 2, 0, cond_l2i2rtr_PacketCmd_FromCPU),
    FLD_MAC(TRGTAGGED, 56, 1),
    FLD_MAC(VIDMODIFIED, 57, 1),
    FLD_MAC(VID, 58, 12),
    FLD_MAC(LOCALDEVICESRCTRUNKID, 70, 7),
    FLD_FULL_MAC(PKTHASH, 77, 3, 0, NULL),
    FLD_MAC(SRCDEVISOWN, 80, 1),
    FLD_MAC(EGRESSFILTERREGISTERED, 81, 1),
    FLD_MAC(MACDATYPE, 82, 2),
    FLD_MAC(SST_ID, 84, 5),
    FLD_MAC(PACKETCMD, 89, 3),
    FLD_MAC(RXSNIFF, 92, 1),
    FLD_MAC(ISIP, 93, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 94, 1, 1, NULL),
    FLD_MAC(USEVIDX, 95, 1),
    FLD_FULL_MAC(VIDX, 96, 1, 0, cond_l2i2rtr_UseVIDX_1),
    FLD_FULL_MAC(TARGETISTRUNK, 96, 1, 0, cond_l2i2rtr_UseVIDX_0),
    FLD_FULL_MAC(VIDX, 97, 11, 1, cond_l2i2rtr_UseVIDX_1),
    FLD_FULL_MAC(TRGPORT_VIRTUALSRCPORT, 97, 6, 0, cond_l2i2rtr_UseVIDX_0_AND_TargetIsTrunk_0),
    FLD_FULL_MAC(TRGDEVICE_VIRTUALSRCDEV, 103, 5, 0, cond_l2i2rtr_UseVIDX_0_AND_TargetIsTrunk_0),
    FLD_FULL_MAC(TRGTRUNKID, 101, 7, 0, cond_l2i2rtr_UseVIDX_0_AND_TargetIsTrunk_1),
    FLD_MAC(CPUCODE, 108, 8),
    FLD_FULL_MAC(ROUTED, 116, 1, 0, cond_l2i2rtr_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(EGRESSFILTEREN, 116, 1, 0, cond_l2i2rtr_PacketCmd_FromCPU),
    FLD_FULL_MAC(ORIGISTRUNK, 117, 1, 0, cond_l2i2rtr_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(EXCLUDEISTRUNK, 117, 1, 0, cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_1),
    FLD_FULL_MAC(ORIGSRCPORT, 119, 6, 0, cond_l2i2rtr_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_0),
    FLD_FULL_MAC(ORIGSRCTRUNK, 118, 7, 0, cond_l2i2rtr_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDTRUNK, 118, 7, 0, cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDDEVISLOCAL, 118, 1, 0, cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(EXCLUDEDPORT, 119, 6, 0, cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(MAILBOXTONEIGHBORCPU, 124, 1, 0, cond_l2i2rtr_PacketCmd_FromCPU_AND_UseVIDX_0_AND_MLLEn_0),
    FLD_MAC(L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET, 125, 4),
    FLD_MAC(OUTERISLLCNONSNAP, 129, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 130, 1, 2, NULL),
    FLD_MAC(TTL, 131, 8),
    FLD_MAC(TUNNELSTART, 139, 1),
    FLD_FULL_MAC(TUNNELPTR, 140, 13, 0, cond_l2i2rtr_TunnelStart_1),
    FLD_FULL_MAC(TUNNELTYPE, 153, 1, 0, cond_l2i2rtr_TunnelStart_1),
    FLD_FULL_MAC(ARPPTR, 140, 14, 0, cond_l2i2rtr_TunnelStart_NOT_1),
    FLD_MAC(IPM, 154, 1),
    FLD_FULL_MAC(PKTHASH, 155, 3, 3, NULL),
    FLD_MAC(IPXPROTOCOL, 158, 8),
    FLD_MAC(ISARPREPLY, 166, 1),
    FLD_MAC(L4VALID, 167, 1),
    FLD_MAC(TCP_UDPDESTPORT, 168, 16),
    FLD_MAC(INNERISLLCNONSNAP, 184, 1),
    FLD_MAC(SYNWITHDATA, 185, 1),
    FLD_MAC(ISIPV4, 186, 1),
    FLD_MAC(APPSPECIFICCPUCODE, 187, 1),
    FLD_MAC(ISSYN, 188, 1),
    FLD_MAC(PCLASSIGNEDSSTID, 196, 1),
    FLD_MAC(IPV4_DF, 197, 1),
    FLD_MAC(IPV4OPTIONFIELD, 198, 1),
    FLD_MAC(IPV6HBHEXT, 199, 1),
    FLD_MAC(IPV6NONHBHEXT, 200, 1),
    FLD_MAC(QOSPROFILEPRECEDENCE, 201, 1),
    FLD_MAC(BYPASSINGRESSPIPE, 202, 1),
    FLD_MAC(IPLEGAL, 203, 1),
    FLD_MAC(IPXMACMCMISMATCH, 204, 1),
    FLD_MAC(BADIPXBC, 205, 1),
    FLD_MAC(ROUTETOENTRY, 206, 1),
    FLD_FULL_MAC(RGEGRESSINTERFACE, 207, 13, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGPCEROUTEENTRY, 221, 1, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGARPINDEX, 222, 10, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGNHEVID, 232, 12, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGFWDCMD, 244, 3, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGMIRRORTOINGRESSANALYZERPORT, 247, 1, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGQOS_PRECEDENCE, 248, 1, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGQOS_PROFILEMARKINGEN, 249, 1, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGQOSPROFILE, 250, 7, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGMODIFYDSCP, 257, 2, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGMODIFYUP, 259, 2, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGICMP_REDIRECTCHECKEN, 261, 1, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGBYPASSTTLANDOPTIONCHECKS, 262, 1, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RGDECTTL, 263, 1, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(RSRVDCOPY, 264, 5, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(IPV6HBHOPTIONS, 269, 1, 0, cond_l2i2rtr_RouteToEntry_1),
    FLD_FULL_MAC(SIP_0_31, 207, 32, 0, cond_l2i2rtr_RouteToEntry_NOT_1),
    FLD_FULL_MAC(SIP_32_63, 207+32, 32, 0, cond_l2i2rtr_RouteToEntry_NOT_1),
    FLD_FULL_MAC(SIP_64_95, 207+64, 32, 0, cond_l2i2rtr_RouteToEntry_NOT_1),
    FLD_FULL_MAC(SIP_96_127, 207+96, 32, 0, cond_l2i2rtr_RouteToEntry_NOT_1),
    FLD_FULL_MAC(DIP_0_31, 335, 32, 0, NULL),
    FLD_FULL_MAC(DIP_32_63, 335+32, 32, 0, NULL),
    FLD_FULL_MAC(DIP_64_95, 335+64, 32, 0, NULL),
    FLD_FULL_MAC(DIP_96_127, 335+96, 32, 0, NULL),
    FLD_MAC(POLICYBASEDROUTED, 463, 1),
    FLD_FULL_MAC(POLICYRTTINDEX, 464, 14, 0, NULL),
    FLD_MAC(BYPASSBRIDGE_DUP, 478, 1),
    FLD_FULL_MAC(POLICYRTTINDEX, 479, 1, 14, NULL),
    FLD_MAC(IPXHEADERTOTALLENGTH, 480, 16),
    FLD_MAC(IPV6SITEID, 496, 1),
    FLD_MAC(IPV4UNICASTEN, 497, 1),
    FLD_MAC(IPV4MULTICASTEN, 498, 1),
    FLD_MAC(IPV6UNICASTEN, 499, 1),
    FLD_MAC(IPV6MULTICASTEN, 500, 1),
    FLD_MAC(IPMGMNTEN, 501, 1),
    FLD_MAC(DOROUTE, 502, 1),
    FLD_FULL_MAC(MACSA_0_31, 503, 32, 0, NULL),
    FLD_FULL_MAC(MACSA_32_48, 503+32, 16, 0, NULL),
    FLD_MAC(SRCDEV, 551, 5),
    FLD_MAC(CFI, 556, 1),
    FLD_MAC(PKTISLOOPED, 557, 1),
    FLD_MAC(DROPONSOURCE, 558, 1),
    FLD_MAC(ORIGVID, 559, 12),
    FLD_FULL_MAC(CFMPKT, 571, 1, 0, cond_l2i2rtr_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 571, 1, 0, cond_l2i2rtr_IsPTP_TRUE),
    FLD_MAC(POLICEREN, 577, 1),
    FLD_MAC(POLICERPTR, 578, 12),
    FLD_MAC(BILLINGEN, 590, 1),
    FLD_MAC(DOROUTEHA, 591, 1),
    FLD_MAC(VRF_ID, 593, 12),
    FLD_MAC(TCP_UDP_SRCPORT, 605, 16)
};

/* Ingress Policer Outgoing */
static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_TunnelTerminated_TT[] =
{
    COND_MAC(LAST, 40, 1, 1, EQUAL) /* (<TunnelTerminated> == TT) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_TunnelTerminated_NoTT_IsPTP_FALSE[] =
{
    COND_MAC(AND, 40, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT  */
    COND_MAC(LAST, 42, 1, 0, EQUAL) /* <IsPTP>==FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_TunnelTerminated_NoTT_IsPTP_TRUE[] =
{
    COND_MAC(AND, 40, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT  */
    COND_MAC(LAST, 42, 1, 1, EQUAL) /* <IsPTP>==TRUE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_PacketCmd_NOT_FromCPU[] =
{
    COND_MAC(LAST, 89, 3, 7, DIFFER) /* (<PacketCmd> != FromCPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_PacketCmd_FromCPU[] =
{
    COND_MAC(LAST, 89, 3, 7, EQUAL) /* (<PacketCmd> == FromCPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_UseVIDX_1[] =
{
    COND_MAC(LAST, 95, 1, 1, EQUAL) /* (<UseVIDX> ==1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_UseVIDX_0[] =
{
    COND_MAC(LAST, 95, 1, 0, EQUAL) /* (<UseVIDX> ==0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_UseVIDX_0_AND_TargetIsTrunk_0[] =
{
    COND_MAC(AND, 95, 1, 0, EQUAL), /* (<UseVIDX> ==0) */
    COND_MAC(LAST, 96, 1, 0, EQUAL) /* (<TargetIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_UseVIDX_0_AND_TargetIsTrunk_1[] =
{
    COND_MAC(AND, 95, 1, 0, EQUAL), /* (<UseVIDX> ==0) */
    COND_MAC(LAST, 96, 1, 1, EQUAL) /* (<TargetIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_1[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(LAST, 95, 1, 1, EQUAL) /* (<UseVIDX> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_0[] =
{
    COND_MAC(AND, 89, 3, 7, DIFFER), /* (<PacketCmd> != FromCPU) */
    COND_MAC(LAST, 117, 1, 0, EQUAL) /* (<OrigIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_1[] =
{
    COND_MAC(AND, 89, 3, 7, DIFFER), /* (<PacketCmd> != FromCPU) */
    COND_MAC(LAST, 117, 1, 1, EQUAL) /* (<OrigIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(AND, 95, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 117, 1, 1, EQUAL) /* (<ExcludeIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(AND, 95, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 117, 1, 0, EQUAL) /* (<ExcludeIsTrunk> ==0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_0[] =
{
    COND_MAC(AND, 89, 3, 7, EQUAL), /* (<PacketCmd> == FromCPU) */
    COND_MAC(LAST, 95, 1, 0, EQUAL), /* (<UseVIDX> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_MLLEn_1[] =
{
    COND_MAC(LAST, 200, 1, 1, EQUAL) /* (<MLLEn> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_MLLEn_0_AND_TunnelStart_1[] =
{
    COND_MAC(AND, 200, 1, 0, EQUAL), /* (<MLLEn> == 0) */
    COND_MAC(LAST, 139, 1, 1, EQUAL) /* (<TunnelStart> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_MLLEn_0_AND_TunnelStart_0[] =
{
    COND_MAC(AND, 200, 1, 0, EQUAL), /* (<MLLEn> == 0) */
    COND_MAC(LAST, 139, 1, 0, EQUAL) /* (<TunnelStart> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_IsPTP_FALSE[] =
{
    COND_MAC(LAST, 42, 1, 0, EQUAL) /* (<IsPTP> == FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_plr_outgoing_IsPTP_TRUE[] =
{
    COND_MAC(LAST, 42, 1, 1, EQUAL) /* (<IsPTP> == TRUE) */
};

static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat2plr_outgoing[] =
{
    FLD_MAC(FIRSTBUFFER, 0, 14),
    FLD_MAC(MDB, 14, 1),
    FLD_MAC(BANK, 15, 1),
    FLD_MAC(BYTECOUNT, 16, 14),
    FLD_MAC(RECALCCRC, 30, 1),
    FLD_MAC(MARVELLTAGGED, 31, 1),
    FLD_MAC(LOCALDEVSRCPORT, 32, 6),
    FLD_MAC(SRCTAGGED, 38, 1),
    FLD_MAC(MARVELLTAGGEDEXTENDED, 39, 1),
    FLD_MAC(TUNNELTERMINATED, 40, 1),
    FLD_FULL_MAC(INNERPKTTYPE, 41, 2, 0, cond_plr_outgoing_TunnelTerminated_TT),
    FLD_FULL_MAC(L2ECHO, 41, 1, 0, cond_plr_outgoing_TunnelTerminated_NoTT_IsPTP_FALSE),
    FLD_FULL_MAC(ISPTP, 42, 1, 0, cond_plr_outgoing_TunnelTerminated_NoTT_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 41, 1, 0, cond_plr_outgoing_TunnelTerminated_NoTT_IsPTP_TRUE),
    FLD_FULL_MAC(ISPTP, 42, 1, 0, cond_plr_outgoing_TunnelTerminated_NoTT_IsPTP_TRUE),
    FLD_FULL_MAC(INNERL3OFFSET, 43, 1, 0, NULL),
    FLD_MAC(MODIFYUP, 44, 1),
    FLD_MAC(UP, 45, 3),
    FLD_MAC(MODIFYDSCP_EXP, 48, 1),
    FLD_FULL_MAC(QOSPROFILE, 49, 7, 0, cond_plr_outgoing_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(CPU2CPUMIRROR, 49, 1, 0, cond_plr_outgoing_PacketCmd_FromCPU),
    FLD_FULL_MAC(CONTROLTC, 50, 1, 0, cond_plr_outgoing_PacketCmd_FromCPU),
    FLD_FULL_MAC(FROMCPUTC, 51, 3, 0, cond_plr_outgoing_PacketCmd_FromCPU),
    FLD_FULL_MAC(FROMCPUDP, 54, 2, 0, cond_plr_outgoing_PacketCmd_FromCPU),
    FLD_MAC(TRGTAGGED, 56, 1),
    FLD_MAC(VIDMODIFIED, 57, 1),
    FLD_MAC(VID, 58, 12),
    FLD_MAC(LOCALDEVICESRCTRUNKID, 70, 7),
    FLD_FULL_MAC(PKTHASH, 77, 3, 0, NULL),
    FLD_MAC(SRCDEVISOWN, 80, 1),
    FLD_MAC(EGRESSFILTERREGISTERED, 81, 1),
    FLD_MAC(MACDATYPE, 82, 2),
    FLD_MAC(SST_ID, 84, 5),
    FLD_MAC(PACKETCMD, 89, 3),
    FLD_MAC(RXSNIFF, 92, 1),
    FLD_MAC(ISIP, 93, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 94, 1, 0, NULL),
    FLD_MAC(USEVIDX, 95, 1),
    FLD_FULL_MAC(VIDX, 96, 1, 0, cond_plr_outgoing_UseVIDX_1),
    FLD_FULL_MAC(TARGETISTRUNK, 96, 1, 0, cond_plr_outgoing_UseVIDX_0),
    FLD_FULL_MAC(VIDX, 97, 11, 1, cond_plr_outgoing_UseVIDX_1),
    FLD_FULL_MAC(TRGPORT_VIRTUALSRCPORT, 97, 6, 0, cond_plr_outgoing_UseVIDX_0_AND_TargetIsTrunk_0),
    FLD_FULL_MAC(TRGDEVICE_VIRTUALSRCDEV, 103, 5, 0, cond_plr_outgoing_UseVIDX_0_AND_TargetIsTrunk_0),
    FLD_FULL_MAC(TRGTRUNKID, 101, 7, 0, cond_plr_outgoing_UseVIDX_0_AND_TargetIsTrunk_1),
    FLD_MAC(CPUCODE, 108, 8),
    FLD_FULL_MAC(ROUTED, 116, 1, 0, cond_plr_outgoing_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(EGRESSFILTEREN, 116, 1, 0, cond_plr_outgoing_PacketCmd_FromCPU),
    FLD_FULL_MAC(ORIGISTRUNK, 117, 1, 0, cond_plr_outgoing_PacketCmd_NOT_FromCPU),
    FLD_FULL_MAC(EXCLUDEISTRUNK, 117, 1, 0, cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_1),
    FLD_FULL_MAC(ORIGSRCPORT, 119, 6, 0, cond_plr_outgoing_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_0),
    FLD_FULL_MAC(ORIGSRCTRUNK, 118, 7, 0, cond_plr_outgoing_PacketCmd_NOT_FromCPU_AND_OrigIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDTRUNK, 118, 7, 0, cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDDEVISLOCAL, 118, 1, 0, cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(EXCLUDEDPORT, 119, 6, 0, cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(MAILBOXTONEIGHBORCPU, 124, 1, 0, cond_plr_outgoing_PacketCmd_FromCPU_AND_UseVIDX_0),
    FLD_MAC(L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET, 125, 4),
    FLD_MAC(OUTERISLLCNONSNAP, 129, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 130, 1, 0, NULL),
    FLD_MAC(TTL, 131, 8),
    FLD_MAC(TUNNELSTART, 139, 1),
    FLD_FULL_MAC(MLLPTR, 140, 14, 0, cond_plr_outgoing_MLLEn_1),
    FLD_FULL_MAC(TUNNELPTR, 140, 13, 0, cond_plr_outgoing_MLLEn_0_AND_TunnelStart_1),
    FLD_FULL_MAC(TUNNELTYPE, 153, 1, 0, cond_plr_outgoing_MLLEn_0_AND_TunnelStart_1),
    FLD_FULL_MAC(ARPPTR, 140, 14, 0, cond_plr_outgoing_MLLEn_0_AND_TunnelStart_0),
    FLD_MAC(IPM, 154, 1),
    FLD_FULL_MAC(PKTHASH, 155, 3, 3, NULL),
    FLD_MAC(IPXPROTOCOL, 158, 8),
    FLD_MAC(ISARPREPLY, 166, 1),
    FLD_MAC(L4VALID, 167, 1),
    FLD_MAC(TCP_UDPDESTPORT, 168, 16),
    FLD_MAC(INNERISLLCNONSNAP, 184, 1),
    FLD_MAC(SYNWITHDATA, 185, 1),
    FLD_MAC(ISIPV4, 186, 1),
    FLD_MAC(APPSPECIFICCPUCODE, 187, 1),
    FLD_MAC(ISSYN, 188, 1),
    FLD_MAC(PCLASSIGNEDSSTID, 196, 1),
    FLD_MAC(IPV4_DF, 197, 1),
    FLD_MAC(DOROUTEHA, 198, 1),
    FLD_MAC(DECTTL, 199, 1),
    FLD_MAC(MLLEN, 200, 1),
    FLD_MAC(MCRPFFAIL, 201, 1),
    FLD_MAC(NHEMCRPFFAILCMD, 202, 3),
    FLD_MAC(IPMODIFYUP, 205, 1),
    FLD_MAC(IPMODIFYDSCP, 206, 1),
    FLD_MAC(IPQOSPROFILE, 207, 7),
    FLD_MAC(CFI, 214, 1),
    FLD_MAC(PKTISLOOPED, 215, 1),
    FLD_MAC(DROPONSOURCE, 216, 1),
    FLD_MAC(ORIGVID, 217, 12),
    FLD_FULL_MAC(CFMPKT, 229, 1, 0, cond_plr_outgoing_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 229, 1, 1, cond_plr_outgoing_IsPTP_TRUE),
    FLD_MAC(POLICEREN, 235, 1),
    FLD_MAC(POLICERPTR, 236, 12),
    FLD_MAC(BILLINGEN, 248, 1)
};

/* Pre-egress to TXQ */
static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_TunnelTerminated_TT[] =
{
    COND_MAC(LAST, 40, 1, 1, EQUAL) /* (<TunnelTerminated> == TT) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_TunnelTerminated_NoTT_IsPTP_FALSE[] =
{
    COND_MAC(AND, 40, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT  */
    COND_MAC(LAST, 42, 1, 0, EQUAL) /* <IsPTP>==FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_TunnelTerminated_NoTT_IsPTP_TRUE[] =
{
    COND_MAC(AND, 40, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT  */
    COND_MAC(LAST, 42, 1, 1, EQUAL) /* <IsPTP>==TRUE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FROM_CPU[] =
{
    COND_MAC(LAST, 86, 2, 1, EQUAL) /* (<OutGoingMtagCmd> == FROM_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_NOT_FROM_CPU[] =
{
    COND_MAC(LAST, 86, 2, 1, DIFFER) /* (<OutGoingMtagCmd> != FROM_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True[] =
{
    COND_MAC(AND, 86, 2, 2, EQUAL), /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 80, 1, 1, EQUAL) /* (<QCN> == True) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False[] =
{
    COND_MAC(OR, 86, 2, 2, DIFFER), /* (<OutGoingMtagCmd> != TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 80, 1, 0, EQUAL) /* (<QCN> == False) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(LAST, 86, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER[] =
{
    COND_MAC(LAST, 86, 2, 2, DIFFER) /* (<OutGoingMtagCmd> != TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FORWARD[] =
{
    COND_MAC(LAST, 86, 2, 3, EQUAL) /* (<OutGoingMtagCmd> == FORWARD) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_OrigIsTrunk_0[] =
{
    COND_MAC(AND, 86, 2, 3, EQUAL), /* (<OutGoingMtagCmd> == FORWARD) */
    COND_MAC(LAST, 97, 1, 0, EQUAL) /* (<OrigIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_OrigIsTrunk_1[] =
{
    COND_MAC(AND, 86, 2, 3, EQUAL), /* (<OutGoingMtagCmd> == FORWARD) */
    COND_MAC(LAST, 97, 1, 1, EQUAL) /* (<OrigIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_UseVIDX_1[] =
{
    COND_MAC(AND, 86, 2, 3, EQUAL), /* (<OutGoingMtagCmd> == FORWARD) */
    COND_MAC(LAST, 112, 1, 1, EQUAL) /* (<UseVIDX> ==1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_UseVIDX_0[] =
{
    COND_MAC(AND, 86, 2, 3, EQUAL), /* (<OutGoingMtagCmd> == FORWARD) */
    COND_MAC(LAST, 112, 1, 0, EQUAL) /* (<UseVIDX> ==0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1[] =
{
    COND_MAC(AND, 86, 2, 1, EQUAL), /* (<OutGoingMtagCmd> == FROM_CPU) */
    COND_MAC(LAST, 112, 1, 1, EQUAL) /* (<UseVIDX> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_0[] =
{
    COND_MAC(AND, 86, 2, 1, EQUAL), /* (<OutGoingMtagCmd> == FROM_CPU) */
    COND_MAC(LAST, 112, 1, 0, EQUAL) /* (<UseVIDX> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_TO_CPU[] =
{
    COND_MAC(LAST, 86, 2, 0, EQUAL) /* (<OutGoingMtagCmd> == TO_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_SrcTrg_1_AND_OutGoingMtagCmd_TO_CPU[] =
{
    COND_MAC(AND, 112, 1, 1, EQUAL), /* (<SrcTrg> == 1) */
    COND_MAC(LAST, 86, 2, 0, EQUAL) /* (<OutGoingMtagCmd> == TO_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_SrcTrg_0_AND_OutGoingMtagCmd_TO_CPU[] =
{
    COND_MAC(AND, 112, 1, 0, EQUAL), /* (<SrcTrg> == 0) */
    COND_MAC(LAST, 86, 2, 0, EQUAL) /* (<OutGoingMtagCmd> == TO_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1[] =
{
    COND_MAC(AND, 86, 2, 1, EQUAL), /* (<OutGoingMtagCmd> == FROM_CPU) */
    COND_MAC(AND, 112, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 97, 1, 1, EQUAL) /* (<ExcludeIsTrunk> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0[] =
{
    COND_MAC(AND, 86, 2, 1, EQUAL), /* (<OutGoingMtagCmd> == FROM_CPU) */
    COND_MAC(AND, 112, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 97, 1, 0, EQUAL) /* (<ExcludeIsTrunk> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_UseVIDX_1_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(AND, 112, 1, 1, EQUAL), /* (<UseVIDX> == 1) */
    COND_MAC(LAST, 86, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_UseVIDX_0_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(AND, 112, 1, 0, EQUAL), /* (<UseVIDX> == 0) */
    COND_MAC(LAST, 86, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_RxSniff_Rx_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(AND, 111, 1, 1, EQUAL), /* (<RxSniff> == Rx) */
    COND_MAC(LAST, 86, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_RxSniff_Tx_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(AND, 111, 1, 0, EQUAL), /* (<RxSniff> == Tx) */
    COND_MAC(LAST, 86, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_TunnelStart_1[] =
{
    COND_MAC(LAST, 139, 1, 1, EQUAL) /* (<TunnelStart> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_TunnelStart_0[] =
{
    COND_MAC(LAST, 139, 1, 0, EQUAL) /* (<TunnelStart> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_IsPTP_FALSE[] =
{
    COND_MAC(LAST, 42, 1, 0, EQUAL) /* (<IsPTP> == FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_IsPTP_TRUE[] =
{
    COND_MAC(LAST, 42, 1, 1, EQUAL) /* (<IsPTP> == TRUE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_QCNPacket_1[] =
{
    COND_MAC(LAST, 201, 1, 1, EQUAL) /* (<QCNPacket> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_QCNPacket_NOT_1[] =
{
    COND_MAC(LAST, 201, 1, 1, DIFFER) /* (<QCNPacket> != 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_TunnelTerminated_NoTT[] =
{
    COND_MAC(LAST, 40, 1, 0, EQUAL) /* (<TunnelTerminated> == NoTT) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_RxSniff_Tx[] =
{
    COND_MAC(LAST, 111, 1, 0, EQUAL) /* (<RxSniff> == Tx) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_eq2txq_TunnelStart_NOT_1[] =
{
    COND_MAC(LAST, 139, 1, 1, DIFFER) /* (<TunnelStart> != 1) */
};

static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcateq2txq[] =
{
    FLD_MAC(FIRSTBUFFER, 0, 14),
    FLD_MAC(MDB, 14, 1),
    FLD_MAC(BANK, 15, 1),
    FLD_MAC(BYTECOUNT, 16, 14),
    FLD_MAC(RECALCCRC, 30, 1),
    FLD_MAC(MARVELLTAGGED, 31, 1),
    FLD_MAC(LOCALDEVSRCPORT, 32, 6),
    FLD_MAC(SRCTAGGED, 38, 1),
    FLD_MAC(MARVELLTAGGEDEXTENDED, 39, 1),
    FLD_MAC(TUNNELTERMINATED, 40, 1),
    FLD_FULL_MAC(INNERPKTTYPE, 41, 2, 0, cond_eq2txq_TunnelTerminated_TT),
    FLD_FULL_MAC(L2ECHO, 41, 1, 0, cond_eq2txq_TunnelTerminated_NoTT),
    FLD_FULL_MAC(INNERL3OFFSET, 43, 1, 0, NULL),
    FLD_MAC(MODIFYUP, 44, 1),
    FLD_MAC(UP, 45, 3),
    FLD_MAC(MODIFYDSCP, 48, 1),
    FLD_FULL_MAC(FROMCPUDP, 49, 2, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_MAC(FROMCPUTC, 51, 3),
    FLD_FULL_MAC(DSCP, 49, 6, 0, cond_eq2txq_OutGoingMtagCmd_NOT_FROM_CPU),
    FLD_FULL_MAC(INNERL3OFFSET, 55, 1, 1, NULL),
    FLD_FULL_MAC(TRGTAGGED, 56, 1, 0, NULL),
    FLD_MAC(VIDMODIFIED, 57, 1),
    FLD_FULL_MAC(CONGESTEDQUEUENUMBER, 58, 4, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_MAC(CONGESTEDPORTNUMBER, 62, 6),
    FLD_MAC(CONGESTEDQUEUEFILLLEVEL, 68, 2),
    FLD_MAC(LOCALDEVICESRCTRUNKID, 70, 7),
    FLD_MAC(PORTSPEEDINDEX_OR_ORIGQCNPRIO, 77, 3),
    FLD_MAC(VID, 58, 12),
    FLD_MAC(LOCALDEVICESRCTRUNKID, 70, 7),
    FLD_FULL_MAC(PKTHASH, 77, 3, 0, NULL),
    FLD_FULL_MAC(QCN, 80, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCDEVISOWN, 80, 1, 0, cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(TC, 81, 3, 0, NULL),
    FLD_FULL_MAC(DP, 84, 2, 0, NULL),
    FLD_MAC(OUTGOINGMTAGCMD, 86, 2),
    FLD_FULL_MAC(EGRESSFILTERREGISTERED, 88, 1, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD),
    FLD_MAC(MACDATYPE, 89, 2),
    FLD_MAC(SST_ID, 91, 5),
    FLD_MAC(ROUTED, 96, 1),
    FLD_MAC(ORIGISTRUNK, 97, 1),
    FLD_MAC(ORIGSRCPORT, 99, 6),
    FLD_FULL_MAC(ORIGSRCTRUNK, 98, 7, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_OrigIsTrunk_1),
    FLD_MAC(QOSPROFILE, 105, 7),
    FLD_MAC(USEVIDX, 112, 1),
    FLD_FULL_MAC(VIDX, 113, 12, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_UseVIDX_1),
    FLD_MAC(TRGPORT, 114, 6),
    FLD_MAC(TRGDEV, 120, 5),
    FLD_FULL_MAC(EGRESSFILTERREGISTERED, 88, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_MAC(MACDATYPE, 89, 2),
    FLD_MAC(SST_ID, 91, 5),
    FLD_MAC(EGRESSFILTEREN, 96, 1),
    FLD_FULL_MAC(EXCLUDEISTRUNK, 97, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1),
    FLD_FULL_MAC(EXCLUDEDTRUNK, 98, 7, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDDEVISLOCAL, 98, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_MAC(EXCLUDEDPORT, 99, 6),
    FLD_MAC(CONTROLTC, 110, 1),
    FLD_MAC(MAILBOXTONEIGHBORCPU, 111, 1),
    FLD_MAC(USEVIDX, 112, 1),
    FLD_FULL_MAC(VIDX, 113, 12, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1),
    FLD_MAC(TRGPORT, 114, 6),
    FLD_MAC(TRGDEV, 120, 5),
    FLD_MAC(TRUNCATED, 95, 1),
    FLD_MAC(ROUTED, 96, 1),
    FLD_MAC(CPUTRGDEV, 98, 5),
    FLD_MAC(CPUCODE, 103, 8),
    FLD_MAC(MAILBOXTONEIGHBORCPU, 111, 1),
    FLD_MAC(SRCTRG, 112, 1),
    FLD_MAC(TRGTAGGED_DUP, 113, 1),
    FLD_MAC(TRGPORT, 114, 6),
    FLD_MAC(TRGDEV, 120, 5),
    FLD_MAC(SRCTAGGED_DUP, 113, 1),
    FLD_MAC(SRCPORT, 114, 6),
    FLD_MAC(SRCDEV, 120, 5),
    FLD_MAC(ROUTED, 96, 1),
    FLD_FULL_MAC(SNIFFVIDX, 98, 12, 0, cond_eq2txq_UseVIDX_1_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SNIFFTRGDEV, 98, 5, 0, cond_eq2txq_UseVIDX_0_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_MAC(SNIFFTRGPORT, 103, 6),
    FLD_MAC(RXSNIFF, 111, 1),
    FLD_MAC(USEVIDX, 112, 1),
    FLD_MAC(SRCTAGGED_DUP, 113, 1),
    FLD_MAC(SRCPORT, 114, 6),
    FLD_MAC(SRCDEV, 120, 5),
    FLD_FULL_MAC(TRGTAGGED_DUP, 113, 1, 0, cond_eq2txq_RxSniff_Tx),
    FLD_MAC(TRGPORT, 114, 6),
    FLD_MAC(TRGDEV, 120, 5),
    FLD_MAC(L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET, 125, 4),
    FLD_MAC(OUTERISLLCNONSNAP, 129, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 130, 1, 2, NULL),
    FLD_MAC(TTL, 131, 8),
    FLD_MAC(TUNNELSTART, 139, 1),
    FLD_FULL_MAC(TUNNELPTR, 140, 13, 0, cond_eq2txq_TunnelStart_1),
    FLD_MAC(TUNNELTYPE, 153, 1),
    FLD_FULL_MAC(ARPPTR, 140, 14, 0, cond_eq2txq_TunnelStart_NOT_1),
    FLD_MAC(IPM, 154, 1),
    FLD_MAC(INNERISLLCNONSNAP, 155, 1),
    FLD_MAC(DOROUTEHA, 156, 1),
    FLD_MAC(DECTTL, 157, 1),
    FLD_MAC(MLL, 158, 1),
    FLD_MAC(MLLLAST, 159, 1),
    FLD_MAC(DROPPKT, 160, 1),
    FLD_MAC(CFI, 161, 1),
    FLD_MAC(PKTISLOOPED, 162, 1),
    FLD_MAC(DROPONSOURCE, 163, 1),
    FLD_MAC(ORIGVID, 164, 12),
    FLD_MAC(CFMPKT, 176, 1),
    FLD_MAC(ORIGSRCDEV, 177, 5),
    FLD_MAC(NUM_OF_INGRESSTAGS, 182, 2),
    FLD_MAC(VID1, 184, 12),
    FLD_MAC(CFI1, 196, 1),
    FLD_MAC(UP1, 197, 3),
    FLD_FULL_MAC(TRGTAGGED, 201, 2, 1, NULL)
};



static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat2eq2txq[] =
{
    FLD_MAC(FIRSTBUFFER, 0, 14),
    FLD_MAC(MDB, 14, 1),
    FLD_MAC(BANK, 15, 1),
    FLD_MAC(BYTECOUNT, 16, 14),
    FLD_MAC(RECALCCRC, 30, 1),
    FLD_MAC(MARVELLTAGGED, 31, 1),
    FLD_MAC(LOCALDEVSRCPORT, 32, 6),
    FLD_MAC(SRCTAGGED, 38, 1),
    FLD_MAC(MARVELLTAGGEDEXTENDED, 39, 1),
    FLD_MAC(TUNNELTERMINATED, 40, 1),
    FLD_FULL_MAC(INNERPKTTYPE, 41, 2, 0, cond_eq2txq_TunnelTerminated_TT),
    FLD_FULL_MAC(L2ECHO, 41, 1, 0, cond_eq2txq_TunnelTerminated_NoTT_IsPTP_FALSE),
    FLD_FULL_MAC(ISPTP, 42, 1, 0, cond_eq2txq_TunnelTerminated_NoTT_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 41, 1, 0, cond_eq2txq_TunnelTerminated_NoTT_IsPTP_TRUE),
    FLD_FULL_MAC(ISPTP, 42, 1, 0, cond_eq2txq_TunnelTerminated_NoTT_IsPTP_TRUE),
    FLD_FULL_MAC(INNERL3OFFSET, 43, 1, 0, NULL),
    FLD_MAC(MODIFYUP, 44, 1),
    FLD_MAC(UP, 45, 3),
    FLD_MAC(MODIFYDSCP_EXP, 48, 1),
    FLD_FULL_MAC(FROMCPUDP, 49, 2, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(FROMCPUTC, 51, 3, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(DSCP, 49, 6, 0, cond_eq2txq_OutGoingMtagCmd_NOT_FROM_CPU),
    FLD_FULL_MAC(INNERL3OFFSET, 55, 1, 1, NULL),
    FLD_FULL_MAC(TRGTAGGED, 56, 1, 0, NULL),
    FLD_MAC(VIDMODIFIED, 57, 1),
    FLD_FULL_MAC(CONGESTEDQUEUENUMBER, 58, 3, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(CONGESTEDPORTNUMBER, 61, 6, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(QCNQOFFSET, 67, 3, 11, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(LOCALDEVICESRCTRUNKID, 70, 7, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(PORTSPEEDINDEX_OR_ORIGQCNPRIO, 77, 3, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(VID, 58, 12, 0, cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(LOCALDEVICESRCTRUNKID, 70, 7, 0, cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(PKTHASH, 77, 3, 0, cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(QCN, 80, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCDEVISOWN, 80, 1, 0, cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER),
    FLD_MAC(TC, 81, 3),
    FLD_MAC(DP, 84, 2),
    FLD_MAC(OUTGOINGMTAGCMD, 86, 2),
    FLD_FULL_MAC(EGRESSFILTERREGISTERED, 88, 1, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD),
    FLD_FULL_MAC(MACDATYPE, 89, 2, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD),
    FLD_FULL_MAC(SST_ID, 91, 5, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD),
    FLD_FULL_MAC(ROUTED, 96, 1, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD),
    FLD_FULL_MAC(ORIGISTRUNK, 97, 1, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD),
    FLD_FULL_MAC(ORIGSRCPORT, 99, 6, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_OrigIsTrunk_0),
    FLD_FULL_MAC(ORIGSRCTRUNK, 98, 7, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_OrigIsTrunk_1),
    FLD_FULL_MAC(QOSPROFILE, 105, 7, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD),
    FLD_FULL_MAC(USEVIDX, 112, 1, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD),
    FLD_FULL_MAC(VIDX, 113, 12, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_UseVIDX_1),
    FLD_FULL_MAC(TRGPORT, 114, 6, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_UseVIDX_0),
    FLD_FULL_MAC(TRGDEV, 120, 5, 0, cond_eq2txq_OutGoingMtagCmd_FORWARD_AND_UseVIDX_0),
    FLD_FULL_MAC(EGRESSFILTERREGISTERED, 88, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(MACDATYPE, 89, 2, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(SST_ID, 91, 5, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(EGRESSFILTEREN, 96, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(EXCLUDEISTRUNK, 97, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1),
    FLD_FULL_MAC(EXCLUDEDTRUNK, 98, 7, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_1),
    FLD_FULL_MAC(EXCLUDEDDEVISLOCAL, 98, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(EXCLUDEDPORT, 99, 6, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1_AND_ExcludeIsTrunk_0),
    FLD_FULL_MAC(CONTROLTC, 110, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(MAILBOXTONEIGHBORCPU, 111, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(USEVIDX, 112, 1, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(VIDX, 113, 12, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_1),
    FLD_FULL_MAC(TRGPORT, 114, 6, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_0),
    FLD_FULL_MAC(TRGDEV, 120, 5, 0, cond_eq2txq_OutGoingMtagCmd_FROM_CPU_AND_UseVIDX_0),
    FLD_FULL_MAC(TRUNCATED, 95, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(ROUTED, 96, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(CPUTRGDEV, 98, 5, 0, cond_eq2txq_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(CPUCODE, 103, 8, 0, cond_eq2txq_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(MAILBOXTONEIGHBORCPU, 111, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCTRG, 112, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRGTAGGED_DUP, 113, 1, 0, cond_eq2txq_SrcTrg_1_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRGPORT, 114, 6, 0, cond_eq2txq_SrcTrg_1_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRGDEV, 120, 5, 0, cond_eq2txq_SrcTrg_1_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCTAGGED_DUP, 113, 1, 0, cond_eq2txq_SrcTrg_0_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCPORT, 114, 6, 0, cond_eq2txq_SrcTrg_0_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCDEV, 120, 5, 0, cond_eq2txq_SrcTrg_0_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRUNCATED, 95, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(ROUTED, 96, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SNIFFVIDX, 98, 12, 0, cond_eq2txq_UseVIDX_1_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SNIFFTRGDEV, 98, 5, 0, cond_eq2txq_UseVIDX_0_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SNIFFTRGPORT, 103, 6, 0, cond_eq2txq_UseVIDX_0_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(RXSNIFF, 111, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(USEVIDX, 112, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCTAGGED_DUP, 113, 1, 0, cond_eq2txq_RxSniff_Rx_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCPORT, 114, 6, 0, cond_eq2txq_RxSniff_Rx_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCDEV, 120, 5, 0, cond_eq2txq_RxSniff_Rx_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(TRGTAGGED_DUP, 113, 1, 0, cond_eq2txq_RxSniff_Tx_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(TRGPORT, 114, 6, 0, cond_eq2txq_RxSniff_Tx_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(TRGDEV, 120, 5, 0, cond_eq2txq_RxSniff_Tx_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_MAC(L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET, 125, 4),
    FLD_MAC(OUTERISLLCNONSNAP, 129, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 130, 1, 2, NULL),
    FLD_MAC(TTL, 131, 8),
    FLD_MAC(TUNNELSTART, 139, 1),
    FLD_FULL_MAC(QCNQDELTA, 140, 14, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(TUNNELPTR, 140, 13, 0, cond_eq2txq_TunnelStart_1),
    FLD_FULL_MAC(TUNNELTYPE, 153, 1, 0, cond_eq2txq_TunnelStart_1),
    FLD_FULL_MAC(ARPPTR, 140, 14, 0, cond_eq2txq_TunnelStart_0),
    FLD_MAC(IPM, 154, 1),
    FLD_MAC(INNERISLLCNONSNAP, 155, 1),
    FLD_MAC(DOROUTEHA, 156, 1),
    FLD_MAC(DECTTL, 157, 1),
    FLD_MAC(MLL, 158, 1),
    FLD_MAC(MLLLAST, 159, 1),
    FLD_MAC(DROPPKT, 160, 1),
    FLD_MAC(CFI, 161, 1),
    FLD_MAC(PKTISLOOPED, 162, 1),
    FLD_MAC(DROPONSOURCE, 163, 1),
    FLD_MAC(ORIGVID, 164, 12),
    FLD_FULL_MAC(CFMPKT, 176, 1, 0, cond_eq2txq_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 176, 1, 1, cond_eq2txq_IsPTP_TRUE),
    FLD_MAC(ORIGSRCDEV, 177, 5),
    FLD_MAC(NUM_OF_INGRESSTAGS, 182, 2),
    FLD_FULL_MAC(VID1, 184, 12, 0, cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(CFI1, 196, 1, 0, cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(UP1, 197, 3, 0, cond_eq2txq_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(QCNQOFFSET, 184, 11, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(QCNQSATURATED, 195, 1, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(QCNQFB, 195, 6, 0, cond_eq2txq_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_MAC(QCNPACKET, 201, 1),
    FLD_FULL_MAC(ORIGQCNPRIO, 202, 3, 0, cond_eq2txq_QCNPacket_1),
    FLD_FULL_MAC(PACKETISCNTAGGED, 202, 1, 0, cond_eq2txq_QCNPacket_NOT_1),
    FLD_FULL_MAC(CPIDMATCH, 203, 1, 0, cond_eq2txq_QCNPacket_NOT_1),
    FLD_FULL_MAC(PACKETISLLCSNAP, 204, 1, 0, cond_eq2txq_QCNPacket_NOT_1),
    FLD_FULL_MAC(TRGTAGGED, 205, 2, 1, NULL),
    FLD_FULL_MAC(PKTHASH, 207, 3, 3, NULL)
};

/* Outgoing TXQ Descriptor */

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_TunnelTerminated_TT[] =
{
    COND_MAC(LAST, 40, 1, 1, EQUAL) /* (<TunnelTerminated> == TT) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_TunnelTerminated_NoTT[] =
{
    COND_MAC(LAST, 40, 1, 0, EQUAL) /* (<TunnelTerminated> == NoTT) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_FROM_CPU[] =
{
    COND_MAC(LAST, 70, 2, 1, EQUAL) /* (<OutGoingMtagCmd> == FROM_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_NOT_FROM_CPU[] =
{
    COND_MAC(LAST, 70, 2, 1, DIFFER) /* (<OutGoingMtagCmd> != FROM_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_CCFC_True[] =
{
    COND_MAC(AND, 70, 2, 2, EQUAL), /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 72, 1, 1, EQUAL) /* (<CCFC> == True) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_Not_TO_TARGET_SNIFFER_OR_CCFC_False[] =
{
    COND_MAC(OR, 70, 2, 2, DIFFER), /* (<OutGoingMtagCmd> != TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 72, 1, 0, EQUAL) /* (<CCFC> == False) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU[] =
{
    COND_MAC(OR, 70, 2, 3, EQUAL),   /* (<OutGoingMtagCmd> == FORWARD) */
    COND_MAC(LAST, 70, 2, 1, EQUAL), /* (<OutGoingMtagCmd> == FROM_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_useVidx0[] =
{
    COND_MAC(AND, 70, 2, 3, EQUAL),    /* (<OutGoingMtagCmd> == FORWARD) */
    COND_MAC(OR_SEQ, 70, 2, 1, EQUAL), /* (<OutGoingMtagCmd> == FROM_CPU) */
    COND_MAC(LAST, 86, 1, 0, EQUAL)    /* (<useVidx> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_useVidx1[] =
{
    COND_MAC(AND, 70, 2, 3, EQUAL),    /* (<OutGoingMtagCmd> == FORWARD) */
    COND_MAC(OR_SEQ, 70, 2, 1, EQUAL), /* (<OutGoingMtagCmd> == FROM_CPU) */
    COND_MAC(LAST, 86, 1, 1, EQUAL)    /* (<useVidx> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_TO_CPU[] =
{
    COND_MAC(LAST, 70, 2, 0, EQUAL) /* (<OutGoingMtagCmd> == TO_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_TO_CPU_AND_srcTrg0[] =
{
    COND_MAC(AND, 70, 2, 0, EQUAL), /* (<OutGoingMtagCmd> == TO_CPU) */
    COND_MAC(LAST, 86, 1, 0, EQUAL) /* (<srcTrg> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_TO_CPU_AND_srcTrg1[] =
{
    COND_MAC(AND, 70, 2, 0, EQUAL), /* (<OutGoingMtagCmd> == TO_CPU) */
    COND_MAC(LAST, 86, 1, 1, EQUAL) /* (<srcTrg> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(LAST, 70, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_srcTrg0[] =
{
    COND_MAC(AND, 70, 2, 2, EQUAL), /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 86, 1, 0, EQUAL) /* (<srcTrg> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_srcTrg1[] =
{
    COND_MAC(AND, 70, 2, 2, EQUAL), /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 86, 1, 1, EQUAL) /* (<srcTrg> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_TunnelStart_1[] =
{
    COND_MAC(LAST, 113, 1, 1, EQUAL) /* (<TunnelStart> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_txq2rddma_TunnelStart_NOT_1[] =
{
    COND_MAC(LAST, 113, 1, 1, DIFFER) /* (<TunnelStart> != 1) */
};

static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat_txq2rddma[] =
{
    FLD_MAC(FIRSTBUFFER, 0, 14),
    FLD_MAC(MDB, 14, 1),
    FLD_MAC(BANK, 15, 1),
    FLD_MAC(BYTECOUNT, 16, 14),
    FLD_MAC(RECALCCRC, 30, 1),
    FLD_MAC(MARVELLTAGGED, 31, 1),
    FLD_MAC(LOCALDEVSRCPORT, 32, 6),
    FLD_MAC(SRCTAGGED, 38, 1),
    FLD_MAC(MARVELLTAGGEDEXTENDED, 39, 1),
    FLD_MAC(TUNNELTERMINATED, 40, 1),
    FLD_FULL_MAC(INNERPKTTYPE, 41, 2, 0, cond_txq2rddma_TunnelTerminated_TT),
    FLD_FULL_MAC(L2ECHO, 41, 1, 0, cond_txq2rddma_TunnelTerminated_NoTT),
    FLD_FULL_MAC(INNERL3OFFSET, 43, 1, 0, NULL),
    FLD_MAC(MODIFYUP, 44, 1),
    FLD_MAC(UP, 45, 3),
    FLD_MAC(MODIFYDSCP, 48, 1),
    FLD_FULL_MAC(FROMCPUDP, 49, 2, 0, cond_txq2rddma_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(FROMCPUTC, 51, 3, 0, cond_txq2rddma_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(DSCP, 49, 6, 0, cond_txq2rddma_OutGoingMtagCmd_NOT_FROM_CPU),
    FLD_FULL_MAC(INNERL3OFFSET, 55, 1, 1, NULL),
    FLD_FULL_MAC(TRGTAGGED, 56, 1, 0, NULL),
    FLD_MAC(VIDMODIFIED, 57, 1),
    FLD_FULL_MAC(CONGESTEDQUEUENUMBER, 58, 4, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_CCFC_True),
    FLD_FULL_MAC(CONGESTEDPORTNUMBER, 62, 6, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_CCFC_True),
    FLD_FULL_MAC(CONGESTEDQUEUEFILLLEVEL, 68, 2, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_CCFC_True),
    FLD_FULL_MAC(VID, 58, 12, 0, cond_txq2rddma_OutGoingMtagCmd_Not_TO_TARGET_SNIFFER_OR_CCFC_False),
    FLD_MAC(OUTGOINGMTAGCMD, 70, 2),
    FLD_FULL_MAC(EGRESSFILTERREGISTERED, 72, 1, 0, cond_txq2rddma_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU),
    FLD_MAC(ROUTED, 73, 1),
    FLD_MAC(SST_ID, 74, 5),
    FLD_MAC(QOSPROFILE, 79, 7),
    FLD_MAC(USEVIDX, 86, 1),
    FLD_FULL_MAC(VIDX, 87, 12, 0, cond_txq2rddma_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_useVidx1),
    FLD_FULL_MAC(TRGPORT, 88, 6, 0, cond_txq2rddma_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_useVidx0),
    FLD_FULL_MAC(TRGDEV, 94, 5, 0, cond_txq2rddma_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_useVidx0),
    FLD_FULL_MAC(TC, 72, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(ROUTED, 73, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TC, 74, 2, 1, cond_txq2rddma_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(CPUMAILTONEIGBORCPU, 76, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(CPUCODE, 77, 8, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRUNCATED, 85, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCTRG, 86, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRGTAGGED_DUP, 87, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU_AND_srcTrg1),
    FLD_FULL_MAC(TRGPORT, 88, 6, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU_AND_srcTrg1),
    FLD_FULL_MAC(TRGDEV, 94, 5, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU_AND_srcTrg1),
    FLD_FULL_MAC(SRCTAGGED_DUP, 87, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU_AND_srcTrg0),
    FLD_FULL_MAC(SRCPORT, 88, 6, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU_AND_srcTrg0),
    FLD_FULL_MAC(SRCDEV, 94, 5, 0, cond_txq2rddma_OutGoingMtagCmd_TO_CPU_AND_srcTrg0),
    FLD_FULL_MAC(CCFC, 72, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(ROUTED, 73, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(PORTSPEEDINDEX_OR_ORIGQCNPRIO, 74, 3, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(RXSNIFF, 86, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCTAGGED_DUP, 87, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_srcTrg0),
    FLD_FULL_MAC(SRCPORT, 88, 6, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_srcTrg0),
    FLD_FULL_MAC(SRCDEV, 94, 5, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_srcTrg0),
    FLD_FULL_MAC(TRGTAGGED_DUP, 87, 1, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_srcTrg1),
    FLD_FULL_MAC(TRGPORT, 88, 6, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_srcTrg1),
    FLD_FULL_MAC(TRGDEV, 94, 5, 0, cond_txq2rddma_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_srcTrg1),
    FLD_MAC(L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET, 99, 4),
    FLD_MAC(OUTERISLLCNONSNAP, 103, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 104, 1, 2, NULL),
    FLD_MAC(TTL, 105, 8),
    FLD_MAC(TUNNELSTART, 113, 1),
    FLD_FULL_MAC(TUNNELPTR, 114, 13, 0, cond_txq2rddma_TunnelStart_1),
    FLD_FULL_MAC(TUNNELTYPE, 127, 1, 0, cond_txq2rddma_TunnelStart_1),
    FLD_FULL_MAC(ARPPTR, 114, 14, 0, cond_txq2rddma_TunnelStart_NOT_1),
    FLD_MAC(IPM, 128, 1),
    FLD_MAC(INNERISLLCNONSNAP, 129, 1),
    FLD_MAC(DOROUTEHA, 130, 1),
    FLD_MAC(DECTTL, 131, 1),
    FLD_MAC(CFI, 132, 1),
    FLD_MAC(PKTISLOOPED, 133, 1),
    FLD_MAC(DROPONSOURCE, 134, 1),
    FLD_MAC(ORIGVID, 135, 12),
    FLD_MAC(CFMPKT, 147, 1),
    FLD_MAC(NUM_OF_INGRESSTAGS, 148, 2),
    FLD_MAC(VID1, 150, 12),
    FLD_MAC(CFI1, 162, 1),
    FLD_MAC(UP1, 163, 3),
    FLD_FULL_MAC(TRGTAGGED, 167, 2, 1, NULL)
};

/* Incoming Header Alteration descriptor */
static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_TunnelTerminated_TT[] =
{
    COND_MAC(LAST, 24, 1, 1, EQUAL) /* (<TunnelTerminated> == TT) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_TunnelTerminated_NoTT_IsPTP_FALSE[] =
{
    COND_MAC(AND, 24, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT  */
    COND_MAC(LAST, 26, 1, 0, EQUAL) /* <IsPTP>==FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_TunnelTerminated_NoTT_IsPTP_TRUE[] =
{
    COND_MAC(AND, 24, 1, 0, EQUAL), /* (<TunnelTerminated> == NoTT  */
    COND_MAC(LAST, 26, 1, 1, EQUAL) /* <IsPTP>==TRUE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_FROM_CPU[] =
{
    COND_MAC(LAST, 54, 2, 1, EQUAL) /* (<OutGoingMtagCmd> == FROM_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_NOT_FROM_CPU[] =
{
    COND_MAC(LAST, 54, 2, 1, DIFFER) /* (<OutGoingMtagCmd> != FROM_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True[] =
{
    COND_MAC(AND, 54, 2, 2, EQUAL), /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 56, 1, 1, EQUAL) /* (<QCN> == True) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False[] =
{
    COND_MAC(OR, 54, 2, 2, DIFFER), /* (<OutGoingMtagCmd> != TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 56, 1, 0, EQUAL) /* (<QCN> == False) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU[] =
{
    COND_MAC(OR, 54, 2, 3, EQUAL), /* (<OutGoingMtagCmd> == FORWARD) */
    COND_MAC(LAST, 54, 2, 1, EQUAL) /* (<OutGoingMtagCmd> == FROM_CPU) */
};

/* convert condition to be
(<OutGoingMtagCmd> != TO_CPU) & (<OutGoingMtagCmd> != TO_TARGET_SNIFFER) & (<UseVidx> == 1)*/
static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_UseVidx_1[] =
{
    COND_MAC(AND, 54, 2, 0, DIFFER), /* (<OutGoingMtagCmd> != TO_CPU) */
    COND_MAC(AND, 54, 2, 2, DIFFER), /* (<OutGoingMtagCmd> != TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 70, 1, 1, EQUAL) /* (<UseVidx> == 1) */
};

/* convert condition to be
(<OutGoingMtagCmd> != TO_CPU) & (<OutGoingMtagCmd> != TO_TARGET_SNIFFER) & (<UseVidx> == 0)*/
static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_UseVidx_0[] =
{
    COND_MAC(AND, 54, 2, 0, DIFFER), /* (<OutGoingMtagCmd> != TO_CPU) */
    COND_MAC(AND, 54, 2, 2, DIFFER), /* (<OutGoingMtagCmd> != TO_TARGET_SNIFFER) */
    COND_MAC(LAST, 70, 1, 0, EQUAL) /* (<UseVidx> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_TO_CPU[] =
{
    COND_MAC(LAST, 54, 2, 0, EQUAL) /* (<OutGoingMtagCmd> == TO_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_SrcTrg_1_AND_OutGoingMtagCmd_TO_CPU[] =
{
    COND_MAC(AND, 70, 1, 1, EQUAL), /* (<SrcTrg> ==1) */
    COND_MAC(LAST, 54, 2, 0, EQUAL) /* (<OutGoingMtagCmd> == TO_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_SrcTrg_0_AND_OutGoingMtagCmd_TO_CPU[] =
{
    COND_MAC(AND, 70, 1, 0, EQUAL), /* (<SrcTrg> ==0) */
    COND_MAC(LAST, 54, 2, 0, EQUAL) /* (<OutGoingMtagCmd> == TO_CPU) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(LAST, 54, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_RxSniff_1_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(AND, 70, 1, 1, EQUAL), /* (<RxSniff> ==1) */
    COND_MAC(LAST, 54, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_RxSniff_0_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER[] =
{
    COND_MAC(AND, 70, 1, 0, EQUAL), /* (<RxSniff> ==0) */
    COND_MAC(LAST, 54, 2, 2, EQUAL) /* (<OutGoingMtagCmd> == TO_TARGET_SNIFFER) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_TunnelStart_1[] =
{
    COND_MAC(LAST, 97, 1, 1, EQUAL) /* (<TunnelStart> == 1) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_TunnelStart_0[] =
{
    COND_MAC(LAST, 97, 1, 0, EQUAL) /* (<TunnelStart> == 0) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_IsPTP_FALSE[] =
{
    COND_MAC(LAST, 26, 1, 0, EQUAL) /* (<IsPTP> == FALSE) */
};

static PRV_CPSS_DXCH_DIAG_CONDITION_STC cond_rddma2ha_IsPTP_TRUE[] =
{
    COND_MAC(LAST, 26, 1, 1, EQUAL) /* (<IsPTP> == TRUE) */
};



static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat2rddma2ha[] =
{
    FLD_MAC(BYTECOUNT, 0, 14),
    FLD_MAC(RECALCCRC, 14, 1),
    FLD_MAC(MARVELLTAGGED, 15, 1),
    FLD_MAC(LOCALDEVSRCPORT, 16, 6),
    FLD_MAC(SRCTAGGED, 22, 1),
    FLD_MAC(MARVELLTAGGEDEXTENDED, 23, 1),
    FLD_MAC(TUNNELTERMINATED, 24, 1),
    FLD_FULL_MAC(INNERPKTTYPE, 25, 2, 0, cond_rddma2ha_TunnelTerminated_TT),
    FLD_FULL_MAC(L2ECHO, 25, 1, 0, cond_rddma2ha_TunnelTerminated_NoTT_IsPTP_FALSE),
    FLD_FULL_MAC(ISPTP, 26, 1, 0, cond_rddma2ha_TunnelTerminated_NoTT_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 25, 1, 0, cond_rddma2ha_TunnelTerminated_NoTT_IsPTP_TRUE),
    FLD_FULL_MAC(ISPTP, 26, 1, 0, cond_rddma2ha_TunnelTerminated_NoTT_IsPTP_TRUE),
    FLD_FULL_MAC(INNERL3OFFSET, 27, 1, 0, NULL),
    FLD_MAC(MODIFYUP, 28, 1),
    FLD_MAC(UP, 29, 3),
    FLD_MAC(MODIFYDSCP_EXP, 32, 1),
    FLD_FULL_MAC(FROMCPUDP, 33, 2, 0, cond_rddma2ha_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(FROMCPUTC, 35, 3, 0, cond_rddma2ha_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(DSCP, 33, 6, 0, cond_rddma2ha_OutGoingMtagCmd_NOT_FROM_CPU),
    FLD_FULL_MAC(INNERL3OFFSET, 39, 1, 1, NULL),
    FLD_FULL_MAC(TRGTAGGED, 40, 1, 0, NULL),
    FLD_MAC(VIDMODIFIED, 41, 1),
    FLD_FULL_MAC(CONGESTEDQUEUENUMBER, 42, 3, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(CONGESTEDPORTNUMBER, 45, 6, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(QCNQOFFSET, 51, 3, 11, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(VID, 42, 12, 0, cond_rddma2ha_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_MAC(OUTGOINGMTAGCMD, 54, 2),
    FLD_FULL_MAC(EGRESSFILTERREGISTERED, 56, 1, 0, cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(ROUTED, 57, 1, 0, cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(SST_ID, 58, 5, 0, cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(QOSPROFILE, 63, 7, 0, cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(USEVIDX, 70, 1, 0, cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU),
    FLD_FULL_MAC(VIDX, 71, 12, 0, cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_UseVidx_1),
    FLD_FULL_MAC(TRGPORT, 72, 6, 0, cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_UseVidx_0),
    FLD_FULL_MAC(TRGDEV, 78, 5, 0, cond_rddma2ha_OutGoingMtagCmd_FORWARD_OR_OutGoingMtagCmd_FROM_CPU_AND_UseVidx_0),
    FLD_FULL_MAC(TC, 56, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(ROUTED, 57, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TC, 58, 2, 1, cond_rddma2ha_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(CPUMAILTONEIGBORCPU, 60, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(CPUCODE, 61, 8, 0, cond_rddma2ha_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRUNCATED, 69, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCTRG, 70, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRGTAGGED_DUP, 71, 1, 0, cond_rddma2ha_SrcTrg_1_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRGPORT, 72, 6, 0, cond_rddma2ha_SrcTrg_1_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(TRGDEV, 78, 5, 0, cond_rddma2ha_SrcTrg_1_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCTAGGED_DUP, 71, 1, 0, cond_rddma2ha_SrcTrg_0_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCPORT, 72, 6, 0, cond_rddma2ha_SrcTrg_0_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(SRCDEV, 78, 5, 0, cond_rddma2ha_SrcTrg_0_AND_OutGoingMtagCmd_TO_CPU),
    FLD_FULL_MAC(QCN, 56, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(ROUTED, 57, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(PORTSPEEDINDEX_OR_ORIGQCNPRIO, 58, 3, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(TRUNCATED, 69, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(RXSNIFF, 70, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCTAGGED_DUP, 71, 1, 0, cond_rddma2ha_RxSniff_1_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCPORT, 72, 6, 0, cond_rddma2ha_RxSniff_1_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(SRCDEV, 78, 5, 0, cond_rddma2ha_RxSniff_1_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(TRGTAGGED_DUP, 71, 1, 0, cond_rddma2ha_RxSniff_0_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(TRGPORT, 72, 6, 0, cond_rddma2ha_RxSniff_0_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_FULL_MAC(TRGDEV, 78, 5, 0, cond_rddma2ha_RxSniff_0_AND_OutGoingMtagCmd_TO_TARGET_SNIFFER),
    FLD_MAC(L3TOTALOFFSET_OR_INNERHEADERTOTALOFFSET, 83, 4),
    FLD_MAC(OUTERISLLCNONSNAP, 87, 1),
    FLD_FULL_MAC(INNERL3OFFSET, 88, 1, 2, NULL),
    FLD_MAC(TTL, 89, 8),
    FLD_MAC(TUNNELSTART, 97, 1),
    FLD_FULL_MAC(QCNQDELTA, 98, 14, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(TUNNELPTR, 98, 13, 0, cond_rddma2ha_TunnelStart_1),
    FLD_FULL_MAC(TUNNELTYPE, 111, 1, 0, cond_rddma2ha_TunnelStart_1),
    FLD_FULL_MAC(ARPPTR, 98, 14, 0, cond_rddma2ha_TunnelStart_0),
    FLD_MAC(IPM, 112, 1),
    FLD_MAC(INNERISLLCNONSNAP, 113, 1),
    FLD_MAC(DOROUTEHA, 114, 1),
    FLD_MAC(DECTTL, 115, 1),
    FLD_MAC(CFI, 116, 1),
    FLD_MAC(PKTISLOOPED, 117, 1),
    FLD_MAC(DROPONSOURCE, 118, 1),
    FLD_MAC(ORIGVID, 119, 12),
    FLD_FULL_MAC(CFMPKT, 131, 1, 0, cond_rddma2ha_IsPTP_FALSE),
    FLD_FULL_MAC(PTPTRIGGERTYPE, 131, 1, 1, cond_rddma2ha_IsPTP_TRUE),
    FLD_MAC(NUM_OF_INGRESSTAGS, 132, 2),
    FLD_FULL_MAC(VID1, 134, 12, 0, cond_rddma2ha_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(PACKETISCNTAGGED, 146, 1, 0, cond_rddma2ha_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(PACKETISLLCSNAP, 147, 1, 0, cond_rddma2ha_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(CFI1, 146, 1, 0, cond_rddma2ha_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(UP1, 147, 3, 0, cond_rddma2ha_OutGoingMtagCmd_NOT_TO_TARGET_SNIFFER_OR_QCN_False),
    FLD_FULL_MAC(QCNQOFFSET, 134, 11, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(QCNQSATURATED, 145, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(PACKETISCNTAGGED, 146, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(PACKETISLLCSNAP, 147, 1, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(QCNQFB, 145, 6, 0, cond_rddma2ha_OutGoingMtagCmd_TO_TARGET_SNIFFER_AND_QCN_True),
    FLD_FULL_MAC(TRGTAGGED, 151, 2, 1, NULL),
    FLD_MAC(SRCOWNDEV0, 153, 1) /* Supported for lion2 only */
};

/* Egress Policer to Egress PCL */
static PRV_CPSS_DXCH_DIAG_HW_DESCR_STC xcat2eplr2epcl[] =
{
    FLD_MAC(PKTCMD, 0, 1),
    FLD_MAC(DROPCODE, 1, 1),
    FLD_MAC(MODIFYUP, 2, 1),
    FLD_MAC(UP, 3, 3),
    FLD_MAC(MODIFYDSCP, 6, 1),
    FLD_MAC(DSCP, 7, 6),
    FLD_MAC(MODIFYEXP, 13, 1),
    FLD_MAC(EXP, 14, 3),
    FLD_MAC(MODIFYTC, 17, 1),
    FLD_MAC(TC, 18, 3),
    FLD_MAC(MODIFYDP, 21, 1),
    FLD_MAC(DP, 22, 2)
};

/*******************************************************************************
* prvCpssDxChDiagDescriptorRawGet
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
*                            (bytes)
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
static GT_STATUS prvCpssDxChDiagDescriptorRawGet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    INOUT GT_U32                            *rawDataLengthPtr,
    OUT GT_U32                              *rawDataPtr
)
{
    GT_STATUS   rc;                /* function return code */
    GT_U32      fieldValue;        /* register field value */
    GT_U32      wrDmaToTtiFieldValue;  /* register field value */
    GT_U32      portGroupId;      /*the port group Id - support multi-port-groups device */
    GT_U32      addr;             /* Address offset to read from. */
    GT_U32      addrNext = 0;     /* Address offset to read from for descriptors
                                     with multiple address instances */
    GT_U32      length;           /* Number of Words (4 byte) to read. */
    GT_U32      totalLength;       /* Number of bytes to read. */
    GT_BOOL     multipleInstances; /* multiple instances of registers */
    GT_BOOL     readViaWindow;     /* read descriptor parts via the same register */
    GT_U32      indexWindowReg;    /* word index in window register address       */
    GT_U32      offsetWindowReg0;  /* offset of word index in window register     */
    GT_U32      lengthWindowReg0;  /* length of word index in window register     */
    GT_U32      offsetWindowReg1;  /* offset of word index in window register     */
    GT_U32      lengthWindowReg1;  /* length of word index in window register     */
    GT_U32      windowWords;       /* amount of words in descriptor               */
    GT_U32      i;                 /* loop index                                  */
    GT_U32      mask;              /* register mask                               */
    GT_U32      value;             /* register value                              */


    multipleInstances = GT_FALSE;
    readViaWindow     = GT_FALSE;
    indexWindowReg    = 0;
    offsetWindowReg0  = 0;
    lengthWindowReg0  = 0;
    offsetWindowReg1  = 0;
    lengthWindowReg1  = 0;
    windowWords       = 0;

     switch(descriptorType)
    {
        case CPSS_DXCH_DIAG_DESCRIPTOR_WR_DMA_TO_TTI_E:
            addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.wrDmaToTti;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E:
            addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.ttiToPcl;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_PCL_TO_BRIDGE_E:
            addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.pclToBridge;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E:
            multipleInstances = GT_TRUE;
            addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.bridgeToRouterSet0;
            addrNext = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.bridgeToRouterSet1;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E:
            addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.preEgressToTxq0;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER0_TO_INGRESS_POLICER1_E:
            multipleInstances = GT_TRUE;
            addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.ingressPolicer0ToIngressPolicer1Set0;
            addrNext = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.ingressPolicer0ToIngressPolicer1Set1;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER1_TO_PRE_EGRESS_E:
            multipleInstances = GT_TRUE;
            addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.ingressPolicer1ToPreEgressSet0;
            addrNext = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.ingressPolicer1ToPreEgressSet1;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_EGRESS_POLICER_TO_EGRESS_PCL_E:
            addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.egressPolicerEgressToPclSet0;
            addrNext = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.egressPolicerEgressToPclSet1;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_TXQ_TO_HEADER_ALTERATION_E:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
                readViaWindow     = GT_TRUE;
                indexWindowReg    = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                 egrTxQConf.txQueueConfigExt;
                offsetWindowReg0   = 26;
                lengthWindowReg0   = 2;
                offsetWindowReg1   = 31;
                lengthWindowReg1   = 1;
                windowWords        = 6;
                addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                 descriptorsConfig.txqToHeaderAlteration;
            }
            else
            {
               addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                descriptorsConfig.txqToHeaderAlteration;
            }
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
             addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                 descriptorsConfig.routerToIngressPolicer;
             break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

     }

     rc = cpssDxChDiagDescriptorRawSizeGet(devNum, descriptorType,
                                           &totalLength);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* insufficient descriptor raw (HW) data size */
     if(totalLength > *rawDataLengthPtr)
     {
         CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_SIZE, LOG_ERROR_NO_MSG);
     }

     if(descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E)
     {
         /* the last 2 words are placed in other register */
         length = totalLength / 4 - 2;
     }
     else
     {
         length = totalLength / 4;
     }

     PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
         devNum, portGroupsBmp, portGroupId);

     if (readViaWindow != GT_FALSE)
     {
         /* reading descriptor via window */
         mask = 0;
         if (lengthWindowReg0 != 0)
         {
             mask |= (((1 << lengthWindowReg0) - 1) << offsetWindowReg0);
         }
         if (lengthWindowReg1 != 0)
         {
             mask |= (((1 << lengthWindowReg1) - 1) << offsetWindowReg1);
         }

         for (i = 0; (i < windowWords); i++)
         {
             value = 0;
             if (lengthWindowReg0 != 0)
             {
                 value  |= ((((1 << lengthWindowReg0) - 1) & i) << offsetWindowReg0);
             }
             if (lengthWindowReg1 != 0)
             {
                 value |=
                     ((((1 << lengthWindowReg1) - 1) & (i >> lengthWindowReg0))
                      << offsetWindowReg1);
             }

             rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(
                 devNum, portGroupId, indexWindowReg, mask, value);
             if (rc != GT_OK)
             {
                 return rc;
             }
             rc = prvCpssDrvHwPpPortGroupReadRegister(
                 devNum, portGroupId, addr, &(rawDataPtr[i]));
             if (rc != GT_OK)
             {
                 return rc;
             }
         }

         *rawDataLengthPtr = totalLength;
         return GT_OK;
     }

     if(multipleInstances == GT_TRUE)
     {
        /* read first bits of WR DMA to TTI descriptor (FirstBuffer bits) */
         rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                      PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                      descriptorsConfig.wrDmaToTti, 0, 14,
                                                 &wrDmaToTtiFieldValue);
         if(rc != GT_OK)
         {
             return rc;
         }

         /*  read first bits of first instance of given descriptor.*/
         rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                     addr, 0, 14, &fieldValue);
         if(rc != GT_OK)
         {
             return rc;
         }

         if(wrDmaToTtiFieldValue != fieldValue)
         {
             /*Go to next instance */
             addr = addrNext;
         }

     }
     rc = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId, addr, length,
                                          rawDataPtr);
     if(rc != GT_OK)
     {
         return rc;
     }

     if(descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E)
     {
         /* read last words */
         rc = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId,
                                             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                             descriptorsConfig.preEgressToTxq1,
                                             2, &rawDataPtr[5]);
         if(rc != GT_OK)
         {
             return rc;
         }
     }
     *rawDataLengthPtr = totalLength;

     return GT_OK;

}

/*******************************************************************************
* internal_cpssDxChDiagDescriptorRawGet
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
static GT_STATUS internal_cpssDxChDiagDescriptorRawGet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    INOUT GT_U32                            *rawDataLengthPtr,
    OUT GT_U32                              *rawDataPtr
)
{
    return cpssDxChDiagDescriptorPortGroupRawGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               descriptorType,
                                               rawDataLengthPtr, rawDataPtr);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDescriptorRawGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, descriptorType, rawDataLengthPtr, rawDataPtr));

    rc = internal_cpssDxChDiagDescriptorRawGet(devNum, descriptorType, rawDataLengthPtr, rawDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, descriptorType, rawDataLengthPtr, rawDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChDiagDescriptorPortGroupRawGet
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
static GT_STATUS internal_cpssDxChDiagDescriptorPortGroupRawGet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    INOUT GT_U32                            *rawDataLengthPtr,
    OUT GT_U32                              *rawDataPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E |
         CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(rawDataLengthPtr);
    CPSS_NULL_PTR_CHECK_MAC(rawDataPtr);

    return prvCpssDxChDiagDescriptorRawGet(devNum, portGroupsBmp, descriptorType,
                                           rawDataLengthPtr, rawDataPtr);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDescriptorPortGroupRawGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, descriptorType, rawDataLengthPtr, rawDataPtr));

    rc = internal_cpssDxChDiagDescriptorPortGroupRawGet(devNum, portGroupsBmp, descriptorType, rawDataLengthPtr, rawDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, descriptorType, rawDataLengthPtr, rawDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChDiagDescriptorRawSizeGet
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
static GT_STATUS internal_cpssDxChDiagDescriptorRawSizeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT  descriptorType,
    OUT GT_U32                              *rawDataLengthPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E |
         CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(rawDataLengthPtr);

    switch(descriptorType)
    {
        case CPSS_DXCH_DIAG_DESCRIPTOR_WR_DMA_TO_TTI_E:
            *rawDataLengthPtr = 8;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
                *rawDataLengthPtr = 104;
            }
            else
            {
                *rawDataLengthPtr = 152;
            }
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_PCL_TO_BRIDGE_E:
            *rawDataLengthPtr = 112;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E:
            *rawDataLengthPtr = 80;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E:
            *rawDataLengthPtr = 28;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_TXQ_TO_HEADER_ALTERATION_E:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
                *rawDataLengthPtr = 24;
            }
            else
            {
                *rawDataLengthPtr = 20;
            }
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            *rawDataLengthPtr = 32;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER0_TO_INGRESS_POLICER1_E:
        case CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER1_TO_PRE_EGRESS_E:
        case CPSS_DXCH_DIAG_DESCRIPTOR_EGRESS_POLICER_TO_EGRESS_PCL_E:
            *rawDataLengthPtr = 48;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
     return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDescriptorRawSizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, descriptorType, rawDataLengthPtr));

    rc = internal_cpssDxChDiagDescriptorRawSizeGet(devNum, descriptorType, rawDataLengthPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, descriptorType, rawDataLengthPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChDiagDescriptorGet
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
static GT_STATUS internal_cpssDxChDiagDescriptorGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    OUT CPSS_DXCH_DIAG_DESCRIPTOR_STC        *descriptorPtr
)
{
    return cpssDxChDiagDescriptorPortGroupGet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              descriptorType,
                                              descriptorPtr);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDescriptorGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, descriptorType, descriptorPtr));

    rc = internal_cpssDxChDiagDescriptorGet(devNum, descriptorType, descriptorPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, descriptorType, descriptorPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChDiagDescriptorPortGroupGet
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
static GT_STATUS internal_cpssDxChDiagDescriptorPortGroupGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT   descriptorType,
    OUT CPSS_DXCH_DIAG_DESCRIPTOR_STC        *descriptorPtr
)
{
    GT_STATUS   rc;             /* function return code */
    GT_U32      i;              /* iterator */
    GT_U32      ii;             /* iterator */
    PRV_CPSS_DXCH_DIAG_HW_DESCR_STC *hwDescr; /* pointer to descriptor in HW format */
    GT_U32      index;          /* index in descriptor */
    GT_U32      fieldsNum;      /* the number of fields in descriptor */
    GT_U32      *hwDataPtr;     /* pointer to HW data */
    GT_U32      hwDataLength;   /* HW data length */
    GT_U32      fieldOffset;    /* field offset */
    GT_BOOL     validField;     /* GT_TRUE - the field is valid,
                                   GT_FALSE - the field is invalid */
    GT_U32      fieldValue;     /* descriptor field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E |
         CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(descriptorPtr);

    switch(descriptorType)
    {
        case CPSS_DXCH_DIAG_DESCRIPTOR_WR_DMA_TO_TTI_E:
            fieldsNum = sizeof(xcat2wrdma2tti)/sizeof(xcat2wrdma2tti[0]);
            hwDescr = xcat2wrdma2tti;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E:
            fieldsNum = sizeof(xcat2tti2pcl)/sizeof(xcat2tti2pcl[0]);
            hwDescr = xcat2tti2pcl;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_PCL_TO_BRIDGE_E:
            fieldsNum = sizeof(xcat2pcl2l2i)/sizeof(xcat2pcl2l2i[0]);
            hwDescr = xcat2pcl2l2i;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E:
            fieldsNum = sizeof(xcat2l2i2rtr)/sizeof(xcat2l2i2rtr[0]);
            hwDescr = xcat2l2i2rtr;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E:
        case CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER0_TO_INGRESS_POLICER1_E:
            if((descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E) &&
               ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* both descriptors have a same format */
            fieldsNum = sizeof(xcat2plr_outgoing)/sizeof(xcat2plr_outgoing[0]);
            hwDescr = xcat2plr_outgoing;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER1_TO_PRE_EGRESS_E:
            fieldsNum = sizeof(xcat2plr_outgoing)/sizeof(xcat2plr_outgoing[0]);
            hwDescr = xcat2plr_outgoing;
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
                fieldsNum = sizeof(xcateq2txq)/sizeof(xcateq2txq[0]);
                hwDescr = xcateq2txq;
            }
            else
            {
                fieldsNum = sizeof(xcat2eq2txq)/sizeof(xcat2eq2txq[0]);
                hwDescr = xcat2eq2txq;
            }
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_TXQ_TO_HEADER_ALTERATION_E:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
                fieldsNum = sizeof(xcat_txq2rddma)/sizeof(xcat_txq2rddma[0]);
                hwDescr = xcat_txq2rddma;
            }
            else
            {
                fieldsNum = sizeof(xcat2rddma2ha)/sizeof(xcat2rddma2ha[0]);
                hwDescr = xcat2rddma2ha;
            }
            break;
        case CPSS_DXCH_DIAG_DESCRIPTOR_EGRESS_POLICER_TO_EGRESS_PCL_E:
            fieldsNum = sizeof(xcat2eplr2epcl)/sizeof(xcat2eplr2epcl[0]);
            hwDescr = xcat2eplr2epcl;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = cpssDxChDiagDescriptorRawSizeGet(devNum, descriptorType, &hwDataLength);
    if(rc != GT_OK)
    {
        return rc;
    }

    hwDataPtr = (GT_U32*) cpssOsMalloc(sizeof(GT_U32) *  hwDataLength / 4);


    rc = prvCpssDxChDiagDescriptorRawGet(devNum, portGroupsBmp, descriptorType,
                                        &hwDataLength, hwDataPtr);
    if(rc != GT_OK)
    {
        cpssOsFree(hwDataPtr);
        return rc;
    }

    cpssOsMemSet(descriptorPtr, 0, sizeof(CPSS_DXCH_DIAG_DESCRIPTOR_STC));

    for(i = 0; i < fieldsNum; i++)
    {

        /* set valid fields */
        validField = GT_TRUE;

        if(hwDescr[i].condDataPtr != NULL)
        {
            ii = 0;
            do
            {
                fieldOffset = hwDescr[i].condDataPtr[ii].fieldOffset;

                /* Get condition field */
                U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, fieldOffset,
                                     hwDescr[i].condDataPtr[ii].fieldLength,
                                     fieldValue);

                if ((hwDescr[i].condDataPtr[ii].operation ==
                   PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_AND_E)
                    || (hwDescr[i].condDataPtr[ii].operation ==
                   PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_LAST_E))
                {
                    /* ANS and LAST operation case */
                    if((((hwDescr[i].condDataPtr[ii].type ==
                          PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_EQUAL_E) &&
                       (fieldValue != hwDescr[i].condDataPtr[ii].conditionValue))) ||
                       (((hwDescr[i].condDataPtr[ii].type ==
                           PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_DIFFER_E) &&
                         (fieldValue == hwDescr[i].condDataPtr[ii].conditionValue))))
                    {
                        validField = GT_FALSE;
                    }
                }
                else
                {
                    /* OR and OR_SEQ operation case */
                    if(((hwDescr[i].condDataPtr[ii].type ==
                          PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_EQUAL_E) &&
                       (fieldValue == hwDescr[i].condDataPtr[ii].conditionValue)) ||
                       ((hwDescr[i].condDataPtr[ii].type ==
                         PRV_CPSS_DXCH_DIAG_CONDITION_TYPE_DIFFER_E) &&
                       (fieldValue != hwDescr[i].condDataPtr[ii].conditionValue)))
                    {
                        validField = GT_TRUE;
                        if (hwDescr[i].condDataPtr[ii].operation ==
                            PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_OR_E)
                        {
                            break;
                        }
                    }
                }

            }while(hwDescr[i].condDataPtr[ii++].operation !=
                   PRV_CPSS_DXCH_DIAG_OPERATION_TYPE_LAST_E);
        }


        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
        {
            /* field SrcOwnDev0 isn't valid for xCat2 and lion */
            if((descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_TXQ_TO_HEADER_ALTERATION_E) &&
               (hwDescr[i].fieldName == CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SRCOWNDEV0_E))
            {
                validField = GT_FALSE;
            }
        }

        if( (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
        {
            /* field BadIPxBC isn't valid for lion and lion2 */
            if((descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E) &&
               (hwDescr[i].fieldName == CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_BADIPXBC_E))
            {
                validField = GT_FALSE;
            }

        }

        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
        {
            if((hwDescr[i].fieldName == CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ISPTP_E) ||
                (hwDescr[i].fieldName ==
                 CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PTPTRIGGERTYPE_E))
            {
                validField = GT_FALSE;
            }
            if(((descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER0_TO_INGRESS_POLICER1_E) ||
               (descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E))  &&
               ((hwDescr[i].fieldName ==
                CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_QCNPACKET_E) ||
                (hwDescr[i].fieldName ==
                CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ORIGQCNPRIO_E) ||
                (hwDescr[i].fieldName ==
                CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETISCNTAGGED_E) ||
                (hwDescr[i].fieldName ==
                CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_CPIDMATCH_E)))

            {
                validField = GT_FALSE;
            }

            if((descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E) &&
                ((hwDescr[i].fieldName ==
                   CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DOROUTEHA_E) ||
                 (hwDescr[i].fieldName ==
                   CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_LABEL0_E) ||
                 (hwDescr[i].fieldName ==
                   CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_LABEL1_E) ||
                 (hwDescr[i].fieldName ==
                   CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MPLS_LABEL2_E) ||
                 (hwDescr[i].fieldName ==
                   CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_HASHMASKINDEX_E) ||
                 (hwDescr[i].fieldName ==
                  CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_USEINTERFACEBASEDINDEX_E)))

            {
                validField = GT_FALSE;
            }

            if((descriptorType == CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E) &&
               (hwDescr[i].fieldName == CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_PACKETISLLCSNAP_E))
            {
                validField = GT_FALSE;
            }
        }

        if(validField == GT_TRUE)
        {
            index = hwDescr[i].fieldName;

            descriptorPtr->fieldValueValid[index] = GT_TRUE;

            fieldOffset = hwDescr[i].fieldOffset;

            U32_GET_FIELD_IN_ENTRY_MAC(hwDataPtr, fieldOffset,
                                        hwDescr[i].fieldLength, fieldValue);

            /* connect splitted fields */
           descriptorPtr->fieldValue[index] |=
               (fieldValue << hwDescr[i].swFieldOffset);
        }

    }

    cpssOsFree(hwDataPtr);

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDescriptorPortGroupGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, descriptorType, descriptorPtr));

    rc = internal_cpssDxChDiagDescriptorPortGroupGet(devNum, portGroupsBmp, descriptorType, descriptorPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, descriptorType, descriptorPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}




