/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortIfModeCfgBcat2Resource.c
*
* DESCRIPTION:
*       CPSS BC2 implementation for Port interface mode resource configuration.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortInterlaken.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2B0ResourceList.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2B0ResourceTables.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgResource.h>
#include <cpss/generic/private/utils/prvCpssUnitGenArrDrv.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <mvHwsPortInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <private/mvPortModeElements.h>
#include <mac/mvHwsMacIf.h>
#include <pcs/mvHwsPcsIf.h>



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
//                                                                      TxDMA speed(A0)         Shifter Threshold(A0)         Shifter Threshold 
//                                                                      Rate Limit(A0)          Payload StartTrasmThreshold   Payload StartTrasmThreshold 
//                                                                      Burst Full Limit(B0)      
//                                                                      Burst Amost Full Limit(B0)  
//                                                                      Payload Credits
//                                                                      Headers Credits
//
//----------------------------------------------------------------------------------------------------------------------------------
//        a.  RXDMA_IF_WIDTH
//            /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<RXDMA_IP> RxDMA IP Units/Single Channel DMA Configurations/SCDMA %n Configuration 0
//            0-2 : IF_WIDTH values  0 -- width64 <= 20G
//                                   2 -- width256 == 40G
//                                   3 -- width512 == 100G ILKN
//
//            ~12G <=> 10G
//            ~24G <=> 20G
//            ~48G <=> 40G
//
//        Regular Port
//        b0.  TXFIFO_IF_WIDTH
//            Bobcat2 {Current}/Switching Core/<TXFIFO_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/SCDMA %p Shifters Conf
//            A0,B0 : 0-2 : IF_WIDTH values  0 -- 1B  == 1G
//                                           3 -- 8B  == 2.5G 10G 20G
//                                           5 -- 32B == 40G
//                                           6 -- 64B == 100G
//
//        c0.  Shifter %p Threshold
//            /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<TXFIFO_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/SCDMA %p Shifters Conf
//            A0,B0 : 3-5 shifter threshold
//
//       Port Pass through TM
//        b1   Bobcat2 {Current}/Switching Core/<SIP_ETH_TXFIFO_IP> SIP_ETH_TXFIFO_IP Units/TxFIFO Shifters Configuration/SCDMA %p Shifters Conf
//            A0,B0 : 0-2 : IF_WIDTH values  0 -- 1B  == 1G
//                                           3 -- 8B  == 2.5G 10G 20G
//                                           5 -- 32B == 40G
//                                           6 -- 64B == 100G
//
//        c1   Bobcat2 {Current}/Switching Core/<SIP_ETH_TXFIFO_IP> SIP_ETH_TXFIFO_IP Units/TxFIFO Shifters Configuration/SCDMA %p Shifters Conf
//            /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<TXFIFO_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/SCDMA %p Shifters Conf
//            A0,B0 : 3-5 shifter threshold

//            portNumMappedForUnit = portMapShadowPtr->portMap.tmPortIdx;
//            regAddr = regsAddrPtr->SIP_ETH_TXFIFO.txFIFOShiftersConfig.SCDMAShiftersConf[portNumMappedForUnit];
//            regAddr = regsAddrPtr->SIP_ETH_TXFIFO.txFIFOGlobalConfig.SCDMAPayloadThreshold[portNumMappedForUnit];
//
//        d. TxDMA per SCDMA config
//            1.  TxDMA SCDMA speed
//                /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<TXDMA_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/SCDMA %p Configurations
//                A0  16-18           B0  16-19
//                0x0 = 1G;           0x0 = 1G;
//                0x1 = 2.5G          0x1 = 2.5G;
//                0x2 = 10G           0x2 = 10G;
//                0x3 = 20G           0x3 = 10G eDSA;
//                0x4 = 40G           0x4 = 20G;
//                0x5 = 100G          0x5 = 20G eDSA;
//                0x6 = Above 100G    0x6 = 40G;
//                                    0x7 = 40G eDSA;
//                                    0x8 = 100G;
//                                    0x9 = 100G eDSA;
//                                    0xa = Other Above 100G; other_above_100g
//                                    0xb = Other Below 100G; other_below_100g
//
//
//
//            2.  Rate limit threshold
//                /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<TXDMA_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/SCDMA %p Configurations
//                A0 19-28            B0  20-29
//
//                is configured for BC2 A0 only ?!
//                ceil(350/speedForCalc) = (350+(speedForCalc-1))/speedForCalc)
//
//            3.  TxDMA credits:
//                /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<TXDMA_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/FIFOs Thresholds Configurations SCDMA %p Reg 1
//                A0 17-24                 B0 0-8
//                PRV_CPSS_DXCH_PORT_BOBCAT2_DESC_FIFO_SIZE_CNS   206
//
//            4   Counters configuration (both Header and Payload)
//                /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<TXDMA_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/TxFIFO Counters Configurations SCDMA %p
//                 0- 9 Header threshold
//                10-19 Payload Counter threshold
//
//        e.  TXFIFO_PAYLOAD_COUNTER_THRESHOLD_SCDMA
//                Threshold : PRV_CPSS_DXCH_PORT_BOBCAT2_PAYLOAD_CREDITS_CNS (541)
//        f.  TXFIFO_HEADER_COUNTER_THRESHOLD_SCDMA
//                Threshold : PRV_CPSS_DXCH_PORT_BOBCAT2_HEADER_CREDITS_CNS  (541)
//    6.  prvCpssDxChPortBcat2FcaBusWidthSet
//
//        /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/GOP/<FCA IP> FCA IP Units%f/FCA Control
//            B0 4-6 Port Type (BUS width)
//            Configures the port type.
//            0x0 = Type A; BUS_8_BIT; 1G port
//            0x1 = Type B; BUS_64_BIT; 2.5G-10G port
//            0x2 = Type C; BUS_128_BIT; No such port
//            0x3 = Type D; BUS_256_BIT; 40G port
//            0x4 = Type E; BUS_512_BIT; 100G port
//
//----------------------------------------------------------------------------------------------------
*/

/*


   TxQ Credits [Desc]
+------+------+------+--------+-------------------------------------------------------------------------+
|      | Core | MPPM | Pipe   |                        Port Speed (Gbps)                                |
| MODE | Freq | Clk  | Max BW |  1 2.5  10  12  13  20  24  25  40  48  49  52  95 100 116 120 125 130  |
|      | (MHz)| Freq | (Gbps) |                                                                         |
+------+------+------+--------+-------------------------------------------------------------------------+
|  0   | 175  |  175 |   58   |  5   7  14  13  13  21  20  20  36  34  33  36  59  61  71  71  76  79  |
|  1   | 250  |  250 |   80   |  2   3   8   7   7  13  12  12  23  22  21  23  40  42  48  48  52  54  |
|  2   | 360  |  360 |  120   |  2   3   7   6   6  10   9  10  17  16  16  17  29  30  35  34  36  38  |
|  3   | 400  |  400 |  128   |  2   3   6   6   5   9   9   9  15  15  15  16  26  27  32  32  34  34  |
|  4   | 500  |  500 |  168   |  2   3   6   5   5   8   8   8  13  12  13  13  22  22  25  25  27  28  |
|  5   | 520  |  520 |  241   |  3   4   7   6   6   9   8   8  14  13  13  14  22  22  25  25  27  28  |
+------+------+------+--------+-------------------------------------------------------------------------+

   TxFIFO Headers Credits [Desc]
+------+------+------+--------+-------------------------------------------------------------------------+
|      | Core | MPPM | Pipe   |                        Port Speed (Gbps)                                |
| MODE | Freq | Clk  | Max BW |  1 2.5  10  12  13  20  24  25  40  48  49  52  95 100 116 120 125 130  |
|      | (MHz)| Freq | (Gbps) |                                                                         |
+------+------+------+--------+-------------------------------------------------------------------------+
|  0   | 175  |  175 |   58   |  4  10  49  47  47  96  93  94 191 185 187 200 366 385 446 446 481 500  |
|  1   | 250  |  250 |   80   |  3   8  34  33  34  68  65  66 134 130 131 140 256 270 313 313 337 350  |
|  2   | 360  |  360 |  120   |  2   5  24  24  23  47  46  46  93  90  91  98 178 187 217 217 234 243  |
|  3   | 400  |  400 |  128   |  2   5  22  21  21  43  41  42  84  82  82  88 160 169 196 196 211 219  |
|  4   | 500  |  500 |  168   |  2   4  18  17  17  34  33  34  67  66  66  71 129 135 157 157 169 175  |
|  5   | 520  |  520 |  241   |  2   4  18  17  17  33  32  33  66  63  64  68 123 130 151 151 163 169  |
+------+------+------+--------+-------------------------------------------------------------------------+

   TxFIFO Payload Credits [Desc]
+------+------+------+--------+-------------------------------------------------------------------------+
|      | Core | MPPM | Pipe   |                        Port Speed (Gbps)                                |
| MODE | Freq | Clk  | Max BW |  1 2.5  10  12  13  20  24  25  40  48  49  52  95 100 116 120 125 130  |
|      | (MHz)| Freq | (Gbps) |                                                                         |
+------+------+------+--------+-------------------------------------------------------------------------+
|  0   | 175  |  175 |   58   |  6  13  58  58  58 113 113 114 224 224 226 242 442 465 538 538 580 603  |
|  1   | 250  |  250 |   80   |  5  11  41  41  42  80  80  81 157 158 159 170 309 326 378 378 407 423  |
|  2   | 360  |  360 |  120   |  4   7  29  30  29  56  57  57 110 110 111 119 216 226 263 263 283 294  |
|  3   | 400  |  400 |  128   |  4   7  27  27  27  51  51  52  99 100 100 107 194 205 237 237 255 265  |
|  4   | 500  |  500 |  168   |  4   6  22  22  22  41  41  42  79  81  81  87 156 164 190 190 205 212  |
|  5   | 520  |  520 |  241   |  4   6  22  22  22  40  40  41  78  77  78  83 150 158 183 183 197 205  |
+------+------+------+--------+-------------------------------------------------------------------------+
*/


/*----------------------------------------------*/



#define FLD_OFF(STR,fld)      (GT_U32)offsetof(STR,fld)
#define FLD_OFFi(STR,idx,fld) idx*sizeof(STR) + offsetof(STR,fld)

#ifdef PRV_SUPPORT_BCAT_A0
    PRV_CPSS_DRV_FLD_ARR_INIT_STC prv_bc2PortRes_revA0[] =
    {
        /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
        /* fld id                                                      {          reg off in reg struct,                                                       fld-offs,len }, {min/maxValue},   name                             */
        /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
         { BC2_PORT_FLD_RXDMA_IfWidth                            , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,rxDMA[0].singleChannelDMAConfigs.SCDMAConfig0                 ),    0,  3 }, 73, { 0 , 3   },  "RXDMA_IfWidth"                   }
        /* Tx-DMA */
        ,{ BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit          , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1 ),   17,  8 }, 74, { 0 , 206 },  "TXDMA_SCDMA_TxQDescriptorCredit" }
        ,{ BC2_PORT_FLD_TXDMA_SCDMA_speed                        , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.SCDMAConfigs                    ),   16,  3 }, 74, { 0 , 6   },  "TXDMA_SCDMA_speed"               }
        ,{ BC2_PORT_FLD_TXDMA_SCDMA_rateLimitThreshold           , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.SCDMAConfigs                    ),   19, 10 }, 74, { 0 , 100 },  "TXDMA_SCDMA_rateLimitThreshold"  }
        ,{ BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold  , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA         ),    0, 10 }, 74, { 0 , 0x3FF },  "TXDMA_SCDMA_TxFIFOHeaderCreditThreshold"  } 
        ,{ BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA         ),   10, 10 }, 74, { 0 , 0x3FF },  "TXDMA_SCDMA_TxFIFOPayloadCreditThreshold" } 
        /* Tx-FIFO */
        ,{ BC2_PORT_FLD_TXFIFO_IfWidth                           , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txFIFO[0].txFIFOShiftersConfig.SCDMAShiftersConf              ),    0,  3 }, 74, { 0 , 6   },  "TXFIFO_IfWidth"                           }
        ,{ BC2_PORT_FLD_TXFIFO_shifterThreshold                  , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txFIFO[0].txFIFOShiftersConfig.SCDMAShiftersConf              ),    3,  3 }, 74, { 0 , 7   },  "TXFIFO_shifterThreshold"                  }
        ,{ BC2_PORT_FLD_TXFIFO_payloadThreshold                  , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txFIFO[0].txFIFOGlobalConfig.SCDMAPayloadThreshold            ),    0,  7 }, 74, { 0 , 0x7F},  "TXFIFO_payloadThreshold"                  }
        /* Eth-Tx-FIFO */
        ,{ BC2_PORT_FLD_Eth_TXFIFO_IfWidth                       , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,SIP_ETH_TXFIFO.txFIFOShiftersConfig.SCDMAShiftersConf         ),    0,  3 }, 74, { 0 , 6   },  "Eth-TXFIFO_IfWidth"                       }  
        ,{ BC2_PORT_FLD_Eth_TXFIFO_shifterThreshold              , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,SIP_ETH_TXFIFO.txFIFOShiftersConfig.SCDMAShiftersConf         ),    3,  3 }, 74, { 0 , 7   },  "Eth-TXFIFO_shifterThreshold"              }  
        ,{ BC2_PORT_FLD_Eth_TXFIFO_payloadThreshold              , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,SIP_ETH_TXFIFO.txFIFOGlobalConfig.SCDMAPayloadThreshold       ),    0,  7 }, 74, { 0 , 0x7F},  "Eth-TXFIFO_payloadThreshold"              }
        /* FCA */
        /* ,{ BC2_PORT_FLD_FCA_BUS_WIDTH                         , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,GOP.FCA. */
        ,{ PRV_BAD_VAL                                           , {PRV_BAD_VAL,    PRV_BAD_VAL, PRV_BAD_VAL},  PRV_BAD_VAL,  { PRV_BAD_VAL, PRV_BAD_VAL},   (GT_CHAR *)NULL  }
    };
    PRV_CPSS_DRV_FLD_ARR_DEF_STC   prv_dxChBcat2A0_FldInitStc[BC2_PORT_FLD_MAX];
    PRV_CPSS_DXCH_ARR_DRV_STC      prv_dxChBcat2A0;
#endif

PRV_CPSS_DRV_FLD_ARR_INIT_STC prv_bc2PortRes_revB0[] =
{
    /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    /* fld id                                                      {          reg off in reg struct,                                                                      fld-offs,len },size,{min/maxValue},   name                             */
    /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
     { BC2_PORT_FLD_RXDMA_IfWidth                            , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,rxDMA[0].singleChannelDMAConfigs.SCDMAConfig0                 ),    0,  3 }, 73, { 0 , 3       },  "RXDMA_IfWidth"                        }
    /* Tx-DMA */
    ,{ BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit          , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1 ),    0,  9 }, 74, { 0 , 206     },  "TXDMA_SCDMA_TxQDescriptorCredit"      }
    ,{ BC2_PORT_FLD_TXDMA_SCDMA_burstAlmostFullThreshold     , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.burstLimiterSCDMA               ),   16, 16 }, 74, { 0 , 0xFFFFF },  "TXDMA_SCDMA_burstAlmostFullThreshold" }
    ,{ BC2_PORT_FLD_TXDMA_SCDMA_burstFullThreshold           , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.burstLimiterSCDMA               ),    0, 16 }, 74, { 0 , 0xFFFFF },  "TXDMA_SCDMA_burstThreshold"           }
    ,{ BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold  , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA         ),    0, 10 }, 74, { 0 , 0x3FF   },  "TXDMA_SCDMA_TxFIFOHeaderCreditThreshold"  } 
    ,{ BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txDMA[0].txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA         ),   10, 10 }, 74, { 0 , 0x3FF   },  "TXDMA_SCDMA_TxFIFOPayloadCreditThreshold" } 
    /* Tx-FIFO */
    ,{ BC2_PORT_FLD_TXFIFO_IfWidth                           , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txFIFO[0].txFIFOShiftersConfig.SCDMAShiftersConf              ),    0,  3 }, 74, { 0 , 7       },  "TXFIFO_IfWidth"                  }
    ,{ BC2_PORT_FLD_TXFIFO_shifterThreshold                  , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txFIFO[0].txFIFOShiftersConfig.SCDMAShiftersConf              ),    3,  3 }, 74, { 0 , 7       },  "TXFIFO_shifterThreshold"                  }
    ,{ BC2_PORT_FLD_TXFIFO_payloadThreshold                  , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,txFIFO[0].txFIFOGlobalConfig.SCDMAPayloadThreshold            ),    0,  7 }, 74, { 0 , 0x7F    },  "TXFIFO_payloadThreshold"                  }
    /* Eth-Tx-FIFO */                                                                                                                                                                         
    ,{ BC2_PORT_FLD_Eth_TXFIFO_IfWidth                       , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,SIP_ETH_TXFIFO[0].txFIFOShiftersConfig.SCDMAShiftersConf      ),    0,  3 }, 74, { 0 , 7       },  "Eth-TXFIFO_IfWidth"                       }  
    ,{ BC2_PORT_FLD_Eth_TXFIFO_shifterThreshold              , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,SIP_ETH_TXFIFO[0].txFIFOShiftersConfig.SCDMAShiftersConf      ),    3,  3 }, 74, { 0 , 7       },  "Eth-TXFIFO_shifterThreshold"              }  
    ,{ BC2_PORT_FLD_Eth_TXFIFO_payloadThreshold              , { FLD_OFF(PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC,SIP_ETH_TXFIFO[0].txFIFOGlobalConfig.SCDMAPayloadThreshold    ),    0,  8 }, 74, { 0 , 0xFF    },  "Eth-TXFIFO_payloadThreshold"              }
    /* FCA */
    /*,{ BC2_PORT_FLD_FCA_BUS_WIDTH                          , { */
    ,{ PRV_BAD_VAL                                           , {PRV_BAD_VAL,    PRV_BAD_VAL, PRV_BAD_VAL },  PRV_BAD_VAL,  { PRV_BAD_VAL, PRV_BAD_VAL},   (GT_CHAR *)NULL  }
};



PRV_CPSS_DRV_FLD_ARR_DEF_STC   prv_dxChBcat2B0_FldInitStc[BC2_PORT_FLD_MAX];
PRV_CPSS_DXCH_ARR_DRV_STC      prv_dxChBcat2B0;




/*******************************************************************************
* prvCpssDxChBcat2PortResourcesConfig
*
* DESCRIPTION:
*       Allocate/free resources of port per it's current status/interface.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       ifMode      - interface mode
*       speed       - port data speed
*       allocate    - allocate/free resources:
*                       GT_TRUE - up;
*                       GT_FALSE - down;
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
extern GT_STATUS prvCpssDxChPortBcat2ResourcesRelease
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum
);



#define PRV_CPSS_DXCH_BCAT2_B0_TXQ_CREDITS_CNS            206
#define PRV_CPSS_DXCH_BCAT2_B0_TXFIFO_HEADERS_CREDITS_CNS 460
#define PRV_CPSS_DXCH_BCAT2_B0_TXFIFO_PAYLOAD_CREDITS_CNS 654


CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC        prv_bc2_B0_resConfig;



/*---------------------------------------------------------------------------------------------------
 *    global data end
 *---------------------------------------------------------------------------------------------------*/


GT_STATUS prvCpssDxChPort_Bcat2B0_ResourcesInHwConfig
(
    IN  GT_U8                                  devNum,
    CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC     *bc2_B0_resConfigPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DRV_FLD_ARR_QEUERY_STC query;
    GT_U32 i;
    rc = prvCpssArrDrvQeueryInit(&query,&prv_dxChBcat2B0,devNum,0,(GT_U32*)PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum));
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0 ; i < sizeof(bc2_B0_resConfigPtr->arr)/sizeof(bc2_B0_resConfigPtr->arr[0]); i++)
    {
        if (prv_bc2_B0_resConfig.arr[i].fldId != BC2_PORT_FLD_INVALID_E)
        {
            rc = prvCpssArrDrvQeueryFldSet(&query,bc2_B0_resConfigPtr->arr[i].fldId,
                                                  bc2_B0_resConfigPtr->arr[i].fldArrIdx,
                                                  bc2_B0_resConfigPtr->arr[i].fldVal);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return GT_OK;
}




GT_STATUS prvCpssDxChPort_Bcat2B0_ResourcesConfig
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS rc;
    GT_U32  coreClock;
    GT_U32  txQDescrCredits;
    GT_U32  txFifoHeaderCredits;
    GT_U32  txFifoPayloadCredits;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;

    coreClock = PRV_CPSS_PP_MAC(devNum)->coreClock;


    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, /*OUT*/&portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }


    rc = prvCpssDxChBcat2_B0_Port_ResourcesCompute(devNum,portNum,ifMode,speed,coreClock,/*OUT*/&prv_bc2_B0_resConfig);
    if (rc != GT_OK)
    {
        return rc;
    }

    txQDescrCredits      = 0;
    txFifoHeaderCredits  = 0;
    txFifoPayloadCredits = 0;
    if (portMapShadowPtr->portMap.trafficManagerEn == GT_FALSE)
    {
        /* get resources to be checked */
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit,          /*OUT*/&txQDescrCredits     );
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold,  /*OUT*/&txFifoHeaderCredits );
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold, /*OUT*/&txFifoPayloadCredits);
    }
    /* check resources availability */
    rc = prvCpssDxChPortResourcesConfigDbAvailabilityCheck(devNum, 0, txQDescrCredits,txFifoHeaderCredits,txFifoPayloadCredits);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* OK there is enough credits, configure HW */
    /* configure HW */
    if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E)
    {
    rc = prvCpssDxChPort_Bcat2B0_ResourcesInHwConfig(devNum, &prv_bc2_B0_resConfig);
    if (rc != GT_OK)
    {
        return rc;
        }
    }

    /* update DB    */
    rc = prvCpssDxChPortResourcesConfigDbAdd(devNum,0,txQDescrCredits,txFifoHeaderCredits,txFifoPayloadCredits);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


GT_STATUS prvCpssDxChPort_Bcat2B0_ResourcesFree
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum
)
{
    GT_STATUS rc;
    GT_U32  coreClock;
    GT_U32  txQDescrCredits;
    GT_U32  txFifoHeaderCredits;
    GT_U32  txFifoPayloadCredits;
    GT_U32  i;
    CPSS_PORT_SPEED_ENT             speed;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode;

    CPSS_DXCH_BC2_PORT_RESOURCES_FLD_ENT resId;
    CPSS_DXCH_BC2_PORT_RESOURCES_FLD_ENT res2freeList[] = 
    {
         BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit         
        ,BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold 
        ,BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold
    };

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;

    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, /*OUT*/&portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    if (portMapShadowPtr->valid == GT_FALSE)  
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    coreClock = PRV_CPSS_PP_MAC(devNum)->coreClock;
    ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMapShadowPtr->portMap.macNum);
    speed  = PRV_CPSS_DXCH_PORT_SPEED_MAC (devNum, portMapShadowPtr->portMap.macNum);

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (speed == CPSS_PORT_SPEED_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    rc = prvCpssDxChBcat2_B0_Port_ResourcesCompute(devNum,portNum,ifMode,speed,coreClock,/*OUT*/&prv_bc2_B0_resConfig);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*-----------------------------------------------------------------------------------------------* 
	 *  get value for follwing resources in order to free them after resources in HW were configured *
     *-----------------------------------------------------------------------------------------------*/
    txQDescrCredits      = 0;
    txFifoHeaderCredits  = 0;
    txFifoPayloadCredits = 0;
    if (portMapShadowPtr->portMap.trafficManagerEn == GT_FALSE)
    {
        /* get resources to be checked */
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit,          /*OUT*/&txQDescrCredits     );
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold,  /*OUT*/&txFifoHeaderCredits );
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold, /*OUT*/&txFifoPayloadCredits);
    }

    for (i = 0 ; i < sizeof(res2freeList)/sizeof(res2freeList[0]); i++)
    {
        resId = res2freeList[i];
        if (prv_bc2_B0_resConfig.arr[resId].fldId != BC2_PORT_FLD_INVALID_E)
        {
            prv_bc2_B0_resConfig.arr[resId].fldVal = 0;
        }
    }
    rc = prvCpssDxChPort_Bcat2B0_ResourcesInHwConfig(devNum, &prv_bc2_B0_resConfig);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortResourcesConfigDbDelete(devNum,0,txQDescrCredits,txFifoHeaderCredits,txFifoPayloadCredits);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}



extern GT_STATUS prvCpssDxChPort_Bcat2A0_ResourcesConfig
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_BOOL                         allocate
);

/*******************************************************************************
* prvCpssDxChBcat2PortResourcesConfig
*
* DESCRIPTION:
*       Allocate/free resources of port per it's current status/interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       ifMode      - interface mode
*       speed       - port data speed
*       allocate    - allocate/free resources:
*                       GT_TRUE - up;
*                       GT_FALSE - down;
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBcat2PortResourcesConfig
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_BOOL                         allocate
)
{
    GT_STATUS rc = GT_OK;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
            CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
            CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
            CPSS_XCAT3_E);


    if (PRV_CPSS_DXCH_BOBCAT2_A0_CHECK_MAC(devNum))
    {
        /* BC2 A0 case */
        rc = prvCpssDxChPort_Bcat2A0_ResourcesConfig(devNum,portNum,ifMode,speed,allocate);
    }
    else
    {
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E) && 
           (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_NONE_E))
        {
            /* BC2 B0 case */
            if (GT_TRUE == allocate)
            {
                rc = prvCpssDxChPort_Bcat2B0_ResourcesConfig(devNum,portNum,ifMode,speed);
            }
            else
            {
                rc = prvCpssDxChPort_Bcat2B0_ResourcesFree(devNum,portNum);
            }
        }
    }
    return rc;
}




/*******************************************************************************
* prvCpssDxChBcat2B0PortResourceTmSet
*
* DESCRIPTION:
*       Bcat B0 TM port resource configuration
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum          - physical device number
*       tmBandwidthMbps - system TM bandwidth
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note: 
*           - API should not be called under traffic
*           - zero bandwidth will release resources of TM for non TM ports
******************************************************************************/
GT_STATUS prvCpssDxChBcat2B0PortResourceTmSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  tmBandwidthMbps
)
{
    GT_STATUS                                   rc;                         /* return code */
    GT_U32  txQDescrCredits;
    GT_U32  txFifoHeaderCredits;
    GT_U32  txFifoPayloadCredits;
    GT_U32  old_txQDescrCredits;
    GT_U32  old_txFifoHeaderCredits;
    GT_U32  old_txFifoPayloadCredits;

    GT_U32  old_tmBandwidthMbps;
    static CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC        old_bc2_B0_TM_resConfig;
    

    /* read BW from DB */
    rc = prvCpssDxChPortResourcesConfigDbTMBWGet(devNum,/*OUT*/&old_tmBandwidthMbps);
    if (rc != GT_OK)
    {
        return rc;
    }

    old_txQDescrCredits      = 0;
    old_txFifoHeaderCredits  = 0;
    old_txFifoPayloadCredits = 0;
    if (old_tmBandwidthMbps > 0)
    {
        /* compute old resoources */
        rc = prvCpssDxChBcat2_B0_TM_ResourcesCompute(devNum,PRV_CPSS_PP_MAC(devNum)->coreClock,old_tmBandwidthMbps,/*OUT*/&old_bc2_B0_TM_resConfig);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* get resources to be removed (old TM settings */
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&old_bc2_B0_TM_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit,          /*OUT*/&old_txQDescrCredits     );
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&old_bc2_B0_TM_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold,  /*OUT*/&old_txFifoHeaderCredits );
        PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&old_bc2_B0_TM_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold, /*OUT*/&old_txFifoPayloadCredits);

        /* delet old resources from system used */
        rc = prvCpssDxChPortResourcesConfigDbDelete(devNum,0,old_txQDescrCredits,old_txFifoHeaderCredits,old_txFifoPayloadCredits);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* compute new resources */
    rc = prvCpssDxChBcat2_B0_TM_ResourcesCompute(devNum,PRV_CPSS_PP_MAC(devNum)->coreClock,tmBandwidthMbps,&prv_bc2_B0_resConfig);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*-----------------------------------------------------------------------------------------------* 
	 *  get value for follwing resources in order to free them after resources in HW were configured *
     *-----------------------------------------------------------------------------------------------*/
    PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit,          /*OUT*/&txQDescrCredits     );
    PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold,  /*OUT*/&txFifoHeaderCredits );
    PRV_DXCH_BCAT2_RESOURCE_LIST_Get(&prv_bc2_B0_resConfig, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold, /*OUT*/&txFifoPayloadCredits);

    /* check resources availability */
    rc = prvCpssDxChPortResourcesConfigDbAvailabilityCheck(devNum, 0, txQDescrCredits,txFifoHeaderCredits,txFifoPayloadCredits);
    if (rc != GT_OK)
    {
        GT_STATUS rc1;
        /* restore old resources */
        rc1 = prvCpssDxChPortResourcesConfigDbAdd(devNum,0,old_txQDescrCredits,old_txFifoHeaderCredits,old_txFifoPayloadCredits);
        if (rc1 != GT_OK)
        {
            return rc1;
        }
        return rc;
    }


    /* configure in HW */
    rc = prvCpssDxChPort_Bcat2B0_ResourcesInHwConfig(devNum, &prv_bc2_B0_resConfig);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* configure DB */
    rc = prvCpssDxChPortResourcesConfigDbTMBWSet(devNum,tmBandwidthMbps,0,txQDescrCredits,txFifoHeaderCredits,txFifoPayloadCredits);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}



GT_STATUS prvCpssDxChPort_Bcat2B0_Resources2GetBouild
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC  *resConfigPtr
)
{
    GT_STATUS rc;

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(resConfigPtr);
    
    PRV_DXCH_BCAT2_RESOURCE_LIST_Init(resConfigPtr);



    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, /*OUT*/&portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (portMapShadowPtr->valid == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /*----------------------------------------------------------------------------
     * Rx-DMA 
     *    BC2_PORT_FLD_RXDMA_IfWidth 
     *----------------------------------------------------------------------------*/
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_RXDMA_IfWidth, portMapShadowPtr->portMap.rxDmaNum, 0);
    /*----------------------------------------------------------------------------
     *  Tx-DMA 
     *      BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit         
     *      BC2_PORT_FLD_TXDMA_SCDMA_burstAlmostFullThreshold    
     *      BC2_PORT_FLD_TXDMA_SCDMA_burstFullThreshold          
     *      BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold 
     *      BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold
     *------------------------------------------------------------------------------*/
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit,          portMapShadowPtr->portMap.txDmaNum, 0);
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_TXDMA_SCDMA_burstAlmostFullThreshold,     portMapShadowPtr->portMap.txDmaNum, 0);
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_TXDMA_SCDMA_burstFullThreshold,           portMapShadowPtr->portMap.txDmaNum, 0);
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold,  portMapShadowPtr->portMap.txDmaNum, 0);
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold, portMapShadowPtr->portMap.txDmaNum, 0);


    /*--------------------------------------------------------
    *  Tx-FIFO 
    *      BC2_PORT_FLD_TXFIFO_IfWidth                          
    *      BC2_PORT_FLD_TXFIFO_shifterThreshold
    *      BC2_PORT_FLD_TXFIFO_payloadThreshold           
    *--------------------------------------------------------*/
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_TXFIFO_IfWidth,           portMapShadowPtr->portMap.txFifoPortNum, 0);
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_TXFIFO_shifterThreshold,  portMapShadowPtr->portMap.txFifoPortNum, 0); /* shall have default value */
    PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr,BC2_PORT_FLD_TXFIFO_payloadThreshold,  portMapShadowPtr->portMap.txFifoPortNum, 0);


    if (portMapShadowPtr->portMap.trafficManagerEn == GT_TRUE)
    {
        GT_U32 ethTxFifoNum = portMapShadowPtr->portMap.rxDmaNum;
        /*--------------------------------------------------------
        *  Eth-Tx-FIFO
        *      BC2_PORT_FLD_Eth_TXFIFO_IfWidth                      
        *      BC2_PORT_FLD_Eth_TXFIFO_shifterThreshold             
        *      BC2_PORT_FLD_Eth_TXFIFO_payloadThreshold             
        *--------------------------------------------------------*/
        PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr, BC2_PORT_FLD_Eth_TXFIFO_IfWidth,          ethTxFifoNum, 0);
        PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr, BC2_PORT_FLD_Eth_TXFIFO_shifterThreshold, ethTxFifoNum, 0);
        PRV_DXCH_BCAT2_RESOURCE_LIST_Append(resConfigPtr, BC2_PORT_FLD_Eth_TXFIFO_payloadThreshold, ethTxFifoNum, 0);
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChBcat2PortResoursesStateGet
*
* DESCRIPTION:
*       read port resources 
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       resPtr      - pointer to list of resources
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - on bad ptr
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported revision
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       just revion B0 and above are supported
*******************************************************************************/
GT_STATUS prvCpssDxChBcat2PortResoursesStateGet
(
    IN  GT_U8                                    devNum,
    IN  GT_PHYSICAL_PORT_NUM                     portNum,
   OUT  CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC  *resPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DRV_FLD_ARR_QEUERY_STC query;
    GT_U32 i;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
            CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
            CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
            CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(resPtr);

    if (PRV_CPSS_PP_MAC(devNum)->revision == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* build list of required resources */
    rc = prvCpssDxChPort_Bcat2B0_Resources2GetBouild(devNum,portNum,/*OUT*/resPtr);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = prvCpssArrDrvQeueryInit(&query,&prv_dxChBcat2B0,devNum,0,(GT_U32*)PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum));
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0 ; i < sizeof(resPtr->arr)/sizeof(resPtr->arr[0]); i++)
    {
        if (resPtr->arr[i].fldId != BC2_PORT_FLD_INVALID_E)
        {
            rc = prvCpssArrDrvQeueryFldGet(&query,resPtr->arr[i].fldId,
                                                  resPtr->arr[i].fldArrIdx,
                                                  /*OUT*/&resPtr->arr[i].fldVal);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortBcat2B0ResourcesCalculateFromHW
*
* DESCRIPTION:
*       Calculate cummulative port resources from HW
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       resourceId  - resource id.
*                     Applicable values:
*                     BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit,
*                     BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold,
*                     BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold
*
* OUTPUTS:
*       totalResourcesNumPtr - (pointer to) total number of resources
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad input parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortBcat2B0ResourcesCalculateFromHW
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_BC2_PORT_RESOURCES_FLD_ENT    resourceId,
    OUT GT_U32                                  *totalResourcesNumPtr    
)
{
    PRV_CPSS_DRV_FLD_ARR_QEUERY_STC query;
    GT_U32 fldArrIdx;
    GT_U32 val;
    GT_U32 sum;
    GT_U32 size;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
                                                  CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E |
                                                  CPSS_LION_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(totalResourcesNumPtr);

    if ((resourceId != BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit) &&
        (resourceId != BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold) &&
        (resourceId != BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssArrDrvQeueryInit(&query, &prv_dxChBcat2B0, devNum, 0,(GT_U32*)PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum));
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssArrDrvFldAttributeArrSize(&prv_dxChBcat2B0, resourceId, &size);
    if (rc != GT_OK)
    {
        return rc;
    }

    sum = 0;
    for (fldArrIdx = 0; fldArrIdx < size; fldArrIdx++) 
    {
        rc = prvCpssArrDrvQeueryFldGet(&query, resourceId, fldArrIdx, /*OUT*/&val);
        if (rc != GT_OK)
        {
            return rc;
        }
        sum += val;
    }
    *totalResourcesNumPtr = sum;
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortBcat2B0ResourceHwInit
*
* DESCRIPTION:
*       Initialize HW with port resource
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       resourceId  - resource id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBcat2B0ResourceHwInit
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_BC2_PORT_RESOURCES_FLD_ENT    resourceId
)
{
    PRV_CPSS_DRV_FLD_ARR_QEUERY_STC query;
    GT_U32 fldArrIdx;
    GT_U32 size;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
                                                  CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E |
                                                  CPSS_LION_E | CPSS_LION2_E);

    rc = prvCpssArrDrvQeueryInit(&query, &prv_dxChBcat2B0, devNum, 0,(GT_U32*)PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum));
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssArrDrvFldAttributeArrSize(&prv_dxChBcat2B0, resourceId, &size);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (fldArrIdx = 0; fldArrIdx < size; fldArrIdx++) 
    {
        rc = prvCpssArrDrvQeueryFldSet(&query, resourceId, fldArrIdx, 0);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChBcat2PortResourcesHwInit
*
* DESCRIPTION:
*       Initialize HW with port resources
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBcat2PortResourcesHwInit
(
    GT_U8 devNum
)
{
    GT_STATUS rc;

    rc = prvCpssDxChPortBcat2B0ResourceHwInit(devNum,BC2_PORT_FLD_TXDMA_SCDMA_TxQDescriptorCredit);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortBcat2B0ResourceHwInit(devNum, BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOHeaderCreditThreshold); 
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortBcat2B0ResourceHwInit(devNum,BC2_PORT_FLD_TXDMA_SCDMA_TxFIFOPayloadCreditThreshold);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChBcat2PortResourcesInit
*
* DESCRIPTION:
*       Initialize data structure for port resource allocation
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBcat2PortResourcesInit
(
    IN    GT_U8                   devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DRV_FLD_ARR_INIT_STC * intSeqPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* prvCpssDxChBcat2Resourses[CPSS_PORT_SPEED_NA_E][PRV_BC2_CORE_CLOCK_MAX_IDX_E] =  */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                                                  CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E |
                                                  CPSS_XCAT3_E | CPSS_LION_E | CPSS_LION2_E |CPSS_XCAT2_E);

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChBcat2B0ResourcesTablesInit(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortResourcesConfigDbInit(devNum,
                                                  /* dpIndex */0,
                                                  PRV_CPSS_DXCH_BCAT2_B0_TXQ_CREDITS_CNS,
                                                  PRV_CPSS_DXCH_BCAT2_B0_TXFIFO_HEADERS_CREDITS_CNS,
                                                  PRV_CPSS_DXCH_BCAT2_B0_TXFIFO_PAYLOAD_CREDITS_CNS);
        if (rc != GT_OK)
        {
            return rc;
        }

        #ifdef PRV_SUPPORT_BCAT_A0
            intSeqPtr = &prv_bc2PortRes_revA0[0];
            rc = prvCpssArrDrvInit(/*INOUT*/&prv_dxChBcat2A0
                                            ,&prv_dxChBcat2A0_FldInitStc[0]
                                            ,intSeqPtr
                                            ,BC2_PORT_FLD_MAX);

            if (rc != GT_OK)
            {
                return rc;
            }
        #endif

        intSeqPtr = &prv_bc2PortRes_revB0[0];
        rc = prvCpssArrDrvInit(/*INOUT*/&prv_dxChBcat2B0
                                        ,&prv_dxChBcat2B0_FldInitStc[0]
                                        ,intSeqPtr
                                        ,BC2_PORT_FLD_MAX);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (PRV_CPSS_DXCH_BOBCAT2_B0_CHECK_MAC(devNum))
        {
            if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E)
            {
            rc = prvCpssDxChBcat2PortResourcesHwInit(devNum);
            if (rc != GT_OK)
            {
                return rc;
                }
            }
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}
