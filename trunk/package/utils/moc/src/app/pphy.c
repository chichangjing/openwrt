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
    return printf("Usage: pphy [options]\n"
        "   Options:{read|write} [port] [page] [regAddr] <data>\n"
        "   read  [port] [page] [regAddr]        : Pphy read register \n"
        "   write [port] [page] [regAddr] <data> : Pphy write register \n"
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
    char * _page = argv[3];
    char * _regaddr = argv[4];
    char * _data = argv[5];
    AC_8 port = 0;
    AC_8 page = 0;
    AC_8 regaddr = 0;
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
    if(!isAcU8(_port) || !isAcU8(_page) || !isAcU8(_regaddr)){
        return usage();
    }
    port=strToAc8(_port);
    page=strToAc8(_page);
    regaddr=strToAc8(_regaddr);
    if(!(strcmp(opt, "read"))){
        if(DL_OK == (retVal = hwReadPagedPhyReg(&brDev, port, page, regaddr, 1, &data))){
            printf("PagedPhy read from port %d page %d regAddr %d : 0x%04hx.\n", port, page, regaddr, data);
        }else{
            printf("PagedPhy read from port %d page %d regAddr %d failed!\n", port, page, regaddr);
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
        if(DL_OK == (retVal == hwWritePagedPhyReg(&brDev, port, page, regaddr, 1, data))){
            printf("PagedPhy write to port %d page %d regAddr %d : 0x%04hx.\n", port, page, regaddr, data);
        }else{
            printf("PagedPhy write to port %d page %d regAddr %d failed!\n", port, page, regaddr);
        }
    }else{
        return usage();
    }
    return 0;
}
