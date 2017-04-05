#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "RmiAtu.h"
#include "RmiMibs.h"
#include "RmiRegs.h"
#include "Rmu.h"

static u_int32_t loop_atu_mib(u_int32_t dev){
    int port;
    do{
        for(port= 0;port<PORTS_MV6097;port++){
            MIB_dump(dev, port);
        }
        ATU_dump(dev);
        usleep(1000000);
    }while(1);
    return 0;
}

void loop_rmi(void){
    pthread_t id;
    int32_t port, ret;
    int32_t lo=2;
    while(lo--){
        for(port= 0;port<PORTS_MV6097;port++){
            MIB_dump(DEV_MV6097, port);
        }
    }
    if((ret=pthread_create(&id, NULL, (void *)loop_atu_mib, DEV_MV6097))){
        LOG_ERROR("Create pthread error!\n");
        exit (1);
    }
}

int32_t rmi_init(char * if_name){
    if(rmi_sock_init(if_name)<0){
        return -1;
    }
    mib_init();
    atu_init();
    return 0;
}
