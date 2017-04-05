/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* phyMacRegs1540M.h
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#include <cpss/generic/extMac/cpssExtMacDrv.h> 


/* Top Level Memory Map */

#define PHYMAC_1540_SLICE0              0x0000  /*/< Port 0 registers/memory */
#define PHYMAC_1540_SLICE1              0x0800  /*/< Port 1 registers/memory */
#define PHYMAC_1540_SLICE2              0x1000  /*/< Port 2 registers/memory */
#define PHYMAC_1540_SLICE3              0x1800  /*/< Port 3 registers/memory */
#define PHYMAC_1540_MAC_STATS           0x2000 /*/< MAC Statistics */
#define PHYMAC_1540_GLOBAL              0x2100 /*/< Global registers */
#define PHYMAC_1540_SEC_STATS           0x2800 /*/< MACsec Statistics  */


/* MAC Configuration Defines  */

#define PHYMAC_1540_CFG_WIR_MAC         0x040  /* Wire Side Mac Configuration */
#define PHYMAC_1540_CFG_SYS_MAC         0x050  /* Wire Side Mac Configuration */


/*  System MAC Configuration Defines */
#define PHYMAC_1540_SYS_MAC_CONTROL0       PHYMAC_1540_CFG_SYS_MAC+0x0
#define PHYMAC_1540_SYS_MAC_CONTROL1       PHYMAC_1540_CFG_SYS_MAC+0x1
#define PHYMAC_1540_SYS_MAC_CONTROL2       PHYMAC_1540_CFG_SYS_MAC+0x2
#define PHYMAC_1540_SYS_AUTO_NEG_CONTROL   PHYMAC_1540_CFG_SYS_MAC+0x3
#define PHYMAC_1540_SYS_MAC_STATUS0        PHYMAC_1540_CFG_SYS_MAC+0x4
#define PHYMAC_1540_SYS_MAC_INTR           PHYMAC_1540_CFG_SYS_MAC+0x5
#define PHYMAC_1540_SYS_MAC_INTR_MSK       PHYMAC_1540_CFG_SYS_MAC+0x6
#define PHYMAC_1540_SYS_HIDDEN0            PHYMAC_1540_CFG_SYS_MAC+0x7
#define PHYMAC_1540_SYS_HIDDEN1            PHYMAC_1540_CFG_SYS_MAC+0x8



/*  Wire MAC Configuration Defines */
#define PHYMAC_1540_WIR_MAC_CONTROL0       PHYMAC_1540_CFG_WIR_MAC+0x0
#define PHYMAC_1540_WIR_MAC_CONTROL1       PHYMAC_1540_CFG_WIR_MAC+0x1
#define PHYMAC_1540_WIR_MAC_CONTROL2       PHYMAC_1540_CFG_WIR_MAC+0x2
#define PHYMAC_1540_WIR_AUTO_NEG_CONTROL   PHYMAC_1540_CFG_WIR_MAC+0x3
#define PHYMAC_1540_WIR_MAC_STATUS0        PHYMAC_1540_CFG_WIR_MAC+0x4
#define PHYMAC_1540_WIR_MAC_INTR           PHYMAC_1540_CFG_WIR_MAC+0x5
#define PHYMAC_1540_WIR_MAC_INTR_MSK       PHYMAC_1540_CFG_WIR_MAC+0x6
#define PHYMAC_1540_WIR_HIDDEN0            PHYMAC_1540_CFG_WIR_MAC+0x7
#define PHYMAC_1540_WIR_HIDDEN1            PHYMAC_1540_CFG_WIR_MAC+0x8


/* phy MAC bitmaps */
#define PHYMAC_1540_SPEED_BITMAP                                0x03
#define PHYMAC_1540_LINK_ENABLE_BITMAP                          0x01
#define PHYMAC_1540_AN_DUPLEX_ENABLE_BITMAP                     0x08
#define PHYMAC_1540_AN_SPEED_ENABLE_BITMAP                      0x20
#define PHYMAC_1540_AN_FC_PAUSE_ENABLE_BITMAP                   0x40
#define PHYMAC_1540_FLOW_CONTROL_MODE_BITMAP                    0x0A
#define PHYMAC_1540_PERIODIC_FLOW_CONTROL_ENABLE_BITMAP         0x02
#define PHYMAC_1540_LINK_STATUS_BITMAP                          0x40
#define PHYMAC_1540_PERIODIC_FLOW_CONTROL_BITMAP                0x02
#define PHYMAC_1540_PORT_DUPLEX_STATUS_BITMAP                   0x02
#define PHYMAC_1540_PORT_DUPLEX_MODE_BITMAP                     0x10
#define PHYMAC_1540_PORT_ENABLE_BITMAP                          0x01
#define PHYMAC_1540_PADDING_ENABLE_BITMAP                       0x01
#define PHYMAC_1540_CRC_CHECK_ENABLE_BITMAP                     0x01
#define PHYMAC_1540_MRU_BITMAP                                  0xFFFC
#define PHYMAC_1540_PORT_BACKPRESSURE_BITMAP                    0x80

/* Flow Control mode bitmaps */
#define PHYMAC_1540_FLOW_CONTROL_RX_TX_BITMAP                   0x0C
#define PHYMAC_1540_FLOW_CONTROL_RX_ONLY_BITMAP                 0x08
#define PHYMAC_1540_FLOW_CONTROL_TX_ONLY_BITMAP                 0x04
#define PHYMAC_1540_FLOW_CONTROL_RX_TX_DISABLED_BITMAP          0x0

#define PHYMAC_1540_FLOW_CONTROL_GET_RX_TX_DISABLED_BITMAP      0x18

/*********************************************************************
*  PHYMAC 1540 EEE definitions
**********************************************************************/
/*  EEE Configuration Regs */
#define PHYMAC_1540_EEE_LPI_TIMEOUT                             0x3C0
#define PHYMAC_1540_EEE_LPI_CONFIG                              0x3C1
#define PHYMAC_1540_EEE_LPI_IGRESS_BUF_THRESHOLD                0x3C4

/*  EEE bitmaps */
#define PHYMAC_1540_EEE_MASTER_MODE_BITMAP                      0x0001
#define PHYMAC_1540_EEE_SLAVE_MODE_BITMAP                       0x0002
#define PHYMAC_1540_EEE_FORCE_WIRE_SIDE_MODE_BITMAP             0x0004
#define PHYMAC_1540_EEE_FORCE_SYSTEM_SIDE_MODE_BITMAP           0x0008
#define PHYMAC_1540_EEE_FORCE_MODE_BITMAP                       0x000C
#define PHYMAC_1540_EEE_DISABLE_MODE_BITMAP                     0x000F

