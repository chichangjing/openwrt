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
* cpssDxChPcl.c
*
* DESCRIPTION:
*       CPSS CH general PCL API implementation
*
* FILE REVISION NUMBER:
*       $Revision: 123$
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpss/dxCh/dxChxGen/tcam/private/prvCpssDxChTcam.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPclLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/* used in structures at the places of not relevant values */
/* as offset of not used register address                  */
#define DUMMY8_CNS  0xFF
#define DUMMY32_CNS 0xFFFFFFFF

/* different devices have the same structure of register addresses */
/* but register addresses                                          */
/* the macros below helps to keep in DB not the register address   */
/* but the offset of register address in the structure of register */
/* addresses                                                       */

/* offset from the beginning of structure */
#define MEMBER_OFFSET_MAC(_type, _member)        (GT_UINTPTR)&(((_type*)0)->_member)

/* extract member address by pointer      */
#define MEMBER_POINTER_MAC(_ptr, _offset, _type) \
    (_type*)(((char*)(_ptr)) + (_offset))

/* offset of PCL register */
#define PCL_REG_OFFSET_MAC(_reg) \
    MEMBER_OFFSET_MAC(PRV_CPSS_DXCH_PP_REGS_ADDR_STC, pclRegs._reg)

/* address of PCL register */
#define PCL_REG_ADDRESS_MAC(_devNum, _offset) \
    *(MEMBER_POINTER_MAC(PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum), _offset, GT_U32))

/* HW values for Cheetah offset types in UDBs */
#define UDB_HW_L2_CNS      0
#define UDB_HW_L3_CNS      1
#define UDB_HW_L4_CNS      2
#define UDB_HW_IPV6EH_CNS  3
/* HW values for XCAT offset types in UDBs */
#define UDB_XCAT_HW_L2_CNS              0
#define UDB_XCAT_HW_MPLS_CNS            1
#define UDB_XCAT_HW_L3_CNS              2
#define UDB_XCAT_HW_L4_CNS              3

#define UDB_DB_L2_CNS      (1 << UDB_HW_L2_CNS)
#define UDB_DB_L3_CNS      (1 << UDB_HW_L3_CNS)
#define UDB_DB_L4_CNS      (1 << UDB_HW_L4_CNS)
#define UDB_DB_IPV6EH_CNS (1 << UDB_HW_IPV6EH_CNS)

#define UDB_DB_L2_L3_CNS   (UDB_DB_L2_CNS | UDB_DB_L3_CNS)
#define UDB_DB_L3_L4_CNS   (UDB_DB_L3_CNS | UDB_DB_L4_CNS)
#define UDB_DB_L2_L3_L4_CNS  \
    (UDB_DB_L2_CNS | UDB_DB_L3_CNS | UDB_DB_L4_CNS)
#define UDB_DB_L2_L3_L4_IPV6EH_CNS \
    (UDB_DB_L2_CNS | UDB_DB_L3_CNS | UDB_DB_L4_CNS | UDB_DB_IPV6EH_CNS)
#define UDB_DB_L3_L4_IPV6EH_CNS \
    (UDB_DB_L3_CNS | UDB_DB_L4_CNS | UDB_DB_IPV6EH_CNS)

/* 10 words in sip5    ipcl 316 bits (for 50 UDBs)
   10 words in sip5    epcl 306 bits (for 50 UDBs)
   10 words in sip5.20 epcl 506 bits (for 70 UDBs)
   10 words in sip5.20 epcl 306 bits (for 50 UDBs)
    */
#define UDB_SELECTION_WORDS     ((506 + 31)/32)

/* static variables */

/* Key User Defined Bytes DB */
static struct
{
    GT_U32  regAddrOff[2]; /* address of UDB configuration registers */
    GT_U8   udbAmount;     /* number of UDBs */
    GT_U8   comparatorBit; /* bit to switch some UDB to comparator  */
    GT_U8   comparatorUdb; /*index of UDB that can contain comparator*/
    struct
    {
        GT_U8 regIdx;         /* register 0 or 1 of pair                   */
        GT_U8 udbCfgStartBit; /* 0-31 bit shift for pair <off-type, offset>*/
        GT_U8 validOffTypesBmp; /* bitmap (1 << hw_value)                  */
    } udbInfo[6];
} keyUdbInfo[6] =
{
    /* key0 - EXT_NOT_IPV6 */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey0bytes0_2),
            PCL_REG_OFFSET_MAC(udbConfigKey0bytes3_5),
        },
        6 /*udbAmount*/, 31 /*comparatorBit*/, 2 /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_L4_CNS},
            {0, 11, UDB_DB_L2_L3_L4_CNS},
            {0, 22, UDB_DB_L3_L4_CNS},
            {1, 0,  UDB_DB_L3_L4_CNS},
            {1, 11, UDB_DB_L3_L4_CNS},
            {1, 22, UDB_DB_L3_L4_CNS},
        }
    },
    /* key1 - EXT_IPV6_L2 */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey1bytes0_2),
            PCL_REG_OFFSET_MAC(udbConfigKey1bytes3_5),
        },
        6 /*udbAmount*/, 31 /*comparatorBit*/, 2 /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 11, UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 22, UDB_DB_L3_L4_IPV6EH_CNS},
            {1, 0,  UDB_DB_L3_L4_IPV6EH_CNS},
            {1, 11, UDB_DB_L3_L4_IPV6EH_CNS},
            {1, 22, UDB_DB_L3_L4_IPV6EH_CNS},
        }
    },
    /* key2 - EXT_IPV6_L4 */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey2bytes0_2),
            DUMMY32_CNS,
        },
        3 /*udbAmount*/, 31 /*comparatorBit*/, 2 /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 11, UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 22, UDB_DB_L3_L4_IPV6EH_CNS},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
        }
    },
    /* key3 - STD_NOT_IP */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey3bytes0_2),
            DUMMY32_CNS,
        },
        3 /*udbAmount*/, DUMMY8_CNS /*comparatorBit*/,
        DUMMY8_CNS /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_CNS},
            {0, 11, UDB_DB_L2_L3_CNS},
            {0, 22, UDB_DB_L3_CNS},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
        }
    },
    /* key4 - STD_IPV4_IPV6_L2_QOS */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey4bytes0_1),
            DUMMY32_CNS,
        },
        2 /*udbAmount*/, 20 /*comparatorBit*/, 0 /*comparatorUdb*/,
        {
            {0, 11, UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 22, UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
        }
    },
    /* key5 - STD_IPV4_L4 */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey5bytes0_2),
            DUMMY32_CNS,
        },
        3 /*udbAmount*/, 9 /*comparatorBit*/, 0 /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_L4_CNS},
            {0, 11, UDB_DB_L2_L3_L4_CNS},
            {0, 22, UDB_DB_L3_L4_CNS},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
        }
    },
};

/* HW <==> SW conversion tables for PCL CFG tables key formats */
/* Index is the HW value                                       */

/* DxCh3 IPCL LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxCh3SwKeyType_IPCL_NOT_IP[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    /* 1 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E,
    /* 6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2 IPCL LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxCh2SwKeyType_IPCL_NOT_IP[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 2 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* xCat IPCL xCat LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChXCatSwKeyType_IPCL_NOT_IP[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    /* 1 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /* 4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /* 5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E,
    /* 6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2, DxCh3 IPCL LOOKUP IPV4 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChSwKeyType_IPCL_IPV4[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E,
    /* 6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* xCat IPCL xCat LOOKUP IPV4 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChXCatSwKeyType_IPCL_IPV4[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /* 4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /* 5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E,
    /* 6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh3 IPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxCh3SwKeyType_IPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E,
    /* 6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2 IPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxCh2SwKeyType_IPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* xCat IPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChXCatSwKeyType_IPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /* 4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /* 5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E,
    /* 6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E,
    /* 7 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E
};

/* DxCh2, DxCh3, xCat, xCat3 EPCL LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChSwKeyType_EPCL_NOT_IP[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2, DxCh3, xCat, xCat3 EPCL LOOKUP IPV4 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChSwKeyType_EPCL_IPV4[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2, DxCh3, xCat, xCat3 EPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChSwKeyType_EPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* Bobcat2, Caelum, Bobcat3 IPCL LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChLion3SwKeyType_IPCL_NOT_IP[] =
{
    /*  0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    /*  1 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /*  2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /*  3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /*  4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /*  5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E,
    /*  6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E,
    /*  7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /*  8 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E,
    /*  9 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E,
    /* 10 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E,
    /* 11 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E,
    /* 12 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E,
    /* 13 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E,
    /* 14 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 15 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* Bobcat2, Caelum, Bobcat3 IPCL LOOKUP IPV4 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChLion3SwKeyType_IPCL_IPV4[] =
{
    /*  0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /*  1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
    /*  2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /*  3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /*  4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /*  5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E,
    /*  6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E,
    /*  7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /*  8 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E,
    /*  9 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E,
    /* 10 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E,
    /* 11 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E,
    /* 12 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E,
    /* 13 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E,
    /* 14 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 15 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* Bobcat2, Caelum, Bobcat3 IPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChLion3SwKeyType_IPCL_IPV6[] =
{
    /*  0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /*  1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    /*  2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    /*  3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /*  4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /*  5 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E,
    /*  6 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E,
    /*  7 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,
    /*  8 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E,
    /*  9 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E,
    /* 10 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E,
    /* 11 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E,
    /* 12 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E,
    /* 13 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E,
    /* 14 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 15 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* Bobcat2, Caelum, Bobcat3 EPCL LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChLion3SwKeyType_EPCL_NOT_IP[] =
{
    /*  0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,
    /*  1 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    /*  2 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E,
    /*  3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /*  4 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E,
    /*  5 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E,
    /*  6 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E,
    /*  7 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E,
    /*  8 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E,
    /*  9 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E,
    /* 10 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 11 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 12 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 13 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 14 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 15 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* Bobcat2, Caelum, Bobcat3 EPCL LOOKUP IPV4 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChLion3SwKeyType_EPCL_IPV4[] =
{
    /*  0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    /*  1 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,
    /*  2 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    /*  3 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E,
    /*  4 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E,
    /*  5 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E,
    /*  6 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E,
    /*  7 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E,
    /*  8 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E,
    /*  9 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E,
    /* 10 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 11 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 12 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 13 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 14 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 15 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* Bobcat2, Caelum, Bobcat3 EPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChLion3SwKeyType_EPCL_IPV6[] =
{
    /*  0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    /*  1 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E,
    /*  2 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,
    /*  3 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E,
    /*  4 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E,
    /*  5 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E,
    /*  6 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E,
    /*  7 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E,
    /*  8 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E,
    /*  9 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E,
    /* 10 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 11 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 12 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 13 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 14 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 15 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/*
 * Typedef: enum LION3_HW_PACKET_TYPE_ENT
 *
 * Description: IPCL,EPCL supports the SIP5 enumeration
 *
 * Enumerations:
 *    LION3_HW_PACKET_TYPE_IPV4_TCP_E       - IPV4 TCP
 *    LION3_HW_PACKET_TYPE_IPV4_UDP_E       - IPV4 UDP
 *    LION3_HW_PACKET_TYPE_MPLS_E           - MPLS
 *    LION3_HW_PACKET_TYPE_IPV4_FRAGMENT_E  - IPV4 Fragment
 *    LION3_HW_PACKET_TYPE_IPV4_OTHER_E     - IPV4 Other
 *    LION3_HW_PACKET_TYPE_ETHERNET_OTHER_E - Ethernet Other
 *    LION3_HW_PACKET_TYPE_IPV6_E           - IPV6
 *    LION3_HW_PACKET_TYPE_IPV6_TCP_E       - IPV6 TCP
 *    LION3_HW_PACKET_TYPE_IPV6_UDP_E       - IPV6 UDP
 *    LION3_HW_PACKET_TYPE_UDE0_E           - User Defined Ethertype 0
 *    LION3_HW_PACKET_TYPE_UDE1_E           - User Defined Ethertype 1
 *    LION3_HW_PACKET_TYPE_UDE2_E           - User Defined Ethertype 2
 *    LION3_HW_PACKET_TYPE_UDE3_E           - User Defined Ethertype 3
 *    LION3_HW_PACKET_TYPE_UDE4_E           - User Defined Ethertype 4
 *    LION3_HW_PACKET_TYPE_UDE5_E           - User Defined Ethertype 5
 *    LION3_HW_PACKET_TYPE_UDE6_E           - User Defined Ethertype 6
 *    LION3_HW_PACKET_TYPE_INVALID_E        - not suppotred packet type
 *
 * Comment:
 *
 */
typedef enum{
    LION3_HW_PACKET_TYPE_IPV4_TCP_E                ,/* 0*/
    LION3_HW_PACKET_TYPE_IPV4_UDP_E                ,/* 1*/
    LION3_HW_PACKET_TYPE_MPLS_E                    ,/* 2*/
    LION3_HW_PACKET_TYPE_IPV4_FRAGMENT_E           ,/* 3*/
    LION3_HW_PACKET_TYPE_IPV4_OTHER_E              ,/* 4*/
    LION3_HW_PACKET_TYPE_ETHERNET_OTHER_E          ,/* 5*/
    LION3_HW_PACKET_TYPE_IPV6_E                    ,/* 6*/
    LION3_HW_PACKET_TYPE_IPV6_TCP_E                ,/* 7*/
    LION3_HW_PACKET_TYPE_IPV6_UDP_E                ,/* 8*/
    LION3_HW_PACKET_TYPE_UDE0_E                    ,/* 9*/
    LION3_HW_PACKET_TYPE_UDE1_E                    ,/*10*/
    LION3_HW_PACKET_TYPE_UDE2_E                    ,/*11*/
    LION3_HW_PACKET_TYPE_UDE3_E                    ,/*12*/
    LION3_HW_PACKET_TYPE_UDE4_E                    ,/*13*/
    LION3_HW_PACKET_TYPE_UDE5_E                    ,/*14*/
    LION3_HW_PACKET_TYPE_UDE6_E                    ,/*15*/
    LION3_HW_PACKET_TYPE_INVALID_E = 0xFF           /*0xFF*/


}LION3_HW_PACKET_TYPE_ENT;
/* convert from CPSS_DXCH_PCL_PACKET_TYPE_ENT to LION3_HW_PACKET_TYPE_ENT */
static LION3_HW_PACKET_TYPE_ENT  lion3ConvertSwPacketTypeToHwValueArr[CPSS_DXCH_PCL_PACKET_TYPE_LAST_E] =
{
/*CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E        */  LION3_HW_PACKET_TYPE_IPV4_TCP_E          ,
/*CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E        */  LION3_HW_PACKET_TYPE_IPV4_UDP_E          ,
/*CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E            */  LION3_HW_PACKET_TYPE_MPLS_E              ,
/*CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E   */  LION3_HW_PACKET_TYPE_IPV4_FRAGMENT_E     ,
/*CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E      */  LION3_HW_PACKET_TYPE_IPV4_OTHER_E        ,
/*CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E  */  LION3_HW_PACKET_TYPE_ETHERNET_OTHER_E    ,
/*CPSS_DXCH_PCL_PACKET_TYPE_UDE_E             */  LION3_HW_PACKET_TYPE_UDE0_E              ,
/*CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E            */  LION3_HW_PACKET_TYPE_INVALID_E           ,
/*CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E            */  LION3_HW_PACKET_TYPE_UDE1_E              ,
/*CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E            */  LION3_HW_PACKET_TYPE_UDE2_E              ,
/*CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E            */  LION3_HW_PACKET_TYPE_UDE3_E              ,
/*CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E            */  LION3_HW_PACKET_TYPE_UDE4_E              ,
/*CPSS_DXCH_PCL_PACKET_TYPE_IPV6_TCP_E        */  LION3_HW_PACKET_TYPE_IPV6_TCP_E          ,
/*CPSS_DXCH_PCL_PACKET_TYPE_IPV6_UDP_E        */  LION3_HW_PACKET_TYPE_IPV6_UDP_E          ,
/*CPSS_DXCH_PCL_PACKET_TYPE_IPV6_OTHER_E      */  LION3_HW_PACKET_TYPE_IPV6_E              ,
/*CPSS_DXCH_PCL_PACKET_TYPE_UDE5_E            */  LION3_HW_PACKET_TYPE_UDE5_E              ,
/*CPSS_DXCH_PCL_PACKET_TYPE_UDE6_E            */  LION3_HW_PACKET_TYPE_UDE6_E
};


/*
 * Typedef: enum LION3_HW_PCL_UDB_ANCHOR_ENT
 *
 * Description: UDB Anchors.
 *    IPCL,EPCL supports the SIP5 enumeration.
 *
 * Enumerations:
 *    LION3_HW_PCL_UDB_ANCHOR_L2_E            - L2
 *    LION3_HW_PCL_UDB_ANCHOR_MPLS_MINUS_2_E  - MPLS header origin minus 2 bytes
 *    LION3_HW_PCL_UDB_ANCHOR_L3_MINUS_2_E    - L3 header origin minus 2 bytes
 *    LION3_HW_PCL_UDB_ANCHOR_L4_E       E    - L4
 *    LION3_HW_PCL_UDB_ANCHOR_TUNNEL_L2_E     - TUNNEL L2
 *    LION3_HW_PCL_UDB_ANCHOR_TUNNEL_L3_E_E   - TUNNEL L3
 *    LION3_HW_PCL_UDB_ANCHOR_METADATA_E      - METADATA
 *    LION3_HW_PCL_UDB_ANCHOR_NOT_SUPPORTED_E - Not supported - conversion table placeholder
 *
 * Comment:
 *
 */
typedef enum{
    LION3_HW_PCL_UDB_ANCHOR_L2_E            =  0,
    LION3_HW_PCL_UDB_ANCHOR_MPLS_MINUS_2_E  =  1,
    LION3_HW_PCL_UDB_ANCHOR_L3_MINUS_2_E    =  2,
    LION3_HW_PCL_UDB_ANCHOR_L4_E            =  3,
    LION3_HW_PCL_UDB_ANCHOR_TUNNEL_L2_E     =  4,
    LION3_HW_PCL_UDB_ANCHOR_TUNNEL_L3_E     =  5,
    LION3_HW_PCL_UDB_ANCHOR_METADATA_E      =  6,
    LION3_HW_PCL_UDB_ANCHOR_NOT_SUPPORTED_E =  0xFF

}LION3_HW_PCL_UDB_ANCHOR_ENT;

/* convert from CPSS_DXCH_PCL_OFFSET_TYPE_ENT to  LION3_HW_PCL_UDB_ANCHOR_ENT*/
static LION3_HW_PCL_UDB_ANCHOR_ENT lion3ConvertSwUdbAnchorToHwValueArr[] =
{
    /*CPSS_DXCH_PCL_OFFSET_L2_E,                 */ LION3_HW_PCL_UDB_ANCHOR_L2_E,
    /*CPSS_DXCH_PCL_OFFSET_L3_E,                 */ LION3_HW_PCL_UDB_ANCHOR_NOT_SUPPORTED_E,
    /*CPSS_DXCH_PCL_OFFSET_L4_E,                 */ LION3_HW_PCL_UDB_ANCHOR_L4_E,
    /*CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E,       */ LION3_HW_PCL_UDB_ANCHOR_NOT_SUPPORTED_E,
    /*CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E, */ LION3_HW_PCL_UDB_ANCHOR_NOT_SUPPORTED_E,
    /*CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,         */ LION3_HW_PCL_UDB_ANCHOR_L3_MINUS_2_E,
    /*CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E,       */ LION3_HW_PCL_UDB_ANCHOR_MPLS_MINUS_2_E,
    /*CPSS_DXCH_PCL_OFFSET_TUNNEL_L2_E,          */ LION3_HW_PCL_UDB_ANCHOR_TUNNEL_L2_E,
    /*CPSS_DXCH_PCL_OFFSET_TUNNEL_L3_MINUS_2_E,  */ LION3_HW_PCL_UDB_ANCHOR_TUNNEL_L3_E,
    /*CPSS_DXCH_PCL_OFFSET_METADATA_E,           */ LION3_HW_PCL_UDB_ANCHOR_METADATA_E,
    /*CPSS_DXCH_PCL_OFFSET_INVALID_E             */ LION3_HW_PCL_UDB_ANCHOR_L2_E
};

/*
 * struct (unnamed)
 *
 * Description:
 *     This structure describes Ranges of UDB number and offset
 *     per offset type.
 *     (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 * Fields:
 *     lowUdb        - The UDB number low  (included) bound
 *     maxUdb        - The UDB number high (excluded) bound
 *     ingrMaxOffset - The Ingress PCL packet offset high (included) bound
 *     egrMaxOffset  - The Egress PCL packet offset high (included) bound
 *
 *  Comments:
 */
static struct
{
    GT_U32 lowUdb;
    GT_U32 maxUdb;
    GT_U32 ingrMaxOffset;
    GT_U32 egrMaxOffset;
} lion3UdbRangePerAnchorArr[] =
{
    /*CPSS_DXCH_PCL_OFFSET_L2_E,                 */ {0,  CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS, 127, 255},
    /*CPSS_DXCH_PCL_OFFSET_L3_E,                 */ {0,  0, 0, 0},
    /*CPSS_DXCH_PCL_OFFSET_L4_E,                 */ {0,  CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS, 127, 255},
    /*CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E,       */ {0,  0, 0, 0},
    /*CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E, */ {0,  0, 0, 0},
    /*CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,         */ {0,  CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS, 127, 255},
    /*CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E,       */ {0,  CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS, 127, 20},
    /*CPSS_DXCH_PCL_OFFSET_TUNNEL_L2_E,          */ {0,  CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS, 62, 18},
    /*CPSS_DXCH_PCL_OFFSET_TUNNEL_L3_MINUS_2_E,  */ {0,  CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS, 56, 64},
    /*CPSS_DXCH_PCL_OFFSET_METADATA_E,           */ {30, CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS, 127, 255},
    /*CPSS_DXCH_PCL_OFFSET_INVALID_E             */ {0, CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS, 127, 255}
} ;

/*******************************************************************************
* prvCpssDxChPclHwToSwCnvArrGet
*
* DESCRIPTION:
*   The function calculates address of 8-elements array of SW values
*   for 3-bit key type HW fields in PCL Configuration tables
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*       direction          - ingress or egress PCL key
*       pktType            - NOT-IP, IPV4 or IPV6 packet
*
* OUTPUTS:
*       hwToSwCnvPtrPtr    - pointer to array of SW values
*       hwToSwCnvArraySizePtr - (pointer to) size of array of SW values
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclHwToSwCnvArrGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    IN  GT_U32                             pktType,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT **hwToSwCnvPtrPtr,
    OUT GT_U32                             *hwToSwCnvArraySizePtr
)
{
    GT_BOOL isCh2;     /* is Cheeta2 device */

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */

        *hwToSwCnvArraySizePtr = 16;

        switch (direction)
        {
            case CPSS_PCL_DIRECTION_INGRESS_E:
                switch (pktType)
                {
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChLion3SwKeyType_IPCL_NOT_IP;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChLion3SwKeyType_IPCL_IPV4;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChLion3SwKeyType_IPCL_IPV6;
                        break;
                    default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;
            case CPSS_PCL_DIRECTION_EGRESS_E:
                switch (pktType)
                {
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChLion3SwKeyType_EPCL_NOT_IP;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChLion3SwKeyType_EPCL_IPV4;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChLion3SwKeyType_EPCL_IPV6;
                        break;
                    default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        return GT_OK;
    }

    /* for not EARCH devices - 8 values in table */
    *hwToSwCnvArraySizePtr = PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS;

    isCh2 = (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_CHEETAH2_E)
        ? GT_TRUE : GT_FALSE;

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat A1 and above */
                switch (pktType)
                {
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChXCatSwKeyType_IPCL_NOT_IP;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChXCatSwKeyType_IPCL_IPV4;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChXCatSwKeyType_IPCL_IPV6;
                        break;
                    default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            else
            {
                /* DxCh, DxCh2, DxCh3 */
                switch (pktType)
                {
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS:
                        *hwToSwCnvPtrPtr =
                            (isCh2 == GT_FALSE)
                                ? prvCpssDxCh3SwKeyType_IPCL_NOT_IP
                                : prvCpssDxCh2SwKeyType_IPCL_NOT_IP;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChSwKeyType_IPCL_IPV4;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS:
                        *hwToSwCnvPtrPtr =
                            (isCh2 == GT_TRUE)
                                ? prvCpssDxCh2SwKeyType_IPCL_IPV6
                                : prvCpssDxCh3SwKeyType_IPCL_IPV6;
                        break;
                    default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            switch (pktType)
            {
                case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS:
                    *hwToSwCnvPtrPtr = prvCpssDxChSwKeyType_EPCL_NOT_IP;
                    break;
                case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS:
                    *hwToSwCnvPtrPtr = prvCpssDxChSwKeyType_EPCL_IPV4;
                    break;
                case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS:
                    *hwToSwCnvPtrPtr = prvCpssDxChSwKeyType_EPCL_IPV6;
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclKeyTypeHwToSw
*
* DESCRIPTION:
*   The function converts HW CFG table key type value to SW value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*       direction          - ingress or egress PCL key
*       pktType            - NOT-IP, IPV4 or IPV6 packet
*       hwKeyType          - HW CFG table key type value
*
* OUTPUTS:
*       swKeyTypePtr       - pointer to SW CFG table key type value
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on wrong parameters
*       GT_BAD_STATE       - unsupported HW value
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclKeyTypeHwToSw
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    IN  GT_U32                             pktType,
    IN  GT_U32                             hwKeyType,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *swKeyTypePtr
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *hwToSwCnvArrPtr; /* CNV table    */
    GT_STATUS                           rc;              /* return code  */
    GT_U32                              hwKeyRange;      /* HW Key Range */

    /* select HW <==> SW convertion table */
    rc = prvCpssDxChPclHwToSwCnvArrGet(
        devNum, direction, pktType, &hwToSwCnvArrPtr, &hwKeyRange);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (hwKeyType >= hwKeyRange)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* convert using HW value as index */
    *swKeyTypePtr = hwToSwCnvArrPtr[hwKeyType];

    return (*swKeyTypePtr == PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS)
            ? GT_BAD_STATE : GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclKeyTypeSwToHw
*
* DESCRIPTION:
*   The function converts SW CFG table key type value to HW value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*       direction          - ingress or egress PCL key
*       pktType            - NOT-IP, IPV4 or IPV6 packet
*       swKeyType          - SW CFG table key type value
*
* OUTPUTS:
*       hwKeyTypePtr       - pointer to HW CFG table key type value
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclKeyTypeSwToHw
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    IN  GT_U32                             pktType,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT swKeyType,
    OUT GT_U32                             *hwKeyTypePtr
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *hwToSwCnvArrPtr; /* CNV table    */
    GT_STATUS                           rc;              /* return code  */
    GT_U32                              i;               /* loop counter */
    GT_U32                              hwKeyRange;      /* HW Key Range */


    if( PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E || /* ch1,2*/
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) /*xcat2*/
    {
        /* this device not supports those 6 keys */
        switch (swKeyType)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            default:
                break;
        }
    }

    /* select HW <==> SW convertion table */
    rc = prvCpssDxChPclHwToSwCnvArrGet(
        devNum, direction, pktType, &hwToSwCnvArrPtr, &hwKeyRange);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* search SW value in the table - the index is HW value */
    for (i = 0; (i < hwKeyRange); i++)
    {
        if (hwToSwCnvArrPtr[i] == swKeyType)
        {
            *hwKeyTypePtr = i;
            return GT_OK;
        }
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* internal_cpssDxChPclInit
*
* DESCRIPTION:
*   The function initializes the device for following configuration
*   and using Policy engine
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclInit
(
    IN GT_U8                           devNum
)
{
    /* this function is reserved */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclInit
*
* DESCRIPTION:
*   The function initializes the device for following configuration
*   and using Policy engine
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclInit
(
    IN GT_U8                           devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChPclInit(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChCheetahPclUDBRegGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte HW configuration position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       regAddrPtr       - (pointer to) register address
*       startBitPtr      - (pointer to) start bit in the register
*       validTypesPtr    - (pointer to) mask of valid Offset types
*       comparatorBitPtr - (pointer to) number of bit contains the
*                          TCP/UDB port comparator flag in register
*                          or 0xFFFFFFFF if none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCheetahPclUDBRegGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  GT_U32                               udbIndex,
    OUT GT_U32                               *regAddrPtr,
    OUT GT_U32                               *startBitPtr,
    OUT GT_U32                               *validTypesPtr,
    OUT GT_U32                               *comparatorBitPtr
)
{
    GT_U32    keyIdx;        /* key format index 0-5     */
    GT_U32    regOff;        /* register address offset  */
    GT_BOOL   isNotCheetah1; /* GT_TRUE for not Cheetah1 */

    /* get device type */
     isNotCheetah1 =
         (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
          == CPSS_PP_FAMILY_CHEETAH_E)
         ? GT_FALSE : GT_TRUE;

    /* For the given user defined byte exactly one register must be updated. */
    /* If this byte cannot be used for comparator the updated bits in the    */
    /* register can be represented as subfield .                             */
    /* For the byte that may be was used for comparator but now will be      */
    /* configured as regular user defined byte both the "comparator" bit     */
    /* must be set to 0 and regular <offset_type, offeset> bits must be set. */
    /* get Lookup Key index */
    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            keyIdx = 3;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            keyIdx = 4;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            keyIdx = 5;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            keyIdx = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            keyIdx = 1;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            keyIdx = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check UDB index by DB */
    if (udbIndex >= keyUdbInfo[keyIdx].udbAmount)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* retrieve register address */
    regOff  =
        keyUdbInfo[keyIdx].regAddrOff[
            keyUdbInfo[keyIdx].udbInfo[udbIndex].regIdx];
    *regAddrPtr    = PCL_REG_ADDRESS_MAC(devNum, regOff);

    *startBitPtr   = keyUdbInfo[keyIdx].udbInfo[udbIndex].udbCfgStartBit;
    *validTypesPtr = keyUdbInfo[keyIdx].udbInfo[udbIndex].validOffTypesBmp;

    if ((isNotCheetah1 == GT_TRUE)
        && (keyUdbInfo[keyIdx].comparatorUdb == udbIndex))
    {
        *comparatorBitPtr = keyUdbInfo[keyIdx].comparatorBit;
    }
    else
    {
        *comparatorBitPtr = 0xFFFFFFFF;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXCatPclUDBEntryIndexGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte Entry index
*   Relevant for Ingress PCL only.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*       packetType   - packet Type
*       udbIndex     - index of UDB specified by application
*
* OUTPUTS:
*       entryIndexPtr    - (pointer to) User Defined Byte Entry index
*       internalIndexPtr - (pointer to) internal index of UDB in the entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXCatPclUDBEntryIndexGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  GT_U32                               udbIndex,
    OUT GT_U32                               *entryIndexPtr,
    OUT GT_U32                               *internalIndexPtr
)
{
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (packetType > CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E)
    {
        /* assumed that the device is Lion and above (but not sip5) */
        /* the UDE1-4 packet types supported by Lion and above only */
        if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum) == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    switch (packetType)
    {
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E:
            *entryIndexPtr = 0;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E:
            *entryIndexPtr = 1;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E:
            *entryIndexPtr = 2;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E:
            *entryIndexPtr = 3;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E:
            *entryIndexPtr = 4;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E:
            *entryIndexPtr = 5;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE_E:
            *entryIndexPtr = 6;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E:
            *entryIndexPtr = 7;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E:
            *entryIndexPtr = 8;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E:
            *entryIndexPtr = 9;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E:
            *entryIndexPtr = 10;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E:
            *entryIndexPtr = 11;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* default assignment */
    *internalIndexPtr = udbIndex;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) != GT_FALSE)
    {
        if (udbIndex > 42)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if (udbIndex > 22)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* lion3PclUDBEntryIndexGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte Entry index.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;.
*
* INPUTS:
*       devNum       - device number
*       packetType   - packet Type
*
* OUTPUTS:
*       entryIndexPtr    - (pointer to) User Defined Byte Entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3PclUDBEntryIndexGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    OUT GT_U32                               *entryIndexPtr
)
{
    devNum = devNum;

    if(packetType >= CPSS_DXCH_PCL_PACKET_TYPE_LAST_E)
    {
        /* check for access violation into the array of
            lion3ConvertSwPacketTypeToHwValueArr[] */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *entryIndexPtr = (GT_U32)(lion3ConvertSwPacketTypeToHwValueArr[packetType]);

    if (*entryIndexPtr == LION3_HW_PACKET_TYPE_INVALID_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN CPSS_PCL_DIRECTION_ENT               direction,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    return cpssDxChPclPortGroupUserDefinedByteSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, packetType, direction, udbIndex,
        offsetType, offset);

}

/*******************************************************************************
* cpssDxChPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN CPSS_PCL_DIRECTION_ENT               direction,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclUserDefinedByteSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleFormat, packetType, direction, udbIndex, offsetType, offset));

    rc = internal_cpssDxChPclUserDefinedByteSet(devNum, ruleFormat, packetType, direction, udbIndex, offsetType, offset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleFormat, packetType, direction, udbIndex, offsetType, offset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_DIRECTION_ENT               direction,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
{
    return cpssDxChPclPortGroupUserDefinedByteGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, packetType, direction, udbIndex,
        offsetTypePtr, offsetPtr);
}

/*******************************************************************************
* cpssDxChPclUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_DIRECTION_ENT               direction,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclUserDefinedByteGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleFormat, packetType, direction, udbIndex, offsetTypePtr, offsetPtr));

    rc = internal_cpssDxChPclUserDefinedByteGet(devNum, ruleFormat, packetType, direction, udbIndex, offsetTypePtr, offsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleFormat, packetType, direction, udbIndex, offsetTypePtr, offsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
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
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has up to 3.5K (3584) rows.
*                          See datasheet of particular device for TCAM size.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                            Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
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
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The pattern and mask input structures should be set to zero
*      by an application before assignment of fields.
*      The function places all muxed fields to rule patern and mask
*      by using bitwise OR operation.
*      The function does not check validity of muxed (overlapped) fields.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleSet
(
    IN GT_U8                                            devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN GT_U32                                           ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC                         *actionPtr
)
{
    return cpssDxChPclPortGroupRuleSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, ruleIndex, ruleOptionsBmp,
        maskPtr, patternPtr, actionPtr);
}

/*******************************************************************************
* cpssDxChPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
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
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has up to 3.5K (3584) rows.
*                          See datasheet of particular device for TCAM size.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                            Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
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
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The pattern and mask input structures should be set to zero
*      by an application before assignment of fields.
*      The function places all muxed fields to rule patern and mask
*      by using bitwise OR operation.
*      The function does not check validity of muxed (overlapped) fields.
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleSet
(
    IN GT_U8                                            devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN GT_U32                                           ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC                         *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleSet);

    CPSS_API_LOCK_MAC(0,0);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleFormat, ruleIndex, ruleOptionsBmp, maskPtr, patternPtr, actionPtr));

    rc = internal_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, ruleOptionsBmp, maskPtr, patternPtr, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleFormat, ruleIndex, ruleOptionsBmp, maskPtr, patternPtr, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleParsedGet
*
* DESCRIPTION:
*   The function gets the Policy Rule valid status, Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*
* OUTPUTS:
*       isRuleValidPtr   - GT_TRUE - rule is valid, GT_FALSE - rule is invalid
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
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - when size is trash or not related to ruleFormat.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleParsedGet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN  GT_U32                                           ruleIndex,
    IN  CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    OUT GT_BOOL                                          *isRuleValidPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *patternPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC                         *actionPtr
)
{
    return cpssDxChPclPortGroupRuleParsedGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, ruleIndex, ruleOptionsBmp,
        isRuleValidPtr, maskPtr, patternPtr, actionPtr);
}

/*******************************************************************************
* cpssDxChPclRuleParsedGet
*
* DESCRIPTION:
*   The function gets the Policy Rule valid status, Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*
* OUTPUTS:
*       isRuleValidPtr   - GT_TRUE - rule is valid, GT_FALSE - rule is invalid
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
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - when size is trash or not related to ruleFormat.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleParsedGet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN  GT_U32                                           ruleIndex,
    IN  CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    OUT GT_BOOL                                          *isRuleValidPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *patternPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC                         *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleParsedGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleFormat, ruleIndex, ruleOptionsBmp, isRuleValidPtr, maskPtr, patternPtr, actionPtr));

    rc = internal_cpssDxChPclRuleParsedGet(devNum, ruleFormat, ruleIndex, ruleOptionsBmp, isRuleValidPtr, maskPtr, patternPtr, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleFormat, ruleIndex, ruleOptionsBmp, isRuleValidPtr, maskPtr, patternPtr, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
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
static GT_STATUS internal_cpssDxChPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    return cpssDxChPclPortGroupRuleActionUpdate(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex, actionPtr);
}

/*******************************************************************************
* cpssDxChPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
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
GT_STATUS cpssDxChPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleActionUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleSize, ruleIndex, actionPtr));

    rc = internal_cpssDxChPclRuleActionUpdate(devNum, ruleSize, ruleIndex, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleSize, ruleIndex, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN CPSS_PCL_DIRECTION_ENT              direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    return cpssDxChPclPortGroupRuleActionGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex, direction, actionPtr);
}

/*******************************************************************************
* cpssDxChPclRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN CPSS_PCL_DIRECTION_ENT              direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleActionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleSize, ruleIndex, direction, actionPtr));

    rc = internal_cpssDxChPclRuleActionGet(devNum, ruleSize, ruleIndex, direction, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleSize, ruleIndex, direction, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
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
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleInvalidate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
)
{
    return cpssDxChPclPortGroupRuleInvalidate(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex);
}

/*******************************************************************************
* cpssDxChPclRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
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
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleInvalidate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleSize, ruleIndex));

    rc = internal_cpssDxChPclRuleInvalidate(devNum, ruleSize, ruleIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleSize, ruleIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
{
    return cpssDxChPclPortGroupRuleValidStatusSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex, valid);
}

/*******************************************************************************
* cpssDxChPclRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleValidStatusSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleSize, ruleIndex, valid));

    rc = internal_cpssDxChPclRuleValidStatusSet(devNum, ruleSize, ruleIndex, valid);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleSize, ruleIndex, valid));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclRuleCopy should be
*  called. And after this cpssDxChPclRuleInvalidate or
*  cpssDxChPclRuleValidStatusSet should be used
*  to invalidate Rule in old position.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of Rule.
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
static GT_STATUS internal_cpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
{
    return cpssDxChPclPortGroupRuleCopy(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleSrcIndex, ruleDstIndex);
}

/*******************************************************************************
* cpssDxChPclRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclRuleCopy should be
*  called. And after this cpssDxChPclRuleInvalidate or
*  cpssDxChPclRuleValidStatusSet should be used
*  to invalidate Rule in old position.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of Rule.
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
GT_STATUS cpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleCopy);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleSize, ruleSrcIndex, ruleDstIndex));

    rc = internal_cpssDxChPclRuleCopy(devNum, ruleSize, ruleSrcIndex, ruleDstIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleSize, ruleSrcIndex, ruleDstIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleMatchCounterGet
*
* DESCRIPTION:
*       Get rule matching counters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       counterPtr   - pointer to the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleMatchCounterGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    OUT GT_U32                        *counterPtr
)
{
    GT_U32    regAddr;       /* register address                    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    if(counterIndex > 31)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ruleMatchCounersBase
        + (counterIndex * 4);

    return prvCpssPortGroupsCounterSummary(devNum, regAddr, 0,32,counterPtr,NULL);
}

/*******************************************************************************
* cpssDxChPclRuleMatchCounterGet
*
* DESCRIPTION:
*       Get rule matching counters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       counterPtr   - pointer to the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleMatchCounterGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    OUT GT_U32                        *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleMatchCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counterIndex, counterPtr));

    rc = internal_cpssDxChPclRuleMatchCounterGet(devNum, counterIndex, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counterIndex, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleMatchCounterSet
*
* DESCRIPTION:
*       Set rule matching counters.
*       To reset value of counter use counterValue = 0
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (APPLICABLE RANGES: 0..31)
*       counterValue - counter value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleMatchCounterSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    IN  GT_U32                        counterValue
)
{
    GT_U32    regAddr;       /* register address                    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    if(counterIndex > 31)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ruleMatchCounersBase
        + (counterIndex * 4);

    return prvCpssPortGroupsCounterSet(devNum, regAddr,0,32, counterValue);
}

/*******************************************************************************
* cpssDxChPclRuleMatchCounterSet
*
* DESCRIPTION:
*       Set rule matching counters.
*       To reset value of counter use counterValue = 0
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (APPLICABLE RANGES: 0..31)
*       counterValue - counter value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleMatchCounterSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    IN  GT_U32                        counterValue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleMatchCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counterIndex, counterValue));

    rc = internal_cpssDxChPclRuleMatchCounterSet(devNum, counterIndex, counterValue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counterIndex, counterValue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*    enable          - GT_TRUE - Ingress Policy enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortIngressPolicyEnable
(
    IN GT_U8    devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 bitNum; /* number of "PCL enable" bit in VLAN QoS entry */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
        /* write value to post-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                             LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_POLICY_EN_E, /* field name */
                                             PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                             ((enable == GT_FALSE) ? 0 : 1));
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat & xCat3 devices */
            bitNum = 10;
        }
        else
        {
            /* default for DxCh - DxCh3 devices */
            bitNum = 17;
        }

        rc = prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                           portNum,
                                           0 /*fieldWordNum*/,
                                           bitNum /*fieldOffset*/,
                                           1 /*fieldLength*/,
                                           ((enable == GT_FALSE) ? 0 : 1) /*fieldValue*/);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*    enable          - GT_TRUE - Ingress Policy enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnable
(
    IN GT_U8    devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortIngressPolicyEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPclPortIngressPolicyEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get the Enable/Disable ingress policy status per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*
* OUTPUTS:
*    enablePtr       - Pointer to ingress policy status.
*                      GT_TRUE - Ingress Policy is enabled.
*                      GT_FALSE - Ingress Policy is disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortIngressPolicyEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 bitNum; /* number of "PCL enable" bit in VLAN QoS entry */
    GT_U32 value;  /* register field value */
    GT_STATUS rc;         /* return code                                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
        /* read value from post-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_POLICY_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat & xCat3 devices */
            bitNum = 10;
        }
        else
        {
            /* default for DxCh - DxCh3 devices */
            bitNum = 17;
        }
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            portNum,
        0 /*fieldWordNum*/,
        bitNum /*fieldOffset*/,
        1 /*fieldLength*/,
        &value /*fieldValue*/);
    }
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get the Enable/Disable ingress policy status per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*
* OUTPUTS:
*    enablePtr       - Pointer to ingress policy status.
*                      GT_TRUE - Ingress Policy is enabled.
*                      GT_FALSE - Ingress Policy is disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortIngressPolicyEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPclPortIngressPolicyEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortLookupCfgTabAccessModeSet
*
* DESCRIPTION:
*    Configures VLAN/PORT/ePort access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number.
*                      In eArch devices
*                      - for each ingress lookup portNum is default ePort.
*                      - for egress lookup portNum is physical port.
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  xCat and above devices.
*                      Ignored for other devices.
*                      xCat and above devices supports two sub lookups only for
*                      ingress lookup CPSS_PCL_LOOKUP_0_E , (APPLICABLE RANGES: 0..1).
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
*    mode            - PCL Configuration Table access mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - on bad entryIndex
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - since dxCh1 supports Ingress direction only, this API set the same access
*     type for both lookups regardless <lookupNum> parameter value for dxCh1.
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                          devNum,
    IN GT_PORT_NUM                                    portNum,
    IN CPSS_PCL_DIRECTION_ENT                         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN GT_U32                                         subLookupNum,
    IN CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
)
{
    GT_U32    rc;
    GT_U32    regAddr;       /* register address                    */
    GT_BOOL   isNotCh1;      /* GT_TRUE - Not Cheetah1 device       */
    GT_BOOL   isXCat;        /* GT_TRUE - XCat or above             */
    GT_U32    bitPos;        /* bit position                        */
    GT_U32    portGroupId;        /* port group Id - support multi-port-groups device    */
    GT_U32     localPort;     /* local port - support multi-port-groups device */
    GT_U32    lookupMode;    /* Determines the index to the third lookup in the PCL Configuration table.
                                The index may be source port, VLAN ID or lower bits of ePort based on this field.*/


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E:
            lookupMode = 0;
            break;
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E:
            lookupMode = 1;
            break;
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_EPORT_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == 0)
            {
                /* only for Bobcat2; Caelum; Bobcat3 and above devices        */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            lookupMode = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch (direction)
        {
            case CPSS_PCL_DIRECTION_INGRESS_E:

                switch (lookupNum)
                {
                    case CPSS_PCL_LOOKUP_0_E:
                        if (subLookupNum > 1)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        if (subLookupNum == 0)
                        {
                            bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP0_PCL_CFG_MODE_E;
                        }
                        else
                        {
                            bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP1_PCL_CFG_MODE_E;
                        }
                        break;
                    case CPSS_PCL_LOOKUP_1_E:
                        if (subLookupNum > 0)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP2_PCL_CFG_MODE_E;
                        break;
                    case CPSS_PCL_LOOKUP_0_0_E:
                        if (subLookupNum > 0)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP0_PCL_CFG_MODE_E;
                        break;
                    case CPSS_PCL_LOOKUP_0_1_E:
                        if (subLookupNum > 0)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP1_PCL_CFG_MODE_E;
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

                /* write value to pre-tti-lookup-ingress-eport table */
                rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            bitPos, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            lookupMode);

                break;

            case CPSS_PCL_DIRECTION_EGRESS_E:
                PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

                /* write value to table */
                rc = prvCpssDxChWriteTableEntryField(devNum,
                    PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                    portNum,
                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                    LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_LOOKUP_CONFIGURATION_MODE_E, /* field name */
                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                    lookupMode);
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
        isNotCh1 =
            (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
              == CPSS_PP_FAMILY_CHEETAH_E)
             ? GT_FALSE : GT_TRUE;

        isXCat =
            (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            ? GT_TRUE : GT_FALSE;

        if (portNum == CPSS_CPU_PORT_NUM_CNS)
        {
            portNum = 63;
        }

        switch (direction)
        {
            case CPSS_PCL_DIRECTION_INGRESS_E:

                /* both lookups for cheetah+ */
                bitPos = 20;
                if (isNotCh1 != GT_FALSE)
                {
                    if (isXCat == GT_FALSE)
                    {
                        /* DxCh - DxCh3 devices*/
                        switch (lookupNum)
                        {
                            case CPSS_PCL_LOOKUP_0_E:
                                bitPos = 20;
                                break;
                            case CPSS_PCL_LOOKUP_1_E:
                                bitPos = 16;
                                break;
                            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                    }
                    else
                    {
                        /* xCat & xCat3 devices*/
                        switch (lookupNum)
                        {
                            case CPSS_PCL_LOOKUP_0_E:
                                if (subLookupNum > 1)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                }
                                bitPos = (subLookupNum == 0) ? 11 : 12;
                                break;
                            case CPSS_PCL_LOOKUP_1_E:
                                if (subLookupNum > 0)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                }
                                bitPos = 13;
                                break;
                            case CPSS_PCL_LOOKUP_0_0_E:
                                if (subLookupNum > 0)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                }
                                bitPos = 11;
                                break;
                            case CPSS_PCL_LOOKUP_0_1_E:
                                if (subLookupNum > 0)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                }
                                bitPos = 12;
                                break;
                            default:
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                    }
                }

                rc = prvCpssDxChWriteTableEntryField(devNum,
                                                       PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                                       portNum,
                                                       0 /*fieldWordNum*/,
                                                       bitPos,
                                                       1 /*fieldLength*/,
                                                       lookupMode);

                break;
            case CPSS_PCL_DIRECTION_EGRESS_E:

                if (isNotCh1 == GT_FALSE)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                if ((isXCat != GT_FALSE) && (subLookupNum > 0))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
                    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

                /* get the start bit 0..63 (in the bmp of registers) */
                bitPos = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                         PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                         localPort;

                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                        epclCfgTblAccess[OFFSET_TO_WORD_MAC(bitPos)];

                    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                                                              portGroupId,
                                                              regAddr,
                                                              OFFSET_TO_BIT_MAC(bitPos),
                                                              1,
                                                          lookupMode);
                    break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeSet
*
* DESCRIPTION:
*    Configures VLAN/PORT/ePort access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number.
*                      In eArch devices
*                      - for each ingress lookup portNum is default ePort.
*                      - for egress lookup portNum is physical port.
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  xCat and above devices.
*                      Ignored for other devices.
*                      xCat and above devices supports two sub lookups only for
*                      ingress lookup CPSS_PCL_LOOKUP_0_E , (APPLICABLE RANGES: 0..1).
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
*    mode            - PCL Configuration Table access mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - on bad entryIndex
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - since dxCh1 supports Ingress direction only, this API set the same access
*     type for both lookups regardless <lookupNum> parameter value for dxCh1.
*******************************************************************************/
GT_STATUS cpssDxChPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                          devNum,
    IN GT_PORT_NUM                                    portNum,
    IN CPSS_PCL_DIRECTION_ENT                         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN GT_U32                                         subLookupNum,
    IN CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortLookupCfgTabAccessModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, lookupNum, subLookupNum, mode));

    rc = internal_cpssDxChPclPortLookupCfgTabAccessModeSet(devNum, portNum, direction, lookupNum, subLookupNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, lookupNum, subLookupNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortLookupCfgTabAccessModeGet
*
* DESCRIPTION:
*    Gets VLAN/PORT/ePort access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*                      In eArch devices
*                      - for each ingress lookup portNum is default ePort.
*                      - for egress lookup portNum is physical port.
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  xCat and above devices.
*                      Ignored for other devices.
*                      xCat and above devices supports two sub lookups only for
*                      ingress lookup CPSS_PCL_LOOKUP_0_E , (APPLICABLE RANGES: 0..1).
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
* OUTPUTS:
*    modePtr         - (pointer to)PCL Configuration Table access mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only and set same access type
*     for both lookups regardless the <lookupNum> parameter value
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortLookupCfgTabAccessModeGet
(
    IN  GT_U8                                          devNum,
    IN  GT_PORT_NUM                                    portNum,
    IN  CPSS_PCL_DIRECTION_ENT                         direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN  GT_U32                                         subLookupNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT *modePtr
)
{
    GT_U32    rc;
    GT_U32    regAddr;       /* register address                    */
    GT_BOOL   isNotCh1;      /* GT_TRUE - Not Cheetah1 device       */
    GT_BOOL   isXCat;        /* GT_TRUE - XCat or above             */
    GT_U32    bitPos;        /* bit position                        */
    GT_U32    portGroupId;   /* port group Id - support multi-port-groups device    */
    GT_U32     localPort;     /* local port - support multi-port-groups device */
    GT_U32    lookupMode;    /* Determines the index to the third lookup in the PCL Configuration table.
                                The index may be source port, VLAN ID or lower bits of ePort based on this field.*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch (direction)
        {
            case CPSS_PCL_DIRECTION_INGRESS_E:

                switch (lookupNum)
                {
                    case CPSS_PCL_LOOKUP_0_E:
                        if (subLookupNum > 1)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        if (subLookupNum == 0)
                        {
                            bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP0_PCL_CFG_MODE_E;
                        }
                        else
                        {
                            bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP1_PCL_CFG_MODE_E;
                        }
                        break;
                    case CPSS_PCL_LOOKUP_1_E:
                        if (subLookupNum > 0)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP2_PCL_CFG_MODE_E;
                        break;
                    case CPSS_PCL_LOOKUP_0_0_E:
                        if (subLookupNum > 0)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP0_PCL_CFG_MODE_E;
                        break;
                    case CPSS_PCL_LOOKUP_0_1_E:
                        if (subLookupNum > 0)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                        bitPos = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP1_PCL_CFG_MODE_E;
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

                /* write value to pre-tti-lookup-ingress-eport table */
                rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            bitPos, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &lookupMode);
                break;

            case CPSS_PCL_DIRECTION_EGRESS_E:
                PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

                /* read value from table */
                rc = prvCpssDxChReadTableEntryField(devNum,
                    PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                    portNum,
                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                    LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_LOOKUP_CONFIGURATION_MODE_E, /* field name */
                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                    &lookupMode);
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /*  E_ARCH Disabled */
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
        isNotCh1 =
            (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
              == CPSS_PP_FAMILY_CHEETAH_E)
             ? GT_FALSE : GT_TRUE;

        isXCat =
            (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            ? GT_TRUE : GT_FALSE;

        if (portNum == CPSS_CPU_PORT_NUM_CNS)
        {
            portNum = 63;
        }

        switch (direction)
        {
            case CPSS_PCL_DIRECTION_INGRESS_E:

                /* both lookups for cheetah+ */
                bitPos = 20;
                if (isNotCh1 != GT_FALSE)
                {
                    if (isXCat == GT_FALSE)
                    {
                        /* DxCh - DxCh3 devices*/
                        switch (lookupNum)
                        {
                            case CPSS_PCL_LOOKUP_0_E:
                                bitPos = 20;
                                break;
                            case CPSS_PCL_LOOKUP_1_E:
                                bitPos = 16;
                                break;
                            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                    }
                    else
                    {
                        /* xCat & xCat3 devices*/
                        switch (lookupNum)
                        {
                            case CPSS_PCL_LOOKUP_0_E:
                                if (subLookupNum > 1)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                }
                                bitPos = (subLookupNum == 0) ? 11 : 12;
                                break;
                            case CPSS_PCL_LOOKUP_1_E:
                                if (subLookupNum > 0)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                }
                                bitPos = 13;
                                break;
                            case CPSS_PCL_LOOKUP_0_0_E:
                                if (subLookupNum > 0)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                }
                                bitPos = 11;
                                break;
                            case CPSS_PCL_LOOKUP_0_1_E:
                                if (subLookupNum > 0)
                                {
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                }
                                bitPos = 12;
                                break;
                                default:
                                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                    }
                }

                rc = prvCpssDxChReadTableEntryField(
                    devNum,
                    PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                    portNum,
                    0 /*fieldWordNum*/,
                    bitPos,
                    1 /*fieldLength*/,
                    &lookupMode);
                break;

            case CPSS_PCL_DIRECTION_EGRESS_E:

                if (isNotCh1 == GT_FALSE)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                if ((isXCat != GT_FALSE) && (subLookupNum > 0))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

                /* get the start bit 0..63 (in the bmp of registers) */
                bitPos = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                         PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                         localPort;

                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                        epclCfgTblAccess[OFFSET_TO_WORD_MAC(bitPos)];

                rc = prvCpssDrvHwPpPortGroupGetRegField(
                    devNum,
                    portGroupId,
                    regAddr,
                    OFFSET_TO_BIT_MAC(bitPos),
                    1,
                    &lookupMode);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if (rc != 0)
    {
        return rc;
    }

    if(lookupMode <= 2)
    {
        *modePtr = (lookupMode == 0) ? CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E :
                   (lookupMode == 1) ? CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E :
                 /*(lookupMode == 2)*/ CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_EPORT_E;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeGet
*
* DESCRIPTION:
*    Gets VLAN/PORT/ePort access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*                      In eArch devices
*                      - for each ingress lookup portNum is default ePort.
*                      - for egress lookup portNum is physical port.
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  xCat and above devices.
*                      Ignored for other devices.
*                      xCat and above devices supports two sub lookups only for
*                      ingress lookup CPSS_PCL_LOOKUP_0_E , (APPLICABLE RANGES: 0..1).
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
* OUTPUTS:
*    modePtr         - (pointer to)PCL Configuration Table access mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only and set same access type
*     for both lookups regardless the <lookupNum> parameter value
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortLookupCfgTabAccessModeGet
(
    IN  GT_U8                                          devNum,
    IN  GT_PORT_NUM                                    portNum,
    IN  CPSS_PCL_DIRECTION_ENT                         direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN  GT_U32                                         subLookupNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortLookupCfgTabAccessModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, lookupNum, subLookupNum, modePtr));

    rc = internal_cpssDxChPclPortLookupCfgTabAccessModeGet(devNum, portNum, direction, lookupNum, subLookupNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, lookupNum, subLookupNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclCfgTblAccessModeSet
*
* DESCRIPTION:
*    Configures global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    accModePtr      - global configuration of access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only with 32 ports per device and the
*     non-local-device-entries-segment-base == 0
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclCfgTblAccessModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    hwMask;        /* HW mask                             */
    GT_STATUS res;           /* return code                         */
    GT_U32    swData;        /* work                                */
    GT_U32    nonLocalBitPos;/* positon of nonLocal bit             */
    GT_U32    EPCLPortInterfacePCLBlockBit;/* EPCL : bit index of field <EPCL Port Interface PCL Block>*/
    GT_U32    EPCLMaxPortPerDevBit;/* EPCL : bit index of field <EPCL Max Port Per Dev>*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(accModePtr);

    /* ingress PCL */

    hwValue = hwMask = 0;

    /* LOCAL / NON_LOCAL ACCESS */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah+ - for Cheeta2 another values */
        hwMask |= (1 << 1);
        switch (accModePtr->ipclAccMode)
        {
            case CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E:
                hwValue |= (1 << 1);
                break;
            case CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E:
                /* hwValue already 0*/
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* all other configurations not relevant for Cheetah+ */
        return prvCpssHwPpWriteRegBitMask(
            devNum, regAddr, hwMask, hwValue);
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        nonLocalBitPos = 2;
    }
    else
    {
        nonLocalBitPos = 1;
    }

    hwMask |= (1 << nonLocalBitPos);

    switch (accModePtr->ipclAccMode)
    {
        case CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E:
            /* hwValue already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E:
            hwValue |= (1 << nonLocalBitPos);
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* max ports per device for non-local access */
    /* base of segment entries for non-local access */

    swData = 0;

    switch (accModePtr->ipclMaxDevPorts)
    {
        case CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E:
            /* swData already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E:
            swData |= (1 << 2);
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (accModePtr->ipclDevPortBase)
    {
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E:
            /* swData already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E:
            swData |= 1;
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E:
            swData |= 2;
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E:
            swData |= 3;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        hwMask |= (1 << 7) | (3 << 3);
        switch (swData)
        {
            case 0:    /* 32 ports base 0 */
                /* hwValue bits 7, 4, 3 already 0 */
                break;
            case 1:    /* 32 ports base 1024 */
                hwValue |= (1 << 3);
                break;
            case 2:    /* 32 ports base 2048 */
                hwValue |= (2 << 3);
                break;
            case 3:    /* 32 ports base 3072 */
                hwValue |= (3 << 3);
                break;
            case 4:    /* 64 ports base 0 */
                hwValue |= (1 << 7);
                break;
            case 6:    /* 64 ports base 2048 */
                hwValue |= (1 << 7) | (1 << 3);
                break;
            /* all other parameter values are invalid */
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* DxCh2 and DxCh3 */
        hwMask |= (1 << 18) | (3 << 16);
        switch (swData)
        {
            case 0:    /* 32 ports base 0 */
                /* hwValue bits 18, 17, 16 already 0 */
                break;
            case 1:    /* 32 ports base 1024 */
                hwValue |= (1 << 16);
                break;
            case 2:    /* 32 ports base 2048 */
                hwValue |= (2 << 16);
                break;
            case 3:    /* 32 ports base 3072 */
                hwValue |= (3 << 16);
                break;
            case 4:    /* 64 ports base 0 */
                hwValue |= (1 << 18);
                break;
            case 6:    /* 64 ports base 2048 */
                hwValue |= (1 << 18) | (1 << 16);
                break;
            /* all other parameter values are invalid */
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }


    res = prvCpssHwPpWriteRegBitMask(
        devNum, regAddr, hwMask, hwValue);
    if (res != GT_OK)
    {
        return res;
    }

    /* egress PCL */

    hwValue = hwMask = 0;

    /* LOCAL / NON_LOCAL ACCESS */

    hwMask |= (1 << 1);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;

        EPCLPortInterfacePCLBlockBit = 6;
        EPCLMaxPortPerDevBit = 8;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

        EPCLPortInterfacePCLBlockBit = 16;
        EPCLMaxPortPerDevBit = 18;
    }

    switch (accModePtr->epclAccMode)
    {
        case CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E:
            /* hwValue already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E:
            hwValue |= (1 << 1);
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* max ports per device for non-local access */
    /* base of segment entries for non-local access */

    hwMask |= (1 << EPCLMaxPortPerDevBit) | (3 << EPCLPortInterfacePCLBlockBit);
    swData = 0;

    switch (accModePtr->epclMaxDevPorts)
    {
        case CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E:
            /* swData already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E:
            swData |= (1 << 2);
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (accModePtr->epclDevPortBase)
    {
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E:
            /* swData already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E:
            swData |= 1;
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E:
            swData |= 2;
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E:
            swData |= 3;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        switch (swData)
        {
            case 0:    /* 32 ports base 0 */
                /* hwValue bits 18, 17, 16 already 0 */
                break;
            case 2:    /* 32 ports base 2048 */
                hwValue |= (1 << EPCLPortInterfacePCLBlockBit);
                break;
            case 4:    /* 64 ports base 0 */
                hwValue |= (1 << EPCLMaxPortPerDevBit);
                break;
            case 6:    /* 64 ports base 2048 */
                hwValue |= (1 << EPCLMaxPortPerDevBit) | (1 << EPCLPortInterfacePCLBlockBit);
                break;
            /* Not supported */
            case 1:    /* 32 ports base 1024 */
            case 3:    /* 32 ports base 3072 */
            /* all other parameter values are invalid */
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* DxCh2 and DxCh3 */
        switch (swData)
        {
            case 0:    /* 32 ports base 0 */
                /* hwValue bits 18, 17, 16 already 0 */
                break;
            case 1:    /* 32 ports base 1024 */
                hwValue |= (1 << 16);
                break;
            case 2:    /* 32 ports base 2048 */
                hwValue |= (2 << 16);
                break;
            case 3:    /* 32 ports base 3072 */
                hwValue |= (3 << 16);
                break;
            case 4:    /* 64 ports base 0 */
                hwValue |= (1 << 18);
                break;
            case 6:    /* 64 ports base 2048 */
                hwValue |= (1 << 18) | (1 << 16);
                break;
            /* all other parameter values are invalid */
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }


    res = prvCpssHwPpWriteRegBitMask(
        devNum, regAddr, hwMask, hwValue);
    if (res != GT_OK)
    {
        return res;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeSet
*
* DESCRIPTION:
*    Configures global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    accModePtr      - global configuration of access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only with 32 ports per device and the
*     non-local-device-entries-segment-base == 0
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclCfgTblAccessModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, accModePtr));

    rc = internal_cpssDxChPclCfgTblAccessModeSet(devNum, accModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, accModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclCfgTblAccessModeGet
*
* DESCRIPTION:
*    Get global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    accModePtr      - global configuration of access mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    see cpssDxChPclCfgTblAccessModeSet
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclCfgTblAccessModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    hwData;        /* HW Data                             */
    GT_STATUS res;           /* return code                         */
    GT_U32    nonLocalBitPos;/* positon of nonLocal bit             */
    GT_U32    EPCLPortInterfacePCLBlockBit;/* EPCL : bit index of field <EPCL Port Interface PCL Block>*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(accModePtr);

    /* ingress PCL */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }

    res = prvCpssHwPpReadRegister(devNum, regAddr, &hwValue);
    if (res != GT_OK)
    {
        return res;
    }

    /* LOCAL / NON_LOCAL ACCESS */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah+ and Cheetah2 has different values */

        /* LOCAL / NON_LOCAL ACCESS */

            accModePtr->ipclAccMode =
            ((hwValue & (1 << 1)) == 0)
            ? CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E
            : CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;

        /* not relevant structure fields */
        accModePtr->ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
        accModePtr->ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
        accModePtr->epclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
        accModePtr->epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
        accModePtr->epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;

        return GT_OK;
    }

    /* LOCAL / NON_LOCAL ACCESS */

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        nonLocalBitPos = 2;
    }
    else
    {
        nonLocalBitPos = 1;
    }

    accModePtr->ipclAccMode =
        ((hwValue & (1 << nonLocalBitPos)) == 0)
        ? CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E
        : CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E;

    /* max ports per device for non-local access */
    /* base of segment entries for non-local access */

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        /* bits 7 and 4:3 */
        hwData = (((hwValue >> 3) & 3) | ((hwValue >> 5) & 4));
    }
    else
    {
        /* DxCh2 and DxCh3 */
        hwData = ((hwValue >> 16) & 7);
    }

    switch (hwData)
    {
        case 0:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
            break;
        case 1:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E;
            break;
        case 2:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
            break;
        case 3:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E;
            break;
        case 4:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
            break;
        case 5:
        case 6:
        case 7:
            /* CASES 6 AND 7 NEVER CONFIGURED BY CPSS AND UNDEFINED */
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
            break;
        /* must never occure */
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }


    /* egress PCL */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;

        EPCLPortInterfacePCLBlockBit = 6;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

        EPCLPortInterfacePCLBlockBit = 16;
    }

    res = prvCpssHwPpReadRegister(devNum, regAddr, &hwValue);
    if (res != GT_OK)
    {
        return res;
    }

    /* LOCAL / NON_LOCAL ACCESS */

    accModePtr->epclAccMode =
        ((hwValue & (1 << 1)) == 0)
        ? CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E
        : CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E;

    /* max ports per device for non-local access */
    /* base of segment entries for non-local access */

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        switch ((hwValue >> EPCLPortInterfacePCLBlockBit) & 7)
        {
            case 0:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
                break;
            case 1:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
                break;
            case 4:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
                break;
            case 5:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* DxCh2 And DxCh3 */
        switch ((hwValue >> 16) & 7)
        {
            case 0:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
                break;
            case 1:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E;
                break;
            case 2:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
                break;
            case 3:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E;
                break;
            case 4:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
                break;
            case 5:
            case 6:
            case 7:
                /* CASES 6 AND 7 NEVER CONFIGURED BY CPSS AND UNDEFINED */
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
                break;
            /* must never occure */
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeGet
*
* DESCRIPTION:
*    Get global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    accModePtr      - global configuration of access mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    see cpssDxChPclCfgTblAccessModeSet
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclCfgTblAccessModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, accModePtr));

    rc = internal_cpssDxChPclCfgTblAccessModeGet(devNum, accModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, accModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclCfgTblSet
(
    IN GT_U8                           devNum,
    IN CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    return cpssDxChPclPortGroupCfgTblSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        interfaceInfoPtr, direction, lookupNum, lookupCfgPtr);
}

/*******************************************************************************
* cpssDxChPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblSet
(
    IN GT_U8                           devNum,
    IN CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclCfgTblSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr));

    rc = internal_cpssDxChPclCfgTblSet(devNum, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    return cpssDxChPclPortGroupCfgTblGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        interfaceInfoPtr, direction, lookupNum, lookupCfgPtr);
}

/*******************************************************************************
* cpssDxChPclCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclCfgTblGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr));

    rc = internal_cpssDxChPclCfgTblGet(devNum, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable ingress policy
*                      GT_TRUE  - enable,
*                      GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclIngressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitPos;        /* the position of bit                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* XCAT and above */

        bitPos = 1;

        /* 1 - enable, 0 - disable */
        hwValue = (enable == GT_FALSE) ? 0 : 1;
    }
    else
    {
        /* CH1-3 */

        bitPos = 7;

        /* 0 - enable, 1 - disable */
        hwValue = (enable == GT_FALSE) ? 1 : 0;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, bitPos, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable ingress policy
*                      GT_TRUE  - enable,
*                      GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIngressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclIngressPolicyEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPclIngressPolicyEnable(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get status (Enable or Disable) of Ingress Policy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    enablePtr        - (pointer to) enable Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclIngressPolicyEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitPos;        /* the position of bit                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }

    /* BIT 1 for xCat and above; BIT 7 for DxCh1-3 */
    bitPos = (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)) ? 1 : 7;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitPos, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 1 - enable, 0 - disable for xCat and above ; 0 - enable, 1 - disable for DxCh1-3 */
    *enablePtr = (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)) ?
                  ((hwValue == 0) ? GT_FALSE : GT_TRUE) :
                  ((hwValue == 1) ? GT_FALSE : GT_TRUE);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get status (Enable or Disable) of Ingress Policy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    enablePtr        - (pointer to) enable Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIngressPolicyEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclIngressPolicyEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPclIngressPolicyEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclTcamRuleSizeModeSet
*
* DESCRIPTION:
*    Set TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum               - device number
*    mixedRuleSizeMode    - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSize             - Rule size for not mixed mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclTcamRuleSizeModeSet
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     mixedRuleSizeMode,
    IN CPSS_PCL_RULE_SIZE_ENT      ruleSize
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }

    hwValue = ((mixedRuleSizeMode == GT_FALSE) ? 0 : (1 << 8));

    switch (ruleSize)
    {
        case CPSS_PCL_RULE_SIZE_STD_E: /*bit0 is 0*/ break;
        case CPSS_PCL_RULE_SIZE_EXT_E: hwValue |= 1; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpWriteRegBitMask(
        devNum, regAddr, ((1 << 8) | 1) /*mask*/, hwValue);
}

/*******************************************************************************
* cpssDxChPclTcamRuleSizeModeSet
*
* DESCRIPTION:
*    Set TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum               - device number
*    mixedRuleSizeMode    - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSize             - Rule size for not mixed mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamRuleSizeModeSet
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     mixedRuleSizeMode,
    IN CPSS_PCL_RULE_SIZE_ENT      ruleSize
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclTcamRuleSizeModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mixedRuleSizeMode, ruleSize));

    rc = internal_cpssDxChPclTcamRuleSizeModeSet(devNum, mixedRuleSizeMode, ruleSize);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mixedRuleSizeMode, ruleSize));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclTcamRuleSizeModeGet
*
* DESCRIPTION:
*    Get TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum               - device number
*
* OUTPUTS:
*    mixedRuleSizeModePtr - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSizePtr          - Rule size for not mixed mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclTcamRuleSizeModeGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *mixedRuleSizeModePtr,
    OUT CPSS_PCL_RULE_SIZE_ENT      *ruleSizePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(mixedRuleSizeModePtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleSizePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;

    rc = prvCpssHwPpReadRegBitMask(
                        devNum, regAddr, ((1 << 8) | 1) /*mask*/, &hwValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    value = (hwValue >> 8) & 0x1;

    *mixedRuleSizeModePtr = (0 == value) ? GT_FALSE : GT_TRUE;

    value = hwValue & 0x1;

    switch (value)
    {
        case 0:
            *ruleSizePtr = CPSS_PCL_RULE_SIZE_STD_E;
            break;
        case 1:
            *ruleSizePtr = CPSS_PCL_RULE_SIZE_EXT_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPclTcamRuleSizeModeGet
*
* DESCRIPTION:
*    Get TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum               - device number
*
* OUTPUTS:
*    mixedRuleSizeModePtr - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSizePtr          - Rule size for not mixed mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamRuleSizeModeGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *mixedRuleSizeModePtr,
    OUT CPSS_PCL_RULE_SIZE_ENT      *ruleSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclTcamRuleSizeModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mixedRuleSizeModePtr, ruleSizePtr));

    rc = internal_cpssDxChPclTcamRuleSizeModeGet(devNum, mixedRuleSizeModePtr, ruleSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mixedRuleSizeModePtr, ruleSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclUdeEtherTypeRegAddeGet
*
* DESCRIPTION:
*       This function gets UDE Ethertype register address
*       and bit offset of this subfield in register.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index
*                       The valid range documented in Functional Specification.
*                       (APPLICABLE RANGES: Lion, xCat2, Lion2 0..4; Bobcat2, Caelum, Bobcat3 0..6)
*
* OUTPUTS:
*       regAddrPtr    - (pointer to) address of register.
*       bitOffsetPtr  - (pointer to) bit offset in register.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclUdeEtherTypeRegAddeGet
(
    IN   GT_U8           devNum,
    IN   GT_U32          index,
    OUT  GT_U32          *regAddrPtr,
    OUT  GT_U32          *bitOffsetPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (index >= 8)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        *regAddrPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PCLUserDefinedEtherTypes[index / 2];

        *bitOffsetPtr = (index & 1) ? 16 : 0;

        return GT_OK;
    }

    if (index == 0)
    {
        /* UDE for index == 0 */
        *regAddrPtr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.specialEthertypes;

        *bitOffsetPtr = 0;

        return GT_OK;
    }

    if (index < 5)
    {
        /* UDE for index == 1..4                      */
        /* the case of 0 already treated upper        */

        /* for index 1,2 - word0, for index 3,4 - word1 */
        *regAddrPtr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.udeEthertypesBase
            + (4 * ((index - 1) / 2));

        /* for index 1,3 - 0, for index 2,4 - 16 */
        *bitOffsetPtr = ((index - 1) & 1) ? 16 : 0;

        return GT_OK;
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* internal_cpssDxChPclUdeEtherTypeSet
*
* DESCRIPTION:
*       This function sets UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index,
*                       (APPLICABLE RANGES: Lion, xCat2, Lion2 0..4; Bobcat2, Caelum, Bobcat3 0..6)
*       ethType       - Ethertype value (APPLICABLE RANGES:  0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - on out of range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclUdeEtherTypeSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          ethType
)
{
    GT_STATUS   rc;           /* return code      */
    GT_U32      regAddr;      /* register address */
    GT_U32      bitOffset;    /* bit offset       */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);


    if (ethType >= BIT_16)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    /* check devNum and index validity */
    /* calculate regAddr and bitOffset */
    rc = prvCpssDxChPclUdeEtherTypeRegAddeGet(
        devNum, index, &regAddr, &bitOffset);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write value */
    return prvCpssHwPpSetRegField(
        devNum, regAddr, bitOffset, 16, ethType);
}

/*******************************************************************************
* cpssDxChPclUdeEtherTypeSet
*
* DESCRIPTION:
*       This function sets UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index,
*                       (APPLICABLE RANGES: Lion, xCat2, Lion2 0..4; Bobcat2, Caelum, Bobcat3 0..6)
*       ethType       - Ethertype value (APPLICABLE RANGES:  0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - on out of range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUdeEtherTypeSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          ethType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclUdeEtherTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, ethType));

    rc = internal_cpssDxChPclUdeEtherTypeSet(devNum, index, ethType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, ethType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclUdeEtherTypeGet
*
* DESCRIPTION:
*       This function gets the UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index,
*                       (APPLICABLE RANGES: Lion, xCat2, Lion2 0..4; Bobcat2, Caelum, Bobcat3 0..6)
*
* OUTPUTS:
*       ethTypePtr    - points to Ethertype value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclUdeEtherTypeGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *ethTypePtr
)
{
    GT_STATUS   rc;           /* return code      */
    GT_U32      regAddr;      /* register address */
    GT_U32      bitOffset;    /* bit offset       */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(ethTypePtr);

    /* check devNum and index validity */
    /* calculate regAddr and bitOffset */
    rc = prvCpssDxChPclUdeEtherTypeRegAddeGet(
        devNum, index, &regAddr, &bitOffset);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssHwPpGetRegField(
        devNum, regAddr, bitOffset, 16, ethTypePtr);
}

/*******************************************************************************
* cpssDxChPclUdeEtherTypeGet
*
* DESCRIPTION:
*       This function gets the UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index,
*                       (APPLICABLE RANGES: Lion, xCat2, Lion2 0..4; Bobcat2, Caelum, Bobcat3 0..6)
*
* OUTPUTS:
*       ethTypePtr    - points to Ethertype value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUdeEtherTypeGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *ethTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclUdeEtherTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, ethTypePtr));

    rc = internal_cpssDxChPclUdeEtherTypeGet(devNum, index, ethTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, ethTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclTwoLookupsCpuCodeResolution
*
* DESCRIPTION:
*    Resolve the result CPU Code if both lookups has action commands
*    are either both TRAP or both MIRROR To CPU
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum               - device number
*    lookupNum            - lookup number from which the CPU Code is selected
*                           when action commands are either both TRAP or
*                           both MIRROR To CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclTwoLookupsCpuCodeResolution
(
    IN GT_U8                       devNum,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
        bitNum = 6;
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 6;
    }
    else
    {
        /* DxCh1-3 devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 5;
    }

    switch (lookupNum)
    {
        case CPSS_PCL_LOOKUP_0_E: hwValue = 0; break;
        case CPSS_PCL_LOOKUP_1_E: hwValue = 1; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, bitNum, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclTwoLookupsCpuCodeResolution
*
* DESCRIPTION:
*    Resolve the result CPU Code if both lookups has action commands
*    are either both TRAP or both MIRROR To CPU
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum               - device number
*    lookupNum            - lookup number from which the CPU Code is selected
*                           when action commands are either both TRAP or
*                           both MIRROR To CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTwoLookupsCpuCodeResolution
(
    IN GT_U8                       devNum,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclTwoLookupsCpuCodeResolution);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookupNum));

    rc = internal_cpssDxChPclTwoLookupsCpuCodeResolution(devNum, lookupNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookupNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclInvalidUdbCmdSet
*
* DESCRIPTION:
*    Set the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    udbErrorCmd    - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclInvalidUdbCmdSet
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_UDB_ERROR_CMD_ENT  udbErrorCmd
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* Bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        bitNum = 8;
    }
    else
    {
        /* DxCh - DxCh3 */
        bitNum = 12;
    }

    switch (udbErrorCmd)
    {
        case CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E:      hwValue = 0; break;
        case CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E: hwValue = 1; break;
        case CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E:   hwValue = 3; break;
        case CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E:   hwValue = 2; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, bitNum, 2, hwValue);
}

/*******************************************************************************
* cpssDxChPclInvalidUdbCmdSet
*
* DESCRIPTION:
*    Set the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    udbErrorCmd    - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclInvalidUdbCmdSet
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_UDB_ERROR_CMD_ENT  udbErrorCmd
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclInvalidUdbCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, udbErrorCmd));

    rc = internal_cpssDxChPclInvalidUdbCmdSet(devNum, udbErrorCmd);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, udbErrorCmd));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclInvalidUdbCmdGet
*
* DESCRIPTION:
*    Get the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*
* OUTPUTS:
*    udbErrorCmdPtr - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclInvalidUdbCmdGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_UDB_ERROR_CMD_ENT  *udbErrorCmdPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* Bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(udbErrorCmdPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        bitNum = 8;
    }
    else
    {
        /* DxCh - DxCh3 */
        bitNum = 12;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitNum, 2, &hwValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    switch (hwValue)
    {
        case 0:
            *udbErrorCmdPtr = CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E;
            break;
        case 1:
            *udbErrorCmdPtr = CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            *udbErrorCmdPtr = CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E;
            break;
        case 2:
            *udbErrorCmdPtr = CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPclInvalidUdbCmdGet
*
* DESCRIPTION:
*    Get the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*
* OUTPUTS:
*    udbErrorCmdPtr - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclInvalidUdbCmdGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_UDB_ERROR_CMD_ENT  *udbErrorCmdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclInvalidUdbCmdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, udbErrorCmdPtr));

    rc = internal_cpssDxChPclInvalidUdbCmdGet(devNum, udbErrorCmdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, udbErrorCmdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclIpLengthCheckModeSet
*
* DESCRIPTION:
*    Set the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum           - device number
*    mode             - IP packet length checking mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclIpLengthCheckModeSet
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  mode
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* Lion and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 10;
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 9;
    }
    else
    {
        /* DxCh1-3 devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 15;
    }

    switch (mode)
    {
        case CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E:
            hwValue = 1;
            break;
        case CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E:
            hwValue = 0;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, bitNum, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclIpLengthCheckModeSet
*
* DESCRIPTION:
*    Set the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum           - device number
*    mode             - IP packet length checking mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpLengthCheckModeSet
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclIpLengthCheckModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChPclIpLengthCheckModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclIpLengthCheckModeGet
*
* DESCRIPTION:
*    Get the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    modePtr          - IP packet length checking mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclIpLengthCheckModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* Lion and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 10;
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 9;
    }
    else
    {
        /* DxCh1-3 devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 15;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitNum, 1, &hwValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    *modePtr = (0 == hwValue) ? CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E :
                                CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E;

    return rc;
}

/*******************************************************************************
* cpssDxChPclIpLengthCheckModeGet
*
* DESCRIPTION:
*    Get the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    modePtr          - IP packet length checking mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpLengthCheckModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclIpLengthCheckModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChPclIpLengthCheckModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}




/*******************************************************************************
* internal_cpssDxCh2PclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported enable parameter value
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxCh2PclEgressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitIndex;/*start bit*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
        bitIndex = 0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
        bitIndex = 7;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above  */
        /* 1 - enable, 0 - disable */
        hwValue = (enable == GT_FALSE) ? 0 : 1;

        if((GT_FALSE == enable) &&
           (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                      PRV_CPSS_DXCH_XCAT2_EPCL_GLOBAL_EN_NOT_FUNCTIONAL_WA_E)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

    }
    else
    {
        /* CH2, CH3 */
        /* 0 - enable, 1 - disable */
        hwValue = (enable != GT_FALSE) ? 0 : 1;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, bitIndex, 1, hwValue);
}

/*******************************************************************************
* cpssDxCh2PclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported enable parameter value
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclEgressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxCh2PclEgressPolicyEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxCh2PclEgressPolicyEnable(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxCh2PclEgressPolicyEnableGet
*
* DESCRIPTION:
*    Get status of Egress Policy.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    enablePtr        - status of Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL poinet
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxCh2PclEgressPolicyEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitIndex;      /*start bit                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
        bitIndex = 0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
        bitIndex = 7;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitIndex, 1, &hwValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above  */
        *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE;
    }
    else
    {
        /* CH2, CH3 */
        *enablePtr = (hwValue != 0) ? GT_FALSE : GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxCh2PclEgressPolicyEnableGet
*
* DESCRIPTION:
*    Get status of Egress Policy.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    enablePtr        - status of Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL poinet
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclEgressPolicyEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxCh2PclEgressPolicyEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxCh2PclEgressPolicyEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressForRemoteTunnelStartEnableSet
*
* DESCRIPTION:
*       Enable/Disable the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
*       enable         - enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressForRemoteTunnelStartEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitIndex;/*start bit*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
        bitIndex = 9;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
        bitIndex = 19;
    }

    /* 1 - enable, 0 - disable */
    hwValue = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssHwPpSetRegField(devNum, regAddr, bitIndex, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableSet
*
* DESCRIPTION:
*       Enable/Disable the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
*       enable         - enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressForRemoteTunnelStartEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPclEgressForRemoteTunnelStartEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressForRemoteTunnelStartEnableGet
*
* DESCRIPTION:
*       Gets Enable/Disable of the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressForRemoteTunnelStartEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitIndex;/*start bit*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
        bitIndex = 9;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
        bitIndex = 19;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitIndex, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 1 - enable, 0 - disable */
    *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableGet
*
* DESCRIPTION:
*       Gets Enable/Disable of the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressForRemoteTunnelStartEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPclEgressForRemoteTunnelStartEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
*
* DESCRIPTION:
*       Sets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       cfgTabAccMode  - PCL Configuration Table access mode
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - VLAN ID
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - Normal
*                        Normal means that access to PCL configuration table is done
*                        according to settings done by cpssDxChPclCfgTblAccessModeSet
*                        and cpssDxChPclPortLookupCfgTabAccessModeSet
*                        as non tunnel start packets.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   cfgTabAccMode
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitIndex;/*start bit*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
        bitIndex = 11;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
        bitIndex = 26;
    }

    switch (cfgTabAccMode)
    {
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E:
            hwValue = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    return prvCpssHwPpSetRegField(devNum, regAddr, bitIndex, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
*
* DESCRIPTION:
*       Sets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       cfgTabAccMode  - PCL Configuration Table access mode
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - VLAN ID
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - Normal
*                        Normal means that access to PCL configuration table is done
*                        according to settings done by cpssDxChPclCfgTblAccessModeSet
*                        and cpssDxChPclPortLookupCfgTabAccessModeSet
*                        as non tunnel start packets.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   cfgTabAccMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cfgTabAccMode));

    rc = internal_cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet(devNum, cfgTabAccMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cfgTabAccMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
*
* DESCRIPTION:
*       Gets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       cfgTabAccModePtr   - (pointer to) PCL Configuration Table access mode
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - VLAN ID
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - Normal
*                        Normal means that access to PCL configuration table is done
*                        according to settings done by cpssDxChPclCfgTblAccessModeSet
*                        and cpssDxChPclPortLookupCfgTabAccessModeSet
*                        as non tunnel start packets.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   *cfgTabAccModePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitIndex;/*start bit*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(cfgTabAccModePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
        bitIndex = 11;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
        bitIndex = 26;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitIndex, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *cfgTabAccModePtr = (hwValue == 0) ?
                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E :
                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
*
* DESCRIPTION:
*       Gets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       cfgTabAccModePtr   - (pointer to) PCL Configuration Table access mode
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - VLAN ID
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - Normal
*                        Normal means that access to PCL configuration table is done
*                        according to settings done by cpssDxChPclCfgTblAccessModeSet
*                        and cpssDxChPclPortLookupCfgTabAccessModeSet
*                        as non tunnel start packets.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   *cfgTabAccModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cfgTabAccModePtr));

    rc = internal_cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet(devNum, cfgTabAccModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cfgTabAccModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
{
    return cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, vidUpMode);
}

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressKeyFieldsVidUpModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vidUpMode));

    rc = internal_cpssDxChPclEgressKeyFieldsVidUpModeSet(devNum, vidUpMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vidUpMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
{
    return cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, vidUpModePtr);
}

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressKeyFieldsVidUpModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vidUpModePtr));

    rc = internal_cpssDxChPclEgressKeyFieldsVidUpModeGet(devNum, vidUpModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vidUpModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxCh2EgressPclPacketTypesSet
*
* DESCRIPTION:
*   Enables/disables Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*   enable        - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxCh2EgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              port,
    IN CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    IN GT_BOOL                           enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    GT_U32  bitPos;   /* position of a bit */
    PRV_CPSS_DXCH_TABLE_ENT tableType; /* table's type */
    GT_U32  fieldName; /* name of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                  CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    /* 1 - enable, 0 - disable */
    hwValue = (enable == GT_TRUE) ? 1 : 0;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, port);

        /* Bobcat2; Caelum; Bobcat3 */
        switch (pktType)
        {
            case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_FROM_CPU_CONTROL_PACKETS_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_FROM_CPU_DATA_PACKETS_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_EPCL_TO_CPU_PACKETS_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_EPCL_TO_ANALYZER_PACKETS_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_TS_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_TS_DATA_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_NON_TS_DATA_PORT_ENABLE_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        return prvCpssDxChWriteTableEntryField(
            devNum,
            tableType,
            port,
            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
            fieldName, /* field name */
            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
            hwValue);
    }

    /* DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2 */
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    /* get the start bit 0..63 (in the bmp of registers) */
    bitPos = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
             PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
             localPort;

    switch (pktType)
    {
        case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktFromCpuControl[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktFromCpuData[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktToCpu[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktToAnalyzer[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TS_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktTs[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktNonTs[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr,
            OFFSET_TO_BIT_MAC(bitPos), 1, hwValue);
}

/*******************************************************************************
* cpssDxCh2EgressPclPacketTypesSet
*
* DESCRIPTION:
*   Enables/disables Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*   enable        - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
GT_STATUS cpssDxCh2EgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              port,
    IN CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    IN GT_BOOL                           enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxCh2EgressPclPacketTypesSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, pktType, enable));

    rc = internal_cpssDxCh2EgressPclPacketTypesSet(devNum, port, pktType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, pktType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxCh2EgressPclPacketTypesGet
*
* DESCRIPTION:
*   Get status of Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*
* OUTPUTS:
*   enablePtr     - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxCh2EgressPclPacketTypesGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              port,
    IN  CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    OUT GT_BOOL                           *enablePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    GT_U32  fieldName;  /* name of the field */
    GT_U32  bitPos;     /* position of a bit */
    PRV_CPSS_DXCH_TABLE_ENT tableType; /* table's type */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                  CPSS_CH1_E | CPSS_CH1_DIAMOND_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, port);
        /* Bobcat2; Caelum; Bobcat3 */
        switch (pktType)
        {
            case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_FROM_CPU_CONTROL_PACKETS_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_FROM_CPU_DATA_PACKETS_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_EPCL_TO_CPU_PACKETS_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_EPCL_TO_ANALYZER_PACKETS_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_TS_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_TS_DATA_PORT_ENABLE_E;
                break;
            case CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E:
                tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E;
                fieldName = LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_NON_TS_DATA_PORT_ENABLE_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChReadTableEntryField(
              devNum,
              tableType,
              port,
              PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
              fieldName, /* field name */
              PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
              &hwValue);
        if(GT_OK != rc)
        {
            return rc;
        }

        *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE;

        return rc;
    }

    /* DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2 */
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    /* get the start bit 0..63 (in the bmp of registers) */
    bitPos = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
             PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
             localPort;

    switch (pktType)
    {
        case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktFromCpuControl[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktFromCpuData[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktToCpu[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktToAnalyzer[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TS_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktTs[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktNonTs[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, portGroupId, regAddr,
            OFFSET_TO_BIT_MAC(bitPos), 1, &hwValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE;

    return rc;
}

/*******************************************************************************
* cpssDxCh2EgressPclPacketTypesGet
*
* DESCRIPTION:
*   Get status of Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*
* OUTPUTS:
*   enablePtr     - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxCh2EgressPclPacketTypesGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              port,
    IN  CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    OUT GT_BOOL                           *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxCh2EgressPclPacketTypesGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, pktType, enablePtr));

    rc = internal_cpssDxCh2EgressPclPacketTypesGet(devNum, port, pktType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, pktType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxCh3PclTunnelTermForceVlanModeEnableSet
*
* DESCRIPTION:
*   The function enables/disables forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       enable         - force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxCh3PclTunnelTermForceVlanModeEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIEngineConfig;
            bitNum = 5;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
            bitNum = 10;
        }
    }
    else
    {
        /* DxCh3*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 28;
    }


    /* 1 - enable, 0 - disable */
    hwValue = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssHwPpSetRegField(devNum, regAddr, bitNum, 1, hwValue);
}

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableSet
*
* DESCRIPTION:
*   The function enables/disables forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       enable         - force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxCh3PclTunnelTermForceVlanModeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxCh3PclTunnelTermForceVlanModeEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxCh3PclTunnelTermForceVlanModeEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable of the forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxCh3PclTunnelTermForceVlanModeEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E );

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIEngineConfig;
            bitNum = 5;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
            bitNum = 10;
        }
    }
    else
    {
        /* DxCh1-3 devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 28;
    }


    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitNum, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 1 - enable, 0 - disable */
    *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable of the forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxCh3PclTunnelTermForceVlanModeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxCh3PclTunnelTermForceVlanModeEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxCh2PclTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*    l4PortType        - TCP/UDP port type, source or destination port
*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxCh2PclTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
)
{
    return cpssDxChPclPortGroupTcpUdpPortComparatorSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, l4Protocol, entryIndex,
        l4PortType, compareOperator, value);
}

/*******************************************************************************
* cpssDxCh2PclTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*    l4PortType        - TCP/UDP port type, source or destination port
*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxCh2PclTcpUdpPortComparatorSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value));

    rc = internal_cpssDxCh2PclTcpUdpPortComparatorSet(devNum, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxCh2PclTcpUdpPortComparatorGet
*
* DESCRIPTION:
*        Get TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*    l4PortTypePtr      - TCP/UDP port type, source or destination port
*    compareOperatorPtr - compare operator FALSE, LTE, GTE, NEQ
*    valuePtr           - 16 bit value to compare with
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxCh2PclTcpUdpPortComparatorGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_PCL_DIRECTION_ENT            direction,
    IN  CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN  GT_U8                             entryIndex,
    OUT CPSS_L4_PROTOCOL_PORT_TYPE_ENT    *l4PortTypePtr,
    OUT CPSS_COMPARE_OPERATOR_ENT         *compareOperatorPtr,
    OUT GT_U16                            *valuePtr
)
{
    return cpssDxChPclPortGroupTcpUdpPortComparatorGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, l4Protocol, entryIndex,
        l4PortTypePtr, compareOperatorPtr, valuePtr);
}

/*******************************************************************************
* cpssDxCh2PclTcpUdpPortComparatorGet
*
* DESCRIPTION:
*        Get TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*    l4PortTypePtr      - TCP/UDP port type, source or destination port
*    compareOperatorPtr - compare operator FALSE, LTE, GTE, NEQ
*    valuePtr           - 16 bit value to compare with
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclTcpUdpPortComparatorGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_PCL_DIRECTION_ENT            direction,
    IN  CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN  GT_U8                             entryIndex,
    OUT CPSS_L4_PROTOCOL_PORT_TYPE_ENT    *l4PortTypePtr,
    OUT CPSS_COMPARE_OPERATOR_ENT         *compareOperatorPtr,
    OUT GT_U16                            *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxCh2PclTcpUdpPortComparatorGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, l4Protocol, entryIndex, l4PortTypePtr, compareOperatorPtr, valuePtr));

    rc = internal_cpssDxCh2PclTcpUdpPortComparatorGet(devNum, direction, l4Protocol, entryIndex, l4PortTypePtr, compareOperatorPtr, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, l4Protocol, entryIndex, l4PortTypePtr, compareOperatorPtr, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*       Old function for DxCh1 and DxCh2 devices and for
*       standard rules on DxCh3 devices.
*       The general function is cpssDxChPclRuleAnyStateGet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    return cpssDxChPclRuleAnyStateGet(
        devNum, CPSS_PCL_RULE_SIZE_STD_E,
        ruleIndex, validPtr, ruleSizePtr);
}

/*******************************************************************************
* cpssDxChPclRuleStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*       Old function for DxCh1 and DxCh2 devices and for
*       standard rules on DxCh3 devices.
*       The general function is cpssDxChPclRuleAnyStateGet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleIndex, validPtr, ruleSizePtr));

    rc = internal_cpssDxChPclRuleStateGet(devNum, ruleIndex, validPtr, ruleSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleIndex, validPtr, ruleSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    return cpssDxChPclPortGroupRuleAnyStateGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex,
        validPtr, ruleSizePtr);
}

/*******************************************************************************
* cpssDxChPclRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleAnyStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleSize, ruleIndex, validPtr, ruleSizePtr));

    rc = internal_cpssDxChPclRuleAnyStateGet(devNum, ruleSize, ruleIndex, validPtr, ruleSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleSize, ruleIndex, validPtr, ruleSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 21  words.
*       patternArr       - rule pattern       - 21  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclRuleGet
(
    IN  GT_U8                  devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
)
{
    return cpssDxChPclPortGroupRuleGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex,
        maskArr, patternArr, actionArr);
}

/*******************************************************************************
* cpssDxChPclRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 21  words.
*       patternArr       - rule pattern       - 21  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleGet
(
    IN  GT_U8                  devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclRuleGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleSize, ruleIndex, maskArr, patternArr, actionArr));

    rc = internal_cpssDxChPclRuleGet(devNum, ruleSize, ruleIndex, maskArr, patternArr, actionArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleSize, ruleIndex, maskArr, patternArr, actionArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
{
    return cpssDxChPclPortGroupCfgTblEntryGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, lookupNum, entryIndex, pclCfgTblEntryPtr);
}

/*******************************************************************************
* cpssDxChPclCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclCfgTblEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, lookupNum, entryIndex, pclCfgTblEntryPtr));

    rc = internal_cpssDxChPclCfgTblEntryGet(devNum, direction, lookupNum, entryIndex, pclCfgTblEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, lookupNum, entryIndex, pclCfgTblEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
{
    return cpssDxChPclPortGroupLookupCfgPortListEnableSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, lookupNum, subLookupNum, enable);
}

/*******************************************************************************
* cpssDxChPclLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclLookupCfgPortListEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, lookupNum, subLookupNum, enable));

    rc = internal_cpssDxChPclLookupCfgPortListEnableSet(devNum, direction, lookupNum, subLookupNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, lookupNum, subLookupNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
{
    return cpssDxChPclPortGroupLookupCfgPortListEnableGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, lookupNum, subLookupNum, enablePtr);
}

/*******************************************************************************
* cpssDxChPclLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclLookupCfgPortListEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, lookupNum, subLookupNum, enablePtr));

    rc = internal_cpssDxChPclLookupCfgPortListEnableGet(devNum, direction, lookupNum, subLookupNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, lookupNum, subLookupNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has 3.5K (3584) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Write rule to TCAM not using port-list format.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
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
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The pattern and mask input structures should be set to zero
*      by an application before assignment of fields.
*      The function places all muxed fields to rule patern and mask
*      by using bitwise OR operation.
*      The function does not check validity of muxed (overlapped) fields.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupRuleSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN GT_U32                                           ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC                         *actionPtr
)
{
    GT_STATUS    rc;            /* return code                          */
    GT_U32       ruleSize;      /* ruleSize (in STD rules)              */
                    /* for E_ARCH - CPSS_DXCH_TCAM_RULE_SIZE_ENT member */
    GT_U32       hwAction[CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS];
                                /* action in HW format                  */
    GT_U32       hwMask[CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS];
                                /* mask in HW format                    */
    GT_U32       hwPattern[CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS];
                                /* pattern in HW format                 */
    GT_BOOL      egressRule;    /* egress Rule                          */
    GT_BOOL      validRule;     /* write valid rule                     */
    GT_U32       portGroupId;   /* port group Id                        */
    PRV_CPSS_DXCH_PCL_RULE_SOURCE_PORT_GROUP_MODIFIED_FLD_STC fldSrc;
    PRV_CPSS_DXCH_PCL_RULE_STORE_PORT_GROUP_MODIFIED_FLD_STC  storeFld;
    GT_U32      entryNumber = 0;/* Rule's index in TCAM    */
    GT_U32      dummy;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_10_B_E; /* default */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    CPSS_NULL_PTR_CHECK_MAC(patternPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* check rule format and get egressRule value */
    /* defaults to aviod compiler warnings        */
    ruleSize   = 1;
    egressRule = GT_FALSE;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule's format to rule's size */
        PRV_CPSS_DXCH_PCL_CONVERT_RULE_FORMAT_TO_TCAM_RULE_SIZE_VAL_MAC(tcamRuleSize,ruleFormat);

        /* convert rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,ruleIndex,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssDxChPclRuleGetDirAndSize(
            ruleFormat, &egressRule, &ruleSize);
        if (rc != GT_OK)
        {
            return rc;
        }

        PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSize, ruleIndex);
    }

    rc = prvCpssDxChPclRuleDataSw2HwConvert(
        devNum, ruleFormat,
        maskPtr, patternPtr, actionPtr,
        hwMask, hwPattern, hwAction);
    if (rc != GT_OK)
    {
        return rc;
    }

    validRule =
        (ruleOptionsBmp & CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E)
        ? GT_FALSE : GT_TRUE;

    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum) == 0)
    {
        /* all devices up to xCat */
        /* write rule             */
        return prvCpssDxChPclTcamRuleWrite(
            devNum, 0 /*portGroupId*/,
            ruleSize, ruleIndex, validRule,
            0 /*waitPrevReady*/, hwAction, hwMask, hwPattern);
    }

    /* retrieve modified fields sources */
    rc = prvCpssDxChPclRuleGetModifiedFieldsSources(
        ruleFormat, maskPtr, patternPtr, &fldSrc);
    if (rc != GT_OK)
    {
        return rc;
    }


    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {

        /* convert values according to current port group */
        rc = prvCpssDxChPclRuleDataConvertForPortGroup(
            devNum, portGroupId, &fldSrc, &storeFld);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* store converted values of fields to HW formated */
        /* of pattern and mask                             */
        rc = prvCpssDxChPclRuleDataStorePortGroupModifiedFields(
            devNum, &storeFld, hwMask, hwPattern);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
             /* invalidate rule in TCAM */
            rc = cpssDxChTcamPortGroupRuleValidStatusSet(devNum,portGroupsBmp,ruleIndex,GT_FALSE);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* write action */
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                                     portGroupId,
                                                     PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                     (entryNumber/2),
                                                     hwAction);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* write rule to the TCAM */
            rc = cpssDxChTcamPortGroupRuleWrite(devNum,portGroupsBmp,ruleIndex,validRule,tcamRuleSize,hwPattern,hwMask);

            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* write rule */
            rc = prvCpssDxChPclTcamRuleWrite(
                devNum, portGroupId,
                ruleSize, ruleIndex, validRule,
                0 /*waitPrevReady*/, hwAction, hwMask, hwPattern);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPclPortGroupRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has 3.5K (3584) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Write rule to TCAM not using port-list format.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
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
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The pattern and mask input structures should be set to zero
*      by an application before assignment of fields.
*      The function places all muxed fields to rule patern and mask
*      by using bitwise OR operation.
*      The function does not check validity of muxed (overlapped) fields.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN GT_U32                                           ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC                         *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleFormat, ruleIndex, ruleOptionsBmp, maskPtr, patternPtr, actionPtr));

    rc = internal_cpssDxChPclPortGroupRuleSet(devNum, portGroupsBmp, ruleFormat, ruleIndex, ruleOptionsBmp, maskPtr, patternPtr, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleFormat, ruleIndex, ruleOptionsBmp, maskPtr, patternPtr, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclBits191_1Drag
*
* DESCRIPTION:
*   The function drags bits [191:1] to [190:0] and bit [0] to 191
*   supported for devices up to Lion (inclusive) , but upper function not supports
*   ch3 and below
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       ruleDataPtr           - (pointer to) 192-bit rule data buffer
*
* OUTPUTS:
*       ruleDataPtr           - (pointer to) 192-bit rule data buffer
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxChPclBits191_1Drag
(
    INOUT GT_U32                                         *ruleDataPtr
)
{
    GT_U32       workBit0;       /* bit value for drag         */
    GT_U32       workBit1;       /* bit value for drag         */
    GT_U32       i,j;            /* loop index                 */

    /* drag bit0 - to 191-th position   */
    /* drag bits [191:1] to [190:0]     */
    workBit0 = ruleDataPtr[0] & 1;
    for (i = 6; (i > 0); i--)
    {
        j = i - 1;
        workBit1 = ruleDataPtr[j] & 1;
        ruleDataPtr[j] =
            (((ruleDataPtr[j] >> 1) & 0x7FFFFFFF)
            | (workBit0 << 31));
        workBit0 = workBit1;
    }
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleParsedGet
*
* DESCRIPTION:
*   The function gets the Policy Rule valid state, Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ruleFormat       - format of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*
* OUTPUTS:
*       isRuleValidPtr   - GT_TRUE - rule is valid, GT_FALSE - rule is invalid
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
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - when size is trash or not related to ruleFormat.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupRuleParsedGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN GT_U32                                           ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    OUT GT_BOOL                                         *isRuleValidPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                   *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                   *patternPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC                        *actionPtr
)
{
    GT_STATUS    rc;            /* return code                 */
    GT_U32       portGroupId;   /*the port group Id            */
    GT_U32       ruleSizeFound; /* rule size (in STD rules)    */
    GT_U32       ruleSizeVal;   /* rule  size (in STD rules)   */
    GT_BOOL      egressRule;    /* egress Rule                 */
    GT_U32       mask[CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS] = {0};
                                /* mask in HW format           */
    GT_U32       pattern[CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS];
                                /* pattern in HW format        */
    GT_U32       action[CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS];
                                /* action in HW format         */
    PRV_CPSS_DXCH_PCL_RULE_STORE_PORT_GROUP_MODIFIED_FLD_STC   storeFld;
                                /* srcPort and portListBmp retrieved from HW     */
    PRV_CPSS_DXCH_PCL_RULE_STC_PORT_GROUP_MODIFIED_PTRS_STC    maskSrcPointers;
                                /* pointers to srcPort and portListBmp in mask   */
    PRV_CPSS_DXCH_PCL_RULE_STC_PORT_GROUP_MODIFIED_PTRS_STC    patternSrcPointers;
                                /*pointers to srcPort and portListBmp in pattern */
    CPSS_DXCH_TCAM_RULE_SIZE_ENT tcamRuleSize;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT tcamRuleSizeFound;
    GT_BOOL     validFound; /* is rule found valid     */
    GT_PORT_GROUPS_BMP currPortGroupsBmp = 0;
    GT_U32      entryNumber = 0;/* Rule's index in TCAM    */
    GT_U32      dummy;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    CPSS_NULL_PTR_CHECK_MAC(isRuleValidPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    CPSS_NULL_PTR_CHECK_MAC(patternPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);

    ruleOptionsBmp = ruleOptionsBmp;/*unused parameter*/

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* for not E_ARCH devices get ruleSizeVal */
    /* any case get egressRule                */
    rc = prvCpssDxChPclRuleGetDirAndSize(
        ruleFormat, &egressRule, &ruleSizeVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule's format to rule's size */
        PRV_CPSS_DXCH_PCL_CONVERT_RULE_FORMAT_TO_TCAM_RULE_SIZE_VAL_MAC(tcamRuleSize,ruleFormat);

        rc = cpssDxChTcamPortGroupRuleValidStatusGet(
            devNum,currPortGroupsBmp,ruleIndex,isRuleValidPtr, &tcamRuleSizeFound);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (tcamRuleSizeFound != tcamRuleSize)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* read rule from the TCAM */
        rc = cpssDxChTcamPortGroupRuleRead(
            devNum,currPortGroupsBmp,ruleIndex,&validFound,&tcamRuleSize,pattern,mask);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(
            devNum, ruleSizeVal, ruleIndex);

        rc = prvCpssDxChPclTcamRuleStateGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, isRuleValidPtr, &ruleSizeFound);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (ruleSizeVal != ruleSizeFound)
        {
            /* source rule is valid, but has another size */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        rc = prvCpssDxChPclTcamRuleDataGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, mask, pattern);
        if (rc != GT_OK)
        {
            return rc;
        }

        if ((egressRule != GT_FALSE)
            && (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) == 0)
            /* next 2 keys not need the 'shift' (not hold 'valid bit' at bit 0)*/
            && (ruleFormat != CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E)
            && (ruleFormat != CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E)
            )
        {
            /* egress rules xCat and Lion             */
            /* drag bit0 - valid to 191-th position   */
            /* drag bits [191:1] to [190:0]           */
            /* to be at the same places as on devices */
            /* xCat2 and above                        */
            prvCpssDxChPclBits191_1Drag(mask);
            prvCpssDxChPclBits191_1Drag(pattern);
        }
    }

    rc = prvCpssDxChPclTcamRuleDataHw2SwConvert(
        devNum, ruleFormat, mask, maskPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamRuleDataHw2SwConvert(
        devNum, ruleFormat, pattern, patternPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,ruleIndex,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read action */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                                portGroupId,
                                                PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                (entryNumber/2),
                                                action);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssDxChPclTcamActionGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, GT_TRUE /*waitPrevReady*/, action);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChPclTcamRuleActionHw2SwConvert(
        devNum,
        ((egressRule == GT_FALSE)
            ? CPSS_PCL_DIRECTION_INGRESS_E
            : CPSS_PCL_DIRECTION_EGRESS_E),
        action, actionPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (! PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        return GT_OK;
    }

    /* the sip5 device does not need any conversions on the src port field */
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
       return GT_OK;
    }

    rc = prvCpssDxChPclRuleGetModifiedFieldsSourcesPointers(
        ruleFormat, maskPtr, &maskSrcPointers);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclRuleGetModifiedFieldsSourcesPointers(
        ruleFormat, patternPtr, &patternSrcPointers);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Clear srcPort and port list after first retrieve */

    if (maskSrcPointers.srcPortPtr != NULL)
    {
        *(maskSrcPointers.srcPortPtr)    = 0;
    }

    if (patternSrcPointers.srcPortPtr != NULL)
    {
        *(patternSrcPointers.srcPortPtr)    = 0;
    }

    if(maskSrcPointers.portListBmpPtr &&
       patternSrcPointers.portListBmpPtr)
    {
        maskSrcPointers.portListBmpPtr->ports[0]    = 0;
        patternSrcPointers.portListBmpPtr->ports[0] = 0;
    }


    /* rescan the fields modified by RuleSet function */
    /* according to port group                        */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDxChPclTcamRuleDataGet(
                    devNum, portGroupId,
                    ruleSizeVal, ruleIndex, mask, pattern);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* the xCat and Lion egress rules bits drag  */
        /* done before raw to structure conversion   */
        /* (see in this function upper)              */
        /* not relevant for rule formats treted here */
        storeFld.isEgress = egressRule;
        storeFld.ruleSize = ruleSizeVal;
        rc = prvCpssDxChPclRuleDataLoadPortGroupModifiedFields(
            devNum, mask, pattern, &storeFld);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPclRuleDataAccumulateFromPortGroup(
            devNum, portGroupId, &storeFld,
            &maskSrcPointers, &patternSrcPointers);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleParsedGet
*
* DESCRIPTION:
*   The function gets the Policy Rule valid state, Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ruleFormat       - format of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*
* OUTPUTS:
*       isRuleValidPtr   - GT_TRUE - rule is valid, GT_FALSE - rule is invalid
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
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - when size is trash or not related to ruleFormat.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleParsedGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN GT_U32                                           ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    OUT GT_BOOL                                         *isRuleValidPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                   *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                   *patternPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC                        *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleParsedGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleFormat, ruleIndex, ruleOptionsBmp, isRuleValidPtr, maskPtr, patternPtr, actionPtr));

    rc = internal_cpssDxChPclPortGroupRuleParsedGet(devNum, portGroupsBmp, ruleFormat, ruleIndex, ruleOptionsBmp, isRuleValidPtr, maskPtr, patternPtr, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleFormat, ruleIndex, ruleOptionsBmp, isRuleValidPtr, maskPtr, patternPtr, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
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
static GT_STATUS internal_cpssDxChPclPortGroupRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       portGroupId; /*the port group Id            */
    GT_U32       hwAction[CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS];
                              /* action in HW format       */
    GT_U32       ruleSizeVal=0; /* ruleSize (in STD rules)   */
    GT_U32       entryNumber = 0;/* Rule's index in TCAM    */
    GT_U32       dummy;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* convert action to HW format */
    rc = prvCpssDxChPclTcamRuleActionSw2HwConvert(
        devNum, actionPtr, hwAction);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,ruleIndex,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
        PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* write action */
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                                     portGroupId,
                                                     PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                     (entryNumber/2),
                                                     hwAction);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* write action */
            rc = prvCpssDxChPclTcamRuleActionUpdate(
                devNum, portGroupId,
                ruleSizeVal, ruleIndex, hwAction);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
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
GT_STATUS cpssDxChPclPortGroupRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleActionUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleSize, ruleIndex, actionPtr));

    rc = internal_cpssDxChPclPortGroupRuleActionUpdate(devNum, portGroupsBmp, ruleSize, ruleIndex, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleSize, ruleIndex, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       portGroupId; /*the port group Id            */
    GT_U32       ruleSizeVal=0; /* rule  size (in STD rules)   */
    GT_U32       action[CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS];
                               /* action in HW format       */
    GT_U32       entryNumber = 0;/* Rule's index in TCAM    */
    GT_U32       dummy;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_PCL_DIRECTION_CHECK_MAC(direction);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,ruleIndex,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read action */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                                portGroupId,
                                                PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                (entryNumber/2),
                                                action);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
        PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);

        rc = prvCpssDxChPclTcamActionGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, GT_TRUE /*waitPrevReady*/, action);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return prvCpssDxChPclTcamRuleActionHw2SwConvert(
        devNum, direction, action, actionPtr);
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleActionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleSize, ruleIndex, direction, actionPtr));

    rc = internal_cpssDxChPclPortGroupRuleActionGet(devNum, portGroupsBmp, ruleSize, ruleIndex, direction, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleSize, ruleIndex, direction, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
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
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupRuleInvalidate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
)
{
    GT_STATUS    rc;            /* return code                             */
    GT_U32       portGroupId;   /*the port group Id                        */
    GT_U32       ruleSizeVal;   /* rule size (in std rules)                */
    GT_U32       absIndex;      /* absolute rule index (in STD rules)      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* invalidate rule in TCAM */
        rc = cpssDxChTcamPortGroupRuleValidStatusSet(devNum,portGroupsBmp,ruleIndex,GT_FALSE);

        return rc;
    }

    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        /* CH and CH2 devices */
        /* invalidate rule    */
        rc = prvCpssDxChPclTcamOperationStart(
            devNum,
            ruleSizeVal /*ruleSize*/,
            ruleIndex, GT_TRUE /*waitPrevReady*/,
            PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID  /*tcamOperation*/);

        return rc;
    }

    /* CH3 devices and above */

    /* get index of first Standard part of rule */
    rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
        devNum, ruleSizeVal, ruleIndex, 0/*ruleSegment*/, &absIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* invalidate first standard part of rule */
        rc = prvCpssDxChPclTcamStdRuleValidStateSet(
            devNum, portGroupId, absIndex, GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
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
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleInvalidate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleSize, ruleIndex));

    rc = internal_cpssDxChPclPortGroupRuleInvalidate(devNum, portGroupsBmp, ruleSize, ruleIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleSize, ruleIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
{
    GT_STATUS    rc;                           /* return code              */
    GT_U32       portGroupId;                  /* the port group Id        */
    GT_U32       ruleSizeVal;                  /* rule size (in std rules) */
    GT_U32       ruleSizeFound;                /* rule size found          */
    GT_BOOL      validFound;                   /* is rule found valid      */
    GT_U32       mask[12];                     /* TCAM mask of rule        */
    GT_U32       pattern[12];                  /* TCAM pattern of rule     */
    PRV_CPSS_DXCH_PCL_TCAM_OPERATION tcamOper; /* write valid or invalid   */
    GT_U32       absIndex;      /* absolute rule inex (in STD rules)       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* set valid status for rule in TCAM */
        rc = cpssDxChTcamPortGroupRuleValidStatusSet(devNum,portGroupsBmp,ruleIndex,valid);

        return rc;
    }

    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDxChPclTcamRuleStateGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, &validFound, &ruleSizeFound);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (ruleSizeVal != ruleSizeFound)
        {
            /* source rule has another size */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        if (valid == validFound)
        {
            /* the rule is aready in requred valid state */
            return GT_OK;
        }

        if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            tcamOper = (valid == GT_FALSE)
                ? PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID
                : PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_VALID;

            rc = prvCpssDxChPclTcamRuleDataGet(
                devNum, portGroupId,
                ruleSizeVal, ruleIndex, mask, pattern);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* put pattern data to buffer registers */
            rc = prvCpssDxChPclTcamOperDataPut(
                devNum, ruleSizeVal, tcamOper, pattern);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* validate/invalidate rule */
            rc = prvCpssDxChPclTcamOperationStart(
                devNum, ruleSizeVal, ruleIndex, GT_TRUE /*waitPrevReady*/, tcamOper);
            {
                return rc;
            }
        }
        else
        {
            /* Cheetah3 and above */
            rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
                devNum, ruleSizeVal, ruleIndex, 0/*ruleSegment*/, &absIndex);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* modify validity of first standard part of rule*/
            rc = prvCpssDxChPclTcamStdRuleValidStateSet(
                devNum, portGroupId, absIndex, valid);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleValidStatusSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleSize, ruleIndex, valid));

    rc = internal_cpssDxChPclPortGroupRuleValidStatusSet(devNum, portGroupsBmp, ruleSize, ruleIndex, valid);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleSize, ruleIndex, valid));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize       - size of rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    GT_U32    ruleSizeVal=0; /* rule size (in STD rules)              */
    GT_U32    ruleSizeFound; /* rule size (in STD rules)              */
    GT_STATUS rc;            /* return code                           */
    GT_U32    portGroupId;   /* the port group Id                     */
    CPSS_DXCH_TCAM_RULE_SIZE_ENT tcamRuleSize; /* TCAM rule size      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleSizePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* get valid status for rule in TCAM */
        rc = cpssDxChTcamPortGroupRuleValidStatusGet(
            devNum, portGroupsBmp, ruleIndex, validPtr, &tcamRuleSize);
        if (rc != GT_OK)
        {
            return rc;
        }

        switch (tcamRuleSize)
        {
            case CPSS_DXCH_TCAM_RULE_SIZE_30_B_E:
                *ruleSizePtr = CPSS_PCL_RULE_SIZE_STD_E;
                break;
            case CPSS_DXCH_TCAM_RULE_SIZE_60_B_E:
                *ruleSizePtr = CPSS_PCL_RULE_SIZE_EXT_E;
                break;
            case CPSS_DXCH_TCAM_RULE_SIZE_80_B_E:
                *ruleSizePtr = CPSS_PCL_RULE_SIZE_ULTRA_E;
                break;
            case CPSS_DXCH_TCAM_RULE_SIZE_10_B_E:
                *ruleSizePtr = CPSS_PCL_RULE_SIZE_10_BYTES_E;
                break;
            case CPSS_DXCH_TCAM_RULE_SIZE_20_B_E:
                *ruleSizePtr = CPSS_PCL_RULE_SIZE_20_BYTES_E;
                break;
            case CPSS_DXCH_TCAM_RULE_SIZE_40_B_E:
                *ruleSizePtr = CPSS_PCL_RULE_SIZE_40_BYTES_E;
                break;
            case CPSS_DXCH_TCAM_RULE_SIZE_50_B_E:
                *ruleSizePtr = CPSS_PCL_RULE_SIZE_50_BYTES_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        return GT_OK;
    }
    else
    {
        PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
        PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);

        rc = prvCpssDxChPclTcamRuleStateGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, validPtr, &ruleSizeFound);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *ruleSizePtr = (ruleSizeFound == 1) ?  CPSS_PCL_RULE_SIZE_STD_E :
                   (ruleSizeFound == 2) ?  CPSS_PCL_RULE_SIZE_EXT_E :
                                           CPSS_PCL_RULE_SIZE_ULTRA_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize       - size of rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleAnyStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleSize, ruleIndex, validPtr, ruleSizePtr));

    rc = internal_cpssDxChPclPortGroupRuleAnyStateGet(devNum, portGroupsBmp, ruleSize, ruleIndex, validPtr, ruleSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleSize, ruleIndex, validPtr, ruleSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclPortGroupRuleCopy should be
*  called. And after this cpssDxChPclPortGroupRuleInvalidate or
*  cpssDxChPclPortGroupRuleValidStatusSet should
*  be used to invalidate Rule in old position.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize         - size of Rule.
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
static GT_STATUS internal_cpssDxChPclPortGroupRuleCopy
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       portGroupId; /* port group Id for multi-port-group devices */
    GT_U32       ruleSizeVal=0; /* rule size (in std rules)      */
    CPSS_DXCH_TCAM_RULE_SIZE_ENT tcamRuleSize = CPSS_DXCH_TCAM_RULE_SIZE_10_B_E; /* default */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_LION3_TCAM_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert pcl rule's size to tcam rule's size */
        PRV_CPSS_DXCH_PCL_CONVERT_RULE_SIZE_TO_TCAM_RULE_SIZE_VAL_MAC(tcamRuleSize, ruleSize);
    }
    else
    {
        PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
        PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(
            devNum, ruleSizeVal, ruleSrcIndex);
        PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(
            devNum, ruleSizeVal, ruleDstIndex);
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = prvCpssDxChPclLion3RuleCopy(devNum,portGroupId,tcamRuleSize,ruleSrcIndex, ruleDstIndex);
        }
        else
        {
            rc = prvCpssDxChPclRuleCopy(
                devNum, portGroupId,
                ruleSizeVal, ruleSrcIndex, ruleDstIndex);
        }
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclPortGroupRuleCopy should be
*  called. And after this cpssDxChPclPortGroupRuleInvalidate or
*  cpssDxChPclPortGroupRuleValidStatusSet should
*  be used to invalidate Rule in old position.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize         - size of Rule.
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
GT_STATUS cpssDxChPclPortGroupRuleCopy
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleCopy);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleSize, ruleSrcIndex, ruleDstIndex));

    rc = internal_cpssDxChPclPortGroupRuleCopy(devNum, portGroupsBmp, ruleSize, ruleSrcIndex, ruleDstIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleSize, ruleSrcIndex, ruleDstIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 21  words.
*       patternArr       - rule pattern       - 21  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupRuleGet
(
    IN  GT_U8                  devNum,
    IN  GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
)
{
    GT_STATUS  rc;            /* return code                 */
    GT_U32     portGroupId;   /* port group Id               */
    GT_BOOL    valid;         /* valid source rule           */
    GT_U32     ruleSizeVal=0; /* rule size (in STD rules)    */
    GT_U32     ruleSizeFound; /* rule size (in STD rules)    */
    GT_PORT_GROUPS_BMP currPortGroupsBmp = 0;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT tcamRuleSize; /* TCAM rule size */
    CPSS_DXCH_TCAM_RULE_SIZE_ENT tcamRuleSizeFound; /* TCAM rule size Found */
    GT_U32      entryNumber = 0;/* Rule's index in TCAM    */
    GT_U32      dummy;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(maskArr);
    CPSS_NULL_PTR_CHECK_MAC(patternArr);
    CPSS_NULL_PTR_CHECK_MAC(actionArr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PCL_CONVERT_RULE_SIZE_TO_TCAM_RULE_SIZE_VAL_MAC(
            tcamRuleSize, ruleSize);

        rc = cpssDxChTcamPortGroupRuleValidStatusGet(
            devNum, currPortGroupsBmp, ruleIndex, &valid, &tcamRuleSizeFound);
        if (rc != GT_OK)
        {
            return rc;
        }
        if ((tcamRuleSize != tcamRuleSizeFound) && (valid != GT_FALSE))
        {
            /* source rule is valid, but has another size */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
        PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(
            devNum, ruleSizeVal, ruleIndex);

        rc = prvCpssDxChPclTcamRuleStateGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, &valid, &ruleSizeFound);
        if (rc != GT_OK)
        {
            return rc;
        }

        if ((ruleSizeVal != ruleSizeFound) && (valid != GT_FALSE))
        {
            /* source rule is valid, but has another size */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,ruleIndex,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read action */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                                portGroupId,
                                                PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                (entryNumber/2),
                                                actionArr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* read rule from the TCAM */
        rc = cpssDxChTcamPortGroupRuleRead(devNum,currPortGroupsBmp,ruleIndex,&valid,&tcamRuleSize,patternArr,maskArr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssDxChPclTcamActionGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, GT_TRUE /*waitPrevReady*/, actionArr);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPclTcamRuleDataGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, maskArr, patternArr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 21  words.
*       patternArr       - rule pattern       - 21  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleGet
(
    IN  GT_U8                  devNum,
    IN  GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupRuleGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleSize, ruleIndex, maskArr, patternArr, actionArr));

    rc = internal_cpssDxChPclPortGroupRuleGet(devNum, portGroupsBmp, ruleSize, ruleIndex, maskArr, patternArr, actionArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleSize, ruleIndex, maskArr, patternArr, actionArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupCfgTblSet
(
    IN GT_U8                           devNum,
    IN GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN CPSS_INTERFACE_INFO_STC         *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC    lookupHwCfg;
    GT_STATUS res;           /* return code                                       */
    GT_U32    entryIndex;    /* pcl configuration table entry Index               */
    GT_BOOL   extKey;        /* GT_TRUE - extended key, GT_FALSE - standard       */
    GT_BOOL   l4Key;         /* GT_TRUE - l4 style key, GT_FALSE - l2 style       */
    GT_BOOL   dipKey;        /* GT_TRUE - ipv6 DIP style key, GT_FALSE - l2 style */
    GT_U32    hwValue;       /* pcl configuration table HW value                  */
    GT_U32    portGroupId;        /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(interfaceInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(lookupCfgPtr);
    PRV_CPSS_DXCH_PCL_LOOKUP_NUM_CHECK_MAC(lookupNum);

    res = prvCpssDxChPclCfgTblEntryIndexGet(
        devNum, interfaceInfoPtr, direction, &entryIndex , &portGroupId);
    if (res != GT_OK)
    {
        return res;
    }

    if (portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        if ((portGroupsBmp  != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
            && (portGroupsBmp != (GT_U32)(1 << portGroupId)))
        {
            /* valid only "All port groups" or the correct port group */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* update bitmap to correct port group */
        portGroupsBmp = (1 << portGroupId);
    }

    /* common for all cases */
    cpssOsMemSet(&lookupHwCfg, 0, sizeof(lookupHwCfg));
    lookupHwCfg.enableLookup = (GT_U8)BOOL2BIT_MAC(lookupCfgPtr->enableLookup);
    lookupHwCfg.pclId        = lookupCfgPtr->pclId;

    if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat2 and above - 3 configuration tables */
        lookupHwCfg.dualLookup   = 0;
        lookupHwCfg.pclIdL01     = 0;
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and Lion */
        lookupHwCfg.dualLookup   =
            (GT_U8)BOOL2BIT_MAC(lookupCfgPtr->dualLookup);
        lookupHwCfg.pclIdL01     = lookupCfgPtr->pclIdL01;
    }
    else
    {
        /* DxCh1-3 */
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
        {
            /* DxCh3 */
            lookupHwCfg.dualLookup   =
                (GT_U8)BOOL2BIT_MAC(lookupCfgPtr->dualLookup);
        }
        else
        {
            /* DxCh1-2 devices */
            lookupHwCfg.dualLookup   = 0;
        }
        lookupHwCfg.pclIdL01     = 0;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* check Non IP Key type and set extKey variable */
        switch (lookupCfgPtr->groupKeyTypes.nonIpKey)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                extKey = GT_FALSE;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                extKey = GT_TRUE;
                break;
            /* all other types not relevant */
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Set default for l4Key and prevent compiler warning */
        l4Key = GT_FALSE;

        switch (lookupCfgPtr->groupKeyTypes.ipv4Key)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                if (extKey == GT_TRUE)
                {
                    /* both IPv4 and nonIp must be same size */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                if (extKey == GT_TRUE)
                {
                    /* both IPv4 and nonIp must be same size */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                l4Key  = GT_TRUE;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                if (extKey == GT_FALSE)
                {
                    /* both IPv4 and nonIp must be same size */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;
            /* all other types not relevant */
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Set default for dipKey and prevent compiler warning */
        dipKey = GT_FALSE;

        switch (lookupCfgPtr->groupKeyTypes.ipv6Key)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                if (extKey == GT_TRUE)
                {
                    /* both IPv6 and nonIp must be same size */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
                if (extKey == GT_TRUE)
                {
                    /* both IPv6 and nonIp must be same size */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                if (lookupNum == CPSS_PCL_LOOKUP_0_E)
                {
                    /* DIP only for lookup1*/
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                dipKey = GT_TRUE;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                if (extKey == GT_FALSE)
                {
                    /* both IPv6 and nonIp must be same size */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                l4Key  = GT_FALSE;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                if (extKey == GT_FALSE)
                {
                    /* both IPv6 and nonIp must be same size */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                l4Key  = GT_TRUE;
                break;
            /* all other types not relevant */
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* DxChPlus specific field values use */
        lookupHwCfg.nonIpKeyType = (GT_U8)((extKey == GT_FALSE) ? 0 : 1);
        lookupHwCfg.ipv4KeyType  = (GT_U8)((l4Key == GT_FALSE) ? 0 : 1);
        lookupHwCfg.ipv6KeyType  = (GT_U8)((dipKey == GT_FALSE) ? 0 : 1);
    }
    else
    {  /* DxCh2 and above */

        /* non IP key style */
        res = prvCpssDxChPclKeyTypeSwToHw(
            devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS,
            lookupCfgPtr->groupKeyTypes.nonIpKey,
            &hwValue);
        if (res != GT_OK)
        {
            return res;
        }
        lookupHwCfg.nonIpKeyType = (GT_U8)hwValue;

        /* IPv4 key style */
        res = prvCpssDxChPclKeyTypeSwToHw(
            devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS,
            lookupCfgPtr->groupKeyTypes.ipv4Key,
            &hwValue);
        if (res != GT_OK)
        {
            return res;
        }
        lookupHwCfg.ipv4KeyType = (GT_U8)hwValue;

        /* IPv6 key style */
        res = prvCpssDxChPclKeyTypeSwToHw(
            devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS,
            lookupCfgPtr->groupKeyTypes.ipv6Key,
            &hwValue);
        if (res != GT_OK)
        {
            return res;
        }
        lookupHwCfg.ipv6KeyType = (GT_U8)hwValue;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        lookupHwCfg.udbKeyBitmapEnable = BOOL2BIT_MAC(lookupCfgPtr->udbKeyBitmapEnable);

        switch (lookupCfgPtr->tcamSegmentMode)
        {
            case CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_1_TCAM_E:
                lookupHwCfg.tcamSegmentMode = 0;
                break;
            case CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_2_AND_2_E:
                lookupHwCfg.tcamSegmentMode = 1;
                break;
            case CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_1_AND_3_E:
                lookupHwCfg.tcamSegmentMode = 2;
                break;
            case CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_3_AND_1_E:
                lookupHwCfg.tcamSegmentMode = 3;
                break;
            case CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_4_TCAMS_E:
                lookupHwCfg.tcamSegmentMode = 4;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        res = prvCpssDxChPclCfgTblHwWrite(
            devNum, portGroupId , direction, entryIndex,
            lookupNum, 0 /*subLookupNum*/, &lookupHwCfg);
        if (res != GT_OK)
        {
            return res;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    if ((0 == PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        || (direction != CPSS_PCL_DIRECTION_INGRESS_E)
        || (lookupNum != CPSS_PCL_LOOKUP_0_E))
    {
        /* only one configuration table needed update */
        return GT_OK;
    }

    /* xCat2 ingress lookup01 table update              */
    /* Ingress PCL CPSS_PCL_LOOKUP_0_E - configure      */
    /* lookup_0_1 with the same key types as lookup_0_0 */
    lookupHwCfg.enableLookup = (GT_U8)BOOL2BIT_MAC(lookupCfgPtr->dualLookup);
    lookupHwCfg.pclId        = lookupCfgPtr->pclIdL01;
    /* key types alredy converted for lookup_0_0 */
    /* lookupHwCfg.nonIpKeyType;                 */
    /* lookupHwCfg.ipv4KeyType;                  */
    /* lookupHwCfg.ipv6KeyType;                  */

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        res = prvCpssDxChPclCfgTblHwWrite(
            devNum, portGroupId , direction, entryIndex,
            lookupNum, 1 /*subLookupNum*/, &lookupHwCfg);
        if (res != GT_OK)
        {
            return res;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblSet
(
    IN GT_U8                           devNum,
    IN GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN CPSS_INTERFACE_INFO_STC         *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupCfgTblSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr));

    rc = internal_cpssDxChPclPortGroupCfgTblSet(devNum, portGroupsBmp, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC    lookupHwCfg;
    GT_STATUS res;           /* return code                            */
    GT_U32    entryIndex;    /* index of PCL configuration table entry */
    GT_U32  portGroupId;     /* the port group Id                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(interfaceInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(lookupCfgPtr);
    PRV_CPSS_DXCH_PCL_LOOKUP_NUM_CHECK_MAC(lookupNum);

    /* clear output data */
    cpssOsMemSet(lookupCfgPtr, 0, sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));

    res = prvCpssDxChPclCfgTblEntryIndexGet(
        devNum, interfaceInfoPtr, direction, &entryIndex , &portGroupId);
    if (res != GT_OK)
    {
        return res;
    }

    if (portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        if ((portGroupsBmp  != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
            && (portGroupsBmp != (GT_U32)(1 << portGroupId)))
        {
            /* valid only "All port groups" or the correct port group */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* update bitmap to correct port group */
        portGroupsBmp = (1 << portGroupId);
    }
    else
    {
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
            devNum, portGroupsBmp, portGroupId);
    }

    res = prvCpssDxChPclCfgTblHwRead(
        devNum, portGroupId , direction, entryIndex,
        lookupNum, 0 /*subLookupNum*/, &lookupHwCfg);
    if (res != GT_OK)
    {
        return res;
    }

    /* common for all devices */
    cpssOsMemSet(lookupCfgPtr, 0, sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));
    lookupCfgPtr->enableLookup        = BIT2BOOL_MAC(lookupHwCfg.enableLookup);
    lookupCfgPtr->pclId               = lookupHwCfg.pclId;
    lookupCfgPtr->dualLookup          = BIT2BOOL_MAC(lookupHwCfg.dualLookup);
    lookupCfgPtr->pclIdL01            = lookupHwCfg.pclIdL01;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* nonIpKey */
        switch (lookupHwCfg.nonIpKeyType)
        {
            case 0: /* standard */
                lookupCfgPtr->groupKeyTypes.nonIpKey =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
                break;
            case 1: /* extended */
                lookupCfgPtr->groupKeyTypes.nonIpKey =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG); /* must never occure */
        }

        /* IPV4 */
        switch (lookupHwCfg.nonIpKeyType | (lookupHwCfg.ipv4KeyType << 1))
        {
            case 0: /* standard, IP L2 */
                lookupCfgPtr->groupKeyTypes.ipv4Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
                break;
            case 2: /* standard, IP L4 */
                lookupCfgPtr->groupKeyTypes.ipv4Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
                break;
            default: /* extended, any */
                lookupCfgPtr->groupKeyTypes.ipv4Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
                break;
        }

        /* IPV6 */
        switch (lookupHwCfg.nonIpKeyType
                | (lookupHwCfg.ipv4KeyType << 1)
                | (lookupHwCfg.ipv6KeyType << 2))
        {
            case 0: /* standard, IP L2 */
            case 2: /* standard, IP L4 */
                lookupCfgPtr->groupKeyTypes.ipv6Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
                break;
            case 4: /* standard, IP L2, DIP */
            case 6: /* standard, IP L4, DIP */
                lookupCfgPtr->groupKeyTypes.ipv6Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
                break;
            case 1: /* standard, IP L2, */
            case 5: /* extended, IP L2, DIP - presents, but not works */
                lookupCfgPtr->groupKeyTypes.ipv6Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
                break;
            case 3: /* extended, IP L4 */
            case 7: /* extended, IP L4, DIP - presents, but not works */
                lookupCfgPtr->groupKeyTypes.ipv6Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG); /* must never occur */
        }

        return GT_OK;
    }

    /* CH2 and above code */

    /* non IP key style */
    res = prvCpssDxChPclKeyTypeHwToSw(
        devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS,
        lookupHwCfg.nonIpKeyType,
        &lookupCfgPtr->groupKeyTypes.nonIpKey);
    if (res != GT_OK)
    {
        return res;
    }

    /* IPv4 key style */
    res = prvCpssDxChPclKeyTypeHwToSw(
        devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS,
        lookupHwCfg.ipv4KeyType,
        &lookupCfgPtr->groupKeyTypes.ipv4Key);
    if (res != GT_OK)
    {
        return res;
    }

    /* IPv6 key style */
    res = prvCpssDxChPclKeyTypeHwToSw(
        devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS,
        lookupHwCfg.ipv6KeyType,
        &lookupCfgPtr->groupKeyTypes.ipv6Key);
    if (res != GT_OK)
    {
        return res;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        lookupCfgPtr->udbKeyBitmapEnable = BIT2BOOL_MAC(lookupHwCfg.udbKeyBitmapEnable);

        switch (lookupHwCfg.tcamSegmentMode)
        {
            case 0:
                lookupCfgPtr->tcamSegmentMode = CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_1_TCAM_E;
                break;
            case 1:
                lookupCfgPtr->tcamSegmentMode = CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_2_AND_2_E;
                break;
            case 2:
                lookupCfgPtr->tcamSegmentMode = CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_1_AND_3_E;
                break;
            case 3:
                lookupCfgPtr->tcamSegmentMode = CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_3_AND_1_E;
                break;
            case 4:
                lookupCfgPtr->tcamSegmentMode = CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_4_TCAMS_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    if ((0 == PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        || (direction != CPSS_PCL_DIRECTION_INGRESS_E)
        || (lookupNum != CPSS_PCL_LOOKUP_0_E))
    {
        /* only one configuration table needed retrieve */
        return GT_OK;
    }

    /* xCat2 ingress lookup01 table read                       */
    /* Ingress PCL CPSS_PCL_LOOKUP_0_E - means both sublookups */

    res = prvCpssDxChPclCfgTblHwRead(
        devNum, portGroupId , direction, entryIndex,
        lookupNum, 1 /*subLookupNum*/, &lookupHwCfg);
    if (res != GT_OK)
    {
        return res;
    }

    /* lookup_0_1 related data                    */
    /* lookup_0_1 key types assumed as lookup_0_0 */
    lookupCfgPtr->dualLookup = BIT2BOOL_MAC(lookupHwCfg.enableLookup);
    lookupCfgPtr->pclIdL01   = lookupHwCfg.pclId;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupCfgTblGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr));

    rc = internal_cpssDxChPclPortGroupCfgTblGet(devNum, portGroupsBmp, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, interfaceInfoPtr, direction, lookupNum, lookupCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
{
    GT_STATUS rc;              /* return code                              */
    GT_U32    portGroupId;     /* the port group Id                        */
    PRV_CPSS_DXCH_TABLE_ENT tableId; /* id of ingress of egress Policy Cfg */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pclCfgTblEntryPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if ((PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        && (direction == CPSS_PCL_DIRECTION_INGRESS_E)
        && (lookupNum == CPSS_PCL_LOOKUP_0_E))
    {
        /* xCat2 read entries from both lookup0 and lookup01 tables */

        /* lookup0 */
        tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
        rc = prvCpssDxChPortGroupReadTableEntry(
            devNum, portGroupId, tableId, entryIndex, pclCfgTblEntryPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* lookup01 - next two words */
        tableId = PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
        rc = prvCpssDxChPortGroupReadTableEntry(
            devNum, portGroupId, tableId, entryIndex, &(pclCfgTblEntryPtr[2]));

        return rc;
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat2 and above */
                switch (lookupNum)
                {
                    /* case CPSS_PCL_LOOKUP_0_E treated above */
                    case CPSS_PCL_LOOKUP_0_0_E:
                        tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_0_1_E:
                        tableId =
                            PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_1_E:
                        tableId =
                            PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
                        break;
                    default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat and above */
                switch (lookupNum)
                {
                    case CPSS_PCL_LOOKUP_0_E:
                        tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_1_E:
                        tableId =
                            PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
                        break;
                    default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            else
            {
                /* DxCh1-DxCh3 */
                tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
            }
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
                 == CPSS_PP_FAMILY_CHEETAH_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat and above */
                if (lookupNum != CPSS_PCL_LOOKUP_0_E)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            tableId = PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId, tableId, entryIndex, pclCfgTblEntryPtr);
}

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupCfgTblEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, lookupNum, entryIndex, pclCfgTblEntryPtr));

    rc = internal_cpssDxChPclPortGroupCfgTblEntryGet(devNum, portGroupsBmp, direction, lookupNum, entryIndex, pclCfgTblEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, lookupNum, entryIndex, pclCfgTblEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*    l4PortType        - TCP/UDP port type, source or destination port

*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    directonNum;   /* direction => 0 or 1                 */
    GT_U32    protocolNum ;  /* protocol => 0 or 1                  */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    portGroupId;   /* the port group Id                   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(entryIndex > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E: directonNum = 0; break;
        case CPSS_PCL_DIRECTION_EGRESS_E:  directonNum = 1; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (l4Protocol)
    {
        case CPSS_L4_PROTOCOL_TCP_E: protocolNum = 0; break;
        case CPSS_L4_PROTOCOL_UDP_E: protocolNum = 1; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch ((directonNum << 1) | protocolNum)
    {
        case 0: /* ingress TCP */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr =
                    PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).userDefinedBytes.IPCLTCPPortRangeComparatorConfig[entryIndex];
            }
            else
            {
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    ipclTcpPortComparator0 + (entryIndex * 4);
            }
            break;
        case 1: /* ingress UDP */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr =
                    PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).userDefinedBytes.IPCLUDPPortRangeComparatorConfig[entryIndex];
            }
            else
            {
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    ipclUdpPortComparator0 + (entryIndex * 4);
            }
            break;
        case 2: /* egress TCP */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr =
                    PRV_DXCH_REG1_UNIT_HA_MAC(devNum).egrTCPPortRangeComparatorConfig[entryIndex];
            }
            else
            {
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclTcpPortComparator0 + (entryIndex * 4);
            }
            break;
        case 3: /* egress UDP */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr =
                    PRV_DXCH_REG1_UNIT_HA_MAC(devNum).egrUDPPortRangeComparatorConfig[entryIndex];
            }
            else
            {
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclUdpPortComparator0 + (entryIndex * 4);
            }
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwValue = (value & 0xFFFF);

    switch (l4PortType)
    {
        case CPSS_L4_PROTOCOL_PORT_SRC_E: /* bit18 aleady 0 */  break;
        case CPSS_L4_PROTOCOL_PORT_DST_E: hwValue |= (1 << 18); break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (compareOperator)
    {
        case CPSS_COMPARE_OPERATOR_INVALID_E: /*bit16-17 aleady 0*/ break;
        case CPSS_COMPARE_OPERATOR_LTE:       hwValue |= (2 << 16); break;
        case CPSS_COMPARE_OPERATOR_GTE:       hwValue |= (1 << 16); break;
        case CPSS_COMPARE_OPERATOR_NEQ:       hwValue |= (3 << 16); break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr, 0, 19, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*    l4PortType        - TCP/UDP port type, source or destination port

*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupTcpUdpPortComparatorSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value));

    rc = internal_cpssDxChPclPortGroupTcpUdpPortComparatorSet(devNum, portGroupsBmp, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupTcpUdpPortComparatorGet
*
* DESCRIPTION:
*        Get TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*    l4PortTypePtr      - TCP/UDP port type, source or destination port
*    compareOperatorPtr - compare operator FALSE, LTE, GTE, NEQ
*    valuePtr           - 16 bit value to compare with
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupTcpUdpPortComparatorGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT            direction,
    IN  CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN  GT_U8                             entryIndex,
    OUT CPSS_L4_PROTOCOL_PORT_TYPE_ENT    *l4PortTypePtr,
    OUT CPSS_COMPARE_OPERATOR_ENT         *compareOperatorPtr,
    OUT GT_U16                            *valuePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    directonNum;   /* direction => 0 or 1                 */
    GT_U32    protocolNum ;  /* protocol => 0 or 1                  */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    value;         /* tmp value                           */
    GT_U32    portGroupId;   /* the port group Id                   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(l4PortTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(compareOperatorPtr);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if(entryIndex > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E: directonNum = 0; break;
        case CPSS_PCL_DIRECTION_EGRESS_E:  directonNum = 1; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (l4Protocol)
    {
        case CPSS_L4_PROTOCOL_TCP_E: protocolNum = 0; break;
        case CPSS_L4_PROTOCOL_UDP_E: protocolNum = 1; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch ((directonNum << 1) | protocolNum)
    {
        case 0: /* ingress TCP */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr =
                    PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).userDefinedBytes.IPCLTCPPortRangeComparatorConfig[entryIndex];
            }
            else
            {
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    ipclTcpPortComparator0 + (entryIndex * 4);
            }
            break;
        case 1: /* ingress UDP */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr =
                    PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).userDefinedBytes.IPCLUDPPortRangeComparatorConfig[entryIndex];
            }
            else
            {
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    ipclUdpPortComparator0 + (entryIndex * 4);
            }
            break;
        case 2: /* egress TCP */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr =
                    PRV_DXCH_REG1_UNIT_HA_MAC(devNum).egrTCPPortRangeComparatorConfig[entryIndex];
            }
            else
            {
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclTcpPortComparator0 + (entryIndex * 4);
            }
            break;
        case 3: /* egress UDP */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr =
                    PRV_DXCH_REG1_UNIT_HA_MAC(devNum).egrUDPPortRangeComparatorConfig[entryIndex];
            }
            else
            {
                regAddr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclUdpPortComparator0 + (entryIndex * 4);
            }
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpPortGroupGetRegField(
        devNum, portGroupId, regAddr, 0, 19, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *valuePtr = (GT_U16)(hwValue & 0xFFFF);

    /* get value from hwValue */
    value = (hwValue >> 18) & 0x1;

    *l4PortTypePtr = (0 == value) ? CPSS_L4_PROTOCOL_PORT_SRC_E :
                                    CPSS_L4_PROTOCOL_PORT_DST_E;

    /* get compareOperator from hwValue */
    value = (hwValue >> 16) & 0x3;

    switch (value)
    {
        case 0:
          *compareOperatorPtr = CPSS_COMPARE_OPERATOR_INVALID_E;
          break;
        case 2:
          *compareOperatorPtr = CPSS_COMPARE_OPERATOR_LTE;
          break;
        case 1:
          *compareOperatorPtr = CPSS_COMPARE_OPERATOR_GTE;
          break;
        case 3:
          *compareOperatorPtr = CPSS_COMPARE_OPERATOR_NEQ;
          break;
        default:
          CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupTcpUdpPortComparatorGet
*
* DESCRIPTION:
*        Get TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*    l4PortTypePtr      - TCP/UDP port type, source or destination port
*    compareOperatorPtr - compare operator FALSE, LTE, GTE, NEQ
*    valuePtr           - 16 bit value to compare with
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupTcpUdpPortComparatorGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT            direction,
    IN  CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN  GT_U8                             entryIndex,
    OUT CPSS_L4_PROTOCOL_PORT_TYPE_ENT    *l4PortTypePtr,
    OUT CPSS_COMPARE_OPERATOR_ENT         *compareOperatorPtr,
    OUT GT_U16                            *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupTcpUdpPortComparatorGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, l4Protocol, entryIndex, l4PortTypePtr, compareOperatorPtr, valuePtr));

    rc = internal_cpssDxChPclPortGroupTcpUdpPortComparatorGet(devNum, portGroupsBmp, direction, l4Protocol, entryIndex, l4PortTypePtr, compareOperatorPtr, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, l4Protocol, entryIndex, l4PortTypePtr, compareOperatorPtr, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* lion3PclUdbOffsetTypeSw2Hw
*
* DESCRIPTION:
*   The function converts UDB OffsetType SW to HW.
*
* APPLICABLE DEVICES:
*        Bobcat2, Caelum, Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;.
*
* INPUTS:
*       swOffsetType   - packet Type
*
* OUTPUTS:
*       entryIndexPtr    - (pointer to) User Defined Byte Entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3PclUdbOffsetTypeSw2Hw
(
    IN  CPSS_DXCH_PCL_OFFSET_TYPE_ENT        swOffsetType,
    OUT GT_U32                               *hwOffsetTypePtr
)
{
    if (swOffsetType >=
        (sizeof(lion3ConvertSwUdbAnchorToHwValueArr) / sizeof(lion3ConvertSwUdbAnchorToHwValueArr[0])))
    {
        /* check for access violation into the array of
            lion3ConvertSwUdbAnchorToHwValueArr[] */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *hwOffsetTypePtr = (GT_U32)(lion3ConvertSwUdbAnchorToHwValueArr[swOffsetType]);

    if (*hwOffsetTypePtr == LION3_HW_PCL_UDB_ANCHOR_NOT_SUPPORTED_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* lion3PclPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB).
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       packetType   - packet Type
*       direction    - Policy direction:
*                      Ingress or Egress.
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3PclPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN CPSS_PCL_DIRECTION_ENT               direction,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    GT_STATUS rc;              /* return code              */
    GT_U32    regValue;        /* register  value*/
    GT_U32    hwAnchor;        /* HW offset Type           */
    GT_U32    startBit;        /* start Bit                */
    GT_U32    udbEntryIndex;   /* XCat HW UDB entry index  */
    GT_U32    portGroupId;     /*the port group Id         */
    GT_U32    numBitsInfo;/* number of bits of info per UDB*/
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    if(direction != CPSS_PCL_DIRECTION_INGRESS_E &&
       direction != CPSS_PCL_DIRECTION_EGRESS_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Calculate entry index by packet type */
    rc = lion3PclUDBEntryIndexGet(devNum, packetType, &udbEntryIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = lion3PclUdbOffsetTypeSw2Hw(offsetType, &hwAnchor);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((udbIndex >= lion3UdbRangePerAnchorArr[offsetType].maxUdb)
        || (udbIndex < lion3UdbRangePerAnchorArr[offsetType].lowUdb))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {

        /*
            bit 0 - valid
            bits 1..3 - Anchor
            bits 4..10 - offset (7 bits)
        */
        tableType = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E;
        numBitsInfo = 11;/* 11 bits per UDB */
        if (offset > lion3UdbRangePerAnchorArr[offsetType].ingrMaxOffset)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /*
            bit 0 - valid
            bits 1..3 - Anchor
            bits 4..11 - offset (8 bits)
        */
        tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_EPCL_UDB_CONFIG_E;
        numBitsInfo = 12;/* 12 bits per UDB */
        if (offset > lion3UdbRangePerAnchorArr[offsetType].egrMaxOffset)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }

    regValue = (offset << 4);

    /*
         type of the offset for UDB:
         0x0 =  L2; Offset is defined from the beginning of the outgoing passenger MAC header (if no tunnel start, then it refers to the full outgoing packet header);
         0x1 =  MPLS; Offset is defined from start of MPLS header - 2.;
         0x2 =  L3; Offset is defined from the beginning of the outgoing passenger L3 header (according to L3 offset. if no tunnel start, then it refers to the L3 of the full outgoing packet header);
         0x3 =  L4; Offset is defined from the beginning of the outgoing passenger L4 header (if no tunnel start, then it refers to the L4 of the full outgoing packet header);
         0x4 = Tunnel L2; Tunnel_L2; Refers to the beginning of the outgoing packet’s MAC header (after tunnel-start).
                If packet was not tunnel-start this anchor is invalid;
         0x5 = Tunnel L3; Tunnel_L3; Same as ‘L3’ anchor but for the outgoing packet’s header (after to tunnel-start).
                If packet was not tunnel-start this anchor is invalid;
         0x6 = Metadata; Calculated fields;
    */

    regValue |= (hwAnchor << 1);
    if (offsetType != CPSS_DXCH_PCL_OFFSET_INVALID_E)
    {
        regValue |= 1; /* bit 0 means "valid" */
    }

    startBit = numBitsInfo * udbIndex;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc =  prvCpssDxChPortGroupWriteTableEntryField(
            devNum, portGroupId,
            tableType,
            udbEntryIndex,
            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
            startBit,
            numBitsInfo,
            regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)


    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN CPSS_PCL_DIRECTION_ENT               direction,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    GT_STATUS rc;              /* return code              */
    GT_U32    regAddr;         /* register address         */
    GT_U32    regMask;         /* mask to update register  */
    GT_U32    regValue;        /* value to update register */
    GT_U32    startBit;        /* start Bit                */
    GT_U32    validTypesMask;  /* valid Types Mask         */
    GT_U32    comparatorBit;   /* comparator Bit           */
    GT_U32    hwOffType;       /* offset type in HW format */
    GT_U32    udbEntryIndex;   /* XCat HW UDB entry index  */
    GT_U32    portGroupId;     /*the port group Id         */
    GT_U32    udbIntIndex;     /*index of UDB in the entry */
    GT_U32    numBitsInfo;/* number of bits of info per UDB*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return lion3PclPortGroupUserDefinedByteSet(devNum,portGroupsBmp,packetType,direction,
                udbIndex,offsetType,offset);
    }

    if (offset >= BIT_7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* for all Cheetah devices */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChCheetahPclUDBRegGet(
            devNum, ruleFormat, udbIndex,
            &regAddr, &startBit, &validTypesMask, &comparatorBit);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* The updates gathered in "regMask" and "regValue" step by step.        */
        regMask = regValue = 0;

        /* special case of comparator */
        if (offsetType == CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E)
        {
            if (comparatorBit > 31)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* set "comparator" bit to 1 */
            regMask  |= (1 << comparatorBit) ;
            regValue |= (1 << comparatorBit) ;
        }
        else
        {
            /* drop comparator if was */
            if (comparatorBit <= 31)
            {
                /* set "comparator" bit to 0 */
                regMask |= (1 << comparatorBit);
                /* regValue assumed == 0 */
            }

            switch (offsetType)
            {
                case CPSS_DXCH_PCL_OFFSET_L2_E:
                    hwOffType = UDB_HW_L2_CNS;
                    break;
                case CPSS_DXCH_PCL_OFFSET_L3_E:
                    hwOffType = UDB_HW_L3_CNS;
                    break;
                case CPSS_DXCH_PCL_OFFSET_L4_E:
                    hwOffType = UDB_HW_L4_CNS;
                    break;
                case CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E:
                    hwOffType = UDB_HW_IPV6EH_CNS;
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if ((validTypesMask & (1 << hwOffType)) == 0)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* set offset-type and offset */
            regMask  |= (0x1FF << startBit);
            regValue |= (((hwOffType << 7) | offset) << startBit);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(
                devNum, portGroupId, regAddr, regMask, regValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* xCat and above devices */

    regValue = (offset << 3);

    /* bit 0 means "valid", bit 10 - "comparator" */
    switch (offsetType)
    {
        case CPSS_DXCH_PCL_OFFSET_L2_E:
            regValue |= (UDB_XCAT_HW_L2_CNS << 1) | 1;
            break;
        case CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E:
            regValue |= (UDB_XCAT_HW_L3_CNS << 1) | 1;
            break;
        case CPSS_DXCH_PCL_OFFSET_L4_E:
            regValue |= (UDB_XCAT_HW_L4_CNS << 1) | 1;
            break;
        case CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E:
            regValue |= (UDB_XCAT_HW_MPLS_CNS << 1) | 1;
            break;
        case CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E:
            regValue |= (UDB_XCAT_HW_L2_CNS << 1) | 1 | (1 << 10);
            break;
        case CPSS_DXCH_PCL_OFFSET_INVALID_E:
            /* L2 without valid bit*/
            regValue |= (UDB_XCAT_HW_L2_CNS << 1);
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    /* xCat and above up to Lion2 UDBs - direction ignored */
    /* Bobcat2; Caelum; Bobcat3 ingress PCL UDBs                               */

    /* Calculate entry index by packet type */
    rc = prvCpssDxChXCatPclUDBEntryIndexGet(
        devNum, packetType, udbIndex, &udbEntryIndex, &udbIntIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    numBitsInfo = 11;/* 11 bits per UDB */
    startBit = numBitsInfo * udbIntIndex;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* set the UDB info */
        rc =  prvCpssDxChPortGroupWriteTableEntryField(
            devNum, portGroupId,
            PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
            udbEntryIndex,
            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
            startBit,
            numBitsInfo,
            regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN CPSS_PCL_DIRECTION_ENT               direction,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupUserDefinedByteSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleFormat, packetType, direction, udbIndex, offsetType, offset));

    rc = internal_cpssDxChPclPortGroupUserDefinedByteSet(devNum, portGroupsBmp, ruleFormat, packetType, direction, udbIndex, offsetType, offset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleFormat, packetType, direction, udbIndex, offsetType, offset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* lion3PclPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId - bitmap of Port Groups or CPSS_PORT_GROUP_UNAWARE_MODE_CNS.
*       packetType   - packet Type
*       direction    - Policy direction:
*                      Ingress or Egress.
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3PclPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_U32                               portGroupId,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_DIRECTION_ENT               direction,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
{
    GT_STATUS rc;              /* return code              */
    GT_U32    regValue;        /* register  value*/
    GT_U32    startBit;        /* start Bit                */
    GT_U32    udbEntryIndex;   /* XCat HW UDB entry index  */
    GT_U32    numBitsInfo;/* number of bits of info per UDB*/
    GT_U32    i;               /* loop index               */
    GT_U32    numOfTypes;      /* num Of Offset Types      */
    GT_U32    hwAnchor;        /* HW OffsetType            */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    if(direction != CPSS_PCL_DIRECTION_INGRESS_E &&
       direction != CPSS_PCL_DIRECTION_EGRESS_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (udbIndex >= CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Calculate entry index by packet type */
    rc = lion3PclUDBEntryIndexGet(devNum, packetType, &udbEntryIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        /*
            bit 0 - valid
            bits 1..3 - Anchor
            bits 4..10 - offset (7 bits)
        */
        tableType = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E;
        numBitsInfo = 11;/* 11 bits per UDB */
    }
    else
    {
        /*
            bit 0 - valid
            bits 1..3 - Anchor
            bits 4..11 - offset (8 bits)
        */
        tableType = PRV_CPSS_DXCH_LION3_TABLE_HA_EPCL_UDB_CONFIG_E;
        numBitsInfo = 12;/* 12 bits per UDB */
    }

    startBit = numBitsInfo * udbIndex;

    rc =  prvCpssDxChPortGroupReadTableEntryField(
        devNum, portGroupId,
        tableType,
        udbEntryIndex,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        startBit,
        numBitsInfo,
        &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *offsetPtr = (GT_U8) U32_GET_FIELD_MAC(regValue,4,8);

    if (U32_GET_FIELD_MAC(regValue,0,1) == 0)
    {
        /* invalid UDB */
        *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_INVALID_E;
        return GT_OK;
    }

    /*
         type of the offset for UDB:
         0x0 =  L2; Offset is defined from the beginning of the outgoing passenger MAC header (if no tunnel start, then it refers to the full outgoing packet header);
         0x1 =  MPLS; Offset is defined from start of MPLS header - 2.;
         0x2 =  L3; Offset is defined from the beginning of the outgoing passenger L3 header (according to L3 offset. if no tunnel start, then it refers to the L3 of the full outgoing packet header);
         0x3 =  L4; Offset is defined from the beginning of the outgoing passenger L4 header (if no tunnel start, then it refers to the L4 of the full outgoing packet header);
         0x4 = Tunnel L2; Tunnel_L2; Refers to the beginning of the outgoing packet’s MAC header (after tunnel-start).
                If packet was not tunnel-start this anchor is invalid;
         0x5 = Tunnel L3; Tunnel_L3; Same as ‘L3’ anchor but for the outgoing packet’s header (after to tunnel-start).
                If packet was not tunnel-start this anchor is invalid;
         0x6 = Metadata; Calculated fields;
    */

    hwAnchor = U32_GET_FIELD_MAC(regValue,1,3);
    numOfTypes =
        (sizeof(lion3ConvertSwUdbAnchorToHwValueArr) /
         sizeof(lion3ConvertSwUdbAnchorToHwValueArr[0]));

    for (i = 0; (i < numOfTypes); i++)
    {
        if (hwAnchor == (GT_U32)lion3ConvertSwUdbAnchorToHwValueArr[i])
        {
            *offsetTypePtr = (CPSS_DXCH_PCL_OFFSET_TYPE_ENT)i;
            break;
        }
    }

    if (i >= numOfTypes)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_DIRECTION_ENT               direction,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
{
    GT_STATUS rc;               /* return code              */
    GT_U32    regAddr;          /* register address         */
    GT_U32    regMask;          /* mask to update register  */
    GT_U32    regValue;         /* value to update register */
    GT_U32    startBit;         /* start Bit                */

    GT_U32    validTypesMask;   /* valid Types Mask         */
    GT_U32    comparatorBit;    /* comparator Bit           */
    GT_U32    udbEntryIndex;    /* XCat HW UDB entry index  */
    GT_U32    portGroupId;      /*the port group Id         */
    GT_U32    udbIntIndex;      /*index of UDB in the entry */
    GT_U32    numBitsInfo;/* number of bits of info per UDB*/

    /* temporary unused parametres */
    (GT_VOID)direction;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(offsetTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return lion3PclPortGroupUserDefinedByteGet(devNum,portGroupId,packetType,direction,
                udbIndex,offsetTypePtr,offsetPtr);
    }

    /* for all Cheetah devices */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChCheetahPclUDBRegGet(
            devNum, ruleFormat, udbIndex,
            &regAddr, &startBit, &validTypesMask, &comparatorBit);
        if (rc != GT_OK)
        {
            return rc;
        }

        regMask  = (0x1FF << startBit);
        if (comparatorBit < 32)
        {
            regMask  |= (1 << comparatorBit) ;
        }

        rc = prvCpssDrvHwPpPortGroupReadRegBitMask(
            devNum, portGroupId, regAddr, regMask, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        *offsetPtr = (GT_U8)((regValue >> startBit) & 0x7F);

        if ((comparatorBit < 32) && ((regValue & (1 << comparatorBit)) != 0))
        {
            *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E;
        }
        else
        {
            switch ((regValue >> (startBit + 7)) & 3)
            {
                case UDB_HW_L2_CNS:
                    *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L2_E;
                    break;
                case UDB_HW_L3_CNS:
                    *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L3_E;
                    break;
                case UDB_HW_L4_CNS:
                    *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L4_E;
                    break;
                case UDB_HW_IPV6EH_CNS:
                    *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
                    break;
                /* cannot occur */
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }

        return GT_OK;
    }

    /* xCat & xCat3 devices */

    /* xCat and above up to Lion2 UDBs - direction ignored */
    /* Bobcat2; Caelum; Bobcat3 ingress PCL UDBs                               */

    /* Calculate entry index by packet type */
    rc = prvCpssDxChXCatPclUDBEntryIndexGet(
        devNum, packetType, udbIndex, &udbEntryIndex, &udbIntIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    numBitsInfo = 11;/* 11 bits per UDB */
    startBit = numBitsInfo * udbIntIndex;

    /* get the UDB info */
    rc =  prvCpssDxChPortGroupReadTableEntryField(
        devNum, portGroupId,
        PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
        udbEntryIndex,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        startBit,
        numBitsInfo,
        &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *offsetPtr = (GT_U8)((regValue >> 3) & 0x7F);

    if ((regValue & 1) == 0)
    {
        /* invalid UDB */
        *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_INVALID_E;
            return GT_OK;
        }

    if ((regValue & (1 << 10)) != 0)
    {
        /* TCP/UDB comparator */
        *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E;
        return GT_OK;
    }

    switch ((regValue >> 1) & 3)
    {
         case UDB_XCAT_HW_L2_CNS:
             *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L2_E;
             break;
         case UDB_XCAT_HW_L3_CNS:
             *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E;
             break;
         case UDB_XCAT_HW_L4_CNS:
             *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L4_E;
             break;
         case UDB_XCAT_HW_MPLS_CNS:
             *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E;
             break;
         /* cannot occur */
         default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
     }

     return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_DIRECTION_ENT               direction,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupUserDefinedByteGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleFormat, packetType, direction, udbIndex, offsetTypePtr, offsetPtr));

    rc = internal_cpssDxChPclPortGroupUserDefinedByteGet(devNum, portGroupsBmp, ruleFormat, packetType, direction, udbIndex, offsetTypePtr, offsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleFormat, packetType, direction, udbIndex, offsetTypePtr, offsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* lion3PclUDBSelectEntryIndexGet
*
* DESCRIPTION:
*   Get the User Defined Byte (UDB) Selection Configuration entry index.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*
* OUTPUTS:
*       indexPtr     - (pointer to) entry index.
*       egressPtr    - (pointer to) GT_TRUE - Egress PCL, GT_FALSE - Ingress PCL.
*       tableTypePtr - (pointer to) table Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS lion3PclUDBSelectEntryIndexGet
(
    IN GT_U8                                 devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    OUT GT_U32                               *indexPtr,
    OUT GT_BOOL                              *egressPtr,
    OUT PRV_CPSS_DXCH_TABLE_ENT              *tableTypePtr
)
{
    GT_STATUS rc;              /* return code      */
    GT_U32    packetTypeIndex; /* packet Type Index */
    GT_U32    keyTypeIndex;    /* key Type Index    */

    /* Calculate entry index by packet type */
    rc = lion3PclUDBEntryIndexGet(devNum, packetType, &packetTypeIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E:
            *egressPtr = GT_FALSE;
            keyTypeIndex = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E:
            *egressPtr = GT_FALSE;
            keyTypeIndex = 1;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E:
            *egressPtr = GT_FALSE;
            keyTypeIndex = 2;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E:
            *egressPtr = GT_FALSE;
            keyTypeIndex = 3;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E:
            *egressPtr = GT_FALSE;
            keyTypeIndex = 4;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E:
            *egressPtr = GT_FALSE;
            keyTypeIndex = 5;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E:
            *egressPtr = GT_TRUE;
            keyTypeIndex = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E:
            *egressPtr = GT_TRUE;
            keyTypeIndex = 1;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E:
            *egressPtr = GT_TRUE;
            keyTypeIndex = 2;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E:
            *egressPtr = GT_TRUE;
            keyTypeIndex = 3;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E:
            *egressPtr = GT_TRUE;
            keyTypeIndex = 4;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E:
            *egressPtr = GT_TRUE;
            keyTypeIndex = 5;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *indexPtr = (16 * keyTypeIndex) + packetTypeIndex;
    if (*egressPtr == GT_FALSE)
    {
        /* ingress */
        switch (lookupNum)
        {
            case CPSS_PCL_LOOKUP_0_E:
            case CPSS_PCL_LOOKUP_0_0_E:
                *tableTypePtr = PRV_CPSS_DXCH_LION3_TABLE_IPCL0_UDB_SELECT_E;
                break;
            case CPSS_PCL_LOOKUP_0_1_E:
                *tableTypePtr = PRV_CPSS_DXCH_LION3_TABLE_IPCL1_UDB_SELECT_E;
                break;
            case CPSS_PCL_LOOKUP_1_E:
                *tableTypePtr = PRV_CPSS_DXCH_LION3_TABLE_IPCL2_UDB_SELECT_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* egress */
        *tableTypePtr = PRV_CPSS_DXCH_LION3_TABLE_EPCL_UDB_SELECT_E;
    }

    return GT_OK;
}

/*******************************************************************************
* sip5UdbSelectionEntryFormatInfoGet
*
* DESCRIPTION:
*   info about field format in the IPCL/EPCL UDB Select Entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       isEgress     - indication if EPCL or IPCL
* OUTPUTS:
*       numMaxUdbsPtr         - (pointer to) max number of UDBs
*       numBitsPerUdbPtr      - (pointer to) number of bits for field of :
*                               The decimal UDB number in the global UDB pool
*                               that is selected as UDB #n in this UDB key.
*       startBitsAfterUdbsPtr - (pointer to) bit index after the UDBs.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
static void sip5UdbSelectionEntryFormatInfoGet(
    IN   GT_U8    devNum,
    IN   GT_BOOL  isEgress,
    OUT  GT_U32   *numMaxUdbsPtr,
    OUT  GT_U32   *numBitsPerUdbPtr,
    OUT  GT_U32   *startBitsAfterUdbsPtr
)
{
    GT_U32  numMaxUdbs    ;    /*max number of UDBs*/
    GT_U32  numBitsPerUdb ;    /*number of bits for field of :
                               The decimal UDB number in the global UDB pool
                               that is selected as UDB #n in this UDB key.*/
    GT_U32  startBitsAfterUdbs; /*bit index after the UDBs.*/

    if(isEgress == GT_TRUE)
    {
        numMaxUdbs    = 50;
        numBitsPerUdb = 6;
    }
    else
    {
        numMaxUdbs    = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 70 : 50;
        numBitsPerUdb = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ?  7 : 6;
    }

    startBitsAfterUdbs = (numMaxUdbs * numBitsPerUdb);


    *numMaxUdbsPtr          = numMaxUdbs;
    *numBitsPerUdbPtr       = numBitsPerUdb;
    *startBitsAfterUdbsPtr  = startBitsAfterUdbs;

    return ;
}

/*******************************************************************************
* prvCpssDxChPclPortGroupUserDefinedBytesSelectSet
*
* DESCRIPTION:
*   Set the User Defined Byte (UDB) Selection Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*                      Relevant values are:
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*       udbSelectPtr - (pointer to) structure with UDB Selection configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclPortGroupUserDefinedBytesSelectSet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    IN  CPSS_DXCH_PCL_UDB_SELECT_STC         *udbSelectPtr
)
{
    GT_STATUS                rc;          /* return code                          */
    GT_U32                   index;       /* table entry index                    */
    GT_BOOL                  egress;      /* GT_TRUE - egress, GT_FALSE - ingress */
    PRV_CPSS_DXCH_TABLE_ENT  tableType;   /* table Id                             */
    GT_U32                   hwData[UDB_SELECTION_WORDS];  /* data in HW format   */
    GT_U32                   i;           /* loop iterator                        */
    GT_U32                   offset;      /* offset in HW                         */
    GT_U32                   portGroupId; /* the port group Id                    */
    GT_U32  numMaxUdbs    ;    /*max number of UDBs*/
    GT_U32  numBitsPerUdb ;    /*number of bits for field of :
                               The decimal UDB number in the global UDB pool
                               that is selected as UDB #n in this UDB key.*/
    GT_U32  startBitsAfterUdbs; /*bit index after the UDBs.*/

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(udbSelectPtr);

    for (i = 0; (i < CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS); i++)
    {
        if (udbSelectPtr->udbSelectArr[i] >= CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }

    rc = lion3PclUDBSelectEntryIndexGet(
        devNum, ruleFormat, packetType, lookupNum,
        &index, &egress, &tableType);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get info about the format of the entry */
    sip5UdbSelectionEntryFormatInfoGet(devNum,egress,
            &numMaxUdbs,&numBitsPerUdb,
            &startBitsAfterUdbs);

    cpssOsMemSet(hwData, 0, sizeof(hwData));

    for (i = 0; (i < CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS); i++)
    {
        offset = (numBitsPerUdb * i);
        U32_SET_FIELD_IN_ENTRY_MAC(
            hwData, offset, numBitsPerUdb/*length*/, udbSelectPtr->udbSelectArr[i]);
    }

    if (egress == GT_FALSE)
    {
        /* ingress */
        for (i = 0; (i < CPSS_DXCH_PCL_INGRESS_UDB_REPLACE_MAX_CNS); i++)
        {
            offset = (startBitsAfterUdbs + i);
            U32_SET_FIELD_IN_ENTRY_MAC(
                hwData, offset, 1/*length*/,
                BOOL2BIT_MAC(udbSelectPtr->ingrUdbReplaceArr[i]));
        }
    }
    else
    {
        /* egress */
        offset = startBitsAfterUdbs;
        U32_SET_FIELD_IN_ENTRY_MAC(
            hwData, offset, 1/*length*/,
            BOOL2BIT_MAC(udbSelectPtr->egrUdb01Replace));
        offset = startBitsAfterUdbs + 1;
        U32_SET_FIELD_IN_ENTRY_MAC(
            hwData, offset, 1/*length*/,
            BOOL2BIT_MAC(udbSelectPtr->egrUdbBit15Replace));
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDxChPortGroupWriteTableEntry(
            devNum, portGroupId, tableType,
            index, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPclUserDefinedBytesSelectSet
*
* DESCRIPTION:
*   Set the User Defined Byte (UDB) Selection Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*                      Relevant values are:
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*       udbSelectPtr - (pointer to) structure with UDB Selection configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclUserDefinedBytesSelectSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    IN  CPSS_DXCH_PCL_UDB_SELECT_STC         *udbSelectPtr
)
{
    return prvCpssDxChPclPortGroupUserDefinedBytesSelectSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, packetType, lookupNum, udbSelectPtr);
}

/*******************************************************************************
* cpssDxChPclUserDefinedBytesSelectSet
*
* DESCRIPTION:
*   Set the User Defined Byte (UDB) Selection Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*                      Relevant values are:
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*       udbSelectPtr - (pointer to) structure with UDB Selection configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedBytesSelectSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    IN  CPSS_DXCH_PCL_UDB_SELECT_STC         *udbSelectPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclUserDefinedBytesSelectSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleFormat, packetType, lookupNum, udbSelectPtr));

    rc = internal_cpssDxChPclUserDefinedBytesSelectSet(devNum, ruleFormat, packetType, lookupNum, udbSelectPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleFormat, packetType, lookupNum, udbSelectPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclPortGroupUserDefinedBytesSelectGet
*
* DESCRIPTION:
*   Get the User Defined Byte (UDB) Selection Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*                      Relevant values are:
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*
* OUTPUTS:
*       udbSelectPtr - (pointer to) structure with UDB Selection configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on unsupported HW state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclPortGroupUserDefinedBytesSelectGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    OUT CPSS_DXCH_PCL_UDB_SELECT_STC         *udbSelectPtr
)
{
    GT_STATUS                rc;          /* return code                          */
    GT_U32                   index;       /* table entry index                    */
    GT_BOOL                  egress;      /* GT_TRUE - egress, GT_FALSE - ingress */
    PRV_CPSS_DXCH_TABLE_ENT  tableType;   /* table Id                             */
    GT_U32                   entryBuf[UDB_SELECTION_WORDS];  /* data in HW format   */
    GT_U32                   i;           /* loop iterator                        */
    GT_U32                   offset;      /* offset in HW                         */
    GT_U32                   portGroupId; /* the port group Id                    */
    GT_U32                   data;        /* HW field value                       */
    GT_U32  numMaxUdbs    ;    /*max number of UDBs*/
    GT_U32  numBitsPerUdb ;    /*number of bits for field of :
                               The decimal UDB number in the global UDB pool
                               that is selected as UDB #n in this UDB key.*/
    GT_U32  startBitsAfterUdbs; /*bit index after the UDBs.*/

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(udbSelectPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    cpssOsMemSet(udbSelectPtr, 0, sizeof(*udbSelectPtr));

    rc = lion3PclUDBSelectEntryIndexGet(
        devNum, ruleFormat, packetType, lookupNum,
        &index, &egress, &tableType);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get info about the format of the entry */
    sip5UdbSelectionEntryFormatInfoGet(devNum,egress,
            &numMaxUdbs,&numBitsPerUdb,
            &startBitsAfterUdbs);

    rc = prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId, tableType,
        index, entryBuf);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0; (i < CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS); i++)
    {
        offset = (numBitsPerUdb * i);
        U32_GET_FIELD_IN_ENTRY_MAC(
            entryBuf, offset, numBitsPerUdb/*length*/, udbSelectPtr->udbSelectArr[i]);
    }

    if (egress == GT_FALSE)
    {
        /* ingress */

        for (i = 0; (i < CPSS_DXCH_PCL_INGRESS_UDB_REPLACE_MAX_CNS); i++)
        {
            offset = (startBitsAfterUdbs + i);
            U32_GET_FIELD_IN_ENTRY_MAC(
                entryBuf, offset, 1/*length*/, data);
            udbSelectPtr->ingrUdbReplaceArr[i] = BIT2BOOL_MAC(data);
        }
    }
    else
    {
        /* egress */

        offset = startBitsAfterUdbs;
        U32_GET_FIELD_IN_ENTRY_MAC(
            entryBuf, offset, 1/*length*/, data);
        udbSelectPtr->egrUdb01Replace = BIT2BOOL_MAC(data);

        offset = startBitsAfterUdbs + 1;
        U32_GET_FIELD_IN_ENTRY_MAC(
            entryBuf, offset, 1/*length*/, data);
        udbSelectPtr->egrUdbBit15Replace = BIT2BOOL_MAC(data);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPclUserDefinedBytesSelectGet
*
* DESCRIPTION:
*   Get the User Defined Byte (UDB) Selection Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*                      Relevant values are:
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*
* OUTPUTS:
*       udbSelectPtr - (pointer to) structure with UDB Selection configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on unsupported HW state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclUserDefinedBytesSelectGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    OUT CPSS_DXCH_PCL_UDB_SELECT_STC         *udbSelectPtr
)
{
    return prvCpssDxChPclPortGroupUserDefinedBytesSelectGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, packetType, lookupNum, udbSelectPtr);
}

/*******************************************************************************
* cpssDxChPclUserDefinedBytesSelectGet
*
* DESCRIPTION:
*   Get the User Defined Byte (UDB) Selection Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*                      Relevant values are:
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*
* OUTPUTS:
*       udbSelectPtr - (pointer to) structure with UDB Selection configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on unsupported HW state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedBytesSelectGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    OUT CPSS_DXCH_PCL_UDB_SELECT_STC         *udbSelectPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclUserDefinedBytesSelectGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleFormat, packetType, lookupNum, udbSelectPtr));

    rc = internal_cpssDxChPclUserDefinedBytesSelectGet(devNum, ruleFormat, packetType, lookupNum, udbSelectPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleFormat, packetType, lookupNum, udbSelectPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  GT_PORT_GROUPS_BMP                         portGroupsBmp,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    portGroupId;   /*the port group Id                    */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    switch (vidUpMode)
    {
        case CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E:
            hwValue = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr, 21, 1, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  GT_PORT_GROUPS_BMP                         portGroupsBmp,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, vidUpMode));

    rc = internal_cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(devNum, portGroupsBmp, vidUpMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, vidUpMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    IN   GT_PORT_GROUPS_BMP                         portGroupsBmp,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    portGroupId;   /*the port group Id                    */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(vidUpModePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    rc = prvCpssHwPpPortGroupGetRegField(
        devNum, portGroupId, regAddr, 21, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *vidUpModePtr =
        (hwValue == 0)
           ? CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E
           : CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    IN   GT_PORT_GROUPS_BMP                         portGroupsBmp,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, vidUpModePtr));

    rc = internal_cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(devNum, portGroupsBmp, vidUpModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, vidUpModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclLookupCfgPortListEnableRegAddrAndPosGet
*
* DESCRIPTION:
*        The function gets register address and bit position
*        for enable bit of using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*       regAddrPtr     - (pointer to) register address.
*       bitPosPtr      - (pointer to) bit position.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclLookupCfgPortListEnableRegAddrAndPosGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_U32                        *regAddrPtr,
    OUT GT_U32                        *bitPosPtr
)
{
    GT_STATUS rc; /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
        CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);

    rc = prvCpssDxChPclLookupParamCheck(
        devNum, direction, lookupNum, subLookupNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        /* ingress PCL */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            *regAddrPtr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineConfig;
        }
        else
        {
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineConfig;
        }

        switch (lookupNum)
        {
            case CPSS_PCL_LOOKUP_0_E:
                *bitPosPtr = ((subLookupNum == 0) ? 14 : 15);
                break;
            case CPSS_PCL_LOOKUP_1_E:
                *bitPosPtr = 16;
                break;
            case CPSS_PCL_LOOKUP_0_0_E:
                *bitPosPtr = 14;
                break;
            case CPSS_PCL_LOOKUP_0_1_E:
                *bitPosPtr = 15;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* egress PCL */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            *regAddrPtr =
                PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
            *bitPosPtr = 5;
        }
        else
        {
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
            *bitPosPtr = 27;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
{
    GT_STATUS    rc;            /* returned code               */
    GT_U32       data;          /* register data               */
    GT_U32       regAddr;       /* register address            */
    GT_U32       bitPos;        /* bit position                */
    GT_U32       portGroupId;   /*the port group Id            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
        CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);

    rc = prvCpssDxChPclLookupCfgPortListEnableRegAddrAndPosGet(
        devNum, direction, lookupNum, subLookupNum,
        &regAddr, &bitPos);
    if (rc != GT_OK)
    {
        return rc;
    }

    data = (enable == GT_FALSE) ? 0 : 1;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr, bitPos, 1, data);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupLookupCfgPortListEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, lookupNum, subLookupNum, enable));

    rc = internal_cpssDxChPclPortGroupLookupCfgPortListEnableSet(devNum, portGroupsBmp, direction, lookupNum, subLookupNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, lookupNum, subLookupNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS    rc;            /* returned code               */
    GT_U32       data;          /* register data               */
    GT_U32       regAddr;       /* register address            */
    GT_U32       bitPos;        /* bit position                */
    GT_U32       portGroupId;   /*the port group Id            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
        CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    rc = prvCpssDxChPclLookupCfgPortListEnableRegAddrAndPosGet(
        devNum, direction, lookupNum, subLookupNum,
        &regAddr, &bitPos);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpPortGroupGetRegField(
        devNum, portGroupId, regAddr, bitPos, 1, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = ((data != 0) ? GT_TRUE : GT_FALSE);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupLookupCfgPortListEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, lookupNum, subLookupNum, enablePtr));

    rc = internal_cpssDxChPclPortGroupLookupCfgPortListEnableGet(devNum, portGroupsBmp, direction, lookupNum, subLookupNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, lookupNum, subLookupNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclL3L4ParsingOverMplsEnableSet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclL3L4ParsingOverMplsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_BOOL                            enable
)
{
    GT_U32       data;      /* register data               */
    GT_U32       regAddr;   /* register address            */
    GT_U32       bitNum;    /* bit number                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    data = (enable == GT_FALSE) ? 0 : 1;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
#ifdef GM_USED
        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            /* the GM not implemented the functionality behind this field , so we not allow to set it */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
        }
#endif

        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;
        bitNum = 27;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 23;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, bitNum, 1, data);
}

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableSet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_BOOL                            enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclL3L4ParsingOverMplsEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPclL3L4ParsingOverMplsEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPclL3L4ParsingOverMplsEnableGet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
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
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclL3L4ParsingOverMplsEnableGet
(
    IN  GT_U8                              devNum,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_U32       data;      /* register data               */
    GT_STATUS    rc;        /* return code                 */
    GT_U32       regAddr;   /* register address            */
    GT_U32       bitNum;    /* bit number                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;
        bitNum = 27;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 23;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitNum, 1, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableGet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
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
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableGet
(
    IN  GT_U8                              devNum,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclL3L4ParsingOverMplsEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPclL3L4ParsingOverMplsEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclOverrideUDBCfgGet
*
* DESCRIPTION:
*        The function gets the configuration elements
*        of enable status of overriding the
*        UDB configuration for specific UDB in specific key format
*        where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum             - device number
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*    enable             - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       regAddrPtr      - (pointer to) register address
*       maskPtr         - (pointer to) mask of configuration
*       valuePtr        - (pointer to) value of configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclOverrideUDBCfgGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    IN  GT_BOOL                              enable,
    OUT GT_U32                               *regAddrPtr,
    OUT GT_U32                               *maskPtr,
    OUT GT_U32                               *valuePtr
)
{
    GT_BOOL valueWasSet = GT_FALSE;/*indication that (*valuePtr) was set */

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* this device not supports those 6 keys */
        switch (ruleFormat)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            default:
                break;
        }
    }

    switch (udbOverrideType)
    {
        case CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_LSB_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineUserDefinedBytesConfig;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclUDBConfig;
            }

            switch (ruleFormat)
            {
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                    *maskPtr = 1;
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                    *maskPtr = (1 << 2);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                    *maskPtr = (1 << 4);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                    *maskPtr = (1 << 8);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                    *maskPtr = (1 << 10);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                    *maskPtr = (1 << 12);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                    *maskPtr = (1 << 6);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                    *maskPtr = (1 << 14);
                    break;

                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E :
                    *maskPtr = (1 << 20);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E :
                    *maskPtr = (1 << 16);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E :
                    *maskPtr = (1 << 18);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E :
                    *maskPtr = (1 << 22);
                    break;


                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_MSB_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineUserDefinedBytesConfig;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclUDBConfig;
            }
            switch (ruleFormat)
            {
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum)
                        == GT_FALSE)
                    {
                        *maskPtr = (1 << 1);
                    }
                    else
                    {
                        /* eArch device */
                        *maskPtr = (3 << 27);/* 2 bits */
                        *valuePtr = 1 << 27; /* value 1 */
                        valueWasSet = GT_TRUE;
                    }
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                    *maskPtr = (1 << 3);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                    *maskPtr = (1 << 5);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                    *maskPtr = (1 << 9);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                    *maskPtr = (1 << 11);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                    *maskPtr = (1 << 13);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                    *maskPtr = (1 << 7);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                    *maskPtr = (1 << 15);
                    break;

                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E :
                    *maskPtr = (1 << 21);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E :
                    *maskPtr = (1 << 17);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E :
                    *maskPtr = (1 << 19);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E :
                    *maskPtr = (1 << 23);
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_QOS_PROFILE_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineUserDefinedBytesConfig;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclUDBConfig;
            }
            switch (ruleFormat)
            {
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                    *maskPtr = (1 << 24);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                    *maskPtr = (1 << 25);
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_TRUNK_HASH_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineUserDefinedBytesExtConfig;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineExtUdbConfig;
            }

            switch (ruleFormat)
            {
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                    *maskPtr = 1;
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                    *maskPtr = (1 << 2);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                    *maskPtr = (1 << 4);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                    *maskPtr = (1 << 5);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                    *maskPtr = (1 << 6);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                    *maskPtr = (1 << 3);
                    break;
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                    *maskPtr = (1 << 7);
                    break;

                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E :
                    *maskPtr = (1 << 10);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E  :
                    *maskPtr = (1 << 8);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E :
                    *maskPtr = (1 << 9);
                    break;
                case     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E :
                    *maskPtr = (1 << 11);
                    break;

                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_TAG1_INFO_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum)
                == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            *regAddrPtr =
                PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineUserDefinedBytesConfig;
            switch (ruleFormat)
            {
                case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                    *maskPtr = 3 << 27;    /* 2 bits */
                    *valuePtr = 2 << 27;   /* value 2 */
                    valueWasSet = GT_TRUE;
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_MODE_E:
             if (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum)
                  == GT_FALSE)
             {
                 CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
             }
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
            /* ruleFormat parameter ignored */
            *maskPtr = (1 << 16);
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(enable == GT_FALSE)
    {
        *valuePtr = 0;
    }
    else
    if(enable == GT_TRUE && valueWasSet == GT_FALSE)
    {
        *valuePtr = *maskPtr;
    }

    if((*regAddrPtr) == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        /* the chosen register is not supported by this device.
            meaning that the required type is not supported by this device .
        */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet
*
* DESCRIPTION:
*        The function enables or disables overriding of the
*        UDB content for specific UDB in specific key format
*        where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*    enable             - GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB configuration is used.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    IN  GT_BOOL                              enable
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       regAddr;     /* register address            */
    GT_U32       mask;        /* register mask               */
    GT_U32       value;       /* register value              */
    GT_U32       portGroupId; /* port Group Id               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    rc = prvCpssDxChPclOverrideUDBCfgGet(
        devNum, ruleFormat, udbOverrideType, enable,
        &regAddr, &mask, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupWriteRegBitMask(
            devNum, portGroupId, regAddr, mask, value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet
*
* DESCRIPTION:
*        The function enables or disables overriding of the
*        UDB content for specific UDB in specific key format
*        where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*    enable             - GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB configuration is used.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    IN  GT_BOOL                              enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleFormat, udbOverrideType, enable));

    rc = internal_cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet(devNum, portGroupsBmp, ruleFormat, udbOverrideType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleFormat, udbOverrideType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet
*
* DESCRIPTION:
*        The function get status (enabled or disabled) of the
*        overriding of the UDB content for specific UDB
*        in specific key format where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*
* OUTPUTS:
*    enablePtr          - (pointer to)
*                         GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB content may be overriden
*                         by content of another udbOverrideType.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    OUT GT_BOOL                              *enablePtr
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       regAddr;     /* register address            */
    GT_U32       mask;        /* register mask               */
    GT_U32       pattern;     /* register value pattern      */
    GT_U32       value;       /* register value              */
    GT_U32       portGroupId; /* port Group Id               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    rc = prvCpssDxChPclOverrideUDBCfgGet(
        devNum, ruleFormat, udbOverrideType, GT_TRUE /*enable*/,
        &regAddr, &mask, &pattern);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpPortGroupReadRegBitMask(
        devNum, portGroupId, regAddr, mask, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == pattern) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet
*
* DESCRIPTION:
*        The function get status (enabled or disabled) of the
*        overriding of the UDB content for specific UDB
*        in specific key format where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*
* OUTPUTS:
*    enablePtr          - (pointer to)
*                         GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB content may be overriden
*                         by content of another udbOverrideType.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    OUT GT_BOOL                              *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ruleFormat, udbOverrideType, enablePtr));

    rc = internal_cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet(devNum, portGroupsBmp, ruleFormat, udbOverrideType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ruleFormat, udbOverrideType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOverrideUserDefinedBytesEnableSet
*
* DESCRIPTION:
*        The function enables or disables overriding of the
*        UDB content for specific UDB in specific key format
*        where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*    enable             - GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB configuration is used.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOverrideUserDefinedBytesEnableSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    IN  GT_BOOL                              enable
)
{
    return cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, udbOverrideType, enable);
}

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesEnableSet
*
* DESCRIPTION:
*        The function enables or disables overriding of the
*        UDB content for specific UDB in specific key format
*        where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*    enable             - GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB configuration is used.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesEnableSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    IN  GT_BOOL                              enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOverrideUserDefinedBytesEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleFormat, udbOverrideType, enable));

    rc = internal_cpssDxChPclOverrideUserDefinedBytesEnableSet(devNum, ruleFormat, udbOverrideType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleFormat, udbOverrideType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOverrideUserDefinedBytesEnableGet
*
* DESCRIPTION:
*        The function get status (enabled or disabled) of the
*        overriding of the UDB content for specific UDB
*        in specific key format where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum             - device number
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*
* OUTPUTS:
*    enablePtr          - (pointer to)
*                         GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB content may be overriden
*                         by content of another udbOverrideType.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOverrideUserDefinedBytesEnableGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    OUT GT_BOOL                              *enablePtr
)
{
    return cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, udbOverrideType, enablePtr);
}

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesEnableGet
*
* DESCRIPTION:
*        The function get status (enabled or disabled) of the
*        overriding of the UDB content for specific UDB
*        in specific key format where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum             - device number
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*
* OUTPUTS:
*    enablePtr          - (pointer to)
*                         GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB content may be overriden
*                         by content of another udbOverrideType.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesEnableGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    OUT GT_BOOL                              *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOverrideUserDefinedBytesEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ruleFormat, udbOverrideType, enablePtr));

    rc = internal_cpssDxChPclOverrideUserDefinedBytesEnableGet(devNum, ruleFormat, udbOverrideType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ruleFormat, udbOverrideType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclLookup0ForRoutedPacketsEnableSet
*
* DESCRIPTION:
*       Enables/disables PCL lookup0 for routed packets.
*       For not routed packets lookup0 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclLookup0ForRoutedPacketsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    IN  GT_BOOL                            enable
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      hwData;    /* data to write to HW                     */
    GT_U32      wordNum;   /* number of word in the entry             */
    GT_U32      bitNum;    /* number of bit in the entry              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    /* Convert boolean value to the HW format */
    hwData = (enable == GT_TRUE) ? 0 : 1;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
        /* write value to post-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_DIS_IPCL0_FOR_ROUTED_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* DxChXCat and above */
            /* bit 14 in word 0 (abs 14) */
             wordNum = 0;
             bitNum  = 14;
        }
        else
        {
            /* DxCh3 */
            /* bit 23 in word 1 (abs 55) */
            wordNum = 1;
            bitNum  = 23;
        }

        /* Update port mode in the VLAN and QoS Configuration Entry */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,
                                             wordNum,
                                             bitNum,
                                             1, /* 1 bit */
                                             hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPclLookup0ForRoutedPacketsEnableSet
*
* DESCRIPTION:
*       Enables/disables PCL lookup0 for routed packets.
*       For not routed packets lookup0 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookup0ForRoutedPacketsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    IN  GT_BOOL                            enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclLookup0ForRoutedPacketsEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPclLookup0ForRoutedPacketsEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclLookup0ForRoutedPacketsEnableGet
*
* DESCRIPTION:
*       Gets Enable/disable status of PCL lookup0 for routed packets.
*       For not routed packets lookup0 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclLookup0ForRoutedPacketsEnableGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_U32      hwData;   /* data to write to HW                     */
    GT_STATUS   rc;       /* generic return status code              */
    GT_U32      wordNum;  /* number of word in the entry             */
    GT_U32      bitNum;   /* number of bit in the entry              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
        /* read from post-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_DIS_IPCL0_FOR_ROUTED_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* DxChXCat and above */
            /* bit 14 in word 0 (abs 14) */
             wordNum = 0;
             bitNum  = 14;
        }
        else
        {
            /* DxCh3 */
            /* bit 23 in word 1 (abs 55) */
            wordNum = 1;
            bitNum  = 23;
        }

        /* Read port mode from VLAN and QoS Configuration Entry */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            portNum,
                                            wordNum,
                                            bitNum,
                                            1,/* 1 bit */
                                            &hwData);
    }
    if (rc == GT_OK)
    {
        /* Convert gathered HW value to the SW format */
        *enablePtr = (hwData == 1) ? GT_FALSE : GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPclLookup0ForRoutedPacketsEnableGet
*
* DESCRIPTION:
*       Gets Enable/disable status of PCL lookup0 for routed packets.
*       For not routed packets lookup0 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookup0ForRoutedPacketsEnableGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclLookup0ForRoutedPacketsEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPclLookup0ForRoutedPacketsEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclLookup1ForNotRoutedPacketsEnableSet
*
* DESCRIPTION:
*       Enables/disables PCL lookup1 for not routed packets.
*       For routed packets lookup1 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclLookup1ForNotRoutedPacketsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_BOOL                            enable
)
{
    GT_U32       data;      /* register data               */
    GT_STATUS    rc;        /* return code                 */
    GT_U32       regAddr;   /* register address            */
    GT_U32       bitNum;    /* bit bumber                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    data = (enable == GT_TRUE) ? 0 : 1;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineConfig;
        bitNum  = 8;
    }
    else
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* DxChXCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineConfig;
        bitNum  = 8;
    }
    else
    {
        /* DxCh3 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.routedPacket;
        bitNum  = 1;
    }

    rc = prvCpssHwPpSetRegField(
        devNum, regAddr, bitNum, 1, data);

    return rc;
}

/*******************************************************************************
* cpssDxChPclLookup1ForNotRoutedPacketsEnableSet
*
* DESCRIPTION:
*       Enables/disables PCL lookup1 for not routed packets.
*       For routed packets lookup1 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookup1ForNotRoutedPacketsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_BOOL                            enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclLookup1ForNotRoutedPacketsEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPclLookup1ForNotRoutedPacketsEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclLookup1ForNotRoutedPacketsEnableGet
*
* DESCRIPTION:
*       Gets Enable/disable status of PCL lookup1 for not routed packets.
*       For routed packets lookup1 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclLookup1ForNotRoutedPacketsEnableGet
(
    IN  GT_U8                              devNum,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_U32       data;      /* register data               */
    GT_STATUS    rc;        /* return code                 */
    GT_U32       regAddr;   /* register address            */
    GT_U32       bitNum;    /* bit bumber                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineConfig;
        bitNum  = 8;
    }
    else
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* DxChXCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineConfig;
        bitNum  = 8;
    }
    else
    {
        /* DxCh3 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.routedPacket;
        bitNum  = 1;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, bitNum, 1, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data != 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclLookup1ForNotRoutedPacketsEnableGet
*
* DESCRIPTION:
*       Gets Enable/disable status of PCL lookup1 for not routed packets.
*       For routed packets lookup1 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookup1ForNotRoutedPacketsEnableGet
(
    IN  GT_U8                              devNum,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclLookup1ForNotRoutedPacketsEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPclLookup1ForNotRoutedPacketsEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclTcamEccDaemonEnableSet
*
* DESCRIPTION:
*       Enables or disables PCL TCAM ECC Daemon.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       enalbe        - GT_TRUE  - PCL TCAM ECC Daemon enable
*                       GT_FALSE - PCL TCAM ECC Daemon disable
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclTcamEccDaemonEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enalbe
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    /* Configure TCAM Daemon parameters */
    if (enalbe == GT_TRUE)
    {
        /* Policy TCAM Control register; ECCCycleRese */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTcamControl;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 24, 4, 0xF);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Set Read_Delay */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTcamConfig0;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 2, 3);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* Policy TCAM Configuration 2 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTcamConfig2;
        rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,0x32001);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTcamControl;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 28, 1, BOOL2BIT_MAC(enalbe));
}

/*******************************************************************************
* cpssDxChPclTcamEccDaemonEnableSet
*
* DESCRIPTION:
*       Enables or disables PCL TCAM ECC Daemon.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       enalbe        - GT_TRUE  - PCL TCAM ECC Daemon enable
*                       GT_FALSE - PCL TCAM ECC Daemon disable
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamEccDaemonEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enalbe
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclTcamEccDaemonEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enalbe));

    rc = internal_cpssDxChPclTcamEccDaemonEnableSet(devNum, enalbe);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enalbe));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclTcamEccDaemonEnableGet
*
* DESCRIPTION:
*       Gets status (Enabled/Disabled) of PCL TCAM ECC Daemon.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - (pointer to) PCL TCAM ECC Daemon status:
*                       GT_TRUE  - PCL TCAM ECC Daemon enable
*                       GT_FALSE - PCL TCAM ECC Daemon disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclTcamEccDaemonEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32    value;       /* value to read from register */
    GT_STATUS rc;          /* return status */
    GT_U32    regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get ECCDaemonEn */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTcamControl;
    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, 28, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPclTcamEccDaemonEnableGet
*
* DESCRIPTION:
*       Gets status (Enabled/Disabled) of PCL TCAM ECC Daemon.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - (pointer to) PCL TCAM ECC Daemon status:
*                       GT_TRUE  - PCL TCAM ECC Daemon enable
*                       GT_FALSE - PCL TCAM ECC Daemon disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamEccDaemonEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclTcamEccDaemonEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPclTcamEccDaemonEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclIpMinOffsetSet
*
* DESCRIPTION:
*       Set the IP Minimum Offset parameter.
*       If packet is IPV4 fragment and has non zero offset
*       and it's offset less then IP Minimum Offset
*       the packet is counted as small offset.
*       The ipHeaderInfo field in PCL search key is set to be 3 i.e. "Ip Small Offset".
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       ipMinOffset - IP Minimum Offset  parameter (APPLICABLE RANGES: 0..0x1FFF)
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range minIpOffset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclIpMinOffsetSet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             ipMinOffset
)
{

    GT_U32    regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if (ipMinOffset >= BIT_13)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /*IP Minimum Offset*/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).IPMinimumOffset;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ipMinimumOffset;
    }

    return prvCpssHwPpSetRegField(
        devNum,
        regAddr,
        0, 13, ipMinOffset);
}

/*******************************************************************************
* cpssDxChPclIpMinOffsetSet
*
* DESCRIPTION:
*       Set the IP Minimum Offset parameter.
*       If packet is IPV4 fragment and has non zero offset
*       and it's offset less then IP Minimum Offset
*       the packet is counted as small offset.
*       The ipHeaderInfo field in PCL search key is set to be 3 i.e. "Ip Small Offset".
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       ipMinOffset - IP Minimum Offset  parameter (APPLICABLE RANGES: 0..0x1FFF)
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range minIpOffset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpMinOffsetSet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             ipMinOffset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclIpMinOffsetSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ipMinOffset));

    rc = internal_cpssDxChPclIpMinOffsetSet(devNum, ipMinOffset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ipMinOffset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPclIpMinOffsetGet
*
* DESCRIPTION:
*       Get the IP Minimum Offset parameter.
*       If packet is IPV4 fragment and has non zero offset
*       and it's offset less then IP Minimum Offset
*       the packet is counted as small offset.
*       The ipHeaderInfo field in PCL search key is set to be 3 i.e. "Ip Small Offset".
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
* OUTPUTS:
*       ipMinOffsetPtr - (pointer to)IP Minimum Offset  parameter
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range minIpOffset
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclIpMinOffsetGet
(
    IN  GT_U8                              devNum,
    OUT GT_U32                             *ipMinOffsetPtr
)
{
    GT_U32    regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(ipMinOffsetPtr);
    /*IP Minimum Offset*/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).IPMinimumOffset;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ipMinimumOffset;
    }

    return prvCpssHwPpGetRegField(
        devNum,
        regAddr,
        0, 13, ipMinOffsetPtr);
}

/*******************************************************************************
* cpssDxChPclIpMinOffsetGet
*
* DESCRIPTION:
*       Get the IP Minimum Offset parameter.
*       If packet is IPV4 fragment and has non zero offset
*       and it's offset less then IP Minimum Offset
*       the packet is counted as small offset.
*       The ipHeaderInfo field in PCL search key is set to be 3 i.e. "Ip Small Offset".
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
* OUTPUTS:
*       ipMinOffsetPtr - (pointer to)IP Minimum Offset  parameter
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range minIpOffset
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpMinOffsetGet
(
    IN  GT_U8                              devNum,
    OUT GT_U32                             *ipMinOffsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclIpMinOffsetGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ipMinOffsetPtr));

    rc = internal_cpssDxChPclIpMinOffsetGet(devNum, ipMinOffsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ipMinOffsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortIngressLookup0Sublookup1TypeSet
*
* DESCRIPTION:
*    Configures Ingress PCL Lookup0 sublookup1 type.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum          - device number
*    lookupType      - partial or full IPCL lookup0 sublookup1 type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortIngressLookup0Sublookup1TypeSet
(
    IN GT_U8                                     devNum,
    IN CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT lookupType
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineConfig;
    }

    switch (lookupType)
    {
        case CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_PARTIAL_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_FULL_E:
            hwValue = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 12, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclPortIngressLookup0Sublookup1TypeSet
*
* DESCRIPTION:
*    Configures Ingress PCL Lookup0 sublookup1 type.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum          - device number
*    lookupType      - partial or full IPCL lookup0 sublookup1 type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressLookup0Sublookup1TypeSet
(
    IN GT_U8                                     devNum,
    IN CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT lookupType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortIngressLookup0Sublookup1TypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookupType));

    rc = internal_cpssDxChPclPortIngressLookup0Sublookup1TypeSet(devNum, lookupType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookupType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPclPortIngressLookup0Sublookup1TypeGet
*
* DESCRIPTION:
*    Configures Ingress PCL Lookup0 sublookup1 type.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    lookupTypePtr   - (pointer to)partial or full IPCL lookup0 sublookup1 type
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortIngressLookup0Sublookup1TypeGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT *lookupTypePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(lookupTypePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).policyEngineConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineConfig;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 12, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *lookupTypePtr =
        (hwValue == 0)
            ? CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_PARTIAL_E
            : CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_FULL_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortIngressLookup0Sublookup1TypeGet
*
* DESCRIPTION:
*    Configures Ingress PCL Lookup0 sublookup1 type.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    lookupTypePtr   - (pointer to)partial or full IPCL lookup0 sublookup1 type
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressLookup0Sublookup1TypeGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT *lookupTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortIngressLookup0Sublookup1TypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookupTypePtr));

    rc = internal_cpssDxChPclPortIngressLookup0Sublookup1TypeGet(devNum, lookupTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookupTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortListGroupingEnableSet
*
* DESCRIPTION:
*   Enable/disable port grouping mode per direction. When enabled, the 4 MSB
*   bits of the <Port List> field in the PCL keys are replaced by a 4-bit
*   <Port Group>.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       enable      - enable/disable port grouping mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortListGroupingEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PCL_DIRECTION_ENT   direction,
    IN GT_BOOL                  enable
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    GT_U32      bitOffset;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).
                            TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;
            bitOffset = 28;
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
            bitOffset = 19;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData = BOOL2BIT_MAC(enable);

    return prvCpssHwPpSetRegField(devNum, regAddr, bitOffset, 1, hwData);
}

/*******************************************************************************
* cpssDxChPclPortListGroupingEnableSet
*
* DESCRIPTION:
*   Enable/disable port grouping mode per direction. When enabled, the 4 MSB
*   bits of the <Port List> field in the PCL keys are replaced by a 4-bit
*   <Port Group>.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       enable      - enable/disable port grouping mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortListGroupingEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PCL_DIRECTION_ENT   direction,
    IN GT_BOOL                  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortListGroupingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, enable));

    rc = internal_cpssDxChPclPortListGroupingEnableSet(devNum, direction, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortListGroupingEnableGet
*
* DESCRIPTION:
*   Get the port grouping mode enabling status per direction. When enabled,
*   the 4 MSB bits of the <Port List> field in the PCL keys are replaced by a
*   4-bit <Port Group>.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*
* OUTPUTS:
*       enablePtr   - (pointer to) the port grouping mode enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortListGroupingEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    GT_U32      bitOffset;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).
                            TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;
            bitOffset = 28;
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
            bitOffset = 19;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitOffset, 1, &hwData);
    if (rc == GT_OK)
    {
        *enablePtr = BIT2BOOL_MAC(hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPclPortListGroupingEnableGet
*
* DESCRIPTION:
*   Get the port grouping mode enabling status per direction. When enabled,
*   the 4 MSB bits of the <Port List> field in the PCL keys are replaced by a
*   4-bit <Port Group>.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*
* OUTPUTS:
*       enablePtr   - (pointer to) the port grouping mode enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortListGroupingEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortListGroupingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, enablePtr));

    rc = internal_cpssDxChPclPortListGroupingEnableGet(devNum, direction, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortListPortMappingSet
*
* DESCRIPTION:
*   Set port group mapping and offset in port list for PCL working in Physical
*   Port List.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       portNum     - port number
*       enable      - port participate in the port list vector.
*       group       - PCL port group
*                     Relevant only if <enable> == GT_TRUE.
*                     (APPLICABLE RANGES: 0..15)
*       offset      - offset in the port list bit vector.
*                     Relevant only if <enable> == GT_TRUE.
*                     (APPLICABLE RANGES: 0..27)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When working in PCL Port List mode with port grouping enabled, <offset>
*       value in the range 23..27 has no influence.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortListPortMappingSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable,
    IN  GT_U32                  group,
    IN  GT_U32                  offset
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;
    GT_U32      groupNum;
    GT_U32      portOffset;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

    if( GT_TRUE == enable )
    {
        /* check PCL port group and offset in bit vector validity */
        if( (group >= BIT_4) ||
            ((1 << offset) >= BIT_28) )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        groupNum    = group;
        portOffset  = offset;
    }
    else /* GT_FALSE */
    {
        /* offset value (0x1F) for indication that port doesn't participate */
        /* in port list */
        groupNum    = 0;
        portOffset  = 0x1F;
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                if( GT_TRUE == enable )
                {
                    rc = prvCpssDxChWriteTableEntryField(
                            devNum,
                            PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
                            portNum,
                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                            246,
                            4,
                            groupNum);
                    if( GT_OK != rc )
                    {
                        return rc;
                    }
                }
                rc = prvCpssDxChWriteTableEntryField(
                        devNum,
                        PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
                        portNum,
                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                        241,
                        5,
                        portOffset);
            }
            else
            {
                if( GT_TRUE == enable )
                {
                    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).
                                                portMap.portGroupMap[portNum/8];
                    rc = prvCpssHwPpSetRegField(devNum, regAddr,
                                               (portNum%8)*4,
                                               4,
                                               groupNum);
                    if( GT_OK != rc )
                    {
                        return rc;
                    }
                }

                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).
                                                portMap.portListMap[portNum/6];
                rc = prvCpssHwPpSetRegField(devNum, regAddr,
                                               (portNum%6)*5,
                                               5,
                                               portOffset);
            }
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            if( GT_TRUE == enable )
            {
                rc = prvCpssDxChWriteTableEntryField(
                        devNum,
                        PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                        portNum,
                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                        LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PORT_GROUP_E, /* field name */
                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                        groupNum);

                if( GT_OK != rc )
                {
                    return rc;
                }
            }

            rc = prvCpssDxChWriteTableEntryField(
                    devNum,
                    PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                    portNum,
                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                    LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PORT_LIST_BIT_VECTOR_OFFSET_E, /* field name */
                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                    portOffset);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPclPortListPortMappingSet
*
* DESCRIPTION:
*   Set port group mapping and offset in port list for PCL working in Physical
*   Port List.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       portNum     - port number
*       enable      - port participate in the port list vector.
*       group       - PCL port group
*                     Relevant only if <enable> == GT_TRUE.
*                     (APPLICABLE RANGES: 0..15)
*       offset      - offset in the port list bit vector.
*                     Relevant only if <enable> == GT_TRUE.
*                     (APPLICABLE RANGES: 0..27)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When working in PCL Port List mode with port grouping enabled, <offset>
*       value in the range 23..27 has no influence.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortListPortMappingSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable,
    IN  GT_U32                  group,
    IN  GT_U32                  offset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortListPortMappingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, portNum, enable, group, offset));

    rc = internal_cpssDxChPclPortListPortMappingSet(devNum, direction, portNum, enable, group, offset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, portNum, enable, group, offset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclPortListPortMappingGet
*
* DESCRIPTION:
*   Get port group mapping and offset in port list for PCL working in Physical
*   Port List.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) port participate in the port list vector.
*       groupPtr    - (pointer to) PCL port group
*       offsetPtr   - (pointer to) offset in the port list bit vector.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclPortListPortMappingGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr,
    OUT GT_U32                  *groupPtr,
    OUT GT_U32                  *offsetPtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(groupPtr);
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                rc = prvCpssDxChReadTableEntryField(
                        devNum,
                        PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
                        portNum,
                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                        246,
                        4,
                        groupPtr);
                if( GT_OK != rc )
                {
                    return rc;
                }
                rc = prvCpssDxChReadTableEntryField(
                        devNum,
                        PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
                        portNum,
                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                        241,
                        5,
                        offsetPtr);
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).
                                                portMap.portGroupMap[portNum/8];
                rc = prvCpssHwPpGetRegField(devNum, regAddr,
                                               (portNum%8)*4,
                                               4,
                                               groupPtr);
                if( GT_OK != rc )
                {
                    return rc;
                }

                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).
                                                portMap.portListMap[portNum/6];
                rc = prvCpssHwPpGetRegField(devNum, regAddr,
                                               (portNum%6)*5,
                                               5,
                                               offsetPtr);
            }
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            rc = prvCpssDxChReadTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                                                portNum,
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PORT_GROUP_E, /* field name */
                                                PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                groupPtr);
            if( GT_OK != rc )
            {
                return rc;
            }

            rc = prvCpssDxChReadTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
                                                portNum,
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                                LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PORT_LIST_BIT_VECTOR_OFFSET_E, /* field name */
                                                PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                                offsetPtr);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (rc == GT_OK)
    {
        if( 0x1F == *offsetPtr )
        {
            /* offset value (0x1F) for indication that port doesn't participate */
            /* in port list */
            *enablePtr = GT_FALSE;
        }
        else
        {
            *enablePtr = GT_TRUE;
        }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPclPortListPortMappingGet
*
* DESCRIPTION:
*   Get port group mapping and offset in port list for PCL working in Physical
*   Port List.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) port participate in the port list vector.
*       groupPtr    - (pointer to) PCL port group
*       offsetPtr   - (pointer to) offset in the port list bit vector.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortListPortMappingGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr,
    OUT GT_U32                  *groupPtr,
    OUT GT_U32                  *offsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclPortListPortMappingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, portNum, enablePtr, groupPtr, offsetPtr));

    rc = internal_cpssDxChPclPortListPortMappingGet(devNum, direction, portNum, enablePtr, groupPtr, offsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, portNum, enablePtr, groupPtr, offsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet
*
* DESCRIPTION:
*   Set Mapping of MPLS Channel Type Profile to OAM opcode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       channelTypeProfile     - MPLS G-ACh Channel Type Profile
*                                (APPLICABLE RANGES: 1..15)
*       opcode                 - OAM opcode (APPLICABLE RANGES: 0..255)
*       RBitAssignmentEnable   - replace Bit0 of opcode by packet R-flag
*                                GT_TRUE  - Bit0 of result opcode is packet R-flag
*                                GT_FALSE - Bit0 of result opcode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    channelTypeProfile,
    IN  GT_U32                    opcode,
    IN  GT_BOOL                   RBitAssignmentEnable
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_U32      offset;  /* field offset in bits */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((channelTypeProfile > 15) || (channelTypeProfile == 0))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (opcode > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* Ingress PCL */

    /* channelType => OAMOpcode table */
    regAddr =
        PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).OAMConfigs.channelTypeOAMOpcode[channelTypeProfile - 1];

    hwData = opcode;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 8, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* RBitAssignmentEnable */

    regAddr =
        PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).OAMConfigs.RBitAssignmentEnable;

    hwData = BOOL2BIT_MAC(RBitAssignmentEnable);

    rc = prvCpssHwPpSetRegField(devNum, regAddr, channelTypeProfile, 1, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Egress PCL */

    /* channelType => OAMOpcode table */
    regAddr =
        PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).EPCLChannelTypeToOAMOpcodeMap[(channelTypeProfile - 1) / 3];

    offset = (((channelTypeProfile - 1) % 3) * 9);
    hwData = (opcode | (BOOL2BIT_MAC(RBitAssignmentEnable) << 8));

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 9, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet
*
* DESCRIPTION:
*   Set Mapping of MPLS Channel Type Profile to OAM opcode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       channelTypeProfile     - MPLS G-ACh Channel Type Profile
*                                (APPLICABLE RANGES: 1..15)
*       opcode                 - OAM opcode (APPLICABLE RANGES: 0..255)
*       RBitAssignmentEnable   - replace Bit0 of opcode by packet R-flag
*                                GT_TRUE  - Bit0 of result opcode is packet R-flag
*                                GT_FALSE - Bit0 of result opcode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    channelTypeProfile,
    IN  GT_U32                    opcode,
    IN  GT_BOOL                   RBitAssignmentEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, channelTypeProfile, opcode, RBitAssignmentEnable));

    rc = internal_cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet(devNum, channelTypeProfile, opcode, RBitAssignmentEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, channelTypeProfile, opcode, RBitAssignmentEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet
*
* DESCRIPTION:
*   Get Mapping of MPLS Channel Type Profile to OAM opcode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       channelTypeProfile     - MPLS G-ACh Channel Type Profile
*                                (APPLICABLE RANGES: 1..15)
*
* OUTPUTS:
*       opcodePtr               - (pointer to)OAM opcode
*       RBitAssignmentEnablePtr - (pointer to)replace Bit0 of opcode by packet R-flag
*                                GT_TRUE  - Bit0 of result opcode is packet R-flag
*                                GT_FALSE - Bit0 of result opcode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    channelTypeProfile,
    OUT GT_U32                    *opcodePtr,
    OUT GT_BOOL                   *RBitAssignmentEnablePtr
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_U32      offset;  /* field offset in bits */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((channelTypeProfile > 15) || (channelTypeProfile == 0))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(opcodePtr);
    CPSS_NULL_PTR_CHECK_MAC(RBitAssignmentEnablePtr);

    /* Egress PCL */

    /* channelType => OAMOpcode table */
    regAddr =
        PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).EPCLChannelTypeToOAMOpcodeMap[(channelTypeProfile - 1) / 3];

    offset = (((channelTypeProfile - 1) % 3) * 9);

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 9, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *opcodePtr               = (hwData & 0xFF);
    *RBitAssignmentEnablePtr = BIT2BOOL_MAC(((hwData >> 8) & 1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet
*
* DESCRIPTION:
*   Get Mapping of MPLS Channel Type Profile to OAM opcode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       channelTypeProfile     - MPLS G-ACh Channel Type Profile
*                                (APPLICABLE RANGES: 1..15)
*
* OUTPUTS:
*       opcodePtr               - (pointer to)OAM opcode
*       RBitAssignmentEnablePtr - (pointer to)replace Bit0 of opcode by packet R-flag
*                                GT_TRUE  - Bit0 of result opcode is packet R-flag
*                                GT_FALSE - Bit0 of result opcode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    channelTypeProfile,
    OUT GT_U32                    *opcodePtr,
    OUT GT_BOOL                   *RBitAssignmentEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, channelTypeProfile, opcodePtr, RBitAssignmentEnablePtr));

    rc = internal_cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet(devNum, channelTypeProfile, opcodePtr, RBitAssignmentEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, channelTypeProfile, opcodePtr, RBitAssignmentEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamMegLevelModeSet
*
* DESCRIPTION:
*   Set OAM MEG Level Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       megLevelMode           - OAM MEG Level Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamMegLevelModeSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_PCL_DIRECTION_ENT                    direction,
    IN  GT_U32                                    oamProfile,
    IN  CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT  megLevelMode
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_U32      offset;  /* field offset in bits */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (oamProfile > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    offset = (2 * oamProfile);

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            switch (megLevelMode)
            {
                case CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_UDB_E:
                    hwData = 0;
                    break;
                case CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_MAC_DA_E:
                    hwData = 1;
                    break;
                case CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_QOS_E:
                    hwData = 2;
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            regAddr =
                PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).OAMConfigs.OAMMEGLevelModeConfigs;
            break;

        case CPSS_PCL_DIRECTION_EGRESS_E:
            switch (megLevelMode)
            {
                case CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_UDB_E:
                    hwData = 0;
                    break;
                case CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_MAC_DA_E:
                    hwData = 1;
                    break;
                case CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_EXP_E:
                    hwData = 2;
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            regAddr =
                PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).EPCLOAMMEGLevelMode;
            break;

        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 2, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChPclOamMegLevelModeSet
*
* DESCRIPTION:
*   Set OAM MEG Level Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       megLevelMode           - OAM MEG Level Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamMegLevelModeSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_PCL_DIRECTION_ENT                    direction,
    IN  GT_U32                                    oamProfile,
    IN  CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT  megLevelMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamMegLevelModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, oamProfile, megLevelMode));

    rc = internal_cpssDxChPclOamMegLevelModeSet(devNum, direction, oamProfile, megLevelMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, oamProfile, megLevelMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamMegLevelModeGet
*
* DESCRIPTION:
*   Get OAM MEG Level Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       megLevelModePtr        - (pointer to) OAM MEG Level Mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_STATE             - on unexpected value in HW
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamMegLevelModeGet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_PCL_DIRECTION_ENT                    direction,
    IN  GT_U32                                    oamProfile,
    OUT CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT  *megLevelModePtr
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_U32      offset;  /* field offset in bits */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(megLevelModePtr);

    if (oamProfile > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    offset = (2 * oamProfile);

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            regAddr =
                PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).OAMConfigs.OAMMEGLevelModeConfigs;
            break;

        case CPSS_PCL_DIRECTION_EGRESS_E:
            regAddr =
                PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).EPCLOAMMEGLevelMode;
            break;

        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 2, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            switch (hwData)
            {
                case 0:
                    *megLevelModePtr = CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_UDB_E;
                    break;
                case 1:
                    *megLevelModePtr = CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_MAC_DA_E;
                    break;
                case 2:
                    *megLevelModePtr = CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_QOS_E;
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_PCL_DIRECTION_EGRESS_E:
            switch (hwData)
            {
                case 0:
                    *megLevelModePtr = CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_UDB_E;
                    break;
                case 1:
                    *megLevelModePtr = CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_MAC_DA_E;
                    break;
                case 2:
                    *megLevelModePtr = CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_EXP_E;
                    break;
                default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
            break;

        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;;
}

/*******************************************************************************
* cpssDxChPclOamMegLevelModeGet
*
* DESCRIPTION:
*   Get OAM MEG Level Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       megLevelModePtr        - (pointer to) OAM MEG Level Mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_STATE             - on unexpected value in HW
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamMegLevelModeGet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_PCL_DIRECTION_ENT                    direction,
    IN  GT_U32                                    oamProfile,
    OUT CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT  *megLevelModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamMegLevelModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, oamProfile, megLevelModePtr));

    rc = internal_cpssDxChPclOamMegLevelModeGet(devNum, direction, oamProfile, megLevelModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, oamProfile, megLevelModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamRdiMatchingSet
*
* DESCRIPTION:
*   Set OAM RDI(Remote Direct Indicator) Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       rdiMask                - Mask for RDI bit.(APPLICABLE RANGES: 0..255)
*       rdiPattern             - Pattern for RDI bit.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB4 (profile0) or UDB5 (profile1)
*                                Result = ((packetByte & rdiMask) == rdiPattern) ? 0 : 1.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamRdiMatchingSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  GT_U32                        oamProfile,
    IN  GT_U32                        rdiMask,
    IN  GT_U32                        rdiPattern
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_U32      offset;  /* field offset in bits */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (oamProfile > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (rdiMask > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (rdiPattern > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    offset = (16 * oamProfile);

    hwData = ((rdiPattern << 8) | rdiMask);

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            regAddr =
                PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).OAMConfigs.OAMRDIConfigs;
            break;

        case CPSS_PCL_DIRECTION_EGRESS_E:
            regAddr =
                PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).EPCLOAMRDI;
            break;

        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 16, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChPclOamRdiMatchingSet
*
* DESCRIPTION:
*   Set OAM RDI(Remote Direct Indicator) Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       rdiMask                - Mask for RDI bit.(APPLICABLE RANGES: 0..255)
*       rdiPattern             - Pattern for RDI bit.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB4 (profile0) or UDB5 (profile1)
*                                Result = ((packetByte & rdiMask) == rdiPattern) ? 0 : 1.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamRdiMatchingSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  GT_U32                        oamProfile,
    IN  GT_U32                        rdiMask,
    IN  GT_U32                        rdiPattern
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamRdiMatchingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, oamProfile, rdiMask, rdiPattern));

    rc = internal_cpssDxChPclOamRdiMatchingSet(devNum, direction, oamProfile, rdiMask, rdiPattern);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, oamProfile, rdiMask, rdiPattern));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamRdiMatchingGet
*
* DESCRIPTION:
*   Get OAM RDI(Remote Direct Indicator) Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       rdiMaskPtr             - (pointer to)Mask for RDI bit.(APPLICABLE RANGES: 0..255)
*       rdiPatternPtr          - (pointer to)Pattern for RDI bit.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB4 (profile0) or UDB5 (profile1)
*                                Result = ((packetByte & rdiMask) == rdiPattern) ? 0 : 1.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamRdiMatchingGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  GT_U32                        oamProfile,
    OUT GT_U32                        *rdiMaskPtr,
    OUT GT_U32                        *rdiPatternPtr
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_U32      offset;  /* field offset in bits */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rdiMaskPtr);
    CPSS_NULL_PTR_CHECK_MAC(rdiPatternPtr);

    if (oamProfile > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    offset = (16 * oamProfile);

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            regAddr =
                PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).OAMConfigs.OAMRDIConfigs;
            break;

        case CPSS_PCL_DIRECTION_EGRESS_E:
            regAddr =
                PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).EPCLOAMRDI;
            break;

        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 16, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *rdiMaskPtr     = (hwData & 0xFF);
    *rdiPatternPtr  = ((hwData >> 8) & 0xFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclOamRdiMatchingGet
*
* DESCRIPTION:
*   Get OAM RDI(Remote Direct Indicator) Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       rdiMaskPtr             - (pointer to)Mask for RDI bit.(APPLICABLE RANGES: 0..255)
*       rdiPatternPtr          - (pointer to)Pattern for RDI bit.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB4 (profile0) or UDB5 (profile1)
*                                Result = ((packetByte & rdiMask) == rdiPattern) ? 0 : 1.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamRdiMatchingGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  GT_U32                        oamProfile,
    OUT GT_U32                        *rdiMaskPtr,
    OUT GT_U32                        *rdiPatternPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamRdiMatchingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, oamProfile, rdiMaskPtr, rdiPatternPtr));

    rc = internal_cpssDxChPclOamRdiMatchingGet(devNum, direction, oamProfile, rdiMaskPtr, rdiPatternPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, oamProfile, rdiMaskPtr, rdiPatternPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamEgressPacketDetectionSet
*
* DESCRIPTION:
*   Set OAM Egress Packet Detection parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       mode                   - Egress Packet Detection mode.
*       sourceId               - packet Source Id. When mode ==
*                                CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E
*                                and packet source ID equal to sourceId value
*                                the packet treated as OAM packet
*                                regardless to PCL action results.
*                                (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamEgressPacketDetectionSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT  mode,
    IN  GT_U32                                    sourceId
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_STANDARD_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E:
            hwData = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (sourceId > 0xFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwData = ((sourceId << 1) | hwData);

    regAddr =
        PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).EPCLOAMPktDetection;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 13, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChPclOamEgressPacketDetectionSet
*
* DESCRIPTION:
*   Set OAM Egress Packet Detection parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       mode                   - Egress Packet Detection mode.
*       sourceId               - packet Source Id. When mode ==
*                                CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E
*                                and packet source ID equal to sourceId value
*                                the packet treated as OAM packet
*                                regardless to PCL action results.
*                                (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamEgressPacketDetectionSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT  mode,
    IN  GT_U32                                    sourceId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamEgressPacketDetectionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode, sourceId));

    rc = internal_cpssDxChPclOamEgressPacketDetectionSet(devNum, mode, sourceId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode, sourceId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamEgressPacketDetectionGet
*
* DESCRIPTION:
*   Get OAM Egress Packet Detection parameters.
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
*       modePtr                - (pointer to)Egress Packet Detection mode.
*       sourceIdPtr            - (pointer to)packet Source Id. When mode ==
*                                CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E
*                                and packet source ID equal to sourceId value
*                                the packet treated as OAM packet
*                                regardless to PCL action results.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamEgressPacketDetectionGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT  *modePtr,
    OUT GT_U32                                    *sourceIdPtr
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);
    CPSS_NULL_PTR_CHECK_MAC(sourceIdPtr);

    regAddr =
        PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).EPCLOAMPktDetection;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 13, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = ((hwData & 1) == 0)
        ? CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_STANDARD_E
        : CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E;

    *sourceIdPtr = ((hwData >> 1) & 0xFFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclOamEgressPacketDetectionGet
*
* DESCRIPTION:
*   Get OAM Egress Packet Detection parameters.
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
*       modePtr                - (pointer to)Egress Packet Detection mode.
*       sourceIdPtr            - (pointer to)packet Source Id. When mode ==
*                                CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E
*                                and packet source ID equal to sourceId value
*                                the packet treated as OAM packet
*                                regardless to PCL action results.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamEgressPacketDetectionGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT  *modePtr,
    OUT GT_U32                                    *sourceIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamEgressPacketDetectionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr, sourceIdPtr));

    rc = internal_cpssDxChPclOamEgressPacketDetectionGet(devNum, modePtr, sourceIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr, sourceIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamRFlagMatchingSet
*
* DESCRIPTION:
*   Set OAM R-Flag Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       rFlagMask              - Mask for R-Flag.(APPLICABLE RANGES: 0..255)
*       rFlagPattern           - Pattern for R-Flag.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB8 (profile0) or UDB9 (profile1)
*                                Result = ((packetByte & rFlagMask) == rFlagPattern) ? 0 : 1.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamRFlagMatchingSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        oamProfile,
    IN  GT_U32                        rFlagMask,
    IN  GT_U32                        rFlagPattern
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_U32      offset;  /* field offset in bits */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (oamProfile > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (rFlagMask > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (rFlagPattern > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    offset = (16 * oamProfile);

    hwData = ((rFlagPattern << 8) | rFlagMask);

    regAddr =
        PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).OAMConfigs.OAMRFlagConfigs;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 16, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChPclOamRFlagMatchingSet
*
* DESCRIPTION:
*   Set OAM R-Flag Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       rFlagMask              - Mask for R-Flag.(APPLICABLE RANGES: 0..255)
*       rFlagPattern           - Pattern for R-Flag.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB8 (profile0) or UDB9 (profile1)
*                                Result = ((packetByte & rFlagMask) == rFlagPattern) ? 0 : 1.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamRFlagMatchingSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        oamProfile,
    IN  GT_U32                        rFlagMask,
    IN  GT_U32                        rFlagPattern
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamRFlagMatchingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, oamProfile, rFlagMask, rFlagPattern));

    rc = internal_cpssDxChPclOamRFlagMatchingSet(devNum, oamProfile, rFlagMask, rFlagPattern);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, oamProfile, rFlagMask, rFlagPattern));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclOamRFlagMatchingGet
*
* DESCRIPTION:
*   Get OAM R-Flag Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       rFlagMaskPtr           - (pointer to)Mask for R-Flag.(APPLICABLE RANGES: 0..255)
*       rFlagPatternPtr        - (pointer to)Pattern for R-Flag.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB8 (profile0) or UDB9 (profile1)
*                                Result = ((packetByte & rFlagMask) == rFlagPattern) ? 0 : 1.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclOamRFlagMatchingGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        oamProfile,
    OUT GT_U32                        *rFlagMaskPtr,
    OUT GT_U32                        *rFlagPatternPtr
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */
    GT_U32      offset;  /* field offset in bits */
    GT_STATUS   rc;      /* return code          */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rFlagMaskPtr);
    CPSS_NULL_PTR_CHECK_MAC(rFlagPatternPtr);

    if (oamProfile > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    offset = (16 * oamProfile);

    regAddr =
        PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).OAMConfigs.OAMRFlagConfigs;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 16, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *rFlagMaskPtr     = (hwData & 0xFF);
    *rFlagPatternPtr  = ((hwData >> 8) & 0xFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclOamRFlagMatchingGet
*
* DESCRIPTION:
*   Get OAM R-Flag Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       rFlagMaskPtr           - (pointer to)Mask for R-Flag.(APPLICABLE RANGES: 0..255)
*       rFlagPatternPtr        - (pointer to)Pattern for R-Flag.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB8 (profile0) or UDB9 (profile1)
*                                Result = ((packetByte & rFlagMask) == rFlagPattern) ? 0 : 1.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamRFlagMatchingGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        oamProfile,
    OUT GT_U32                        *rFlagMaskPtr,
    OUT GT_U32                        *rFlagPatternPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclOamRFlagMatchingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, oamProfile, rFlagMaskPtr, rFlagPatternPtr));

    rc = internal_cpssDxChPclOamRFlagMatchingGet(devNum, oamProfile, rFlagMaskPtr, rFlagPatternPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, oamProfile, rFlagMaskPtr, rFlagPatternPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressSourcePortSelectionModeSet
*
* DESCRIPTION:
*   Set EPCL source port field selection mode for DSA tagged packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portSelectionMode   - Source port selection mode for DSA packets
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       EPCL key <SrcPort> always reflects the local device source physical
*       ingress port, even for DSA-tagged packets. To maintain backward
*       compatibility for DSA-tagged packets, a global mode allows working in
*       the backward compatible mode where <SrcPort> is taken from the DSA tag
*       if packet was received DSA tagged.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressSourcePortSelectionModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT portSelectionMode
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (portSelectionMode)
    {
        case CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ORIGINAL_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_LOCAL_E:
            hwData = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;

    return prvCpssHwPpSetRegField(devNum, regAddr, 20, 1, hwData);
}

/*******************************************************************************
* cpssDxChPclEgressSourcePortSelectionModeSet
*
* DESCRIPTION:
*   Set EPCL source port field selection mode for DSA tagged packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portSelectionMode   - Source port selection mode for DSA packets
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       EPCL key <SrcPort> always reflects the local device source physical
*       ingress port, even for DSA-tagged packets. To maintain backward
*       compatibility for DSA-tagged packets, a global mode allows working in
*       the backward compatible mode where <SrcPort> is taken from the DSA tag
*       if packet was received DSA tagged.
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressSourcePortSelectionModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT portSelectionMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressSourcePortSelectionModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portSelectionMode));

    rc = internal_cpssDxChPclEgressSourcePortSelectionModeSet(devNum, portSelectionMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portSelectionMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressSourcePortSelectionModeGet
*
* DESCRIPTION:
*   Get EPCL source port field selection mode for DSA tagged packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       portSelectionModePtr    - (pointer to) Source port selection mode for
*                                 DSA packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       EPCL key <SrcPort> always reflects the local device source physical
*       ingress port, even for DSA-tagged packets. To maintain backward
*       compatibility for DSA-tagged packets, a global mode allows working in
*       the backward compatible mode where <SrcPort> is taken from the DSA tag
*       if packet was received DSA tagged.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressSourcePortSelectionModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT *portSelectionModePtr
)
{
    GT_STATUS   rc;      /* return code          */
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portSelectionModePtr);

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 20, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *portSelectionModePtr = (hwData == 0) ?
            CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ORIGINAL_E :
            CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_LOCAL_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclEgressSourcePortSelectionModeGet
*
* DESCRIPTION:
*   Get EPCL source port field selection mode for DSA tagged packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       portSelectionModePtr    - (pointer to) Source port selection mode for
*                                 DSA packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       EPCL key <SrcPort> always reflects the local device source physical
*       ingress port, even for DSA-tagged packets. To maintain backward
*       compatibility for DSA-tagged packets, a global mode allows working in
*       the backward compatible mode where <SrcPort> is taken from the DSA tag
*       if packet was received DSA tagged.
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressSourcePortSelectionModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT *portSelectionModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressSourcePortSelectionModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portSelectionModePtr));

    rc = internal_cpssDxChPclEgressSourcePortSelectionModeGet(devNum, portSelectionModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portSelectionModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressTargetPortSelectionModeSet
*
* DESCRIPTION:
*   Set EPCL target port field selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portSelectionMode   - Target port selection mode
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressTargetPortSelectionModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT portSelectionMode
)
{
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (portSelectionMode)
    {
        case CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_LOCAL_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_FINAL_E:
            hwData = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;

    return prvCpssHwPpSetRegField(devNum, regAddr, 21, 1, hwData);
}

/*******************************************************************************
* cpssDxChPclEgressTargetPortSelectionModeSet
*
* DESCRIPTION:
*   Set EPCL target port field selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portSelectionMode   - Target port selection mode
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTargetPortSelectionModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT portSelectionMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressTargetPortSelectionModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portSelectionMode));

    rc = internal_cpssDxChPclEgressTargetPortSelectionModeSet(devNum, portSelectionMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portSelectionMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressTargetPortSelectionModeGet
*
* DESCRIPTION:
*   Get EPCL target port field selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       portSelectionModePtr    - (pointer to) Target port selection mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressTargetPortSelectionModeGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT
                                                        *portSelectionModePtr
)
{
    GT_STATUS   rc;      /* return code          */
    GT_U32      regAddr; /* register address     */
    GT_U32      hwData;  /* data in HW format    */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portSelectionModePtr);

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 21, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *portSelectionModePtr = (hwData == 0) ?
            CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_LOCAL_E :
            CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_FINAL_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclEgressTargetPortSelectionModeGet
*
* DESCRIPTION:
*   Get EPCL target port field selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       portSelectionModePtr    - (pointer to) Target port selection mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTargetPortSelectionModeGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT
                                                        *portSelectionModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressTargetPortSelectionModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portSelectionModePtr));

    rc = internal_cpssDxChPclEgressTargetPortSelectionModeGet(devNum, portSelectionModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portSelectionModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclSourceIdMaskSet
*
* DESCRIPTION:
*   Set mask so only certain bits in the source ID will be modified due to PCL
*   action.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum         - device number
*    lookupNum      - lookup number
*    mask           - Source ID mask (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclSourceIdMaskSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum,
    IN  GT_U32                      mask
)
{
    GT_U32      regAddr;     /* register address     */
    GT_U32      lookupIndex; /* index in register array for requested lookup */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if( mask > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch (lookupNum)
    {
        case CPSS_PCL_LOOKUP_0_E:
        case CPSS_PCL_LOOKUP_0_0_E:
            lookupIndex = 0;
            break;
        case CPSS_PCL_LOOKUP_0_1_E:
            lookupIndex = 1;
            break;
        case CPSS_PCL_LOOKUP_1_E:
            lookupIndex = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr =
        PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).lookupSourceIDMask[lookupIndex];

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 12, mask);
}

/*******************************************************************************
* cpssDxChPclSourceIdMaskSet
*
* DESCRIPTION:
*   Set mask so only certain bits in the source ID will be modified due to PCL
*   action.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum         - device number
*    lookupNum      - lookup number
*    mask           - Source ID mask (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclSourceIdMaskSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum,
    IN  GT_U32                      mask
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclSourceIdMaskSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookupNum, mask));

    rc = internal_cpssDxChPclSourceIdMaskSet(devNum, lookupNum, mask);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookupNum, mask));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclSourceIdMaskGet
*
* DESCRIPTION:
*   Get mask used for source ID modify due to PCL action.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum         - device number
*    lookupNum      - lookup number
*
* OUTPUTS:
*    maskPtr        - (pointer to) Source ID mask (APPLICABLE RANGES: 0..4095)

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclSourceIdMaskGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum,
    OUT GT_U32                      *maskPtr
)
{
    GT_U32      regAddr;     /* register address     */
    GT_U32      lookupIndex; /* index in register array for requested lookup */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    switch (lookupNum)
    {
        case CPSS_PCL_LOOKUP_0_E:
        case CPSS_PCL_LOOKUP_0_0_E:
            lookupIndex = 0;
            break;
        case CPSS_PCL_LOOKUP_0_1_E:
            lookupIndex = 1;
            break;
        case CPSS_PCL_LOOKUP_1_E:
            lookupIndex = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr =
        PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).lookupSourceIDMask[lookupIndex];

    return prvCpssHwPpGetRegField(devNum, regAddr, 0, 12, maskPtr);
}

/*******************************************************************************
* cpssDxChPclSourceIdMaskGet
*
* DESCRIPTION:
*   Get mask used for source ID modify due to PCL action.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum         - device number
*    lookupNum      - lookup number
*
* OUTPUTS:
*    maskPtr        - (pointer to) Source ID mask (APPLICABLE RANGES: 0..4095)

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclSourceIdMaskGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum,
    OUT GT_U32                      *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclSourceIdMaskGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookupNum, maskPtr));

    rc = internal_cpssDxChPclSourceIdMaskGet(devNum, lookupNum, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookupNum, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclIpPayloadMinSizeSet
*
* DESCRIPTION:
*       Set the IP payload minimum size. This value will determine the Layer 4
*       validity
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       ipPayloadMinSize    - the IP payload minimum size
*                             (APPLICABLE RANGES: 0..16383)
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
*       The L4_Valid in the packet descriptor will be cleared if:
*       For IPv4: total length - header length > IP payload minimum size
*       For IPv6: payload length > IP payload minimum size
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclIpPayloadMinSizeSet
(
    IN GT_U8        devNum,
    IN GT_U32       ipPayloadMinSize
)
{
    GT_U32      regAddr;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((CHECK_BITS_DATA_RANGE_MAC(ipPayloadMinSize, 14)) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).IPLengthChecks;

    return prvCpssHwPpSetRegField(devNum, regAddr, 12, 14, ipPayloadMinSize);
}

/*******************************************************************************
* cpssDxChPclIpPayloadMinSizeSet
*
* DESCRIPTION:
*       Set the IP payload minimum size. This value will determine the Layer 4
*       validity
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       ipPayloadMinSize    - the IP payload minimum size
*                             (APPLICABLE RANGES: 0..16383)
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
*       The L4_Valid in the packet descriptor will be cleared if:
*       For IPv4: total length - header length > IP payload minimum size
*       For IPv6: payload length > IP payload minimum size
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpPayloadMinSizeSet
(
    IN GT_U8        devNum,
    IN GT_U32       ipPayloadMinSize
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclIpPayloadMinSizeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ipPayloadMinSize));

    rc = internal_cpssDxChPclIpPayloadMinSizeSet(devNum, ipPayloadMinSize);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ipPayloadMinSize));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclIpPayloadMinSizeGet
*
* DESCRIPTION:
*       Get the IP payload minimum size. This value will determine the Layer 4
*       validity
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
*       ipPayloadMinSizePtr - the IP payload minimum size
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The L4_Valid in the packet descriptor will be cleared if:
*       For IPv4: total length - header length > IP payload minimum size
*       For IPv6: payload length > IP payload minimum size
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclIpPayloadMinSizeGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *ipPayloadMinSizePtr
)
{
    GT_U32      regAddr;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ipPayloadMinSizePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).IPLengthChecks;

    return prvCpssHwPpGetRegField(devNum, regAddr, 12, 14, ipPayloadMinSizePtr);
}

/*******************************************************************************
* cpssDxChPclIpPayloadMinSizeGet
*
* DESCRIPTION:
*       Get the IP payload minimum size. This value will determine the Layer 4
*       validity
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
*       ipPayloadMinSizePtr - the IP payload minimum size
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The L4_Valid in the packet descriptor will be cleared if:
*       For IPv4: total length - header length > IP payload minimum size
*       For IPv6: payload length > IP payload minimum size
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpPayloadMinSizeGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *ipPayloadMinSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclIpPayloadMinSizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ipPayloadMinSizePtr));

    rc = internal_cpssDxChPclIpPayloadMinSizeGet(devNum, ipPayloadMinSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ipPayloadMinSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet
*
* DESCRIPTION:
*    Function enables/disables for ingress analyzed packet the using of VID from
*    the incoming packet to access the EPCL Configuration table and for lookup
*    keys generation.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum          - device number
*    enable          - GT_TRUE  - use original VID
*                      GT_FALSE - use VLAN assigned by the processing pipe
*
* OUTPUTS:
*    none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet
(
    IN  GT_U8        devNum,
    IN  GT_BOOL      enable
)
{
    GT_U32    regAddr;       /* register address */
    GT_U32    value;         /* register value */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
           CPSS_CH1_E | CPSS_CH1_DIAMOND_E| CPSS_CH2_E | CPSS_CH3_E
           | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;

    value = BOOL2BIT_MAC(enable);
    return prvCpssHwPpSetRegField(devNum, regAddr, 3, 1, value);
}

/*******************************************************************************
* cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet
*
* DESCRIPTION:
*    Function enables/disables for ingress analyzed packet the using of VID from
*    the incoming packet to access the EPCL Configuration table and for lookup
*    keys generation.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum          - device number
*    enable          - GT_TRUE  - use original VID
*                      GT_FALSE - use VLAN assigned by the processing pipe
*
* OUTPUTS:
*    none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    None
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet
(
    IN  GT_U8        devNum,
    IN  GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet
*
* DESCRIPTION:
*    Get state of the flag in charge of the using of VID of the incoming packet
*    for ingress analyzed packet to access the EPCL Configuration table and for
*    lookup keys generation.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    enablePtr       - (pointer to)enable/disable the using of original VID.
*                      GT_TRUE  - use original VID
*                      GT_FALSE - use VLAN assigned by the processing pipe
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet
(
    IN  GT_U8         devNum,
    OUT GT_BOOL       *enablePtr
)
{
    GT_U32      regAddr;       /* register address */
    GT_U32      hwValue;       /* HW Value */
    GT_STATUS   rc;            /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
           CPSS_CH1_E | CPSS_CH1_DIAMOND_E| CPSS_CH2_E | CPSS_CH3_E
           | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 3, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(hwValue);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet
*
* DESCRIPTION:
*    Get state of the flag in charge of the using of VID of the incoming packet
*    for ingress analyzed packet to access the EPCL Configuration table and for
*    lookup keys generation.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    enablePtr       - (pointer to)enable/disable the using of original VID.
*                      GT_TRUE  - use original VID
*                      GT_FALSE - use VLAN assigned by the processing pipe
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    None
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet
(
    IN  GT_U8         devNum,
    OUT GT_BOOL       *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
