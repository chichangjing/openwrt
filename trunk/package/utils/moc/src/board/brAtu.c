#include "brAtu.h"

DL_STATUS brAddrTableShow(void){
    DBG_MSG("%s called. \n", __func__);
    return dlAddrTableShow(&brAddrTable);
}

DL_STATUS brAtuDump(void){
    DBG_MSG("%s called. \n", __func__);
    return dlAtuDump(&brDev, &brAddrTable);
}
