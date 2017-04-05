#include "dlQos.h"

DL_STATUS dlQosWeightShow(GT_QoS_WEIGHT * qosWeight){
    int i;
    DBG_MSG("%s called. \n", __func__);
    TTY_MSG("QOS weight table length :%d \n", qosWeight->len);
    for(i=0; i<qosWeight->len; i++){
        TTY_MSG("%d%s", qosWeight->queue[i], 15==(i%16) ? "\n":"  ");
    }
    TTY_MSG("\n");
    return DL_OK;
}

DL_STATUS dlQosGetWeight(GT_QD_DEV * dev, GT_QoS_WEIGHT * qosWeight){
   DBG_MSG("%s called. \n", __func__);
   memset(qosWeight, 0, sizeof(GT_QoS_WEIGHT));
   if(gsysGetQoSWeight(dev, qosWeight)){
       LOG_MSG("gsysGetQosWeight return Failed\n");
       return DL_FAIL;
   }
   return DL_OK;
};
