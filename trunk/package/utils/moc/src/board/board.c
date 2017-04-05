#include "board.h"
//#define MULTI_ADDR_MODE
GT_QD_DEV brDev;
GT_SYS_CONFIG   brCfg;
GT_QD_DEV brDev;
DL_ADDRTABLE brAddrTable;
GT_QoS_WEIGHT brQosWeight;

DL_STATUS brInit(void){
    DL_STATUS retVal;
    DBG_MSG("%s called. \n", __func__);

    lnxRmuInit("eth0");
    //brDev.rmuTxRx = lnxRmuTxRx;
#ifdef GT_RMGMT_ACCESS
    brCfg.BSPFunctions.rmuTxRxPacket  = lnxRmuTxRx;
    brCfg.BSPFunctions.rmuReadMii  = lnxRmuReadMii;
    brCfg.BSPFunctions.rmuWriteMii  = lnxRmuWriteMii;
    brCfg.BSPFunctions.hwAccess  = lnxRmuHwAcess;
#endif
    brCfg.BSPFunctions.readMii   = lnxRmuReadMii;
    brCfg.BSPFunctions.writeMii  = lnxRmuWriteMii;
#ifdef USE_SEMAPHORE
    brCfg.BSPFunctions.semCreate = lnxSemCreate;
    brCfg.BSPFunctions.semDelete = lnxSemDelete;
    brCfg.BSPFunctions.semTake   = lnxSemTake;
    brCfg.BSPFunctions.semGive   = lnxSemGive;
#else
    brCfg.BSPFunctions.semCreate = NULL;
    brCfg.BSPFunctions.semDelete = NULL;
    brCfg.BSPFunctions.semTake   = NULL;
    brCfg.BSPFunctions.semGive   = NULL;
#endif
    brCfg.BSPFunctions.hwAccessMod=HW_ACCESS_MODE_F2R;
    brCfg.initPorts = DL_TRUE;
    brCfg.cpuPortNum = 0;
#ifdef MANUAL_MODE
    brCfg.mode.scanMode = SMI_MANUAL_MODE;
    brCfg.mode.baseAddr = 0x10;
#else
#ifdef MULTI_ADDR_MODE
    brCfg.mode.scanMode = SMI_MULTI_ADDR_MODE;
    brCfg.mode.baseAddr = 1;
#else
    brCfg.mode.scanMode = SMI_AUTO_SCAN_MODE;
    brCfg.mode.baseAddr = 0;
#endif
#endif
    return dlDevInit(&brDev, &brCfg);
}

DL_STATUS brSwReset(void){
    DBG_MSG("%s called. \n", __func__);
    return dlSwReset(&brDev);
}
