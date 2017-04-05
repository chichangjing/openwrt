#include "board.h"

DL_STATUS brQosWeightShow(void){
    DBG_MSG("%s called. \n", __func__);
    return dlQosWeightShow(&brQosWeight);
}

DL_STATUS brQosGetWeight(void){
    DBG_MSG("%s called. \n", __func__);
    return dlQosGetWeight(&brDev, &brQosWeight);
};
