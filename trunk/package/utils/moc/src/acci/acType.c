#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "acType.h"

/*
 * 字符有效性：
 * isLetter：字母，包括数字、大写字母、小写字母
 * isUpper：大写字母
 * isLower：小写字母
 * isBin：2进制字符
 * isOct：8进制字符
 * isDec：10进制字符
 * isHex：16进制字符
 * isSym：符号
 * inBase：检查字符是否在字符串中
 *
 * */

AC_BOOL isLetter(char letter){
    return ('0' <= letter && '9' >= letter)
         ||('A' <= letter && 'Z' >= letter)
         ||('a' <= letter && 'z' >= letter) ? AC_TRUE:AC_FALSE;
}

AC_BOOL isUpper(char letter){
    return 'A' <= letter && 'Z' >= letter ? AC_TRUE:AC_FALSE;
}

AC_BOOL isLower(char letter){
    return 'a' <= letter && 'z' >= letter ? AC_TRUE:AC_FALSE;
}

AC_BOOL isBin(char letter){
    return '0' == letter || '1' == letter ? AC_TRUE:AC_FALSE;
}

AC_BOOL isOct(char letter){
    return '0' <= letter && '7' >= letter ? AC_TRUE:AC_FALSE;
}

AC_BOOL isDec(char letter){
    return '0' <= letter && '9' >= letter ? AC_TRUE:AC_FALSE;
}

AC_BOOL isHex(char letter){
    return ('0' <= letter && '9' >= letter)
         ||('A' <= letter && 'F' >= letter)
         ||('a' <= letter && 'f' >= letter) ? AC_TRUE:AC_FALSE;
}

AC_BOOL isSym(char letter){
    return ('!' <= letter && '/' >= letter)
         ||(':' <= letter && '@' >= letter)
         ||('[' <= letter && '`' >= letter)
         ||('{' <= letter && '~' >= letter) ? AC_TRUE:AC_FALSE;
}

AC_BOOL inBase(char letter, char * baseSet, int baseLen){
    if(baseSet && baseLen)
        for(baseLen--; baseLen>=0; baseLen--)
            if(letter==baseSet[baseLen])
                return AC_TRUE;
    return AC_FALSE;
}

/*
 * 有效的mac地址字符串，自动支持如下格式：
 * ff:ff:ff:ff:ff:ff
 * ff-ff-ff-ff-ff-ff
 * ffff-ffff-ffff
 *
 * */

AC_BOOL isMac(char * mac){
    AC_8 i = 0;
    if(mac){
        switch(strlen(mac)){
            case (int)(sizeof("ff:ff:ff:ff:ff:ff") - 1):
                for(i=0; i <(sizeof("ff:ff:ff:ff:ff:ff") - 1); i++){
                    if(2==i%3){
                        if(!inBase((char)(mac[i]), ":-", sizeof(":-")-1))
                            return AC_FALSE;
                        continue;
                    }
                    if(!isHex((char)(mac[i])))
                        return AC_FALSE;
                }
                return AC_TRUE;
                break;
            case (int)(sizeof("ffff-ffff-ffff") - 1):
                for(i=0; i < (sizeof("ffff-ffff-ffff") - 1); i++){
                    if(4==i%5){
                        if('-' != (char)(mac[i]))
                            return AC_FALSE;
                        continue;
                    }
                    if(!isHex((char)(mac[i])))
                        return AC_FALSE;
                }
                return AC_TRUE;
                break;
            default:
                return AC_FALSE;
                break;
        }
        return AC_TRUE;
    }
    return AC_FALSE;
}

/*
 * 有效的ip4地址字符串，支持类似下面的地址：
 * 255.255.255.255
 * 255.100.10.0
 * 1.1.1.1
 *
 * */

AC_BOOL isIpv4(char * ip){
    char tmp[sizeof("255.255.255.255")];
    memset(tmp, 0, sizeof(tmp));
    strcpy(tmp, ip);
    char * __eve;
    AC_8 i, len;
    if(ip && (len = strlen(ip))){
        for(i=0, __eve=strtok(tmp, ".");
                __eve;
                __eve=strtok(NULL, "."), i++){
            if(!isAcU8(__eve) || 3<i){
                return AC_FALSE;
            }
        }
        return AC_TRUE;
    }
    return AC_FALSE;
}

/*
 * 有效整形字符串：
 * isAcU8
 * isAcU16
 * isAcU32
 * isAcU64
 *
 * isAc8
 * isAc16
 * isAc32
 * isAc64
 *
 * */

#ifndef FOR_NUM
#define FOR_NUM(__type) do{for(; i<len; tmp %= (__type)(pow(_BASE_DEC, len-i-1)), i++)\
                            if(((AC_U8)(tmp/((__type)(pow(_BASE_DEC, len-i-1))))) < LETTER2NUM(num[i]))\
                                return AC_FALSE;\
                            else if(((AC_U8)(tmp/((__type)(pow(_BASE_DEC, len-i-1))))) == LETTER2NUM(num[i]))\
                                continue;\
                            else return AC_TRUE;\
                         return AC_TRUE;}while(0)
#endif

AC_BOOL isAcU8(char * num){
    AC_U8 tmp=MAX_AC_U8;
    AC_U8 len, i=0;
    if(num && LEN_DEC_AC_U8 >= (len=strlen(num))
            && strIsType(num, isDec) && len){
        FOR_NUM(AC_U8);
       return AC_TRUE;
    }
    return AC_FALSE;
}

AC_BOOL isAcU16(char * num){
    AC_U16 tmp=MAX_AC_U16;
    AC_U16 len=strlen(num), i=0;
    if(num && LEN_DEC_AC_U16 >= (len=strlen(num))
            && strIsType(num, isDec) && len){
        FOR_NUM(AC_U16);
       return AC_TRUE;
    }
    return AC_FALSE;
}

AC_BOOL isAcU32(char * num){
    AC_U32 tmp=MAX_AC_U32;
    AC_U32 len=strlen(num), i=0;
    if(num && LEN_DEC_AC_U32 >= (len=strlen(num))
            && strIsType(num, isDec) && len){
        FOR_NUM(AC_U32);
       return AC_TRUE;
    }
    return AC_FALSE;
}

AC_BOOL isAcU64(char * num){
    AC_U64 tmp=MAX_AC_U64;
    AC_U64 len=strlen(num), i=0;
    if(num && LEN_DEC_AC_U64 >= (len=strlen(num))
            && strIsType(num, isDec) && len){
        FOR_NUM(AC_U64);
       return AC_TRUE;
    }
    return AC_FALSE;
}

#ifndef FOR_NUM_2
#define FOR_NUM_2(__type) do{if('-' == num[0]){\
                                num++;\
                                len--;\
                                tmp++;\
                                }\
                            if(strIsType(num, isDec) && len){\
                                FOR_NUM(__type);\
                            }else{return AC_FALSE;}}while(0)
#endif

AC_BOOL isAc8(char * num){
    AC_U8 tmp=(AC_U8)(MAX_AC_8), i=0, len;
    if(num && LEN_DEC_AC_8 >= (len=strlen(num))){
        FOR_NUM_2(AC_U8);
    }
    return AC_FALSE;
}

AC_BOOL isAc16(char * num){
    AC_U16 tmp=(AC_U16)(MAX_AC_16), i=0, len;
    if(num && LEN_DEC_AC_16 >= (len=strlen(num))){
        FOR_NUM_2(AC_U16);
    }
    return AC_FALSE;
}

AC_BOOL isAc32(char * num){
    AC_U32 tmp=(AC_U32)(MAX_AC_32), i=0, len;
    if(num && LEN_DEC_AC_32 >= (len=strlen(num))){
        FOR_NUM_2(AC_U32);
    }
    return AC_FALSE;
}

AC_BOOL isAc64(char * num){
    AC_U64 tmp=(AC_U64)(MAX_AC_64), i=0, len;
    if(num && LEN_DEC_AC_64 >= (len=strlen(num))){
        FOR_NUM_2(AC_U64);
    }
    return AC_FALSE;
}

/*
 * 字符串中字符有效性：
 * strIsType：字符串中字符是否符合type
 * strInBase：字符串中字符是否在字符序列中
 * arrayIsType：字符数组中字符是否符合type
 * arrayInBase：字符数组中字符是否在字符序列中
 *
 * */

AC_BOOL strIsType(char * str, isType * isType){
    if(str && isType){
        for(; *str; str++)
            if(!isType(*str))
                return AC_FALSE;
        return AC_TRUE;
    }
    return AC_FALSE;
}

AC_BOOL strInBase(char * str, char * baseSet, int baseLen){
    if(str && baseSet){
        for(; *str; str++)
            if(!inBase(*str, baseSet, baseLen))
                return AC_FALSE;
        return AC_TRUE;
    }
    return AC_FALSE;
}

AC_BOOL arrayIsType(char * array, int len, isType * isType){
    if(array && len && isType){
        for(len--; len>=0; len--)
            if(!isType(array[len]))
                return AC_FALSE;
        return AC_TRUE;
    }
    return AC_FALSE;
}

AC_BOOL arrayInBase(char * array, int len, char * baseSet, int baseLen){
    if(array && len && baseSet && baseLen){
        for(len--; len>0; len--)
            if(inBase(array[len], baseSet, baseLen))
                return AC_FALSE;
        return AC_TRUE;
    }
    return AC_FALSE;
}

/*
 * 类型转化
 *
 * strToMac:字符串格式转AC_MAC
 * strToIpv4:字符串格式转AC_IPV4
 *
 * strToAc8
 * strToAc16
 * strToAc32
 * strToAc64
 *
 * strToAcU8
 * strToAcU16
 * strToAcU32
 * strToAcU64
 *
 * macToStr:     cs_mac转字符串格式，无需释放内存
 * ipv4ToStr:     cs_ip4转字符串格式，无需释放内存
 *
 * ac8ToStr:     AC_8转字符串格式，  无需释放内存
 * ac16ToStr:    AC_16转字符串格式， 无需释放内存
 * ac32ToStr:    AC_32转字符串格式， 无需释放内存
 * ac64ToStr:    AC_64转字符串格式， 无需释放内存
 * acU8ToStr:    AC_U8转字符串格式， 无需释放内存
 * acU16ToStr:   AC_U16转字符串格式，无需释放内存
 * acU32ToStr:   AC_U32转字符串格式，无需释放内存
 * acU64ToStr:   AC_U64转字符串格式，无需释放内存
 *
 * */

#ifndef STR_NUMBER
#define STR_NUMBER(__type) do{AC_U8 i = 0;\
                            __type dst = 0;\
                            AC_U8 len = strlen(str);\
                            for(; i<len; i++)\
                                dst += (__type)((__type)(pow(_BASE_DEC, len-i-1))*LETTER2NUM(str[i]));\
                            return dst;}while(0)
#endif

AC_U8 strToAcU8(char * str){
    STR_NUMBER(AC_U8);
}

AC_U16 strToAcU16(char * str){
    STR_NUMBER(AC_U16);
}

AC_U32 strToAcU32(char * str){
    STR_NUMBER(AC_U32);
}

AC_U64 strToAcU64(char * str){
    STR_NUMBER(AC_U64);
}

#ifndef STR_NUMBER_2
#define STR_NUMBER_2(__type)      __type dst = 0;\
                                AC_U8 i = 0, len = strlen(str);\
                                if('-' == *str){\
                                    len--;\
                                    str++;\
                                    for(; i<len; i++)\
                                        dst -= (__type)((__type)(pow(_BASE_DEC, len-i-1))*LETTER2NUM(str[i]));\
                                }else{\
                                    for(; i<len; i++)\
                                        dst += (__type)((__type)(pow(_BASE_DEC, len-i-1))*LETTER2NUM(str[i]));\
                                    }\
                                return dst;
#endif

AC_8 strToAc8(char * str){
    STR_NUMBER_2(AC_8);
}

AC_16 strToAc16(char * str){
    STR_NUMBER_2(AC_16);
}

AC_32 strToAc32(char * str){
    STR_NUMBER_2(AC_32);
}

AC_64 strToAc64(char * str){
    STR_NUMBER_2(AC_64);
}

/*
 * ff:ff:ff:ff:ff:ff
 * 0134679acdfh
 * 0123456789ab
 *
 * ffff-ffff-ffff
 * 01235678abcd
 * 0123456789ab
 *
 * */

AC_MAC strToMac(char * str){
    AC_U8 i = 0;
    static char mac[sizeof(AC_MAC)];
    memset(mac, 0, sizeof(mac));
    if(str){
        switch(strlen(str)){
            case (sizeof("ff:ff:ff:ff:ff:ff") - 1):
                for(; i < sizeof(AC_MAC); i++)
                    mac[i] = LETTER2NUM(str[i*3]) * 0x10 + LETTER2NUM(str[i*3+1]);
                break;
            case (sizeof("ffff-ffff-ffff") - 1):
                for(; i < sizeof(AC_MAC); i++)
                    mac[i] = LETTER2NUM(str[i*2+i/2]) * 0x10 + LETTER2NUM(str[i*2+i/2+1]);
                break;
            default:
                break;
        }
    }
    return *(AC_MAC *)(mac);
}

AC_IPV4 strToIpv4(char * str){
    return *(AC_IPV4 *)(str);
}

/*
 * 转字符串格式
 *
 * */

char * ac8ToStr(AC_8 num){
    static char str[SIZE_DEC_AC_8];
    memset(str, 0, sizeof(str));
    sprintf(str, "%d", num);
    return (char *)(str);
}

char * ac16ToStr(AC_16 num){
    static char str[SIZE_DEC_AC_16];
    memset(str, 0, sizeof(str));
    sprintf(str, "%d", num);
    return (char *)(str);
}

char * ac32ToStr(AC_32 num){
    static char str[SIZE_DEC_AC_32];
    memset(str, 0, sizeof(str));
    sprintf(str, "%d", num);
    return (char *)(str);
}

char * ac64ToStr(AC_64 num){
    static char str[SIZE_DEC_AC_64];
    memset(str, 0, sizeof(str));
    sprintf(str, "%ld", num);
    return (char *)(str);
}

char * acU8ToStr(AC_U8 num){
    static char str[SIZE_DEC_AC_U8];
    memset(str, 0, sizeof(str));
    sprintf(str, "%u", num);
    return (char *)(str);
}

char * acU16ToStr(AC_U16 num){
    static char str[SIZE_DEC_AC_U16];
    memset(str, 0, sizeof(str));
    sprintf(str, "%u", num);
    return (char *)(str);
}

char * acU32ToStr(AC_U32 num){
    static char str[SIZE_DEC_AC_U32];
    memset(str, 0, sizeof(str));
    sprintf(str, "%u", num);
    return (char *)(str);
}

char * acU64ToStr(AC_U64 num){
    static char str[SIZE_DEC_AC_U64];
    memset(str, 0, sizeof(str));
    sprintf(str, "%lu", num);
    return (char *)(str);
}

/*
 * _BASE_MAC_2: ffff-ffff-ffff
 * _BASE_MAC_3: ff-ff-ff-ff-ff-ff
 * _BASE_MAC_4: ff:ff:ff:ff:ff:ff
 *
 * */

char * macToStr(AC_MAC mac, MAC_BASE base){
    static char str[sizeof("ff:ff:ff:ff:ff:ff")];
    memset(str, 0, sizeof(str));
#ifndef MAC_INDEX
#define MAC_INDEX P_INDEX(mac, 0), P_INDEX(mac, 1), P_INDEX(mac, 2),\
                    P_INDEX(mac, 3), P_INDEX(mac, 4), P_INDEX(mac, 5)
#endif
    switch(base){
        case _BASE_MAC_2:
            sprintf(str, "%02x%02x-%02x%02x-%02x%02x", MAC_INDEX);
            return (char *)(str);
            break;
        case _BASE_MAC_3:
            sprintf(str, "%02x-%02x-%02x-%02x-%02x-%02x", MAC_INDEX);
            return (char *)(str);
            break;
        case _BASE_MAC_4:
            sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", MAC_INDEX);
            return (char *)(str);
            break;
        default:
            return NULL;
    }
}


char * ipv4ToStr(AC_IPV4 ipv4){
    static char str[sizeof("255.255.255.255")];
    memset(str, 0, sizeof(str));
    IPV42STR(str, ipv4);
    return (char *)(str);
}
