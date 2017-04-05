#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include "board.h"
#include "acType.h"

static AC_BOOL strIsHex(char * num){
    AC_32 len = strlen(num);
    if(len<3 || '0'!=num[0] || ('x'!=num[1] && 'X'!=num[1]))
        return AC_FALSE;
    for(num+=2; *num; num++){
        if(!isHex(*num))
            return AC_FALSE;
    }
    return AC_TRUE;
}

static int usage(void){
    return printf("Usage: mdio [options]\n"
        "   Options:{read|write} [phyAddr] [regAddr] <data>\n"
        "   read  [phyAddr] [regAddr]        : Mdio read register \n"
        "   write [phyAddr] [regAddr] [data] : Mdio write register \n"
        "\n");
}


int main(int argc, char ** argv){
    brInit();
    AC_32 len;
    if(argc<4 || argc >5){
        return usage();
    }
    char * opt = argv[1];
    char * _phyAddr = argv[2];
    char * _regAddr = argv[3];
    char * _data = argv[4];
    AC_16 phyAddr;
    AC_16 regAddr;
    AC_16 data;
    DL_STATUS retVal;
#define VAL_HEX(phyAddr, _phyAddr) do{\
                                        len = strlen(_phyAddr);\
                                        if(len<3 || len>4 || '0' !=_phyAddr[0] || !isHex(_phyAddr[2])\
                                            || (_phyAddr[3] && !isHex(_phyAddr[3]))\
                                            ||('x' != _phyAddr[1] && 'X' != _phyAddr[1])){\
                                            return usage();\
                                            }\
                                        phyAddr = (3 == len) ? (AC_U16)(LETTER2NUM(_phyAddr[2])):\
                                            (LETTER2NUM(_phyAddr[2])*0x10 + LETTER2NUM(_phyAddr[3]));\
                                    }while(0)
    VAL_HEX(phyAddr, _phyAddr);
    VAL_HEX(regAddr, _regAddr);
    if(!(strcmp(opt, "read"))){
        if(DL_OK == (retVal = brDev.smiReadMii(&brDev, phyAddr, regAddr, &data))){
            printf("Mdio read from phyAddr 0x%02hx regAddr 0x%02hx : 0x%04hx.\n", phyAddr, regAddr, data);
        }else{
            printf("Mdio read from phyAddr 0x%02hx regAddr 0x%02hx failed!\n", phyAddr, regAddr);
        }
    }else if(!(strcmp(opt, "write"))){
        len = strlen(_data);
        if(!strIsHex(_data) || len>6){
            return usage();
        }
        for(data=0, _data+=2; *_data; _data++){
            data*=0x10;
            data+=LETTER2NUM(*_data);

         }
        if(DL_OK == (retVal = brDev.smiWriteMii(&brDev, phyAddr, regAddr, &data))){
            printf("Mdio write to phyAddr 0x%02hx regAddr 0x%02hx : 0x%04hx.\n", phyAddr, regAddr, data);
        }else{
            printf("Mdio write to phyAddr 0x%02hx regAddr 0x%02hx failed!\n", phyAddr, regAddr);
        }
    }else{
        return usage();
    }
    return 0;
}
