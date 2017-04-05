#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "dlDev.h"

DL_STATUS dlDevInit(GT_QD_DEV *dev, GT_SYS_CONFIG * cfg){
    DL_STATUS retVal;
    DBG_MSG("%s called. \n", __func__);
    if(DL_OK != (retVal=qdLoadDriver(cfg, dev))){
        LOG_MSG("qdLoadDriver Failed\n");
        return retVal;
    }
    DBG_MSG("HwAccessMod   : %d\n", dev->fgtHwAccessMod);
    DBG_MSG("Access mode   : %d\n", dev->accessMode);
    DBG_MSG("Use smi       : %d\n", dev->use_smi);
    DBG_MSG("Use mad       : %d\n", dev->use_mad);
    DBG_MSG("Device ID     : %d\n", dev->deviceId);
    DBG_MSG("devNum        : %d\n", dev->devNum);
    DBG_MSG("devEnabled    : %d\n", dev->devEnabled);
    DBG_MSG("DevName       : %d\n", dev->devName);
    DBG_MSG("DevName1      : %d\n", dev->devName1);
    DBG_MSG("DevGroup      : %d\n", dev->devGroup);
    DBG_MSG("No of Ports   : %d\n", dev->numOfPorts);
    DBG_MSG("CPU Ports     : %d\n", dev->cpuPortNum);
    DBG_MSG("MaxPorts      : %d\n", dev->maxPorts);
    DBG_MSG("MaxPhyNum     : %d\n", dev->maxPhyNum);
    DBG_MSG("Base Reg Addr : %d\n", dev->baseRegAddr);
    DBG_MSG("PhyAddr       : %d\n", dev->phyAddr);
    DBG_MSG("ValidPortVec  : %d\n", dev->validPortVec);
    DBG_MSG("ValidPhyVec   : %d\n", dev->validPhyVec);
    DBG_MSG("ValidSerdesVec: %d\n", dev->validSerdesVec);
    //TTY_MSG("Mad deviceId  : %d\n", dev->mad_dev.deviceId);
    if(DL_OK !=  (retVal=sysEnable(dev))){
        LOG_MSG("sysConfig return Failed\n");
        return retVal;
    }
    DBG_MSG("Device has been started.\n");
    return DL_OK;
}

DL_STATUS dlSwReset(GT_QD_DEV *dev){
    DL_STATUS retVal;
    DBG_MSG("%s called. \n", __func__);
    if(DL_OK != (retVal=gsysSwReset(dev))){
        LOG_MSG("gsysSwReset Failed\n");
        return retVal;
    }
    return DL_OK;
}
