/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefPuma3.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- Puma3 devices
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsPuma3.h>

/* for all not PEX and not GPP registers */
/* GT_FALSE == isGpp, 0 == gppId, NULL --gppFuncPtr */
/* nonSumBitMask calculated in run time  */
/* _rwBitMask     - bits in cause register that must be cleared by write        */
/* _maskRcvIntrEn - bits that must be masked up to end of interrupt processing  */
#define PRV_SCAN_NODE_GEN_MAC(                                          \
    _bitNum, _causeRegAddr, _maskRegAddr,                               \
    _regBaseIdx, _startBit, _endBit,                                    \
    _rwBitMask, _maskRcvIntrEn, _subIntrListLen)                        \
    {_bitNum, GT_FALSE /*isGpp*/, 0/*gppId*/, NULL/*gppFuncPtr*/,       \
     _causeRegAddr, _maskRegAddr,                                       \
    prvCpssDrvHwPpPortGroupIsrRead,                                     \
    prvCpssDrvHwPpPortGroupIsrWrite,                                    \
    ((PRV_CPSS_PM3INT_##_regBaseIdx##_E & (~ 0x1F))+ _startBit),        \
    ((PRV_CPSS_PM3INT_##_regBaseIdx##_E & (~ 0x1F)) + _endBit),         \
    0 /*nonSumBitMask*/, _rwBitMask, _maskRcvIntrEn, _subIntrListLen,   \
    NULL /*subIntrScan*/, NULL /*nextIntrScan*/}

/* for typical Summary not PEX and not GPP registers */
/* GT_FALSE == isGpp, 0 == gppId, NULL == gppFuncPtr */
/* rwBitMask == 0 and maskRcvIntrEn == 0             */
#define PRV_SCAN_NODE_GEN_SUM_MAC(                                      \
    _bitNum, _causeRegAddr, _maskRegAddr,                               \
    _regBaseIdx, _startBit, _endBit, _subIntrListLen)                   \
    {_bitNum, GT_FALSE /*isGpp*/, 0/*gppId*/, NULL/*gppFuncPtr*/,       \
     _causeRegAddr, _maskRegAddr,                                       \
    prvCpssDrvHwPpPortGroupIsrRead,                                     \
    prvCpssDrvHwPpPortGroupIsrWrite,                                    \
    ((PRV_CPSS_PM3INT_##_regBaseIdx##_E & (~ 0x1F))+ _startBit),        \
    ((PRV_CPSS_PM3INT_##_regBaseIdx##_E & (~ 0x1F)) + _endBit),         \
    0 /*nonSumBitMask*/, 0 /*rwBitMask*/, 0 /*maskRcvIntrEn*/,          \
    _subIntrListLen, NULL /*subIntrScan*/, NULL /*nextIntrScan*/}

/* for typical Non Summary not PEX and not GPP registers   */
/* GT_FALSE == isGpp, 0 == gppId, NULL --gppFuncPtr        */
/* rwBitMask == 0, maskRcvIntrEn == 0xFFFFFFFF, subIntrListLen == 0 */
#define PRV_SCAN_NODE_GEN_LEAF_MAC(                                     \
    _bitNum, _causeRegAddr, _maskRegAddr,                               \
    _regBaseIdx, _startBit, _endBit)                                    \
    {_bitNum, GT_FALSE /*isGpp*/, 0/*gppId*/, NULL/*gppFuncPtr*/,       \
     _causeRegAddr, _maskRegAddr,                                       \
    prvCpssDrvHwPpPortGroupIsrRead,                                     \
    prvCpssDrvHwPpPortGroupIsrWrite,                                    \
    ((PRV_CPSS_PM3INT_##_regBaseIdx##_E & (~ 0x1F))+ _startBit),        \
    ((PRV_CPSS_PM3INT_##_regBaseIdx##_E & (~ 0x1F)) + _endBit),         \
    0 /*nonSumBitMask*/, 0 /*rwBitMask*/, 0xFFFFFFFF /*maskRcvIntrEn*/, \
    0 /*subIntrListLen*/, NULL /*subIntrScan*/, NULL /*nextIntrScan*/}

/*========================================================================*/
/* macros for port subtree */
#define PRV_SCAN_NODE_NAMED_GEN_SUM_MAC(                                \
    _bitNum, _causeRegAddr, _maskRegAddr,                               \
    _regBaseIdx_name, _startBit, _endBit, _subIntrListLen)              \
    {_bitNum, GT_FALSE /*isGpp*/, 0/*gppId*/, NULL/*gppFuncPtr*/,       \
     _causeRegAddr, _maskRegAddr,                                       \
    prvCpssDrvHwPpPortGroupIsrRead,                                     \
    prvCpssDrvHwPpPortGroupIsrWrite,                                    \
    ((_regBaseIdx_name & (~ 0x1F))+ _startBit),                         \
    ((_regBaseIdx_name & (~ 0x1F)) + _endBit),                          \
    0 /*nonSumBitMask*/, 0 /*rwBitMask*/, 0 /*maskRcvIntrEn*/,          \
    _subIntrListLen, NULL /*subIntrScan*/, NULL /*nextIntrScan*/}

#define PRV_SCAN_NODE_NAMED_GEN_SUM_EX_MAC(                             \
    _bitNum, _causeRegAddr, _maskRegAddr,                               \
    _regBaseIdx_name, _startBit, _endBit,                               \
    _nonSumBitMask, _subIntrListLen)                                    \
    {_bitNum, GT_FALSE /*isGpp*/, 0/*gppId*/, NULL/*gppFuncPtr*/,       \
     _causeRegAddr, _maskRegAddr,                                       \
    prvCpssDrvHwPpPortGroupIsrRead,                                     \
    prvCpssDrvHwPpPortGroupIsrWrite,                                    \
    ((_regBaseIdx_name & (~ 0x1F))+ _startBit),                         \
    ((_regBaseIdx_name & (~ 0x1F)) + _endBit),                          \
    _nonSumBitMask, 0 /*rwBitMask*/, 0 /*maskRcvIntrEn*/,               \
    _subIntrListLen, NULL /*subIntrScan*/, NULL /*nextIntrScan*/}

#define PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(                               \
    _bitNum, _causeRegAddr, _maskRegAddr,                               \
    _regBaseIdx_name, _startBit, _endBit)                               \
    {_bitNum, GT_FALSE /*isGpp*/, 0/*gppId*/, NULL/*gppFuncPtr*/,       \
     _causeRegAddr, _maskRegAddr,                                       \
    prvCpssDrvHwPpPortGroupIsrRead,                                     \
    prvCpssDrvHwPpPortGroupIsrWrite,                                    \
    ((_regBaseIdx_name & (~ 0x1F))+ _startBit),                         \
    ((_regBaseIdx_name & (~ 0x1F)) + _endBit),                          \
    0 /*nonSumBitMask*/, 0 /*rwBitMask*/, 0xFFFFFFFF /*maskRcvIntrEn*/, \
    0 /*subIntrListLen*/, NULL /*subIntrScan*/, NULL /*nextIntrScan*/}

/* these registers in subtrees */

    /*Start ExtUnitsumInt - External Units Ports Interrupts Summary address */
    /* NW0[0x010C0058], NW1[0x010C1058] , NW2[0x010C2058], NW3[0x010C3058]    */
    /* NW4[0x010C4058], NW5[0x010C5058] , NW6[0x010C6058], NW7[0x010C7058]    */
    /* NW8[0x010C8058], NW9[0x010C9058] , NW10[0x010CA058], NW11[0x01BC0058]  */
    /* FB0[0x010D0058], FB1[0x010D1058] , FB2[0x010D2058], FB3[0x010D3058]    */
    /* Tree place: .5.17-28(NW0-11) and .5.3(FB0), .5.7(FB1), .5.11(FB2), .5.15(FB3) */

    /*Start PortPcs40GInt - Ports PCS40G Interrupts address */
    /* NW0[0x010C0408], NW1[0x010C1408] , NW2[0x010C2408], NW3[0x010C3408]    */
    /* NW4[0x010C4408], NW5[0x010C5408] , NW6[0x010C6408], NW7[0x010C7408]    */
    /* NW8[0x010C8408], NW9[0x010C9408] , NW10[0x010CA408], NW11[0x010CB408]  */
    /* FB0_L0[0x010D0A08], FB1_L0[0x010D0C08] , FB2_L0[0x010D0408], FB3_L0[0x010D0808]  */
    /* FB0_L1[0x010D1A08], FB1_L1[0x010D1C08] , FB2_L1[0x010D1408], FB3_L1[0x010D1808]  */
    /* FB0_L2[0x010D2A08], FB1_L2[0x010D2C08] , FB2_L2[0x010D2408], FB3_L2[0x010D2808]  */
    /* FB0_L3[0x010D3A08], FB1_L3[0x010D3C08] , FB2_L3[0x010D3408], FB3_L3[0x010D3808]  */
    /* Tree place: .5.17-28.5(NW0-11) and .5.3.2-5(FB0), .5.7.2-5(FB1), .5.11.2-5(FB2), .5.15.2-5(FB3) */

    /*Start GigePortInt - GIGE (three spid) Ports Interrupts address */
    /* NW0[0x01000020], NW1[0x01001020] , NW2[0x01002020], NW3[0x01003020]    */
    /* NW4[0x01004020], NW5[0x01005020] , NW6[0x01006020], NW7[0x01007020]    */
    /* NW8[0x01008020], NW9[0x01009020] , NW10[0x0100A020], NW11[0x0100B020]  */
    /* Tree place: .5.17-28.2(NW0-11) */

    /*Start PortXlgInt - Ports XLG Interrupts address */
    /* NW0[0x010C0014], NW1[0x010C1014] , NW2[0x010C2014], NW3[0x010C3014]    */
    /* NW4[0x010C4014], NW5[0x010C5014] , NW6[0x010C6014], NW7[0x010C7014]    */
    /* NW8[0x010C8014], NW9[0x010C9014] , NW10[0x010CA014], NW11[0x010CB014]  */
    /* FB0[0x010D0014], FB1[0x010D1014] , FB2[0x010D2014], FB3[0x010D3014]  */
    /* Tree place: .5.17-28.1(NW0-11) and .5.3.1(FB0), .5.7.1(FB1), .5.11.1(FB2), .5.15.1(FB3) */

    /*Start PortXpcsGlobalInt - Ports XPCS Global Interrupts address */
    /* NW0[0x011C0214], NW2[0x011C2214], NW4[0x011C4214], NW6[0x011C6214]    */
    /* NW8[0x011C8214], NW10[0x011CA214]                                     */
    /* Tree place: .5.<17,19,21,23,25,27>.4(NW 0,2,4,6,8,10) */

    /*Start PortXpcsLaneInt - Ports XPCS Lane Interrupts address */
    /* NW0_L0[0x011C0260],  NW0_L1[0x011C02A4], NW0_L2[0x011C02E8], NW0_L3[0x011C032C], */
    /* NW2_L0[0x011C2260],  NW2_L1[0x011C22A4], NW2_L2[0x011C22E8], NW2_L3[0x011C232C], */
    /* NW4_L0[0x011C4260],  NW4_L1[0x011C42A4], NW4_L2[0x011C42E8], NW4_L3[0x011C432C], */
    /* NW6_L0[0x011C6260],  NW6_L1[0x011C62A4], NW6_L2[0x011C62E8], NW6_L3[0x011C632C], */
    /* NW8_L0[0x011C8260],  NW8_L1[0x011C82A4], NW8_L2[0x011C82E8], NW8_L3[0x011C832C], */
    /* NW10_L0[0x011CA260],  NW10_L1[0x011CA2A4], NW10_L2[0x011CA2E8], NW10_L3[0x011CA32C], */
    /* Tree place: .5.<17,19,21,23,25,27>.4.7-12(NW 0,2,4,6,8,10.L0-3) */

/*_portType - FB (fabric) or NW (network) */
/*_portBase - 16 (fabric) or 0 (network) */

/*_bitNum - number of bit in the higher register in the tree */
/*_portNum 0-3 (fabric) */
#define PRV_SCAN_NODE_FB_PORT_SUBTREE_MAC(                                   \
    _bitNum, _portNum)                                                       \
    PRV_SCAN_NODE_NAMED_GEN_SUM_MAC(_bitNum,                                 \
    (0x010C0058 + ((_portNum + 16) * 0x1000))/*_causeRegAddr*/,              \
    (0x010C005C + ((_portNum + 16) * 0x1000))/*_maskRegAddr*/,               \
    PRV_CPSS_PM3INT_PORTS_FB##_portNum##_SUM_GENERAL_E,                      \
    1/*_startBit*/, 5/*_endBit*/, 5/*_subIntrListLen*/),                     \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(1 /*_bitNum*/,                      \
        (0x010C0014 + ((_portNum + 16) * 0x1000))        /*_causeRegAddr*/,  \
        (0x010C0018 + ((_portNum + 16) * 0x1000))        /*_maskRegAddr*/,   \
        PRV_CPSS_PM3INT_PORTS_XLG_FB##_portNum##_GENERAL_E,                  \
        1/*_startBit*/, 12/*_endBit*/),                                      \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(2 /*_bitNum*/,                      \
        (0x010C0408 + ((_portNum + 16) * 0x1000) + 0x400)/*_causeRegAddr*/,  \
        (0x010C040C + ((_portNum + 16) * 0x1000) + 0x400)/*_maskRegAddr*/,   \
        PRV_CPSS_PM3INT_PORTS_PCS40G_FB##_portNum##_L1_GENERAL_E,            \
        2/*_startBit*/, 13/*_endBit*/),                                      \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(3 /*_bitNum*/,                      \
        (0x010C0408 + ((_portNum + 16) * 0x1000) + 0x600)/*_causeRegAddr*/,  \
        (0x010C040C + ((_portNum + 16) * 0x1000) + 0x600)/*_maskRegAddr*/,   \
        PRV_CPSS_PM3INT_PORTS_PCS40G_FB##_portNum##_L2_GENERAL_E,            \
        3/*_startBit*/, 13/*_endBit*/),                                      \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(4 /*_bitNum*/,                      \
        (0x010C0408 + ((_portNum + 16) * 0x1000) + 0x800)/*_causeRegAddr*/,          \
        (0x010C040C + ((_portNum + 16) * 0x1000) + 0x800)/*_maskRegAddr*/,           \
        PRV_CPSS_PM3INT_PORTS_PCS40G_FB##_portNum##_L3_GENERAL_E,            \
        4/*_startBit*/, 13/*_endBit*/),                                      \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(6 /*_bitNum*/,                      \
        (0x010C0408 + ((_portNum + 16) * 0x1000))/*_causeRegAddr*/,  \
        (0x010C040C + ((_portNum + 16) * 0x1000))/*_maskRegAddr*/,   \
        PRV_CPSS_PM3INT_PORTS_PCS40G_FB##_portNum##_L0_GENERAL_E,            \
        5/*_startBit*/, 14/*_endBit*/)

/*_bitNum - number of bit in the higher register in the tree */
/*_portNum 1,3,5,7,9,11 (network) */
#define PRV_SCAN_NODE_NW_PORT_ODD_SUBTREE_MAC(                               \
    _bitNum, _portNum)                                                       \
    PRV_SCAN_NODE_NAMED_GEN_SUM_MAC(_bitNum,                                 \
    (0x010C0058 + ((_portNum) * 0x1000))/*_causeRegAddr*/,                   \
    (0x010C005C + ((_portNum) * 0x1000))/*_maskRegAddr*/,                    \
    PRV_CPSS_PM3INT_PORTS_NW##_portNum##_SUM_GENERAL_E,                      \
    1/*_startBit*/, 5/*_endBit*/, 3/*_subIntrListLen*/),                     \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(1 /*_bitNum*/,                      \
        (0x010C0014 + ((_portNum) * 0x1000))        /*_causeRegAddr*/,       \
        (0x010C0018 + ((_portNum) * 0x1000))        /*_maskRegAddr*/,        \
        PRV_CPSS_PM3INT_PORTS_XLG_NW##_portNum##_GENERAL_E,                  \
        1/*_startBit*/, 12/*_endBit*/),                                      \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(2 /*_bitNum*/,                      \
        (0x01000020 + ((_portNum) * 0x1000))/*_causeRegAddr*/,               \
        (0x01000024 + ((_portNum) * 0x1000))/*_maskRegAddr*/,                \
        PRV_CPSS_PM3INT_PORTS_GIGE_NW##_portNum##_GENERAL_E,                 \
        2/*_startBit*/, 9/*_endBit*/),                                       \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(6 /*_bitNum*/,                      \
        (0x010C0408 + ((_portNum) * 0x1000))/*_causeRegAddr*/,               \
        (0x010C040C + ((_portNum) * 0x1000))/*_maskRegAddr*/,                \
        PRV_CPSS_PM3INT_PORTS_PCS40G_NW##_portNum##_GENERAL_E,               \
        5/*_startBit*/, 14/*_endBit*/)

/*_bitNum - number of bit in the higher register in the tree */
/*_portNum 0,2,4,6,8,10 (network) */
#define PRV_SCAN_NODE_NW_PORT_EVEN_SUBTREE_MAC(                              \
    _bitNum, _portNum)                                                       \
    PRV_SCAN_NODE_NAMED_GEN_SUM_MAC(_bitNum,                                 \
    (0x010C0058 + ((_portNum) * 0x1000))/*_causeRegAddr*/,                   \
    (0x010C005C + ((_portNum) * 0x1000))/*_maskRegAddr*/,                    \
    PRV_CPSS_PM3INT_PORTS_NW##_portNum##_SUM_GENERAL_E,                      \
    1/*_startBit*/, 5/*_endBit*/, 4/*_subIntrListLen*/),                     \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(1 /*_bitNum*/,                      \
        (0x010C0014 + ((_portNum) * 0x1000))        /*_causeRegAddr*/,       \
        (0x010C0018 + ((_portNum) * 0x1000))        /*_maskRegAddr*/,        \
        PRV_CPSS_PM3INT_PORTS_XLG_NW##_portNum##_GENERAL_E,                  \
        1/*_startBit*/, 12/*_endBit*/),                                      \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(2 /*_bitNum*/,                      \
        (0x01000020 + ((_portNum) * 0x1000))/*_causeRegAddr*/,               \
        (0x01000024 + ((_portNum) * 0x1000))/*_maskRegAddr*/,                \
        PRV_CPSS_PM3INT_PORTS_GIGE_NW##_portNum##_GENERAL_E,                 \
        2/*_startBit*/, 9/*_endBit*/),                                       \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_SUM_EX_MAC(4 /*_bitNum*/,                    \
        (0x011C0214 + ((_portNum) * 0x1000))/*_causeRegAddr*/,               \
        (0x011C0218 + ((_portNum) * 0x1000))/*_maskRegAddr*/,                \
        PRV_CPSS_PM3INT_PORTS_XPCS_NW##_portNum##_GENERAL_E,                 \
        4/*_startBit*/, 10/*_endBit*/,                                       \
        0xFFFFE07E /*_nonSumBitMask*/, 4/*_subIntrListLen*/),                \
                                                                             \
            PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(7 /*_bitNum*/,                  \
            (0x011C0260 + ((_portNum) * 0x1000))        /*_causeRegAddr*/,   \
            (0x011C0264 + ((_portNum) * 0x1000))        /*_maskRegAddr*/,    \
            PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW##_portNum##_L0_GENERAL_E,     \
            1/*_startBit*/, 7/*_endBit*/),                                   \
                                                                             \
            PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(8 /*_bitNum*/,                  \
            (0x011C02A4 + ((_portNum) * 0x1000))        /*_causeRegAddr*/,   \
            (0x011C02A8 + ((_portNum) * 0x1000))        /*_maskRegAddr*/,    \
            PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW##_portNum##_L1_GENERAL_E,     \
            1/*_startBit*/, 7/*_endBit*/),                                   \
                                                                             \
            PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(9 /*_bitNum*/,                  \
            (0x011C02E8 + ((_portNum) * 0x1000))        /*_causeRegAddr*/,   \
            (0x011C02EC + ((_portNum) * 0x1000))        /*_maskRegAddr*/,    \
            PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW##_portNum##_L2_GENERAL_E,     \
            1/*_startBit*/, 7/*_endBit*/),                                   \
                                                                             \
            PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(10 /*_bitNum*/,                 \
            (0x011C032C + ((_portNum) * 0x1000))        /*_causeRegAddr*/,   \
            (0x011C0330 + ((_portNum) * 0x1000))        /*_maskRegAddr*/,    \
            PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW##_portNum##_L3_GENERAL_E,     \
            1/*_startBit*/, 7/*_endBit*/),                                   \
                                                                             \
        PRV_SCAN_NODE_NAMED_GEN_LEAF_MAC(6 /*_bitNum*/,                      \
        (0x010C0408 + ((_portNum) * 0x1000))/*_causeRegAddr*/,               \
        (0x010C040C + ((_portNum) * 0x1000))/*_maskRegAddr*/,                \
        PRV_CPSS_PM3INT_PORTS_PCS40G_NW##_portNum##_GENERAL_E,               \
        5/*_startBit*/, 14/*_endBit*/)



/*========================================================================*/

/* number of port groups in Puma3 */
#define PUMA3_NUM_PORT_GROUPS_CNS  2

/* number of mask registers -- according to enum of interrupts */
#define PUMA3_NUM_MASK_REGISTERS_CNS  (PRV_CPSS_EXMXPM3_LAST_INT_E / 32)

/* set values that will be updated/filled during runtime (initialization stage)*/
#define FILLED_IN_RUNTIME_CNS       0

#define INTERRUPT_NUM_MAC(firstIntInReg,bit)           (firstIntInReg + bit)

#define INDEX_GLOBAL_INTERRUPT_CAUSE_CNS   PRV_CPSS_PM3INT_GLOBAL_GENERAL_SUM_E
#define INDEX_PEX_CNS                      PRV_CPSS_PM3INT_PEX_TX_REQ_WHEN_DL_DOWN_E
#define INDEX_PEX_ERR_CNS                  PRV_CPSS_PM3INT_PEX_TX_REQ_WHEN_DL_DOWN_E
#define INDEX_MISC_CNS                     PRV_CPSS_PM3INT_MG_GENERAL_E
#define INDEX_L2I_CNS                      PRV_CPSS_PM3INT_BRIDGE_SUM_E
#define INDEX_FDB_CNS                      PRV_CPSS_PM3INT_FDB_SUM_E
#define INDEX_TX_SDMA_CNS                  PRV_CPSS_PM3INT_TX_SDMA_GENERAL_E
#define INDEX_RX_SDMA_CNS                  PRV_CPSS_PM3INT_RX_SDMA_GENERAL_E

extern GT_BOOL prvCpssPuma3DuplicatedMultiPortGroupsGet
(
    IN GT_U32                   portGroupId,
    IN GT_U32                   regAddr,
    OUT  GT_PORT_GROUPS_BMP     *portGroupsBmpPtr,
    OUT  GT_BOOL                *isAdditionalRegDupNeededPtr,
    OUT  GT_U32                 *additionalRegDupPtr
);


/*
 * Typedef: struct INTERRUPT_SCAN
 *
 * Description: Basic struct for accessing interrupt register in hierarchy
 *              tree.
 *
 * Fields:
 *      bitNum          - Sum bit num in upper hierarchy, representing this
 *                        resigter.
 *      isGpp           - Is hierarcy connected to another device.
 *      gppId           - The gpp Id represented by this node (Valid only if
 *                        isGpp is GT_TRUE).
 *      gppFuncPtr      - Pointer to isrFunc to be called if isGPP == GT_TRUE.
 *      devNum          - Device number for isrFunc Call.
 *      causeRegAddr    - Address of the interrupt cause register to scan.
 *      maskRegAddr     - Address of the interrupt mask register to update after
 *                        scanning.
 *      pRegReadFunc    - A pointer to a function to be used for reading the
 *                        interrupt cause register.
 *      pRegWriteFunc   - A pointer to a function to be used for writing the
 *                        interrupt mask register.
 *      startIdx        - The start interrupt index representing the interrupts
 *                        to be scanned.
 *      endIdx          - The end interrupt index representing the interrupts to
 *                        be scanned.
 *      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 *      subIntrListLen  - List length sub interrupts list.
 *      subIntrScan     - pointer to sum bit interrupt register structure.
 *      nextIntrScan    - pointer to next interrupt struct in hierarchy.
 */

/*order of interrupts*/

/* Don't use "const" because we Fill some values at initialization stage */
#ifdef ASIC_SIMULATION
static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC puma3IntrScanArrSinglePortGroup[] =
{
    /* Global Interrupt Cause */
    {0, GT_FALSE, 0, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        INTERRUPT_NUM_MAC(INDEX_GLOBAL_INTERRUPT_CAUSE_CNS,2),
        INTERRUPT_NUM_MAC(INDEX_GLOBAL_INTERRUPT_CAUSE_CNS,22),
        FILLED_IN_RUNTIME_CNS, 0, 0x0, 7, NULL, NULL},

        /* PexIntSum -  PEX Interrupt Cause */
        {2, GT_FALSE,0, NULL, PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS, PRV_CPSS_DRV_SCAN_PEX_MASK_REG_PLACE_HOLDER_CNS,
            prvCpssDrvHwPpPortGroupReadInternalPciReg,
            prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 24),
            INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 27),
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* PexErrSum -  PEX Error Interrupt Cause */
        {3, GT_FALSE,0, NULL, PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS, PRV_CPSS_DRV_SCAN_PEX_MASK_REG_PLACE_HOLDER_CNS,
            prvCpssDrvHwPpPortGroupReadInternalPciReg,
            prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 0),
            INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 23),
            FILLED_IN_RUNTIME_CNS, 0xFF1F1B, 0xFFFFFFFF, 0, NULL, NULL},

             /* MiscIntSum - Miscellaneous Interrupt Cause */
        {11, GT_FALSE, 0, NULL, 0x00000038, 0x0000003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_MISC_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_MISC_CNS, 24),
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* L2IIntSum - Bridge Interrupt Cause   */
        {14, GT_FALSE,0, NULL, 0x00340130, 0x00340134,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_L2I_CNS, 2),
            INTERRUPT_NUM_MAC(INDEX_L2I_CNS, 28),
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* FDBIntSum - FDB Interrupt Cause,
           maskRcvIntrEn - both AUProcCompletedInt(bit 9) and AgeViaTriggerEnded(bit 4)
           should never be masked to avoid missed events situation.  */
        {17, GT_FALSE,0, NULL, 0x00400308, 0x0040030C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_FDB_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_FDB_CNS, 23),
            FILLED_IN_RUNTIME_CNS, 0x0, (0xFFFFFFFF & ~ (BIT_4 | BIT_9)) , 0, NULL, NULL},

            /* TxSDMASum - Transmit SDMA Interrupt Cause */
        {21, GT_FALSE, 0, NULL, 0x00002810, 0x00002818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_TX_SDMA_CNS, 1),
            INTERRUPT_NUM_MAC(INDEX_TX_SDMA_CNS, 24),
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* RxSDMASum - Receive SDMA Interrupt Cause  */
        {22, GT_FALSE, 0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            INTERRUPT_NUM_MAC(INDEX_RX_SDMA_CNS, 2),
            INTERRUPT_NUM_MAC(INDEX_RX_SDMA_CNS, 21),
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL}
};
#else /* ASIC_SIMULATION */
static /*const*/ PRV_CPSS_DRV_INTERRUPT_SCAN_STC puma3IntrScanArrSinglePortGroup[] =
{
    /*Global Interrupt Cause  address[0x00000030]*/
    /* Tree place: .*/
    PRV_SCAN_NODE_GEN_SUM_MAC(
        0 /*_bitNum*/, 0x00000030, 0x00000034, GLOBAL_GENERAL_SUM, 1, 12,
        10 /*_subIntrListLen*/),

        /* PexIntSum -  PEX Interrupt Cause */
        /*PEX Interrupt Cause  address[0x00001900]*/
        /* Tree place: .1(bits[31:24]) */
        {1, GT_FALSE,0, NULL, PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS, PRV_CPSS_DRV_SCAN_PEX_MASK_REG_PLACE_HOLDER_CNS,
            prvCpssDrvHwPpPortGroupReadInternalPciReg,
            prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 24),
            INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 27),
            FILLED_IN_RUNTIME_CNS, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

        /* PexErrSum -  PEX Error Interrupt Cause */
        /*PEX Interrupt Cause  address[0x00001900]*/
        /* Tree place: .2(bits[23:0] */
        {2, GT_FALSE,0, NULL, PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS, PRV_CPSS_DRV_SCAN_PEX_MASK_REG_PLACE_HOLDER_CNS,
            prvCpssDrvHwPpPortGroupReadInternalPciReg,
            prvCpssDrvHwPpPortGroupWriteInternalPciReg,
            INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 0),
            INTERRUPT_NUM_MAC(INDEX_PEX_ERR_CNS, 23),
            FILLED_IN_RUNTIME_CNS, 0xFF1F1B, 0xFFFFFFFF, 0, NULL, NULL},

        /*FUNC Interrupt Cause  address[0x000003F8]*/
        /* Tree place: .3*/
        PRV_SCAN_NODE_GEN_SUM_MAC(
            3 /*_bitNum*/, 0x000003F8, 0x000003FC, FUNC_GENERAL_SUM, 1, 31, 26),

            /*Start PCL -- Policy Engine Interrupt Cause address[0x00200004]*/
            /* Tree place: .3.1*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                1 /*_bitNum*/, 0x00200004, 0x00200008, POLICY_ENGINE_SUM, 1, 15),

            /*Start TTIEngine_IntSum address[0x00100004]*/
            /* Tree place: .3.2*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                2 /*_bitNum*/, 0x00100004, 0x00100008, TTI_GENERAL_SUM, 1, 4),

            /*Start TCC0_IntSum address[0x01400060]*/
            /* Tree place: .3.3*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                3 /*_bitNum*/, 0x01400060, 0x01400064, TCC0_GENERAL_SUM, 1, 4),

            /*Start TCC1_IntSum address[0x01500060]*/
            /* Tree place: .3.4*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                4 /*_bitNum*/, 0x01500060, 0x01500064, TCC1_GENERAL_SUM, 1, 4),

            /*Start EtcIntSum -  External TCAM Interrupt Cause address[0x02000008]*/
            /* Tree place: .3.5*/
            /* may be _maskRcvIntrEn must be fixed */
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                5 /*_bitNum*/, 0x02000008, 0x0200000C, EXT_TCAM_GENERAL_SUM, 1, 7),

            /*Start LbPortIntSum -  Loopback Port Interrupt Cause address[0x04F00040]*/
            /* Tree place: .3.6*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                6 /*_bitNum*/, 0x04F00040, 0x04F00044, LOOPBACK_PORT_GENERAL_SUM, 1, 3),

            /*Start EgrPlrIntsum - Egress Policer Interrupt Cause address[0x00E00100]*/
            /* Tree place: .3.7*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                7 /*_bitNum*/, 0x00E00100, 0x00E00104, EGRESS_POLICER_SUMMARY, 1, 5),

            /*Start L2IIntSum - Bridge Interrupt Cause address[0x00340130]*/
            /* Tree place: .3.8*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                8 /*_bitNum*/, 0x00340130, 0x00340134, BRIDGE_SUM, 1, 28),

            /*Start FDBIntSum - FDB Interrupt Cause address[0x00400308]*/
            /* Tree place: .3.9*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                9 /*_bitNum*/, 0x00400308, 0x0040030C, FDB_SUM, 1, 23),

            /*Start EqIntSum - Eq Global Interrupt Cause address[0x00D00390]*/
            /* Tree place: .3.10*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                10 /*_bitNum*/, 0x00D00390, 0x00D00394, EQ_GLOBAL_EQ_GLOBAL_SUM, 1, 11, 10),

                /*Start Stc0IntSum - STC Interrupt Cause address[0x00D00060]*/
                /* Tree place: .3.10.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x00D00060, 0x00D00068, EQ_STC_0_GENERAL, 1, 31),

                /*Start Stc1IntSum - STC Interrupt Cause address[0x00D00064]*/
                /* Tree place: .3.10.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x00D00064, 0x00D0006C, EQ_STC_1_GENERAL, 1, 31),

                /*Start RateLimit0IntSum - Rate Limit Interrupt Cause address[0x00D00080]*/
                /* Tree place: .3.10.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/, 0x00D00080, 0x00D000A4, EQ_RATE_LIMIT_0_GENERAL, 1, 31),

                /*Start RateLimit1IntSum - Rate Limit Interrupt Cause address[0x00D00084]*/
                /* Tree place: .3.10.4*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    4 /*_bitNum*/, 0x00D00084, 0x00D000A8, EQ_RATE_LIMIT_1_GENERAL, 1, 31),

                /*Start RateLimit2IntSum - Rate Limit Interrupt Cause address[0x00D00088]*/
                /* Tree place: .3.10.5*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    5 /*_bitNum*/, 0x00D00088, 0x00D000AC, EQ_RATE_LIMIT_2_GENERAL, 1, 31),

                /*Start RateLimit3IntSum - Rate Limit Interrupt Cause address[0x00D0008C]*/
                /* Tree place: .3.10.6*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/, 0x00D0008C, 0x00D000B0, EQ_RATE_LIMIT_3_GENERAL, 1, 31),

                /*Start RateLimit4IntSum - Rate Limit Interrupt Cause address[0x00D00090]*/
                /* Tree place: .3.10.7*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    7 /*_bitNum*/, 0x00D00090, 0x00D000B4, EQ_RATE_LIMIT_4_GENERAL, 1, 31),

                /*Start RateLimit5IntSum - Rate Limit Interrupt Cause address[0x00D00094]*/
                /* Tree place: .3.10.8*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    8 /*_bitNum*/, 0x00D00094, 0x00D000B8, EQ_RATE_LIMIT_5_GENERAL, 1, 31),

                /*Start RateLimit6IntSum - Rate Limit Interrupt Cause address[0x00D00098]*/
                /* Tree place: .3.10.9*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    9 /*_bitNum*/, 0x00D00098, 0x00D000BC, EQ_RATE_LIMIT_6_GENERAL, 1, 31),

                /*Start RateLimit7IntSum - Rate Limit Interrupt Cause address[0x00D0009C]*/
                /* Tree place: .3.10.10*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    10 /*_bitNum*/, 0x00D0009C, 0x00D000C0, EQ_RATE_LIMIT_7_GENERAL, 1, 31),

                /*Start RateLimit8IntSum - Rate Limit Interrupt Cause address[0x00D000A0]*/
                /* Tree place: .3.10.11*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    11 /*_bitNum*/, 0x00D000A0, 0x00D000C4, EQ_RATE_LIMIT_8_GENERAL, 1, 31),

            /*Start IngrPlrIntsum - Ingress Policer Interrupt Cause address[0x00B00100]*/
            /* Tree place: .3.11*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                11 /*_bitNum*/, 0x00B00100, 0x00B00104, INGRESS_POLICER_SUMMARY, 1, 5),

            /*Start IpvxIntSum - IPVX Interrupt Cause address[0x00600E80]*/
            /* Tree place: .3.12*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                12 /*_bitNum*/, 0x00600E80, 0x00600E84, IPVX_SUM, 1, 11),

            /*Start Lpm0IntSum - LPM0 Interrupt Cause address[0x0078000C]*/
            /* Tree place: .3.13*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                13 /*_bitNum*/, 0x0078000C, 0x00780010, LPM0_SUM, 1, 2),

            /*Start Lpm1IntSum - LPM1 Interrupt Cause address[0x0088000C]*/
            /* Tree place: .3.14*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                14 /*_bitNum*/, 0x0088000C, 0x00880010, LPM1_SUM, 1, 2),

            /*Start Lpm2IntSum - LPM2 Interrupt Cause address[0x0098000C]*/
            /* Tree place: .3.15*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                15 /*_bitNum*/, 0x0098000C, 0x00980010, LPM2_SUM, 1, 2),

            /*Start Lpm3IntSum - LPM3 Interrupt Cause address[0x00A8000C]*/
            /* Tree place: .3.16*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                16 /*_bitNum*/, 0x00a8000C, 0x00a80010, LPM3_SUM, 1, 2),

            /*Start EtiIntSum - ETI Interrupt Cause address[0x0460003C]*/
            /* Tree place: .3.17*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                17 /*_bitNum*/, 0x0460003C, 0x04600040, ETI_SUM, 1, 10),

            /*Start CncIntSum - CNC Interrupt Cause address[0x02E00100]*/
            /* Tree place: .3.24*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                24 /*_bitNum*/, 0x02E00100, 0x02E00104, CNC_GENERAL, 1, 7, 7),

                /*Start CncWa0IntSum - CNC WRAPAROUND0 Interrupt Cause address[0x002E00190]*/
                /* Tree place: .3.24.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x002E00190, 0x002E001A4, CNC_WRAPAROUND_0_GENERAL, 1, 31),

                /*Start CncWa1IntSum - CNC WRAPAROUND1 Interrupt Cause address[0x002E00194]*/
                /* Tree place: .3.24.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x002E00194, 0x002E001A8, CNC_WRAPAROUND_1_GENERAL, 1, 31),

                /*Start CncWa2IntSum - CNC WRAPAROUND2 Interrupt Cause address[0x002E00198]*/
                /* Tree place: .3.24.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/, 0x002E00198, 0x002E001AC, CNC_WRAPAROUND_2_GENERAL, 1, 2),

                /*Start CncRl0IntSum - CNC RateLimit0 Interrupt Cause address[0x002E001B8]*/
                /* Tree place: .3.24.4*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    4 /*_bitNum*/, 0x002E001B8, 0x002E001CC, CNC_RATE_LIMIT_0_GENERAL, 1, 31),

                /*Start CncRl1IntSum - CNC RateLimit1 Interrupt Cause address[0x002E001BC]*/
                /* Tree place: .3.24.5*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    5 /*_bitNum*/, 0x002E001BC, 0x002E001D0, CNC_RATE_LIMIT_1_GENERAL, 1, 31),

                /*Start CncRl2IntSum - CNC RateLimit2 Interrupt Cause address[0x002E001C0]*/
                /* Tree place: .3.24.6*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/, 0x002E001C0, 0x002E001D4, CNC_RATE_LIMIT_2_GENERAL, 1, 2),

                /*Start CncMiscIntSum - CNC MISC Interrupt Cause address[0x02E001E0]*/
                /* Tree place: .3.24.7*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    7 /*_bitNum*/, 0x02E001E0, 0x02E001E4, CNC_MISC_GENERAL, 1, 1),

            /*Start EgrFltIntSum - Egress Filtering0 Interrupt Cause address[0x035000A0]*/
            /* Tree place: .3.25*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                25 /*_bitNum*/, 0x035000A0, 0x035000B0, EGR_FLT0_GENERAL, 1, 13),

            /*Start EgrFltIntSum - Egress Filtering1 Interrupt Cause address[0x036000A0]*/
            /* Tree place: .3.26*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                26 /*_bitNum*/, 0x036000A0, 0x036000B0, EGR_FLT1_GENERAL, 1, 13),

            /*Start EgrFltIntSum - Egress Filtering2 Interrupt Cause address[0x037000A0]*/
            /* Tree place: .3.27*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                27 /*_bitNum*/, 0x037000A0, 0x037000B0, EGR_FLT2_GENERAL, 1, 13),

            /*Start EgrFltIntSum - Egress Filtering3 Interrupt Cause address[0x038000A0]*/
            /* Tree place: .3.28*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                28 /*_bitNum*/, 0x038000A0, 0x038000B0, EGR_FLT3_GENERAL, 1, 13),

            /*Start EmcIntSum - EMC Forward Interrupt Cause address[0x03200200]*/
            /* Tree place: .3.29*/
            PRV_SCAN_NODE_GEN_MAC(
                29 /*_bitNum*/, 0x03200200, 0x03200204, EMC_FW_GENERAL, 1, 16,
                0 /*_rwBitMask*/, 0xFFFFFFF8/*_maskRcvIntrEn*/, 2/*_subIntrListLen*/),

                /*Start EmcC0IntSum - EMC Forward Cause0 Interrupt Cause address[0x03200208]*/
                /* Tree place: .3.29.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x03200208, 0x0320020C, EMC_FW_CAUSE0_GENERAL, 1, 30),

                /*Start EmcC0IntSum - EMC Forward Cause1 Interrupt Cause address[0x03200210]*/
                /* Tree place: .3.29.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x03200210, 0x03200214, EMC_FW_CAUSE1_GENERAL, 1, 1),

            /*Start EmcIntSum - EMC Lookup Interrupt Cause address[0x03000200]*/
            /* Tree place: .3.30*/
            PRV_SCAN_NODE_GEN_MAC(
                30 /*_bitNum*/, 0x03000200, 0x03000204, EMC_LU_GENERAL, 1, 16,
                0 /*_rwBitMask*/, 0xFFFFFFF8/*_maskRcvIntrEn*/, 2/*_subIntrListLen*/),

                /*Start EmcC0IntSum - EMC Lookup Cause0 Interrupt Cause address[0x03000208]*/
                /* Tree place: .3.30.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x03000208, 0x0300020C, EMC_LU_CAUSE0_GENERAL, 1, 30),

                /*Start EmcC0IntSum - EMC Lookup Cause1 Interrupt Cause address[0x03000210]*/
                /* Tree place: .3.30.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x03000210, 0x03000214, EMC_LU_CAUSE1_GENERAL, 1, 1),

            /*Start DitIntSum - DIT Interrupt Cause address[0x01D00290]*/
            /* Tree place: .3.31*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                31 /*_bitNum*/, 0x01D00290, 0x01D00294, DIT_GENERAL, 1, 5),


        /*Start DataPathIntSum - Data Path Interrupt Cause address[0x000000A4]*/
        /* Tree place: .4*/
        PRV_SCAN_NODE_GEN_SUM_MAC(
            4 /*_bitNum*/, 0x000000A4, 0x000000A8, DATA_PATH_GENERAL, 1, 12, 12),

            /*Start BufMan0Cause0IntSum - Buffer Management0Cause0 */
            /* Interrupt Cause address[0x01600040]          */
            /* Tree place: .4.1*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                1 /*_bitNum*/, 0x01600040, 0x01600044, BUF_MAN0_CAUSE0_GENERAL, 1, 23),

            /*Start BufMan0Cause1IntSum - Buffer Management0Cause1 */
            /* Interrupt Cause address[0x01600048]          */
            /* Tree place: .4.2*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                2 /*_bitNum*/, 0x01600048, 0x0160004C, BUF_MAN0_CAUSE1_GENERAL, 1, 31),

            /*Start BufMan1Cause0IntSum - Buffer Management1 Cause0 */
            /* Interrupt Cause address[0x01900040]          */
            /* Tree place: .4.3 */
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                3 /*_bitNum*/, 0x01900040, 0x01900044, BUF_MAN1_CAUSE0_GENERAL, 1, 23),

            /*Start BufMan1Cause1IntSum - Buffer Management1Cause1 */
            /* Interrupt Cause address[0x01900048]          */
            /* Tree place: .4.4*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                4 /*_bitNum*/, 0x01900048, 0x0190004C, BUF_MAN1_CAUSE1_GENERAL, 1, 31),

            /*Start MrgAlignerIntSum - Merger Aligner Interrupt Cause address */
            /* MA0(network)[0x02500004]           */
            /* Tree place: .4.5 */
            PRV_SCAN_NODE_GEN_SUM_MAC(
                5 /*_bitNum*/, 0x02500004, 0x02500008, MA0_SUM_GENERAL, 1, 3, 3),

                /*Start MAMergerIntSum - MA Merger Interrupt Cause address */
                /* MA0(network)[0x02580100]    */
                /* Tree place: .4.5.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x02580100, 0x02580104, MA0_MERGER_GLOBAL_GENERAL, 1, 4),

                /*Start MAMergerPortsIntSum - MA Merger Ports Interrupt Cause address */
                /* MA0(network)[0x02580108]]               */
                /* Tree place: .4.5.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x02580108, 0x0258010C, MA0_MERGER_PORTS_GENERAL, 1, 26),

                /*Start MAAlignerIntSum - MA Aligner Interrupt Cause address */
                /* MA0(network)[0x02500100]      */
                /* Tree place: .4.5.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/, 0x02500100, 0x02500104, MA0_ALIGNER_GENERAL, 1, 5),

            /*Start MrgAlignerIntSum - Merger Aligner Interrupt Cause address */
            /* MA1(fabric)[0x02B00004]           */
            /* Tree place: .4.6 */
            PRV_SCAN_NODE_GEN_SUM_MAC(
                6 /*_bitNum*/, 0x02B00004, 0x02B00008, MA1_SUM_GENERAL, 1, 3, 3),

                /*Start MAMergerIntSum - MA Merger Interrupt Cause address */
                /* MA1(fabric)[0x02B80100]    */
                /* Tree place: .4.6.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x02B80100, 0x02B80104, MA1_MERGER_GLOBAL_GENERAL, 1, 4),

                /*Start MAMergerPortsIntSum - MA Merger Ports Interrupt Cause address */
                /* MA1(fabric)[0x02B80108]               */
                /* Tree place: .4.6.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x02B80108, 0x02B8010C, MA1_MERGER_PORTS_GENERAL, 1, 26),

                /*Start MAAlignerIntSum - MA Aligner Interrupt Cause address */
                /* MA1(fabric)[0x02B00100]      */
                /* Tree place: .4.6.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/, 0x02B00100, 0x02B00104, MA1_ALIGNER_GENERAL, 1, 5),

            /*Start LmsIntSum - LMS Interrupt Cause address */
            /* LMS0(network)[0x02200000]       */
            /* Tree place: .4.7 */
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                7 /*_bitNum*/, 0x02200000, 0x02200004, LMS0_GENERAL, 1, 1),

            /*Start LmsIntSum - LMS Interrupt Cause address */
            /* LMS1(fabric)[0x02800000]      */
            /* Tree place: .4.8 */
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                8 /*_bitNum*/, 0x02800000, 0x02800004, LMS1_GENERAL, 1, 1),

            /*Start TxDmaIntSum - TX_DMA Interrupt Summary address */
            /* TX_DMA0(network)[0x02600130]      */
            /* Tree place: .4.9 */
            PRV_SCAN_NODE_GEN_SUM_MAC(
                9 /*_bitNum*/, 0x02600130, 0x02600134, TX_DMA0_SUM_GENERAL, 1, 4, 4),

                /*Start TxDmaIntCause0 - TX_DMA Interrupt Cause0 address */
                /* TX_DMA0(network)[0x02600138]      */
                /* Tree place: .4.9.1 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x02600138, 0x0260013C, TX_DMA0_CAUSE0_GENERAL, 1, 31),

                /*Start TxDmaIntError - TX_DMA Interrupt Error address */
                /* TX_DMA0(network)[0x02600140]      */
                /* Tree place: .4.9.2 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x02600140, 0x02600144, TX_DMA0_ERROR_GENERAL, 1, 22),

                /*Start TxDmaIntCause1 - TX_DMA Interrupt Cause1 address */
                /* TX_DMA0(network)[0x02600530]       */
                /* Tree place: .4.9.3 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/, 0x02600530, 0x02600534, TX_DMA0_CAUSE1_GENERAL, 1, 18),

                /*Start TxDmaIntCause2 - TX_DMA Interrupt Cause2 address */
                /* TX_DMA0(network)[0x02600538]      */
                /* Tree place: .4.9.4 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    4 /*_bitNum*/, 0x02600538, 0x0260053C, TX_DMA0_CAUSE2_GENERAL, 1, 15),

            /*Start TxDmaIntSum - TX_DMA Interrupt Summary address */
            /* TX_DMA1(fabric)[0x02C00130]      */
            /* Tree place: .4.10 */
            PRV_SCAN_NODE_GEN_SUM_MAC(
                10 /*_bitNum*/, 0x02C00130, 0x02C00134, TX_DMA1_SUM_GENERAL, 1, 4, 4),

                /*Start TxDmaIntCause0 - TX_DMA Interrupt Cause0 address */
                /* TX_DMA1(fabric)[0x02C00138]      */
                /* Tree place: .4.10.1 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x02C00138, 0x02C0013C, TX_DMA1_CAUSE0_GENERAL, 1, 31),

                /*Start TxDmaIntError - TX_DMA Interrupt Error address */
                /* TX_DMA1(fabric)[0x02C00140]      */
                /* Tree place: .4.10.2 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x02C00140, 0x02C00144, TX_DMA1_ERROR_GENERAL, 1, 22),

                /*Start TxDmaIntCause1 - TX_DMA Interrupt Cause1 address */
                /* TX_DMA1(fabric)[0x02C00530]      */
                /* Tree place: .4.10.3 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/, 0x02C00530, 0x02C00534, TX_DMA1_CAUSE1_GENERAL, 1, 18),

                /*Start TxDmaIntCause2 - TX_DMA Interrupt Cause2 address */
                /* TX_DMA1(fabric)[0x02C00538]      */
                /* Tree place: .4.10.4 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    4 /*_bitNum*/, 0x02C00538, 0x02C0053C, TX_DMA1_CAUSE2_GENERAL, 1, 15),

            /*Start RxDmaIntSum - RX_DMA Interrupt Summary address */
            /* RX_DMA0(network)[0x01700570]      */
            /* Tree place: .4.11 */
            PRV_SCAN_NODE_GEN_SUM_MAC(
                11 /*_bitNum*/, 0x01700570, 0x01700574, RX_DMA0_SUM_GENERAL, 1, 14, 14),

                /*Start RxDmaInt0 - RX_DMA Interrupt0 address */
                /* RX_DMA0(network)[0x01700500]      */
                /* Tree place: .4.11.1 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x01700500, 0x01700504, RX_DMA0_INT0_GENERAL, 1, 3),

                /*Start RxDmaScDma0 - RX_DMA SCDMA0 address */
                /* RX_DMA0(network)[0x01700508]       */
                /* Tree place: .4.11.2 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x01700508, 0x0170053C, RX_DMA0_SCDMA0_GENERAL, 1, 2),

                /*Start RxDmaScDma1 - RX_DMA SCDMA1 address */
                /* RX_DMA0(network)[0x0170050C]       */
                /* Tree place: .4.11.3 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/, 0x0170050C, 0x01700540, RX_DMA0_SCDMA1_GENERAL, 1, 2),

                /*Start RxDmaScDma2 - RX_DMA SCDMA2 address */
                /* RX_DMA0(network)[0x01700510]       */
                /* Tree place: .4.11.4 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    4 /*_bitNum*/, 0x01700510, 0x01700544, RX_DMA0_SCDMA2_GENERAL, 1, 2),

                /*Start RxDmaScDma3 - RX_DMA SCDMA3 address */
                /* RX_DMA0(network)[0x01700514]       */
                /* Tree place: .4.11.5 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    5 /*_bitNum*/, 0x01700514, 0x01700548, RX_DMA0_SCDMA3_GENERAL, 1, 2),

                /*Start RxDmaScDma4 - RX_DMA SCDMA4 address */
                /* RX_DMA0(network)[0x01700518]       */
                /* Tree place: .4.11.6 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/, 0x01700518, 0x0170054C, RX_DMA0_SCDMA4_GENERAL, 1, 2),

                /*Start RxDmaScDma5 - RX_DMA SCDMA5 address */
                /* RX_DMA0(network)[0x0170051C]       */
                /* Tree place: .4.11.7 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    7 /*_bitNum*/, 0x0170051C, 0x01700550, RX_DMA0_SCDMA5_GENERAL, 1, 2),

                /*Start RxDmaScDma6 - RX_DMA SCDMA6 address */
                /* RX_DMA0(network)[0x01700520]       */
                /* Tree place: .4.11.8 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    8 /*_bitNum*/, 0x01700520, 0x01700554, RX_DMA0_SCDMA6_GENERAL, 1, 2),

                /*Start RxDmaScDma7 - RX_DMA SCDMA7 address */
                /* RX_DMA0(network)[0x01700524]       */
                /* Tree place: .4.11.9 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    9 /*_bitNum*/, 0x01700524, 0x01700558, RX_DMA0_SCDMA7_GENERAL, 1, 2),

                /*Start RxDmaScDma8 - RX_DMA SCDMA8 address */
                /* RX_DMA0(network)[0x01700528]       */
                /* Tree place: .4.11.10 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    10 /*_bitNum*/, 0x01700528, 0x0170055C, RX_DMA0_SCDMA8_GENERAL, 1, 2),

                /*Start RxDmaScDma9 - RX_DMA SCDMA9 address */
                /* RX_DMA0(network)[0x0170052C]       */
                /* Tree place: .4.11.11 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    11 /*_bitNum*/, 0x0170052C, 0x01700560, RX_DMA0_SCDMA9_GENERAL, 1, 2),

                /*Start RxDmaScDma10 - RX_DMA SCDMA10 address */
                /* RX_DMA0(network)[0x01700530]       */
                /* Tree place: .4.11.12 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    12 /*_bitNum*/, 0x01700530, 0x01700564, RX_DMA0_SCDMA10_GENERAL, 1, 2),

                /*Start RxDmaScDma11 - RX_DMA SCDMA11 address */
                /* RX_DMA0(network)[0x01700534]       */
                /* Tree place: .4.11.13 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    13 /*_bitNum*/, 0x01700534, 0x01700568, RX_DMA0_SCDMA11_GENERAL, 1, 2),

                /*Start RxDmaScDma12 - RX_DMA SCDMA12 address */
                /* RX_DMA0(network)[0x01700538]       */
                /* Tree place: .4.11.14 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    14 /*_bitNum*/, 0x01700538, 0x0170056C, RX_DMA0_SCDMA12_GENERAL, 1, 2),

            /*Start RxDmaIntSum - RX_DMA Interrupt Summary address */
            /* RX_DMA1(fabric)[0x01A00570]      */
            /* Tree place: .4.12 */
            PRV_SCAN_NODE_GEN_SUM_MAC(
                12 /*_bitNum*/, 0x01A00570, 0x01A00574, RX_DMA1_SUM_GENERAL, 1, 14, 14),

                /*Start RxDmaInt0 - RX_DMA Interrupt0 address */
                /* RX_DMA1(fabric)[0x01A00500]      */
                /* Tree place: .4.12.1 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/, 0x01A00500, 0x01A00504, RX_DMA1_INT0_GENERAL, 1, 3),

                /*Start RxDmaScDma0 - RX_DMA SCDMA0 address */
                /* RX_DMA1(fabric)[0x01A00508]      */
                /* Tree place: .4.12.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/, 0x01A00508, 0x01A0053C, RX_DMA1_SCDMA0_GENERAL, 1, 2),

                /*Start RxDmaScDma1 - RX_DMA SCDMA1 address */
                /* RX_DMA1(fabric)[0x01A0050C]      */
                /* Tree place: .4.12.3 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/, 0x01A0050C, 0x01A00540, RX_DMA1_SCDMA1_GENERAL, 1, 2),

                /*Start RxDmaScDma2 - RX_DMA SCDMA2 address */
                /* RX_DMA1(fabric)[0x01A00510]      */
                /* Tree place: .4.12.4 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    4 /*_bitNum*/, 0x01A00510, 0x01A00544, RX_DMA1_SCDMA2_GENERAL, 1, 2),

                /*Start RxDmaScDma3 - RX_DMA SCDMA3 address */
                /* RX_DMA1(fabric)[0x01A00514]      */
                /* Tree place: .4.12.5 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    5 /*_bitNum*/, 0x01A00514, 0x01A00548, RX_DMA1_SCDMA3_GENERAL, 1, 2),

                /*Start RxDmaScDma4 - RX_DMA SCDMA4 address */
                /* RX_DMA1(fabric)[0x01A00518]      */
                /* Tree place: .4.12.6 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/, 0x01A00518, 0x01A0054C, RX_DMA1_SCDMA4_GENERAL, 1, 2),

                /*Start RxDmaScDma5 - RX_DMA SCDMA5 address */
                /* RX_DMA1(fabric)[0x01A0051C]      */
                /* Tree place: .4.12.7 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    7 /*_bitNum*/, 0x01A0051C, 0x01A00550, RX_DMA1_SCDMA5_GENERAL, 1, 2),

                /*Start RxDmaScDma6 - RX_DMA SCDMA6 address */
                /* RX_DMA1(fabric)[0x01A00520]      */
                /* Tree place: .4.12.8 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    8 /*_bitNum*/, 0x01A00520, 0x01A00554, RX_DMA1_SCDMA6_GENERAL, 1, 2),

                /*Start RxDmaScDma7 - RX_DMA SCDMA7 address */
                /* RX_DMA0(network)[0x01700524]       */
                /* Tree place: .4.12.9 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    9 /*_bitNum*/, 0x01A00524, 0x01A00558, RX_DMA1_SCDMA7_GENERAL, 1, 2),

                /*Start RxDmaScDma8 - RX_DMA SCDMA8 address */
                /* RX_DMA1(fabric)[0x01A00528]      */
                /* Tree place: .4.12.10 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    10 /*_bitNum*/, 0x01A00528, 0x01A0055C, RX_DMA1_SCDMA8_GENERAL, 1, 2),

                /*Start RxDmaScDma9 - RX_DMA SCDMA9 address */
                /* RX_DMA1(fabric)[0x01A0052C]      */
                /* Tree place: .4.12.11 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    11 /*_bitNum*/, 0x01A0052C, 0x01A00560, RX_DMA1_SCDMA9_GENERAL, 1, 2),

                /*Start RxDmaScDma10 - RX_DMA SCDMA10 address */
                /* RX_DMA0(network)[0x01700530]       */
                /* Tree place: .4.12.12 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    12 /*_bitNum*/, 0x01A00530, 0x01A00564, RX_DMA1_SCDMA10_GENERAL, 1, 2),

                /*Start RxDmaScDma11 - RX_DMA SCDMA11 address */
                /* RX_DMA1(fabric)[0x01A00534]      */
                /* Tree place: .4.12.13 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    13 /*_bitNum*/, 0x01A00534, 0x01A00568, RX_DMA1_SCDMA11_GENERAL, 1, 2),

                /*Start RxDmaScDma12 - RX_DMA SCDMA12 address */
                /* RX_DMA1(fabric)[0x01A00538]      */
                /* Tree place: .4.12.14 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    14 /*_bitNum*/, 0x01A00538, 0x01A0056C, RX_DMA1_SCDMA12_GENERAL, 1, 2),

        /*Start PortsIntSum - Ports interrupt Summary address [0x00000080]*/
        /* Tree place: .5*/
        PRV_SCAN_NODE_GEN_SUM_MAC(
            5 /*_bitNum*/, 0x00000080, 0x00000084, PORTS_SUM_GENERAL, 1, 29, 19),

            /*Start CgPortsIntSum - CG Ports Interrupts Summary address */
            /* NW_CG(network)[0x01800040]       */
            /* Tree place: .5.1 */
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                1 /*_bitNum*/, 0x01800040, 0x01800044, PORTS_NW_CG_GENERAL, 1, 7),

            /*Start CgPortsIntSum - CG Ports Interrupts Summary address */
            /* FB_CG(fabric)[0x01B00040]      */
            /* Tree place: .5.2 */
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                2 /*_bitNum*/, 0x01B00040, 0x01B00044, PORTS_FB_CG_GENERAL, 1, 7),

            /* Tree place: .5.3(FB0), .5.7(FB1), .5.11(FB2), .5.15(FB3) */
            /* Tree place: .5.3.1(FB0), .5.7.1(FB1), .5.11.1(FB2), .5.15.1(FB3) */
            /* Tree place: .5.3.2-5(FB0_L0-3), .5.7.2-5(FB1_L0-3), .5.11.2-5(FB2_L0-3), .5.15.2-5(FB3_L0-3) */

            PRV_SCAN_NODE_FB_PORT_SUBTREE_MAC(3 /*_bitNum*/, 0 /*_portNum*/),

            PRV_SCAN_NODE_FB_PORT_SUBTREE_MAC(7 /*_bitNum*/, 1 /*_portNum*/),

            PRV_SCAN_NODE_FB_PORT_SUBTREE_MAC(11 /*_bitNum*/, 2 /*_portNum*/),

            PRV_SCAN_NODE_FB_PORT_SUBTREE_MAC(15 /*_bitNum*/, 3 /*_portNum*/),

            /* Tree place: .5.17-28(NW0-11) */
            /* Tree place: .5.17-28.1(NW0-11) */
            /* Tree place: .5.<17,19,21,23,25,27>.4(NW 0,2,4,6,8,10) */
            /* Tree place: .5.<17,19,21,23,25,27>.4.7-12(NW 0,2,4,6,8,10.L0-3) */

            PRV_SCAN_NODE_NW_PORT_EVEN_SUBTREE_MAC(17/*_bitNum*/, 0/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_ODD_SUBTREE_MAC(18/*_bitNum*/, 1/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_EVEN_SUBTREE_MAC(19/*_bitNum*/, 2/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_ODD_SUBTREE_MAC(20/*_bitNum*/, 3/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_EVEN_SUBTREE_MAC(21/*_bitNum*/, 4/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_ODD_SUBTREE_MAC(22/*_bitNum*/, 5/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_EVEN_SUBTREE_MAC(23/*_bitNum*/, 6/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_ODD_SUBTREE_MAC(24/*_bitNum*/, 7/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_EVEN_SUBTREE_MAC(25/*_bitNum*/, 8/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_ODD_SUBTREE_MAC(26/*_bitNum*/, 9/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_EVEN_SUBTREE_MAC(27/*_bitNum*/, 10/*_portNum*/),

            PRV_SCAN_NODE_NW_PORT_ODD_SUBTREE_MAC(28/*_bitNum*/, 11/*_portNum*/),

            /*Start InterlakenInt - Interlaken Interrupt Cause address[0x01F00870]*/
            /* Tree place: .5.29*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                29 /*_bitNum*/, 0x01F00870, 0x01F00874, INTERLAKEN_GENERAL, 1, 5),

        /*Start MgIntSum - MG Interrupt Summary Cause address[0x00000038]*/
        /* Tree place: .7*/
        /* Register contains 8 GPP bits, the GPP callback support not added */
        PRV_SCAN_NODE_GEN_MAC(
            7 /*_bitNum*/, 0x00000038, 0x0000003C, MG_GENERAL, 1, 25,
            0 /*_rwBitMask*/, 0xFCFFFFFF/*_maskRcvIntrEn*/, 2/*_subIntrListLen*/),

            /*Start Z80GlobalInt - Z80 Global Interrupt Cause address[0x00000508]*/
            /* Tree place: .7.24*/
            PRV_SCAN_NODE_GEN_MAC(
                24 /*_bitNum*/, 0x00000508, 0x0000050C, Z80_GLOBAL_GENERAL, 1, 31,
                0 /*_rwBitMask*/, 0x7FFFFFFF/*_maskRcvIntrEn*/, 1/*_subIntrListLen*/),

                /*Start Z80Int - Z80 Interrupt Cause address[0x00000510]*/
                /* Tree place: .7.24.31*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    31 /*_bitNum*/, 0x00000510, 0x00000514, Z80_GENERAL, 1, 1),

            /*Start I2CBridgeInt - I2C Bridge Interrupt Cause address[0x000910D8]*/
            /* Tree place: .7.25*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                25 /*_bitNum*/, 0x000910D8, 0x000910DC, I2C_BRIDGE_TRANSACTION_END_OR_ERR, 0, 0),

        /*Start TxSdmaInt - TX SDMA Interrupt Cause address[0x00002810]*/
        /* Tree place: .8*/
        PRV_SCAN_NODE_GEN_LEAF_MAC(
            8 /*_bitNum*/, 0x00002810, 0x00002818, TX_SDMA_GENERAL, 1, 24),

        /*Start RxSdmaInt - RX SDMA Interrupt Cause address[0x0000280C]*/
        /* Tree place: .9*/
        PRV_SCAN_NODE_GEN_LEAF_MAC(
            9 /*_bitNum*/, 0x0000280C, 0x00002814, RX_SDMA_GENERAL, 1, 18),

        /*Start Func1IntSum - FUNC1 Interrupt Summary Cause address[0x000003F4]*/
        /* Tree place: .11*/
        PRV_SCAN_NODE_GEN_SUM_MAC(
            11 /*_bitNum*/, 0x000003F4, 0x000003F0, FUNC1_SUM_GENERAL, 2, 17, 14 /*15*/),

            /*Start HaEpclIntSum - HA and EPCL Interrupt Summary Cause address[0x01C003A8]*/
            /* Tree place: .11.2*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                2 /*_bitNum*/,  0x01C003A8,  0x01C003AC,  HEP_SUM_GENERAL, 2, 2),

            /*Start TtiEngineInt - TTI Engine Interrupt  Cause address[0x01E00004]*/
            /* Tree place: .11.4*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                4 /*_bitNum*/,  0x01E00004,  0x01E00008,  TTI_ENGINE_GENERAL, 1, 4),

            /*Start QagInt - QAG Interrupt  Cause address[0x03B00010]*/
            /* Tree place: .11.5*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                5 /*_bitNum*/,  0x03B00010,  0x03B00020,  QAG_GENERAL, 1, 1),

            /*Start DfxServerSumInt - DFX Server Interrupt Summary Cause address[0x044F8100]*/
            /* Tree place: .11.6*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                6 /*_bitNum*/, 0x044F8100, 0x044F8100, DFX_SERVER_SUM_GENERAL, 1, 2, 2),

                /*Start DfxServerInt - DFX Server Interrupt  Cause address[0x044F8108]*/
                /* Tree place: .11.6.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/,  0x044F8108,  0x044F810C,  DFX_SERVER_GENERAL, 1, 20),

                /*Start DfxServerSpareInt - DFX Server Spare Interrupt  Cause address[0x044F8110]*/
                /* Tree place: .11.6.2*/
                /* bit0 - self summary, bits [31:1] for spares 30-0 */
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/,  0x044F8110,  0x044F8114,  DFX_SRV_0_GENERAL, 1, 31),

            /*Start SharedTabInt - SHT Interrupt Cause address */
            /* SHT0[0x039F0010]              */
            /* Tree place: .11.7 */
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                7 /*_bitNum*/,  0x039F0010,  0x039F0020,  SHT0_GENERAL, 1, 1),


            /*Start SharedTabInt - SHT Interrupt Cause address */
            /* SHT1[0x03AF0010]              */
            /* Tree place: .11.8 */
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                8 /*_bitNum*/,  0x03AF0010,  0x03AF0020,  SHT1_GENERAL, 1, 1),

            /*Start TxqLlInt - TXQ Link List Interrupt  Cause address[0x03DA0508]*/
            /* Tree place: .11.9*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                9 /*_bitNum*/,  0x03DA0508,  0x03DA050C,  TXQ_LL_GENERAL, 1, 6),

            /*Start TxqPfcIntSum - TXQ PFC Interrupt Summary Cause address[0x03E01300]*/
            /* Tree place: .11.10*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                10 /*_bitNum*/, 0x03E01300, 0x03E01304, TXQ_PFC_SUM_GENERAL, 1, 24, 23),

                /*Start TxqPfcParityInt - TXQ PFC Parity Interrupt Cause address[0x03E01310]*/
                /* Tree place: .11.10.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/,  0x03E01310,  0x03E01314,  TXQ_PFC_PARITY_GENERAL, 1, 9),

                /*Start TxqPfcOverflowInt - TXQ PFC Overflow Interrupt Cause address[0x03E01320]*/
                /* Tree place: .11.10.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/,  0x03E01320,  0x03E01324,  TXQ_PFC_OVERFLOW_GENERAL, 1, 16),

                /*Start TxqPfcGlobalOverflowInt - TXQ PFC Global Overflow Interrupt Cause address */
                /* PIPE0[0x03E01330], PIPE1[0x03E01334], PIPE2[0x03E01338], PIPE3[0x03E0133C], */
                /* PIPE4[0x03E01340], PIPE5[0x03E01344], PIPE6[0x03E01348], PIPE7[0x03E0134C], */
                /* Tree place: .11.10.3-10*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/,  0x03E01330,  0x03E01350,  TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    4 /*_bitNum*/,  0x03E01334,  0x03E01354,  TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    5 /*_bitNum*/,  0x03E01338,  0x03E01358,  TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/,  0x03E0133C,  0x03E0135C,  TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    7 /*_bitNum*/,  0x03E01340,  0x03E01360,  TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    8 /*_bitNum*/,  0x03E01344,  0x03E01364,  TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    9 /*_bitNum*/,  0x03E01348,  0x03E01368,  TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    10 /*_bitNum*/,  0x03E0134C,  0x03E0136C,  TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_GENERAL, 1, 8),

                /*Start TxqPfcGlobalUnderflowInt - TXQ PFC Global Underflow Interrupt Cause address */
                /* PIPE0[0x03E01370], PIPE1[0x03E01374], PIPE2[0x03E01378], PIPE3[0x03E0137C], */
                /* PIPE4[0x03E01380], PIPE5[0x03E01384], PIPE6[0x03E01388], PIPE7[0x03E0138C], */
                /* Tree place: .11.10.12-19*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    12 /*_bitNum*/,  0x03E01370,  0x03E01390,  TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    13 /*_bitNum*/,  0x03E01374,  0x03E01394,  TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    14 /*_bitNum*/,  0x03E01378,  0x03E01398,  TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    15 /*_bitNum*/,  0x03E0137C,  0x03E0139C,  TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    16 /*_bitNum*/,  0x03E01380,  0x03E013A0,  TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    17 /*_bitNum*/,  0x03E01384,  0x03E013A4,  TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    18 /*_bitNum*/,  0x03E01388,  0x03E013A8,  TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_GENERAL, 1, 8),

                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    19 /*_bitNum*/,  0x03E0138C,  0x03E013AC,  TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_GENERAL, 1, 8),

                /*Start TxqPfcOverrunsInt - TXQ PFC Overruns Interrupt Cause address[0x03E013B0]*/
                /* Tree place: .11.10.20*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    20 /*_bitNum*/,  0x03E013B0,  0x03E013B4,  TXQ_PFC_OVERRUN_GENERAL, 1, 16),

                /*Start TxqPfcMsgSetsOverrunErr3_0Int - TXQ PFC Msg Sets Overruns Egress3 Cause0 */
                /* Interrupt Cause address[0x03E013B8]                                           */
                /* Tree place: .11.10.21*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    21 /*_bitNum*/,  0x03E013B8,  0x03E013BC,  TXQ_PFC_MSG_OVERRRUN_EGR3_0_GENERAL, 1, 24),

                /*Start TxqPfcMsgSetsOverrunErr3_1Int - TXQ PFC Msg Sets Overruns Egress3 Cause1 */
                /* Interrupt Cause address[0x03E013C0]                                           */
                /* Tree place: .11.10.22*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    22 /*_bitNum*/,  0x03E013C0,  0x03E013C4,  TXQ_PFC_MSG_OVERRRUN_EGR3_1_GENERAL, 1, 24),

                /*Start TxqPfcMsgSetsOverrunErr7_0Int - TXQ PFC Msg Sets Overruns Egress7 Cause0 */
                /* Interrupt Cause address[0x03E013D0]                                           */
                /* Tree place: .11.10.23*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    23 /*_bitNum*/,  0x03E013D0,  0x03E013D4,  TXQ_PFC_MSG_OVERRRUN_EGR7_0_GENERAL, 1, 22),

                /*Start TxqPfcMsgSetsOverrunErr7_1Int - TXQ PFC Msg Sets Overruns Egress7 Cause1 */
                /* Interrupt Cause address[0x03E013D8]                                           */
                /* Tree place: .11.10.24*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    23 /*_bitNum*/,  0x03E013D8,  0x03E013DC,  TXQ_PFC_MSG_OVERRRUN_EGR7_1_GENERAL, 1, 23),

            /*Start TxqTxqSumInt - TXQ Transmit Queue Summary Interrupt Cause address[0x03C90000]*/
            /* Tree place: .11.12*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                12 /*_bitNum*/,  0x03C90000,  0x03C90004,  TXQ_TXQ_SUM_GENERAL, 1, 7, 7),

                /*Start TxqTxqGenInt - TXQ Transmit Queue General Interrupt Cause address[0x03C90008]*/
                /* Tree place: .11.12.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/,  0x03C90008,  0x03C9000C,  TXQ_TXQ_GEN_GENERAL, 1, 24),

                /*Start TxqTxqPortFullBuf0Int - TXQ Transmit Queue Port Full Buf0 (ports 0-31)*/
                /* Interrupt Cause address[0x03C90060]*/
                /* Tree place: .11.12.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/,  0x03C90060,  0x03C90064,  TXQ_TXQ_BUF_FULL_0_GENERAL, 1, 31),

                /*Start TxqTxqPortFullBuf1Int - TXQ Transmit Queue Port Full Buf1 (ports 32-61)*/
                /* Interrupt Cause address[0x03C90068]*/
                /* Tree place: .11.12.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/,  0x03C90068,  0x03C9006C,  TXQ_TXQ_BUF_FULL_1_GENERAL, 1, 31),

                /*Start TxqTxqPortFullBuf2Int - TXQ Transmit Queue Port Full Buf2 (ports 62-71)*/
                /* Interrupt Cause address[0x03C90070]*/
                /* Tree place: .11.12.4*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    4 /*_bitNum*/,  0x03C90070,  0x03C90074,  TXQ_TXQ_BUF_FULL_2_GENERAL, 1, 11),

                /*Start TxqTxqPortFullDesc0Int - TXQ Transmit Queue Port Full Desc0 (ports 0-31)*/
                /* Interrupt Cause address[0x03C90040]*/
                /* Tree place: .11.12.5*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    5 /*_bitNum*/,  0x03C90040,  0x03C90044,  TXQ_TXQ_DESC_FULL_0_GENERAL, 1, 31),

                /*Start TxqTxqPortFullDesc1Int - TXQ Transmit Queue Port Full Desc1 (ports 32-61)*/
                /* Interrupt Cause address[0x03C90048]*/
                /* Tree place: .11.12.6*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/,  0x03C90048,  0x03C9004C,  TXQ_TXQ_DESC_FULL_1_GENERAL, 1, 31),

                /*Start TxqTxqPortFullDesc2Int - TXQ Transmit Queue Port Full Desc2 (ports 62-71)*/
                /* Interrupt Cause address[0x03C90050]*/
                /* Tree place: .11.12.7*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    7 /*_bitNum*/,  0x03C90050,  0x03C90054,  TXQ_TXQ_DESC_FULL_2_GENERAL, 1, 11),

            /*Start TxqDqSumInt - TXQ DQ Summary Interrupt Cause address */
            /*DQ0[0x04000400]*/
            /* Tree place: .11.13*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                13 /*_bitNum*/,  0x04000400,  0x04000404,  TXQ_DQ0_SUM_GENERAL, 1, 6, 4),

                /*Start TxqDqFlushDoneInt - TXQ DQ Flus Done Interrupt Cause address */
                /* DQ0[0x04000408]*/
                /* Tree place: .11.13.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/,  0x04000408,  0x0400040C,  TXQ_DQ0_FLUSH_DONE_GENERAL, 1, 16),

                /*Start TxqDqMemoryErrInt - TXQ DQ Memory Error Interrupt Cause address */
                /* DQ0[0x04000410]*/
                /* Tree place: .11.13.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/,  0x04000410,  0x04000414,  TXQ_DQ0_MEMO_ERR_GENERAL, 1, 2),

                /*Start TxqDqEgrStcInt - TXQ DQ Egress STC Interrupt Cause address */
                /* DQ0[0x04000418]*/
                /* Tree place: .11.13.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/,  0x04000418,  0x04000424,  TXQ_DQ0_EGR_STC_GENERAL, 1, 16),

                /*Start TxqDqGenInt - TXQ DQ General Interrupt Cause address */
                /* DQ0[0x04000430]*/
                /* Tree place: .11.13.6*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/,  0x04000430,  0x04000444,  TXQ_DQ0_EGR_GEN_GENERAL, 1, 2),

            /*Start TxqDqSumInt - TXQ DQ Summary Interrupt Cause address */
            /*DQ1[0x04100400]*/
            /* Tree place: .11.14*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                14 /*_bitNum*/,  0x04100400,  0x04100404,  TXQ_DQ1_SUM_GENERAL, 1, 6, 4),

                /*Start TxqDqFlushDoneInt - TXQ DQ Flus Done Interrupt Cause address */
                /* DQ1[0x04100408]*/
                /* Tree place: .11.14.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/,  0x04100408,  0x0410040C,  TXQ_DQ1_FLUSH_DONE_GENERAL, 1, 16),

                /*Start TxqDqMemoryErrInt - TXQ DQ Memory Error Interrupt Cause address */
                /* DQ1[0x04100410]*/
                /* Tree place: .11.14.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/,  0x04100410,  0x04100414,  TXQ_DQ1_MEMO_ERR_GENERAL, 1, 2),

                /*Start TxqDqEgrStcInt - TXQ DQ Egress STC Interrupt Cause address */
                /* DQ1[0x04100418]*/
                /* Tree place: .11.14.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/,  0x04100418,  0x04100424,  TXQ_DQ1_EGR_STC_GENERAL, 1, 16),

                /*Start TxqDqGenInt - TXQ DQ General Interrupt Cause address */
                /* DQ1[0x04100430]*/
                /* Tree place: .11.14.6*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/,  0x04100430,  0x04100444,  TXQ_DQ1_EGR_GEN_GENERAL, 1, 2),

            /*Start TxqDqSumInt - TXQ DQ Summary Interrupt Cause address */
            /*DQ2[0x04200400]*/
            /* Tree place: .11.15*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                15 /*_bitNum*/,  0x04200400,  0x04200404,  TXQ_DQ2_SUM_GENERAL, 1, 6, 4),

                /*Start TxqDqFlushDoneInt - TXQ DQ Flus Done Interrupt Cause address */
                /* DQ2[0x04200408]*/
                /* Tree place: .11.15.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/,  0x04200408,  0x0420040C,  TXQ_DQ2_FLUSH_DONE_GENERAL, 1, 16),

                /*Start TxqDqMemoryErrInt - TXQ DQ Memory Error Interrupt Cause address */
                /* DQ2[0x04200410]*/
                /* Tree place: .11.15.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/,  0x04200410,  0x04200414,  TXQ_DQ2_MEMO_ERR_GENERAL, 1, 2),

                /*Start TxqDqEgrStcInt - TXQ DQ Egress STC Interrupt Cause address */
                /* DQ2[0x04200418]*/
                /* Tree place: .11.15.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/,  0x04200418,  0x04200424,  TXQ_DQ2_EGR_STC_GENERAL, 1, 16),

                /*Start TxqDqGenInt - TXQ DQ General Interrupt Cause address */
                /* DQ2[0x04200430]*/
                /* Tree place: .11.15.6*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/,  0x04200430,  0x04200444,  TXQ_DQ2_EGR_GEN_GENERAL, 1, 2),

            /*Start TxqDqSumInt - TXQ DQ Summary Interrupt Cause address */
            /*DQ3[0x04300400]*/
            /* Tree place: .11.16*/
            PRV_SCAN_NODE_GEN_SUM_MAC(
                16 /*_bitNum*/,  0x04300400,  0x04300404,  TXQ_DQ3_SUM_GENERAL, 1, 6, 4),

                /*Start TxqDqFlushDoneInt - TXQ DQ Flus Done Interrupt Cause address */
                /* DQ3[0x04300408]*/
                /* Tree place: .11.16.1*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    1 /*_bitNum*/,  0x04300408,  0x0430040C,  TXQ_DQ3_FLUSH_DONE_GENERAL, 1, 16),

                /*Start TxqDqMemoryErrInt - TXQ DQ Memory Error Interrupt Cause address */
                /* DQ3[0x04300410]*/
                /* Tree place: .11.16.2*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    2 /*_bitNum*/,  0x04300410,  0x04300414,  TXQ_DQ3_MEMO_ERR_GENERAL, 1, 2),

                /*Start TxqDqEgrStcInt - TXQ DQ Egress STC Interrupt Cause address */
                /* DQ3[0x04300418]*/
                /* Tree place: .11.16.3*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    3 /*_bitNum*/,  0x04300418,  0x04300424,  TXQ_DQ3_EGR_STC_GENERAL, 1, 16),

                /*Start TxqDqGenInt - TXQ DQ General Interrupt Cause address */
                /* DQ3[0x04300430]*/
                /* Tree place: .11.16.6*/
                PRV_SCAN_NODE_GEN_LEAF_MAC(
                    6 /*_bitNum*/,  0x04300430,  0x04300444,  TXQ_DQ3_EGR_GEN_GENERAL, 1, 2),

            /*Start EtaInt - External TCAM Action Table Interrupt Cause address [0x04501104]*/
            /* Tree place: .11.17*/
            PRV_SCAN_NODE_GEN_LEAF_MAC(
                17 /*_bitNum*/,  0x04501104,  0x04501100,  ETA_GENERAL, 1, 9),

        /*Start XSmiInt - XSMI Interrupt Cause address [0x00040010]*/
        /* Tree place: .12*/
        PRV_SCAN_NODE_GEN_LEAF_MAC(
            12 /*_bitNum*/,  0x00040010,  0x00040014,  XSMI_GENERAL, 1, 2),

};
#endif /* ASIC_SIMULATION */

#define PUMA3_NUM_ELEMENTS_IN_SCAN_TREE_CNS \
    sizeof(puma3IntrScanArrSinglePortGroup)/sizeof(puma3IntrScanArrSinglePortGroup[0])

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 puma3MaskRegMapArr[PUMA3_NUM_MASK_REGISTERS_CNS] =
{
    0
};

/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bounded by appl.                                                         */
static GT_U32 puma3MaskRegDefaultSummaryArr[PUMA3_NUM_MASK_REGISTERS_CNS] =
{
    0
};

/* array per port group for puma3MaskRegMapArr */
static GT_U32 puma3MaskRegMapArr_perPortGroup[PUMA3_NUM_PORT_GROUPS_CNS][PUMA3_NUM_MASK_REGISTERS_CNS] =
{
    {FILLED_IN_RUNTIME_CNS}
};

/*
    array for the 8 port groups of puma3 .
    for each port group : puma3 copied from Lion 2 and updated in runtime
*/
/* Don't use "const" because we Fill values at initialization stage */
static PRV_CPSS_DRV_INTERRUPT_SCAN_STC puma3IntrScanArr[PUMA3_NUM_PORT_GROUPS_CNS][PUMA3_NUM_ELEMENTS_IN_SCAN_TREE_CNS]=
{
    {{FILLED_IN_RUNTIME_CNS}}
};



/* array of interrupt registers that can't be accessed before 'Start Init' */
static GT_U32   notAccessibleBeforeStartInitArray[] =
{
    0x00340134, /*Bridge Interrupt Mask*/
    0x00200008  /* Policy Engine Interrupt Mask */
};

static GT_U32 numOfInterruptRegistersNotAccessibleBeforeStartInit =
                (sizeof(notAccessibleBeforeStartInitArray)/
                sizeof(notAccessibleBeforeStartInitArray[0]));

/*******************************************************************************
* prvCpssDrvPpIntDefPuma3Init
*
* DESCRIPTION:
*       Interrupts initialization for the Puma devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefPuma3Init(void)
{
    GT_U32  portGroupId;/*port group Id*/
    GT_U32  ii;
    GT_STATUS   rc;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */
    GT_U32  dummyMaskRegAddr = 0x000000F4;/*user defined register*/
    GT_BOOL isAdditionalRegDupNeeded; /* not used*/
    GT_U32  additionalRegDup; /* not used*/

    /*
       1. fill the array of mask registers addresses
       2. fill the array of default values for the mask registers
       3. update the bits of nonSumBitMask in the scan tree
    */
    rc = prvCpssDrvExMxDxHwPpMaskRegInfoGet(
            CPSS_PP_FAMILY_PUMA3_E,
            PUMA3_NUM_ELEMENTS_IN_SCAN_TREE_CNS,
            puma3IntrScanArrSinglePortGroup,
            PUMA3_NUM_MASK_REGISTERS_CNS,
            puma3MaskRegDefaultSummaryArr,
            puma3MaskRegMapArr);
    if(rc != GT_OK)
    {
        /* error */
        return ;
    }

    /* Puma3 info , port group 0
       code will loop on all port groups */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_PUMA3_PORT_GROUP_ID_0_E];

    for(portGroupId = 0; portGroupId < PUMA3_NUM_PORT_GROUPS_CNS; portGroupId++ ,
        devFamilyPtr++)
    {
        cpssOsMemCpy(puma3MaskRegMapArr_perPortGroup[portGroupId],
                    puma3MaskRegMapArr,
                    sizeof(puma3MaskRegMapArr));

        for(ii = 0 ; ii < PUMA3_NUM_MASK_REGISTERS_CNS; ii++)
        {
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            /* check if some addresses are not applicable in some port groups */
            if(GT_TRUE == prvCpssPuma3DuplicatedMultiPortGroupsGet(
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,puma3MaskRegMapArr[ii],
                    &portGroupsBmp,&isAdditionalRegDupNeeded,&additionalRegDup))
            {
                if(0 == (portGroupsBmp & (1 << portGroupId)))
                {
                    puma3MaskRegMapArr_perPortGroup[portGroupId][ii] = dummyMaskRegAddr;
                }
            }
        }

        devFamilyPtr->numOfInterrupts = PRV_CPSS_EXMXPM3_LAST_INT_E;

        devFamilyPtr->maskRegistersAddressesArray = puma3MaskRegMapArr_perPortGroup[portGroupId];
        devFamilyPtr->maskRegistersDefaultValuesArray = puma3MaskRegDefaultSummaryArr;
        devFamilyPtr->interruptsScanArray = &puma3IntrScanArr[portGroupId][0];
        devFamilyPtr->numOfScanElements = PUMA3_NUM_ELEMENTS_IN_SCAN_TREE_CNS;
        devFamilyPtr->fdbTrigEndedId = 0;/* don't care !!!! */
        devFamilyPtr->fdbTrigEndedCbPtr = NULL;
        devFamilyPtr->hasFakeInterrupts = GT_FALSE;
        devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
        devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpIntPumaIntCauseToUniEvConvert;
        devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = numOfInterruptRegistersNotAccessibleBeforeStartInit;
        devFamilyPtr->notAccessibleBeforeStartInitPtr=notAccessibleBeforeStartInitArray;

        /* copy interrupts generic tree into the per port group tree */
        cpssOsMemCpy((PRV_CPSS_DRV_INTERRUPT_SCAN_STC *)devFamilyPtr->interruptsScanArray,/*casting to remove the 'const'*/
            puma3IntrScanArrSinglePortGroup,
            PUMA3_NUM_ELEMENTS_IN_SCAN_TREE_CNS * sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC));
    }

    return;
}

#ifdef DUMP_DEFAULT_INFO
/*******************************************************************************
* prvCpssDrvPpIntDefPuma3Print
*
* DESCRIPTION:
*      print the interrupts arrays info, of puma3 devices
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       void
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void  prvCpssDrvPpIntDefPuma3Print(
    void
)
{
    prvCpssDrvPpIntDefPrint(PUMA3_NUM_ELEMENTS_IN_SCAN_TREE_CNS,puma3IntrScanArrSinglePortGroup,
                            PUMA3_NUM_MASK_REGISTERS_CNS,puma3MaskRegMapArr,
                            puma3MaskRegDefaultSummaryArr);
}
#endif/*DUMP_DEFAULT_INFO*/

/* Interrupt cause to unified event map for puma3
 This Table is for unified event with extended data

  Array structure:
  1. The first element of the array and the elements after MARK_END_OF_UNI_EV_CNS are unified event types.
  2. Elements after unified event type are pairs of interrupt cause and event extended data,
     until MARK_END_OF_UNI_EV_CNS.
*/
GT_U16 puma3UniEvMapTableWithExtData[] = {
    CPSS_PP_BM_MAX_BUFF_REACHED_E,
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT0_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(0),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT1_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(1),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT2_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(2),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT3_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(3),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT4_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(4),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT5_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(5),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT6_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(6),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT7_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(7),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT8_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(8),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT9_BUF_LIMIT_E,       MARK_PER_PORT_INT_MAC(9),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT10_BUF_LIMIT_E,      MARK_PER_PORT_INT_MAC(10),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT11_BUF_LIMIT_E,      MARK_PER_PORT_INT_MAC(11),
       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_CPU_PORT_BUF_LIMIT_E, 63,
    MARK_END_OF_UNI_EV_CNS,

 /* Per Port events */
 CPSS_PP_PORT_LINK_STATUS_CHANGED_E,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_LINK_STATUS_CHANGE_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_LINK_STATUS_CHANGE_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_LINK_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(41),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_TX_FIFO_UNDERRUN_E,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_TX_FIFO_UNDERRUN_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_TX_FIFO_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_TX_FIFO_UNDERRUN_E,        MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_ADDRESS_OUT_OF_RANGE_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_ADDRESS_OUT_OF_RANGE_E,        MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_ADDRESS_OUT_OF_RANGE_E,         MARK_PER_PORT_INT_MAC(41),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PRBS_ERROR_E,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_PRBS_ERROR_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_PRBS_ERROR_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_PRBS_ERROR_E,        MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_SYNC_STATUS_CHANGED_E,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_SYNC_STATUS_CHANGE_E,         MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_SYNC_STATUS_CHANGE_E,        MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_SYNC_STATUS_CHANGE_E,        MARK_PER_PORT_INT_MAC(11),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_ILLEGAL_SEQUENCE_E,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_UNKNOWN_SEQUENCE_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_UNKNOWN_SEQUENCE_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_UNKNOWN_SEQUENCE_E,      MARK_PER_PORT_INT_MAC(41),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_FAULT_TYPE_CHANGE_E,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_FAULT_CHANGE_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_FAULT_CHANGE_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_FAULT_CHANGE_E,      MARK_PER_PORT_INT_MAC(41),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_FC_STATUS_CHANGED_E,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_FC_STATUS_CHANGE_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_FC_STATUS_CHANGE_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_FC_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(41),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_TXQ2_FLUSH_PORT_E,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_0_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_1_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_2_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_3_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_4_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_5_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_6_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_7_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_8_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_9_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_10_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_11_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_0_E,     63,
 MARK_END_OF_UNI_EV_CNS,

    CPSS_PP_EGRESS_SFLOW_E,
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT0_E,      MARK_PER_PORT_INT_MAC(0),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT1_E,      MARK_PER_PORT_INT_MAC(1),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT2_E,      MARK_PER_PORT_INT_MAC(2),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT3_E,      MARK_PER_PORT_INT_MAC(3),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT4_E,      MARK_PER_PORT_INT_MAC(4),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT5_E,      MARK_PER_PORT_INT_MAC(5),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT6_E,      MARK_PER_PORT_INT_MAC(6),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT7_E,      MARK_PER_PORT_INT_MAC(7),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT8_E,      MARK_PER_PORT_INT_MAC(8),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT9_E,      MARK_PER_PORT_INT_MAC(9),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT10_E,     MARK_PER_PORT_INT_MAC(10),
        PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT11_E,     MARK_PER_PORT_INT_MAC(11),

        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT0_E,      MARK_PER_PORT_INT_MAC(16),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT1_E,      MARK_PER_PORT_INT_MAC(17),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT2_E,      MARK_PER_PORT_INT_MAC(18),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT3_E,      MARK_PER_PORT_INT_MAC(19),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT4_E,      MARK_PER_PORT_INT_MAC(20),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT5_E,      MARK_PER_PORT_INT_MAC(21),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT6_E,      MARK_PER_PORT_INT_MAC(22),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT7_E,      MARK_PER_PORT_INT_MAC(23),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT8_E,      MARK_PER_PORT_INT_MAC(24),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT9_E,      MARK_PER_PORT_INT_MAC(25),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT10_E,     MARK_PER_PORT_INT_MAC(26),
        PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT11_E,     MARK_PER_PORT_INT_MAC(27),
    MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_MAC_SFLOW_E,
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_0_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_1_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_2_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_3_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_4_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_5_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_6_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_7_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_8_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_9_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_10_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_11_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_12_E,     MARK_PER_PORT_INT_MAC(12),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_13_E,     MARK_PER_PORT_INT_MAC(13),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_14_E,     MARK_PER_PORT_INT_MAC(14),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_15_E,     MARK_PER_PORT_INT_MAC(15),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_16_E,     MARK_PER_PORT_INT_MAC(16),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_17_E,     MARK_PER_PORT_INT_MAC(17),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_18_E,     MARK_PER_PORT_INT_MAC(18),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_19_E,     MARK_PER_PORT_INT_MAC(19),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_20_E,     MARK_PER_PORT_INT_MAC(20),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_21_E,     MARK_PER_PORT_INT_MAC(21),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_22_E,     MARK_PER_PORT_INT_MAC(22),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_23_E,     MARK_PER_PORT_INT_MAC(23),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_24_E,     MARK_PER_PORT_INT_MAC(24),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_25_E,     MARK_PER_PORT_INT_MAC(25),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_26_E,     MARK_PER_PORT_INT_MAC(26),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_27_E,     MARK_PER_PORT_INT_MAC(27),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_28_E,     MARK_PER_PORT_INT_MAC(28),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_29_E,     MARK_PER_PORT_INT_MAC(29),
    PRV_CPSS_PM3INT_EQ_STC_PORT_0_MUL_31_PLUS_30_E,     MARK_PER_PORT_INT_MAC(30),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_0_E,      MARK_PER_PORT_INT_MAC(31),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_1_E,      MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_2_E,      MARK_PER_PORT_INT_MAC(22),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_3_E,      MARK_PER_PORT_INT_MAC(34),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_4_E,      MARK_PER_PORT_INT_MAC(35),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_5_E,      MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_6_E,      MARK_PER_PORT_INT_MAC(37),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_7_E,      MARK_PER_PORT_INT_MAC(38),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_8_E,      MARK_PER_PORT_INT_MAC(39),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_9_E,      MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_10_E,     MARK_PER_PORT_INT_MAC(41),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_11_E,     MARK_PER_PORT_INT_MAC(42),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_12_E,     MARK_PER_PORT_INT_MAC(43),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_13_E,     MARK_PER_PORT_INT_MAC(44),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_14_E,     MARK_PER_PORT_INT_MAC(45),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_15_E,     MARK_PER_PORT_INT_MAC(46),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_16_E,     MARK_PER_PORT_INT_MAC(47),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_17_E,     MARK_PER_PORT_INT_MAC(48),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_18_E,     MARK_PER_PORT_INT_MAC(49),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_19_E,     MARK_PER_PORT_INT_MAC(50),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_20_E,     MARK_PER_PORT_INT_MAC(51),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_21_E,     MARK_PER_PORT_INT_MAC(52),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_22_E,     MARK_PER_PORT_INT_MAC(53),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_23_E,     MARK_PER_PORT_INT_MAC(54),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_24_E,     MARK_PER_PORT_INT_MAC(55),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_25_E,     MARK_PER_PORT_INT_MAC(56),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_26_E,     MARK_PER_PORT_INT_MAC(57),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_27_E,     MARK_PER_PORT_INT_MAC(57),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_28_E,     MARK_PER_PORT_INT_MAC(59),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_29_E,     MARK_PER_PORT_INT_MAC(60),
    PRV_CPSS_PM3INT_EQ_STC_PORT_1_MUL_31_PLUS_30_E,     MARK_PER_PORT_INT_MAC(61),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_NO_BUFF_PACKET_DROP_E,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_NO_BUFFER_PKT_DROP_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_NO_BUFFER_PKT_DROP_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_NO_BUFFER_PKT_DROP_E,      MARK_PER_PORT_INT_MAC(41),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_PORT_DESC_FULL_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_0_E, MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_1_E, MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_2_E, MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_3_E, MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_4_E, MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_5_E, MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_6_E, MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_7_E, MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_8_E, MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_9_E, MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_10_E, MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_11_E, MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_12_E, MARK_PER_PORT_INT_MAC(12),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_13_E, MARK_PER_PORT_INT_MAC(13),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_14_E, MARK_PER_PORT_INT_MAC(14),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_15_E, MARK_PER_PORT_INT_MAC(15),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_16_E, MARK_PER_PORT_INT_MAC(16),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_17_E, MARK_PER_PORT_INT_MAC(17),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_18_E, MARK_PER_PORT_INT_MAC(18),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_19_E, MARK_PER_PORT_INT_MAC(19),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_20_E, MARK_PER_PORT_INT_MAC(20),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_21_E, MARK_PER_PORT_INT_MAC(21),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_22_E, MARK_PER_PORT_INT_MAC(22),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_23_E, MARK_PER_PORT_INT_MAC(23),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_24_E, MARK_PER_PORT_INT_MAC(24),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_25_E, MARK_PER_PORT_INT_MAC(25),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_26_E, MARK_PER_PORT_INT_MAC(26),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_27_E, MARK_PER_PORT_INT_MAC(27),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_28_E, MARK_PER_PORT_INT_MAC(28),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_29_E, MARK_PER_PORT_INT_MAC(29),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_0_MUL_31_PLUS_30_E, MARK_PER_PORT_INT_MAC(30),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_0_E, MARK_PER_PORT_INT_MAC(31),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_1_E, MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_2_E, MARK_PER_PORT_INT_MAC(33),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_3_E, MARK_PER_PORT_INT_MAC(34),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_4_E, MARK_PER_PORT_INT_MAC(35),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_5_E, MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_6_E, MARK_PER_PORT_INT_MAC(37),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_7_E, MARK_PER_PORT_INT_MAC(38),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_8_E, MARK_PER_PORT_INT_MAC(39),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_9_E, MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_10_E, MARK_PER_PORT_INT_MAC(41),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_11_E, MARK_PER_PORT_INT_MAC(42),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_12_E, MARK_PER_PORT_INT_MAC(43),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_13_E, MARK_PER_PORT_INT_MAC(44),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_14_E, MARK_PER_PORT_INT_MAC(45),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_15_E, MARK_PER_PORT_INT_MAC(46),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_16_E, MARK_PER_PORT_INT_MAC(47),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_17_E, MARK_PER_PORT_INT_MAC(48),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_18_E, MARK_PER_PORT_INT_MAC(49),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_19_E, MARK_PER_PORT_INT_MAC(50),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_20_E, MARK_PER_PORT_INT_MAC(51),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_21_E, MARK_PER_PORT_INT_MAC(52),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_22_E, MARK_PER_PORT_INT_MAC(53),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_23_E, MARK_PER_PORT_INT_MAC(54),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_24_E, MARK_PER_PORT_INT_MAC(55),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_25_E, MARK_PER_PORT_INT_MAC(56),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_26_E, MARK_PER_PORT_INT_MAC(57),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_27_E, MARK_PER_PORT_INT_MAC(58),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_28_E, MARK_PER_PORT_INT_MAC(59),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_29_E, MARK_PER_PORT_INT_MAC(60),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_1_MUL_31_PLUS_30_E, MARK_PER_PORT_INT_MAC(61),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_0_E, MARK_PER_PORT_INT_MAC(62),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_1_E, MARK_PER_PORT_INT_MAC(63),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_2_E, MARK_PER_PORT_INT_MAC(64),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_3_E, MARK_PER_PORT_INT_MAC(65),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_4_E, MARK_PER_PORT_INT_MAC(66),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_5_E, MARK_PER_PORT_INT_MAC(67),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_6_E, MARK_PER_PORT_INT_MAC(68),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_7_E, MARK_PER_PORT_INT_MAC(69),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_8_E, MARK_PER_PORT_INT_MAC(70),
    PRV_CPSS_PM3INT_TXQ_TXQ_DESC_FULL_PORT_2_MUL_31_PLUS_9_E, MARK_PER_PORT_INT_MAC(71),
 MARK_END_OF_UNI_EV_CNS,

 /* Per Port PCS events */
 CPSS_PP_PORT_PCS_LINK_STATUS_CHANGED_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_LINK_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_LINK_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_LINK_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_LINK_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_LINK_STATUS_CHANGE_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_LINK_STATUS_CHANGE_E,     MARK_PER_PORT_INT_MAC(10),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_DESKEW_TIMEOUT_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_DESKEW_TIMEOUT_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_DESKEW_TIMEOUT_E,     MARK_PER_PORT_INT_MAC(10),
 MARK_END_OF_UNI_EV_CNS,


/* Per Port MMPCS events */ 
 CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_GB_LOCK_SYNC_CHANGE_E ,     MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_GB_LOCK_SYNC_CHANGE_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_GB_LOCK_SYNC_CHANGE_E,      MARK_PER_PORT_INT_MAC(8),    
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_PORT_PCS_DETECTED_COLUMN_IIAII_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_DETECTED_COLUMN_IIAII_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_DETECTED_COLUMN_IIAII_E,     MARK_PER_PORT_INT_MAC(10),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_DESKEW_ERROR_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_DESKEW_ERROR_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_DESKEW_ERROR_E,     MARK_PER_PORT_INT_MAC(10),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_PPM_FIFO_UNDERRUN_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_PPM_FIFO_UNDERRUN_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_PPM_FIFO_UNDERRUN_E,     MARK_PER_PORT_INT_MAC(10),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_PCS_PPM_FIFO_OVERRUN_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_PPM_FIFO_OVERRUN_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_PPM_FIFO_OVERRUN_E,     MARK_PER_PORT_INT_MAC(10),
 MARK_END_OF_UNI_EV_CNS,

    /* Per Port Per lane events */
 CPSS_PP_PORT_LANE_PRBS_ERROR_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_PRBS_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_PORT_LANE_DISPARITY_ERROR_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_DISPARITY_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_SYMBOL_ERROR_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_SYMBOL_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_CJR_PAT_ERROR_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_CJR_PAT_ERROR_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_SYGNAL_DETECT_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_SYNC_STATUS_CHANGED_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PORT_LANE_DETECTED_IIAII_E,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(0,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(2,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(4,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(6,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(8,3),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,0),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,1),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,2),
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_DETECTED_IIAII_E, MARK_PER_PORT_PER_LANE_INT_MAC(10,3),
 MARK_END_OF_UNI_EV_CNS,

 /* Per Queue events */
 CPSS_PP_TX_BUFFER_QUEUE_E,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE0_E,  0,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE1_E,  1,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE2_E,  2,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE3_E,  3,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE4_E,  4,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE5_E,  5,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE6_E,  6,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TX_ERR_QUEUE_E,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE0_E,  0,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE1_E,  1,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE2_E,  2,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE3_E,  3,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE4_E,  4,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE5_E,  5,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE6_E,  6,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TX_END_E,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE0_E,  0,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE1_E,  1,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE2_E,  2,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE3_E,  3,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE4_E,  4,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE5_E,  5,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE6_E,  6,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

  /* Per Index events */
 CPSS_PP_GPP_E,
    PRV_CPSS_PM3INT_MG_GPP0_E,  0,
    PRV_CPSS_PM3INT_MG_GPP1_E,  1,
    PRV_CPSS_PM3INT_MG_GPP2_E,  2,
    PRV_CPSS_PM3INT_MG_GPP3_E,  3,
    PRV_CPSS_PM3INT_MG_GPP4_E,  4,
    PRV_CPSS_PM3INT_MG_GPP5_E,  5,
    PRV_CPSS_PM3INT_MG_GPP6_E,  6,
    PRV_CPSS_PM3INT_MG_GPP7_E,  7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_CNC_WRAPAROUND_BLOCK_E,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_0_E, 0,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_1_E, 1,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_2_E, 2,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_3_E, 3,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_4_E, 4,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_5_E, 5,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_6_E, 6,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_7_E, 7,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_8_E, 8,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_9_E, 9,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_10_E, 10,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_11_E, 11,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_12_E, 12,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_13_E, 13,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_14_E, 14,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_15_E, 15,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_16_E, 16,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_17_E, 17,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_18_E, 18,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_19_E, 19,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_20_E, 20,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_21_E, 21,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_22_E, 22,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_23_E, 23,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_24_E, 24,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_25_E, 25,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_26_E, 26,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_27_E, 27,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_28_E, 28,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_29_E, 29,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_0_MUL_31_PLUS_30_E, 30,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_0_E, 31,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_1_E, 32,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_2_E, 33,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_3_E, 34,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_4_E, 35,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_5_E, 35,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_6_E, 37,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_7_E, 38,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_8_E, 39,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_9_E, 40,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_10_E, 41,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_11_E, 42,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_12_E, 43,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_13_E, 44,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_14_E, 45,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_15_E, 46,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_16_E, 47,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_17_E, 48,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_18_E, 49,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_19_E, 50,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_20_E, 51,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_21_E, 52,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_22_E, 53,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_23_E, 54,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_24_E, 55,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_25_E, 56,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_26_E, 57,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_27_E, 58,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_28_E, 59,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_29_E, 60,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_1_MUL_31_PLUS_30_E, 61,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_2_MUL_31_PLUS_0_E, 62,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND_BLOCK_2_MUL_31_PLUS_1_E, 63,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_CNT_UNDERRUN_E,      0,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GE_GROUP_CNT_UNDERRUN_E,  1,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_CNT_UNDERRUN_E,    3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_CNT_OVERRUN_E,       0,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GE_GROUP_CNT_OVERRUN_E,   1,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_CNT_OVERRUN_E,     3,
 MARK_END_OF_UNI_EV_CNS,


 CPSS_PP_BM_MISC_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_CTRL_ACCESS_ERR_E,   2,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_RX_FULL_E,      4,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_LL_P1_ACCESS_ERR_E,  16,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_LL_P2_ACCESS_ERR_E,  17,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_CTRL_ACCESS_ERR_E,   18,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_PORT_RX_FULL_E,      20,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_TQ_MISC_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_WRONG_ADDRESS_E,          0,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_GLOBAL_DESC_FULL_E,    1,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_GLOBAL_BUFF_FULL_E,    2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_SCT_RATE_LIMITER_E,
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_0_E,      MARK_PER_PORT_INT_MAC(0),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_1_E,      MARK_PER_PORT_INT_MAC(1),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_2_E,      MARK_PER_PORT_INT_MAC(2),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_3_E,      MARK_PER_PORT_INT_MAC(3),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_4_E,      MARK_PER_PORT_INT_MAC(4),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_5_E,      MARK_PER_PORT_INT_MAC(5),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_6_E,      MARK_PER_PORT_INT_MAC(6),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_7_E,      MARK_PER_PORT_INT_MAC(7),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_8_E,      MARK_PER_PORT_INT_MAC(8),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_9_E,      MARK_PER_PORT_INT_MAC(9),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_10_E,     MARK_PER_PORT_INT_MAC(10),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_11_E,     MARK_PER_PORT_INT_MAC(11),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_12_E,     MARK_PER_PORT_INT_MAC(12),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_13_E,     MARK_PER_PORT_INT_MAC(13),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_14_E,     MARK_PER_PORT_INT_MAC(14),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_15_E,     MARK_PER_PORT_INT_MAC(15),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_16_E,     MARK_PER_PORT_INT_MAC(16),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_17_E,     MARK_PER_PORT_INT_MAC(17),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_18_E,     MARK_PER_PORT_INT_MAC(18),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_19_E,     MARK_PER_PORT_INT_MAC(19),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_20_E,     MARK_PER_PORT_INT_MAC(20),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_21_E,     MARK_PER_PORT_INT_MAC(21),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_22_E,     MARK_PER_PORT_INT_MAC(22),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_23_E,     MARK_PER_PORT_INT_MAC(23),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_24_E,     MARK_PER_PORT_INT_MAC(24),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_25_E,     MARK_PER_PORT_INT_MAC(25),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_26_E,     MARK_PER_PORT_INT_MAC(26),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_27_E,     MARK_PER_PORT_INT_MAC(27),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_28_E,     MARK_PER_PORT_INT_MAC(28),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_29_E,     MARK_PER_PORT_INT_MAC(29),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_0_MUL_31_PLUS_30_E,     MARK_PER_PORT_INT_MAC(30),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_0_E,      MARK_PER_PORT_INT_MAC(31),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_1_E,      MARK_PER_PORT_INT_MAC(32),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_2_E,      MARK_PER_PORT_INT_MAC(22),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_3_E,      MARK_PER_PORT_INT_MAC(34),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_4_E,      MARK_PER_PORT_INT_MAC(35),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_5_E,      MARK_PER_PORT_INT_MAC(36),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_6_E,      MARK_PER_PORT_INT_MAC(37),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_7_E,      MARK_PER_PORT_INT_MAC(38),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_8_E,      MARK_PER_PORT_INT_MAC(39),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_9_E,      MARK_PER_PORT_INT_MAC(40),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_10_E,     MARK_PER_PORT_INT_MAC(41),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_11_E,     MARK_PER_PORT_INT_MAC(42),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_12_E,     MARK_PER_PORT_INT_MAC(43),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_13_E,     MARK_PER_PORT_INT_MAC(44),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_14_E,     MARK_PER_PORT_INT_MAC(45),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_15_E,     MARK_PER_PORT_INT_MAC(46),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_16_E,     MARK_PER_PORT_INT_MAC(47),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_17_E,     MARK_PER_PORT_INT_MAC(48),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_18_E,     MARK_PER_PORT_INT_MAC(49),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_19_E,     MARK_PER_PORT_INT_MAC(50),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_20_E,     MARK_PER_PORT_INT_MAC(51),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_21_E,     MARK_PER_PORT_INT_MAC(52),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_22_E,     MARK_PER_PORT_INT_MAC(53),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_23_E,     MARK_PER_PORT_INT_MAC(54),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_24_E,     MARK_PER_PORT_INT_MAC(55),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_25_E,     MARK_PER_PORT_INT_MAC(56),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_26_E,     MARK_PER_PORT_INT_MAC(57),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_27_E,     MARK_PER_PORT_INT_MAC(57),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_28_E,     MARK_PER_PORT_INT_MAC(59),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_29_E,     MARK_PER_PORT_INT_MAC(60),
    PRV_CPSS_PM3INT_EQ_RATE_LIMIT_RATE_1_MUL_31_PLUS_30_E,     MARK_PER_PORT_INT_MAC(61),
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E,
    PRV_CPSS_PM3INT_INGRESS_POLICER_ADDRESS_OUT_OF_MEMORY_E,    0,
    PRV_CPSS_PM3INT_EGRESS_POLICER_ADDRESS_OUT_OF_MEMORY_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_DATA_ERR_E,
    PRV_CPSS_PM3INT_INGRESS_POLICER_DATA_ERROR_E,    0,
    PRV_CPSS_PM3INT_EGRESS_POLICER_DATA_ERROR_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E,
    PRV_CPSS_PM3INT_INGRESS_POLICER_IPFIX_COUNTER_WRAPAROUND_E,    0,
    PRV_CPSS_PM3INT_EGRESS_POLICER_IPFIX_COUNTER_WRAPAROUND_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_ALARM_E,
    PRV_CPSS_PM3INT_INGRESS_POLICER_IPFIX_ALARM_E,    0,
    PRV_CPSS_PM3INT_EGRESS_POLICER_IPFIX_ALARM_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E,
    PRV_CPSS_PM3INT_INGRESS_POLICER_IPFIX_ALARM_FIFO_WRAPAROUND_E,    0,
    PRV_CPSS_PM3INT_EGRESS_POLICER_IPFIX_ALARM_FIFO_WRAPAROUND_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PCL_LOOKUP_DATA_ERROR_E,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL0_TCAM_ACCESS_DATA_ERROR_LU0_E,  0,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL0_TCAM_ACCESS_DATA_ERROR_LU1_E,  1,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL1_TCAM_ACCESS_DATA_ERROR_LU0_E,  2,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL1_TCAM_ACCESS_DATA_ERROR_LU1_E,  3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_PCL_LOOKUP_FIFO_FULL_E,
    PRV_CPSS_PM3INT_POLICY_ENGINE_FIFO_FULL_LU0_E,       0,
    PRV_CPSS_PM3INT_POLICY_ENGINE_FIFO_FULL_LU1_E,       1,
 MARK_END_OF_UNI_EV_CNS,
#if 0
 CPSS_PP_TCC_TCAM_ERROR_DETECTED_E,
    PRV_CPSS_PM3INT_TCC0_TCAM_ERROR_DETECTED_E,     0,
    PRV_CPSS_PM3INT_TCC1_TCAM_ERROR_DETECTED_E,     1,
 MARK_END_OF_UNI_EV_CNS,
#endif /*0*/

 CPSS_PP_TCC_TCAM_BIST_FAILED_E,
    PRV_CPSS_PM3INT_TCC0_TBL_TC_BIST2RF_BAD_ADDR_E,     0,
    PRV_CPSS_PM3INT_TCC1_TBL_TC_BIST2RF_BAD_ADDR_E,     1,
 MARK_END_OF_UNI_EV_CNS,
 CPSS_PP_RX_BUFFER_QUEUE0_E,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE0_E,     0,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE1_E,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE1_E,     1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE2_E,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE2_E,     2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE3_E,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE3_E,     3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE4_E,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE4_E,     4,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE5_E,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE5_E,     5,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE6_E,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE6_E,     6,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_BUFFER_QUEUE7_E,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE7_E,     7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE0_E,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE0_E,      0,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE1_E,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE1_E,      1,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE2_E,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE2_E,      2,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE3_E,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE3_E,      3,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE4_E,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE4_E,      4,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE5_E,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE5_E,      5,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE6_E,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE6_E,      6,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_RX_ERR_QUEUE7_E,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE7_E,      7,
 MARK_END_OF_UNI_EV_CNS,

 CPSS_PP_CRITICAL_HW_ERROR_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_BURST_FIFO_UNDERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_BURST_FIFO_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_BURST_FIFO_UNDERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_BURST_FIFO_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_CLEAR_FIFO_FULL_E,           PRV_CPSS_PM3INT_TX_DMA0_ERROR_CLEAR_FIFO_FULL_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_CLEAR_FIFO_FULL_E,           PRV_CPSS_PM3INT_TX_DMA1_ERROR_CLEAR_FIFO_FULL_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO0_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO0_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO0_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO0_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO1_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO1_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO1_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO1_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO2_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO2_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO2_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO2_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO3_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO3_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO3_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO3_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO4_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO4_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO4_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO4_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO5_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO5_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO5_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO5_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO6_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO6_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO6_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO6_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO7_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO7_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO7_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO7_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO8_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO8_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO8_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO8_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO9_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO9_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO9_UNDERRUN_E,        PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO9_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO10_UNDERRUN_E,       PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO10_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO10_UNDERRUN_E,       PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO10_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO11_UNDERRUN_E,       PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO11_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO11_UNDERRUN_E,       PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO11_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO12_UNDERRUN_E,       PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO12_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO12_UNDERRUN_E,       PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO12_UNDERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_CUT_THROW_ERR_E,             PRV_CPSS_PM3INT_TX_DMA0_ERROR_CUT_THROW_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_CUT_THROW_ERR_E,             PRV_CPSS_PM3INT_TX_DMA1_ERROR_CUT_THROW_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E, PRV_CPSS_PM3INT_TX_DMA0_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E, PRV_CPSS_PM3INT_TX_DMA1_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_HA_INFO_FIFO_ECC_ONE_ERR_E,  PRV_CPSS_PM3INT_TX_DMA0_ERROR_HA_INFO_FIFO_ECC_ONE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E, PRV_CPSS_PM3INT_TX_DMA0_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO0_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO0_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO1_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO1_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO2_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO2_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO3_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO3_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO4_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO4_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO5_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO5_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO6_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO6_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO7_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO7_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO0_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO0_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO1_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO1_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO2_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO2_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO3_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO3_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO4_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO4_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO5_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO5_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO6_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO6_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO7_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO7_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_TXDMA2HA_ECC_SINGLE_ERR_E,   PRV_CPSS_PM3INT_TX_DMA0_ERROR_TXDMA2HA_ECC_SINGLE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_TXDMA2HA_ECC_DOUBLE_ERR_E,   PRV_CPSS_PM3INT_TX_DMA0_ERROR_TXDMA2HA_ECC_DOUBLE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_HA_INFO_FIFO_ECC_ONE_ERR_E,  PRV_CPSS_PM3INT_TX_DMA1_ERROR_HA_INFO_FIFO_ECC_ONE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E, PRV_CPSS_PM3INT_TX_DMA1_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO0_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO0_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO1_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO1_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO2_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO2_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO3_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO3_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO4_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO4_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO5_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO5_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO6_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO6_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO7_BK0_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO7_BK0_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO0_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO0_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO1_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO1_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO2_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO2_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO3_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO3_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO4_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO4_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO5_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO5_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO6_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO6_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO7_BK1_OVERRUN_E,      PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO7_BK1_OVERRUN_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_TXDMA2HA_ECC_SINGLE_ERR_E,   PRV_CPSS_PM3INT_TX_DMA1_ERROR_TXDMA2HA_ECC_SINGLE_ERR_E,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_TXDMA2HA_ECC_DOUBLE_ERR_E,   PRV_CPSS_PM3INT_TX_DMA1_ERROR_TXDMA2HA_ECC_DOUBLE_ERR_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_CNT_UNDERRUN_E,       PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_CNT_UNDERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_CNT_OVERRUN_E,        PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_CNT_OVERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GE_GROUP_CNT_UNDERRUN_E,   PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GE_GROUP_CNT_UNDERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_PORT_CNT_UNDERRUN_E,       PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_PORT_CNT_UNDERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_PORT_CNT_OVERRUN_E,        PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_PORT_CNT_OVERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GE_GROUP_CNT_UNDERRUN_E,   PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GE_GROUP_CNT_UNDERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_CNT_UNDERRUN_E,     PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_CNT_UNDERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_CNT_OVERRUN_E,      PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_CNT_OVERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GLOBAL_CNT_UNDERRUN_E,     PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GLOBAL_CNT_UNDERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GLOBAL_CNT_OVERRUN_E,      PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GLOBAL_CNT_OVERRUN_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_CTRL_ACCESS_ERR_E,         PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_CTRL_ACCESS_ERR_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_LL_P1_ACCESS_ERR_E,        PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_LL_P1_ACCESS_ERR_E,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_LL_P2_ACCESS_ERR_E,        PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_LL_P2_ACCESS_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO0_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO0_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO1_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO1_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO2_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO2_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO3_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO3_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT0_EGR_WRONG_ADDR_E,                 PRV_CPSS_PM3INT_EGR_FLT0_EGR_WRONG_ADDR_E,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO0_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO0_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO1_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO1_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO2_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO2_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO3_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO3_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT1_EGR_WRONG_ADDR_E,                 PRV_CPSS_PM3INT_EGR_FLT1_EGR_WRONG_ADDR_E,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO0_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO0_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO1_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO1_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO2_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO2_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO3_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO3_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT2_EGR_WRONG_ADDR_E,                 PRV_CPSS_PM3INT_EGR_FLT2_EGR_WRONG_ADDR_E,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO0_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO0_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO1_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO1_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO2_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO2_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO3_PARITY_ERR_E,            PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO3_PARITY_ERR_E,
    PRV_CPSS_PM3INT_EGR_FLT3_EGR_WRONG_ADDR_E,                 PRV_CPSS_PM3INT_EGR_FLT3_EGR_WRONG_ADDR_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_TAIL_DROP_LATENCY_FIFO_OVERFLOW_E, PRV_CPSS_PM3INT_TXQ_TXQ_GEN_TAIL_DROP_LATENCY_FIFO_OVERFLOW_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_LATENCY_FIFO_OVERRUN_E,        PRV_CPSS_PM3INT_TXQ_TXQ_GEN_LATENCY_FIFO_OVERRUN_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_LIMIT_ID_FIFO_OVERRUN_E,       PRV_CPSS_PM3INT_TXQ_TXQ_GEN_LIMIT_ID_FIFO_OVERRUN_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE0_DROP_E,            PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE0_DROP_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE1_DROP_E,            PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE1_DROP_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE2_DROP_E,            PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE2_DROP_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE3_DROP_E,            PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE3_DROP_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO0_E,           PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO0_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO1_E,           PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO1_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO2_E,           PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO2_E,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO3_E,           PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO3_E,
    PRV_CPSS_PM3INT_TXQ_LL_LATENCY_FIFO_OVERRUN_E,             PRV_CPSS_PM3INT_TXQ_LL_LATENCY_FIFO_OVERRUN_E,
    PRV_CPSS_PM3INT_TXQ_LL_LIMIT_ID_FIFO_OVERRUN_E,            PRV_CPSS_PM3INT_TXQ_LL_LIMIT_ID_FIFO_OVERRUN_E,
 MARK_END_OF_UNI_EV_CNS
};
/* Interrupt cause to unified event map for puma3 with extended data size */
GT_U32 puma3UniEvMapTableWithExtDataSize = (sizeof(puma3UniEvMapTableWithExtData)/(sizeof(GT_U16)));


/* Interrupt cause to unified event map for puma3
 This Table is for unified event without extended data
*/
GT_U16 puma3UniEvMapTable[][2] =
{
 {CPSS_PP_MISC_TWSI_TIME_OUT_E,          PRV_CPSS_PM3INT_MG_TWSI_TIMEOUT_E},
 {CPSS_PP_MISC_TWSI_STATUS_E,            PRV_CPSS_PM3INT_MG_TWSI_STATUS_E},
 {CPSS_PP_BM_AGED_PACKET_E,              PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_AGED_PKT_E},
 {CPSS_PP_BM_MAX_BUFF_REACHED_E,         PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORTGROUP_BUF_LIMIT_E},
 {CPSS_PP_BM_WRONG_SRC_PORT_E,           PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_WRONG_SRC_PORT_E},
 {CPSS_PP_TQ_SNIFF_DESC_DROP_E,          PRV_CPSS_PM3INT_TXQ_DQ0_EGR_GEN_EGR_MIRROR_DESC_DROP_E},
 {CPSS_PP_EB_SECURITY_BREACH_UPDATE_E,   PRV_CPSS_PM3INT_BRIDGE_UPDATE_SECURITY_BREACH_REGISTER_E},
 {CPSS_PP_MAC_NUM_OF_HOP_EXP_E,          PRV_CPSS_PM3INT_FDB_NUM_OF_HOP_EXP_E},
 {CPSS_PP_MAC_NA_LEARNED_E,              PRV_CPSS_PM3INT_FDB_NA_LEARNT_E},
 {CPSS_PP_MAC_NA_NOT_LEARNED_E,          PRV_CPSS_PM3INT_FDB_NA_NOT_LEARNT_E},
 {CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E,   PRV_CPSS_PM3INT_FDB_AGE_VIA_TRIGGER_ENDED_E},
 {CPSS_PP_MAC_UPDATE_FROM_CPU_DONE_E,    PRV_CPSS_PM3INT_FDB_AU_PROC_COMPLETED_E},
 {CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E,    PRV_CPSS_PM3INT_FDB_AU_MSG_TO_CPU_READY_E},
 {CPSS_PP_MAC_NA_SELF_LEARNED_E,         PRV_CPSS_PM3INT_FDB_NA_SELF_LEARNT_E},
 {CPSS_PP_MAC_NA_FROM_CPU_LEARNED_E,     PRV_CPSS_PM3INT_FDB_NA_FROM_CPU_LEARNED_E},
 {CPSS_PP_MAC_NA_FROM_CPU_DROPPED_E,     PRV_CPSS_PM3INT_FDB_NA_FROM_CPU_DROPPED_E},
 {CPSS_PP_MAC_AGED_OUT_E,                PRV_CPSS_PM3INT_FDB_AGED_OUT_E},
 {CPSS_PP_MAC_FIFO_2_CPU_EXCEEDED_E,     PRV_CPSS_PM3INT_FDB_AU_FIFO_TO_CPU_IS_FULL_E},
 {CPSS_PP_EB_AUQ_PENDING_E,              PRV_CPSS_PM3INT_MG_AU_QUEUE_PENDING_E},
 {CPSS_PP_EB_AUQ_FULL_E,                 PRV_CPSS_PM3INT_MG_AU_QUEUE_FULL_E},
 {CPSS_PP_EB_AUQ_OVER_E,                 PRV_CPSS_PM3INT_MG_AUQ_FIFO_OVERRUN_E},
 {CPSS_PP_EB_FUQ_PENDING_E,              PRV_CPSS_PM3INT_MG_FU_QUEUE_PENDING_E},
 {CPSS_PP_EB_FUQ_FULL_E,                 PRV_CPSS_PM3INT_MG_FU_QUEUE_FULL_E},
 {CPSS_PP_MISC_GENXS_READ_DMA_DONE_E,    PRV_CPSS_PM3INT_MG_GENXS_READ_DMA_DONE_E},
 {CPSS_PP_MISC_PEX_ADDR_UNMAPPED_E,      PRV_CPSS_PM3INT_MG_PEX_ADDR_UNMAPPED_E},
 {CPSS_PP_EB_MG_ADDR_OUT_OF_RANGE_E,     PRV_CPSS_PM3INT_BRIDGE_ADDRESS_OUT_OF_RANGE_E},
 {CPSS_PP_PEX_DL_DOWN_TX_ACC_ERR_E,      PRV_CPSS_PM3INT_PEX_TX_REQ_WHEN_DL_DOWN_E},
 {CPSS_PP_PEX_MASTER_DISABLED_E,         PRV_CPSS_PM3INT_PEX_TRANS_WHEN_MASTER_DISABLE_E},
 {CPSS_PP_PEX_ERROR_WR_TO_REG_E,         PRV_CPSS_PM3INT_PEX_ERR_WRITE_INTERN_REG_E},
 {CPSS_PP_PEX_HIT_DEFAULT_WIN_ERR_E,     PRV_CPSS_PM3INT_PEX_HIT_DEFAULT_WIN_ERR_E},
 {CPSS_PP_PEX_COR_ERROR_DET_E,           PRV_CPSS_PM3INT_PEX_CORRECTABLE_ERR_E},
 {CPSS_PP_PEX_NON_FATAL_ERROR_DET_E,     PRV_CPSS_PM3INT_PEX_NON_FATAL_ERR_E},
 {CPSS_PP_PEX_FATAL_ERROR_DET_E,         PRV_CPSS_PM3INT_PEX_FATAL_ERR_E},
 {CPSS_PP_PEX_DSTATE_CHANGED_E,          PRV_CPSS_PM3INT_PEX_D_STATE_CHANGED_E},
 {CPSS_PP_PEX_BIST_E,                    PRV_CPSS_PM3INT_PEX_BIST_ACTIVATED_E},
 {CPSS_PP_PEX_RCV_ERROR_FATAL_E,         PRV_CPSS_PM3INT_PEX_RCV_FATAL_ERR_MSG_E},
 {CPSS_PP_PEX_RCV_ERROR_NON_FATAL_E,     PRV_CPSS_PM3INT_PEX_RCV_NON_FATAL_ERR_MSG_E},
 {CPSS_PP_PEX_RCV_ERROR_COR_E,           PRV_CPSS_PM3INT_PEX_RCV_ERR_COR_MSG_E},
 {CPSS_PP_PEX_RCV_CRS_E,                 PRV_CPSS_PM3INT_PEX_RCV_CFG_REQ_RETRY_E},
 {CPSS_PP_PEX_PEX_SLAVE_HOT_RESET_E,     PRV_CPSS_PM3INT_PEX_RCV_HOT_RESET_MSG_E},
 {CPSS_PP_PEX_PEX_SLAVE_DISABLE_LINK_E,  PRV_CPSS_PM3INT_PEX_SLAVE_DIABLE_LINK_E},
 {CPSS_PP_PEX_PEX_SLAVE_LOOPBACK_E,      PRV_CPSS_PM3INT_PEX_SLAVE_LOOPBACK_E},
 {CPSS_PP_PEX_PEX_LINK_FAIL_E,           PRV_CPSS_PM3INT_PEX_LINK_FAIL_E},
 {CPSS_PP_PEX_RCV_A_E,                   PRV_CPSS_PM3INT_PEX_RCV_INT_A_MSG_E},
 {CPSS_PP_PEX_RCV_B_E,                   PRV_CPSS_PM3INT_PEX_RCV_INT_B_MSG_E},
 {CPSS_PP_PEX_RCV_C_E,                   PRV_CPSS_PM3INT_PEX_RCV_INT_C_MSG_E},
 {CPSS_PP_PEX_RCV_D_E,                   PRV_CPSS_PM3INT_PEX_RCV_INT_D_MSG_E},
 {CPSS_PP_CNC_DUMP_FINISHED_E,           PRV_CPSS_PM3INT_CNC_MISC_DUMP_FINISHED_E},
 {CPSS_PP_TTI_CPU_ADDRESS_OUT_OF_RANGE_E,PRV_CPSS_PM3INT_TTI_CPU_ADDRESS_OUT_OF_RANGE_E},
 {CPSS_PP_TTI_ACCESS_DATA_ERROR_E,       PRV_CPSS_PM3INT_TTI_TTI_TCAM_ACCESS_DATA_ERROR_E},
 {CPSS_PP_PCL_MG_ADDR_OUT_OF_RANGE_E,    PRV_CPSS_PM3INT_POLICY_ENGINE_MG_ADDR_OUTOF_RANGE_E},
 {CPSS_PP_MISC_ILLEGAL_ADDR_E,           PRV_CPSS_PM3INT_MG_PEX_ADDR_UNMAPPED_E},
 {CPSS_PP_MISC_CPU_PORT_RX_OVERRUN_E,    PRV_CPSS_PM3INT_MG_RX_FIFO_OVERRUN_E},
 {CPSS_PP_PORT_802_3_AP_E,               PRV_CPSS_PM3INT_MG_Z80_E}
};

/* Interrupt cause to unified event map for puma3 without extended data size */
GT_U32 puma3UniEvMapTableSize = (sizeof(puma3UniEvMapTable)/(sizeof(GT_U16)*2));


