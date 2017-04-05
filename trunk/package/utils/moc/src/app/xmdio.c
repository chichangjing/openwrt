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
    return printf("Usage: xmdio [options]\n"
        "   Options:{read|write} [phyAddr] [device] [regAddr] <data>\n"
        "   read  [port] [device] [regAddr]        : XMdio read register \n"
        "   write [port] [device] [regAddr] [data] : XMdio write register \n"
        "\n");
}


int main(int argc, char ** argv){
    brInit();
    AC_32 len;
    if(argc<5 || argc >6){
        return usage();
    }
    char * opt = argv[1];
    char * _port = argv[2];
    char * _device = argv[3];
    char * _regaddr = argv[4];
    char * _data = argv[5];
    AC_32 port = 0;
    AC_32 device = 0;
    AC_32 regaddr = 0;
    AC_16 data = 0;
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
    len = strlen(_regaddr);
    if(!isAcU32(_port) || !isAcU32(_device) || !strIsHex(_regaddr) || len>6){
        return usage();
    }
    port=strToAc32(_port);
    device=strToAc32(_device);
    for(regaddr=0, _regaddr+=2; *_regaddr; _regaddr++){
        regaddr*=0x10;
        regaddr+=LETTER2NUM(*_regaddr);
    }
    if(!(strcmp(opt, "read"))){
        if(DL_OK == (retVal = gprtGet45PhyReg(&brDev, port, device, regaddr, &data))){
            printf("XMdio read from port %d device %d regAddr 0x%04x : 0x%04hx.\n", port, device, regaddr, data);
        }else{
            printf("XMdio read from port %d device %d regAddr 0x%04x failed!\n", port, device, regaddr);
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
        if(DL_OK == (retVal = gprtSet45PhyReg(&brDev, port, device, regaddr, data))){
            printf("XMdio write to port %d device %d regAddr 0x%04x : 0x%04hx.\n", port, device, regaddr, data);
        }else{
            printf("XMdio write to port %d device %d regAddr 0x%04x failed!\n", port, device, regaddr);
        }
    }else{
        return usage();
    }
    return 0;
}
