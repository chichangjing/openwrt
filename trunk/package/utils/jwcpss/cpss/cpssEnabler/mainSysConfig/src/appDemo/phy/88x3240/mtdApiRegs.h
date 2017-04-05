#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions global register definitions for the
registers in the 88X3240/3220.
********************************************************************/
#ifndef APIREGS_H
#define APIREGS_H
#if C_LINKAGE
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

/* 88X3240/3220 Device Number Definitions */
#define MTD_T_UNIT_PMA_PMD  1
#define MTD_T_UNIT_PCS_CU   3
#define MTD_X_UNIT          3
#define MTD_H_UNIT          4
#define MTD_T_UNIT_AN       7
#define MTD_XFI_DSP         30
#define MTD_C_UNIT_GENERAL  31
#define MTD_M_UNIT          31

/* 88X3240/3220 Device Number Definitions Host Redundant Mode */
#define MTD_BASER_LANE_0  MTD_H_UNIT
#define MTD_BASER_LANE_1  MTD_X_UNIT

/* 88X3240/3220 T Unit Registers MMD 1 */
#define MTD_TUNIT_IEEE_PMA_CTRL1       MTD_T_UNIT_PMA_PMD,0x0000
#define MTD_TUNIT_IEEE_PMA_DEVID2      MTD_T_UNIT_PMA_PMD,0x0003
#define MTD_TUNIT_PHY_EXT_CTRL_1       MTD_T_UNIT_PMA_PMD,0xC000
#define MTD_TUNIT_XG_EXT_STATUS        MTD_T_UNIT_PMA_PMD,0xC001
#define MTD_TUNIT_BIST_STATUS_REG      MTD_T_UNIT_PMA_PMD,0xC00C
#define MTD_TUNIT_PHY_REV_INFO_REG     MTD_T_UNIT_PMA_PMD,0xC04E
#define MTD_BOOT_STATUS_REG            MTD_T_UNIT_PMA_PMD,0xC050

#define MTD_TUNIT_IEEE_PCS_CTRL1       MTD_T_UNIT_PCS_CU,0x0000

/* 88X3240/3220 C Unit Registers MMD 31 */
#define MTD_CUNIT_MODE_CONFIG          MTD_C_UNIT_GENERAL,0xF000
#define MTD_CUNIT_PORT_CTRL            MTD_C_UNIT_GENERAL,0xF001

/* 88X3240/3220 H Unit and X Unit 1000BASE-X and SGMII Registers 
   Shared registers:
 */
#define MTD_1000X_SGMII_CONTROL               0x2000
#define MTD_1000X_SGMII_STATUS                0x2001
#define MTD_1000X_SGMII_PHYID1                0x2002
#define MTD_1000X_SGMII_PHYID2                0x2003
#define MTD_1000X_SGMII_AN_ADV                0x2004
#define MTD_1000X_SGMII_LP_ABL                0x2005

/* X unit only registers */
#define MTD_1000X_AN_EXPANSION                MTD_X_UNIT,0x2006
#define MTD_1000X_NP_TX                       MTD_X_UNIT,0x2007
#define MTD_1000X_LP_NP                       MTD_X_UNIT,0x2008
#define MTD_1000X_EXT_STATUS                  MTD_X_UNIT,0x200F
#define MTD_1000X_TIMR_MODE_SEL               MTD_X_UNIT,0xA000
#define MTD_1000X_INTR_ENABLE                 MTD_X_UNIT,0xA001
#define MTD_1000X_INTR_STATUS                 MTD_X_UNIT,0xA002
#define MTD_1000X_PHY_SPEC_STATUS             MTD_X_UNIT,0xA003

/* 88X3240/3220 H Unit and X Unit 10GBASE-R Shared Registers */
#define MTD_10GBR_PCS_CONTROL                 0x1000
#define MTD_10GBR_PCS_STATUS1                 0x1001
#define MTD_10GBR_PCS_FAULT                   0x1008 /* called STATUS2 in datasheet */
#define MTD_10GBR_PCS_RXSTATUS                0x1020 /* called STATUS1 in datasheet */ 
#define MTD_10GBR_PCS_STATUS2                 0x1021

/* 88X3240/3220 H Unit and X Unit Common Registers */
#define MTD_SERDES_CONTROL1                  0xF003
#define MTD_SERDES_CONTROL2                  0xF004
#define MTD_FIFO_CRC_INTR_ENABLE             0xF00A
#define MTD_FIFO_CRC_INTR_STATUS             0xF00B
#define MTD_FIFO_CONTROL1                    0xF00C
#define MTD_PKT_GEN_CONTROL1                 0xF010
#define MTD_PKT_GEN_CONTROL2                 0xF011
#define MTD_PKT_GEN_PAYLOAD1                 0xF012
#define MTD_PKT_GEN_PAYLOAD2                 0xF013
#define MTD_PKT_GEN_LENGTH                   0xF016
#define MTD_PKT_GEN_BURSTSEQ                 0xF017
#define MTD_PKT_GEN_IPG                      0xF018
#define MTD_PKT_GEN_TXPKTCTR1                0xF01B
#define MTD_PKT_GEN_TXPKTCTR2                0xF01C
#define MTD_PKT_GEN_TXPKTCTR3                0xF01D
#define MTD_PKT_GEN_TXBYTCTR1                0xF01E
#define MTD_PKT_GEN_TXBYTCTR2                0xF01F
#define MTD_PKT_GEN_TXBYTCTR3                0xF020
#define MTD_PKT_GEN_RXPKTCTR1                0xF021
#define MTD_PKT_GEN_RXPKTCTR2                0xF022
#define MTD_PKT_GEN_RXPKTCTR3                0xF023
#define MTD_PKT_GEN_RXBYTCTR1                0xF024
#define MTD_PKT_GEN_RXBYTCTR2                0xF025
#define MTD_PKT_GEN_RXBYTCTR3                0xF026
#define MTD_PKT_GEN_ERRPKTCTR1               0xF027
#define MTD_PKT_GEN_ERRPKTCTR2               0xF028
#define MTD_PKT_GEN_ERRPKTCTR3               0xF029
#define MTD_LINK_DROP_COUNTER                0xF02A
#define MTD_PRBS_CONTROL                     0xF030
#define MTD_PRBS_SYM_TXCTR1                  0xF031
#define MTD_PRBS_SYM_TXCTR2                  0xF032
#define MTD_PRBS_SYM_TXCTR3                  0xF033
#define MTD_PRBS_SYM_RXCTR1                  0xF034
#define MTD_PRBS_SYM_RXCTR2                  0xF035
#define MTD_PRBS_SYM_RXCTR3                  0xF036
#define MTD_PRBS_SYM_ERRCTR1                 0xF037
#define MTD_PRBS_SYM_ERRCTR2                 0xF038
#define MTD_PRBS_SYM_ERRCTR3                 0xF039
#define MTD_PRBS_CONTROL2                    0xF040
#define MTD_PRBS_ELAPSE_TIME                 0xF041
#define MTD_PRBS_MSTR_ERRCTR1                0xF042
#define MTD_PRBS_MSTR_ERRCTR2                0xF043
#define MTD_PRBS_MSTR_ERRCTR3                0xF044
#define MTD_PRBS_INTM_ERRCTR1                0xF045
#define MTD_PRBS_INTM_ERRCTR2                0xF046
#define MTD_PRBS_INTM_ERRCTR3                0xF047



#if C_LINKAGE
#if defined __cplusplus
}
#endif
#endif


#endif

