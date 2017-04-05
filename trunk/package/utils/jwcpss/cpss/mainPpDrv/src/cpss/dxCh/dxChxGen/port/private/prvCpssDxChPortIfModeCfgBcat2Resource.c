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


#include <mvHwsPortInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <private/mvPortModeElements.h>
#include <mac/mvHwsMacIf.h>
#include <pcs/mvHwsPcsIf.h>



/*----------------------------------------------------------------------------------------------------
//        a.  RXDMA_IF_WIDTH
//            /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<RXDMA_IP> RxDMA IP Units/Single Channel DMA Configurations/SCDMA %n Configuration 0
//            0-2 : IF_WIDTH values  0 -- width64 <= 20G
//                                   2 -- width256 == 40G
//                                   3 -- width512 == 100G ILKN
//
//            ~12 <=> 10
//            ~24 <=> 20
//            ~48 <=> 40
//
//        Regular Port
//        b.  TXDMA_IF_WIDTH
//            Bobcat2 {Current}/Switching Core/<TXFIFO_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/SCDMA %p Shifters Conf
//            A0,B0 : 0-2 : IF_WIDTH values  0 -- 1B  == 1G
//                                           3 -- 8B  == 2.5G 10G 20G
//                                           5 -- 32B == 40G
//                                           6 -- 64B == 100G
//
//        c.  Shifter %p Threshold
//            /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/<TXFIFO_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/SCDMA %p Shifters Conf
//            A0,B0 : 3-5 shifter threshold
//
//            Port Pass through TM
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


#define PRV_CPSS_DXCH_PORT_BOBCAT2_MINIMAL_IPG_CNS        2
#define PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_CONSTANT_CNS      20
#define PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_DFB_CNS           16
#define PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_SPARE_CNS        100
#define PRV_CPSS_DXCH_PORT_BOBCAT2_DESC_FIFO_SIZE_CNS   206

#define PRV_CPSS_DXCH_PORT_BOBCAT2_MPPM_MIN_LATENCY_CNS                     32
#define PRV_CPSS_DXCH_PORT_BOBCAT2_MAXIMAL_SINGLE_BANK_FIFO_FILL_LEVEL_CNS  70
#define PRV_CPSS_DXCH_PORT_BOBCAT2_NUMBER_OF_MPPM_CLIENTS_CNS                2
#define PRV_CPSS_DXCH_PORT_BOBCAT2_OTHERS_LATENCY_CNS                      267

#define PRV_CPSS_DXCH_PORT_BOBCAT2_PAYLOAD_CREDITS_CNS  541
#define PRV_CPSS_DXCH_PORT_BOBCAT2_HEADER_CREDITS_CNS   541

/*
   TxQ Credits [Desc]
+------+------+------+--------+-------------------------------------------------------------------------+
|      | Core | MPPM | Pipe   |                        Port Speed (Gbps)                                |
| MODE | Freq | Clk  | Max BW |  1 2.5  10  12  13  20  24  25  40  48  49  52  95 100 116 120 125 130  |
|      | (MHz)| Freq | (Gbps) |                                                                         |
+------+------+------+--------+-------------------------------------------------------------------------+
|  0   | 175  |  175 |   58          |  5   7  14  13  13  21  20  20  36  34  33  36  59  61  71  71  76  79  |
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

typedef struct PRV_CPSS_SPEED_2_CREDIT_STCT
{
    CPSS_PORT_SPEED_ENT speed;
    GT_U32              credit;
}PRV_CPSS_SPEED_2_CREDIT_STC;

typedef struct PRV_CPSS_CORECLOCK_2_SPEED2CREDIT_STCT
{
    GT_U32 coreClock;
    PRV_CPSS_SPEED_2_CREDIT_STC * listPtr;
}PRV_CPSS_CORECLOCK_2_SPEED2CREDIT_STC;


static PRV_CPSS_SPEED_2_CREDIT_STC clock_175_MHz_speed_2_creditList[] =
{
     {   CPSS_PORT_SPEED_10_E,    5 }
    ,{  CPSS_PORT_SPEED_100_E,    5 }
    ,{ CPSS_PORT_SPEED_1000_E,    5 }
    ,{ CPSS_PORT_SPEED_2500_E,    7 }
    ,{ CPSS_PORT_SPEED_10000_E,  14 }
    ,{ CPSS_PORT_SPEED_11800_E,  13 }
    ,{ CPSS_PORT_SPEED_20000_E,  21 }
    ,{ CPSS_PORT_SPEED_40000_E,  36 }
    ,{ CPSS_PORT_SPEED_47200_E,  34 }
    ,{ CPSS_PORT_SPEED_NA_E,      0 }
};


static PRV_CPSS_SPEED_2_CREDIT_STC clock_250_MHz_speed_2_creditList[] =
{
     {   CPSS_PORT_SPEED_10_E,    2 }
    ,{  CPSS_PORT_SPEED_100_E,    2 }
    ,{ CPSS_PORT_SPEED_1000_E,    2 }
    ,{ CPSS_PORT_SPEED_2500_E,    3 }
    ,{ CPSS_PORT_SPEED_10000_E,   8 }
    ,{ CPSS_PORT_SPEED_11800_E,   7 }
    ,{ CPSS_PORT_SPEED_20000_E,  13 }
    ,{ CPSS_PORT_SPEED_40000_E,  23 }
    ,{ CPSS_PORT_SPEED_47200_E,  22 }
    ,{ CPSS_PORT_SPEED_NA_E,      0 }
};

static PRV_CPSS_SPEED_2_CREDIT_STC clock_360_MHz_speed_2_creditList[] =
{
     {   CPSS_PORT_SPEED_10_E,    2 }
    ,{  CPSS_PORT_SPEED_100_E,    2 }
    ,{ CPSS_PORT_SPEED_1000_E,    2 }
    ,{ CPSS_PORT_SPEED_2500_E,    3 }
    ,{ CPSS_PORT_SPEED_10000_E,   7 }
    ,{ CPSS_PORT_SPEED_11800_E,   6 }
    ,{ CPSS_PORT_SPEED_20000_E,  10 }
    ,{ CPSS_PORT_SPEED_40000_E,  17 }
    ,{ CPSS_PORT_SPEED_47200_E,  16 }
    ,{ CPSS_PORT_SPEED_NA_E,      0 }
};

static PRV_CPSS_SPEED_2_CREDIT_STC clock_520_MHz_speed_2_creditList[] =
{
     {   CPSS_PORT_SPEED_10_E,    2 }
    ,{  CPSS_PORT_SPEED_100_E,    2 }
    ,{ CPSS_PORT_SPEED_1000_E,    2 }
    ,{ CPSS_PORT_SPEED_2500_E,    3 }
    ,{ CPSS_PORT_SPEED_10000_E,   6 }
    ,{ CPSS_PORT_SPEED_11800_E,   6 }
    ,{ CPSS_PORT_SPEED_20000_E,   8 }
    ,{ CPSS_PORT_SPEED_40000_E,  13 }
    ,{ CPSS_PORT_SPEED_47200_E,  13 }
    ,{ CPSS_PORT_SPEED_NA_E,      0 }
};



static PRV_CPSS_CORECLOCK_2_SPEED2CREDIT_STC clock_MHz_2_speed2creditList_List[] =
{
     { 175, &clock_175_MHz_speed_2_creditList[0] }
    ,{ 250, &clock_250_MHz_speed_2_creditList[0] }
    ,{ 362, &clock_360_MHz_speed_2_creditList[0] }
    ,{ 521, &clock_520_MHz_speed_2_creditList[0] }
    ,{   0, (PRV_CPSS_SPEED_2_CREDIT_STC *)NULL  }
};


GT_U32 prvCpssDxChPortBcatTxDMACreditNumberGet
(
    GT_U32              coreClock,
    CPSS_PORT_SPEED_ENT speed
)
{
    GT_U32 clockIdx;
    GT_U32 speedIdx;

    PRV_CPSS_CORECLOCK_2_SPEED2CREDIT_STC * clock2listPtr;
    PRV_CPSS_SPEED_2_CREDIT_STC           * speed2creditListPtr;

    clock2listPtr = &clock_MHz_2_speed2creditList_List[0];
    for (clockIdx = 0; clock2listPtr->coreClock != 0; clockIdx++, clock2listPtr++)
    {
        if (clock2listPtr->coreClock == coreClock)
        {
            speed2creditListPtr = clock2listPtr->listPtr;
            for (speedIdx = 0; speed2creditListPtr->speed != CPSS_PORT_SPEED_NA_E; speedIdx++, speed2creditListPtr++)
            {
                if (speed2creditListPtr->speed == speed)
                {
                    return speed2creditListPtr->credit;
                }
            }
            return 0;
        }
    }
    return 0; /* not found */
}


/*-----------------------------------------------------------------------------
 *  TxQ credits number
 *-----------------------------------------------------------------------------*/

GT_STATUS prvCpssDxChPortBcat2TxQCreditNumberGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                         *creditNumPtr
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldSize;
    GT_U32   portNumMappedForUnit;

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);



    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
    if (portNumMappedForUnit == CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        fieldOffset = 0;
        fieldSize = 9;
    }
    else
    {
        fieldOffset = 17;
        fieldSize = 8;
    }

    if (portNumMappedForUnit >= sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1)/sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[0]))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[portNumMappedForUnit];
    rc = prvCpssHwPpPortGroupGetRegField(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,
                                            fieldOffset, fieldSize, creditNumPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


GT_STATUS prvCpssDxChPortBcat2TxQCreditNumberSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                          creditNum
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldSize;
    GT_U32   portNumMappedForUnit;

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);




    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
    if (portNumMappedForUnit == CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        fieldOffset = 0;
        fieldSize = 9;
    }
    else
    {
        fieldOffset = 17;
        fieldSize = 8;
    }

    if (portNumMappedForUnit >= sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1)/sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[0]))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[portNumMappedForUnit];
    rc = prvCpssHwPpPortGroupSetRegField(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,
                                            fieldOffset, fieldSize, creditNum);
    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*-----------------------------------------------------------------------------
 *  TxFIFO Headers number
 *-----------------------------------------------------------------------------*/
GT_STATUS prvCpssDxChPortBcat2TxFIFOHeadersCreditNumberSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                          creditNum
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldSize;
    GT_U32   portNumMappedForUnit;

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);




    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
    if (portNumMappedForUnit == CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fieldOffset = 0;
    fieldSize   = 10;

    if (portNumMappedForUnit >= sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1)/sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[0]))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA[portNumMappedForUnit];
    rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, fieldOffset, fieldSize, creditNum);

    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS prvCpssDxChPortBcat2TxFIFOHeadersCreditNumberGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                         *creditNumPtr
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldSize;
    GT_U32   portNumMappedForUnit;

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);




    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
    if (portNumMappedForUnit == CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fieldOffset = 0;
    fieldSize   = 10;

    if (portNumMappedForUnit >= sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1)/sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[0]))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA[portNumMappedForUnit];
    rc = prvCpssHwPpPortGroupGetRegField(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, fieldOffset, fieldSize, creditNumPtr);

    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*-----------------------------------------------------------------------------
 * TxFIFO payloads
 *-----------------------------------------------------------------------------*/
GT_STATUS prvCpssDxChPortBcat2TxFIFOPayloadsCreditNumberSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                          creditNum
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldSize;
    GT_U32   portNumMappedForUnit;

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);




    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
    if (portNumMappedForUnit == CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fieldOffset = 10;
    fieldSize   = 10;

    if (portNumMappedForUnit >= sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1)/sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[0]))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA[portNumMappedForUnit];
    rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, fieldOffset, fieldSize, creditNum);

    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS prvCpssDxChPortBcat2TxFIFOPayloadsCreditNumberGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                         *creditNumPtr
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldSize;
    GT_U32   portNumMappedForUnit;

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);




    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
    if (portNumMappedForUnit == CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fieldOffset = 10;
    fieldSize   = 10;

    if (portNumMappedForUnit >= sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1)/sizeof(PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.FIFOsThresholdsConfigsSCDMAReg1[0]))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA[portNumMappedForUnit];
    rc = prvCpssHwPpPortGroupGetRegField(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, fieldOffset, fieldSize, creditNumPtr);

    if(rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}




extern GT_BOOL g_doNotCheckCredits;




/*******************************************************************************
* prvCpssDxChPort_Bcat2A0_ResourcesConfig
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
*       JUST rev A0
*******************************************************************************/
GT_STATUS prvCpssDxChPort_Bcat2A0_ResourcesConfig
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_BOOL                         allocate
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldSize;

    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC *regsAddrPtr =
                                        PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);
    GT_U32  portNumMappedForUnit;
    float   speedForCalc,
            A64B_packet_time_ns,
            A128B_trans_time_ns,
            A64B_packet_time_cc,
            /*A128B_trans_time_cc,*/
            MPPM_latency_variance,
            MPPM_max_latency,
            variance_latency,
            total_max_latency_cc,
            total_max_latency_ns,
            coreFrequency,
            mppmFrequency;

    GT_U32  A64B_sent,
            txq_latency,
            min_thr_for_transmission,
            total_IDs,
            value;

    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;

    GT_U32  gopBandwidth; /*  = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.pipeBWCapacityInGbps; *//* max chip pipe in HW spec 125Gb */

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc = hwPpPhase1Part2Enhanced_BandWidthByDevRevCoreClockGet(devNum, /*OUT*/&gopBandwidth);
    if(rc != GT_OK)
    {
        return rc;
    }


    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        fieldOffset = 0;
        fieldSize = 9;
    }
    else
    {
        fieldOffset = 17;
        fieldSize = 8;
    }

    if(!allocate)
    {/* release resources */
        /* DESCRIPTOR_CREDITS_SCDMA */
        if(portMapShadowPtr->portMap.trafficManagerEn == GT_FALSE)
        {
            rc = prvCpssDxChPortBcat2TxQCreditNumberGet(devNum,portNum, &value);
            if(rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDxChPortBcat2TxQCreditNumberSet(devNum,portNum, 0);
            if(rc != GT_OK)
            {
                return rc;
            }

            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[0] -= value;
            /* --- TXFIFO_PAYLOAD_COUNTER_THRESHOLD_SCDMA and
                                    TXFIFO_HEADER_COUNTER_THRESHOLD_SCDMA --- */
            portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
            regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA[portNumMappedForUnit];
            rc = prvCpssHwPpPortGroupGetRegField(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 0, 20, &value);
            if(rc != GT_OK)
            {
                    return rc;
            }
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[0] -= ((value>>10)&0x3FF);
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[0]  -= (value&0x3FF);
            rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 0, 20, 0);
            if(rc != GT_OK)
            {
                    return rc;
            }
        }
        return GT_OK;
    }

    /* ---------------- RXDMA_IF_WIDTH (RXDMA_GOP_MODE) ------------- */
CPSS_TBD_BOOKMARK_BOBCAT2 /* it should be fixed to be configurable by application
                            for now use default */
    switch(speed)
    {
        case CPSS_PORT_SPEED_10_E:
        case CPSS_PORT_SPEED_100_E:
        case CPSS_PORT_SPEED_1000_E:
        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_12000_E:
        case CPSS_PORT_SPEED_2500_E:
        case CPSS_PORT_SPEED_5000_E:
        case CPSS_PORT_SPEED_13600_E:
        case CPSS_PORT_SPEED_16000_E:
        case CPSS_PORT_SPEED_15000_E:
        case CPSS_PORT_SPEED_20000_E:
        case CPSS_PORT_SPEED_11800_E:
            value   = 0; /* width64 */
            break;

        case CPSS_PORT_SPEED_40000_E:
        case CPSS_PORT_SPEED_47200_E:
            value   = 2; /* width256 */
            break;

        case CPSS_PORT_SPEED_75000_E:
        case CPSS_PORT_SPEED_100G_E:
        case CPSS_PORT_SPEED_50000_E:
        case CPSS_PORT_SPEED_140G_E:
            value   = 3; /* width512 */
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((ifMode == CPSS_PORT_INTERFACE_MODE_ILKN4_E) ||
       (ifMode == CPSS_PORT_INTERFACE_MODE_ILKN8_E))
    {
        value = 3;
    }

    portNumMappedForUnit = portMapShadowPtr->portMap.rxDmaNum;
    regAddr = PRV_DXCH_REG1_UNIT_RXDMA_MAC(devNum).singleChannelDMAConfigs.SCDMAConfig0[portNumMappedForUnit];
    rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, 0, 3, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* ---------------- TXDMA_IF_WIDTH (TXFIFO_GOP_MODE) ------------- */
CPSS_TBD_BOOKMARK_BOBCAT2 /* should be configurable by application ? */

    /* Outgoing Bus %p Width */
    switch(speed)
    {
        case CPSS_PORT_SPEED_10_E:
        case CPSS_PORT_SPEED_100_E:
        case CPSS_PORT_SPEED_1000_E:
            value   = 0; /* 1B */
            break;
        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_12000_E:
        case CPSS_PORT_SPEED_2500_E:
        case CPSS_PORT_SPEED_5000_E:
        case CPSS_PORT_SPEED_13600_E:
        case CPSS_PORT_SPEED_16000_E:
        case CPSS_PORT_SPEED_15000_E:
        case CPSS_PORT_SPEED_20000_E:
        case CPSS_PORT_SPEED_11800_E:
            value   = 3; /* 8B */
            break;

        case CPSS_PORT_SPEED_40000_E:
        case CPSS_PORT_SPEED_47200_E:
            value   = 5; /* 32B */
            break;

        case CPSS_PORT_SPEED_75000_E:
        case CPSS_PORT_SPEED_100G_E:
        case CPSS_PORT_SPEED_50000_E:
        case CPSS_PORT_SPEED_140G_E:
            value   = 6; /* 64B */
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /*--------------------------------------------------------------* 
     * overwrite default value of CPU DMA-72 (1B at speed 1G        *
     * CPU DMA-72 has outgoing bus width 8B                         *
     *--------------------------------------------------------------*/ 
    if (portMapShadowPtr->portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E)
    {
        value = 3; /* 8B */
    }

    /* Shifter %p Threshold */
    switch(speed)
    {
        case CPSS_PORT_SPEED_10_E:
        case CPSS_PORT_SPEED_100_E:
        case CPSS_PORT_SPEED_1000_E:
        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_12000_E:
        case CPSS_PORT_SPEED_2500_E:
        case CPSS_PORT_SPEED_5000_E:
        case CPSS_PORT_SPEED_13600_E:
        case CPSS_PORT_SPEED_16000_E:
        case CPSS_PORT_SPEED_15000_E:
        case CPSS_PORT_SPEED_20000_E:
        case CPSS_PORT_SPEED_11800_E:
            value |= (2 << 3);
            break;

        case CPSS_PORT_SPEED_40000_E:
        case CPSS_PORT_SPEED_75000_E:
        case CPSS_PORT_SPEED_50000_E:
        case CPSS_PORT_SPEED_47200_E:
            value |= (3 << 3);
            break;

        case CPSS_PORT_SPEED_100G_E:
        case CPSS_PORT_SPEED_140G_E:
            value |= (4 << 3);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((ifMode == CPSS_PORT_INTERFACE_MODE_ILKN4_E) ||
       (ifMode == CPSS_PORT_INTERFACE_MODE_ILKN8_E))
    {
        value = 0x36;
    }

        /* in case of TM don't change TM TxDMA (73) properties */
    if(portMapShadowPtr->portMap.trafficManagerEn == GT_FALSE)
    {
        portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
        regAddr = PRV_DXCH_REG1_UNIT_TX_FIFO_MAC(devNum).txFIFOShiftersConfig.SCDMAShiftersConf[portNumMappedForUnit];
        rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 0, 6, value);
        if(rc != GT_OK)
        {
                return rc;
        }
    }
    else /* (portMapShadowPtr->portMap.trafficManagerEn == GT_TRUE) */
    {
        if (portMapShadowPtr->portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E)
        {
            portNumMappedForUnit = portMapShadowPtr->portMap.rxDmaNum;
            regAddr = regsAddrPtr->SIP_ETH_TXFIFO[0].txFIFOShiftersConfig.SCDMAShiftersConf[portNumMappedForUnit];
            rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 0, 6, value);
            if(rc != GT_OK)
            {
                return rc;
            }
            regAddr = regsAddrPtr->SIP_ETH_TXFIFO[0].txFIFOGlobalConfig.SCDMAPayloadThreshold[portNumMappedForUnit];
            rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    regAddr, 0, 8, 0xF0);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(portMapShadowPtr->portMap.trafficManagerEn == GT_FALSE)
    {
        /* --------------- Configure txq_credits -------------------- */
        /* ------------ DESCRIPTOR_CREDITS_SCDMA ------------- */

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            fieldOffset = 16;
            fieldSize = 14;
        }
        else
        {
            fieldOffset = 16;
            fieldSize = 13;
        }

        switch(speed)
        {
            case CPSS_PORT_SPEED_10_E:
            case CPSS_PORT_SPEED_100_E:
            case CPSS_PORT_SPEED_1000_E:
                    speedForCalc   = 1;
                    value = 0;
                    break;
            case CPSS_PORT_SPEED_10000_E:
                    speedForCalc   = 10;
                    value = 2;
                    break;
            case CPSS_PORT_SPEED_11800_E:
                    speedForCalc   = (float)11.818;
                    value = 2;
                    break;
            case CPSS_PORT_SPEED_12000_E:
                    speedForCalc   = 12;
                    value = 2;
                    break;
            case CPSS_PORT_SPEED_2500_E:
                    speedForCalc   = (float)2.5;
                    value = 1;
                    break;
            case CPSS_PORT_SPEED_5000_E:
                    speedForCalc   = 5;
                    value = 1;
                    break;
            case CPSS_PORT_SPEED_13600_E:
                    speedForCalc   = (float)13.6;
                    value = 2;
                    break;
            case CPSS_PORT_SPEED_16000_E:
                    speedForCalc   = 16;
                    value = 2;
                    break;
            case CPSS_PORT_SPEED_15000_E:
                    speedForCalc   = 15;
                    value = 2;
                    break;
            case CPSS_PORT_SPEED_20000_E:
                    speedForCalc   = 20;
                    value = 3;
                    break;

            case CPSS_PORT_SPEED_40000_E:
                    speedForCalc   = 40;
                    value = 4;
                    break;

            case CPSS_PORT_SPEED_47200_E:
                    speedForCalc   = (float)47.272;
                    value = 4;
                    break;

            case CPSS_PORT_SPEED_75000_E:
                    speedForCalc   = 75;
                    value = 4;
                    break;
            case CPSS_PORT_SPEED_100G_E:
                    speedForCalc   = 100;
                    value = 5;
                    break;
            case CPSS_PORT_SPEED_50000_E:
                    speedForCalc   = 50;
                    value = 4;
                    break;
            case CPSS_PORT_SPEED_140G_E:
                    speedForCalc   = 140;
                    value = 6;
                    break;
            default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        portNumMappedForUnit = portMapShadowPtr->portMap.txDmaNum;
        if(!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            value |= PRV_CPSS_DXCH_BC2_PORT_TXDMA_CONFIG_CALC_MAC(speedForCalc);
        }

        regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.SCDMAConfigs[portNumMappedForUnit];
        rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, fieldOffset, fieldSize, value);
        if(rc != GT_OK)
        {
            return rc;
        }

        A64B_packet_time_ns = ((64 + PRV_CPSS_DXCH_PORT_BOBCAT2_MINIMAL_IPG_CNS)*8)
                                                        /speedForCalc;
        A128B_trans_time_ns = (128*8)/speedForCalc;

        coreFrequency = (float)PRV_CPSS_PP_MAC(devNum)->coreClock;

        switch (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.revision)
        {
            case 0: /* just BC2 A0 */
            {

                A64B_packet_time_cc = A64B_packet_time_ns/(((float)1000)/coreFrequency);
                /*A128B_trans_time_cc = A128B_trans_time_ns/(((float)1000)/coreFrequency);*/
                txq_latency = PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_CONSTANT_CNS +
                                                        PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_DFB_CNS +
                                                        PRV_CPSS_DXCH_PORT_BOBCAT2_TXQ_SPARE_CNS +
                                                        /* round up devision */
                                                        2*(GT_U32)((gopBandwidth+(speedForCalc-1))/speedForCalc);

                /* DESCRIPTOR_CREDITS_SCDMA */
                value = (GT_U32)((txq_latency+(A64B_packet_time_cc-1))/A64B_packet_time_cc);
            }
            break;
            case 1:/* just BC2 B0 */
            {
                value = prvCpssDxChPortBcatTxDMACreditNumberGet(PRV_CPSS_PP_MAC(devNum)->coreClock,speed);
                if (value == 0)  /* either clock or speed is not found !!! */
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }
            }
            break;
            default:
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
        }

        if (g_doNotCheckCredits == GT_FALSE)  /* check credits */
        {
            if((PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[0] + value) > PRV_CPSS_DXCH_PORT_BOBCAT2_DESC_FIFO_SIZE_CNS)
            {
                cpssOsPrintf("\n--> ERROR : TXQ credit: number (%d) of credit exceeds limit (%d)\n",
                                      PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[0] + value,
                                      PRV_CPSS_DXCH_PORT_BOBCAT2_DESC_FIFO_SIZE_CNS);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }

        rc = prvCpssDxChPortBcat2TxQCreditNumberSet(devNum,portNum,value);
        if (rc != GT_OK)
        {
            return rc;
        }
#if 0
        cpssOsPrintf("  usedDescCredits = %3d add = %2d",PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits,
                                                        value);
#endif
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedDescCredits[0] += value;
        /* BURST_ALMOST_FULL_THRESHOLD_SCDMA */
CPSS_TBD_BOOKMARK_BOBCAT2 /* this feature has enable bit and according to Shira
                                                        not mandatory, need DR to find out how to configure it */
#if 0
        value = (GT_U32)((txq_latency + (A128B_trans_time_cc - 1)) / A128B_trans_time_cc);
        regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.
                                        burstLimiterSCDMA[portNumMappedForUnit];
        rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                /* value + enable bit */
                                                regAddr, 15, 16, (value | (1<<15)));
        if(rc != GT_OK)
        {
                return rc;
        }
#endif
        /* --- TXFIFO_PAYLOAD_COUNTER_THRESHOLD_SCDMA and TXFIFO_HEADER_COUNTER_THRESHOLD_SCDMA --- */

        MPPM_latency_variance = PRV_CPSS_DXCH_PORT_BOBCAT2_MAXIMAL_SINGLE_BANK_FIFO_FILL_LEVEL_CNS*
                                PRV_CPSS_DXCH_PORT_BOBCAT2_NUMBER_OF_MPPM_CLIENTS_CNS;

        MPPM_max_latency = PRV_CPSS_DXCH_PORT_BOBCAT2_MPPM_MIN_LATENCY_CNS +
                                                        MPPM_latency_variance;

CPSS_TBD_BOOKMARK_BOBCAT2 /* usually memory frequency indeed equal to core clock, but could be not.
                                                        Need to check if external configuration should be supported. */
        mppmFrequency = coreFrequency;

        variance_latency        = MPPM_latency_variance/mppmFrequency*(float)1000;
        total_max_latency_cc    = MPPM_max_latency*coreFrequency/mppmFrequency + PRV_CPSS_DXCH_PORT_BOBCAT2_OTHERS_LATENCY_CNS;
        total_max_latency_ns    = total_max_latency_cc/coreFrequency*(float)1000;
        A64B_sent               = (GT_U32)((total_max_latency_ns + (A64B_packet_time_ns - 1))/
                                                                           A64B_packet_time_ns);
        min_thr_for_transmission = (GT_U32)((variance_latency + (A128B_trans_time_ns - 1))/
                                                                                A128B_trans_time_ns);
        total_IDs               = A64B_sent + min_thr_for_transmission;

        if (g_doNotCheckCredits == GT_FALSE)  /* check credits */
        {
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[0] + total_IDs > PRV_CPSS_DXCH_PORT_BOBCAT2_PAYLOAD_CREDITS_CNS)
            {
                    cpssOsPrintf("\n--> ERROR : TxFIFO payload credits: number (%d) of credit exceeds limit (%d)\n",
                                          PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[0] + total_IDs,
                                          PRV_CPSS_DXCH_PORT_BOBCAT2_PAYLOAD_CREDITS_CNS);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

            }
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[0] + total_IDs > PRV_CPSS_DXCH_PORT_BOBCAT2_HEADER_CREDITS_CNS)
            {
                cpssOsPrintf("\n--> ERROR : TxFIFO headers credits: number (%d) of credit exceeds limit (%d)\n",
                                        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[0] + total_IDs,
                                        PRV_CPSS_DXCH_PORT_BOBCAT2_HEADER_CREDITS_CNS);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }

        rc = prvCpssDxChPortBcat2TxFIFOPayloadsCreditNumberSet(devNum,portNum,total_IDs);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDxChPortBcat2TxFIFOHeadersCreditNumberSet(devNum,portNum,total_IDs);
        if (rc != GT_OK)
        {
            return rc;
        }
#if 0
        value = total_IDs | (total_IDs << 10);
        regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMAPerSCDMAConfigs.txFIFOCntrsConfigsSCDMA[portNumMappedForUnit];
        rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 0, 20, value);
        if(rc != GT_OK)
        {
            return rc;
        }
#endif
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedPayloadCredits[0] += total_IDs;
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.groupResorcesStatus.usedHeaderCredits[0] += total_IDs;

        /* SCDMA_PAYLOAD_THRESHOLD */
        regAddr = PRV_DXCH_REG1_UNIT_TX_FIFO_MAC(devNum).txFIFOGlobalConfig.SCDMAPayloadThreshold[portNumMappedForUnit];
        rc = prvCpssHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 0, 7, min_thr_for_transmission);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

CPSS_TBD_BOOKMARK_BOBCAT2
#if 0
    /* values configured by this function cause TX FIFO underrun interrupts
        and harm traffic */
    rc = prvCpssDxChPortBcat2TxFifoThresholdsSet(devNum, portNum, ifMode, speed);
#endif

    return rc;
}



