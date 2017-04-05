#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "board.h"
#include "stdarg.h"
int main(int argc, char * argv[]){
    brInit();
    printf(" %s : %d\n", __func__, __LINE__);
#if 1
    do{
        brQosGetWeight();
        //brAtuDump();
        //brAddrTableShow();
    }while(0);
#endif

    //brQosWeightShow();
    DL_STATUS retVal=1;
#if 1

#endif
    GT_U16 data;
    GT_SERDES_MODE mode;
    gprtSerdesPowerDown(&brDev, 9, GT_TRUE);
    gprtGetSerdesMode(&brDev, 10, &mode);
        printf("mode : %02x!\n", mode);
    //brQosWeightShow();
    return DL_OK;
}
