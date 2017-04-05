#include "lnxSem.h"

GT_STATUS lnxSemCreate(GT_SEM * semId, GT_SEM_BEGIN_STATE state){
    DBG_MSG("%s called. \n", __func__);
    if(GT_OK != (sem_init(semId, state, 1))){
        return GT_FAIL;
    }
    return GT_OK;
}

GT_STATUS lnxSemDelete(GT_SEM * smid){
    DBG_MSG("%s called. \n", __func__);
    if(GT_OK != (sem_destroy(smid))){
        return GT_FAIL;
    }
    return GT_OK;
}

GT_STATUS lnxSemTake(GT_SEM * smid){
    DBG_MSG("%s called. \n", __func__);
    if(GT_OK != (sem_wait(smid))){
        return GT_FAIL;
    }
    return GT_OK;
}

GT_STATUS lnxSemGive(GT_SEM * smid){
    DBG_MSG("%s called. \n", __func__);
    if(GT_OK != (sem_post(smid))){
        return GT_FAIL;
    }
    return GT_OK;
}
