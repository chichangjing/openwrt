/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwRegAddr.h
*
* DESCRIPTION:
*       Private definition for register addresses of the DXCH devices.
*
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#ifndef __prvCpssDxChHwRegAddrh
#define __prvCpssDxChHwRegAddrh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PRV_CPSS_DXCH_REG_ADDR_MASK_TO_UNIT_BASE_CNS    0xFF800000

#define PRV_CPSS_DXCH_LION2_TXQ_EGR0_BASE_ADDRESS_CNS   0x12000000
#define PRV_CPSS_DXCH_LION2_TXQ_EGR1_BASE_ADDRESS_CNS   0x12800000
#define PRV_CPSS_DXCH_LION2_TXQ_QUEUE_BASE_ADDRESS_CNS  0x10000000
#define PRV_CPSS_DXCH_LION2_TXQ_DIST_BASE_ADDRESS_CNS   0x13000000
#define PRV_CPSS_DXCH_LION2_TXQ_LL_BASE_ADDRESS_CNS     0x10800000
#define PRV_CPSS_DXCH_LION2_TXQ_SHT_BASE_ADDRESS_CNS    0x11800000

#define PRV_CPSS_DXCH_LION2_BMA_BASE_ADDRESS_CNS        0x03800000
#define PRV_CPSS_DXCH_LION2_DFX_SERVER_BASE_ADDRESS_CNS 0x01800000

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChHwRegAddrh */
