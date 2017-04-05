/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortMapping.c
*
* DESCRIPTION:
*       CPSS physical port mapping
*
* FILE REVISION NUMBER:
*       $Revision: 26 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>



#define PRV_CPSS_MAPPING_DEBUG_CNS           0
#if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
    #define cpssOsPrintf printf
    #define PRV_CPSS_MAPPING_CHECK_PRINT_CNS     1
    #define PRV_CPSS_MAPPING_UNIT_LIST_DEBUG_CNS 0
#else
    #define PRV_CPSS_MAPPING_CHECK_PRINT_CNS     0
    #define PRV_CPSS_MAPPING_UNIT_LIST_DEBUG_CNS 0
#endif



/*-----------------------------------------------------------------------------------------------------
+-----+-----------------------------------------------------------------------+-----------------+
| #   |       Mapping                                                         |  Type           |
+-----+-----------------------------------------------------------------------+-----------------+
| 1   | RX DMA to Local Device Physical Source Port                           | RxDMA  -> Phys  |
| 2   | Cut Through TX DMA map of Local Device Physical Port to RX DMA number | Phys --> TxDMA  |
| 3   | BMA map of Local Device Physical Port to RX DMA number                | Phys -> RxDMA   |
| 4.1 | EGF Target Local Physical Port to TxQ port map                        | Phys -> TxQ     |
| 4.2 | Target Local Physical Port to MAC number map                          | Phys -> MAC     |
| 5   | TxQ port to TX DMA map                                                | TxQ  -> TxDMA   |
| 6   | PFC Response Local Device Source Port to TxQ port map                 | Phys -> TxQ     |
| 7   | PFC Trigger Local Device Source Port to MAC map                       | Phys -> MAC     |
| 8   | Local Device Target Physical to hemisphere map                        | Phys -> TxQ (?) |
| 9   | CPU TxQ port number                                                   | Phys -> TxQ     |
+-----+-----------------------------------------------------------------------+-----------------+

Also used ...

/Cider/EBU/Bobcat2B/Bobcat2 {Current}/Switching Core/TM_Egress_Glue/Target Interface       TM --> TxDMA ?
                                                                                           TM --> Ilkn channel
/Cider/EBU/Bobcat2B/Bobcat2 {Current}/Switching Core/TM_FCU/EthDma2TM Port Mapping Table   RxDMA --> TM
/Cider/EBU/Bobcat2B/Bobcat2 {Current}/Switching Core/TM_FCU/IlkDma2TM Port Mapping Table   Ilkn  --> TM

Mapping related to PFC message to TM:
/Cider/EBU/Bobcat2B/Bobcat2 {Current}/Switching Core/TM_FCU/TM FCU Ingress Port Mapping<<%r>>
/Cider/EBU/Bobcat2B/Bobcat2 {Current}/Switching Core/TM_FCU/TC Port to CNode Port Mapping Table



+------------+-----------+-----------+-----------+-----------+-----------+
|            |  0  Phys  |  1  MAC   |  2  RxDMA |  3 TxDMA  |  4  TxQ   |
+------------+-----------+-----------+-----------+-----------+-----------+
| Phys     0 | ---       |  7 4.2    |  3        | 2         | 4.1 6     |
+------------+-----------+-----------+-----------+-----------+-----------+
| MAC      1 |           | ---       |           |           |           |
+------------+-----------+-----------+-----------+-----------+-----------+
| RxDMA    2 |   1       |           | ---       |           |           |
+------------+-----------+-----------+-----------+-----------+-----------+
| TxDMA    3 |           |           |           | ---       |           |
+------------+-----------+-----------+-----------+-----------+-----------+
| TxQ      4 |           |           |           | 5         | ---       |
+------------+-----------+-----------+-----------+-----------+-----------+



+-----------------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
|                                                                             | ETHER_MAC | CPU_SDMA  |  ILNK_MAC |  ILNK_CHNL|  REMOTE_P |
+-----------------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 1    | RX DMA to Local Device Physical Source Port                          |    x      |    x      |  x(const) |           |           |
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 2    | Cut Through TX DMA map of Local Device Physical Port to RX DMA number|    x      |    x      |    x      |    x      |    x      | Bobcat3 only
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 3    | BMA map of Local Device Physical Port to RX DMA number               |    x      |    x      |    x      |    x      |    x      |
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 4.1  | EGF Target Local Physical Port to TxQ port map                       |    x      |    x      |    x      |    x      |    x      |
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 4.2  | EGF Target Local Physical Port to MAC number                         |    x      |    x      |    x      |    x      |    x      |
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 5    | TxQ port to TX DMA map                                               |    x      |    x      |    x      |    x      |    x      |
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 6    | PFC Response Local Device Source Port to TxQ port map                |    x      |    x      |    x      |    x      |    x      |
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 7    | PFC Trigger Local Device Source Port to MAC map                      |    x      |           |    x      |    x      |    x      |
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 8    | Local Device Target Physical to hemisphere map                       |    x      |    ?      |           |           |           | Lion 3 only
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+
| 9    | CPU TxQ port number                                                  |           |    x      |           |           |           |
+------+----------------------------------------------------------------------+-----------+-----------+-----------+-----------+-----------+

ILNK_MAC : MAC 1 DMA 1 64 channels (physical port : base + channel#)
   mapping 5 TxQ port to TX DMA map

   if tmEnable == TRUE
       txDMA TM port 73 instead of inteface number

there is need to INLK_CHANNEL (all 9 mapping)


GetInteface

                      +----> 72 (BobCat2) CPU_SDMA)
                      |
      (3)             |             (1)
Phy -------->RxDMA ---+----> 73  -------->RxDMA --->  +-- n1 != n2 ---------> ILKN_CNHL
              (n1)    |                    (n2)       |
                      |                               +---n1 == n2 --------> INLKN_MAC
                      |               (1)
                      +---> !(72,73) ------> RxDMA --> +-- n1 != n2 ---------> ETHERNET
                                              n2       |
                                                       +---n1 == n2 --------> REMOTE_PORT


(1) RX DMA to Local Device Physical Source Port

formula : 0x17000450 + n*0x4: where n (0-72) represents Port
  7       0     Port Number <<%n>>  0x<<%n>> The source port number assigned to the packet upon its reception.
                                     According to this field, the port number will be changed when sending it to the control pipe.


(2) Cut Through TX DMA map of Local Device Physical Port to RX DMA number

  Address: 0x26000020 + [i/4]*0x4: where i (0-63) represents entry

   0  -  7      4 ports per register, total
   8  - 15
   16 - 23
   24 - 31

(3)  BMA map of Local Device Physical Port to RX DMA number
  Port Mapping table %n
  Description: Virtual => Physical source port mapping
  Address: CPU 0x2D01A000
  Size: 1024 Bytes

(4.1) EGF Target Local Physical Port to TxQ port map
  Internal Name: Target_Port_Mapper
  Address: CPU 0x3B900000

  Size: 1024 Bytes
  Width (Line Width): 8 Bits
  No. of Lines: 256
  Line Address Alignment: 4 bytes

(4.2) EGF Target Local Physical Port to Mac number
    Offset Formula: 0x35010120 + 4*n: where n (0-63) represents offset
    0- 7    Remaps port <<4*%n+0>> to its corresponding MAC number
    8-15    Remaps port <<4*%n+1>> to its corresponding MAC number
   16-23    Remaps port <<4*%n+2>> to its corresponding MAC number
   24-31    Remaps port <<4*%n+3>> to its corresponding MAC number

(5) TxQ port to TX DMA map

    Address: CPU   0x400002C0
    formula : 0x400002C0 + n*0x4: where n (0-71) represents Port

(6) PFC Response Local Device Source Port to TxQ port map  (Phys --> TxQ)
    0x4000C500 + p*4: where p (0-255) represents Source Port
    0-6

(7) PFC Trigger Local Device Source Port to MAC map  (Phys --> MAC)
    Offset Formula: 0x3E000900 + p*4: where p (0-255) represents Global Port
     0-3 PFC Counter Index
     4-6 PFC Counter block
     7-9 Local Core ID
    10-17 MAC Index
*/
/*---------------------------------------------------------*/
/* Useful data structures                                  */
/*---------------------------------------------------------*/

#define PRV_CPSS_DXCH_UNUSED_PARAM_MAC(p) p = p

typedef GT_STATUS (*PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 resId,
    IN GT_U32 valueToSet
);

typedef GT_STATUS (*PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 resId,
   OUT GT_U32 * valuePtr
);



/****************************************************
 * typedef: enum PRV_CPSS_DXCH_PORT_TYPE_ENT
 *
 * Description: port type used in mapping functions
 *
 *
 * Enumerations:
 *    PRV_MAPTYPE_INVALID_E           - invalide maping type
 *    PRV_MAPTYPE_ETHERNET_MAC_E      - Ethernet MAC
 *    PRV_MAPTYPE_CPU_SDMA_E          - CPU SDMA
 *    PRV_MAPTYPE_ILKN_MAC_E          - Interlaken Channel where mac shall be configured ( first in list)
 *    PRV_MAPTYPE_ILKN_CHANNEL_E      - Interlaken Channel where mac shall be not configured
 *    PRV_MAPTYPE_REMOTE_PHYS_PORT_E  - Port represent physical interface on remote device or Interlaken channel.
 ****************************************************/

typedef enum
{
    PRV_MAPTYPE_INVALID_E          = 0x0,
    PRV_MAPTYPE_ETHERNET_MAC_E     = BIT_0,
    PRV_MAPTYPE_CPU_SDMA_E         = BIT_1,
    PRV_MAPTYPE_ILKN_MAC_E         = BIT_2,
    PRV_MAPTYPE_ILKN_CHANNEL_E     = BIT_3,
    PRV_MAPTYPE_REMOTE_PHYS_PORT_E = BIT_4
}PRV_MAPTYPE_ENT;

/******************************************************************
 * partial mapping description structure
 *      describe operation set/get on particalr mapping
 *      ---------------------------
 *      mapping[srcPort] = dstPort
 *      ---------------------------
 *
 *      stringMapTypePtr - pointer mapping name (debug purpose only) , at the end of list NULL
 *      mapingTypeBmp    - bitmap of all mapping (system) when this prticular mapping type is used
 *                         0 at the end of list of all mapping
 *      applicableDevice - appicable device (same mapping are supported on specific device
 *                         (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3, or both, 0 and the end of list)
 *      srcPortType      - mapping source from;
 *                         (APPLICABLE RANGES: PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
 *                                             PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E,
 *                                             PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E,
 *                                             PRV_CPSS_DXCH_PORT_TYPE_MAC_E,
 *                                             PRV_CPSS_DXCH_PORT_TYPE_TXQ_E;
 *                              PRV_CPSS_DXCH_PORT_TYPE_INVALID_E at the end of list)
 *      dstPortType      - mapping destination from;
 *                         (APPLICABLE RANGES: PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
 *                                             PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E,
 *                                             PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E,
 *                                             PRV_CPSS_DXCH_PORT_TYPE_MAC_E,
 *                                             PRV_CPSS_DXCH_PORT_TYPE_TXQ_E;
 *                              PRV_CPSS_DXCH_PORT_TYPE_INVALID_E at the end of list)
 *      setFun          - pointer to set functiopn (NULL at the end)
 *      getFun          - pointer to get functiopn (NULL at the end)
 *
 * Comments:
 *      array of ports (of size CPSS_DXCH_PORT_MAPPING_TYPE_MAX_E) shall be build by the system arguments;
 *      +-------+-------+-------+------+-------+
 *      | phys  | rxdma | txdma | mac  |  txq  |
 *      +-------+-------+-------+------+-------+
 *      the caller to set function shall get argument     from source index and
 *                                           destionation from destination index
 *      +--------------+-----------+----------+-------+------+-----+----+
 *      | "map-type-1" | ETHER,CPU |BC2,LION3 | rxdma | phys | SetF|GetF|
 *      | "map-type-2" | ETHER     | LION3    | rxdma | txqs | SetF|GetF|
 *      | NULL         | 0         | 0        | INV   | INV  |NULL |NULL|
 *      +--------------+-----------+----------+-------+------+-----+----+
 *        the called iterate over the list till mappingTypeBmp is not zero.
 *             if mapping type and system type are appropriate,
 *             calls the set function , taking argument from argument array at relevant array.
 *******************************************************************/
typedef struct
{
    #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        GT_CHAR                          * stringMapTypePtr;
    #endif
    GT_U32                                 mapingTypeBmp;
    GT_U32                                 applicableDevice;
    PRV_CPSS_DXCH_PORT_TYPE_ENT            srcPortType;
    PRV_CPSS_DXCH_PORT_TYPE_ENT            dstPortType;
    PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR      setFun;
    PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR      getFun;
}PRV_DXCH_CPSS_MAPPING_SET_GET_FUN_STC;


#define PRV_CPSS_DXCH_MAPPING_BOBCAT2_MAX_DMA_NUM_CNS   71
#define PRV_CPSS_DXCH_MAPPING_BOBCAT2_MAX_MAC_NUM_CNS   71
#define PRV_CPSS_DXCH_MAPPING_LION3_MAX_DMA_NUM_CNS     31
#define PRV_CPSS_DXCH_MAPPING_LION3_MAX_MAC_NUM_CNS     31
#define PRV_CPSS_DXCH_MAPPING_MAX_TXQ_PORT_CNS(dev)          \
        (PRV_CPSS_SIP_5_20_CHECK_MAC(dev) ?                  \
            SIP_5_20_TXQ_DQ_MAX_PORTS_MAC(dev):              \
             71)


#define PRV_CPSS_DXCH_MAPPING_ETH_MIN_TM_PORT_IDX_CNS   0    /* range 0 .. 71 */
#define PRV_CPSS_DXCH_MAPPING_ETH_MAX_TM_PORT_IDX_CNS   71
#define PRV_CPSS_DXCH_MAPPING_ILKN_MIN_TM_PORT_IDX_CNS  128  /* range 128 .. 191 */
#define PRV_CPSS_DXCH_MAPPING_ILKN_MAX_TM_PORT_IDX_CNS  191

#define PRV_CPSS_DXCH_MAPPING_BOBCAT2_ILKN_CHANNEL_MAX_PHYS_PORT_DIFF_CNS  64

/* TM port index range is 0..71 and 128..191*/
#define PRV_CPSS_DXCH_MAPPING_BOBCAT2_TM_PORT_NUM_LOW_RANGE_MAX_CNS  \
            (PRV_CPSS_DXCH_MAPPING_ETH_MAX_TM_PORT_IDX_CNS + 1)
#define PRV_CPSS_DXCH_MAPPING_BOBCAT2_TM_PORT_NUM_HIGH_RANGE_START_CNS  PRV_CPSS_DXCH_MAPPING_ILKN_MIN_TM_PORT_IDX_CNS
#define PRV_CPSS_DXCH_MAPPING_BOBCAT2_TM_PORT_NUM_HIGH_RANGE_MAX_CNS \
            (PRV_CPSS_DXCH_MAPPING_ILKN_MAX_TM_PORT_IDX_CNS + 1)

/*--------------------------------------------------------------*/
/* 1. RxDMA 2 Local Physical Port                               */
/*--------------------------------------------------------------*/
/* Allowed range 0:72 */
#define PRV_CPSS_DXCH_RXDMA_2_PHYS_MAX_RxDMA_PORT_CNS  72
#define PRV_CPSS_DXCH_RXDMA_2_PHYS_MAX_PHYS_PORT_CNS(devNum)   MAX(255,(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.maxPhysicalPorts-1))

#define PRV_CPSS_DXCH_RXDMA_2_PHYS_OFFS_CNS 0
#define PRV_CPSS_DXCH_RXDMA_2_PHYS_LEN_CNS(devNum)  MAX(8,PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).phyPort)


/*******************************************************************************
* prvCpssDxChPortMappingRxDMA2LocalPhysSet
*
* DESCRIPTION:
*       configure RxDMA 2 phys port mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       rxDmaNum   - rxDMA port
*       physPort    - physical port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingRxDMA2LocalPhysSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 rxDmaNum,
    IN GT_U32 physPort
)
{
    GT_STATUS rc = GT_OK;
    GT_U32      regAddr;            /* register's address */
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC * pReg;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    if (rxDmaNum > PRV_CPSS_DXCH_RXDMA_2_PHYS_MAX_RxDMA_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (physPort > PRV_CPSS_DXCH_RXDMA_2_PHYS_MAX_PHYS_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    pReg = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    regAddr = pReg->rxDMA[0].singleChannelDMAConfigs.SCDMAConfig1[rxDmaNum];
    rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                            PRV_CPSS_DXCH_RXDMA_2_PHYS_OFFS_CNS,
                                            PRV_CPSS_DXCH_RXDMA_2_PHYS_LEN_CNS(devNum),
                                            physPort);

    if (GT_OK != rc )
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingRxDMA2LocalPhysGet
*
* DESCRIPTION:
*       RxDMA 2 phys port mapping  get
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       rxDmaNum   - rxDMA port
*
* OUTPUTS:
*       physPortPtr - (pointer to) physical port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingRxDMA2LocalPhysGet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 rxDmaNum,
   OUT GT_U32 *physPortPtr
)
{
    GT_STATUS rc;
    GT_U32      regAddr;            /* register's address */
    PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC * pReg;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(physPortPtr);

    *physPortPtr = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;

    if (rxDmaNum > PRV_CPSS_DXCH_RXDMA_2_PHYS_MAX_RxDMA_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* it is enough to get data from single pipe mapping, since them bothe are configured same */
    pReg = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum);

    regAddr = pReg->rxDMA[0].singleChannelDMAConfigs.SCDMAConfig1[rxDmaNum];
    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            PRV_CPSS_DXCH_RXDMA_2_PHYS_OFFS_CNS,
                                            PRV_CPSS_DXCH_RXDMA_2_PHYS_LEN_CNS(devNum),
                                            physPortPtr);

    if (GT_OK != rc )
    {
        return rc;
    }

    return GT_OK;
}


/*-----------------------------------------------------------------------------*/
/* 2. Cut Through TX DMA map of Local Device Physical Port to RX DMA number    */
/*-----------------------------------------------------------------------------*/
/* allowed port 0-63 */
#define PRV_CPSS_DXCH_CutTrough_TXDMA_2_PHYS_MAX_PHYS_PORT_CNS 63
#define PRV_CPSS_DXCH_CutTrough_TXDMA_2_PHYS_MAX_TXDMA_PORT_CNS 255


/*-----------------------------------------------------------------------------*/
/* 3. BMA map of Local Device Physical Port to RX DMA number                   */
/*-----------------------------------------------------------------------------*/
#define PRV_CPSS_DXCH_BMA_PHYS_2_RXDMA_MAX_PHYS_PORT_CNS(devNum) MAX(255,(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.maxPhysicalPorts-1))
#define PRV_CPSS_DXCH_BMA_PHYS_2_RXDMA_MAX_RXDMA_PORT_CNS 255

/*******************************************************************************
* prvCpssDxChPortMappingBMAMapOfLocalPhys2RxDMASet
*
* DESCRIPTION:
*       configure BMA local physical port 2 RxDMA port  mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - physical port
*       rxDmaNum   - rxDMA port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMappingBMAMapOfLocalPhys2RxDMASet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 physPort,
    IN GT_U32 rxDMAPort
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    dataPath;        /* data path number */
    GT_U32    dataPathIterNum; /* data path iterations number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(portGroupId);

    if (physPort > PRV_CPSS_DXCH_BMA_PHYS_2_RXDMA_MAX_PHYS_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* need to write the 'local' DMA number */
    rc = prvCpssDxChHwPpDmaGlobalNumToLocalNumInDpConvert(devNum,
        rxDMAPort/*global number*/,
        NULL,
        &rxDMAPort);/*converted to local number*/
    if (GT_OK != rc )
    {
        return rc;
    }

    if (rxDMAPort > PRV_CPSS_DXCH_BMA_PHYS_2_RXDMA_MAX_RXDMA_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E) ||
        (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportMultiDataPath == 0))
    {
        /* Bobcat3 is multiDataPath device but it use only one instance of the BMA map table */
        dataPathIterNum = 1;
    }
    else
    {
        dataPathIterNum = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.maxDp;
    }


    /* need to configure all instances of BMA map for multi-datapath devices like Caelum.
       CPSS use one logical table for all instances. Each instance has 256 entires.*/
    for (dataPath = 0; dataPath < dataPathIterNum; dataPath++)
    {
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_BMA_PORT_MAPPING_E,
                                        physPort + (256 * dataPath),
                                        &rxDMAPort);
        if (GT_OK != rc )
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingBMAMapOfLocalPhys2RxDMAGet
*
* DESCRIPTION:
*       get mapping of BMA local physical port 2 RxDMA port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - physical port
*
* OUTPUTS:
*       rxDmaPortPtr   - rxDMA port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMappingBMAMapOfLocalPhys2RxDMAGet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 physPort,
   OUT GT_U32 *rxDmaPortPtr
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(portGroupId);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (physPort > PRV_CPSS_DXCH_BMA_PHYS_2_RXDMA_MAX_PHYS_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(rxDmaPortPtr);

    *rxDmaPortPtr = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;

    rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_BMA_PORT_MAPPING_E,
                                    physPort,
                                    rxDmaPortPtr);
    if (GT_OK != rc )
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.supportRelativePortNum)
    {
        /* the value in (*rxDmaPortPtr) is 'local DMA' number , and we don't know the 'DP_index'
           so we need to use the DB.
        */
        CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
        GT_U32  hwLocalRxDmaPort = *rxDmaPortPtr;
        GT_U32  convertedLocalRxDmaPort;

        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, physPort, /*OUT*/&portMapShadowPtr);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(portMapShadowPtr->valid)
        {
            /* check that the info match the HW value */
            /* need to write the 'local' DMA number */
            rc = prvCpssDxChHwPpDmaGlobalNumToLocalNumInDpConvert(devNum,
                portMapShadowPtr->portMap.rxDmaNum/*global number*/,
                NULL,
                &convertedLocalRxDmaPort);/*converted to local number*/
            if (GT_OK != rc )
            {
                return rc;
            }

            if(convertedLocalRxDmaPort != hwLocalRxDmaPort)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "DB management error : the DB hold global rxDMA [%d] "
                    "that considered local rxDMA[%d] that not match the "
                    "value [%d] read from the HW for physical port[%d] \n",
                    portMapShadowPtr->portMap.rxDmaNum,
                    convertedLocalRxDmaPort,
                    hwLocalRxDmaPort,
                    physPort
                    );
            }

            *rxDmaPortPtr = portMapShadowPtr->portMap.rxDmaNum;
        }
    }



    return GT_OK;
}


/*-----------------------------------------------------------------------------*/
/* 4.1 EGF Target  Local Device Physical Port to TxQ port Map                  */
/*-----------------------------------------------------------------------------*/
#define PRV_CPSS_DXCH_EGF_PHYS_2_TXQ_MAX_PHYS_PORT_CNS(dev) \
        (GT_U32)(PRV_CPSS_SIP_5_20_CHECK_MAC(dev) ?         \
             512:                                    \
             255)
#define PRV_CPSS_DXCH_EGF_PHYS_2_TXQ_MAX_TXQ_PORT_CNS(dev)  \
        (PRV_CPSS_SIP_5_20_CHECK_MAC(dev) ?         \
            SIP_5_20_TXQ_DQ_MAX_PORTS_MAC(dev):              \
             255)

/*******************************************************************************
* prvCpssDxChPortMappingEGFTargetLocalPhys2TxQPortMapSet
*
* DESCRIPTION:
*       configure EGF  target local physical port 2 TxQ port  mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - physical port
*       txqNum     - txq port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMappingEGFTargetLocalPhys2TxQPortMapSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 physPort,
    IN GT_U32 txqNum
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(portGroupId);

    if (CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS == txqNum)
    {
        return GT_OK;
    }

    if (physPort > PRV_CPSS_DXCH_EGF_PHYS_2_TXQ_MAX_PHYS_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (txqNum > PRV_CPSS_DXCH_EGF_PHYS_2_TXQ_MAX_TXQ_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_TARGET_PORT_MAPPER_E,
                                    physPort,
                                    &txqNum);
    if (GT_OK != rc )
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingEGFTargetLocalPhys2TxQPortMapGet
*
* DESCRIPTION:
*       get mapping of EGF target local physical port 2 TxQ port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - physical port
*
* OUTPUTS:
*       txqNumPtr  - txq port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingEGFTargetLocalPhys2TxQPortMapGet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 physPort,
   OUT GT_U32 *txqNumPtr
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(portGroupId);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    if (physPort > PRV_CPSS_DXCH_EGF_PHYS_2_TXQ_MAX_PHYS_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(txqNumPtr);

    rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_TARGET_PORT_MAPPER_E,
                                    physPort,
                                    txqNumPtr);
    if (GT_OK != rc )
    {
        return rc;
    }

    return GT_OK;
}


/*-----------------------------------------------------------------------------*/
/* 4.2 EGF Target  Local Device Physical Port to TxQ port Map                   */
/*-----------------------------------------------------------------------------*/
#define PRV_CPSS_DXCH_EGF_PHYS_2_MAC_MAX_PHYS_PORT_CNS 255
#define PRV_CPSS_DXCH_EGF_PHYS_2_MAC_MAX_MAC_PORT_CNS  255

/*******************************************************************************
* prvCpssDxChPortMappingEGFTargetLocalPhys2MACNumMapSet
*
* DESCRIPTION:
*       configure EGF  target local physical port 2 Mac number  mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - physical port
*       macNum      - mac number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingEGFTargetLocalPhys2MACNumMapSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 physPort,
    IN GT_U32 macNum
)
{
    GT_STATUS rc = GT_OK;
    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(portGroupId);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (physPort > PRV_CPSS_DXCH_EGF_PHYS_2_MAC_MAX_PHYS_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (macNum > PRV_CPSS_DXCH_EGF_PHYS_2_MAC_MAX_MAC_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChHwEgfEftFieldSet(devNum,physPort/*virtual port (target port)*/,
        PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_VIRTUAL_2_PHYSICAL_PORT_REMAP_E,
        macNum/*global physical port*/);

    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingEGFTargetLocalPhys2MACNumMapGet
*
* DESCRIPTION:
*       get mapping of  EGF  target local physical port 2 Mac number
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - physical port
*
* OUTPUTS:
*       macNumPtr   - mac number
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingEGFTargetLocalPhys2MACNumMapGet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 physPort,
   OUT GT_U32 *macNumPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(portGroupId);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (physPort > PRV_CPSS_DXCH_EGF_PHYS_2_MAC_MAX_PHYS_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(macNumPtr);


    rc = prvCpssDxChHwEgfEftFieldGet(devNum,physPort/*virtual port (target port)*/,
        PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_VIRTUAL_2_PHYSICAL_PORT_REMAP_E,
        macNumPtr/*global physical port*/);
    if (GT_OK != rc)
        {
            return rc;
    }
        return GT_OK;
}


/*-----------------------------------------------------------------------------*/
/* 5. TxQ port to TX DMA map                                                   */
/*-----------------------------------------------------------------------------*/
#define PRV_CPSS_MAPPING_TXQ_2_TXDMA_MAX_TXQ_PORT_CNS(dev)    \
        (PRV_CPSS_SIP_5_20_CHECK_MAC(dev) ?                 \
            SIP_5_20_TXQ_DQ_MAX_PORTS_MAC(dev):              \
             71)
#define PRV_CPSS_MAPPING_TXQ_2_TXDMA_MAX_TXDMA_PORT_CNS  255

#define PRV_CPSS_MAPPING_TXQ_2_TXDMA_OFFS_CNS          0
#define PRV_CPSS_MAPPING_TXQ_2_TXDMA_LEN_CNS           8

/*******************************************************************************
* prvCpssDxChPortMappingTxQPort2TxDMAMapSet
*
* DESCRIPTION:
*       configure TxQ port 2 TxDMA  mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       txqNum     - txq port
*       txDmaNum   - txDMA port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMappingTxQPort2TxDMAMapSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 txqNum,
    IN GT_U32 txDmaNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32      regAddr;            /* register's address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    if(CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS == txqNum)
    {
        return GT_OK;
    }

    if (txqNum > PRV_CPSS_MAPPING_TXQ_2_TXDMA_MAX_TXQ_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        /* need to write the 'local' DMA number */
        rc = prvCpssDxChHwPpDmaGlobalNumToLocalNumInDpConvert(devNum,
            txDmaNum/*global number*/,
            NULL,
            &txDmaNum);/*converted to local number*/
        if (GT_OK != rc )
        {
            return rc;
        }
    }

    if(txDmaNum > PRV_CPSS_MAPPING_TXQ_2_TXDMA_MAX_TXDMA_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).global.globalDQConfig.portToDMAMapTable[txqNum];

    rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                         PRV_CPSS_MAPPING_TXQ_2_TXDMA_OFFS_CNS,
                                         PRV_CPSS_MAPPING_TXQ_2_TXDMA_LEN_CNS,
                                         txDmaNum);
    if (GT_OK != rc )
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingTxQPort2TxDMAMapGet
*
* DESCRIPTION:
*       get mapping of TxQ port 2 TxDMA
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       txqNum     - txq port
*
* OUTPUTS:
*       txDmaNumPtr - txDMA port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingTxQPort2TxDMAMapGet
(
    IN GT_U8   devNum,
    IN GT_U32  portGroupId,
    IN GT_U32  txqNum,
   OUT GT_U32 *txDmaNumPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32      regAddr;            /* register's address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    if (txqNum > PRV_CPSS_MAPPING_TXQ_2_TXDMA_MAX_TXQ_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).global.globalDQConfig.portToDMAMapTable[txqNum];

    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            PRV_CPSS_MAPPING_TXQ_2_TXDMA_OFFS_CNS,
                                            PRV_CPSS_MAPPING_TXQ_2_TXDMA_LEN_CNS,
                                            txDmaNumPtr);
    if (GT_OK != rc )
    {
        return rc;
    }


    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        /* need to convert the 'local' DMA number to global DMA number */

        /* convert global to dq unit index */
        rc = prvCpssDxChHwPpDmaLocalNumInDpToGlobalNumConvert(devNum,
            SIP_5_20_GLOBAL_TXQ_DQ_PORT_TO_DQ_UNIT_INDEX_MAC(devNum,txqNum),
            *txDmaNumPtr,/* local DMA */
            txDmaNumPtr);/* converted global DMA */

        if (GT_OK != rc )
        {
            return rc;
        }
    }

    return GT_OK;
}


/*-----------------------------------------------------------------------------*/
/* 6 PFC Response Local Device Source Port to TxQ port map                     */
/*-----------------------------------------------------------------------------*/
#define PRV_CPSS_MAPPING_PHYS_2_TXQ_MAX_PHYS_PORT_CNS    255
#define PRV_CPSS_MAPPING_PHYS_2_TXQ_MAX_TXQ_PORT_CNS     255


/*
#define PRV_CPSS_MAPPING_TXQ_2_TXDMA_OFFS_CNS          0
#define PRV_CPSS_MAPPING_TXQ_2_TXDMA_LEN_CNS           8
*/

/*******************************************************************************
* prvCpssDxChPortMappingPFCResponsePhysPort2TxQMapSet
*
* DESCRIPTION:
*       configure PFC response Phys port mapping to txq
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - phys port
*       txqNum      - txq port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingPFCResponsePhysPort2TxQMapSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 physPort,
    IN GT_U32 txqNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32      regAddr;            /* register's address */
    GT_U32      dqIndex = 0;/*dq unit index*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    { CPSS_TBD_BOOKMARK_BOBCAT3
        CPSS_LOG_INFORMATION_MAC("the mapping <TxQ Dequeue/Flow Control Response/TTI Port Mapping Table> is not supported yet \n");
        return GT_OK;
    }


    if (CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS == txqNum)
    {
        return GT_OK;
    }

    if (physPort > PRV_CPSS_MAPPING_PHYS_2_TXQ_MAX_PHYS_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        if (txqNum > PRV_CPSS_DXCH_MAPPING_MAX_TXQ_PORT_CNS(devNum))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* convert global to dq unit index */
        dqIndex = SIP_5_20_GLOBAL_TXQ_DQ_PORT_TO_DQ_UNIT_INDEX_MAC(devNum,txqNum);
        /* convert global to local */
        txqNum = SIP_5_20_GLOBAL_TXQ_DQ_PORT_TO_LOCAL_TXQ_DQ_PORT_MAC(devNum,txqNum);

        /* save the mapping */
        SIP_5_20_TXQ_DQ_UNIT_INDEX_OF_PORT_MAC(devNum,physPort) = dqIndex;


    }
    else
    if (txqNum > PRV_CPSS_MAPPING_PHYS_2_TXQ_MAX_TXQ_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_INDEX_MAC(devNum,dqIndex).flowCtrlResponse.portToTxQPortMap[physPort];

    rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                            PRV_CPSS_MAPPING_TXQ_2_TXDMA_OFFS_CNS,
                                            PRV_CPSS_MAPPING_TXQ_2_TXDMA_LEN_CNS,
                                            txqNum);
    if (GT_OK != rc )
    {
        return rc;
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingPFCResponsePhysPort2TxQMapGet
*
* DESCRIPTION:
*       configure PFC response Phys port mapping to txq
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - phys port
*
* OUTPUTS:
*       txqNumPtr   - txq port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingPFCResponsePhysPort2TxQMapGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  GT_U32 physPort,
    OUT GT_U32 *txqNumPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    regAddr;            /* register's address */
    GT_U32      dqIndex = 0;/*dq unit index*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (physPort > PRV_CPSS_MAPPING_PHYS_2_TXQ_MAX_PHYS_PORT_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(txqNumPtr);

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        dqIndex = SIP_5_20_TXQ_DQ_UNIT_INDEX_OF_PORT_MAC(devNum,physPort);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_INDEX_MAC(devNum,dqIndex).flowCtrlResponse.portToTxQPortMap[physPort];

    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            PRV_CPSS_MAPPING_TXQ_2_TXDMA_OFFS_CNS, PRV_CPSS_MAPPING_TXQ_2_TXDMA_LEN_CNS, txqNumPtr);
    if (GT_OK != rc )
    {
        return rc;
    }

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        /* convert local to global */
        (*txqNumPtr) = SIP_5_20_LOCAL_TXQ_DQ_PORT_TO_GLOBAL_TXQ_DQ_PORT_MAC(devNum,(*txqNumPtr),dqIndex);
    }


    return GT_OK;
}


/*-----------------------------------------------------------------------------*/
/* 7 PFC Trigger Local Device Source Port to MAC map                           */
/*-----------------------------------------------------------------------------*/
#define PRV_CPSS_PFC_COUNTER_IDX_OFFS_CNS    0
#define PRV_CPSS_PFC_COUNTER_IDX_LEN_CNS     4

#define PRV_CPSS_PFC_COUNTER_BLK_OFFS_CNS    4
#define PRV_CPSS_PFC_COUNTER_BLK_LEN_CNS     3

#define PRV_CPSS_PFC_LOCAL_CORE_IDX_OFFS_CNS 7
#define PRV_CPSS_PFC_LOCAL_CORE_IDX_LEN_CNS  3

#define PRV_CPSS_PFC_MAC_IDX_OFFS_CNS        10
#define PRV_CPSS_PFC_MAC_IDX_LEN_CNS         8

/*******************************************************************************
* prvCpssDxChPortMappingPFCTriggerPhysPort2MACMapSet
*
* DESCRIPTION:
*       configure PFC trigger Phys port mapping to mac number
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - phys port
*       macNum      - mac port
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingPFCTriggerPhysPort2MACMapSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 physPort,
    IN GT_U32 macNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32      regAddr;            /* register's address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->TXQ.pfc.PFCSourcePortToPFCIndexMap[physPort];

    rc = prvCpssHwPpPortGroupSetRegField(devNum, 0, regAddr,
                                             PRV_CPSS_PFC_LOCAL_CORE_IDX_OFFS_CNS, PRV_CPSS_PFC_LOCAL_CORE_IDX_LEN_CNS, portGroupId);
    if (GT_OK != rc )
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->TXQ.pfc.PFCSourcePortToPFCIndexMap[physPort];

    rc = prvCpssHwPpPortGroupSetRegField(devNum, 0, regAddr,
                                             PRV_CPSS_PFC_MAC_IDX_OFFS_CNS, PRV_CPSS_PFC_MAC_IDX_LEN_CNS, macNum);
    if (GT_OK != rc )
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingPFCTriggerPhysPort2MACMapGet
*
* DESCRIPTION:
*       configure PFC trigger Phys port mapping to mac number
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       physPort    - phys port
*
* OUTPUTS:
*       macNumPtr   - mac port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingPFCTriggerPhysPort2MACMapGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  GT_U32 physPort,
    OUT GT_U32 *macNumPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(macNumPtr);

    *macNumPtr = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->TXQ.pfc.PFCSourcePortToPFCIndexMap[physPort];

    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                             PRV_CPSS_PFC_MAC_IDX_OFFS_CNS, PRV_CPSS_PFC_MAC_IDX_LEN_CNS, macNumPtr);
    if (GT_OK != rc )
    {
        return rc;
    }



    return GT_OK;
}

/*-----------------------------------------------------------------------------*/
/* 9 CPU TxQ port number                                                       */
/*-----------------------------------------------------------------------------*/
#define PRV_CPSS_MAPPING_CPU_PORT_EN_OFFS_CNS  0
#define PRV_CPSS_MAPPING_CPU_PORT_EN_LEN_CNS   1
#define PRV_CPSS_MAPPING_CPU_TXQ_PORT_OFFS_CNS 1
#define PRV_CPSS_MAPPING_CPU_TXQ_PORT_LEN_CNS  7

/*******************************************************************************
* prvCpssDxChPortMappingCPU_2_TxQPortSet
*
* DESCRIPTION:
*       configure CPU txq number
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       cpuNum     - cpu port
*
* OUTPUTS:
*       txqNumPtr  - txq number
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingCPU_2_TxQPortSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 cpuNum,
    IN GT_U32 txqNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(cpuNum);

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).global.globalDQConfig.globalDequeueConfig;

    rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                             PRV_CPSS_MAPPING_CPU_TXQ_PORT_OFFS_CNS,
                                             PRV_CPSS_MAPPING_CPU_TXQ_PORT_LEN_CNS,
                                             txqNum);
    if (GT_OK != rc )
    {
        return rc;
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingCPU_2_TxQPortGet
*
* DESCRIPTION:
*       configure CPU txq number
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       cpuNum     - cpu port
*
* OUTPUTS:
*       txqNumPtr  - txq number
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMappingCPU_2_TxQPortGet
(
    IN GT_U8   devNum,
    IN GT_U32  portGroupId,
    IN GT_U32  cpuNum,
   OUT GT_U32 *txqNumPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_UNUSED_PARAM_MAC(cpuNum);

    CPSS_NULL_PTR_CHECK_MAC(txqNumPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).global.globalDQConfig.globalDequeueConfig;
    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                             PRV_CPSS_MAPPING_CPU_TXQ_PORT_OFFS_CNS,
                                             PRV_CPSS_MAPPING_CPU_TXQ_PORT_LEN_CNS,
                                             txqNumPtr);
    if (GT_OK != rc )
    {
        return rc;
    }


    return GT_OK;
}


/*--------------------------------------------------------------*/
/* Mapping Declaration                                          */
/*--------------------------------------------------------------*/
static PRV_DXCH_CPSS_MAPPING_SET_GET_FUN_STC prv_MappingFunList[] =
{
    /* 1  RX DMA to Local Device Physical Source Port */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  "1 RxDMA2-rxDma2Phys",
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ PRV_MAPTYPE_ETHERNET_MAC_E | PRV_MAPTYPE_CPU_SDMA_E | PRV_MAPTYPE_ILKN_MAC_E,
        /*  GT_U32                             applicableDevice; */ CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ prvCpssDxChPortMappingRxDMA2LocalPhysSet,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ prvCpssDxChPortMappingRxDMA2LocalPhysGet
    },
    /* 3 BMA map of Local Device Physical Port to RX DMA number */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  "3 BMA-phys2rxDma",
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ PRV_MAPTYPE_ETHERNET_MAC_E  | PRV_MAPTYPE_CPU_SDMA_E | PRV_MAPTYPE_ILKN_MAC_E | PRV_MAPTYPE_ILKN_CHANNEL_E | PRV_MAPTYPE_REMOTE_PHYS_PORT_E,
        /*  GT_U32                             applicableDevice; */ CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ prvCpssDxChPortMappingBMAMapOfLocalPhys2RxDMASet,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ prvCpssDxChPortMappingBMAMapOfLocalPhys2RxDMAGet
    },
    /* 4.1 EGF Target Local Physical Port to TxQ port map */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  "4.1 EGF-phys2txq",
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ PRV_MAPTYPE_ETHERNET_MAC_E  | PRV_MAPTYPE_CPU_SDMA_E | PRV_MAPTYPE_ILKN_MAC_E | PRV_MAPTYPE_ILKN_CHANNEL_E | PRV_MAPTYPE_REMOTE_PHYS_PORT_E,
        /*  GT_U32                             applicableDevice; */ CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_TXQ_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ prvCpssDxChPortMappingEGFTargetLocalPhys2TxQPortMapSet,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ prvCpssDxChPortMappingEGFTargetLocalPhys2TxQPortMapGet
    },
    /* 4.2 EGF Target Local Physical Port to MAC port map */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  "4.2 EGF-phys2mac",
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ PRV_MAPTYPE_ETHERNET_MAC_E  | PRV_MAPTYPE_CPU_SDMA_E | PRV_MAPTYPE_ILKN_MAC_E | PRV_MAPTYPE_ILKN_CHANNEL_E | PRV_MAPTYPE_REMOTE_PHYS_PORT_E,
        /*  GT_U32                             applicableDevice; */ CPSS_BOBCAT2_E | CPSS_CAELUM_E /*| CPSS_BOBCAT3_E removed from sip_5_20*/,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_MAC_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ prvCpssDxChPortMappingEGFTargetLocalPhys2MACNumMapSet,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ prvCpssDxChPortMappingEGFTargetLocalPhys2MACNumMapGet
    },
    /* 5 TxQ port to TX DMA map */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  "5 TXQ-rxq2txDma",
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ PRV_MAPTYPE_ETHERNET_MAC_E  | PRV_MAPTYPE_CPU_SDMA_E | PRV_MAPTYPE_ILKN_MAC_E | PRV_MAPTYPE_ILKN_CHANNEL_E | PRV_MAPTYPE_REMOTE_PHYS_PORT_E,
        /*  GT_U32                             applicableDevice; */ CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_TXQ_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ prvCpssDxChPortMappingTxQPort2TxDMAMapSet,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ prvCpssDxChPortMappingTxQPort2TxDMAMapGet
    },

    /* 6 PFC Response Local Device Source Port to TxQ port map */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  "6 PFCResp-phys22txq",
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ PRV_MAPTYPE_ETHERNET_MAC_E  | PRV_MAPTYPE_CPU_SDMA_E | PRV_MAPTYPE_ILKN_MAC_E | PRV_MAPTYPE_ILKN_CHANNEL_E | PRV_MAPTYPE_REMOTE_PHYS_PORT_E,
        /*  GT_U32                             applicableDevice; */ CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_TXQ_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ prvCpssDxChPortMappingPFCResponsePhysPort2TxQMapSet,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ prvCpssDxChPortMappingPFCResponsePhysPort2TxQMapGet
    },

    /* 7 PFC Trigger Local Device Source Port to MAC map */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  "7 PFCTrig-phys2mac",
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ PRV_MAPTYPE_ETHERNET_MAC_E  |  PRV_MAPTYPE_ILKN_MAC_E | PRV_MAPTYPE_ILKN_CHANNEL_E | PRV_MAPTYPE_REMOTE_PHYS_PORT_E,
        /*  GT_U32                             applicableDevice; */ CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_MAC_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ prvCpssDxChPortMappingPFCTriggerPhysPort2MACMapSet,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ prvCpssDxChPortMappingPFCTriggerPhysPort2MACMapGet
    },
    /* 9 CPU TxQ port number  */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  "9 CPU-txq",
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ PRV_MAPTYPE_CPU_SDMA_E,
        /*  GT_U32                             applicableDevice; */ CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_PHYS_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_TXQ_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ prvCpssDxChPortMappingCPU_2_TxQPortSet,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ prvCpssDxChPortMappingCPU_2_TxQPortGet
    },
    /* Last Mapping : Empty Mapping   */
    {
        #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        /* GT_CHAR                            stringMapType[20];*/  NULL,
        #endif
        /*  GT_U32                             mapingTypeBmp;    */ 0,
        /*  GT_U32                             applicableDevice; */ 0,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        srcPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_INVALID_E,
        /*  PRV_CPSS_DXCH_PORT_TYPE_ENT        dstPortType;      */ PRV_CPSS_DXCH_PORT_TYPE_INVALID_E,
        /*  PRV_DXCH_CPSS_MAPPING_SET_FUN_PTR  setFun;           */ NULL,
        /*  PRV_DXCH_CPSS_MAPPING_GET_FUN_PTR  getFun;           */ NULL
    }
};


/*******************************************************************************
* prvCpssDxChPortMappingTypeCheck
*
* DESCRIPTION:
*       basic mapping check for HW feature
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum            - device
*       portMapPtr        - pointer to mapping
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on bad pointer
*
* COMMENTS:
*
********************************************************************************/
static GT_STATUS prvCpssDxChPortMappingTypeCheck
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_PORT_MAP_STC      *portMapPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(portMapPtr);
    if (portMapPtr->mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E && PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_ilkn.supported == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /*
    if (portMapPtr->tmEnable == GT_TRUE && PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.TmSupported == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }
    */
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingTypeGet
*
* DESCRIPTION:
*       build  mapping type in internal format (bit coded)
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       systemMappingType - system mapping type
*
* OUTPUTS:
*       prvMappingTypePtr - internal mapping type
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on bad pointer
*
* COMMENTS:
*
********************************************************************************/
static GT_STATUS prvCpssDxChPortMappingTypeGet
(
    IN  CPSS_DXCH_PORT_MAPPING_TYPE_ENT  systemMappingType,
    OUT PRV_MAPTYPE_ENT                 *prvMappingTypePtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(prvMappingTypePtr);

    switch (systemMappingType)
    {
        case CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E:
            *prvMappingTypePtr = PRV_MAPTYPE_ETHERNET_MAC_E;
        break;
        case CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E:
            *prvMappingTypePtr = PRV_MAPTYPE_CPU_SDMA_E    ;
        break;
        case CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E:
            *prvMappingTypePtr = PRV_MAPTYPE_ILKN_CHANNEL_E;
        break;
        case CPSS_DXCH_PORT_MAPPING_TYPE_REMOTE_PHYSICAL_PORT_E:
            *prvMappingTypePtr = PRV_MAPTYPE_REMOTE_PHYS_PORT_E;
        break;
        default:
        {
            *prvMappingTypePtr = PRV_MAPTYPE_INVALID_E;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpsssDxChPortMappingDeviceTypeGet
*
* DESCRIPTION:
*       build  device type in bit coded style
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devFamily       - device family
*
* OUTPUTS:
*       devTypePtr      device type in bit coded style
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on bad pointer
*
* COMMENTS:
*
********************************************************************************/
static GT_STATUS prvCpsssDxChPortMappingDeviceTypeGet
(
    IN  CPSS_PP_FAMILY_TYPE_ENT devFamily,
    IN  CPSS_PP_SUB_FAMILY_TYPE_ENT devSubFamily,
    OUT CPSS_PP_FAMILY_BIT_ENT *devTypePtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(devTypePtr);

    switch (devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            if(devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
            {
                *devTypePtr = CPSS_CAELUM_E;
            }
            else
            {
                *devTypePtr = CPSS_BOBCAT2_E;
            }
        break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
            *devTypePtr = CPSS_BOBCAT3_E;
        break;
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
            *devTypePtr = CPSS_CAELUM_E;
        break;
        default:
        {
            *devTypePtr = CPSS_UNDEFINED_DEVICE_E;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;
}

/*-------------------------------------*/
/* check function */
/*-------------------------------------*/
/*******************************************************************************
* prvCpssDxChCheckDMAPort
*
* DESCRIPTION:
*       check DMA Port range
*
* APPLICABLE DEVICES:
*        Bobcat2
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum       - device family
*       dmaNum       - dma num
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on error in params
*
* COMMENTS:
*
********************************************************************************/
static GT_STATUS prvCpssDxChCheckDMAPort
(
    IN GT_U8  devNum,
    IN GT_U32 dmaNum
)
{

    /* the 'Regular DMA' that is not CPU/ILKN is actually 'MAC number' that
       exists in the 'Existing ports' (in valid range of MACs) */
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,dmaNum);

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChCheckTxQPort
*
* DESCRIPTION:
*       check txq Port range
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum     - device family
*       txqNum     - txq num
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on error in params
*
* COMMENTS:
*
********************************************************************************/
static GT_STATUS prvCpssDxChCheckTxQPort
(
    IN GT_U8  devNum,
    IN GT_U32 txqNum
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if(txqNum == CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS)
    {
        return GT_OK;
    }

    if (txqNum > PRV_CPSS_DXCH_MAPPING_MAX_TXQ_PORT_CNS(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCheckIlknChannelPort
*
* DESCRIPTION:
*       check Interlaken channel Port range
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       ilknChannel  - Interlaken channel num
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on error in params
*
* COMMENTS:
*
********************************************************************************/
static GT_STATUS prvCpssDxChCheckIlknChannelPort
(
    IN GT_U32 ilknChannel
)
{
    if (ilknChannel >=
            PRV_CPSS_DXCH_MAPPING_BOBCAT2_ILKN_CHANNEL_MAX_PHYS_PORT_DIFF_CNS )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*-----------------------------------------------------*/
#define PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS         512

#define PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS ((GT_U32)(~0))

/******************************************************************
 * Typedef: struct PRV_CPSS_DXCH_PORT_MAPPING_STC
 *
 * Description:  general port map structure used for many-2-one check
 *
 * Fields:
 *      devNum              - device number
 *      rxDma2PhysMapArr    - rxDma 2 physical port number mapping
 *      phys2rxDmaMapArr    - physical port number 2 rxDma mapping
 *      phys2txDmaMapArr    - physical port number 2 txDma mapping
 *      phys2macMapArr      - physical port number 2 mac   mapping
 *      phys2txqMapArr      - physical port number 2 txq   mapping
 *      txq2txDmaMapArr     - txq port 2 tx Dma mapping
 *      tm2physMapArr       - tm 2 phys (actually eth-Tx-fifo)
 *      Ilkn2physMapArr     - ilkn channel 2 phys
 *      minIlknChnnalPhysPort - min interlaken physical port
 *      maxIlknChnnalPhysPort - max interlaken physical port, the difference shall be less than 64
 *
 * Comments:
 *
 *******************************************************************/
typedef struct
{
    GT_U8     devNum;
    GT_BOOL   isPhysPortUsed         [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_BOOL   physIsEqualt2OldMapping[PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    /* HW mappings */
    GT_U32    rxDma2PhysMapArr       [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_U32    phys2rxDmaMapArr       [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_U32    phys2txDmaMapArr       [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_U32    phys2macMapArr         [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_U32    phys2txqMapArr         [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_U32    txq2txDmaMapArr        [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_U32    tm2physMapArr          [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_U32    Ilkn2physMapArr        [PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS];
    GT_U32    minIlknChnnalPhysPort;
    GT_U32    maxIlknChnnalPhysPort;
}PRV_CPSS_DXCH_PORT_MAPPING_STC;


/*******************************************************************************
* prvCpsssDxChPortMappingArgumentsBuild
*
* DESCRIPTION:
*       build  argumenst for mappings (from user format into internal format) based on
*       mapping flag.
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum           - device number
*       portMapPtr       - pointer to array of mappings
*
* OUTPUTS:
*       portArg          - argumenst array
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on bad param
*       GT_BAD_PTR            - on bad pointer
*
* COMMENTS:
*
*       takes into account specific settings like
*
*      1. MAC number is equal to RxDMA port in all cases.
*      2. RxDMA port = TxDMA port in all cases exept for TM is enabled
*         TxDMA port (73) for traffic manager if TM flag is true
*      3. for CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E :     Rx/Tx DMA port is set to 72
*      4. for CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E : Rx/Tx DMA port is set to 64
*
********************************************************************************/
static GT_STATUS prvCpsssDxChPortMappingArgumentsBuild
(
     IN GT_U8                    devNum,
     IN CPSS_DXCH_PORT_MAP_STC  *portMapPtr,
     OUT GT_U32  portArg[PRV_CPSS_DXCH_PORT_TYPE_MAX_E]
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portMapPtr->physicalPortNumber);
    CPSS_NULL_PTR_CHECK_MAC(portMapPtr);
    CPSS_NULL_PTR_CHECK_MAC(portArg);

    if (GT_TRUE == portMapPtr->tmEnable)
    {
        if(CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E == portMapPtr->mappingType)
        {
            if((portMapPtr->tmPortInd < PRV_CPSS_DXCH_MAPPING_BOBCAT2_TM_PORT_NUM_HIGH_RANGE_START_CNS)
               || (portMapPtr->tmPortInd > PRV_CPSS_DXCH_MAPPING_BOBCAT2_TM_PORT_NUM_HIGH_RANGE_MAX_CNS))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            if (portMapPtr->tmPortInd > PRV_CPSS_DXCH_MAPPING_ETH_MAX_TM_PORT_IDX_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }

    rc = prvCpssDxChCheckTxQPort(devNum,portMapPtr->txqPortNumber);
    if (GT_OK != rc )
    {
        return rc;
    }

    /* clear mapping ports */
    portArg[PRV_CPSS_DXCH_PORT_TYPE_PHYS_E          ] = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    portArg[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E         ] = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    portArg[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E         ] = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    portArg[PRV_CPSS_DXCH_PORT_TYPE_MAC_E           ] = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    portArg[PRV_CPSS_DXCH_PORT_TYPE_TXQ_E           ] = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    portArg[PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E  ] = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    portArg[PRV_CPSS_DXCH_PORT_TYPE_TM_PORT_IDX_E   ] = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;

    /* now compute mapping */
    portArg[PRV_CPSS_DXCH_PORT_TYPE_PHYS_E] = (GT_U32)portMapPtr->physicalPortNumber;
    if (portMapPtr->mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E)
    {
        portArg[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E ] = PRV_CPSS_DXCH_PORT_NUM_CPU_DMA_CNS;
        portArg[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E ] = PRV_CPSS_DXCH_PORT_NUM_CPU_DMA_CNS;

        prvCpssDxChHwPpSdmaCpuPortReservedIndexGet(devNum,
            &portArg[PRV_CPSS_DXCH_PORT_TYPE_MAC_E   ]);
    }
    else if (portMapPtr->mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E)
    {
            portArg[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E ] = PRV_CPSS_DXCH_PORT_NUM_ILKN_DMA_CNS;
            portArg[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E ] = PRV_CPSS_DXCH_PORT_NUM_ILKN_DMA_CNS;
            portArg[PRV_CPSS_DXCH_PORT_TYPE_MAC_E   ] = PRV_CPSS_DXCH_PORT_NUM_ILKN_MAC_CNS;

            if( GT_TRUE == portMapPtr->tmEnable )
            {
                rc = prvCpssDxChCheckIlknChannelPort(portMapPtr->interfaceNum);
                if (GT_OK != rc )
                {
                    return rc;
                }

                if (   portMapPtr->tmPortInd <  PRV_CPSS_DXCH_MAPPING_ILKN_MIN_TM_PORT_IDX_CNS
                    || portMapPtr->tmPortInd >  PRV_CPSS_DXCH_MAPPING_ILKN_MAX_TM_PORT_IDX_CNS)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
                }


                portArg[PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E] = portMapPtr->interfaceNum;
            }
            else
            {
                /*only channel 0 is supported without TM*/
                if(portMapPtr->interfaceNum == 0)
                {
                    portArg[PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E] = portMapPtr->interfaceNum;
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
    }
    else
    {
        if (portMapPtr->mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E)
        {
            rc = prvCpssDxChCheckDMAPort(devNum,portMapPtr->interfaceNum);
            if (GT_OK != rc )
            {
                return rc;
            }
        }
        if( GT_TRUE == portMapPtr->tmEnable )
        {
            if (portMapPtr->tmPortInd >  PRV_CPSS_DXCH_MAPPING_ETH_MAX_TM_PORT_IDX_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
        }
        portArg[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E ] = (GT_U32)portMapPtr->interfaceNum;
        portArg[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E ] = (GT_U32)portMapPtr->interfaceNum;
        portArg[PRV_CPSS_DXCH_PORT_TYPE_MAC_E   ] = (GT_U32)portMapPtr->interfaceNum;
    }
    portArg[PRV_CPSS_DXCH_PORT_TYPE_TXQ_E ] = (GT_U32)portMapPtr->txqPortNumber;

    /* TM enable - Tx-DMA/TxQ related configuration */
    if (GT_TRUE == portMapPtr->tmEnable)
    {
        portArg[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E      ] = PRV_CPSS_DXCH_PORT_NUM_TM_TxDMA_CNS;
        portArg[PRV_CPSS_DXCH_PORT_TYPE_TXQ_E        ] = PRV_CPSS_DXCH_PORT_NUM_TM_TxQ_CNS;
        portArg[PRV_CPSS_DXCH_PORT_TYPE_TM_PORT_IDX_E] = portMapPtr->tmPortInd;
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpsssDxChPortPhysicalPortTmMapping
*
* DESCRIPTION:
*       Configure TM realted port mappings.
*
* INPUTS:
*       devNum           - device number
*       portMapPtr       - pointer to array of mappings
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on bad param
*       GT_BAD_PTR            - on bad pointer
*
* COMMENTS:
*
********************************************************************************/
static GT_STATUS prvCpssDxChPortPhysicalPortTmMapping
(
     IN GT_U8                    devNum,
     IN CPSS_DXCH_PORT_MAP_STC  *portMapPtr
)
{
    GT_STATUS rc;                         /* return value */

                                          /* table type definition */
    PRV_CPSS_DXCH_TABLE_ENT tableTypeArr[2] = {PRV_CPSS_DXCH_TABLE_LAST_E, PRV_CPSS_DXCH_TABLE_LAST_E};
    GT_U32    regAddr;                    /* register address */
    GT_U32    regData;                    /* register data */
    GT_U32    bitOffset;                  /* data offset in register*/
    GT_U32    i;

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.trafficManager.supported == GT_TRUE)
    {
       /* TM enable - Physical port enabling */
       regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                    TMDROP.portsInterface[(portMapPtr->physicalPortNumber)/32];
       rc = prvCpssHwPpSetRegField(devNum, regAddr,
                                (portMapPtr->physicalPortNumber)%32, 1,
                                BOOL2BIT_MAC(portMapPtr->tmEnable));
       if (GT_OK != rc )
       {
           return rc;
       }

       /* TM port interface mapping */
       if( (CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E == portMapPtr->mappingType) ||
           (CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E == portMapPtr->mappingType) )
       {
           if(CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E == portMapPtr->mappingType)
           {
               switch((portMapPtr->interfaceNum)/32)
               {
                    case 0: regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                                                    TM_EGR_GLUE.portInterface0;
                            break;
                    case 1: regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                                                    TM_EGR_GLUE.portInterface1;
                            break;
                    case 2: regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                                                    TM_EGR_GLUE.portInterface2;
                            break;
                    default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
               }

               bitOffset = (portMapPtr->interfaceNum)%32;
           }
           else /* CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E == portMapPtr->mappingType */
           {
               regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                                                    TM_EGR_GLUE.portInterface2;
               bitOffset = 9;
           }

           rc = prvCpssHwPpSetRegField(devNum, regAddr, bitOffset, 1,
                                            BOOL2BIT_MAC(portMapPtr->tmEnable));
           if (GT_OK != rc )
           {
               return rc;
           }
       }

       /* TM enable - Tx DMA related configuration */
       if (GT_TRUE == portMapPtr->tmEnable)
       {
           if( ((portMapPtr->tmPortInd >= PRV_CPSS_DXCH_MAPPING_BOBCAT2_TM_PORT_NUM_LOW_RANGE_MAX_CNS) &&
                (portMapPtr->tmPortInd <  PRV_CPSS_DXCH_MAPPING_BOBCAT2_TM_PORT_NUM_HIGH_RANGE_START_CNS)) ||
                (portMapPtr->tmPortInd >=  PRV_CPSS_DXCH_MAPPING_BOBCAT2_TM_PORT_NUM_HIGH_RANGE_MAX_CNS) )
           {
               CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
           }

           regData = 0;
           /* TM can be used for Ethernet & Interlaken port type */
           switch(portMapPtr->mappingType)
           {
              case CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E:
                  U32_SET_FIELD_MAC(regData, 7, 1, 0);
                  tableTypeArr[0] = PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E;
                  break;

              case CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E:
                  U32_SET_FIELD_MAC(regData, 7, 1, 1);
                  tableTypeArr[0] = PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E;
                  break;

              default:
                  CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
           }

           U32_SET_FIELD_MAC(regData, 0, 7, portMapPtr->interfaceNum);

           /* Set TM port to physical port or interlaken channel mapping */
           rc = prvCpssDxChWriteTableEntryField
                            (devNum,
                            PRV_CPSS_DXCH_LION3_TABLE_TM_EGR_GLUE_TARGET_INTERFACE_E,
                            (portMapPtr->tmPortInd)/4,
                            0,
                            ((portMapPtr->tmPortInd)%4)*8,
                            8,
                            regData);

           if( GT_OK != rc )
           {
               return rc;
           }

           /* for bobk, ILKN mapping is used low ports, and Eth mapping is used for high ports  */
           if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E &&
              PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
           {
               tableTypeArr[1] = PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E;
           }

           /* Set physical port or interlaken channel to TM port mapping */
           for (i = 0; i < 2; i++)
           {

               if(tableTypeArr[i] != PRV_CPSS_DXCH_TABLE_LAST_E)
               {
                   rc = prvCpssDxChWriteTableEntryField
                                (devNum,
                                 tableTypeArr[i],
                                 (portMapPtr->interfaceNum)/4,
                                 0,
                                 ((portMapPtr->interfaceNum)%4)*8,
                                 8,
                                 portMapPtr->tmPortInd);
               }

               if( GT_OK != rc )
               {
                   return rc;
               }
           }
       }
    }
    else /* no TM */
    {
        if (GT_TRUE == portMapPtr->tmEnable)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMappingInit
*
* DESCRIPTION:
*       Init mapping structure to default (no mapping)
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       portMappingPtr - pointer to mappings structure
*       devNum         - device number
*
* OUTPUTS:
*       portMappingPtr - pointer to mappings structure
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on bad pointer
*
* COMMENTS:
*
*
********************************************************************************/
static GT_STATUS prvCpssDxChPortMappingInit
(
    INOUT PRV_CPSS_DXCH_PORT_MAPPING_STC *portMappingPtr,
       IN GT_U8                           devNum
)
{
    GT_U32    i;

    CPSS_NULL_PTR_CHECK_MAC(portMappingPtr);

    portMappingPtr->devNum = devNum;
    for (i = 0 ; i < PRV_CPSS_DXCH_PORT_NUM_MAPPING_MAX_PORT_CNS;  i++)
    {
        portMappingPtr->isPhysPortUsed         [i] = GT_FALSE;
        portMappingPtr->physIsEqualt2OldMapping[i] = GT_FALSE;
        portMappingPtr->rxDma2PhysMapArr       [i] = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
        portMappingPtr->phys2rxDmaMapArr       [i] = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
        portMappingPtr->phys2txDmaMapArr       [i] = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
        portMappingPtr->phys2macMapArr         [i] = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
        portMappingPtr->phys2txqMapArr         [i] = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
        portMappingPtr->txq2txDmaMapArr        [i] = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
        portMappingPtr->tm2physMapArr          [i] = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
        portMappingPtr->Ilkn2physMapArr        [i] = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
    }
    portMappingPtr->minIlknChnnalPhysPort = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
    portMappingPtr->maxIlknChnnalPhysPort = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortMappingCheckAndAdd
*
* DESCRIPTION:
*       Check current mapping and add it
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       portMappingPtr       - pointer to mappings structure
*       devNum               - device number
*       prvMappingType       - private mapping type (first ILKN channel is ILKN MAC channel)
*       portArgArr           - entry of single user mapping
*
* OUTPUTS:
*       portMappingPtr       - pointer to mappings structure
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on bad pointer
*
* COMMENTS:
*
*
********************************************************************************/

static GT_STATUS prvCpssDxChPortMappingCheckAndAdd
(
    INOUT PRV_CPSS_DXCH_PORT_MAPPING_STC *portMappingPtr,
       IN GT_U8                           devNum,
       IN PRV_MAPTYPE_ENT                 prvMappingType,
       IN GT_U32                          portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAX_E]
)
{
    GT_STATUS rc;

    GT_U32 phys  ;
    GT_U32 rxDma ;
    GT_U32 txDma ;
    GT_U32 mac   ;
    GT_U32 txq   ;
    GT_U32 ilknIdx;
    GT_U32 tmIdx;
    GT_BOOL isEqual2OldMapping;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (!PRV_CPSS_SIP_5_CHECK_MAC(devNum)) /* currently only Bobcat2/3 family is supported */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(portMappingPtr) ;
    CPSS_NULL_PTR_CHECK_MAC(portArgArr);

    if (devNum != portMappingPtr->devNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    phys     = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_PHYS_E ];
    rxDma    = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E];
    txDma    = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E];
    mac      = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAC_E  ];
    txq      = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TXQ_E  ];
    ilknIdx  = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E];
    tmIdx    = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TM_PORT_IDX_E];



    /* check that physical is steel not in use */
    if (portMappingPtr->isPhysPortUsed[phys] == GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* ILKN channel has no DMA port */
    /* Several remote ports could be mapped to same DMA/TXQ */
    if ((prvMappingType != PRV_MAPTYPE_ILKN_CHANNEL_E) &&
        (prvMappingType != PRV_MAPTYPE_CPU_SDMA_E) &&
        (prvMappingType != PRV_MAPTYPE_REMOTE_PHYS_PORT_E))
    {
        if (portMappingPtr->rxDma2PhysMapArr[rxDma] != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS && portMappingPtr->rxDma2PhysMapArr[rxDma] != phys)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if (portMappingPtr->phys2rxDmaMapArr[phys] != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS && portMappingPtr->phys2rxDmaMapArr[phys] != rxDma)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (portMappingPtr->phys2txDmaMapArr[phys] != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS && portMappingPtr->phys2txDmaMapArr[phys] != txDma)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (portMappingPtr->phys2macMapArr[phys] != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS && portMappingPtr->phys2macMapArr[phys] != mac)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (portMappingPtr->phys2txqMapArr[phys] != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS && portMappingPtr->phys2txqMapArr[phys] != txq)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (portMappingPtr->txq2txDmaMapArr[txq] != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS && portMappingPtr->txq2txDmaMapArr[txq] != txDma)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (tmIdx != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS)
    {
        if (portMappingPtr->tm2physMapArr[tmIdx] != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS && portMappingPtr->tm2physMapArr[tmIdx] != phys)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    if (ilknIdx != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS)
    {
        if (portMappingPtr->Ilkn2physMapArr[ilknIdx] != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS && portMappingPtr->Ilkn2physMapArr[ilknIdx] != phys)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /*---------------------------------------------------------------------*/
    /* test Interlaken channel physical port that shall be smaller than 64 */
    /*---------------------------------------------------------------------*/
    if (prvMappingType == PRV_MAPTYPE_ILKN_CHANNEL_E || prvMappingType == PRV_MAPTYPE_ILKN_MAC_E)
    {
        if (portMappingPtr->minIlknChnnalPhysPort == PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS)
        {
            portMappingPtr->minIlknChnnalPhysPort = phys;
            portMappingPtr->maxIlknChnnalPhysPort = phys;
        }
        else
        {
            if (phys < portMappingPtr->minIlknChnnalPhysPort)
            {
                portMappingPtr->minIlknChnnalPhysPort = phys;
            }
            else
            {
                if (phys > portMappingPtr->maxIlknChnnalPhysPort)
                {
                    portMappingPtr->maxIlknChnnalPhysPort = phys;
                }
            }
        }
        if (portMappingPtr->maxIlknChnnalPhysPort - portMappingPtr->minIlknChnnalPhysPort > PRV_CPSS_DXCH_MAPPING_BOBCAT2_ILKN_CHANNEL_MAX_PHYS_PORT_DIFF_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, phys, /*OUT*/&portMapShadowPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    isEqual2OldMapping = GT_FALSE;
    if (portMapShadowPtr->valid == GT_TRUE)
    {
        if (/*prvMappingType == portMapShadowPtr->portMap.mappingType */
               mac         == portMapShadowPtr->portMap.macNum
            && rxDma       == portMapShadowPtr->portMap.rxDmaNum
            && txDma       == portMapShadowPtr->portMap.txDmaNum
            && txq         == portMapShadowPtr->portMap.txqNum
            && tmIdx       == portMapShadowPtr->portMap.tmPortIdx
            && ilknIdx     == portMapShadowPtr->portMap.ilknChannel
            )
        {
            isEqual2OldMapping = GT_TRUE;
        }
    }
    /* strore mapping */
    portMappingPtr->isPhysPortUsed   [phys] = GT_TRUE;
    portMappingPtr->physIsEqualt2OldMapping[phys] = isEqual2OldMapping;
    portMappingPtr->rxDma2PhysMapArr[rxDma] = phys ;
    portMappingPtr->phys2rxDmaMapArr[phys ] = rxDma;
    portMappingPtr->phys2txDmaMapArr[phys ] = txDma;
    portMappingPtr->phys2macMapArr  [phys ] = mac;
    portMappingPtr->phys2txqMapArr  [phys ] = txq;
    portMappingPtr->txq2txDmaMapArr [txq  ] = txDma;

    if (tmIdx != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS)
    {
        portMappingPtr->tm2physMapArr   [tmIdx]   = phys;
    }
    if (ilknIdx != PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS)
    {
        portMappingPtr->Ilkn2physMapArr [ilknIdx] = phys;
    }
    return GT_OK;
}


#if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
    static GT_CHAR * prv_mappingTypeStr[CPSS_DXCH_PORT_MAPPING_TYPE_MAX_E];
    static GT_CHAR * prv_portArgStr[PRV_CPSS_DXCH_PORT_TYPE_MAX_E];
#endif


/*******************************************************************************
* internal_cpssDxChPortPhysicalPortMapSet
*
* DESCRIPTION:
*       configure ports mapping
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portMapArraySize - Number of ports to map, array size
*       portMapArrayPtr  - pointer to array of mappings
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - wrong devNum, portNum
*       GT_BAD_PTR            - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. All usable ports shall be configured at ONCE. Any port, that was mapped previously and is
*          not included into current mapping, losses its mapping.
*       2. Changing mapping under traffic may cause to traffic interruption.
*       3. when CPSS_DXCH_CFG_PORT_MAPPING_TYPE_INLK_CHANNEL_E  is defined the first channel
*          phsical port is used for DMA configuration
*       4. The function checks whether all mapping are consistent.
*          In case of contradiction between mappings
*          (e,g. single TxQ port is mapped to several Tx DMA ports) the function returns with BAD_PARAM.
*       5. Other CPSS APIs that receive GT_PHYSICAL_PORT_NUM as input parameter will use this mapping
*          to convert the physical port to mac/dma/txq  to configure them
*       6. Usage example
*        #define GT_NA ((GT_U32)-1)
*        CPSS_DXCH_CFG_PORT_MAP_STC bc2ExampleMap[] =
*        {
*            {  0, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,      0,  0, GT_FALSE},
*            {  1, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,      1,  1, GT_FALSE},
*            { 50, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,     50,  2, GT_FALSE},
*            { 53, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,     53,  3, GT_FALSE},
*            { 63, _INTRFACE_TYPE_CPU_SDMA_E,     GT_NA,  GT_NA, 71, GT_FALSE},
*            {100, _INTRFACE_TYPE_ILNK_MAC_E,     GT_NA,  GT_NA, 40, GT_TRUE},
*            {101, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 41, GT_TRUE},
*            {102, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 42, GT_TRUE},
*            {103, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 43, GT_TRUE}
*        };
*        rc = cpssDxChCfgPhysicalPortMapSet(devNum, sizeof(bc2ExampleMap)/sizeof(bc2ExampleMap[0]), &bc2ExampleMap[0]);
*
*
********************************************************************************/
static GT_STATUS internal_cpssDxChPortPhysicalPortMapSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portMapArraySize,
    IN  CPSS_DXCH_PORT_MAP_STC     *portMapArrayPtr
)
{
    GT_U32 i;
    GT_U32 srcPort;
    GT_U32 dstPort;
    CPSS_PP_FAMILY_BIT_ENT devType;
    PRV_MAPTYPE_ENT        prvMappingType;
    GT_U32 physPort;

    static PRV_CPSS_DXCH_PORT_MAPPING_STC   portMapping;
    GT_U32                                  portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAX_E];
    CPSS_DXCH_PORT_MAP_STC                * portMapPtr;
    PRV_DXCH_CPSS_MAPPING_SET_GET_FUN_STC * funListPtr;
    GT_STATUS rc;
    GT_BOOL   firstInterlackingChannelFound;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(portMapArrayPtr);

    rc = prvCpssDxChPortMappingInit(&portMapping,devNum);
    if (GT_OK != rc )
    {
        return rc;
    }

    /*----------------------------------*/
    /* first pass : check the arguments */
    /*----------------------------------*/
    #if (PRV_CPSS_MAPPING_DEBUG_CNS == 1)
        prv_portArgStr[PRV_CPSS_DXCH_PORT_TYPE_PHYS_E         ] = "phys ";
        prv_portArgStr[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E        ] = "rxDma";
        prv_portArgStr[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E        ] = "txDma";
        prv_portArgStr[PRV_CPSS_DXCH_PORT_TYPE_MAC_E          ] = "mac  ";
        prv_portArgStr[PRV_CPSS_DXCH_PORT_TYPE_TXQ_E          ] = "txq  ";
        prv_portArgStr[PRV_CPSS_DXCH_PORT_TYPE_TM_PORT_IDX_E  ] = "tm   ";

        prv_mappingTypeStr[CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E ]        = "ETHERNET";
        prv_mappingTypeStr[CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E     ]        = "CPU-SDMA";
        prv_mappingTypeStr[CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E ]        = "ILKN-CHL";
        prv_mappingTypeStr[CPSS_DXCH_PORT_MAPPING_TYPE_REMOTE_PHYSICAL_PORT_E] = "REMOTE-P";
    #endif

    #if (PRV_CPSS_MAPPING_CHECK_PRINT_CNS == 1)
        cpssOsPrintf("\nCheck Mapping:");
    #endif
    firstInterlackingChannelFound = GT_FALSE;
    portMapPtr = portMapArrayPtr;
    #if (PRV_CPSS_MAPPING_CHECK_PRINT_CNS == 1)
        cpssOsPrintf("\n+-----------------+----------------------------+-----------------------------------+-----+");
        cpssOsPrintf("\n|                 |      Input                 |          Computed                 |     |");
        cpssOsPrintf("\n|   mapping Type  |------+---------------------+-----------------------------------+ Res |");
        cpssOsPrintf("\n|                 | phys |  if txq tmEn tmPort | phys rxdma txdma mac txq ilkn  tm |     |");
        cpssOsPrintf("\n+-----------------+------+---------------------+-----------------------------------+-----+");
    #endif
    for ( i = 0; i< portMapArraySize ; i++, portMapPtr++)
    {
        CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
        #if (PRV_CPSS_MAPPING_CHECK_PRINT_CNS == 1)
            cpssOsPrintf("\n| %-15s | %4d | %3d %3d %4d %6d |",
                                               prv_mappingTypeStr[portMapPtr->mappingType],
                                               portMapPtr->physicalPortNumber,
                                               portMapPtr->interfaceNum,
                                               portMapPtr->txqPortNumber,
                                               portMapPtr->tmEnable,
                                               portMapPtr->tmPortInd);
        #endif
        rc = prvCpssDxChPortMappingTypeCheck(devNum,portMapPtr);
        if (GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortMappingTypeGet(portMapPtr->mappingType, /*OUT*/&prvMappingType);
        if (GT_OK != rc )
        {
            return rc;
        }
        if (prvMappingType == PRV_MAPTYPE_ILKN_CHANNEL_E)
        {
            if (firstInterlackingChannelFound == GT_FALSE)
            {
                prvMappingType = PRV_MAPTYPE_ILKN_MAC_E;
                firstInterlackingChannelFound = GT_TRUE;
            }
        }
        /*-------------------------*/
        /* build argumets          */
        /*-------------------------*/
        rc = prvCpsssDxChPortMappingArgumentsBuild(devNum,portMapPtr,/*OUT*/&portArgArr[0]);
        if (GT_OK != rc )
        {
            return rc;
        }
        /*---------------------------------------------------------------------------------*/
        /* check if port already initilized i.e. has bandwidth , that mapping is unchanged */
        /*---------------------------------------------------------------------------------*/
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portMapPtr->physicalPortNumber,/*OUT*/&portMapShadowPtr);
        if (portMapShadowPtr->valid == GT_TRUE)
        {
            if (portMapShadowPtr->portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E)
            {
                CPSS_PORT_INTERFACE_MODE_ENT ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMapShadowPtr->portMap.macNum);
                CPSS_PORT_SPEED_ENT          speed = PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum,   portMapShadowPtr->portMap.macNum);
                if(CPSS_PORT_INTERFACE_MODE_NA_E != ifMode)
                {
                    if (CPSS_PORT_SPEED_NA_E != speed)
                    {
                        if (portMapShadowPtr->portMap.macNum   != portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAC_E])
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                        }
                        if (portMapShadowPtr->portMap.rxDmaNum != portArgArr[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E])
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                        }
                        if (portMapShadowPtr->portMap.txDmaNum != portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E])
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                        }
                        if (portMapShadowPtr->portMap.txqNum != portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TXQ_E])
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                        }
                    }
                }
            }
        }


        #if (PRV_CPSS_MAPPING_CHECK_PRINT_CNS == 1)
            cpssOsPrintf(" %4d %5d %5d %3d %3d %4d %3d |",
                        portArgArr[PRV_CPSS_DXCH_PORT_TYPE_PHYS_E],
                        portArgArr[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E],
                        portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E],
                        portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAC_E],
                        portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TXQ_E],
                        portArgArr[PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E],
                        portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TM_PORT_IDX_E]);
        #endif

        /*-------------------------*/
        /*  check                  */
        /*-------------------------*/
        rc = prvCpssDxChPortMappingCheckAndAdd(/*INOUT*/&portMapping, /*IN*/devNum,/*IN*/prvMappingType,/*IN*/portArgArr);
        if (GT_OK != rc )
        {
            return rc;
        }
        #if (PRV_CPSS_MAPPING_CHECK_PRINT_CNS == 1)
            cpssOsPrintf("  OK |");
        #endif

    }
    #if (PRV_CPSS_MAPPING_CHECK_PRINT_CNS == 1)
        cpssOsPrintf("\n+-----------------+------+---------------------+-----------------------------------+-----+");
    #endif


    /*****************************/
    /*   Shadow DB maintenance   */
    /*****************************/
    /* Clear all shadow DB */
    rc = prvCpssDxChPortPhysicalPortMapShadowDBInit(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*----------------------------------*/
    /* second pass : perform mapping    */
    /*----------------------------------*/
    firstInterlackingChannelFound = GT_FALSE;
    portMapPtr = portMapArrayPtr;
    for ( i = 0; i< portMapArraySize ; i++, portMapPtr++)
    {
        /*-----------------------------*/
        /* build internal mapping type */
        /*------------------------------------------------------------------------------------------*/
        /* physical port of first interlaken channel is used for RxDMA configurationfind            */
        /* it shall be NOT called for any other interlaken channel                                  */
        /* therefor its type is changed to PRV_MAPTYPE_ILKN_MAC_E                                   */
        /*------------------------------------------------------------------------------------------*/
        rc = prvCpssDxChPortMappingTypeGet(portMapPtr->mappingType, /*OUT*/&prvMappingType);
        if (GT_OK != rc )
        {
            return rc;
        }
        if (prvMappingType == PRV_MAPTYPE_ILKN_CHANNEL_E)
        {
            if (firstInterlackingChannelFound == GT_FALSE)
            {
                prvMappingType = PRV_MAPTYPE_ILKN_MAC_E;
                firstInterlackingChannelFound = GT_TRUE;
            }
        }

        /*------------------*/
        /* build dev type   */
        /*------------------*/
        rc = prvCpsssDxChPortMappingDeviceTypeGet(PRV_CPSS_PP_MAC(devNum)->devFamily,
            PRV_CPSS_PP_MAC(devNum)->devSubFamily,/*OUT*/&devType);
        if (GT_OK != rc )
        {
            return rc;
        }

        /*------------------*/
        /* build argumets   */
        /*------------------*/
        rc = prvCpsssDxChPortMappingArgumentsBuild(devNum,portMapPtr,/*OUT*/&portArgArr[0]);
        if (GT_OK != rc )
        {
            return rc;
        }


        /*-------------------------------------------------------------------*/
        /* call all function from list                                       */
        /*-------------------------------------------------------------------*/
        #if (PRV_CPSS_MAPPING_UNIT_LIST_DEBUG_CNS == 1)
            cpssOsPrintf("\n%-15s phys = %3d if = %3d txq = %3d tm = %d tmPort = %d",
                                               prv_mappingTypeStr[portMapPtr->mappingType],
                                               portMapPtr->physicalPortNumber,
                                               portMapPtr->interfaceNum,
                                               portMapPtr->txqPortNumber,
                                               portMapPtr->tmEnable,
                                               portMapPtr->tmPortInd);
        #endif
        physPort = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_PHYS_E];
        if (portMapping.physIsEqualt2OldMapping[physPort] == GT_FALSE)
        {
            for (funListPtr = &prv_MappingFunList[0]; funListPtr->mapingTypeBmp != 0; funListPtr++)
            {
                /* if function is applicable for the mapping type */
                if (0 != (prvMappingType & funListPtr->mapingTypeBmp))
                {
                    /* if function is applicable for the input device */
                    if (0 != (devType & funListPtr->applicableDevice))
                    {
                        srcPort = portArgArr[funListPtr->srcPortType];
                        dstPort = portArgArr[funListPtr->dstPortType];
                        #if (PRV_CPSS_MAPPING_UNIT_LIST_DEBUG_CNS == 1)
                            cpssOsPrintf("\n    %-20s %s (%3d) --> %s (%3d)",funListPtr->stringMapTypePtr,
                                                                              prv_portArgStr[funListPtr->srcPortType],srcPort,
                                                                              prv_portArgStr[funListPtr->dstPortType],dstPort);
                        #endif
                        rc = funListPtr->setFun(devNum,portMapPtr->portGroup, srcPort,dstPort);
                        if (GT_OK != rc )
                        {
                            return rc;
                        }
                    }
                }
            }

            /*-----------------------------*/
            /* TM related configurations   */
            /*-----------------------------*/
            rc = prvCpssDxChPortPhysicalPortTmMapping(devNum,portMapPtr);
            if (GT_OK != rc )
            {
                return rc;
            }
        }
        /****************************************************************************************/
        /*   Shadow DB maintenance shall be performed in any case (previous mapping was erased  */
        /****************************************************************************************/
        rc = prvCpssDxChPortPhysicalPortMapShadowDBSet(devNum, portMapPtr->physicalPortNumber, portMapPtr, portArgArr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChBobKHighSpeedPortConfigure(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }    
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPhysicalPortMapSet
*
* DESCRIPTION:
*       configure ports mapping
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portMapArraySize - Number of ports to map, array size
*       portMapArrayPtr  - pointer to array of mappings
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - wrong devNum, portNum
*       GT_BAD_PTR            - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. All usable ports shall be configured at ONCE. Any port, that was mapped previously and is
*          not included into current mapping, losses its mapping.
*       2. Changing mapping under traffic may cause to traffic interruption.
*       3. when CPSS_DXCH_CFG_PORT_MAPPING_TYPE_INLK_CHANNEL_E  is defined the first channel
*          phsical port is used for DMA configuration
*       4. The function checks whether all mapping are consistent.
*          In case of contradiction between mappings
*          (e,g. single TxQ port is mapped to several Tx DMA ports) the function returns with BAD_PARAM.
*       5. Other CPSS APIs that receive GT_PHYSICAL_PORT_NUM as input parameter will use this mapping
*          to convert the physical port to mac/dma/txq  to configure them
*       6. Usage example
*        #define GT_NA ((GT_U32)-1)
*        CPSS_DXCH_CFG_PORT_MAP_STC bc2ExampleMap[] =
*        {
*            {  0, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,      0,  0, GT_FALSE},
*            {  1, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,      1,  1, GT_FALSE},
*            { 50, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,     50,  2, GT_FALSE},
*            { 53, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,     53,  3, GT_FALSE},
*            { 63, _INTRFACE_TYPE_CPU_SDMA_E,     GT_NA,  GT_NA, 71, GT_FALSE},
*            {100, _INTRFACE_TYPE_ILNK_MAC_E,     GT_NA,  GT_NA, 40, GT_TRUE},
*            {101, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 41, GT_TRUE},
*            {102, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 42, GT_TRUE},
*            {103, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 43, GT_TRUE}
*        };
*        rc = cpssDxChCfgPhysicalPortMapSet(devNum, sizeof(bc2ExampleMap)/sizeof(bc2ExampleMap[0]), &bc2ExampleMap[0]);
*
*
********************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortMapSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portMapArraySize,
    IN  CPSS_DXCH_PORT_MAP_STC     *portMapArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPhysicalPortMapSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portMapArraySize, portMapArrayPtr));

    rc = internal_cpssDxChPortPhysicalPortMapSet(devNum, portMapArraySize, portMapArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portMapArraySize, portMapArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPhysicalPortMapGet
*
* DESCRIPTION:
*       get ports mappping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       physicalPortNum - physical port to bring
*
* OUTPUTS:
*       portMapPtr  - pointer to ports mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_FAIL         - when cannot deliver requested number of items
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPhysicalPortMapGet
(
    IN GT_U8                       devNum,
    IN GT_PHYSICAL_PORT_NUM        physicalPortNum,
    OUT CPSS_DXCH_SHADOW_PORT_MAP_STC  *portMapPtr
)
{
    GT_STATUS rc;
    GT_PHYSICAL_PORT_NUM physPort4Cntrol;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, physicalPortNum);
    CPSS_NULL_PTR_CHECK_MAC(portMapPtr);

    if (!PRV_CPSS_SIP_5_CHECK_MAC(devNum)) /* currently only Bobcat2/3 is supported */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }


    portMapPtr->txqNum      = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
    portMapPtr->macNum      = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
    portMapPtr->rxDmaNum    = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
    portMapPtr->txDmaNum    = PRV_CPSS_DXCH_PORT_NUM_MAPPING_INVALID_PORT_CNS;
    portMapPtr->portGroup   = 0;

    rc = prvCpssDxChPortMappingBMAMapOfLocalPhys2RxDMAGet(devNum,portMapPtr->portGroup,physicalPortNum,/*OUT*/&portMapPtr->rxDmaNum); /* 3  Phys --> RxDma */
    if (GT_OK != rc )
    {
        return rc;
    }

    rc = prvCpssDxChPortMappingPFCTriggerPhysPort2MACMapGet(devNum,portMapPtr->portGroup,physicalPortNum,/*OUT*/&portMapPtr->macNum);  /* 7 Phys --> Mac */
    if (GT_OK != rc )
    {
        return rc;
    }

    rc = prvCpssDxChPortMappingRxDMA2LocalPhysGet(devNum,portMapPtr->portGroup,portMapPtr->rxDmaNum,/*OUT*/&physPort4Cntrol);  /* 1 RxDma --> Phys */
    if (GT_OK != rc )
    {
        return rc;
    }

    rc = prvCpssDxChPortMappingEGFTargetLocalPhys2TxQPortMapGet(devNum,portMapPtr->portGroup,physicalPortNum,/*OUT*/&portMapPtr->txqNum);  /* 4.1 Phys --> TxQ */
    if (GT_OK != rc )
    {
        return rc;
    }

    rc = prvCpssDxChPortMappingTxQPort2TxDMAMapGet(devNum,portMapPtr->portGroup,portMapPtr->txqNum,/*OUT*/&portMapPtr->txDmaNum);             /* 5 TxQ --> txDma */
    if (GT_OK != rc )
    {
        return rc;
    }

    if (portMapPtr->rxDmaNum == PRV_CPSS_DXCH_PORT_NUM_CPU_DMA_CNS)
    {
        portMapPtr->mappingType = CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E;
    }
    else if (portMapPtr->rxDmaNum == PRV_CPSS_DXCH_PORT_NUM_ILKN_DMA_CNS)
    {/* this DMA number can be either ILKN or usual ethernet */
        CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, physicalPortNum, /*OUT*/&portMapShadowPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
                portMapPtr->mappingType = portMapShadowPtr->portMap.mappingType;
    }
    else /* dmaPort != CPU (72) && dmaPort != ILKN (64) */
    {
        if (physPort4Cntrol == physicalPortNum)
        {
            portMapPtr->mappingType = CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E;
        }
        else
        {
            portMapPtr->mappingType = CPSS_DXCH_PORT_MAPPING_TYPE_REMOTE_PHYSICAL_PORT_E;
        }
    }

    portMapPtr->ilknChannel = PRV_CPSS_DXCH_PP_MAC(devNum)->
                port.portsMapInfoShadowArr[physicalPortNum].portMap.ilknChannel;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPhysicalPortTmMappingGet
*
* DESCRIPTION:
*       Get TM related port mappping
*
* INPUTS:
*       devNum   - device number
*       physicalPortNum - physical port to bring
*       portMapPtr  - pointer to port mapping
*
* OUTPUTS:
*       portMapPtr  - pointer to port mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_FAIL         - when cannot deliver requested number of items
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPhysicalPortTmMappingGet
(
    IN GT_U8                       devNum,
    IN GT_PHYSICAL_PORT_NUM        physicalPortNum,
    INOUT CPSS_DXCH_PORT_MAP_STC    *portMapPtr
)
{
    GT_STATUS rc;                       /* return value */
    PRV_CPSS_DXCH_TABLE_ENT tableType;  /* table type definition */
    GT_U32    regAddr;                  /* register address */
    GT_U32    regData;                  /* register data */

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, physicalPortNum);

    if( PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.trafficManager.supported == GT_TRUE)
    {
        /* TM enable - Physical port enabling status */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->
                TMDROP.portsInterface[(portMapPtr->physicalPortNumber)/32];
        rc = prvCpssHwPpGetRegField(devNum, regAddr,
                            (portMapPtr->physicalPortNumber)%32, 1,
                            &regData);
        if (GT_OK != rc )
        {
            return rc;
        }

        portMapPtr->tmEnable = BIT2BOOL_MAC(regData);

        if( GT_TRUE == portMapPtr->tmEnable )
        {
            switch(portMapPtr->mappingType)
            {
                case CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E:
                    tableType =
                  PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E;
                    break;

                case CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E:
                    tableType =
                  PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E;
                    break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            /* Get physical port or interlaken channel to TM port mapping */
            rc = prvCpssDxChReadTableEntryField
                        (devNum,
                         tableType,
                         (portMapPtr->interfaceNum)/4,
                         0,
                         ((portMapPtr->interfaceNum)%4)*8,
                         8,
                         &portMapPtr->tmPortInd);

            if( GT_OK != rc )
            {
                return rc;
            }
        }
    }
    else /* no TM */
    {
        portMapPtr->tmEnable = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortPhysicalPortMapGet
*
* DESCRIPTION:
*       get ports mappping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       firstPhysicalPortNumber - first physical port to bring
*       portMapArraySize - Mapping array size
*
* OUTPUTS:
*       portMapArrayPtr  - pointer to ports mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_FAIL         - when cannot deliver requested number of items
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPhysicalPortMapGet
(
    IN GT_U8                       devNum,
    IN GT_PHYSICAL_PORT_NUM        firstPhysicalPortNumber,
    IN GT_U32                      portMapArraySize,
    OUT CPSS_DXCH_PORT_MAP_STC    *portMapArrayPtr
)
{
    GT_STATUS rc;
    GT_U32 i;
    GT_PHYSICAL_PORT_NUM physPort;

    CPSS_DXCH_SHADOW_PORT_MAP_STC detailedPortMap;
    CPSS_DXCH_PORT_MAP_STC * portMapPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (!PRV_CPSS_SIP_5_CHECK_MAC(devNum))  /* currently only Bobcat2/3 is supported */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(portMapArrayPtr);

    if ((firstPhysicalPortNumber + portMapArraySize - 1) > PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    portMapPtr = portMapArrayPtr;
    physPort = firstPhysicalPortNumber;
    for ( i = 0; i< portMapArraySize ; i++, portMapPtr++, physPort++)
    {
        rc = prvCpssDxChPortPhysicalPortMapGet(devNum,physPort,/*OUT*/&detailedPortMap);
        if (GT_OK != rc )
        {
            return rc;
        }

        portMapPtr->physicalPortNumber = physPort;
        portMapPtr->mappingType        = detailedPortMap.mappingType;
        if( CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E == portMapPtr->mappingType )
        {
            portMapPtr->interfaceNum = detailedPortMap.ilknChannel;
        }
        else
        {
            portMapPtr->interfaceNum       = detailedPortMap.rxDmaNum;       /* MAC == DMA , interface num is used for MAC type of mappings */
        }
        portMapPtr->portGroup          = detailedPortMap.portGroup;
        portMapPtr->txqPortNumber      = detailedPortMap.txqNum;

        rc = prvCpssDxChPortPhysicalPortTmMappingGet(devNum, physPort, /*OUT*/portMapPtr);
        if (GT_OK != rc )
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPhysicalPortMapGet
*
* DESCRIPTION:
*       get ports mappping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       firstPhysicalPortNumber - first physical port to bring
*       portMapArraySize - Mapping array size
*
* OUTPUTS:
*       portMapArrayPtr  - pointer to ports mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_FAIL         - when cannot deliver requested number of items
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortMapGet
(
    IN GT_U8                       devNum,
    IN GT_PHYSICAL_PORT_NUM        firstPhysicalPortNumber,
    IN GT_U32                      portMapArraySize,
    OUT CPSS_DXCH_PORT_MAP_STC    *portMapArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPhysicalPortMapGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, firstPhysicalPortNumber, portMapArraySize, portMapArrayPtr));

    rc = internal_cpssDxChPortPhysicalPortMapGet(devNum, firstPhysicalPortNumber, portMapArraySize, portMapArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, firstPhysicalPortNumber, portMapArraySize, portMapArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPhysicalPortMapIsValidGet
*
* DESCRIPTION:
*       Check if physical port was mapped
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       isValidPtr      - (pointer to) is valid mapping
*                         GT_TRUE - given port was mapped
*                         GT_FALSE - there is no valid mapping for given port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPhysicalPortMapIsValidGet
(
    IN GT_U8                       devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_BOOL                    *isValidPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(isValidPtr);

    *isValidPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[portNum].valid;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPhysicalPortMapIsValidGet
*
* DESCRIPTION:
*       Check if physical port was mapped
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       isValidPtr      - (pointer to) is valid mapping
*                         GT_TRUE - given port was mapped
*                         GT_FALSE - there is no valid mapping for given port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortMapIsValidGet
(
    IN GT_U8                       devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_BOOL                    *isValidPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPhysicalPortMapIsValidGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, isValidPtr));

    rc = internal_cpssDxChPortPhysicalPortMapIsValidGet(devNum, portNum, isValidPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, isValidPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPhysicalPortMapReverseMappingGet
*
* DESCRIPTION:
*       Function checks and returns physical port number that mapped to given
*       MAC/TXQ/DMA port number.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       origPortType      - original port type
*       origPortNum       - MAC/TXQ/DMA port number
*
* OUTPUTS:
*       physicalPortNumPtr  - (pointer to) physical port number
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, origPortNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_FOUND    - not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPhysicalPortMapReverseMappingGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_PORT_MAPPING_TYPE_ENT origPortType,
    IN  GT_U32                          origPortNum,
    OUT GT_PHYSICAL_PORT_NUM            *physicalPortNumPtr
)
{
    if(origPortType != CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E)
    {
        return GT_NOT_IMPLEMENTED;
    }

    return prvCpssDxChPortPhysicalPortMapReverseMappingGet(devNum,
        PRV_CPSS_DXCH_PORT_TYPE_MAC_E,origPortNum,physicalPortNumPtr);
}


/*******************************************************************************
* cpssDxChPortPhysicalPortMapReverseMappingGet
*
* DESCRIPTION:
*       Function checks and returns physical port number that mapped to given
*       MAC/TXQ/DMA port number.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       origPortType      - original port type
*       origPortNum       - MAC/TXQ/DMA port number
*
* OUTPUTS:
*       physicalPortNumPtr  - (pointer to) physical port number
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, origPortNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_FOUND    - not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortMapReverseMappingGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_PORT_MAPPING_TYPE_ENT origPortType,
    IN  GT_U32                          origPortNum,
    OUT GT_PHYSICAL_PORT_NUM            *physicalPortNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPhysicalPortMapReverseMappingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, origPortType, origPortNum, physicalPortNumPtr));

    rc = internal_cpssDxChPortPhysicalPortMapReverseMappingGet(devNum, origPortType,
        origPortNum, physicalPortNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, origPortType, origPortNum, physicalPortNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


static GT_STATUS internal_cpssDxChPortPhysicalPortDetailedMapGet
(
    IN   GT_U8                   devNum,
    IN   GT_PHYSICAL_PORT_NUM    portNum,
    OUT  CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr
)
{
    GT_STATUS rc;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *tmp_portMapShadowPtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    /* PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum); */

    CPSS_NULL_PTR_CHECK_MAC(portMapShadowPtr);

    rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portNum,/*OUT*/&tmp_portMapShadowPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    /*---------------------------------*
     *   copy data                     *
     *---------------------------------*/
    *portMapShadowPtr = *tmp_portMapShadowPtr;
    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortPhysicalPortDetailedMapGet
*
* DESCRIPTION:
*       Function returns physical port detailed mapping
*       MAC/TXQ/DMA port number.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number
*
* OUTPUTS:
*       portMapShadowPtr  - (pointer to) detailed mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortDetailedMapGet
(
    IN   GT_U8                   devNum,
    IN   GT_PHYSICAL_PORT_NUM    portNum,
    OUT  CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPhysicalPortDetailedMapGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portMapShadowPtr));

    rc = internal_cpssDxChPortPhysicalPortDetailedMapGet(devNum,portNum,portMapShadowPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portMapShadowPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


