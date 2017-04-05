#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegsVer1.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBobKResource.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortInterlaken.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPizzaArbiterWorkConserving.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPAUnitBW.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtDbDxBobcat2Mappings.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern GT_STATUS cpssInitSystem
(
    IN  GT_U32  boardIdx,
    IN  GT_U32  boardRevId,
    IN  GT_U32  reloadEeprom
);

extern GT_STATUS appDemoTraceHwAccessEnable
(
    IN GT_U8                                devNum,
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT    accessType,
    IN GT_BOOL                              enable
);


extern GT_STATUS appDemoDbEntryAdd
(
    IN  GT_CHAR *namePtr,
    IN  GT_U32  value
);

extern GT_STATUS appDemoDebugDeviceIdSet
(
    IN GT_U8    devNum,
    IN GT_U32   deviceId
);

extern GT_STATUS appDemoBc2PortListInit
(
    IN  GT_U8 dev,
    PortInitList_STC * portInitList
);

extern GT_STATUS gtBobcat2PortPizzaArbiterIfStateDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId
);

extern GT_STATUS gtBobcat2PortPizzaArbiterIfUnitStateDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  CPSS_DXCH_PA_UNIT_ENT unit
);

extern GT_STATUS gtBobcat2PortMappingDump
(
    IN  GT_U8  dev
);


extern GT_STATUS appDemoBobk2PortListMacPcsStatePrint
(
    IN GT_U8 dev
);

extern GT_STATUS appDemoBobk2PortListResourcesPrint
(
    IN GT_U8 dev
);

extern GT_STATUS gtBobcat2TxQHighSpeedPortPrint
(
    IN  GT_U8  dev
);

extern GT_STATUS gtBobcat2TxQHighSpeed_DQ_LL_Dump
(
    IN  GT_U8  dev
);


extern GT_STATUS prvCpssDxChCaelumPortResourseConfigGet
(
    IN  GT_U8                                        devNum,
    IN  GT_PHYSICAL_PORT_NUM                         portNum,
    OUT PRV_CPSS_DXCH_CAELUM_PORT_RESOURCE_STC    *resourcePtr
);

extern void appDemoBc2SetCustomPortsMapping
(
    IN GT_BOOL enable
);

extern GT_STATUS appDemoBc2TmPortsInit
(
    IN GT_U8 dev
);

extern GT_CHAR * CPSS_SPEED_2_STR
(
    CPSS_PORT_SPEED_ENT speed
);


extern GT_CHAR * CPSS_IF_2_STR
(
    CPSS_PORT_INTERFACE_MODE_ENT ifEnm
);




extern GT_CHAR * CPSS_MAPPING_2_STR
(
    CPSS_DXCH_PORT_MAPPING_TYPE_ENT mapEnm
);


extern GT_CHAR * RXDMA_IfWidth_2_STR
(
    IN GT_U32 RXDMA_IfWidth
);


extern GT_CHAR * TX_FIFO_IfWidth_2_STR
(
    IN GT_U32 TX_FIFO_IfWidth
);

extern GT_CHAR * CPSS_SPEED_2_STR
(
    CPSS_PORT_SPEED_ENT speed
);


extern GT_STATUS appDemoBobKSimCoreClockSet
(
    GT_U8  devNum,
    GT_U32 coreClock
);



/* 
    API to configure TmPorts, with non default ports mapping
*/
extern void appDemoBc2SetCustomPortsMapping(GT_BOOL enable);
extern GT_STATUS appDemoBc2TmPortsInit(GT_U8 dev);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef WIN32
    #include <stdio.h>
    #define cpssOsPrintf printf
#endif

extern GT_STATUS appDemoBobKSimCoreClockSet
(
    GT_U8  devNum,
    GT_U32 coreClock
);

GT_STATUS bobkStart
(
    GT_U32 coreClock
)
{
    GT_STATUS rc;

    #ifdef ASIC_SIMULATION
    {
        GT_U8 dev = 0; 

        rc = appDemoBobKSimCoreClockSet(dev,coreClock);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    #endif

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


typedef struct 
{
    GT_U32   idxInExcel;
    GT_CHAR *unitName;
    GT_U32   regAddr;
    GT_U32   data;
    GT_CHAR *regName;
    GT_CHAR *cider;
}UnitRegList_STC;


typedef struct 
{
    GT_U32                  coreClock;
    GT_U32                  initSystemType;
    GT_BOOL                 tmEnable;
    PortInitList_STC       *portInitlistPtr;
    CPSS_DXCH_PORT_MAP_STC *mapPtr;
    UnitRegList_STC        *regList2Print;
}APPDEMO_BOBK_SCENARIO_DATA_STC;

typedef struct 
{
    GT_U32                          scenarioId;
    APPDEMO_BOBK_SCENARIO_DATA_STC  scenarioData;
}APPDEMO_BOBK_SCENARIO_STC;

#ifdef WIN32
    #define cpssOsPrintf printf
#endif

#define BAD_VALUE (GT_U32)(~0)


UnitRegList_STC cetus_wo_TM_RegList_M4[] = 
{
    /*                                unit_name,     address,       data0,  reg_name */
      {    2 , "MG Global registers SDMA and TWSI registers" , 0x0000020c , 0x00000e04 , "Base Address %n                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Decoding/" }
    , {    3 , "MG Global registers SDMA and TWSI registers" , 0x00000204 , 0x000000e4 , "Unit Default ID (UDID)                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Decoding/" }
    , {    4 , "MG Global registers SDMA and TWSI registers" , 0x00000254 , 0x00000006 , "Window Control Register%n                                   " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Decoding/" }
    , {    5 , "MG Global registers SDMA and TWSI registers" , 0x00000210 , 0xffff0000 , "Size (S) %n                                                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Decoding/" }
    , {    6 , "MG Global registers SDMA and TWSI registers" , 0x00002700 , 0x00000000 , "Tx SDMA Token-Bucket Queue<%n> Counter                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {    7 , "MG Global registers SDMA and TWSI registers" , 0x00002704 , 0x00000000 , "Tx SDMA Token Bucket Queue<%n> Configuration                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {    8 , "MG Global registers SDMA and TWSI registers" , 0x00002708 , 0x00000000 , "Transmit SDMA Weighted-Round-Robin Queue<%n> Configuration  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {    9 , "MG Global registers SDMA and TWSI registers" , 0x00002710 , 0x00000000 , "Tx SDMA Token-Bucket Queue<%n> Counter                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   10 , "MG Global registers SDMA and TWSI registers" , 0x00002714 , 0x00000000 , "Tx SDMA Token Bucket Queue<%n> Configuration                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   11 , "MG Global registers SDMA and TWSI registers" , 0x00002718 , 0x00000000 , "Transmit SDMA Weighted-Round-Robin Queue<%n> Configuration  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   12 , "MG Global registers SDMA and TWSI registers" , 0x00002720 , 0x00000000 , "Tx SDMA Token-Bucket Queue<%n> Counter                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   13 , "MG Global registers SDMA and TWSI registers" , 0x00002724 , 0x00000000 , "Tx SDMA Token Bucket Queue<%n> Configuration                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   14 , "MG Global registers SDMA and TWSI registers" , 0x00002728 , 0x00000000 , "Transmit SDMA Weighted-Round-Robin Queue<%n> Configuration  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   15 , "MG Global registers SDMA and TWSI registers" , 0x00002730 , 0x00000000 , "Tx SDMA Token-Bucket Queue<%n> Counter                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   16 , "MG Global registers SDMA and TWSI registers" , 0x00002734 , 0x00000000 , "Tx SDMA Token Bucket Queue<%n> Configuration                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   17 , "MG Global registers SDMA and TWSI registers" , 0x00002738 , 0x00000000 , "Transmit SDMA Weighted-Round-Robin Queue<%n> Configuration  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   18 , "MG Global registers SDMA and TWSI registers" , 0x00002740 , 0x00000000 , "Tx SDMA Token-Bucket Queue<%n> Counter                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   19 , "MG Global registers SDMA and TWSI registers" , 0x00002744 , 0x00000000 , "Tx SDMA Token Bucket Queue<%n> Configuration                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   20 , "MG Global registers SDMA and TWSI registers" , 0x00002748 , 0x00000000 , "Transmit SDMA Weighted-Round-Robin Queue<%n> Configuration  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   21 , "MG Global registers SDMA and TWSI registers" , 0x00002750 , 0x00000000 , "Tx SDMA Token-Bucket Queue<%n> Counter                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   22 , "MG Global registers SDMA and TWSI registers" , 0x00002754 , 0x00000000 , "Tx SDMA Token Bucket Queue<%n> Configuration                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   23 , "MG Global registers SDMA and TWSI registers" , 0x00002758 , 0x00000000 , "Transmit SDMA Weighted-Round-Robin Queue<%n> Configuration  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   24 , "MG Global registers SDMA and TWSI registers" , 0x00002760 , 0x00000000 , "Tx SDMA Token-Bucket Queue<%n> Counter                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   25 , "MG Global registers SDMA and TWSI registers" , 0x00002764 , 0x00000000 , "Tx SDMA Token Bucket Queue<%n> Configuration                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   26 , "MG Global registers SDMA and TWSI registers" , 0x00002768 , 0x00000000 , "Transmit SDMA Weighted-Round-Robin Queue<%n> Configuration  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   27 , "MG Global registers SDMA and TWSI registers" , 0x00002770 , 0x00000000 , "Tx SDMA Token-Bucket Queue<%n> Counter                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   28 , "MG Global registers SDMA and TWSI registers" , 0x00002774 , 0x00000000 , "Tx SDMA Token Bucket Queue<%n> Configuration                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   29 , "MG Global registers SDMA and TWSI registers" , 0x00002778 , 0x00000000 , "Transmit SDMA Weighted-Round-Robin Queue<%n> Configuration  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   30 , "MG Global registers SDMA and TWSI registers" , 0x0000260c , 0x00000000 , "Receive SDMA Current Descriptor Pointer<%n>                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   31 , "MG Global registers SDMA and TWSI registers" , 0x0000261c , 0x00000000 , "Receive SDMA Current Descriptor Pointer<%n>                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   32 , "MG Global registers SDMA and TWSI registers" , 0x0000262c , 0x00000000 , "Receive SDMA Current Descriptor Pointer<%n>                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   33 , "MG Global registers SDMA and TWSI registers" , 0x0000263c , 0x00000000 , "Receive SDMA Current Descriptor Pointer<%n>                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   34 , "MG Global registers SDMA and TWSI registers" , 0x0000264c , 0x00000000 , "Receive SDMA Current Descriptor Pointer<%n>                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   35 , "MG Global registers SDMA and TWSI registers" , 0x0000265c , 0x00000000 , "Receive SDMA Current Descriptor Pointer<%n>                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   36 , "MG Global registers SDMA and TWSI registers" , 0x0000266c , 0x00000000 , "Receive SDMA Current Descriptor Pointer<%n>                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   37 , "MG Global registers SDMA and TWSI registers" , 0x0000267c , 0x00000000 , "Receive SDMA Current Descriptor Pointer<%n>                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   38 , "MG Global registers SDMA and TWSI registers" , 0x00002874 , 0xffffffc1 , "Transmit SDMA WRR Token Parameters                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   39 , "MG Global registers SDMA and TWSI registers" , 0x00002780 , 0x00000000 , "Tx SDMA Token-Bucket Counter                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   40 , "MG Global registers SDMA and TWSI registers" , 0x00002680 , 0x00000000 , "Receive SDMA Queue Command                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/SDMA Registers/SDMA Configuration/" }
    , {   41 , "MG Global registers SDMA and TWSI registers" , 0x000000ec , 0xff3fffff , "PHY Register 2C                                             " , "Cider/EBU/BobK/BobK {Current}/IHB Interface/HB_IP/Units/IHB_REG/IHB_REGInterchip Hopping Bus Registers/" }
    , {   42 , "MG Global registers SDMA and TWSI registers" , 0x000000e8 , 0xffffff31 , "PHY Register 28                                             " , "Cider/EBU/BobK/BobK {Current}/IHB Interface/HB_IP/Units/IHB_REG/IHB_REGInterchip Hopping Bus Registers/" }
    , {   43 , "MG Global registers SDMA and TWSI registers" , 0x000000e4 , 0xfb7fffff , "PHY Register 24                                             " , "Cider/EBU/BobK/BobK {Current}/IHB Interface/HB_IP/Units/IHB_REG/IHB_REGInterchip Hopping Bus Registers/" }
    , {   44 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000ce0 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {   45 , "TxFIFO1                                    " , 0x67000708 , 0x00000023 , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {   46 , "TxFIFO1                                    " , 0x670008a4 , 0x0bb8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   47 , "TxFIFO1                                    " , 0x6700071c , 0x0000001b , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {   48 , "TxFIFO1                                    " , 0x67000820 , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   49 , "TxFIFO1                                    " , 0x67000838 , 0xb8323130 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   50 , "TxFIFO1                                    " , 0x670006e0 , 0x0000002d , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {   51 , "TxFIFO1                                    " , 0x6700010c , 0x00000005 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {   52 , "TxFIFO1                                    " , 0x67000810 , 0x0bb8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   53 , "TxFIFO1                                    " , 0x67000840 , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   54 , "TxFIFO1                                    " , 0x67000880 , 0x3130b8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   55 , "TxFIFO1                                    " , 0x670008b4 , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   56 , "TxFIFO1                                    " , 0x67000878 , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   57 , "TxFIFO1                                    " , 0x67000888 , 0xc5b83736 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   58 , "TxFIFO1                                    " , 0x67000860 , 0x11100fb8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   59 , "TxFIFO1                                    " , 0x670008bc , 0x2322b820 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   60 , "TxFIFO1                                    " , 0x6700086c , 0xb8c7c1c0 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   61 , "TxFIFO1                                    " , 0x67000814 , 0x0fb80d0c , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   62 , "TxFIFO1                                    " , 0x670008a0 , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   63 , "TxFIFO1                                    " , 0x67000848 , 0x43b8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   64 , "TxFIFO1                                    " , 0x6700084c , 0x47b84544 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   65 , "TxFIFO1                                    " , 0x67000110 , 0x00000005 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {   66 , "TxFIFO1                                    " , 0x670008c0 , 0xa7a6a524 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   67 , "TxFIFO1                                    " , 0x67000884 , 0x3534b832 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   68 , "TxFIFO1                                    " , 0x6700082c , 0xc4c5b824 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   69 , "TxFIFO1                                    " , 0x6700080c , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   70 , "TxFIFO1                                    " , 0x67000128 , 0x00000002 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {   71 , "TxFIFO1                                    " , 0x67000864 , 0xc4c5b812 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   72 , "TxFIFO1                                    " , 0x67000108 , 0x00000005 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {   73 , "TxFIFO1                                    " , 0x67000718 , 0x00000023 , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {   74 , "TxFIFO1                                    " , 0x670000e8 , 0x00000010 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {   75 , "TxFIFO1                                    " , 0x67000124 , 0x00000005 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {   76 , "TxFIFO1                                    " , 0x670008b0 , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   77 , "TxFIFO1                                    " , 0x67000870 , 0xb8201f1e , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   78 , "TxFIFO1                                    " , 0x67000834 , 0xb8c7c1c0 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   79 , "TxFIFO1                                    " , 0x6700088c , 0xb8c3c6c4 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   80 , "TxFIFO1                                    " , 0x67000828 , 0x2322b820 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   81 , "TxFIFO1                                    " , 0x67000874 , 0xb8242322 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   82 , "TxFIFO1                                    " , 0x6700011c , 0x00000005 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {   83 , "TxFIFO1                                    " , 0x67000700 , 0x0000002b , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {   84 , "TxFIFO1                                    " , 0x67000894 , 0x454443b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   85 , "TxFIFO1                                    " , 0x67000720 , 0x0000001b , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {   86 , "TxFIFO1                                    " , 0x6700085c , 0x0d0c0bb8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   87 , "TxFIFO1                                    " , 0x67000800 , 0x000040b9 , "Pizza Arbiter Control Register                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   88 , "TxFIFO1                                    " , 0x67000844 , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   89 , "TxFIFO1                                    " , 0x670008a8 , 0x0fb80d0c , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   90 , "TxFIFO1                                    " , 0x67000898 , 0x494847b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   91 , "TxFIFO1                                    " , 0x6700083c , 0x37363534 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   92 , "TxFIFO1                                    " , 0x67000714 , 0x0000001b , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {   93 , "TxFIFO1                                    " , 0x67000818 , 0xb8121110 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   94 , "TxFIFO1                                    " , 0x67000858 , 0xc7c1c0c2 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   95 , "TxFIFO1                                    " , 0x6700070c , 0x0000001b , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {   96 , "TxFIFO1                                    " , 0x670008b8 , 0x1f1eb8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   97 , "TxFIFO1                                    " , 0x67000118 , 0x00000005 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {   98 , "TxFIFO1                                    " , 0x67000890 , 0xc7c1c0c2 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {   99 , "TxFIFO1                                    " , 0x6700081c , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  100 , "TxFIFO1                                    " , 0x67000704 , 0x0000001b , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {  101 , "TxFIFO1                                    " , 0x67000824 , 0x1f1eb8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  102 , "TxFIFO1                                    " , 0x6700087c , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  103 , "TxFIFO1                                    " , 0x67000808 , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  104 , "TxFIFO1                                    " , 0x67000710 , 0x0000002b , "SCDMA %p Shifters Configuration                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Shifters Configuration/" }
    , {  105 , "TxFIFO1                                    " , 0x670008ac , 0xb81211c8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  106 , "TxFIFO1                                    " , 0x67000114 , 0x00000005 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {  107 , "TxFIFO1                                    " , 0x67000120 , 0x00000005 , "SCDMA %p Payload Threshold                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/TxFIFO Global Configuration/" }
    , {  108 , "TxFIFO1                                    " , 0x67000830 , 0xc2b8c3c6 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  109 , "TxFIFO1                                    " , 0x67000868 , 0xc2b8c3c6 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  110 , "TxFIFO1                                    " , 0x6700089c , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  111 , "TxFIFO1                                    " , 0x67000854 , 0xb8c3c6c4 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  112 , "TxFIFO1                                    " , 0x67000850 , 0xc5b84948 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TX_FIFO/Units/<TXFIFO1_IP> TxFIFO IP Units/<Pizza Arb> TxFIFO Pizza Arb/Pizza Arbiter/" }
    , {  113 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000cc0 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  114 , "TxDMA1                                     " , 0x6600370c , 0x00018451 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  115 , "TxDMA1                                     " , 0x6600408c , 0xb8c3c6c4 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  116 , "TxDMA1                                     " , 0x660038f8 , 0x00050000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  117 , "TxDMA1                                     " , 0x66003850 , 0x00020201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  118 , "TxDMA1                                     " , 0x660038ac , 0x00006815 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  119 , "TxDMA1                                     " , 0x660038f0 , 0x00020201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  120 , "TxDMA1                                     " , 0x66004044 , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  121 , "TxDMA1                                     " , 0x6600382c , 0x00006815 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  122 , "TxDMA1                                     " , 0x66003838 , 0x00050000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  123 , "TxDMA1                                     " , 0x66003810 , 0x00020201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  124 , "TxDMA1                                     " , 0x660040bc , 0x2322b820 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  125 , "TxDMA1                                     " , 0x6600404c , 0x47b84544 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  126 , "TxDMA1                                     " , 0x6600403c , 0x37363534 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  127 , "TxDMA1                                     " , 0x660038cc , 0x00006815 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  128 , "TxDMA1                                     " , 0x66004074 , 0xb8242322 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  129 , "TxDMA1                                     " , 0x660038d0 , 0x00020201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  130 , "TxDMA1                                     " , 0x66004084 , 0x3534b832 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  131 , "TxDMA1                                     " , 0x66004064 , 0xc4c5b812 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  132 , "TxDMA1                                     " , 0x66004020 , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  133 , "TxDMA1                                     " , 0x66004098 , 0x494847b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  134 , "TxDMA1                                     " , 0x660040b0 , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  135 , "TxDMA1                                     " , 0x66004030 , 0xc2b8c3c6 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  136 , "TxDMA1                                     " , 0x66003700 , 0x00000013 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  137 , "TxDMA1                                     " , 0x6600384c , 0x00006815 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  138 , "TxDMA1                                     " , 0x6600388c , 0x00006815 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  139 , "TxDMA1                                     " , 0x660038b8 , 0x00050000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  140 , "TxDMA1                                     " , 0x66004034 , 0xb8c7c1c0 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  141 , "TxDMA1                                     " , 0x6600386c , 0x00006815 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  142 , "TxDMA1                                     " , 0x66004058 , 0xc7c1c0c2 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  143 , "TxDMA1                                     " , 0x66003710 , 0x00060201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  144 , "TxDMA1                                     " , 0x660040b8 , 0x1f1eb8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  145 , "TxDMA1                                     " , 0x6600405c , 0x0d0c0bb8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  146 , "TxDMA1                                     " , 0x66004040 , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  147 , "TxDMA1                                     " , 0x66003900 , 0x00000003 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  148 , "TxDMA1                                     " , 0x660038c0 , 0x00000007 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  149 , "TxDMA1                                     " , 0x66004060 , 0x11100fb8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  150 , "TxDMA1                                     " , 0x6600400c , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  151 , "TxDMA1                                     " , 0x66004000 , 0x000040b9 , "Pizza Arbiter Control Register                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  152 , "TxDMA1                                     " , 0x66004078 , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  153 , "TxDMA1                                     " , 0x66004094 , 0x454443b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  154 , "TxDMA1                                     " , 0x66004080 , 0x3130b8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  155 , "TxDMA1                                     " , 0x66003918 , 0x00020000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  156 , "TxDMA1                                     " , 0x66003718 , 0x000c0000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  157 , "TxDMA1                                     " , 0x6600401c , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  158 , "TxDMA1                                     " , 0x660038a0 , 0x00000007 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  159 , "TxDMA1                                     " , 0x66004038 , 0xb8323130 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  160 , "TxDMA1                                     " , 0x6600407c , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  161 , "TxDMA1                                     " , 0x66004010 , 0x0bb8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  162 , "TxDMA1                                     " , 0x6600406c , 0xb8c7c1c0 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  163 , "TxDMA1                                     " , 0x660040a4 , 0x0bb8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  164 , "TxDMA1                                     " , 0x6600409c , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  165 , "TxDMA1                                     " , 0x660040ac , 0xb81211c8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  166 , "TxDMA1                                     " , 0x66003860 , 0x00000007 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  167 , "TxDMA1                                     " , 0x66003840 , 0x00000007 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  168 , "TxDMA1                                     " , 0x660038d8 , 0x00050000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  169 , "TxDMA1                                     " , 0x66003800 , 0x00000007 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  170 , "TxDMA1                                     " , 0x66003878 , 0x00050000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  171 , "TxDMA1                                     " , 0x660038b0 , 0x00020201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  172 , "TxDMA1                                     " , 0x66003890 , 0x00020201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  173 , "TxDMA1                                     " , 0x66004090 , 0xc7c1c0c2 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  174 , "TxDMA1                                     " , 0x660038ec , 0x00006815 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  175 , "TxDMA1                                     " , 0x66003858 , 0x00050000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  176 , "TxDMA1                                     " , 0x6600390c , 0x00001403 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  177 , "TxDMA1                                     " , 0x66004024 , 0x1f1eb8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  178 , "TxDMA1                                     " , 0x66004088 , 0xc5b83736 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  179 , "TxDMA1                                     " , 0x66004018 , 0xb8121110 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  180 , "TxDMA1                                     " , 0x660040a8 , 0x0fb80d0c , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  181 , "TxDMA1                                     " , 0x66003820 , 0x00000007 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  182 , "TxDMA1                                     " , 0x660040b4 , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  183 , "TxDMA1                                     " , 0x6600380c , 0x00006815 , "TxFIFO Counters Configurations SCDMA %p                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  184 , "TxDMA1                                     " , 0x660038e0 , 0x00000007 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  185 , "TxDMA1                                     " , 0x66003818 , 0x00050000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  186 , "TxDMA1                                     " , 0x66004054 , 0xb8c3c6c4 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  187 , "TxDMA1                                     " , 0x66003870 , 0x00020201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  188 , "TxDMA1                                     " , 0x66003830 , 0x00020201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  189 , "TxDMA1                                     " , 0x66004014 , 0x0fb80d0c , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  190 , "TxDMA1                                     " , 0x66003898 , 0x00050000 , "Burst Limiter SCDMA %p                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  191 , "TxDMA1                                     " , 0x6600402c , 0xc4c5b824 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  192 , "TxDMA1                                     " , 0x66004068 , 0xc2b8c3c6 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  193 , "TxDMA1                                     " , 0x66004008 , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  194 , "TxDMA1                                     " , 0x66004048 , 0x43b8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  195 , "TxDMA1                                     " , 0x66004028 , 0x2322b820 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  196 , "TxDMA1                                     " , 0x66004050 , 0xc5b84948 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  197 , "TxDMA1                                     " , 0x660040c0 , 0xa7a6a524 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  198 , "TxDMA1                                     " , 0x66003880 , 0x00000007 , "FIFOs Thresholds Configurations SCDMA %p Reg 1              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  199 , "TxDMA1                                     " , 0x660040a0 , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  200 , "TxDMA1                                     " , 0x66003910 , 0x00000201 , "SCDMA %p Configurations Reg 1                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/TxDMA Per SCDMA Configurations/" }
    , {  201 , "TxDMA1                                     " , 0x66004070 , 0xb8201f1e , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TXDMA/Units/<TXDMA1_IP> TxDMA IP Units/<Pizza Arb> TxDMA Pizza Arb/Pizza Arbiter/" }
    , {  202 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000d00 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  203 , "RxDMA1                                     " , 0x6800091c , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  204 , "RxDMA1                                     " , 0x68002a60 , 0x121110b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  205 , "RxDMA1                                     " , 0x68000a64 , 0x00000045 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  206 , "RxDMA1                                     " , 0x68002a68 , 0xc2b8c3c6 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  207 , "RxDMA1                                     " , 0x680009cc , 0x0000001f , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  208 , "RxDMA1                                     " , 0x68000a30 , 0x00000038 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  209 , "RxDMA1                                     " , 0x68000980 , 0x0000000c , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  210 , "RxDMA1                                     " , 0x68000a48 , 0x00000052 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  211 , "RxDMA1                                     " , 0x68002a34 , 0xb8c7c1c0 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  212 , "RxDMA1                                     " , 0x6800096c , 0x00000007 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  213 , "RxDMA1                                     " , 0x680009c0 , 0x0000001c , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  214 , "RxDMA1                                     " , 0x68002a14 , 0x0fb80d0c , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  215 , "RxDMA1                                     " , 0x68002a00 , 0x000040b9 , "Pizza Arbiter Control Register                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  216 , "RxDMA1                                     " , 0x68000918 , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  217 , "RxDMA1                                     " , 0x68002a20 , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  218 , "RxDMA1                                     " , 0x68000a40 , 0x00000050 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  219 , "RxDMA1                                     " , 0x68002a8c , 0xb8c3c6c4 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  220 , "RxDMA1                                     " , 0x68000a18 , 0x00000032 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  221 , "RxDMA1                                     " , 0x68002a58 , 0xc7c1c0c2 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  222 , "RxDMA1                                     " , 0x68000954 , 0x00000001 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  223 , "RxDMA1                                     " , 0x68000960 , 0x00000004 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  224 , "RxDMA1                                     " , 0x68000a2c , 0x00000037 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  225 , "RxDMA1                                     " , 0x68002aa0 , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  226 , "RxDMA1                                     " , 0x68000990 , 0x00000010 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  227 , "RxDMA1                                     " , 0x680009f0 , 0x00000028 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  228 , "RxDMA1                                     " , 0x68000a54 , 0x00000041 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  229 , "RxDMA1                                     " , 0x68002a44 , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  230 , "RxDMA1                                     " , 0x68000998 , 0x00000012 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  231 , "RxDMA1                                     " , 0x68002aa4 , 0x0db8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  232 , "RxDMA1                                     " , 0x68002a3c , 0x37363534 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  233 , "RxDMA1                                     " , 0x68002a80 , 0x3231b8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  234 , "RxDMA1                                     " , 0x68000a60 , 0x00000044 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  235 , "RxDMA1                                     " , 0x68002a0c , 0xc0c2b8c3 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  236 , "RxDMA1                                     " , 0x68000984 , 0x0000000d , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  237 , "RxDMA1                                     " , 0x68002a4c , 0x47b84544 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  238 , "RxDMA1                                     " , 0x68000a34 , 0x00000039 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  239 , "RxDMA1                                     " , 0x68002a1c , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  240 , "RxDMA1                                     " , 0x68000a14 , 0x00000031 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  241 , "RxDMA1                                     " , 0x68002a28 , 0x2322b820 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  242 , "RxDMA1                                     " , 0x680009ec , 0x00000027 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  243 , "RxDMA1                                     " , 0x680009c4 , 0x0000001d , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  244 , "RxDMA1                                     " , 0x68000904 , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  245 , "RxDMA1                                     " , 0x68002ab0 , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  246 , "RxDMA1                                     " , 0x68000988 , 0x0000000e , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  247 , "RxDMA1                                     " , 0x68000994 , 0x00000011 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  248 , "RxDMA1                                     " , 0x680009b0 , 0x00000018 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  249 , "RxDMA1                                     " , 0x68002a90 , 0xc7c1c0c2 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  250 , "RxDMA1                                     " , 0x68002a48 , 0x43b8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  251 , "RxDMA1                                     " , 0x680009f8 , 0x0000002a , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  252 , "RxDMA1                                     " , 0x68002a74 , 0xb8252423 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  253 , "RxDMA1                                     " , 0x68002ab4 , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  254 , "RxDMA1                                     " , 0x68000a70 , 0x0000003f , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  255 , "RxDMA1                                     " , 0x68002a84 , 0x3635b833 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  256 , "RxDMA1                                     " , 0x6800095c , 0x00000003 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  257 , "RxDMA1                                     " , 0x68002a10 , 0x0bb8c7c1 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  258 , "RxDMA1                                     " , 0x680009fc , 0x0000002b , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  259 , "RxDMA1                                     " , 0x68002a38 , 0xb8323130 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  260 , "RxDMA1                                     " , 0x68002a94 , 0x464544b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  261 , "RxDMA1                                     " , 0x68002aac , 0xb81413c8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  262 , "RxDMA1                                     " , 0x680009e4 , 0x00000025 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  263 , "RxDMA1                                     " , 0x68000a10 , 0x00000030 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  264 , "RxDMA1                                     " , 0x68000a04 , 0x0000002d , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  265 , "RxDMA1                                     " , 0x68002a70 , 0xb821201f , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  266 , "RxDMA1                                     " , 0x680009e8 , 0x00000026 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  267 , "RxDMA1                                     " , 0x68002a24 , 0x1f1eb8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  268 , "RxDMA1                                     " , 0x68002a5c , 0x0e0d0cb8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  269 , "RxDMA1                                     " , 0x680009c8 , 0x0000001e , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  270 , "RxDMA1                                     " , 0x68002a08 , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  271 , "RxDMA1                                     " , 0x68000914 , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  272 , "RxDMA1                                     " , 0x68000a24 , 0x00000035 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  273 , "RxDMA1                                     " , 0x68002a2c , 0xc4c5b824 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  274 , "RxDMA1                                     " , 0x68000950 , 0x00000000 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  275 , "RxDMA1                                     " , 0x680009a0 , 0x00000014 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  276 , "RxDMA1                                     " , 0x680009b4 , 0x00000019 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  277 , "RxDMA1                                     " , 0x6800098c , 0x0000000f , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  278 , "RxDMA1                                     " , 0x68002abc , 0x2524b822 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  279 , "RxDMA1                                     " , 0x68000a58 , 0x00000042 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  280 , "RxDMA1                                     " , 0x68000910 , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  281 , "RxDMA1                                     " , 0x68002a78 , 0xc3c6c4c5 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  282 , "RxDMA1                                     " , 0x680009d8 , 0x00000022 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  283 , "RxDMA1                                     " , 0x68000a4c , 0x00000053 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  284 , "RxDMA1                                     " , 0x68002a18 , 0xb8121110 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  285 , "RxDMA1                                     " , 0x680009f4 , 0x00000029 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  286 , "RxDMA1                                     " , 0x68000a08 , 0x0000002e , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  287 , "RxDMA1                                     " , 0x68000970 , 0x00000008 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  288 , "RxDMA1                                     " , 0x68000a00 , 0x0000002c , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  289 , "RxDMA1                                     " , 0x68002ac0 , 0xa9a8a726 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  290 , "RxDMA1                                     " , 0x68000a6c , 0x00000047 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  291 , "RxDMA1                                     " , 0x68002a50 , 0xc5b80048 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  292 , "RxDMA1                                     " , 0x680009bc , 0x0000001b , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  293 , "RxDMA1                                     " , 0x680009a4 , 0x00000015 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  294 , "RxDMA1                                     " , 0x68000a38 , 0x0000003a , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  295 , "RxDMA1                                     " , 0x68000a68 , 0x00000046 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  296 , "RxDMA1                                     " , 0x68002a64 , 0xc4c5b813 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  297 , "RxDMA1                                     " , 0x680009d4 , 0x00000021 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  298 , "RxDMA1                                     " , 0x68002a98 , 0x010048b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  299 , "RxDMA1                                     " , 0x68000a28 , 0x00000036 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  300 , "RxDMA1                                     " , 0x68000908 , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  301 , "RxDMA1                                     " , 0x68000a3c , 0x0000004f , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  302 , "RxDMA1                                     " , 0x680009e0 , 0x00000024 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  303 , "RxDMA1                                     " , 0x68002aa8 , 0x11b80f0e , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  304 , "RxDMA1                                     " , 0x68000924 , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  305 , "RxDMA1                                     " , 0x680009d0 , 0x00000020 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  306 , "RxDMA1                                     " , 0x6800097c , 0x0000000b , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  307 , "RxDMA1                                     " , 0x68002a30 , 0xc2b8c3c6 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  308 , "RxDMA1                                     " , 0x68000a20 , 0x00000034 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  309 , "RxDMA1                                     " , 0x680008e4 , 0x0ffff002 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  310 , "RxDMA1                                     " , 0x68000978 , 0x0000000a , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  311 , "RxDMA1                                     " , 0x6800090c , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  312 , "RxDMA1                                     " , 0x68000920 , 0x0ffff000 , "SCDMA %n Configuration 0                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  313 , "RxDMA1                                     " , 0x68002a9c , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  314 , "RxDMA1                                     " , 0x68000a44 , 0x00000051 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  315 , "RxDMA1                                     " , 0x680009dc , 0x00000023 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  316 , "RxDMA1                                     " , 0x68000a1c , 0x00000033 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  317 , "RxDMA1                                     " , 0x68002ab8 , 0x2120b8c7 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  318 , "RxDMA1                                     " , 0x68000968 , 0x00000006 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  319 , "RxDMA1                                     " , 0x68000964 , 0x00000005 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  320 , "RxDMA1                                     " , 0x680009b8 , 0x0000001a , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  321 , "RxDMA1                                     " , 0x68002a54 , 0xb8c3c6c4 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  322 , "RxDMA1                                     " , 0x68000974 , 0x00000009 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  323 , "RxDMA1                                     " , 0x680009ac , 0x00000017 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  324 , "RxDMA1                                     " , 0x68000958 , 0x00000002 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  325 , "RxDMA1                                     " , 0x68000a0c , 0x0000002f , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  326 , "RxDMA1                                     " , 0x68002a88 , 0xc5b83837 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  327 , "RxDMA1                                     " , 0x68002a6c , 0xb8c7c1c0 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  328 , "RxDMA1                                     " , 0x68000a50 , 0x00000040 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  329 , "RxDMA1                                     " , 0x68002a40 , 0xc6c4c5b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  330 , "RxDMA1                                     " , 0x68000a5c , 0x00000043 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  331 , "RxDMA1                                     " , 0x68002a7c , 0xc1c0c2b8 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/<Pizza Arbiter> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  332 , "RxDMA1                                     " , 0x680009a8 , 0x00000016 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  333 , "RxDMA1                                     " , 0x6800099c , 0x00000013 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  334 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000c80 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  335 , "TxDMA Arbiter                              " , 0x64000008 , 0x000002ef , "Pizza Arbiter Configuration Register <%n>                   " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxDMA Glue/Units/TxDMA Glue/" }
    , {  336 , "TxDMA Arbiter                              " , 0x64000000 , 0x0000bf01 , "Pizza Arbiter Control Register                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxDMA Glue/Units/TxDMA Glue/" }
    , {  337 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x000008e0 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  338 , "MPPM                                       " , 0x47000274 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  339 , "MPPM                                       " , 0x4700026c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  340 , "MPPM                                       " , 0x47000200 , 0x000040f2 , "Pizza Arbiter Control Register                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  341 , "MPPM                                       " , 0x470002a0 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  342 , "MPPM                                       " , 0x47000210 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  343 , "MPPM                                       " , 0x4700024c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  344 , "MPPM                                       " , 0x470002dc , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  345 , "MPPM                                       " , 0x470002f8 , 0x83828381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  346 , "MPPM                                       " , 0x47000240 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  347 , "MPPM                                       " , 0x470002ec , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  348 , "MPPM                                       " , 0x470002c0 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  349 , "MPPM                                       " , 0x47000220 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  350 , "MPPM                                       " , 0x47000208 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  351 , "MPPM                                       " , 0x470002f4 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  352 , "MPPM                                       " , 0x47000264 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  353 , "MPPM                                       " , 0x470002b4 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  354 , "MPPM                                       " , 0x470002ac , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  355 , "MPPM                                       " , 0x4700022c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  356 , "MPPM                                       " , 0x47000294 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  357 , "MPPM                                       " , 0x47000234 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  358 , "MPPM                                       " , 0x4700020c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  359 , "MPPM                                       " , 0x4700021c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  360 , "MPPM                                       " , 0x470002c4 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  361 , "MPPM                                       " , 0x470002e0 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  362 , "MPPM                                       " , 0x47000268 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  363 , "MPPM                                       " , 0x470002cc , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  364 , "MPPM                                       " , 0x47000284 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  365 , "MPPM                                       " , 0x47000254 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  366 , "MPPM                                       " , 0x470002b0 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  367 , "MPPM                                       " , 0x47000228 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  368 , "MPPM                                       " , 0x4700029c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  369 , "MPPM                                       " , 0x47000290 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  370 , "MPPM                                       " , 0x4700023c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  371 , "MPPM                                       " , 0x47000224 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  372 , "MPPM                                       " , 0x470002b8 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  373 , "MPPM                                       " , 0x47000288 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  374 , "MPPM                                       " , 0x470002a8 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  375 , "MPPM                                       " , 0x4700025c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  376 , "MPPM                                       " , 0x470002d0 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  377 , "MPPM                                       " , 0x470002c8 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  378 , "MPPM                                       " , 0x47000244 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  379 , "MPPM                                       " , 0x470002d4 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  380 , "MPPM                                       " , 0x470002e8 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  381 , "MPPM                                       " , 0x47000214 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  382 , "MPPM                                       " , 0x47000258 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  383 , "MPPM                                       " , 0x47000298 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  384 , "MPPM                                       " , 0x47000250 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  385 , "MPPM                                       " , 0x4700027c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  386 , "MPPM                                       " , 0x470002f0 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  387 , "MPPM                                       " , 0x47000238 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  388 , "MPPM                                       " , 0x470002bc , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  389 , "MPPM                                       " , 0x47000248 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  390 , "MPPM                                       " , 0x4700028c , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  391 , "MPPM                                       " , 0x470002d8 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  392 , "MPPM                                       " , 0x47000270 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  393 , "MPPM                                       " , 0x47000278 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  394 , "MPPM                                       " , 0x470002a4 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  395 , "MPPM                                       " , 0x47000260 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  396 , "MPPM                                       " , 0x47000280 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  397 , "MPPM                                       " , 0x47000230 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  398 , "MPPM                                       " , 0x470002e4 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  399 , "MPPM                                       " , 0x47000218 , 0x83818381 , "Pizza Arbiter Configuration Register %n                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/" }
    , {  400 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000c60 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  401 , "RxDMA Arbiter                              " , 0x63000008 , 0x000002ef , "Pizza Arbiter Configuration Register <%n>                   " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RxDMA Glue/Units/RxDMA Glue Pizza Arbiter/" }
    , {  402 , "RxDMA Arbiter                              " , 0x63000000 , 0x0000bf01 , "Pizza Arbiter Control Register                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RxDMA Glue/Units/RxDMA Glue Pizza Arbiter/" }
    , {  403 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000800 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  404 , "TXQ_DQ                                     " , 0x400016a0 , 0xc400b8c5 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  405 , "TXQ_DQ                                     " , 0x40001680 , 0xc600b8c0 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  406 , "TXQ_DQ                                     " , 0x40001a0c , 0x00000002 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  407 , "TXQ_DQ                                     " , 0x400016f0 , 0xc700c600 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  408 , "TXQ_DQ                                     " , 0x4000035c , 0x00000027 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  409 , "TXQ_DQ                                     " , 0x400002f0 , 0x0000000c , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  410 , "TXQ_DQ                                     " , 0x400016ac , 0xc1b800c3 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  411 , "TXQ_DQ                                     " , 0x400003b4 , 0x0000003d , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  412 , "TXQ_DQ                                     " , 0x4000032c , 0x0000001b , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  413 , "TXQ_DQ                                     " , 0x400019c8 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  414 , "TXQ_DQ                                     " , 0x4000034c , 0x00000023 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  415 , "TXQ_DQ                                     " , 0x40000324 , 0x00000019 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  416 , "TXQ_DQ                                     " , 0x40001a08 , 0x00000003 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  417 , "TXQ_DQ                                     " , 0x40000390 , 0x00000034 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  418 , "TXQ_DQ                                     " , 0x400002e0 , 0x00000008 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  419 , "TXQ_DQ                                     " , 0x40001934 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  420 , "TXQ_DQ                                     " , 0x40001668 , 0x00b8c500 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  421 , "TXQ_DQ                                     " , 0x400019d0 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  422 , "TXQ_DQ                                     " , 0x400002c4 , 0x00000001 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  423 , "TXQ_DQ                                     " , 0x4000036c , 0x0000002b , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  424 , "TXQ_DQ                                     " , 0x40001a24 , 0x00000002 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  425 , "TXQ_DQ                                     " , 0x400019c4 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  426 , "TXQ_DQ                                     " , 0x400019f0 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  427 , "TXQ_DQ                                     " , 0x40001610 , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  428 , "TXQ_DQ                                     " , 0x400019a8 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  429 , "TXQ_DQ                                     " , 0x40001988 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  430 , "TXQ_DQ                                     " , 0x4000166c , 0x00b8c0c4 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  431 , "TXQ_DQ                                     " , 0x40001924 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  432 , "TXQ_DQ                                     " , 0x4000195c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  433 , "TXQ_DQ                                     " , 0x40001704 , 0x00b8c7c6 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  434 , "TXQ_DQ                                     " , 0x400019d8 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  435 , "TXQ_DQ                                     " , 0x4000160c , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  436 , "TXQ_DQ                                     " , 0x40001a14 , 0x00000002 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  437 , "TXQ_DQ                                     " , 0x400016c8 , 0x00b8c0c4 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  438 , "TXQ_DQ                                     " , 0x4000030c , 0x00000013 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  439 , "TXQ_DQ                                     " , 0x40000300 , 0x00000010 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  440 , "TXQ_DQ                                     " , 0x400016ec , 0xb8c000c4 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  441 , "TXQ_DQ                                     " , 0x40000368 , 0x0000002a , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  442 , "TXQ_DQ                                     " , 0x40001a28 , 0x00000002 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  443 , "TXQ_DQ                                     " , 0x40000000 , 0x00000541 , "Global Dequeue Configuration                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  444 , "TXQ_DQ                                     " , 0x4000033c , 0x0000001f , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  445 , "TXQ_DQ                                     " , 0x4000168c , 0xb800c5c2 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  446 , "TXQ_DQ                                     " , 0x400002d4 , 0x00000005 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  447 , "TXQ_DQ                                     " , 0x400002f8 , 0x0000000e , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  448 , "TXQ_DQ                                     " , 0x400003a8 , 0x0000003a , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  449 , "TXQ_DQ                                     " , 0x40001954 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  450 , "TXQ_DQ                                     " , 0x40001914 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  451 , "TXQ_DQ                                     " , 0x40000380 , 0x00000030 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  452 , "TXQ_DQ                                     " , 0x400003bc , 0x00000048 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  453 , "TXQ_DQ                                     " , 0x400003dc , 0x00000047 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  454 , "TXQ_DQ                                     " , 0x400019a4 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  455 , "TXQ_DQ                                     " , 0x40001684 , 0xc300b8c7 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  456 , "TXQ_DQ                                     " , 0x40001944 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  457 , "TXQ_DQ                                     " , 0x4000165c , 0x00c7c600 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  458 , "TXQ_DQ                                     " , 0x4000193c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  459 , "TXQ_DQ                                     " , 0x40001674 , 0xb800c300 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  460 , "TXQ_DQ                                     " , 0x40000344 , 0x00000021 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  461 , "TXQ_DQ                                     " , 0x4000037c , 0x0000002f , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  462 , "TXQ_DQ                                     " , 0x400003b8 , 0x0000003e , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  463 , "TXQ_DQ                                     " , 0x40001974 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  464 , "TXQ_DQ                                     " , 0x400002d8 , 0x00000006 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  465 , "TXQ_DQ                                     " , 0x40000330 , 0x0000001c , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  466 , "TXQ_DQ                                     " , 0x400015f0 , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  467 , "TXQ_DQ                                     " , 0x40001628 , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  468 , "TXQ_DQ                                     " , 0x400003ac , 0x0000003b , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  469 , "TXQ_DQ                                     " , 0x40001910 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  470 , "TXQ_DQ                                     " , 0x40001660 , 0xbf00c3b8 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  471 , "TXQ_DQ                                     " , 0x400016f4 , 0xb8c300b8 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  472 , "TXQ_DQ                                     " , 0x4000167c , 0x00c400b8 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  473 , "TXQ_DQ                                     " , 0x4000100c , 0x00000381 , "High Speed Port %i                                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Scheduler Configuration/" }
    , {  474 , "TXQ_DQ                                     " , 0x40001930 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  475 , "TXQ_DQ                                     " , 0x40000364 , 0x00000029 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  476 , "TXQ_DQ                                     " , 0x40001a20 , 0x00000002 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  477 , "TXQ_DQ                                     " , 0x400019dc , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  478 , "TXQ_DQ                                     " , 0x400003c8 , 0x00000042 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  479 , "TXQ_DQ                                     " , 0x4000169c , 0x00c2c100 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  480 , "TXQ_DQ                                     " , 0x40000360 , 0x00000028 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  481 , "TXQ_DQ                                     " , 0x40001614 , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  482 , "TXQ_DQ                                     " , 0x40001708 , 0xabaaa9c3 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  483 , "TXQ_DQ                                     " , 0x40001958 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  484 , "TXQ_DQ                                     " , 0x4000039c , 0x00000037 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  485 , "TXQ_DQ                                     " , 0x400016c0 , 0xc2c100b8 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  486 , "TXQ_DQ                                     " , 0x400002dc , 0x00000007 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  487 , "TXQ_DQ                                     " , 0x400003a0 , 0x00000038 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  488 , "TXQ_DQ                                     " , 0x400016e0 , 0xc300b8c7 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  489 , "TXQ_DQ                                     " , 0x40000334 , 0x0000001d , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  490 , "TXQ_DQ                                     " , 0x400016d0 , 0xb800c300 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  491 , "TXQ_DQ                                     " , 0x400003d4 , 0x00000045 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  492 , "TXQ_DQ                                     " , 0x40000340 , 0x00000020 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  493 , "TXQ_DQ                                     " , 0x400019fc , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  494 , "TXQ_DQ                                     " , 0x40000314 , 0x00000015 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  495 , "TXQ_DQ                                     " , 0x40001a18 , 0x00000002 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  496 , "TXQ_DQ                                     " , 0x40001938 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  497 , "TXQ_DQ                                     " , 0x40001690 , 0xb8c000c4 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  498 , "TXQ_DQ                                     " , 0x4000199c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  499 , "TXQ_DQ                                     " , 0x400002f4 , 0x0000000d , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  500 , "TXQ_DQ                                     " , 0x400003b0 , 0x0000003c , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  501 , "TXQ_DQ                                     " , 0x400019a0 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  502 , "TXQ_DQ                                     " , 0x400016bc , 0x00c3b800 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  503 , "TXQ_DQ                                     " , 0x4000194c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  504 , "TXQ_DQ                                     " , 0x40000304 , 0x00000011 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  505 , "TXQ_DQ                                     " , 0x40001664 , 0xc2c100b8 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  506 , "TXQ_DQ                                     " , 0x40001994 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  507 , "TXQ_DQ                                     " , 0x40000350 , 0x00000024 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  508 , "TXQ_DQ                                     " , 0x400003cc , 0x00000043 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  509 , "TXQ_DQ                                     " , 0x400019b8 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  510 , "TXQ_DQ                                     " , 0x40001624 , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  511 , "TXQ_DQ                                     " , 0x400019ec , 0x00000001 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  512 , "TXQ_DQ                                     " , 0x4000190c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  513 , "TXQ_DQ                                     " , 0x4000196c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  514 , "TXQ_DQ                                     " , 0x400019b4 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  515 , "TXQ_DQ                                     " , 0x400019f8 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  516 , "TXQ_DQ                                     " , 0x40000354 , 0x00000025 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  517 , "TXQ_DQ                                     " , 0x40000348 , 0x00000022 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  518 , "TXQ_DQ                                     " , 0x400016b4 , 0xc000c400 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  519 , "TXQ_DQ                                     " , 0x40001940 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  520 , "TXQ_DQ                                     " , 0x40001678 , 0xc5c200c1 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  521 , "TXQ_DQ                                     " , 0x400019c0 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  522 , "TXQ_DQ                                     " , 0x400002e8 , 0x0000000a , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  523 , "TXQ_DQ                                     " , 0x40001904 , 0x03030303 , "Port Request Mask                                           " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  524 , "TXQ_DQ                                     " , 0x40000310 , 0x00000014 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  525 , "TXQ_DQ                                     " , 0x40000378 , 0x0000002e , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  526 , "TXQ_DQ                                     " , 0x400016d8 , 0x00c400b8 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  527 , "TXQ_DQ                                     " , 0x40000338 , 0x0000001e , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  528 , "TXQ_DQ                                     " , 0x400016a4 , 0x00b8c000 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  529 , "TXQ_DQ                                     " , 0x40001920 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  530 , "TXQ_DQ                                     " , 0x400003a4 , 0x00000039 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  531 , "TXQ_DQ                                     " , 0x40001960 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  532 , "TXQ_DQ                                     " , 0x40000374 , 0x0000002d , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  533 , "TXQ_DQ                                     " , 0x4000162c , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  534 , "TXQ_DQ                                     " , 0x40001978 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  535 , "TXQ_DQ                                     " , 0x400016e8 , 0xb800c5c2 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  536 , "TXQ_DQ                                     " , 0x4000198c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  537 , "TXQ_DQ                                     " , 0x40001654 , 0x00b8c5c2 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  538 , "TXQ_DQ                                     " , 0x40001500 , 0x000016b9 , "Ports Arbiter Configuration                                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  539 , "TXQ_DQ                                     " , 0x40001984 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  540 , "TXQ_DQ                                     " , 0x400016d4 , 0xc5c200c1 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  541 , "TXQ_DQ                                     " , 0x400019bc , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  542 , "TXQ_DQ                                     " , 0x40001a10 , 0x00000002 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  543 , "TXQ_DQ                                     " , 0x400003c4 , 0x00000041 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  544 , "TXQ_DQ                                     " , 0x400016cc , 0xb8c700c6 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  545 , "TXQ_DQ                                     " , 0x40000388 , 0x00000032 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  546 , "TXQ_DQ                                     " , 0x400016fc , 0xc400b8c5 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  547 , "TXQ_DQ                                     " , 0x40001964 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  548 , "TXQ_DQ                                     " , 0x40001a00 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  549 , "TXQ_DQ                                     " , 0x400003d0 , 0x00000044 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  550 , "TXQ_DQ                                     " , 0x40001620 , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  551 , "TXQ_DQ                                     " , 0x400019e0 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  552 , "TXQ_DQ                                     " , 0x400003d8 , 0x00000046 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  553 , "TXQ_DQ                                     " , 0x400019e4 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  554 , "TXQ_DQ                                     " , 0x400016dc , 0xc600b8c0 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  555 , "TXQ_DQ                                     " , 0x400016b8 , 0xc7c600b8 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  556 , "TXQ_DQ                                     " , 0x4000161c , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  557 , "TXQ_DQ                                     " , 0x400002ec , 0x0000000b , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  558 , "TXQ_DQ                                     " , 0x400019d4 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  559 , "TXQ_DQ                                     " , 0x4000192c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  560 , "TXQ_DQ                                     " , 0x40001694 , 0xc700c600 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  561 , "TXQ_DQ                                     " , 0x400002c8 , 0x00000002 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  562 , "TXQ_DQ                                     " , 0x400002cc , 0x00000003 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  563 , "TXQ_DQ                                     " , 0x40000318 , 0x00000016 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  564 , "TXQ_DQ                                     " , 0x400019cc , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  565 , "TXQ_DQ                                     " , 0x40001a1c , 0x00000002 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  566 , "TXQ_DQ                                     " , 0x400019f4 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  567 , "TXQ_DQ                                     " , 0x40000398 , 0x00000036 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  568 , "TXQ_DQ                                     " , 0x400003c0 , 0x00000040 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  569 , "TXQ_DQ                                     " , 0x4000197c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  570 , "TXQ_DQ                                     " , 0x40000320 , 0x00000018 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  571 , "TXQ_DQ                                     " , 0x400002e4 , 0x00000009 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  572 , "TXQ_DQ                                     " , 0x40001990 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  573 , "TXQ_DQ                                     " , 0x40001918 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  574 , "TXQ_DQ                                     " , 0x40001998 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  575 , "TXQ_DQ                                     " , 0x400016e4 , 0x00c1b800 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  576 , "TXQ_DQ                                     " , 0x40001670 , 0xb8c700c6 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  577 , "TXQ_DQ                                     " , 0x400016b0 , 0xb8c5c200 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  578 , "TXQ_DQ                                     " , 0x400016c4 , 0x00b8c500 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  579 , "TXQ_DQ                                     " , 0x40001970 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  580 , "TXQ_DQ                                     " , 0x40000358 , 0x00000026 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  581 , "TXQ_DQ                                     " , 0x4000191c , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  582 , "TXQ_DQ                                     " , 0x40001928 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  583 , "TXQ_DQ                                     " , 0x400019ac , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  584 , "TXQ_DQ                                     " , 0x40001650 , 0x00c1b800 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  585 , "TXQ_DQ                                     " , 0x400019e8 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  586 , "TXQ_DQ                                     " , 0x4000031c , 0x00000017 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  587 , "TXQ_DQ                                     " , 0x40000328 , 0x0000001a , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  588 , "TXQ_DQ                                     " , 0x400002fc , 0x0000000f , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  589 , "TXQ_DQ                                     " , 0x40001700 , 0x00b8c000 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  590 , "TXQ_DQ                                     " , 0x40000384 , 0x00000031 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  591 , "TXQ_DQ                                     " , 0x40001618 , 0x00000001 , "Ports Arbiter Port <%n> Work Conserving  Enable             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  592 , "TXQ_DQ                                     " , 0x400016a8 , 0x00b8c7c6 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  593 , "TXQ_DQ                                     " , 0x40000370 , 0x0000002c , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  594 , "TXQ_DQ                                     " , 0x400002c0 , 0x00000000 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  595 , "TXQ_DQ                                     " , 0x40001698 , 0xb8c300b8 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  596 , "TXQ_DQ                                     " , 0x40001a04 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  597 , "TXQ_DQ                                     " , 0x4000038c , 0x00000033 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  598 , "TXQ_DQ                                     " , 0x400016f8 , 0x00c2c100 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  599 , "TXQ_DQ                                     " , 0x400019b0 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  600 , "TXQ_DQ                                     " , 0x40001980 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  601 , "TXQ_DQ                                     " , 0x40000394 , 0x00000035 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  602 , "TXQ_DQ                                     " , 0x40001968 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  603 , "TXQ_DQ                                     " , 0x40001688 , 0x00c1b800 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  604 , "TXQ_DQ                                     " , 0x40001950 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  605 , "TXQ_DQ                                     " , 0x40001658 , 0xb8c000c4 , "Ports Arbiter Map <%n>                                      " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Arbiter Configuration/" }
    , {  606 , "TXQ_DQ                                     " , 0x40001948 , 0x00000000 , "Port <%n> Request Mask Selector                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Scheduler/Port Shaper/" }
    , {  607 , "TXQ_DQ                                     " , 0x40000308 , 0x00000012 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  608 , "TXQ_DQ                                     " , 0x400002d0 , 0x00000004 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  609 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x000007a1 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  610 , "TXQ_LL                                     " , 0x3d0a0004 , 0x00000071 , "High Speed Port <%n>                                        " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_LL/Units/<TxQ> TXQ_IP_ll/Global/Global LL Config/" }
    , {  611 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000d00 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  612 , "RxDMA1                                     " , 0x68000a48 , 0x00000052 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  613 , "RxDMA1                                     " , 0x68000a40 , 0x00000050 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  614 , "RxDMA1                                     " , 0x68000a70 , 0x0000003f , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  615 , "RxDMA1                                     " , 0x68000a4c , 0x00000053 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  616 , "RxDMA1                                     " , 0x68000a3c , 0x0000004f , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  617 , "RxDMA1                                     " , 0x68000a44 , 0x00000051 , "SCDMA %n Configuration 1                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/RXDMA/Units/<RXDMA1_IP> RxDMA IP Units/Single Channel DMA Configurations/" }
    , {  618 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x000006a0 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  619 , "EGF_EFT                                    " , 0x35010164 , 0x47464544 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  620 , "EGF_EFT                                    " , 0x35010190 , 0x73727170 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  621 , "EGF_EFT                                    " , 0x3501019c , 0x7f7e7d7c , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  622 , "EGF_EFT                                    " , 0x35010130 , 0x13121110 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  623 , "EGF_EFT                                    " , 0x35010140 , 0x23222120 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  624 , "EGF_EFT                                    " , 0x35010154 , 0x37363534 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  625 , "EGF_EFT                                    " , 0x35010170 , 0x3f3e3d3c , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  626 , "EGF_EFT                                    " , 0x35010144 , 0x27262524 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  627 , "EGF_EFT                                    " , 0x35010188 , 0x6b6a6968 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  628 , "EGF_EFT                                    " , 0x35010138 , 0x1b1a1918 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  629 , "EGF_EFT                                    " , 0x35010180 , 0x63626160 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  630 , "EGF_EFT                                    " , 0x35010150 , 0x33323130 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  631 , "EGF_EFT                                    " , 0x35010120 , 0x03020100 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  632 , "EGF_EFT                                    " , 0x3501012c , 0x0f0e0d0c , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  633 , "EGF_EFT                                    " , 0x3501015c , 0x7f7f7f7f , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  634 , "EGF_EFT                                    " , 0x35010184 , 0x67666564 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  635 , "EGF_EFT                                    " , 0x35010174 , 0x57565554 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  636 , "EGF_EFT                                    " , 0x35010194 , 0x77767574 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  637 , "EGF_EFT                                    " , 0x35010148 , 0x2b2a2928 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  638 , "EGF_EFT                                    " , 0x35010160 , 0x43424140 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  639 , "EGF_EFT                                    " , 0x3501017c , 0x5f5e5d5c , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  640 , "EGF_EFT                                    " , 0x35010124 , 0x07060504 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  641 , "EGF_EFT                                    " , 0x35010134 , 0x17161514 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  642 , "EGF_EFT                                    " , 0x35010128 , 0x0b0a0908 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  643 , "EGF_EFT                                    " , 0x35010178 , 0x5b5a5958 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  644 , "EGF_EFT                                    " , 0x35010158 , 0x7f3a3938 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  645 , "EGF_EFT                                    " , 0x3501018c , 0x6f6e6d6c , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  646 , "EGF_EFT                                    " , 0x35010198 , 0x7b7a7978 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  647 , "EGF_EFT                                    " , 0x35010168 , 0x4b4a4948 , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  648 , "EGF_EFT                                    " , 0x3501014c , 0x2f2e2d2c , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  649 , "EGF_EFT                                    " , 0x3501013c , 0x1f1e1d1c , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  650 , "EGF_EFT                                    " , 0x3501016c , 0x3b4e4d4c , "Port Remap <%n>                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Egress Filter Configurations/" }
    , {  651 , "EGF_EFT                                    " , 0x35002240 , 0x00000000 , "Port To Hemisphere Mapping %n                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Multicast FIFO/Multicast FIFO Configurations/" }
    , {  652 , "EGF_EFT                                    " , 0x3500224c , 0x00000000 , "Port To Hemisphere Mapping %n                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Multicast FIFO/Multicast FIFO Configurations/" }
    , {  653 , "EGF_EFT                                    " , 0x35002248 , 0x00000000 , "Port To Hemisphere Mapping %n                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Multicast FIFO/Multicast FIFO Configurations/" }
    , {  654 , "EGF_EFT                                    " , 0x35002244 , 0x00000000 , "Port To Hemisphere Mapping %n                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_eft/Units/EGF_IP_eft/Multicast FIFO/Multicast FIFO Configurations/" }
    , {  655 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000800 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  656 , "TXQ_DQ                                     " , 0x40000000 , 0x0000057f , "Global Dequeue Configuration                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  657 , "TXQ_DQ                                     " , 0x400003bc , 0x00000048 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  658 , "TXQ_DQ                                     " , 0x400003dc , 0x00000047 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  659 , "TXQ_DQ                                     " , 0x400003c8 , 0x00000042 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  660 , "TXQ_DQ                                     " , 0x400003a0 , 0x00000038 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  661 , "TXQ_DQ                                     " , 0x400003d4 , 0x00000045 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  662 , "TXQ_DQ                                     " , 0x400003cc , 0x00000043 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  663 , "TXQ_DQ                                     " , 0x400003c4 , 0x00000041 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  664 , "TXQ_DQ                                     " , 0x400003d0 , 0x00000044 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  665 , "TXQ_DQ                                     " , 0x400003d8 , 0x00000046 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  666 , "TXQ_DQ                                     " , 0x400003c0 , 0x00000040 , "Port <%n> To DMA Map Table                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  667 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000080 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  668 , "MT                                         " , 0x04000004 , 0x07011b3c , "FDB Global Configuration 1                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Global Configuration/" }
    , {  669 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000060 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  670 , "L2I                                        " , 0x03000020 , 0x22023824 , "Bridge Command Configuration0                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/Bridge Engine Configuration/" }
    , {  671 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000160 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  672 , "IPLR0                                      " , 0x0b002004 , 0x00014801 , "Statistical Metering Configuration 0                        " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<IPLR0_IP> PLR/Units/PLR/Global Configuration/" }
    , {  673 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000400 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  674 , "IPLR1                                      " , 0x20002004 , 0x00014801 , "Statistical Metering Configuration 0                        " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<IPLR1_IP> PLR/Units/PLR/Global Configuration/" }
    , {  675 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x000001c0 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  676 , "EPLR                                       " , 0x0e002004 , 0x00014801 , "Statistical Metering Configuration 0                        " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EPLR_IP> PLR/Units/PLR/Global Configuration/" }
    , {  677 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000020 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  678 , "TTI                                        " , 0x01000000 , 0x30002503 , "TTI Unit Global Configuration                               " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Units/TTI_IP_Units/TTI Unit Global Configurations/" }
    , {  679 , "TTI                                        " , 0x0100000c , 0x00000000 , "TTI Engine Configuration                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Units/TTI_IP_Units/TTI Engine/" }
    , {  680 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000040 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  681 , "PCL                                        " , 0x02000000 , 0x00000003 , "Ingress Policy Global Configuration                         " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<IPCL_IP> PCL/Units/PCL/" }
    , {  682 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000080 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  683 , "MT                                         " , 0x04000114 , 0x0000ffff , "MAC Lookup1                                                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Lookup/" }
    , {  684 , "MT                                         " , 0x04000004 , 0x07011b3c , "FDB Global Configuration 1                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Global Configuration/" }
    , {  685 , "MT                                         " , 0x04000118 , 0x00001fff , "VLAN Lookup                                                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Lookup/" }
    , {  686 , "MT                                         " , 0x04000110 , 0xffffffff , "MAC Lookup0                                                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Lookup/" }
    , {  687 , "MT                                         " , 0x04000150 , 0x00000001 , "FDB Indirect Access Access                                  " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , {  688 , "MT                                         " , 0x04000000 , 0x107bfc00 , "FDB Global Configuration                                    " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/" }
    , {  689 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000060 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  690 , "L2I                                        " , 0x03001014 , 0x00000006 , "MAC QoS Table Entry<%n>                                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/MAC-Based QoS Table/" }
    , {  691 , "L2I                                        " , 0x03001010 , 0x00000005 , "MAC QoS Table Entry<%n>                                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/MAC-Based QoS Table/" }
    , {  692 , "L2I                                        " , 0x03001004 , 0x00000002 , "MAC QoS Table Entry<%n>                                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/MAC-Based QoS Table/" }
    , {  693 , "L2I                                        " , 0x03001008 , 0x00000003 , "MAC QoS Table Entry<%n>                                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/MAC-Based QoS Table/" }
    , {  694 , "L2I                                        " , 0x03000000 , 0x02806004 , "Bridge Global Configuration0                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/Bridge Engine Configuration/" }
    , {  695 , "L2I                                        " , 0x03001018 , 0x00000007 , "MAC QoS Table Entry<%n>                                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/MAC-Based QoS Table/" }
    , {  696 , "L2I                                        " , 0x03001000 , 0x00000001 , "MAC QoS Table Entry<%n>                                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/MAC-Based QoS Table/" }
    , {  697 , "L2I                                        " , 0x0300100c , 0x00000004 , "MAC QoS Table Entry<%n>                                     " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Units/L2I_IP_Units/MAC-Based QoS Table/" }
    , {  698 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000160 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  699 , "IPLR0                                      " , 0x0b000000 , 0xbe830003 , "Policer Control0                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<IPLR0_IP> PLR/Units/PLR/Global Configuration/" }
    , {  700 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000400 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  701 , "IPLR1                                      " , 0x20000000 , 0xbe830003 , "Policer Control0                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<IPLR1_IP> PLR/Units/PLR/Global Configuration/" }
    , {  702 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x000001a0 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  703 , "EQ                                         " , 0x0d000118 , 0x00001400 , "CPU Target Device Configuration Register2                   " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EQ_IP> EQ_IP/Units/EQ_IP Units/CPU Target Device Configuration/" }
    , {  704 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x000001c0 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  705 , "EPLR                                       " , 0x0e000000 , 0xbe830003 , "Policer Control0                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EPLR_IP> PLR/Units/PLR/Global Configuration/" }
    , {  706 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000781 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  707 , "TXQ_Queue                                  " , 0x3c0a0000 , 0x000020c8 , "Global Tail Drop Configuration                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Configuration/" }
    , {  708 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000800 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , {  709 , "TXQ_DQ                                     " , 0x40000000 , 0x0000077f , "Global Dequeue Configuration                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_DQ/Units/<TxQ> TXQ_IP_dq/Global/Global DQ Config/" }
    , {  710 , "TXQ_Queue                                  " , 0x3c0a0820 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  711 , "TXQ_Queue                                  " , 0x3c0a0828 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  712 , "TXQ_Queue                                  " , 0x3c0a0910 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  713 , "TXQ_Queue                                  " , 0x3c0a0858 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  714 , "TXQ_Queue                                  " , 0x3c0a083c , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  715 , "TXQ_Queue                                  " , 0x3c0a0834 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  716 , "TXQ_Queue                                  " , 0x3c0a0848 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  717 , "TXQ_Queue                                  " , 0x3c0a0934 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  718 , "TXQ_Queue                                  " , 0x3c0a090c , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  719 , "TXQ_Queue                                  " , 0x3c0a085c , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  720 , "TXQ_Queue                                  " , 0x3c0a0928 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  721 , "TXQ_Queue                                  " , 0x3c0a0830 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  722 , "TXQ_Queue                                  " , 0x3c0a0904 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  723 , "TXQ_Queue                                  " , 0x3c0a0938 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  724 , "TXQ_Queue                                  " , 0x3c0a0908 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  725 , "TXQ_Queue                                  " , 0x3c0a0840 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  726 , "TXQ_Queue                                  " , 0x3c0a082c , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  727 , "TXQ_Queue                                  " , 0x3c0a0920 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  728 , "TXQ_Queue                                  " , 0x3c0a0900 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  729 , "TXQ_Queue                                  " , 0x3c0a0824 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  730 , "TXQ_Queue                                  " , 0x3c0a093c , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  731 , "TXQ_Queue                                  " , 0x3c0a0838 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  732 , "TXQ_Queue                                  " , 0x3c0a0924 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  733 , "TXQ_Queue                                  " , 0x3c0a0930 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  734 , "TXQ_Queue                                  " , 0x3c0a0850 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  735 , "TXQ_Queue                                  " , 0x3c0a0844 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  736 , "TXQ_Queue                                  " , 0x3c0a0914 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  737 , "TXQ_Queue                                  " , 0x3c0a0854 , 0x00003fff , "Profile<%n> Port Descriptor Limits                          " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  738 , "TXQ_Queue                                  " , 0x3c0a0918 , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  739 , "TXQ_Queue                                  " , 0x3c0a091c , 0x000fffff , "Profile<%n> Port Buffer Limits                              " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Units/<TxQ> TXQ_IP_queue/Tail Drop/Tail Drop Limits/" }
    , {  740 , "TXQ_Queue                                  " , 0x3c0a2000 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  741 , "TXQ_Queue                                  " , 0x3c0a2004 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  742 , "TXQ_Queue                                  " , 0x3c0a2008 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  743 , "TXQ_Queue                                  " , 0x3c0a200c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  744 , "TXQ_Queue                                  " , 0x3c0a2010 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  745 , "TXQ_Queue                                  " , 0x3c0a2014 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  746 , "TXQ_Queue                                  " , 0x3c0a2018 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  747 , "TXQ_Queue                                  " , 0x3c0a201c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  748 , "TXQ_Queue                                  " , 0x3c0a2020 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  749 , "TXQ_Queue                                  " , 0x3c0a2024 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  750 , "TXQ_Queue                                  " , 0x3c0a2028 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  751 , "TXQ_Queue                                  " , 0x3c0a202c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  752 , "TXQ_Queue                                  " , 0x3c0a2030 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  753 , "TXQ_Queue                                  " , 0x3c0a2034 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  754 , "TXQ_Queue                                  " , 0x3c0a2038 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  755 , "TXQ_Queue                                  " , 0x3c0a203c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  756 , "TXQ_Queue                                  " , 0x3c0a2040 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  757 , "TXQ_Queue                                  " , 0x3c0a2044 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  758 , "TXQ_Queue                                  " , 0x3c0a2048 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  759 , "TXQ_Queue                                  " , 0x3c0a204c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  760 , "TXQ_Queue                                  " , 0x3c0a2050 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  761 , "TXQ_Queue                                  " , 0x3c0a2054 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  762 , "TXQ_Queue                                  " , 0x3c0a2058 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  763 , "TXQ_Queue                                  " , 0x3c0a205c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  764 , "TXQ_Queue                                  " , 0x3c0a2060 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  765 , "TXQ_Queue                                  " , 0x3c0a2064 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  766 , "TXQ_Queue                                  " , 0x3c0a2068 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  767 , "TXQ_Queue                                  " , 0x3c0a206c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  768 , "TXQ_Queue                                  " , 0x3c0a2070 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  769 , "TXQ_Queue                                  " , 0x3c0a2074 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  770 , "TXQ_Queue                                  " , 0x3c0a2078 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  771 , "TXQ_Queue                                  " , 0x3c0a207c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  772 , "TXQ_Queue                                  " , 0x3c0a2080 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  773 , "TXQ_Queue                                  " , 0x3c0a2084 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  774 , "TXQ_Queue                                  " , 0x3c0a2088 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  775 , "TXQ_Queue                                  " , 0x3c0a208c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  776 , "TXQ_Queue                                  " , 0x3c0a2090 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  777 , "TXQ_Queue                                  " , 0x3c0a2094 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  778 , "TXQ_Queue                                  " , 0x3c0a2098 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  779 , "TXQ_Queue                                  " , 0x3c0a209c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  780 , "TXQ_Queue                                  " , 0x3c0a20a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  781 , "TXQ_Queue                                  " , 0x3c0a20a4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  782 , "TXQ_Queue                                  " , 0x3c0a20a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  783 , "TXQ_Queue                                  " , 0x3c0a20ac , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  784 , "TXQ_Queue                                  " , 0x3c0a20b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  785 , "TXQ_Queue                                  " , 0x3c0a20b4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  786 , "TXQ_Queue                                  " , 0x3c0a20b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  787 , "TXQ_Queue                                  " , 0x3c0a20bc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  788 , "TXQ_Queue                                  " , 0x3c0a20c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  789 , "TXQ_Queue                                  " , 0x3c0a20c4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  790 , "TXQ_Queue                                  " , 0x3c0a20c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  791 , "TXQ_Queue                                  " , 0x3c0a20cc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  792 , "TXQ_Queue                                  " , 0x3c0a20d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  793 , "TXQ_Queue                                  " , 0x3c0a20d4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  794 , "TXQ_Queue                                  " , 0x3c0a20d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  795 , "TXQ_Queue                                  " , 0x3c0a20dc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  796 , "TXQ_Queue                                  " , 0x3c0a20e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  797 , "TXQ_Queue                                  " , 0x3c0a20e4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  798 , "TXQ_Queue                                  " , 0x3c0a20e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  799 , "TXQ_Queue                                  " , 0x3c0a20ec , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  800 , "TXQ_Queue                                  " , 0x3c0a20f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  801 , "TXQ_Queue                                  " , 0x3c0a20f4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  802 , "TXQ_Queue                                  " , 0x3c0a20f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  803 , "TXQ_Queue                                  " , 0x3c0a20fc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  804 , "TXQ_Queue                                  " , 0x3c0a2100 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  805 , "TXQ_Queue                                  " , 0x3c0a2104 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  806 , "TXQ_Queue                                  " , 0x3c0a2108 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  807 , "TXQ_Queue                                  " , 0x3c0a210c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  808 , "TXQ_Queue                                  " , 0x3c0a2110 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  809 , "TXQ_Queue                                  " , 0x3c0a2114 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  810 , "TXQ_Queue                                  " , 0x3c0a2118 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  811 , "TXQ_Queue                                  " , 0x3c0a211c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  812 , "TXQ_Queue                                  " , 0x3c0a2120 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  813 , "TXQ_Queue                                  " , 0x3c0a2124 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  814 , "TXQ_Queue                                  " , 0x3c0a2128 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  815 , "TXQ_Queue                                  " , 0x3c0a212c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  816 , "TXQ_Queue                                  " , 0x3c0a2130 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  817 , "TXQ_Queue                                  " , 0x3c0a2134 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  818 , "TXQ_Queue                                  " , 0x3c0a2138 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  819 , "TXQ_Queue                                  " , 0x3c0a213c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  820 , "TXQ_Queue                                  " , 0x3c0a2140 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  821 , "TXQ_Queue                                  " , 0x3c0a2144 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  822 , "TXQ_Queue                                  " , 0x3c0a2148 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  823 , "TXQ_Queue                                  " , 0x3c0a214c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  824 , "TXQ_Queue                                  " , 0x3c0a2150 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  825 , "TXQ_Queue                                  " , 0x3c0a2154 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  826 , "TXQ_Queue                                  " , 0x3c0a2158 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  827 , "TXQ_Queue                                  " , 0x3c0a215c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  828 , "TXQ_Queue                                  " , 0x3c0a2160 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  829 , "TXQ_Queue                                  " , 0x3c0a2164 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  830 , "TXQ_Queue                                  " , 0x3c0a2168 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  831 , "TXQ_Queue                                  " , 0x3c0a216c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  832 , "TXQ_Queue                                  " , 0x3c0a2170 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  833 , "TXQ_Queue                                  " , 0x3c0a2174 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  834 , "TXQ_Queue                                  " , 0x3c0a2178 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  835 , "TXQ_Queue                                  " , 0x3c0a217c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  836 , "TXQ_Queue                                  " , 0x3c0a2180 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  837 , "TXQ_Queue                                  " , 0x3c0a2184 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  838 , "TXQ_Queue                                  " , 0x3c0a2188 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  839 , "TXQ_Queue                                  " , 0x3c0a218c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  840 , "TXQ_Queue                                  " , 0x3c0a2190 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  841 , "TXQ_Queue                                  " , 0x3c0a2194 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  842 , "TXQ_Queue                                  " , 0x3c0a2198 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  843 , "TXQ_Queue                                  " , 0x3c0a219c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  844 , "TXQ_Queue                                  " , 0x3c0a21a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  845 , "TXQ_Queue                                  " , 0x3c0a21a4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  846 , "TXQ_Queue                                  " , 0x3c0a21a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  847 , "TXQ_Queue                                  " , 0x3c0a21ac , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  848 , "TXQ_Queue                                  " , 0x3c0a21b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  849 , "TXQ_Queue                                  " , 0x3c0a21b4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  850 , "TXQ_Queue                                  " , 0x3c0a21b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  851 , "TXQ_Queue                                  " , 0x3c0a21bc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  852 , "TXQ_Queue                                  " , 0x3c0a21c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  853 , "TXQ_Queue                                  " , 0x3c0a21c4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  854 , "TXQ_Queue                                  " , 0x3c0a21c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  855 , "TXQ_Queue                                  " , 0x3c0a21cc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  856 , "TXQ_Queue                                  " , 0x3c0a21d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  857 , "TXQ_Queue                                  " , 0x3c0a21d4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  858 , "TXQ_Queue                                  " , 0x3c0a21d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  859 , "TXQ_Queue                                  " , 0x3c0a21dc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  860 , "TXQ_Queue                                  " , 0x3c0a21e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  861 , "TXQ_Queue                                  " , 0x3c0a21e4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  862 , "TXQ_Queue                                  " , 0x3c0a21e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  863 , "TXQ_Queue                                  " , 0x3c0a21ec , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  864 , "TXQ_Queue                                  " , 0x3c0a21f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  865 , "TXQ_Queue                                  " , 0x3c0a21f4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  866 , "TXQ_Queue                                  " , 0x3c0a21f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  867 , "TXQ_Queue                                  " , 0x3c0a21fc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  868 , "TXQ_Queue                                  " , 0x3c0a2200 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  869 , "TXQ_Queue                                  " , 0x3c0a2204 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  870 , "TXQ_Queue                                  " , 0x3c0a2208 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  871 , "TXQ_Queue                                  " , 0x3c0a220c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  872 , "TXQ_Queue                                  " , 0x3c0a2210 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  873 , "TXQ_Queue                                  " , 0x3c0a2214 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  874 , "TXQ_Queue                                  " , 0x3c0a2218 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  875 , "TXQ_Queue                                  " , 0x3c0a221c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  876 , "TXQ_Queue                                  " , 0x3c0a2220 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  877 , "TXQ_Queue                                  " , 0x3c0a2224 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  878 , "TXQ_Queue                                  " , 0x3c0a2228 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  879 , "TXQ_Queue                                  " , 0x3c0a222c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  880 , "TXQ_Queue                                  " , 0x3c0a2230 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  881 , "TXQ_Queue                                  " , 0x3c0a2234 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  882 , "TXQ_Queue                                  " , 0x3c0a2238 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  883 , "TXQ_Queue                                  " , 0x3c0a223c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  884 , "TXQ_Queue                                  " , 0x3c0a2240 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  885 , "TXQ_Queue                                  " , 0x3c0a2244 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  886 , "TXQ_Queue                                  " , 0x3c0a2248 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  887 , "TXQ_Queue                                  " , 0x3c0a224c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  888 , "TXQ_Queue                                  " , 0x3c0a2250 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  889 , "TXQ_Queue                                  " , 0x3c0a2254 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  890 , "TXQ_Queue                                  " , 0x3c0a2258 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  891 , "TXQ_Queue                                  " , 0x3c0a225c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  892 , "TXQ_Queue                                  " , 0x3c0a2260 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  893 , "TXQ_Queue                                  " , 0x3c0a2264 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  894 , "TXQ_Queue                                  " , 0x3c0a2268 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  895 , "TXQ_Queue                                  " , 0x3c0a226c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  896 , "TXQ_Queue                                  " , 0x3c0a2270 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  897 , "TXQ_Queue                                  " , 0x3c0a2274 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  898 , "TXQ_Queue                                  " , 0x3c0a2278 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  899 , "TXQ_Queue                                  " , 0x3c0a227c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  900 , "TXQ_Queue                                  " , 0x3c0a2280 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  901 , "TXQ_Queue                                  " , 0x3c0a2284 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  902 , "TXQ_Queue                                  " , 0x3c0a2288 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  903 , "TXQ_Queue                                  " , 0x3c0a228c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  904 , "TXQ_Queue                                  " , 0x3c0a2290 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  905 , "TXQ_Queue                                  " , 0x3c0a2294 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  906 , "TXQ_Queue                                  " , 0x3c0a2298 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  907 , "TXQ_Queue                                  " , 0x3c0a229c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  908 , "TXQ_Queue                                  " , 0x3c0a22a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  909 , "TXQ_Queue                                  " , 0x3c0a22a4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  910 , "TXQ_Queue                                  " , 0x3c0a22a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  911 , "TXQ_Queue                                  " , 0x3c0a22ac , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  912 , "TXQ_Queue                                  " , 0x3c0a22b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  913 , "TXQ_Queue                                  " , 0x3c0a22b4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  914 , "TXQ_Queue                                  " , 0x3c0a22b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  915 , "TXQ_Queue                                  " , 0x3c0a22bc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  916 , "TXQ_Queue                                  " , 0x3c0a22c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  917 , "TXQ_Queue                                  " , 0x3c0a22c4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  918 , "TXQ_Queue                                  " , 0x3c0a22c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  919 , "TXQ_Queue                                  " , 0x3c0a22cc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  920 , "TXQ_Queue                                  " , 0x3c0a22d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  921 , "TXQ_Queue                                  " , 0x3c0a22d4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  922 , "TXQ_Queue                                  " , 0x3c0a22d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  923 , "TXQ_Queue                                  " , 0x3c0a22dc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  924 , "TXQ_Queue                                  " , 0x3c0a22e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  925 , "TXQ_Queue                                  " , 0x3c0a22e4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  926 , "TXQ_Queue                                  " , 0x3c0a22e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  927 , "TXQ_Queue                                  " , 0x3c0a22ec , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  928 , "TXQ_Queue                                  " , 0x3c0a22f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  929 , "TXQ_Queue                                  " , 0x3c0a22f4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  930 , "TXQ_Queue                                  " , 0x3c0a22f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  931 , "TXQ_Queue                                  " , 0x3c0a22fc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  932 , "TXQ_Queue                                  " , 0x3c0a2300 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  933 , "TXQ_Queue                                  " , 0x3c0a2304 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  934 , "TXQ_Queue                                  " , 0x3c0a2308 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  935 , "TXQ_Queue                                  " , 0x3c0a230c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  936 , "TXQ_Queue                                  " , 0x3c0a2310 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  937 , "TXQ_Queue                                  " , 0x3c0a2314 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  938 , "TXQ_Queue                                  " , 0x3c0a2318 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  939 , "TXQ_Queue                                  " , 0x3c0a231c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  940 , "TXQ_Queue                                  " , 0x3c0a2320 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  941 , "TXQ_Queue                                  " , 0x3c0a2324 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  942 , "TXQ_Queue                                  " , 0x3c0a2328 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  943 , "TXQ_Queue                                  " , 0x3c0a232c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  944 , "TXQ_Queue                                  " , 0x3c0a2330 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  945 , "TXQ_Queue                                  " , 0x3c0a2334 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  946 , "TXQ_Queue                                  " , 0x3c0a2338 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  947 , "TXQ_Queue                                  " , 0x3c0a233c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  948 , "TXQ_Queue                                  " , 0x3c0a2340 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  949 , "TXQ_Queue                                  " , 0x3c0a2344 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  950 , "TXQ_Queue                                  " , 0x3c0a2348 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  951 , "TXQ_Queue                                  " , 0x3c0a234c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  952 , "TXQ_Queue                                  " , 0x3c0a2350 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  953 , "TXQ_Queue                                  " , 0x3c0a2354 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  954 , "TXQ_Queue                                  " , 0x3c0a2358 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  955 , "TXQ_Queue                                  " , 0x3c0a235c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  956 , "TXQ_Queue                                  " , 0x3c0a2360 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  957 , "TXQ_Queue                                  " , 0x3c0a2364 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  958 , "TXQ_Queue                                  " , 0x3c0a2368 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  959 , "TXQ_Queue                                  " , 0x3c0a236c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  960 , "TXQ_Queue                                  " , 0x3c0a2370 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  961 , "TXQ_Queue                                  " , 0x3c0a2374 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  962 , "TXQ_Queue                                  " , 0x3c0a2378 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  963 , "TXQ_Queue                                  " , 0x3c0a237c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  964 , "TXQ_Queue                                  " , 0x3c0a2380 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  965 , "TXQ_Queue                                  " , 0x3c0a2384 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  966 , "TXQ_Queue                                  " , 0x3c0a2388 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  967 , "TXQ_Queue                                  " , 0x3c0a238c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  968 , "TXQ_Queue                                  " , 0x3c0a2390 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  969 , "TXQ_Queue                                  " , 0x3c0a2394 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  970 , "TXQ_Queue                                  " , 0x3c0a2398 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  971 , "TXQ_Queue                                  " , 0x3c0a239c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  972 , "TXQ_Queue                                  " , 0x3c0a23a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  973 , "TXQ_Queue                                  " , 0x3c0a23a4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  974 , "TXQ_Queue                                  " , 0x3c0a23a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  975 , "TXQ_Queue                                  " , 0x3c0a23ac , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  976 , "TXQ_Queue                                  " , 0x3c0a23b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  977 , "TXQ_Queue                                  " , 0x3c0a23b4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  978 , "TXQ_Queue                                  " , 0x3c0a23b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  979 , "TXQ_Queue                                  " , 0x3c0a23bc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  980 , "TXQ_Queue                                  " , 0x3c0a23c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  981 , "TXQ_Queue                                  " , 0x3c0a23c4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  982 , "TXQ_Queue                                  " , 0x3c0a23c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  983 , "TXQ_Queue                                  " , 0x3c0a23cc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  984 , "TXQ_Queue                                  " , 0x3c0a23d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  985 , "TXQ_Queue                                  " , 0x3c0a23d4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  986 , "TXQ_Queue                                  " , 0x3c0a23d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  987 , "TXQ_Queue                                  " , 0x3c0a23dc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  988 , "TXQ_Queue                                  " , 0x3c0a23e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  989 , "TXQ_Queue                                  " , 0x3c0a23e4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  990 , "TXQ_Queue                                  " , 0x3c0a23e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  991 , "TXQ_Queue                                  " , 0x3c0a23ec , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  992 , "TXQ_Queue                                  " , 0x3c0a23f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  993 , "TXQ_Queue                                  " , 0x3c0a23f4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  994 , "TXQ_Queue                                  " , 0x3c0a23f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  995 , "TXQ_Queue                                  " , 0x3c0a23fc , 0x00000003 , "not_found                                                   " , "not_found" }
    , {  996 , "TXQ_Queue                                  " , 0x3c0a2800 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  997 , "TXQ_Queue                                  " , 0x3c0a2804 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  998 , "TXQ_Queue                                  " , 0x3c0a2808 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , {  999 , "TXQ_Queue                                  " , 0x3c0a280c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1000 , "TXQ_Queue                                  " , 0x3c0a2810 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1001 , "TXQ_Queue                                  " , 0x3c0a2814 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1002 , "TXQ_Queue                                  " , 0x3c0a2818 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1003 , "TXQ_Queue                                  " , 0x3c0a281c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1004 , "TXQ_Queue                                  " , 0x3c0a2820 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1005 , "TXQ_Queue                                  " , 0x3c0a2824 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1006 , "TXQ_Queue                                  " , 0x3c0a2828 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1007 , "TXQ_Queue                                  " , 0x3c0a282c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1008 , "TXQ_Queue                                  " , 0x3c0a2830 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1009 , "TXQ_Queue                                  " , 0x3c0a2834 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1010 , "TXQ_Queue                                  " , 0x3c0a2838 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1011 , "TXQ_Queue                                  " , 0x3c0a283c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1012 , "TXQ_Queue                                  " , 0x3c0a2840 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1013 , "TXQ_Queue                                  " , 0x3c0a2844 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1014 , "TXQ_Queue                                  " , 0x3c0a2848 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1015 , "TXQ_Queue                                  " , 0x3c0a284c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1016 , "TXQ_Queue                                  " , 0x3c0a2850 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1017 , "TXQ_Queue                                  " , 0x3c0a2854 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1018 , "TXQ_Queue                                  " , 0x3c0a2858 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1019 , "TXQ_Queue                                  " , 0x3c0a285c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1020 , "TXQ_Queue                                  " , 0x3c0a2860 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1021 , "TXQ_Queue                                  " , 0x3c0a2864 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1022 , "TXQ_Queue                                  " , 0x3c0a2868 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1023 , "TXQ_Queue                                  " , 0x3c0a286c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1024 , "TXQ_Queue                                  " , 0x3c0a2870 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1025 , "TXQ_Queue                                  " , 0x3c0a2874 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1026 , "TXQ_Queue                                  " , 0x3c0a2878 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1027 , "TXQ_Queue                                  " , 0x3c0a287c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1028 , "TXQ_Queue                                  " , 0x3c0a2880 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1029 , "TXQ_Queue                                  " , 0x3c0a2884 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1030 , "TXQ_Queue                                  " , 0x3c0a2888 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1031 , "TXQ_Queue                                  " , 0x3c0a288c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1032 , "TXQ_Queue                                  " , 0x3c0a2890 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1033 , "TXQ_Queue                                  " , 0x3c0a2894 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1034 , "TXQ_Queue                                  " , 0x3c0a2898 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1035 , "TXQ_Queue                                  " , 0x3c0a289c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1036 , "TXQ_Queue                                  " , 0x3c0a28a0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1037 , "TXQ_Queue                                  " , 0x3c0a28a4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1038 , "TXQ_Queue                                  " , 0x3c0a28a8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1039 , "TXQ_Queue                                  " , 0x3c0a28ac , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1040 , "TXQ_Queue                                  " , 0x3c0a28b0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1041 , "TXQ_Queue                                  " , 0x3c0a28b4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1042 , "TXQ_Queue                                  " , 0x3c0a28b8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1043 , "TXQ_Queue                                  " , 0x3c0a28bc , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1044 , "TXQ_Queue                                  " , 0x3c0a28c0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1045 , "TXQ_Queue                                  " , 0x3c0a28c4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1046 , "TXQ_Queue                                  " , 0x3c0a28c8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1047 , "TXQ_Queue                                  " , 0x3c0a28cc , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1048 , "TXQ_Queue                                  " , 0x3c0a28d0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1049 , "TXQ_Queue                                  " , 0x3c0a28d4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1050 , "TXQ_Queue                                  " , 0x3c0a28d8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1051 , "TXQ_Queue                                  " , 0x3c0a28dc , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1052 , "TXQ_Queue                                  " , 0x3c0a28e0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1053 , "TXQ_Queue                                  " , 0x3c0a28e4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1054 , "TXQ_Queue                                  " , 0x3c0a28e8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1055 , "TXQ_Queue                                  " , 0x3c0a28ec , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1056 , "TXQ_Queue                                  " , 0x3c0a28f0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1057 , "TXQ_Queue                                  " , 0x3c0a28f4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1058 , "TXQ_Queue                                  " , 0x3c0a28f8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1059 , "TXQ_Queue                                  " , 0x3c0a28fc , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1060 , "TXQ_Queue                                  " , 0x3c0a2900 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1061 , "TXQ_Queue                                  " , 0x3c0a2904 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1062 , "TXQ_Queue                                  " , 0x3c0a2908 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1063 , "TXQ_Queue                                  " , 0x3c0a290c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1064 , "TXQ_Queue                                  " , 0x3c0a2910 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1065 , "TXQ_Queue                                  " , 0x3c0a2914 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1066 , "TXQ_Queue                                  " , 0x3c0a2918 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1067 , "TXQ_Queue                                  " , 0x3c0a291c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1068 , "TXQ_Queue                                  " , 0x3c0a2920 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1069 , "TXQ_Queue                                  " , 0x3c0a2924 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1070 , "TXQ_Queue                                  " , 0x3c0a2928 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1071 , "TXQ_Queue                                  " , 0x3c0a292c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1072 , "TXQ_Queue                                  " , 0x3c0a2930 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1073 , "TXQ_Queue                                  " , 0x3c0a2934 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1074 , "TXQ_Queue                                  " , 0x3c0a2938 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1075 , "TXQ_Queue                                  " , 0x3c0a293c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1076 , "TXQ_Queue                                  " , 0x3c0a2940 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1077 , "TXQ_Queue                                  " , 0x3c0a2944 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1078 , "TXQ_Queue                                  " , 0x3c0a2948 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1079 , "TXQ_Queue                                  " , 0x3c0a294c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1080 , "TXQ_Queue                                  " , 0x3c0a2950 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1081 , "TXQ_Queue                                  " , 0x3c0a2954 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1082 , "TXQ_Queue                                  " , 0x3c0a2958 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1083 , "TXQ_Queue                                  " , 0x3c0a295c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1084 , "TXQ_Queue                                  " , 0x3c0a2960 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1085 , "TXQ_Queue                                  " , 0x3c0a2964 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1086 , "TXQ_Queue                                  " , 0x3c0a2968 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1087 , "TXQ_Queue                                  " , 0x3c0a296c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1088 , "TXQ_Queue                                  " , 0x3c0a2970 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1089 , "TXQ_Queue                                  " , 0x3c0a2974 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1090 , "TXQ_Queue                                  " , 0x3c0a2978 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1091 , "TXQ_Queue                                  " , 0x3c0a297c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1092 , "TXQ_Queue                                  " , 0x3c0a2980 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1093 , "TXQ_Queue                                  " , 0x3c0a2984 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1094 , "TXQ_Queue                                  " , 0x3c0a2988 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1095 , "TXQ_Queue                                  " , 0x3c0a298c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1096 , "TXQ_Queue                                  " , 0x3c0a2990 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1097 , "TXQ_Queue                                  " , 0x3c0a2994 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1098 , "TXQ_Queue                                  " , 0x3c0a2998 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1099 , "TXQ_Queue                                  " , 0x3c0a299c , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1100 , "TXQ_Queue                                  " , 0x3c0a29a0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1101 , "TXQ_Queue                                  " , 0x3c0a29a4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1102 , "TXQ_Queue                                  " , 0x3c0a29a8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1103 , "TXQ_Queue                                  " , 0x3c0a29ac , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1104 , "TXQ_Queue                                  " , 0x3c0a29b0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1105 , "TXQ_Queue                                  " , 0x3c0a29b4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1106 , "TXQ_Queue                                  " , 0x3c0a29b8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1107 , "TXQ_Queue                                  " , 0x3c0a29bc , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1108 , "TXQ_Queue                                  " , 0x3c0a29c0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1109 , "TXQ_Queue                                  " , 0x3c0a29c4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1110 , "TXQ_Queue                                  " , 0x3c0a29c8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1111 , "TXQ_Queue                                  " , 0x3c0a29cc , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1112 , "TXQ_Queue                                  " , 0x3c0a29d0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1113 , "TXQ_Queue                                  " , 0x3c0a29d4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1114 , "TXQ_Queue                                  " , 0x3c0a29d8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1115 , "TXQ_Queue                                  " , 0x3c0a29dc , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1116 , "TXQ_Queue                                  " , 0x3c0a29e0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1117 , "TXQ_Queue                                  " , 0x3c0a29e4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1118 , "TXQ_Queue                                  " , 0x3c0a29e8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1119 , "TXQ_Queue                                  " , 0x3c0a29ec , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1120 , "TXQ_Queue                                  " , 0x3c0a29f0 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1121 , "TXQ_Queue                                  " , 0x3c0a29f4 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1122 , "TXQ_Queue                                  " , 0x3c0a29f8 , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1123 , "TXQ_Queue                                  " , 0x3c0a29fc , 0x00003fff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1124 , "TXQ_Queue                                  " , 0x3c0a1000 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1125 , "TXQ_Queue                                  " , 0x3c0a1004 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1126 , "TXQ_Queue                                  " , 0x3c0a1008 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1127 , "TXQ_Queue                                  " , 0x3c0a100c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1128 , "TXQ_Queue                                  " , 0x3c0a1010 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1129 , "TXQ_Queue                                  " , 0x3c0a1014 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1130 , "TXQ_Queue                                  " , 0x3c0a1018 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1131 , "TXQ_Queue                                  " , 0x3c0a101c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1132 , "TXQ_Queue                                  " , 0x3c0a1020 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1133 , "TXQ_Queue                                  " , 0x3c0a1024 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1134 , "TXQ_Queue                                  " , 0x3c0a1028 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1135 , "TXQ_Queue                                  " , 0x3c0a102c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1136 , "TXQ_Queue                                  " , 0x3c0a1030 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1137 , "TXQ_Queue                                  " , 0x3c0a1034 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1138 , "TXQ_Queue                                  " , 0x3c0a1038 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1139 , "TXQ_Queue                                  " , 0x3c0a103c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1140 , "TXQ_Queue                                  " , 0x3c0a1040 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1141 , "TXQ_Queue                                  " , 0x3c0a1044 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1142 , "TXQ_Queue                                  " , 0x3c0a1048 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1143 , "TXQ_Queue                                  " , 0x3c0a104c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1144 , "TXQ_Queue                                  " , 0x3c0a1050 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1145 , "TXQ_Queue                                  " , 0x3c0a1054 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1146 , "TXQ_Queue                                  " , 0x3c0a1058 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1147 , "TXQ_Queue                                  " , 0x3c0a105c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1148 , "TXQ_Queue                                  " , 0x3c0a1060 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1149 , "TXQ_Queue                                  " , 0x3c0a1064 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1150 , "TXQ_Queue                                  " , 0x3c0a1068 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1151 , "TXQ_Queue                                  " , 0x3c0a106c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1152 , "TXQ_Queue                                  " , 0x3c0a1070 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1153 , "TXQ_Queue                                  " , 0x3c0a1074 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1154 , "TXQ_Queue                                  " , 0x3c0a1078 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1155 , "TXQ_Queue                                  " , 0x3c0a107c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1156 , "TXQ_Queue                                  " , 0x3c0a1080 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1157 , "TXQ_Queue                                  " , 0x3c0a1084 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1158 , "TXQ_Queue                                  " , 0x3c0a1088 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1159 , "TXQ_Queue                                  " , 0x3c0a108c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1160 , "TXQ_Queue                                  " , 0x3c0a1090 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1161 , "TXQ_Queue                                  " , 0x3c0a1094 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1162 , "TXQ_Queue                                  " , 0x3c0a1098 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1163 , "TXQ_Queue                                  " , 0x3c0a109c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1164 , "TXQ_Queue                                  " , 0x3c0a10a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1165 , "TXQ_Queue                                  " , 0x3c0a10a4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1166 , "TXQ_Queue                                  " , 0x3c0a10a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1167 , "TXQ_Queue                                  " , 0x3c0a10ac , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1168 , "TXQ_Queue                                  " , 0x3c0a10b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1169 , "TXQ_Queue                                  " , 0x3c0a10b4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1170 , "TXQ_Queue                                  " , 0x3c0a10b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1171 , "TXQ_Queue                                  " , 0x3c0a10bc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1172 , "TXQ_Queue                                  " , 0x3c0a10c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1173 , "TXQ_Queue                                  " , 0x3c0a10c4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1174 , "TXQ_Queue                                  " , 0x3c0a10c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1175 , "TXQ_Queue                                  " , 0x3c0a10cc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1176 , "TXQ_Queue                                  " , 0x3c0a10d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1177 , "TXQ_Queue                                  " , 0x3c0a10d4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1178 , "TXQ_Queue                                  " , 0x3c0a10d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1179 , "TXQ_Queue                                  " , 0x3c0a10dc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1180 , "TXQ_Queue                                  " , 0x3c0a10e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1181 , "TXQ_Queue                                  " , 0x3c0a10e4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1182 , "TXQ_Queue                                  " , 0x3c0a10e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1183 , "TXQ_Queue                                  " , 0x3c0a10ec , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1184 , "TXQ_Queue                                  " , 0x3c0a10f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1185 , "TXQ_Queue                                  " , 0x3c0a10f4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1186 , "TXQ_Queue                                  " , 0x3c0a10f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1187 , "TXQ_Queue                                  " , 0x3c0a10fc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1188 , "TXQ_Queue                                  " , 0x3c0a1100 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1189 , "TXQ_Queue                                  " , 0x3c0a1104 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1190 , "TXQ_Queue                                  " , 0x3c0a1108 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1191 , "TXQ_Queue                                  " , 0x3c0a110c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1192 , "TXQ_Queue                                  " , 0x3c0a1110 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1193 , "TXQ_Queue                                  " , 0x3c0a1114 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1194 , "TXQ_Queue                                  " , 0x3c0a1118 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1195 , "TXQ_Queue                                  " , 0x3c0a111c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1196 , "TXQ_Queue                                  " , 0x3c0a1120 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1197 , "TXQ_Queue                                  " , 0x3c0a1124 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1198 , "TXQ_Queue                                  " , 0x3c0a1128 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1199 , "TXQ_Queue                                  " , 0x3c0a112c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1200 , "TXQ_Queue                                  " , 0x3c0a1130 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1201 , "TXQ_Queue                                  " , 0x3c0a1134 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1202 , "TXQ_Queue                                  " , 0x3c0a1138 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1203 , "TXQ_Queue                                  " , 0x3c0a113c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1204 , "TXQ_Queue                                  " , 0x3c0a1140 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1205 , "TXQ_Queue                                  " , 0x3c0a1144 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1206 , "TXQ_Queue                                  " , 0x3c0a1148 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1207 , "TXQ_Queue                                  " , 0x3c0a114c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1208 , "TXQ_Queue                                  " , 0x3c0a1150 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1209 , "TXQ_Queue                                  " , 0x3c0a1154 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1210 , "TXQ_Queue                                  " , 0x3c0a1158 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1211 , "TXQ_Queue                                  " , 0x3c0a115c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1212 , "TXQ_Queue                                  " , 0x3c0a1160 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1213 , "TXQ_Queue                                  " , 0x3c0a1164 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1214 , "TXQ_Queue                                  " , 0x3c0a1168 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1215 , "TXQ_Queue                                  " , 0x3c0a116c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1216 , "TXQ_Queue                                  " , 0x3c0a1170 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1217 , "TXQ_Queue                                  " , 0x3c0a1174 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1218 , "TXQ_Queue                                  " , 0x3c0a1178 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1219 , "TXQ_Queue                                  " , 0x3c0a117c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1220 , "TXQ_Queue                                  " , 0x3c0a1180 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1221 , "TXQ_Queue                                  " , 0x3c0a1184 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1222 , "TXQ_Queue                                  " , 0x3c0a1188 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1223 , "TXQ_Queue                                  " , 0x3c0a118c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1224 , "TXQ_Queue                                  " , 0x3c0a1190 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1225 , "TXQ_Queue                                  " , 0x3c0a1194 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1226 , "TXQ_Queue                                  " , 0x3c0a1198 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1227 , "TXQ_Queue                                  " , 0x3c0a119c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1228 , "TXQ_Queue                                  " , 0x3c0a11a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1229 , "TXQ_Queue                                  " , 0x3c0a11a4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1230 , "TXQ_Queue                                  " , 0x3c0a11a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1231 , "TXQ_Queue                                  " , 0x3c0a11ac , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1232 , "TXQ_Queue                                  " , 0x3c0a11b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1233 , "TXQ_Queue                                  " , 0x3c0a11b4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1234 , "TXQ_Queue                                  " , 0x3c0a11b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1235 , "TXQ_Queue                                  " , 0x3c0a11bc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1236 , "TXQ_Queue                                  " , 0x3c0a11c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1237 , "TXQ_Queue                                  " , 0x3c0a11c4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1238 , "TXQ_Queue                                  " , 0x3c0a11c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1239 , "TXQ_Queue                                  " , 0x3c0a11cc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1240 , "TXQ_Queue                                  " , 0x3c0a11d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1241 , "TXQ_Queue                                  " , 0x3c0a11d4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1242 , "TXQ_Queue                                  " , 0x3c0a11d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1243 , "TXQ_Queue                                  " , 0x3c0a11dc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1244 , "TXQ_Queue                                  " , 0x3c0a11e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1245 , "TXQ_Queue                                  " , 0x3c0a11e4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1246 , "TXQ_Queue                                  " , 0x3c0a11e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1247 , "TXQ_Queue                                  " , 0x3c0a11ec , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1248 , "TXQ_Queue                                  " , 0x3c0a11f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1249 , "TXQ_Queue                                  " , 0x3c0a11f4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1250 , "TXQ_Queue                                  " , 0x3c0a11f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1251 , "TXQ_Queue                                  " , 0x3c0a11fc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1252 , "TXQ_Queue                                  " , 0x3c0a1200 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1253 , "TXQ_Queue                                  " , 0x3c0a1204 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1254 , "TXQ_Queue                                  " , 0x3c0a1208 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1255 , "TXQ_Queue                                  " , 0x3c0a120c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1256 , "TXQ_Queue                                  " , 0x3c0a1210 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1257 , "TXQ_Queue                                  " , 0x3c0a1214 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1258 , "TXQ_Queue                                  " , 0x3c0a1218 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1259 , "TXQ_Queue                                  " , 0x3c0a121c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1260 , "TXQ_Queue                                  " , 0x3c0a1220 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1261 , "TXQ_Queue                                  " , 0x3c0a1224 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1262 , "TXQ_Queue                                  " , 0x3c0a1228 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1263 , "TXQ_Queue                                  " , 0x3c0a122c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1264 , "TXQ_Queue                                  " , 0x3c0a1230 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1265 , "TXQ_Queue                                  " , 0x3c0a1234 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1266 , "TXQ_Queue                                  " , 0x3c0a1238 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1267 , "TXQ_Queue                                  " , 0x3c0a123c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1268 , "TXQ_Queue                                  " , 0x3c0a1240 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1269 , "TXQ_Queue                                  " , 0x3c0a1244 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1270 , "TXQ_Queue                                  " , 0x3c0a1248 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1271 , "TXQ_Queue                                  " , 0x3c0a124c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1272 , "TXQ_Queue                                  " , 0x3c0a1250 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1273 , "TXQ_Queue                                  " , 0x3c0a1254 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1274 , "TXQ_Queue                                  " , 0x3c0a1258 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1275 , "TXQ_Queue                                  " , 0x3c0a125c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1276 , "TXQ_Queue                                  " , 0x3c0a1260 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1277 , "TXQ_Queue                                  " , 0x3c0a1264 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1278 , "TXQ_Queue                                  " , 0x3c0a1268 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1279 , "TXQ_Queue                                  " , 0x3c0a126c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1280 , "TXQ_Queue                                  " , 0x3c0a1270 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1281 , "TXQ_Queue                                  " , 0x3c0a1274 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1282 , "TXQ_Queue                                  " , 0x3c0a1278 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1283 , "TXQ_Queue                                  " , 0x3c0a127c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1284 , "TXQ_Queue                                  " , 0x3c0a1280 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1285 , "TXQ_Queue                                  " , 0x3c0a1284 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1286 , "TXQ_Queue                                  " , 0x3c0a1288 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1287 , "TXQ_Queue                                  " , 0x3c0a128c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1288 , "TXQ_Queue                                  " , 0x3c0a1290 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1289 , "TXQ_Queue                                  " , 0x3c0a1294 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1290 , "TXQ_Queue                                  " , 0x3c0a1298 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1291 , "TXQ_Queue                                  " , 0x3c0a129c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1292 , "TXQ_Queue                                  " , 0x3c0a12a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1293 , "TXQ_Queue                                  " , 0x3c0a12a4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1294 , "TXQ_Queue                                  " , 0x3c0a12a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1295 , "TXQ_Queue                                  " , 0x3c0a12ac , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1296 , "TXQ_Queue                                  " , 0x3c0a12b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1297 , "TXQ_Queue                                  " , 0x3c0a12b4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1298 , "TXQ_Queue                                  " , 0x3c0a12b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1299 , "TXQ_Queue                                  " , 0x3c0a12bc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1300 , "TXQ_Queue                                  " , 0x3c0a12c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1301 , "TXQ_Queue                                  " , 0x3c0a12c4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1302 , "TXQ_Queue                                  " , 0x3c0a12c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1303 , "TXQ_Queue                                  " , 0x3c0a12cc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1304 , "TXQ_Queue                                  " , 0x3c0a12d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1305 , "TXQ_Queue                                  " , 0x3c0a12d4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1306 , "TXQ_Queue                                  " , 0x3c0a12d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1307 , "TXQ_Queue                                  " , 0x3c0a12dc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1308 , "TXQ_Queue                                  " , 0x3c0a12e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1309 , "TXQ_Queue                                  " , 0x3c0a12e4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1310 , "TXQ_Queue                                  " , 0x3c0a12e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1311 , "TXQ_Queue                                  " , 0x3c0a12ec , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1312 , "TXQ_Queue                                  " , 0x3c0a12f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1313 , "TXQ_Queue                                  " , 0x3c0a12f4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1314 , "TXQ_Queue                                  " , 0x3c0a12f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1315 , "TXQ_Queue                                  " , 0x3c0a12fc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1316 , "TXQ_Queue                                  " , 0x3c0a1300 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1317 , "TXQ_Queue                                  " , 0x3c0a1304 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1318 , "TXQ_Queue                                  " , 0x3c0a1308 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1319 , "TXQ_Queue                                  " , 0x3c0a130c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1320 , "TXQ_Queue                                  " , 0x3c0a1310 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1321 , "TXQ_Queue                                  " , 0x3c0a1314 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1322 , "TXQ_Queue                                  " , 0x3c0a1318 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1323 , "TXQ_Queue                                  " , 0x3c0a131c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1324 , "TXQ_Queue                                  " , 0x3c0a1320 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1325 , "TXQ_Queue                                  " , 0x3c0a1324 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1326 , "TXQ_Queue                                  " , 0x3c0a1328 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1327 , "TXQ_Queue                                  " , 0x3c0a132c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1328 , "TXQ_Queue                                  " , 0x3c0a1330 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1329 , "TXQ_Queue                                  " , 0x3c0a1334 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1330 , "TXQ_Queue                                  " , 0x3c0a1338 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1331 , "TXQ_Queue                                  " , 0x3c0a133c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1332 , "TXQ_Queue                                  " , 0x3c0a1340 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1333 , "TXQ_Queue                                  " , 0x3c0a1344 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1334 , "TXQ_Queue                                  " , 0x3c0a1348 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1335 , "TXQ_Queue                                  " , 0x3c0a134c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1336 , "TXQ_Queue                                  " , 0x3c0a1350 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1337 , "TXQ_Queue                                  " , 0x3c0a1354 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1338 , "TXQ_Queue                                  " , 0x3c0a1358 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1339 , "TXQ_Queue                                  " , 0x3c0a135c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1340 , "TXQ_Queue                                  " , 0x3c0a1360 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1341 , "TXQ_Queue                                  " , 0x3c0a1364 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1342 , "TXQ_Queue                                  " , 0x3c0a1368 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1343 , "TXQ_Queue                                  " , 0x3c0a136c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1344 , "TXQ_Queue                                  " , 0x3c0a1370 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1345 , "TXQ_Queue                                  " , 0x3c0a1374 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1346 , "TXQ_Queue                                  " , 0x3c0a1378 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1347 , "TXQ_Queue                                  " , 0x3c0a137c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1348 , "TXQ_Queue                                  " , 0x3c0a1380 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1349 , "TXQ_Queue                                  " , 0x3c0a1384 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1350 , "TXQ_Queue                                  " , 0x3c0a1388 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1351 , "TXQ_Queue                                  " , 0x3c0a138c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1352 , "TXQ_Queue                                  " , 0x3c0a1390 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1353 , "TXQ_Queue                                  " , 0x3c0a1394 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1354 , "TXQ_Queue                                  " , 0x3c0a1398 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1355 , "TXQ_Queue                                  " , 0x3c0a139c , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1356 , "TXQ_Queue                                  " , 0x3c0a13a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1357 , "TXQ_Queue                                  " , 0x3c0a13a4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1358 , "TXQ_Queue                                  " , 0x3c0a13a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1359 , "TXQ_Queue                                  " , 0x3c0a13ac , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1360 , "TXQ_Queue                                  " , 0x3c0a13b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1361 , "TXQ_Queue                                  " , 0x3c0a13b4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1362 , "TXQ_Queue                                  " , 0x3c0a13b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1363 , "TXQ_Queue                                  " , 0x3c0a13bc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1364 , "TXQ_Queue                                  " , 0x3c0a13c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1365 , "TXQ_Queue                                  " , 0x3c0a13c4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1366 , "TXQ_Queue                                  " , 0x3c0a13c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1367 , "TXQ_Queue                                  " , 0x3c0a13cc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1368 , "TXQ_Queue                                  " , 0x3c0a13d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1369 , "TXQ_Queue                                  " , 0x3c0a13d4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1370 , "TXQ_Queue                                  " , 0x3c0a13d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1371 , "TXQ_Queue                                  " , 0x3c0a13dc , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1372 , "TXQ_Queue                                  " , 0x3c0a13e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1373 , "TXQ_Queue                                  " , 0x3c0a13e4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1374 , "TXQ_Queue                                  " , 0x3c0a13e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1375 , "TXQ_Queue                                  " , 0x3c0a13ec , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1376 , "TXQ_Queue                                  " , 0x3c0a13f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1377 , "TXQ_Queue                                  " , 0x3c0a13f4 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1378 , "TXQ_Queue                                  " , 0x3c0a13f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1379 , "TXQ_Queue                                  " , 0x3c0a13fc , 0x00000003 , "not_found                                                   " , "not_found" }
    , { 1380 , "TXQ_Queue                                  " , 0x3c0a1800 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1381 , "TXQ_Queue                                  " , 0x3c0a1804 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1382 , "TXQ_Queue                                  " , 0x3c0a1808 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1383 , "TXQ_Queue                                  " , 0x3c0a180c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1384 , "TXQ_Queue                                  " , 0x3c0a1810 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1385 , "TXQ_Queue                                  " , 0x3c0a1814 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1386 , "TXQ_Queue                                  " , 0x3c0a1818 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1387 , "TXQ_Queue                                  " , 0x3c0a181c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1388 , "TXQ_Queue                                  " , 0x3c0a1820 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1389 , "TXQ_Queue                                  " , 0x3c0a1824 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1390 , "TXQ_Queue                                  " , 0x3c0a1828 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1391 , "TXQ_Queue                                  " , 0x3c0a182c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1392 , "TXQ_Queue                                  " , 0x3c0a1830 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1393 , "TXQ_Queue                                  " , 0x3c0a1834 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1394 , "TXQ_Queue                                  " , 0x3c0a1838 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1395 , "TXQ_Queue                                  " , 0x3c0a183c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1396 , "TXQ_Queue                                  " , 0x3c0a1840 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1397 , "TXQ_Queue                                  " , 0x3c0a1844 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1398 , "TXQ_Queue                                  " , 0x3c0a1848 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1399 , "TXQ_Queue                                  " , 0x3c0a184c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1400 , "TXQ_Queue                                  " , 0x3c0a1850 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1401 , "TXQ_Queue                                  " , 0x3c0a1854 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1402 , "TXQ_Queue                                  " , 0x3c0a1858 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1403 , "TXQ_Queue                                  " , 0x3c0a185c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1404 , "TXQ_Queue                                  " , 0x3c0a1860 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1405 , "TXQ_Queue                                  " , 0x3c0a1864 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1406 , "TXQ_Queue                                  " , 0x3c0a1868 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1407 , "TXQ_Queue                                  " , 0x3c0a186c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1408 , "TXQ_Queue                                  " , 0x3c0a1870 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1409 , "TXQ_Queue                                  " , 0x3c0a1874 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1410 , "TXQ_Queue                                  " , 0x3c0a1878 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1411 , "TXQ_Queue                                  " , 0x3c0a187c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1412 , "TXQ_Queue                                  " , 0x3c0a1880 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1413 , "TXQ_Queue                                  " , 0x3c0a1884 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1414 , "TXQ_Queue                                  " , 0x3c0a1888 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1415 , "TXQ_Queue                                  " , 0x3c0a188c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1416 , "TXQ_Queue                                  " , 0x3c0a1890 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1417 , "TXQ_Queue                                  " , 0x3c0a1894 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1418 , "TXQ_Queue                                  " , 0x3c0a1898 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1419 , "TXQ_Queue                                  " , 0x3c0a189c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1420 , "TXQ_Queue                                  " , 0x3c0a18a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1421 , "TXQ_Queue                                  " , 0x3c0a18a4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1422 , "TXQ_Queue                                  " , 0x3c0a18a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1423 , "TXQ_Queue                                  " , 0x3c0a18ac , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1424 , "TXQ_Queue                                  " , 0x3c0a18b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1425 , "TXQ_Queue                                  " , 0x3c0a18b4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1426 , "TXQ_Queue                                  " , 0x3c0a18b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1427 , "TXQ_Queue                                  " , 0x3c0a18bc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1428 , "TXQ_Queue                                  " , 0x3c0a18c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1429 , "TXQ_Queue                                  " , 0x3c0a18c4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1430 , "TXQ_Queue                                  " , 0x3c0a18c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1431 , "TXQ_Queue                                  " , 0x3c0a18cc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1432 , "TXQ_Queue                                  " , 0x3c0a18d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1433 , "TXQ_Queue                                  " , 0x3c0a18d4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1434 , "TXQ_Queue                                  " , 0x3c0a18d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1435 , "TXQ_Queue                                  " , 0x3c0a18dc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1436 , "TXQ_Queue                                  " , 0x3c0a18e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1437 , "TXQ_Queue                                  " , 0x3c0a18e4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1438 , "TXQ_Queue                                  " , 0x3c0a18e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1439 , "TXQ_Queue                                  " , 0x3c0a18ec , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1440 , "TXQ_Queue                                  " , 0x3c0a18f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1441 , "TXQ_Queue                                  " , 0x3c0a18f4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1442 , "TXQ_Queue                                  " , 0x3c0a18f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1443 , "TXQ_Queue                                  " , 0x3c0a18fc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1444 , "TXQ_Queue                                  " , 0x3c0a1900 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1445 , "TXQ_Queue                                  " , 0x3c0a1904 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1446 , "TXQ_Queue                                  " , 0x3c0a1908 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1447 , "TXQ_Queue                                  " , 0x3c0a190c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1448 , "TXQ_Queue                                  " , 0x3c0a1910 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1449 , "TXQ_Queue                                  " , 0x3c0a1914 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1450 , "TXQ_Queue                                  " , 0x3c0a1918 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1451 , "TXQ_Queue                                  " , 0x3c0a191c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1452 , "TXQ_Queue                                  " , 0x3c0a1920 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1453 , "TXQ_Queue                                  " , 0x3c0a1924 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1454 , "TXQ_Queue                                  " , 0x3c0a1928 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1455 , "TXQ_Queue                                  " , 0x3c0a192c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1456 , "TXQ_Queue                                  " , 0x3c0a1930 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1457 , "TXQ_Queue                                  " , 0x3c0a1934 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1458 , "TXQ_Queue                                  " , 0x3c0a1938 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1459 , "TXQ_Queue                                  " , 0x3c0a193c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1460 , "TXQ_Queue                                  " , 0x3c0a1940 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1461 , "TXQ_Queue                                  " , 0x3c0a1944 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1462 , "TXQ_Queue                                  " , 0x3c0a1948 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1463 , "TXQ_Queue                                  " , 0x3c0a194c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1464 , "TXQ_Queue                                  " , 0x3c0a1950 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1465 , "TXQ_Queue                                  " , 0x3c0a1954 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1466 , "TXQ_Queue                                  " , 0x3c0a1958 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1467 , "TXQ_Queue                                  " , 0x3c0a195c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1468 , "TXQ_Queue                                  " , 0x3c0a1960 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1469 , "TXQ_Queue                                  " , 0x3c0a1964 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1470 , "TXQ_Queue                                  " , 0x3c0a1968 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1471 , "TXQ_Queue                                  " , 0x3c0a196c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1472 , "TXQ_Queue                                  " , 0x3c0a1970 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1473 , "TXQ_Queue                                  " , 0x3c0a1974 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1474 , "TXQ_Queue                                  " , 0x3c0a1978 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1475 , "TXQ_Queue                                  " , 0x3c0a197c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1476 , "TXQ_Queue                                  " , 0x3c0a1980 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1477 , "TXQ_Queue                                  " , 0x3c0a1984 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1478 , "TXQ_Queue                                  " , 0x3c0a1988 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1479 , "TXQ_Queue                                  " , 0x3c0a198c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1480 , "TXQ_Queue                                  " , 0x3c0a1990 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1481 , "TXQ_Queue                                  " , 0x3c0a1994 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1482 , "TXQ_Queue                                  " , 0x3c0a1998 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1483 , "TXQ_Queue                                  " , 0x3c0a199c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1484 , "TXQ_Queue                                  " , 0x3c0a19a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1485 , "TXQ_Queue                                  " , 0x3c0a19a4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1486 , "TXQ_Queue                                  " , 0x3c0a19a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1487 , "TXQ_Queue                                  " , 0x3c0a19ac , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1488 , "TXQ_Queue                                  " , 0x3c0a19b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1489 , "TXQ_Queue                                  " , 0x3c0a19b4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1490 , "TXQ_Queue                                  " , 0x3c0a19b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1491 , "TXQ_Queue                                  " , 0x3c0a19bc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1492 , "TXQ_Queue                                  " , 0x3c0a19c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1493 , "TXQ_Queue                                  " , 0x3c0a19c4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1494 , "TXQ_Queue                                  " , 0x3c0a19c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1495 , "TXQ_Queue                                  " , 0x3c0a19cc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1496 , "TXQ_Queue                                  " , 0x3c0a19d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1497 , "TXQ_Queue                                  " , 0x3c0a19d4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1498 , "TXQ_Queue                                  " , 0x3c0a19d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1499 , "TXQ_Queue                                  " , 0x3c0a19dc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1500 , "TXQ_Queue                                  " , 0x3c0a19e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1501 , "TXQ_Queue                                  " , 0x3c0a19e4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1502 , "TXQ_Queue                                  " , 0x3c0a19e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1503 , "TXQ_Queue                                  " , 0x3c0a19ec , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1504 , "TXQ_Queue                                  " , 0x3c0a19f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1505 , "TXQ_Queue                                  " , 0x3c0a19f4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1506 , "TXQ_Queue                                  " , 0x3c0a19f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1507 , "TXQ_Queue                                  " , 0x3c0a19fc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1508 , "TXQ_Queue                                  " , 0x3c0a1a00 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1509 , "TXQ_Queue                                  " , 0x3c0a1a04 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1510 , "TXQ_Queue                                  " , 0x3c0a1a08 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1511 , "TXQ_Queue                                  " , 0x3c0a1a0c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1512 , "TXQ_Queue                                  " , 0x3c0a1a10 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1513 , "TXQ_Queue                                  " , 0x3c0a1a14 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1514 , "TXQ_Queue                                  " , 0x3c0a1a18 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1515 , "TXQ_Queue                                  " , 0x3c0a1a1c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1516 , "TXQ_Queue                                  " , 0x3c0a1a20 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1517 , "TXQ_Queue                                  " , 0x3c0a1a24 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1518 , "TXQ_Queue                                  " , 0x3c0a1a28 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1519 , "TXQ_Queue                                  " , 0x3c0a1a2c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1520 , "TXQ_Queue                                  " , 0x3c0a1a30 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1521 , "TXQ_Queue                                  " , 0x3c0a1a34 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1522 , "TXQ_Queue                                  " , 0x3c0a1a38 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1523 , "TXQ_Queue                                  " , 0x3c0a1a3c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1524 , "TXQ_Queue                                  " , 0x3c0a1a40 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1525 , "TXQ_Queue                                  " , 0x3c0a1a44 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1526 , "TXQ_Queue                                  " , 0x3c0a1a48 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1527 , "TXQ_Queue                                  " , 0x3c0a1a4c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1528 , "TXQ_Queue                                  " , 0x3c0a1a50 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1529 , "TXQ_Queue                                  " , 0x3c0a1a54 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1530 , "TXQ_Queue                                  " , 0x3c0a1a58 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1531 , "TXQ_Queue                                  " , 0x3c0a1a5c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1532 , "TXQ_Queue                                  " , 0x3c0a1a60 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1533 , "TXQ_Queue                                  " , 0x3c0a1a64 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1534 , "TXQ_Queue                                  " , 0x3c0a1a68 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1535 , "TXQ_Queue                                  " , 0x3c0a1a6c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1536 , "TXQ_Queue                                  " , 0x3c0a1a70 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1537 , "TXQ_Queue                                  " , 0x3c0a1a74 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1538 , "TXQ_Queue                                  " , 0x3c0a1a78 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1539 , "TXQ_Queue                                  " , 0x3c0a1a7c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1540 , "TXQ_Queue                                  " , 0x3c0a1a80 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1541 , "TXQ_Queue                                  " , 0x3c0a1a84 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1542 , "TXQ_Queue                                  " , 0x3c0a1a88 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1543 , "TXQ_Queue                                  " , 0x3c0a1a8c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1544 , "TXQ_Queue                                  " , 0x3c0a1a90 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1545 , "TXQ_Queue                                  " , 0x3c0a1a94 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1546 , "TXQ_Queue                                  " , 0x3c0a1a98 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1547 , "TXQ_Queue                                  " , 0x3c0a1a9c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1548 , "TXQ_Queue                                  " , 0x3c0a1aa0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1549 , "TXQ_Queue                                  " , 0x3c0a1aa4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1550 , "TXQ_Queue                                  " , 0x3c0a1aa8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1551 , "TXQ_Queue                                  " , 0x3c0a1aac , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1552 , "TXQ_Queue                                  " , 0x3c0a1ab0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1553 , "TXQ_Queue                                  " , 0x3c0a1ab4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1554 , "TXQ_Queue                                  " , 0x3c0a1ab8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1555 , "TXQ_Queue                                  " , 0x3c0a1abc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1556 , "TXQ_Queue                                  " , 0x3c0a1ac0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1557 , "TXQ_Queue                                  " , 0x3c0a1ac4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1558 , "TXQ_Queue                                  " , 0x3c0a1ac8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1559 , "TXQ_Queue                                  " , 0x3c0a1acc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1560 , "TXQ_Queue                                  " , 0x3c0a1ad0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1561 , "TXQ_Queue                                  " , 0x3c0a1ad4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1562 , "TXQ_Queue                                  " , 0x3c0a1ad8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1563 , "TXQ_Queue                                  " , 0x3c0a1adc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1564 , "TXQ_Queue                                  " , 0x3c0a1ae0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1565 , "TXQ_Queue                                  " , 0x3c0a1ae4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1566 , "TXQ_Queue                                  " , 0x3c0a1ae8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1567 , "TXQ_Queue                                  " , 0x3c0a1aec , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1568 , "TXQ_Queue                                  " , 0x3c0a1af0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1569 , "TXQ_Queue                                  " , 0x3c0a1af4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1570 , "TXQ_Queue                                  " , 0x3c0a1af8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1571 , "TXQ_Queue                                  " , 0x3c0a1afc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1572 , "TXQ_Queue                                  " , 0x3c0a1b00 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1573 , "TXQ_Queue                                  " , 0x3c0a1b04 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1574 , "TXQ_Queue                                  " , 0x3c0a1b08 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1575 , "TXQ_Queue                                  " , 0x3c0a1b0c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1576 , "TXQ_Queue                                  " , 0x3c0a1b10 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1577 , "TXQ_Queue                                  " , 0x3c0a1b14 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1578 , "TXQ_Queue                                  " , 0x3c0a1b18 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1579 , "TXQ_Queue                                  " , 0x3c0a1b1c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1580 , "TXQ_Queue                                  " , 0x3c0a1b20 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1581 , "TXQ_Queue                                  " , 0x3c0a1b24 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1582 , "TXQ_Queue                                  " , 0x3c0a1b28 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1583 , "TXQ_Queue                                  " , 0x3c0a1b2c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1584 , "TXQ_Queue                                  " , 0x3c0a1b30 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1585 , "TXQ_Queue                                  " , 0x3c0a1b34 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1586 , "TXQ_Queue                                  " , 0x3c0a1b38 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1587 , "TXQ_Queue                                  " , 0x3c0a1b3c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1588 , "TXQ_Queue                                  " , 0x3c0a1b40 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1589 , "TXQ_Queue                                  " , 0x3c0a1b44 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1590 , "TXQ_Queue                                  " , 0x3c0a1b48 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1591 , "TXQ_Queue                                  " , 0x3c0a1b4c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1592 , "TXQ_Queue                                  " , 0x3c0a1b50 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1593 , "TXQ_Queue                                  " , 0x3c0a1b54 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1594 , "TXQ_Queue                                  " , 0x3c0a1b58 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1595 , "TXQ_Queue                                  " , 0x3c0a1b5c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1596 , "TXQ_Queue                                  " , 0x3c0a1b60 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1597 , "TXQ_Queue                                  " , 0x3c0a1b64 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1598 , "TXQ_Queue                                  " , 0x3c0a1b68 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1599 , "TXQ_Queue                                  " , 0x3c0a1b6c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1600 , "TXQ_Queue                                  " , 0x3c0a1b70 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1601 , "TXQ_Queue                                  " , 0x3c0a1b74 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1602 , "TXQ_Queue                                  " , 0x3c0a1b78 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1603 , "TXQ_Queue                                  " , 0x3c0a1b7c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1604 , "TXQ_Queue                                  " , 0x3c0a1b80 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1605 , "TXQ_Queue                                  " , 0x3c0a1b84 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1606 , "TXQ_Queue                                  " , 0x3c0a1b88 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1607 , "TXQ_Queue                                  " , 0x3c0a1b8c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1608 , "TXQ_Queue                                  " , 0x3c0a1b90 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1609 , "TXQ_Queue                                  " , 0x3c0a1b94 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1610 , "TXQ_Queue                                  " , 0x3c0a1b98 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1611 , "TXQ_Queue                                  " , 0x3c0a1b9c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1612 , "TXQ_Queue                                  " , 0x3c0a1ba0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1613 , "TXQ_Queue                                  " , 0x3c0a1ba4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1614 , "TXQ_Queue                                  " , 0x3c0a1ba8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1615 , "TXQ_Queue                                  " , 0x3c0a1bac , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1616 , "TXQ_Queue                                  " , 0x3c0a1bb0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1617 , "TXQ_Queue                                  " , 0x3c0a1bb4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1618 , "TXQ_Queue                                  " , 0x3c0a1bb8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1619 , "TXQ_Queue                                  " , 0x3c0a1bbc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1620 , "TXQ_Queue                                  " , 0x3c0a1bc0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1621 , "TXQ_Queue                                  " , 0x3c0a1bc4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1622 , "TXQ_Queue                                  " , 0x3c0a1bc8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1623 , "TXQ_Queue                                  " , 0x3c0a1bcc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1624 , "TXQ_Queue                                  " , 0x3c0a1bd0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1625 , "TXQ_Queue                                  " , 0x3c0a1bd4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1626 , "TXQ_Queue                                  " , 0x3c0a1bd8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1627 , "TXQ_Queue                                  " , 0x3c0a1bdc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1628 , "TXQ_Queue                                  " , 0x3c0a1be0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1629 , "TXQ_Queue                                  " , 0x3c0a1be4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1630 , "TXQ_Queue                                  " , 0x3c0a1be8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1631 , "TXQ_Queue                                  " , 0x3c0a1bec , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1632 , "TXQ_Queue                                  " , 0x3c0a1bf0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1633 , "TXQ_Queue                                  " , 0x3c0a1bf4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1634 , "TXQ_Queue                                  " , 0x3c0a1bf8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1635 , "TXQ_Queue                                  " , 0x3c0a1bfc , 0x0000ffff , "not_found                                                   " , "not_found" }
    , { 1636 , "TXQ_Queue                                  " , 0x3c0a3000 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1637 , "TXQ_Queue                                  " , 0x3c0a3004 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1638 , "TXQ_Queue                                  " , 0x3c0a3008 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1639 , "TXQ_Queue                                  " , 0x3c0a300c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1640 , "TXQ_Queue                                  " , 0x3c0a3010 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1641 , "TXQ_Queue                                  " , 0x3c0a3014 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1642 , "TXQ_Queue                                  " , 0x3c0a3018 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1643 , "TXQ_Queue                                  " , 0x3c0a301c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1644 , "TXQ_Queue                                  " , 0x3c0a3020 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1645 , "TXQ_Queue                                  " , 0x3c0a3024 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1646 , "TXQ_Queue                                  " , 0x3c0a3028 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1647 , "TXQ_Queue                                  " , 0x3c0a302c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1648 , "TXQ_Queue                                  " , 0x3c0a3030 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1649 , "TXQ_Queue                                  " , 0x3c0a3034 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1650 , "TXQ_Queue                                  " , 0x3c0a3038 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1651 , "TXQ_Queue                                  " , 0x3c0a303c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1652 , "TXQ_Queue                                  " , 0x3c0a3040 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1653 , "TXQ_Queue                                  " , 0x3c0a3044 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1654 , "TXQ_Queue                                  " , 0x3c0a3048 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1655 , "TXQ_Queue                                  " , 0x3c0a304c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1656 , "TXQ_Queue                                  " , 0x3c0a3050 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1657 , "TXQ_Queue                                  " , 0x3c0a3054 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1658 , "TXQ_Queue                                  " , 0x3c0a3058 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1659 , "TXQ_Queue                                  " , 0x3c0a305c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1660 , "TXQ_Queue                                  " , 0x3c0a3060 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1661 , "TXQ_Queue                                  " , 0x3c0a3064 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1662 , "TXQ_Queue                                  " , 0x3c0a3068 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1663 , "TXQ_Queue                                  " , 0x3c0a306c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1664 , "TXQ_Queue                                  " , 0x3c0a3070 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1665 , "TXQ_Queue                                  " , 0x3c0a3074 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1666 , "TXQ_Queue                                  " , 0x3c0a3078 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1667 , "TXQ_Queue                                  " , 0x3c0a307c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1668 , "TXQ_Queue                                  " , 0x3c0a3080 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1669 , "TXQ_Queue                                  " , 0x3c0a3084 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1670 , "TXQ_Queue                                  " , 0x3c0a3088 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1671 , "TXQ_Queue                                  " , 0x3c0a308c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1672 , "TXQ_Queue                                  " , 0x3c0a3090 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1673 , "TXQ_Queue                                  " , 0x3c0a3094 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1674 , "TXQ_Queue                                  " , 0x3c0a3098 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1675 , "TXQ_Queue                                  " , 0x3c0a309c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1676 , "TXQ_Queue                                  " , 0x3c0a30a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1677 , "TXQ_Queue                                  " , 0x3c0a30a4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1678 , "TXQ_Queue                                  " , 0x3c0a30a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1679 , "TXQ_Queue                                  " , 0x3c0a30ac , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1680 , "TXQ_Queue                                  " , 0x3c0a30b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1681 , "TXQ_Queue                                  " , 0x3c0a30b4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1682 , "TXQ_Queue                                  " , 0x3c0a30b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1683 , "TXQ_Queue                                  " , 0x3c0a30bc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1684 , "TXQ_Queue                                  " , 0x3c0a30c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1685 , "TXQ_Queue                                  " , 0x3c0a30c4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1686 , "TXQ_Queue                                  " , 0x3c0a30c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1687 , "TXQ_Queue                                  " , 0x3c0a30cc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1688 , "TXQ_Queue                                  " , 0x3c0a30d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1689 , "TXQ_Queue                                  " , 0x3c0a30d4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1690 , "TXQ_Queue                                  " , 0x3c0a30d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1691 , "TXQ_Queue                                  " , 0x3c0a30dc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1692 , "TXQ_Queue                                  " , 0x3c0a30e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1693 , "TXQ_Queue                                  " , 0x3c0a30e4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1694 , "TXQ_Queue                                  " , 0x3c0a30e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1695 , "TXQ_Queue                                  " , 0x3c0a30ec , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1696 , "TXQ_Queue                                  " , 0x3c0a30f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1697 , "TXQ_Queue                                  " , 0x3c0a30f4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1698 , "TXQ_Queue                                  " , 0x3c0a30f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1699 , "TXQ_Queue                                  " , 0x3c0a30fc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1700 , "TXQ_Queue                                  " , 0x3c0a3100 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1701 , "TXQ_Queue                                  " , 0x3c0a3104 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1702 , "TXQ_Queue                                  " , 0x3c0a3108 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1703 , "TXQ_Queue                                  " , 0x3c0a310c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1704 , "TXQ_Queue                                  " , 0x3c0a3110 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1705 , "TXQ_Queue                                  " , 0x3c0a3114 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1706 , "TXQ_Queue                                  " , 0x3c0a3118 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1707 , "TXQ_Queue                                  " , 0x3c0a311c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1708 , "TXQ_Queue                                  " , 0x3c0a3120 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1709 , "TXQ_Queue                                  " , 0x3c0a3124 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1710 , "TXQ_Queue                                  " , 0x3c0a3128 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1711 , "TXQ_Queue                                  " , 0x3c0a312c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1712 , "TXQ_Queue                                  " , 0x3c0a3130 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1713 , "TXQ_Queue                                  " , 0x3c0a3134 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1714 , "TXQ_Queue                                  " , 0x3c0a3138 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1715 , "TXQ_Queue                                  " , 0x3c0a313c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1716 , "TXQ_Queue                                  " , 0x3c0a3140 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1717 , "TXQ_Queue                                  " , 0x3c0a3144 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1718 , "TXQ_Queue                                  " , 0x3c0a3148 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1719 , "TXQ_Queue                                  " , 0x3c0a314c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1720 , "TXQ_Queue                                  " , 0x3c0a3150 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1721 , "TXQ_Queue                                  " , 0x3c0a3154 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1722 , "TXQ_Queue                                  " , 0x3c0a3158 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1723 , "TXQ_Queue                                  " , 0x3c0a315c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1724 , "TXQ_Queue                                  " , 0x3c0a3160 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1725 , "TXQ_Queue                                  " , 0x3c0a3164 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1726 , "TXQ_Queue                                  " , 0x3c0a3168 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1727 , "TXQ_Queue                                  " , 0x3c0a316c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1728 , "TXQ_Queue                                  " , 0x3c0a3170 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1729 , "TXQ_Queue                                  " , 0x3c0a3174 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1730 , "TXQ_Queue                                  " , 0x3c0a3178 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1731 , "TXQ_Queue                                  " , 0x3c0a317c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1732 , "TXQ_Queue                                  " , 0x3c0a3180 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1733 , "TXQ_Queue                                  " , 0x3c0a3184 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1734 , "TXQ_Queue                                  " , 0x3c0a3188 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1735 , "TXQ_Queue                                  " , 0x3c0a318c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1736 , "TXQ_Queue                                  " , 0x3c0a3190 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1737 , "TXQ_Queue                                  " , 0x3c0a3194 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1738 , "TXQ_Queue                                  " , 0x3c0a3198 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1739 , "TXQ_Queue                                  " , 0x3c0a319c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1740 , "TXQ_Queue                                  " , 0x3c0a31a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1741 , "TXQ_Queue                                  " , 0x3c0a31a4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1742 , "TXQ_Queue                                  " , 0x3c0a31a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1743 , "TXQ_Queue                                  " , 0x3c0a31ac , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1744 , "TXQ_Queue                                  " , 0x3c0a31b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1745 , "TXQ_Queue                                  " , 0x3c0a31b4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1746 , "TXQ_Queue                                  " , 0x3c0a31b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1747 , "TXQ_Queue                                  " , 0x3c0a31bc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1748 , "TXQ_Queue                                  " , 0x3c0a31c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1749 , "TXQ_Queue                                  " , 0x3c0a31c4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1750 , "TXQ_Queue                                  " , 0x3c0a31c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1751 , "TXQ_Queue                                  " , 0x3c0a31cc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1752 , "TXQ_Queue                                  " , 0x3c0a31d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1753 , "TXQ_Queue                                  " , 0x3c0a31d4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1754 , "TXQ_Queue                                  " , 0x3c0a31d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1755 , "TXQ_Queue                                  " , 0x3c0a31dc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1756 , "TXQ_Queue                                  " , 0x3c0a31e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1757 , "TXQ_Queue                                  " , 0x3c0a31e4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1758 , "TXQ_Queue                                  " , 0x3c0a31e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1759 , "TXQ_Queue                                  " , 0x3c0a31ec , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1760 , "TXQ_Queue                                  " , 0x3c0a31f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1761 , "TXQ_Queue                                  " , 0x3c0a31f4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1762 , "TXQ_Queue                                  " , 0x3c0a31f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1763 , "TXQ_Queue                                  " , 0x3c0a31fc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1764 , "TXQ_Queue                                  " , 0x3c0a3200 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1765 , "TXQ_Queue                                  " , 0x3c0a3204 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1766 , "TXQ_Queue                                  " , 0x3c0a3208 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1767 , "TXQ_Queue                                  " , 0x3c0a320c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1768 , "TXQ_Queue                                  " , 0x3c0a3210 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1769 , "TXQ_Queue                                  " , 0x3c0a3214 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1770 , "TXQ_Queue                                  " , 0x3c0a3218 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1771 , "TXQ_Queue                                  " , 0x3c0a321c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1772 , "TXQ_Queue                                  " , 0x3c0a3220 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1773 , "TXQ_Queue                                  " , 0x3c0a3224 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1774 , "TXQ_Queue                                  " , 0x3c0a3228 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1775 , "TXQ_Queue                                  " , 0x3c0a322c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1776 , "TXQ_Queue                                  " , 0x3c0a3230 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1777 , "TXQ_Queue                                  " , 0x3c0a3234 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1778 , "TXQ_Queue                                  " , 0x3c0a3238 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1779 , "TXQ_Queue                                  " , 0x3c0a323c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1780 , "TXQ_Queue                                  " , 0x3c0a3240 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1781 , "TXQ_Queue                                  " , 0x3c0a3244 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1782 , "TXQ_Queue                                  " , 0x3c0a3248 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1783 , "TXQ_Queue                                  " , 0x3c0a324c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1784 , "TXQ_Queue                                  " , 0x3c0a3250 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1785 , "TXQ_Queue                                  " , 0x3c0a3254 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1786 , "TXQ_Queue                                  " , 0x3c0a3258 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1787 , "TXQ_Queue                                  " , 0x3c0a325c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1788 , "TXQ_Queue                                  " , 0x3c0a3260 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1789 , "TXQ_Queue                                  " , 0x3c0a3264 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1790 , "TXQ_Queue                                  " , 0x3c0a3268 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1791 , "TXQ_Queue                                  " , 0x3c0a326c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1792 , "TXQ_Queue                                  " , 0x3c0a3270 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1793 , "TXQ_Queue                                  " , 0x3c0a3274 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1794 , "TXQ_Queue                                  " , 0x3c0a3278 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1795 , "TXQ_Queue                                  " , 0x3c0a327c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1796 , "TXQ_Queue                                  " , 0x3c0a3280 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1797 , "TXQ_Queue                                  " , 0x3c0a3284 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1798 , "TXQ_Queue                                  " , 0x3c0a3288 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1799 , "TXQ_Queue                                  " , 0x3c0a328c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1800 , "TXQ_Queue                                  " , 0x3c0a3290 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1801 , "TXQ_Queue                                  " , 0x3c0a3294 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1802 , "TXQ_Queue                                  " , 0x3c0a3298 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1803 , "TXQ_Queue                                  " , 0x3c0a329c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1804 , "TXQ_Queue                                  " , 0x3c0a32a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1805 , "TXQ_Queue                                  " , 0x3c0a32a4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1806 , "TXQ_Queue                                  " , 0x3c0a32a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1807 , "TXQ_Queue                                  " , 0x3c0a32ac , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1808 , "TXQ_Queue                                  " , 0x3c0a32b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1809 , "TXQ_Queue                                  " , 0x3c0a32b4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1810 , "TXQ_Queue                                  " , 0x3c0a32b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1811 , "TXQ_Queue                                  " , 0x3c0a32bc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1812 , "TXQ_Queue                                  " , 0x3c0a32c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1813 , "TXQ_Queue                                  " , 0x3c0a32c4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1814 , "TXQ_Queue                                  " , 0x3c0a32c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1815 , "TXQ_Queue                                  " , 0x3c0a32cc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1816 , "TXQ_Queue                                  " , 0x3c0a32d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1817 , "TXQ_Queue                                  " , 0x3c0a32d4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1818 , "TXQ_Queue                                  " , 0x3c0a32d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1819 , "TXQ_Queue                                  " , 0x3c0a32dc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1820 , "TXQ_Queue                                  " , 0x3c0a32e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1821 , "TXQ_Queue                                  " , 0x3c0a32e4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1822 , "TXQ_Queue                                  " , 0x3c0a32e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1823 , "TXQ_Queue                                  " , 0x3c0a32ec , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1824 , "TXQ_Queue                                  " , 0x3c0a32f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1825 , "TXQ_Queue                                  " , 0x3c0a32f4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1826 , "TXQ_Queue                                  " , 0x3c0a32f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1827 , "TXQ_Queue                                  " , 0x3c0a32fc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1828 , "TXQ_Queue                                  " , 0x3c0a3300 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1829 , "TXQ_Queue                                  " , 0x3c0a3304 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1830 , "TXQ_Queue                                  " , 0x3c0a3308 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1831 , "TXQ_Queue                                  " , 0x3c0a330c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1832 , "TXQ_Queue                                  " , 0x3c0a3310 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1833 , "TXQ_Queue                                  " , 0x3c0a3314 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1834 , "TXQ_Queue                                  " , 0x3c0a3318 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1835 , "TXQ_Queue                                  " , 0x3c0a331c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1836 , "TXQ_Queue                                  " , 0x3c0a3320 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1837 , "TXQ_Queue                                  " , 0x3c0a3324 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1838 , "TXQ_Queue                                  " , 0x3c0a3328 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1839 , "TXQ_Queue                                  " , 0x3c0a332c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1840 , "TXQ_Queue                                  " , 0x3c0a3330 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1841 , "TXQ_Queue                                  " , 0x3c0a3334 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1842 , "TXQ_Queue                                  " , 0x3c0a3338 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1843 , "TXQ_Queue                                  " , 0x3c0a333c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1844 , "TXQ_Queue                                  " , 0x3c0a3340 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1845 , "TXQ_Queue                                  " , 0x3c0a3344 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1846 , "TXQ_Queue                                  " , 0x3c0a3348 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1847 , "TXQ_Queue                                  " , 0x3c0a334c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1848 , "TXQ_Queue                                  " , 0x3c0a3350 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1849 , "TXQ_Queue                                  " , 0x3c0a3354 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1850 , "TXQ_Queue                                  " , 0x3c0a3358 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1851 , "TXQ_Queue                                  " , 0x3c0a335c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1852 , "TXQ_Queue                                  " , 0x3c0a3360 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1853 , "TXQ_Queue                                  " , 0x3c0a3364 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1854 , "TXQ_Queue                                  " , 0x3c0a3368 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1855 , "TXQ_Queue                                  " , 0x3c0a336c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1856 , "TXQ_Queue                                  " , 0x3c0a3370 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1857 , "TXQ_Queue                                  " , 0x3c0a3374 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1858 , "TXQ_Queue                                  " , 0x3c0a3378 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1859 , "TXQ_Queue                                  " , 0x3c0a337c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1860 , "TXQ_Queue                                  " , 0x3c0a3380 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1861 , "TXQ_Queue                                  " , 0x3c0a3384 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1862 , "TXQ_Queue                                  " , 0x3c0a3388 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1863 , "TXQ_Queue                                  " , 0x3c0a338c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1864 , "TXQ_Queue                                  " , 0x3c0a3390 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1865 , "TXQ_Queue                                  " , 0x3c0a3394 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1866 , "TXQ_Queue                                  " , 0x3c0a3398 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1867 , "TXQ_Queue                                  " , 0x3c0a339c , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1868 , "TXQ_Queue                                  " , 0x3c0a33a0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1869 , "TXQ_Queue                                  " , 0x3c0a33a4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1870 , "TXQ_Queue                                  " , 0x3c0a33a8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1871 , "TXQ_Queue                                  " , 0x3c0a33ac , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1872 , "TXQ_Queue                                  " , 0x3c0a33b0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1873 , "TXQ_Queue                                  " , 0x3c0a33b4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1874 , "TXQ_Queue                                  " , 0x3c0a33b8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1875 , "TXQ_Queue                                  " , 0x3c0a33bc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1876 , "TXQ_Queue                                  " , 0x3c0a33c0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1877 , "TXQ_Queue                                  " , 0x3c0a33c4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1878 , "TXQ_Queue                                  " , 0x3c0a33c8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1879 , "TXQ_Queue                                  " , 0x3c0a33cc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1880 , "TXQ_Queue                                  " , 0x3c0a33d0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1881 , "TXQ_Queue                                  " , 0x3c0a33d4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1882 , "TXQ_Queue                                  " , 0x3c0a33d8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1883 , "TXQ_Queue                                  " , 0x3c0a33dc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1884 , "TXQ_Queue                                  " , 0x3c0a33e0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1885 , "TXQ_Queue                                  " , 0x3c0a33e4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1886 , "TXQ_Queue                                  " , 0x3c0a33e8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1887 , "TXQ_Queue                                  " , 0x3c0a33ec , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1888 , "TXQ_Queue                                  " , 0x3c0a33f0 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1889 , "TXQ_Queue                                  " , 0x3c0a33f4 , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1890 , "TXQ_Queue                                  " , 0x3c0a33f8 , 0xffffffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1891 , "TXQ_Queue                                  " , 0x3c0a33fc , 0x0000ffff , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/TxQ_Queue/Tables/<TxQ> TxQ Queue/Tail Drop/" }
    , { 1892 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000002 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1893 , "MG Global registers SDMA and TWSI registers" , 0x0012000c , 0x00002800 , "not_found                                                   " , "not_found" }
    , { 1894 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000942 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1895 , "EGF_SHT                                    " , 0x4a100014 , 0x000000d0 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1896 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000772 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1897 , "EGF_QAG                                    " , 0x3b9000e0 , 0x00000038 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1898 , "EGF_QAG                                    " , 0x3b900100 , 0x00000040 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1899 , "EGF_QAG                                    " , 0x3b900104 , 0x00000041 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1900 , "EGF_QAG                                    " , 0x3b900108 , 0x00000042 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1901 , "EGF_QAG                                    " , 0x3b90010c , 0x00000043 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1902 , "EGF_QAG                                    " , 0x3b900110 , 0x00000044 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1903 , "EGF_QAG                                    " , 0x3b900114 , 0x00000045 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1904 , "EGF_QAG                                    " , 0x3b900118 , 0x00000046 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1905 , "EGF_QAG                                    " , 0x3b90011c , 0x00000047 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_qag/Tables/EGF qag Common/" }
    , { 1906 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000960 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1907 , "EGF_SHT                                    " , 0x4b000040 , 0x00000001 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1908 , "EGF_SHT                                    " , 0x4b000044 , 0x02000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1909 , "EGF_SHT                                    " , 0x4b000048 , 0x000001fe , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1910 , "EGF_SHT                                    " , 0x4b00004c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1911 , "EGF_SHT                                    " , 0x4b000050 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1912 , "EGF_SHT                                    " , 0x4b000054 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1913 , "EGF_SHT                                    " , 0x4b000058 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1914 , "EGF_SHT                                    " , 0x4b00005c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1915 , "EGF_SHT                                    " , 0x4b000060 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<EGF_IP>EGF_IP/<EGF_IP> EGF_IP_sht/Tables/EGF sht Common/" }
    , { 1916 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000022 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1917 , "TTI                                        " , 0x01100200 , 0x00000040 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_BOBK/" }
    , { 1918 , "TTI                                        " , 0x01100204 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_BOBK/" }
    , { 1919 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000024 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1920 , "TTI                                        " , 0x01240100 , 0x00010100 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1921 , "TTI                                        " , 0x01210400 , 0x04000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1922 , "TTI                                        " , 0x01210404 , 0x0000c000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1923 , "TTI                                        " , 0x01210408 , 0x00000200 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1924 , "TTI                                        " , 0x0121040c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1925 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000080 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1926 , "MT                                         " , 0x04000144 , 0x00000000 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1927 , "MT                                         " , 0x04000134 , 0x12000081 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1928 , "MT                                         " , 0x04000140 , 0x00000000 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1929 , "MT                                         " , 0x04000138 , 0x00000089 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1930 , "MT                                         " , 0x0400013c , 0x04100000 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1931 , "MT                                         " , 0x04000130 , 0x00010503 , "FDB Indirect Access Control                                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1932 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000064 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1933 , "L2I                                        " , 0x03240300 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1934 , "L2I                                        " , 0x03240304 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1935 , "L2I                                        " , 0x03240308 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1936 , "L2I                                        " , 0x0324030c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1937 , "L2I                                        " , 0x03240310 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1938 , "L2I                                        " , 0x03240314 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1939 , "L2I                                        " , 0x03240318 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1940 , "L2I                                        " , 0x0324031c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1941 , "L2I                                        " , 0x03240320 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1942 , "L2I                                        " , 0x03240324 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1943 , "L2I                                        " , 0x03240328 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1944 , "L2I                                        " , 0x0324032c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1945 , "L2I                                        " , 0x03240330 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1946 , "L2I                                        " , 0x03240334 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1947 , "L2I                                        " , 0x03240338 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1948 , "L2I                                        " , 0x0324033c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1949 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000066 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1950 , "L2I                                        " , 0x03300400 , 0x00000090 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Ports Bridge Configuration/" }
    , { 1951 , "L2I                                        " , 0x03300404 , 0x01000080 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Ports Bridge Configuration/" }
    , { 1952 , "L2I                                        " , 0x03300408 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Ports Bridge Configuration/" }
    , { 1953 , "L2I                                        " , 0x03200100 , 0x1fffff80 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Ports Bridge Configuration/" }
    , { 1954 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x0000000e , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1955 , "MG Global registers SDMA and TWSI registers" , 0x00700040 , 0x00000003 , "not_found                                                   " , "not_found" }
    , { 1956 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000018 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1957 , "MG Global registers SDMA and TWSI registers" , 0x00c00104 , 0x00104000 , "not_found                                                   " , "not_found" }
    , { 1958 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000022 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1959 , "TTI                                        " , 0x01100208 , 0x00000041 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_BOBK/" }
    , { 1960 , "TTI                                        " , 0x0110020c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_BOBK/" }
    , { 1961 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000024 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1962 , "TTI                                        " , 0x01240104 , 0x00010100 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1963 , "TTI                                        " , 0x01210410 , 0x04000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1964 , "TTI                                        " , 0x01210414 , 0x0000c000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1965 , "TTI                                        " , 0x01210418 , 0x00000200 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1966 , "TTI                                        " , 0x0121041c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<TTI> TTI/Tables/TTI_Common/ePort Attributes/" }
    , { 1967 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000080 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1968 , "MT                                         " , 0x04000144 , 0x00000000 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1969 , "MT                                         " , 0x04000134 , 0x12080081 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1970 , "MT                                         " , 0x04000140 , 0x00000000 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1971 , "MT                                         " , 0x04000138 , 0x00000089 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1972 , "MT                                         " , 0x0400013c , 0x04000000 , "FDB Indirect Access Data<%n>                                " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1973 , "MT                                         " , 0x04000130 , 0x000100c3 , "FDB Indirect Access Control                                 " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<FDB_IP> MT/Units/FDB_IP_Units/FDB Indirect Access/" }
    , { 1974 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000064 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1975 , "L2I                                        " , 0x03240300 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1976 , "L2I                                        " , 0x03240304 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1977 , "L2I                                        " , 0x03240308 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1978 , "L2I                                        " , 0x0324030c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1979 , "L2I                                        " , 0x03240310 , 0x0000000f , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1980 , "L2I                                        " , 0x03240314 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1981 , "L2I                                        " , 0x03240318 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1982 , "L2I                                        " , 0x0324031c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1983 , "L2I                                        " , 0x03240320 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1984 , "L2I                                        " , 0x03240324 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1985 , "L2I                                        " , 0x03240328 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1986 , "L2I                                        " , 0x0324032c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1987 , "L2I                                        " , 0x03240330 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1988 , "L2I                                        " , 0x03240334 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1989 , "L2I                                        " , 0x03240338 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1990 , "L2I                                        " , 0x0324033c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Spanning Tree State/" }
    , { 1991 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000066 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1992 , "L2I                                        " , 0x03300410 , 0x00000090 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Ports Bridge Configuration/" }
    , { 1993 , "L2I                                        " , 0x03300414 , 0x01000080 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Ports Bridge Configuration/" }
    , { 1994 , "L2I                                        " , 0x03300418 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Ports Bridge Configuration/" }
    , { 1995 , "L2I                                        " , 0x03200104 , 0x1fffff80 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress Ports Bridge Configuration/" }
    , { 1996 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x0000000e , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1997 , "MG Global registers SDMA and TWSI registers" , 0x00700040 , 0x00000183 , "not_found                                                   " , "not_found" }
    , { 1998 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000018 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 1999 , "MG Global registers SDMA and TWSI registers" , 0x00c00100 , 0x00100000 , "not_found                                                   " , "not_found" }
    , { 2000 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000065 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 2001 , "L2I                                        " , 0x032c0010 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2002 , "L2I                                        " , 0x032c0014 , 0x003ffc10 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2003 , "L2I                                        " , 0x032c0018 , 0x00008000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2004 , "L2I                                        " , 0x032c001c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2005 , "MG Global registers SDMA and TWSI registers" , 0x00000134 , 0x00000064 , "AddCompRegion_5                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 2006 , "L2I                                        " , 0x03218004 , 0x00000006 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2007 , "L2I                                        " , 0x03280040 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2008 , "L2I                                        " , 0x03280044 , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2009 , "L2I                                        " , 0x03280048 , 0x00000003 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2010 , "L2I                                        " , 0x0328004c , 0x00000000 , "                                                            " , "Cider/EBU/BobK/BobK {Current}/Switching Core/<L2I> L2I/Tables/L2I_IP tables - BobK/Ingress VLAN filterring/" }
    , { 2011 , "DFX                                        " , 0x040f800c , 0x00700007 , "Server Reset Control                                        " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/DFX Server Registers/" }
    , { 2012 , "DFX                                        " , 0x040f8000 , 0x00000000 , "Pipe Select                                                 " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/DFX Server Registers/" }
    , { 2013 , "DFX                                        " , 0x040f8000 , 0x00000001 , "Pipe Select                                                 " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/DFX Server Registers/" }
    , { 2014 , "DFX                                        " , 0x040f8000 , 0x00000000 , "Pipe Select                                                 " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/DFX Server Registers/" }
    , { 2015 , "DFX                                        " , 0x040f8000 , 0x00000002 , "Pipe Select                                                 " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/DFX Server Registers/" }
    , { 2016 , "DFX                                        " , 0x040f8000 , 0x00000000 , "Pipe Select                                                 " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/DFX Server Registers/" }
    , { 2017 , "DFX                                        " , 0x040f8000 , 0x00000001 , "Pipe Select                                                 " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/DFX Server Registers/" }
    , { 2018 , "MG Global registers SDMA and TWSI registers" , 0x00000138 , 0x00000020 , "AddCompRegion_6                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 2019 , "MG Global registers SDMA and TWSI registers" , 0x0000013c , 0x00000040 , "AddCompRegion_7                                             " , "Cider/EBU/BobK/BobK {Current}/Switching Core/MG/Units/<MG_IP> MG_IP/MG Registers/Address Completion 8 regions/" }
    , { 2020 , "MG Global registers SDMA and TWSI registers" , 0x000000b0 , 0xffffffff , "Match Sample %i                                             " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/<XSB_IDEBUG> dfx_xsb_idebug Units/" }
    , { 2021 , "MG Global registers SDMA and TWSI registers" , 0x000000bc , 0xffffffff , "Match Sample %i                                             " , "Cider/EBU/BobK/BobK {Current}/Reset And Init Controller/DFX Units/Units/<XSB_IDEBUG> dfx_xsb_idebug Units/" }
    , { BAD_VALUE, (GT_CHAR *)NULL                           ,  BAD_VALUE ,  BAD_VALUE ,  (GT_CHAR *)NULL                                               ,  (GT_CHAR *)NULL                                                                                             }
};



static CPSS_DXCH_PORT_MAP_STC bobkCetus_withTM_Map_M3[] =
{ /* Port,            mappingType                                     portGroupm, intefaceNum, txQPort, trafficManegerEnable, tmPortInd*/
     {           56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         56,        56,            GT_FALSE,          GT_NA}
    ,{           57, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         57,     GT_NA,             GT_TRUE,             57}
    ,{           58, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         58,     GT_NA,             GT_TRUE,             58}
    ,{           59, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         59,     GT_NA,             GT_TRUE,             59}
    ,{           64, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         64,     GT_NA,             GT_TRUE,             64}
    ,{           65, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         65,     GT_NA,             GT_TRUE,             65}
    ,{           66, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         66,     GT_NA,             GT_TRUE,             66}
    ,{           67, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         67,     GT_NA,             GT_TRUE,             67}
    ,{           68, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         68,     GT_NA,             GT_TRUE,             68}
    ,{           63,     CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E,               0,      GT_NA,        63,            GT_FALSE,          GT_NA}
    ,{    BAD_VALUE,      CPSS_DXCH_PORT_MAPPING_TYPE_INVALID_E,       BAD_VALUE,  BAD_VALUE, BAD_VALUE,            GT_FALSE,      BAD_VALUE}
};

static PortInitList_STC bobkCetus_withTM_portInitlist_M3[] = 
{
     { PORT_LIST_TYPE_LIST,      { 56,57,58,59, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
    ,{ PORT_LIST_TYPE_LIST,      { 64,65,       APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
    ,{ PORT_LIST_TYPE_INTERVAL,  { 66,71,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_SGMII_E  }
    ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E      }
};



static CPSS_DXCH_PORT_MAP_STC bobkCetus_woTM_Map_M4[] =
{ /* Port,            mappingType                                     portGroupm, intefaceNum, txQPort, trafficManegerEnable, tmPortInd*/
     {           56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         56,        56,            GT_FALSE,          GT_NA}
    ,{           64, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         64,        64,            GT_FALSE,          GT_NA}
    ,{           65, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         65,        65,            GT_FALSE,          GT_NA}
    ,{           66, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         66,        66,            GT_FALSE,          GT_NA}
    ,{           67, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         67,        67,            GT_FALSE,          GT_NA}
    ,{           68, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         68,        68,            GT_FALSE,          GT_NA}
    ,{           69, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         69,        69,            GT_FALSE,          GT_NA}
    ,{           70, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         70,        70,            GT_FALSE,          GT_NA}
    ,{           71, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,         71,        71,            GT_FALSE,          GT_NA}
    ,{           63,     CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E,               0,      GT_NA,        63,            GT_FALSE,          GT_NA}
    ,{    BAD_VALUE,      CPSS_DXCH_PORT_MAPPING_TYPE_INVALID_E,       BAD_VALUE,  BAD_VALUE, BAD_VALUE,            GT_FALSE,      BAD_VALUE}
};

static PortInitList_STC bobkCetus_woTM_portInitlist_M4[] = 
{
     { PORT_LIST_TYPE_LIST,      { 56,          APP_INV_PORT_CNS }, CPSS_PORT_SPEED_40000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
    ,{ PORT_LIST_TYPE_INTERVAL,  { 64,71,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
    ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E      }
};

static APPDEMO_BOBK_SCENARIO_STC scenarioList[] = 
{  /* scenario Id,     coreClock,   InitSys,   tmEnable,   port list to init,                       mapping,  regList2Print               */
     {          3,   {       167,         2,    GT_TRUE,   &bobkCetus_withTM_portInitlist_M3[0],  &bobkCetus_withTM_Map_M3[0],   (UnitRegList_STC*)NULL     } }
    ,{          4,   {       365,         1,   GT_FALSE,   &bobkCetus_woTM_portInitlist_M4[0],    &bobkCetus_woTM_Map_M4[0],     &cetus_wo_TM_RegList_M4[0] } }
    ,{ BAD_VALUE,    { BAD_VALUE, BAD_VALUE,   GT_FALSE,   (PortInitList_STC*)NULL,               (CPSS_DXCH_PORT_MAP_STC*)NULL, (UnitRegList_STC*)NULL     } }
};

APPDEMO_BOBK_SCENARIO_DATA_STC * scenarioDataPtr = (APPDEMO_BOBK_SCENARIO_DATA_STC*)NULL;


GT_STATUS bobkCetusUserScenPrintReg
(
    UnitRegList_STC *regListptr
)
{
    GT_U32 rc;
    GT_U8 dev = 0;
    GT_U32 regIdx;
    GT_U32 data;


    cpssOsPrintf("\n+-----------------------------------------------------------------------------------------------------------------------------------------+");
    cpssOsPrintf("\n|                                           Diff Register dump                                                                            |");
    cpssOsPrintf("\n+------+---------------------------------------------+------------+------------+------------+---------------------------------------------+");
    cpssOsPrintf("\n| xls# |       unitName                              |  regAddr   |   dataHW   |   dataSW   | RegName                                     |");
    cpssOsPrintf("\n+------+---------------------------------------------+------------+------------+----------------------------------------------------------+");

    for (regIdx = 0 ; regListptr[regIdx].regAddr != BAD_VALUE ; regIdx++)
    {
        rc = prvCpssDrvHwPpReadRegister(dev,regListptr[regIdx].regAddr, /*OUT*/&data);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (data != regListptr[regIdx].data)
        {
            cpssOsPrintf("\n| %4d | %43s | 0x%08x | 0x%08x | 0x%08x | %40s",regListptr[regIdx].idxInExcel
                                                    ,regListptr[regIdx].unitName
                                                    ,regListptr[regIdx].regAddr
                                                    ,regListptr[regIdx].data
                                                    ,data
                                                    ,regListptr[regIdx].regName);
        }
    }
    cpssOsPrintf("\n+-----+---------------------------------------+------------+------------+----------------------------------------------------------+");
    return GT_OK;
}


GT_STATUS bobkCetusUserScen
(
    GT_U32 scenario,
    GT_U32 printReg
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;

    GT_U32 i;
    GT_U32 mapSize = 0;

    for (i = 0 ; scenarioList[i].scenarioId != BAD_VALUE; i++)
    {
        if (scenarioList[i].scenarioId == scenario)
        {
            scenarioDataPtr = &scenarioList[i].scenarioData;
            break;
        }
    }

    if (scenarioDataPtr == NULL)  /* not found */
    {
        cpssOsPrintf("\n-->ERROR BOBK CETUS scenario %d : : Undefined scenario",scenario );
        cpssOsPrintf("\n");
        return GT_NOT_FOUND;
    }

    #ifdef ASIC_SIMULATION
        rc = appDemoBobKSimCoreClockSet(dev,scenarioDataPtr->coreClock);
        if (rc != GT_OK)
        {
            return rc;
        }
    #endif
    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (scenarioDataPtr->tmEnable == GT_TRUE)
    {
        appDemoBc2SetCustomPortsMapping(GT_TRUE);
    }

    rc = cpssInitSystem(29,scenarioDataPtr->initSystemType,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (scenarioDataPtr->mapPtr != NULL)
    {
        /*------------------------------------------------------------------------*
         * count number of valid lines till CPSS_DXCH_PORT_MAPPING_TYPE_INVALID_E *
         *------------------------------------------------------------------------*/
        for (i = 0 ; scenarioDataPtr->mapPtr[i].mappingType != CPSS_DXCH_PORT_MAPPING_TYPE_INVALID_E; i++);
			mapSize = i;
        
        rc = cpssDxChPortPhysicalPortMapSet(dev,  mapSize, scenarioDataPtr->mapPtr);
		CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
		if (GT_OK != rc)
		{
			return rc;
		}

		/* update port mapping in appDemo DB */
		rc = appDemoDxChMaxMappedPortSet(dev,  mapSize, scenarioDataPtr->mapPtr);
		CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
		if (GT_OK != rc)
		{
			return rc;
		}
    }

	if (scenarioDataPtr->tmEnable == GT_TRUE)
	{
		rc = appDemoBc2TmPortsInit(dev);
		if (rc != GT_OK)
		{
			return rc;
		}
	}


    rc = appDemoBc2PortListInit(dev,scenarioDataPtr->portInitlistPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (printReg)
    {
        rc = appDemoBobk2PortListResourcesPrint(dev);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = gtBobcat2PortPizzaArbiterIfStateDump(dev,0);
        if (rc != GT_OK)
        {
            return rc;
        }

#if 0
        rc = bobkCetusUserScenPrintReg(scenarioDataPtr->regList2Print);
        if (rc != GT_OK)
        {
            return rc;
        }
#endif
        cpssOsPrintf("\n\n");
    }
    return GT_OK;
}


UnitRegList_STC cetus_LedReg[] = 
{
    /*                                unit_name,               address,   data0,     reg_name */
      {    1 , "XLG MAC / XG MibCounter 56 0x0102C0030" , 0x0102C0030 , 0x0000000C , "XLG MAC / XG MibCounter 56 0x0102C0030" , "--" }
    , {    2 , "XLG MAC / XG MibCounter 57 0x0102C1030" , 0x0102C1030 , 0x0000006C , "XLG MAC / XG MibCounter 57 0x0102C1030" , "--" }
    , {    3 , "XLG MAC / XG MibCounter 58 0x0102C2030" , 0x0102C2030 , 0x0000002C , "XLG MAC / XG MibCounter 58 0x0102C2030" , "--" }
    , {    4 , "XLG MAC / XG MibCounter 59 0x0102C3030" , 0x0102C3030 , 0x0000004C , "XLG MAC / XG MibCounter 59 0x0102C3030" , "--" }
    , {    5 , "XLG MAC / XG MibCounter 64 0x0102C8030" , 0x0102C8030 , 0x0000010C , "XLG MAC / XG MibCounter 64 0x0102C8030" , "--" }
    , {    6 , "XLG MAC / XG MibCounter 65 0x0102C9030" , 0x0102C9030 , 0x0000016C , "XLG MAC / XG MibCounter 65 0x0102C9030" , "--" }
    , {    7 , "XLG MAC / XG MibCounter 66 0x0102CA030" , 0x0102CA030 , 0x0000012C , "XLG MAC / XG MibCounter 66 0x0102CA030" , "--" }
    , {    8 , "XLG MAC / XG MibCounter 67 0x0102CB030" , 0x0102CB030 , 0x0000014C , "XLG MAC / XG MibCounter 67 0x0102CB030" , "--" }
    , {    9 , "XLG MAC / XG MibCounter 68 0x0102CC030" , 0x0102CC030 , 0x0000008C , "XLG MAC / XG MibCounter 68 0x0102CC030" , "--" }
    , {   10 , "XLG MAC / XG MibCounter 69 0x0102CD030" , 0x0102CD030 , 0x000000EC , "XLG MAC / XG MibCounter 69 0x0102CD030" , "--" }
    , {   11 , "XLG MAC / XG MibCounter 70 0x0102CE030" , 0x0102CE030 , 0x000000AC , "XLG MAC / XG MibCounter 70 0x0102CE030" , "--" }
    , {   12 , "XLG MAC / XG MibCounter 71 0x0102CF030" , 0x0102CF030 , 0x000000CC , "XLG MAC / XG MibCounter 71 0x0102CF030" , "--" }
    , {   13 , "0x050000000                           " , 0x050000000 , 0x00FF0035 , "0x050000000                           " , "--" }
    , {   14 , "0x050000008                           " , 0x050000008 , 0x00000000 , "0x050000008                           " , "--" }
    , {   15 , "0x050000004                           " , 0x050000004 , 0x0010012A , "0x050000004                           " , "--" }
    , {   16 , "0x05000000C                           " , 0x05000000C , 0x00249249 , "0x05000000C                           " , "--" }
    , { BAD_VALUE, (GT_CHAR *)NULL                      ,   BAD_VALUE ,  BAD_VALUE ,  (GT_CHAR *)NULL                         ,  (GT_CHAR *)NULL }
};


GT_STATUS cetusTestLeds
(
    GT_VOID
)
{
    GT_STATUS rc;
    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = bobkCetusUserScenPrintReg(&cetus_LedReg[0]);
    if (rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("\n\n");
    return GT_OK;
}

/*---------------------------------------------------------------*
 *     Core Clock: 365MHz:                                       *
 *     o    48G and 40G should be configured as fast port        *
 *     Core Clock: 250MHz:                                       *
 *     o    48G, 40G and 24G should be configured as fast port   *
 *     Core Clock: 200MHz:                                       *
 *     o    40G, 24G and 20G should be configured as fast port   *
 *     o    48G isn’t supported in this core clock.              *
 *     Core Clock: 167MHz:                                       *
 *     o    24G and 20G should be configured as fast port        *
 *     o    48G and 40G aren’t supported in this core clock.     *
 *---------------------------------------------------------------*/
GT_STATUS cetusTestTxQ_365Mhz
(
    GT_U32  coreClock
)
{
    typedef struct 
    {
        GT_U32 coreClock;
        PortInitList_STC * initListPtr;
    }coreClock_x_PortInitList_STC;


    static PortInitList_STC bobkCetus_portInitlist_4x10G_2x40G[] = 
    {
         { PORT_LIST_TYPE_LIST,      { 56,57,58,59, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 64,68,       APP_INV_PORT_CNS }, CPSS_PORT_SPEED_40000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E      }
    };

    static PortInitList_STC bobkCetus_portInitlist_4x10G_2x20G[] = 
    {
         { PORT_LIST_TYPE_LIST,      { 56,57,58,59, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 64,68,       APP_INV_PORT_CNS }, CPSS_PORT_SPEED_20000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E      }
    };

    static PortInitList_STC bobkCetus_portInitlist_4x10G_1x20G[] = 
    {
         { PORT_LIST_TYPE_LIST,      { 56,57,58,59, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 64,          APP_INV_PORT_CNS }, CPSS_PORT_SPEED_20000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E      }
    };


    static coreClock_x_PortInitList_STC coreClock_x_PortInitList_Arr[] = 
    {
         { 167,  &bobkCetus_portInitlist_4x10G_1x20G[0] }
        ,{ 200,  &bobkCetus_portInitlist_4x10G_2x20G[0] }
        ,{ 250,  &bobkCetus_portInitlist_4x10G_2x40G[0] }
        ,{ 365,  &bobkCetus_portInitlist_4x10G_2x40G[0] }
        ,{   0,  NULL                                   }
    };

    GT_U8 dev = 0;
    GT_STATUS rc;
    GT_U32    i;


    #ifdef ASIC_SIMULATION
        rc = appDemoBobKSimCoreClockSet(dev,coreClock);
        if (rc != GT_OK)
        {
            return rc;
        }
    #endif

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0 ;  coreClock_x_PortInitList_Arr[i].coreClock > 0; i++)
    {
        if (coreClock_x_PortInitList_Arr[i].coreClock == coreClock)
        {
            break;
        }
    }
    if (coreClock_x_PortInitList_Arr[i].coreClock != coreClock)
    {
        cpssOsPrintf("\n--> ERR : coreclockk %d Mhz : PortInitList not fpund", coreClock);
        return GT_NOT_IMPLEMENTED;
    }
    rc = appDemoBc2PortListInit(dev,coreClock_x_PortInitList_Arr[i].initListPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoBobk2PortListResourcesPrint(dev);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = gtBobcat2PortPizzaArbiterIfStateDump(dev,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = gtBobcat2TxQHighSpeedPortPrint(dev);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\n\n");
    return GT_OK;
}


GT_STATUS cetusTestTxQ_HighSpeedPorts
(
    GT_U32  coreClock
)
{
    CPSS_PORTS_BMP_STC initPortsBmp;/* bitmap of ports to init */
    GT_U8 dev = 0;
    GT_STATUS rc;


    #ifdef ASIC_SIMULATION
        rc = appDemoBobKSimCoreClockSet(dev,coreClock);
        if (rc != GT_OK)
        {
            return rc;
        }
    #endif

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp,56);
    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp,64);

    rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,
                                    CPSS_PORT_INTERFACE_MODE_KR_E,
                                    CPSS_PORT_SPEED_40000_E);

    if(rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoBobk2PortListResourcesPrint(dev);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*
    rc = gtBobcat2PortPizzaArbiterIfStateDump(dev,0);
    if (rc != GT_OK)
    {
        return rc;
    }
    */

    rc = gtBobcat2TxQHighSpeedPortPrint(dev);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = gtBobcat2TxQHighSpeed_DQ_LL_Dump(dev);
    if (rc != GT_OK)
    {
        return rc;
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp,56);

    rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_FALSE,
                                    CPSS_PORT_INTERFACE_MODE_KR_E,
                                    CPSS_PORT_SPEED_40000_E);


    rc = appDemoBobk2PortListResourcesPrint(dev);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = gtBobcat2TxQHighSpeed_DQ_LL_Dump(dev);
    if (rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("\n\n");
    return GT_OK;
}



GT_STATUS user_defined_scenario_tm
(
    GT_VOID
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;


    static CPSS_DXCH_PORT_MAP_STC cetusMap[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort, trafficManegerEnable , tmPortInd*/
         {   0,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        0,       0,           GT_TRUE,              0}
        ,{   1,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        1,       1,           GT_TRUE,              1}
        ,{   2,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        2,       2,           GT_TRUE,              2}
        ,{   3,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        3,       3,           GT_TRUE,              3}
        ,{   4,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        4,       4,           GT_TRUE,              4}
        ,{   5,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        5,       5,           GT_TRUE,              5}
        ,{   6,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        6,       6,           GT_TRUE,              6}
        ,{   7,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        7,       7,           GT_TRUE,              7}
        ,{   8,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        8,       8,           GT_TRUE,              8}
        ,{   9,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,        9,       9,           GT_TRUE,              9}
        ,{   10, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       10,      10,           GT_TRUE,             10}
        ,{   11, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       11,      11,           GT_TRUE,             11}
        ,{   12, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       12,      12,           GT_TRUE,             12}
        ,{   13, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       13,      13,           GT_TRUE,             13}
        ,{   14, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       14,      14,           GT_TRUE,             14}
        ,{   15, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       15,      15,           GT_TRUE,             15}
        ,{   16, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       16,      16,           GT_TRUE,             16}
        ,{   17, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       17,      17,           GT_TRUE,             17}
        ,{   18, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       18,      18,           GT_TRUE,             18}
        ,{   19, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       19,      19,           GT_TRUE,             19}
        ,{   20, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       20,      20,           GT_TRUE,             20}
        ,{   21, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       21,      21,           GT_TRUE,             21}
        ,{   22, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       22,      22,           GT_TRUE,             22}
        ,{   23, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       23,      23,           GT_TRUE,             23}
        ,{   24,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       24,      24,           GT_FALSE,          GT_NA}
        ,{   25,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       25,      25,           GT_FALSE,          GT_NA}
        ,{   26,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       26,      26,           GT_FALSE,          GT_NA}
        ,{   27,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       27,      27,           GT_FALSE,          GT_NA}
        ,{   28,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       28,      28,           GT_FALSE,          GT_NA}
        ,{   29,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       29,      29,           GT_FALSE,          GT_NA}
        ,{   30,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       30,      30,           GT_FALSE,          GT_NA}
        ,{   31,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       31,      31,           GT_FALSE,          GT_NA}
        ,{   32,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       32,      32,           GT_FALSE,          GT_NA}
        ,{   33,  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,             0,       33,      33,           GT_FALSE,          GT_NA}
        ,{   34, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       34,      34,           GT_FALSE,          GT_NA}
        ,{   35, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       35,      35,           GT_FALSE,          GT_NA}
        ,{   36, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       36,      36,           GT_FALSE,          GT_NA}
        ,{   37, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       37,      37,           GT_FALSE,          GT_NA}
        ,{   38, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       38,      38,           GT_FALSE,          GT_NA}
        ,{   39, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       39,      39,           GT_FALSE,          GT_NA}
        ,{   40, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       40,      40,           GT_FALSE,          GT_NA}
        ,{   41, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       41,      41,           GT_FALSE,          GT_NA}
        ,{   42, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       42,      42,           GT_FALSE,          GT_NA}
        ,{   43, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       43,      43,           GT_FALSE,          GT_NA}
        ,{   44, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       44,      44,           GT_FALSE,          GT_NA}
        ,{   45, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       45,      45,           GT_FALSE,          GT_NA}
        ,{   46, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       46,      46,           GT_FALSE,          GT_NA}
  /*      ,{   47, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,              0,       47,      47,           GT_FALSE,          GT_NA}*/

        ,{  56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       56,   56,            GT_FALSE,             GT_NA}
        ,{  57, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       57,   57,            GT_FALSE,             GT_NA}
        ,{  58, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       58,   58,            GT_FALSE,             GT_NA}
        ,{  59, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       59,   59,            GT_FALSE,             GT_NA}

    ,{           63,     CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E,               0,      GT_NA,        63,            GT_FALSE,          GT_NA}

        ,{  64, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       64,   GT_NA,            GT_TRUE,             64}
        ,{  65, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       65,   GT_NA,            GT_TRUE,             65}
        ,{  66, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       66,   GT_NA,            GT_TRUE,             66}
        ,{  67, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       67,   GT_NA,            GT_TRUE,             67}
        ,{  68, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       68,   GT_NA,            GT_TRUE,             68}
        ,{  69, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       69,   GT_NA,            GT_TRUE,             69}
        ,{  70, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       70,   GT_NA,            GT_TRUE,             70}
        ,{  71, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       71,   GT_NA,            GT_TRUE,             71}


    };

    static PortInitList_STC portInitlist_4x10G_4x1G[] = 
    {
         { PORT_LIST_TYPE_INTERVAL,     {0,  46, 1, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_INTERVAL,     {56, 59, 1, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,     {64, 71, 1, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,        {           APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };


    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    #ifdef ASIC_SIMULATION
        appDemoBobKSimCoreClockSet(dev,365); 
    #endif

    rc = cpssInitSystem(29,2,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(cetusMap)/sizeof(cetusMap[0]),
                                        &cetusMap[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoBc2PortListInit(dev,&portInitlist_4x10G_4x1G[0]);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


