/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortIfModeCfgBcat2Resource.h
*
* DESCRIPTION:
*       CPSS BC2 B0 resource tables
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_IF_MODE_CFG_BCAT2_B0_RESOURCE_TABLES_H
#define __PRV_CPSS_DXCH_PORT_IF_MODE_CFG_BCAT2_B0_RESOURCE_TABLES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>

/*--------------------------------------------------------------------------------------------------------------------------------
// Packet travel guide : 
//
//  Regular port x with mapping (mac-x,rxdma-x, txdma-x, txq-x,txfifo-x) 
//        packet --> port-rx --> RxDMA -----------------> TxQ -->---------------- TxDMA ----------> TXFIIFO --> port-tx
//                    mac-x     rxdma-x                  txq-x                    txdma-x           txfifo-x     mac-x
//
//                           RXDMA_IF_WIDTH                ?                    TxQ Credits       TXFIFO IF width
//                                                                           TxDMA speed(A0)      Shifter Threshold (A0)
//                                                                            Rate Limit(A0)      Payload StartTrasmThreshold 
//                                                                           Burst Full Limit(B0)      
//                                                                           Burst Amost Full Limit(B0)  
//                                                                           Payload Credits
//                                                                           Headers Credits
//
//  port with TM (mac-x,rxdma-x,TxDMA = TM,TxQ = TM , TxFIFO = TM, Eth-TxFIFO == eth-txfifo-x) 
//
//        packet --> port-rx --> RxDMA ------------------> TxQ ---------> TxDMA ---------------> TxFIFO -------------------> ETH-THFIFO -----------> port-tx
//                    mac-x      rxdma-x                  64(TM)          73(TM)                 73 (TM)                     eth-txfifo-x             mac-x
//                                                                                                                           
//                            RXDMA_IF_WIDTH                            TxQ Credits             TXFIFO IF width               Eth-TXFIFO IF width
//                                                                      TxDMA speed(A0)         Shifter Threshold(A0)         Shifter Threshold (A0)
//                                                                      Rate Limit(A0)          Payload StartTrasmThreshold   Payload StartTrasmThreshold
//                                                                      Burst Full Limit(B0)      
//                                                                      Burst Amost Full Limit(B0)  
//                                                                      Payload Credits
//                                                                      Headers Credits
//
//----------------------------------------------------------------------------------------------------------------------------------
*/


typedef struct PRV_CPSS_DXCH_CORECLOCK_PLACE_STCT
{                                         
    GT_U32  real_coreClockMHz;            
    GT_U32  computation_coreClockMHz;     
    GT_U32  placeInArr;                   
}PRV_CPSS_DXCH_CORECLOCK_PLACE_STC;            
                                          
typedef struct PRV_CPSS_DXCH_SPEED_PLACE_STCT
{                                         
    CPSS_PORT_SPEED_ENT speedEnm;         
    GT_U32              computation_speed;
    GT_U32              placeInArr;       
}PRV_CPSS_DXCH_SPEED_PLACE_STC;           

/*-----------------------------------------------------------------------------------*
 * TXDMA_SCDMA_TxQDescriptorCredit              : core clock (MHz) x  speed (Mbps)
 * TXDMA_SCDMA_burstAlmostFullThreshold         : core clock (MHz) x  speed (Mbps)
 * TXDMA_SCDMA_burstFullThreshold               : core clock (MHz) x  speed (Mbps)
 * TXDMA_SCDMA_TxFIFOHeaderCreditThreshold      : core clock (MHz) x  speed (Mbps)
 * TXDMA_SCDMA_TxFIFOPayloadCreditThreshold     : core clock (MHz) x  speed (Mbps)
 * TXFIFO_SCDMA_PayloadStartTransmThreshold     : core clock (MHz) x  speed (Mbps)
 * ETH_TXFIFO_SCDMA_PayloadStartTransmThreshold : core clock (MHz) x  speed (Mbps)
 *-----------------------------------------------------------------------------------*/
GT_STATUS prvCpssDxChBcat2_B0_Port_ResourcesCompute
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_U32                          coreClock,
    OUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC  *resConfigPtr
);

/*--------------------------------------------------------------------------------------------------*
 * TXDMA_SCDMA_TxQDescriptorCredit_gran_1G_Arr         [131][6];  speed (Mbps) x core clock (MHz)
 * TXDMA_SCDMA_burstAlmostFullThreshold_gran_1G_Arr    [131][6];  speed (Mbps) x core clock (MHz)
 * TXDMA_SCDMA_burstFullThreshold_gran_1G_Arr          [131][6];  speed (Mbps) x core clock (MHz)
 * TXDMA_SCDMA_TxFIFOHeaderCreditThreshold_gran_1G_Arr [131][6];  speed (Mbps) x core clock (MHz)
 * TXDMA_SCDMA_TxFIFOPayloadCreditThreshold_gran_1G_Arr[131][6];  speed (Mbps) x core clock (MHz)
 * TXFIFO_SCDMA_PayloadStartTransmThreshold_gran_1G_Arr[131][6];  speed (Mbps) x core clock (MHz)
 *--------------------------------------------------------------------------------------------------*/


GT_STATUS prvCpssDxChBcat2_B0_TM_ResourcesCompute
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          coreClock,
    IN  GT_U32                          speedMbps,
    OUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC  *resConfigPtr
);


GT_STATUS prvCpssDxChBcat2B0ResourcesTablesInit
(
    IN    GT_U8                   devNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
