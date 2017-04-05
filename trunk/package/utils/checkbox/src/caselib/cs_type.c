#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cs_type.h"

/*
 * 字符有效性：
 * is_letter：字母，包括数字、大写字母、小写字母
 * is_upper：大写字母
 * is_lower：小写字母
 * is_bin：2进制字符
 * is_oct：8进制字符
 * is_dec：10进制字符
 * is_hex：16进制字符
 * is_sym：符号
 * in_base：检查字符是否在字符串中
 *
 * */

bool is_letter(char _letter){
    return ('0' <= _letter && '9' >= _letter)
         ||('A' <= _letter && 'Z' >= _letter)
         ||('a' <= _letter && 'z' >= _letter) ? true:false;
}

bool is_upper(char _letter){
    return 'A' <= _letter && 'Z' >= _letter ? true:false;
}

bool is_lower(char _letter){
    return 'a' <= _letter && 'z' >= _letter ? true:false;
}

bool is_bin(char _letter){
    return '0' == _letter || '1' == _letter ? true:false;
}

bool is_oct(char _letter){
    return '0' <= _letter && '7' >= _letter ? true:false;
}

bool is_dec(char _letter){
    return '0' <= _letter && '9' >= _letter ? true:false;
}

bool is_hex(char _letter){
    return ('0' <= _letter && '9' >= _letter)
         ||('A' <= _letter && 'F' >= _letter)
         ||('a' <= _letter && 'f' >= _letter) ? true:false;
}

bool is_sym(char _letter){
    return ('!' <= _letter && '/' >= _letter)
         ||(':' <= _letter && '@' >= _letter)
         ||('[' <= _letter && '`' >= _letter)
         ||('{' <= _letter && '~' >= _letter) ? true:false;
}

bool in_base(char _letter, char * baseset, int base_len){
    if(baseset && base_len)
        for(base_len--; base_len>=0; base_len--)
            if(_letter==baseset[base_len])
                return true;
    return false;
}

/*
 * 有效的mac地址字符串，自动支持如下格式：
 * ff:ff:ff:ff:ff:ff
 * ff-ff-ff-ff-ff-ff
 * ffff-ffff-ffff
 *
 * */

bool is_mac(char * _mac){
    char i = 0;
    if(_mac){
        switch(strlen(_mac)){
            case (int)(sizeof("ff:ff:ff:ff:ff:ff") - 1):
                for(i=0; i <(sizeof("ff:ff:ff:ff:ff:ff") - 1); i++){
                    if(2==i%3){
                        if(!in_base((char)(_mac[i]), ":-", sizeof(":-")-1))
                            return false;
                        continue;
                    }
                    if(!is_hex((char)(_mac[i])))
                        return false;
                }
                return true;
                break;
            case (int)(sizeof("ffff-ffff-ffff") - 1):
                for(i=0; i < (sizeof("ffff-ffff-ffff") - 1); i++){
                    if(4==i%5){
                        if('-' != (char)(_mac[i]))
                            return false;
                        continue;
                    }
                    if(!is_hex((char)(_mac[i])))
                        return false;
                }
                return true;
                break;
            default:
                return false;
                break;
        }
        return true;
    }
    return false;
}

/*
 * 有效的ip4地址字符串，支持类似下面的地址：
 * 255.255.255.255
 * 255.100.10.0
 * 1.1.1.1
 *
 * */

bool is_ip4(char * _ip){
    char tmp[sizeof("255.255.255.255")];
    memset(tmp, 0, sizeof(tmp));
    strcpy(tmp, _ip);
    char * __eve;
    cs_u8 i, len;
    if(_ip && (len = strlen(_ip))){
        for(i=0, __eve=strtok(tmp, ".");
                __eve;
                __eve=strtok(NULL, "."), i++){
            if(!is_csu8(__eve) || 3<i){
                return false;
            }
        }
        return true;
    }
    return false;
}

/*
 * 有效整形字符串：
 * is_csu8
 * is_csu16
 * is_csu32
 * is_csu64
 *
 * is_cs8
 * is_cs16
 * is_cs32
 * is_cs64
 *
 * */

#ifndef _for_num
#define _for_num(__type) do{for(; i<len; tmp %= (__type)(pow(_base_dec, len-i-1)), i++)\
                            if(((cs_u8)(tmp/((__type)(pow(_base_dec, len-i-1))))) < letter2num(_num[i]))\
                                return false;\
                            else if(((cs_u8)(tmp/((__type)(pow(_base_dec, len-i-1))))) == letter2num(_num[i]))\
                                continue;\
                            else return true;\
                         return true;}while(0)
#endif

bool is_csu8(char * _num){
    cs_u8 tmp=MAX_CSU8;
    cs_u8 len, i=0;
    if(_num && LEN_DEC_CSU8 >= (len=strlen(_num))
            && _str_is_type(_num, is_dec) && len){
       _for_num(cs_u8);
       return true;
    }
    return false;
}

bool is_csu16(char * _num){
    cs_u16 tmp=MAX_CSU16;
    cs_u16 len=strlen(_num), i=0;
    if(_num && LEN_DEC_CSU16 >= (len=strlen(_num))
            && _str_is_type(_num, is_dec) && len){
       _for_num(cs_u16);
       return true;
    }
    return false;
}

bool is_csu32(char * _num){
    cs_u32 tmp=MAX_CSU32;
    cs_u32 len=strlen(_num), i=0;
    if(_num && LEN_DEC_CSU32 >= (len=strlen(_num))
            && _str_is_type(_num, is_dec) && len){
       _for_num(cs_u32);
       return true;
    }
    return false;
}

bool is_csu64(char * _num){
    cs_u64 tmp=MAX_CSU64;
    cs_u64 len=strlen(_num), i=0;
    if(_num && LEN_DEC_CSU64 >= (len=strlen(_num))
            && _str_is_type(_num, is_dec) && len){
       _for_num(cs_u64);
       return true;
    }
    return false;
}

#ifndef _for_num_2
#define _for_num_2(__type) do{if('-' == _num[0]){\
                                _num++;\
                                len--;\
                                tmp++;\
                                }\
                            if(_str_is_type(_num, is_dec) && len){\
                                _for_num(__type);\
                            }else{return false;}}while(0)
#endif

bool is_cs8(char * _num){
    cs_u8 tmp=(cs_u8)(MAX_CS8), i=0, len;
    if(_num && LEN_DEC_CS8 >= (len=strlen(_num))){
        _for_num_2(cs_u8);
    }
    return false;
}

bool is_cs16(char * _num){
    cs_u16 tmp=(cs_u16)(MAX_CS16), i=0, len;
    if(_num && LEN_DEC_CS16 >= (len=strlen(_num))){
        _for_num_2(cs_u16);
    }
    return false;
}

bool is_cs32(char * _num){
    cs_u32 tmp=(cs_u32)(MAX_CS32), i=0, len;
    if(_num && LEN_DEC_CS32 >= (len=strlen(_num))){
        _for_num_2(cs_u32);
    }
    return false;
}

bool is_cs64(char * _num){
    cs_u64 tmp=(cs_u64)(MAX_CS64), i=0, len;
    if(_num && LEN_DEC_CS64 >= (len=strlen(_num))){
        _for_num_2(cs_u64);
    }
    return false;
}

/*
 * 字符串中字符有效性：
 * _str_is_type：字符串中字符是否符合type
 * _str_in_base：字符串中字符是否在字符序列中
 * _array_is_type：字符数组中字符是否符合type
 * _array_in_base：字符数组中字符是否在字符序列中
 *
 * */

bool _str_is_type(char * _str, is_type * is_type){
    if(_str && is_type){
        for(; *_str; _str++)
            if(!is_type(*_str))
                return false;
        return true;
    }
    return false;
}

bool _str_in_base(char * _str, char * baseset, int base_len){
    if(_str && baseset){
        for(; *_str; _str++)
            if(!in_base(*_str, baseset, base_len))
                return false;
        return true;
    }
    return false;
}

bool _array_is_type(char * _array, int len, is_type * is_type){
    if(_array && len && is_type){
        for(len--; len>=0; len--)
            if(!is_type(_array[len]))
                return false;
        return true;
    }
    return false;
}

bool _array_in_base(char * _array, int len, char * baseset, int base_len){
    if(_array && len && baseset && base_len){
        for(len--; len>0; len--)
            if(in_base(_array[len], baseset, base_len))
                return false;
        return true;
    }
    return false;
}

/*
 * 类型转化
 * str2int:
 * str2mac:
 * str2ip4:
 *
 * int2str:
 * mac2str:
 * ip42str:
 *
 * */

#ifndef _str_sum
#define _str_sum(__type) do{cs_u8 i = 0;\
                            __type dst = 0;\
                            cs_u8 len = strlen(str);\
                            for(; i<len; i++)\
                                dst += (__type)((__type)(pow(_base_dec, len-i-1))*letter2num(str[i]));\
                            return dst;}while(0)
#endif

cs_u8 str2csu8(char * str){
    _str_sum(cs_u8);
}

cs_u16 str2csu16(char * str){
    _str_sum(cs_u16);
}

cs_u32 str2csu32(char * str){
    _str_sum(cs_u32);
}

cs_u64 str2csu64(char * str){
    _str_sum(cs_u64);
}

#ifndef _str_sum_2
#define _str_sum_2(__type)      __type dst = 0;\
                                cs_u8 i = 0, len = strlen(str);\
                                if('-' == *str){\
                                    len--;\
                                    str++;\
                                    for(; i<len; i++)\
                                        dst -= (__type)((__type)(pow(_base_dec, len-i-1))*letter2num(str[i]));\
                                }else{\
                                    for(; i<len; i++)\
                                        dst += (__type)((__type)(pow(_base_dec, len-i-1))*letter2num(str[i]));\
                                    }\
                                return dst;
#endif

cs_8 str2cs8(char * str){
    _str_sum_2(cs_8);
}

cs_16 str2cs16(char * str){
    _str_sum_2(cs_16);
}

cs_32 str2cs32(char * str){
    _str_sum_2(cs_32);
}

cs_64 str2cs64(char * str){
    _str_sum_2(cs_64);
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

cs_mac str2mac(char * _str){
    char i = 0;
    static char _mac[sizeof(cs_mac)];
    memset(_mac, 0, sizeof(_mac));
    if(_str){
        switch(strlen(_str)){
            case (sizeof("ff:ff:ff:ff:ff:ff") - 1):
                for(; i < sizeof(cs_mac); i++)
                    _mac[i] = letter2num(_str[i*3]) * 0x10 + letter2num(_str[i*3+1]);
                break;
            case (sizeof("ffff-ffff-ffff") - 1):
                for(; i < sizeof(cs_mac); i++)
                    _mac[i] = letter2num(_str[i*2+i/2]) * 0x10 + letter2num(_str[i*2+i/2+1]);
                break;
            default:
                break;
        }
    }
    return *(cs_mac *)(_mac);
}

cs_ip4 str2ip4(char * _str){
    return *(cs_ip4 *)(_str);
}

/*
 * 转字符串格式
 *
 * */

char * cs82str(cs_8 _num){
    static char _str[SIZE_DEC_CS8];
    memset(_str, 0, sizeof(_str));
    sprintf(_str, "%d", _num);
    return (char *)(_str);
}

char * cs162str(cs_16 _num){
    static char _str[SIZE_DEC_CS16];
    memset(_str, 0, sizeof(_str));
    sprintf(_str, "%d", _num);
    return (char *)(_str);
}

char * cs322str(cs_32 _num){
    static char _str[SIZE_DEC_CS32];
    memset(_str, 0, sizeof(_str));
    sprintf(_str, "%d", _num);
    return (char *)(_str);
}

char * cs642str(cs_64 _num){
    static char _str[SIZE_DEC_CS64];
    memset(_str, 0, sizeof(_str));
    sprintf(_str, "%ld", _num);
    return (char *)(_str);
}

char * csu82str(cs_u8 _num){
    static char _str[SIZE_DEC_CSU8];
    memset(_str, 0, sizeof(_str));
    sprintf(_str, "%u", _num);
    return (char *)(_str);
}

char * csu162str(cs_u16 _num){
    static char _str[SIZE_DEC_CSU16];
    memset(_str, 0, sizeof(_str));
    sprintf(_str, "%u", _num);
    return (char *)(_str);
}

char * csu322str(cs_u32 _num){
    static char _str[SIZE_DEC_CSU32];
    memset(_str, 0, sizeof(_str));
    sprintf(_str, "%u", _num);
    return (char *)(_str);
}

char * csu642str(cs_u64 _num){
    static char _str[SIZE_DEC_CSU64];
    memset(_str, 0, sizeof(_str));
    sprintf(_str, "%lu", _num);
    return (char *)(_str);
}

/*
 * _base_mac_2: ffff-ffff-ffff
 * _base_mac_3: ff-ff-ff-ff-ff-ff
 * _base_mac_4: ff:ff:ff:ff:ff:ff
 *
 * */

char * mac2str(cs_mac _mac, int base_){
    static char _str[sizeof("ff:ff:ff:ff:ff:ff")];
    memset(_str, 0, sizeof(_str));
#ifndef mac_index
#define mac_index __p_index(_mac, 0), __p_index(_mac, 1), __p_index(_mac, 2),\
                   __p_index(_mac, 3), __p_index(_mac, 4), __p_index(_mac, 5)
#endif
    switch(base_){
        case _base_mac_2:
            sprintf(_str, "%02x%02x-%02x%02x-%02x%02x", mac_index);
            return (char *)(_str);
            break;
        case _base_mac_3:
            sprintf(_str, "%02x-%02x-%02x-%02x-%02x-%02x", mac_index);
            return (char *)(_str);
            break;
        case _base_mac_4:
            sprintf(_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac_index);
            return (char *)(_str);
            break;
        default:
            return NULL;
    }
}

char * ip42str(cs_ip4 __cs_ip4){
    static char _str[sizeof("255.255.255.255")];
    memset(_str, 0, sizeof(_str));
    __ip42str(_str, __cs_ip4);
    return (char *)(_str);
}
