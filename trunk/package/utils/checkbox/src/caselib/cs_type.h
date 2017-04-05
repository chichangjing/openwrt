#ifndef __CS_TYPE_H__
#define __CS_TYPE_H__

#include <stdbool.h>
#include <string.h>
#include <math.h>

/*
 * 整形数据
 * cs_8：  8位长， 有符号整数，-128～127
 * cs_16： 16位长，有符号整数，-32768～32767
 * cs_32： 32位长，有符号整数，-2147483648～2147483647
 * cs_64： 64位长，有符号整数，-9223372036854775808～9223372036854775807
 * cs_u8： 8位长， 无符号整数，0～255
 * cs_u16：16位长，无符号整数，0～65535
 * cs_u32：32位长，无符号整数，0～4294967295
 * cs_u64：64位长，无符号整数，0～18446744073709551615
 *
 * */

typedef signed char         cs_8;
typedef signed short int    cs_16;
typedef signed int          cs_32;
typedef signed long long    cs_64;

typedef unsigned char       cs_u8;
typedef unsigned short int  cs_u16;
typedef unsigned int        cs_u32;
typedef unsigned long long  cs_u64;

#define MAX_CS8          (cs_u8)((0xff-1)/2)
#define MAX_CS16         (cs_u16)((0xffff-1)/2)
#define MAX_CS32         (cs_u32)((0xffffffff-1)/2)
#define MAX_CS64         (cs_u64)((0xffffffffffffffff-1)/2)

#define MIN_CS8          (cs_8)(0x00-MAX_CS8-1)
#define MIN_CS16         (cs_16)(0x0000-MAX_CS16-1)
#define MIN_CS32         (cs_32)(0x00000000-MAX_CS32-1)
#define MIN_CS64         (cs_64)(0x0000000000000000-MAX_CS64-1)

#define MAX_CSU8         0xff
#define MAX_CSU16        0xffff
#define MAX_CSU32        0xffffffff
#define MAX_CSU64        0xffffffffffffffff

#define MIN_CSU8         0x00
#define MIN_CSU16        0x0000
#define MIN_CSU32        0x00000000
#define MIN_CSU64        0x0000000000000000

#define LEN_DEC_CS8      strlen("-128")
#define LEN_DEC_CS16     strlen("-32768")
#define LEN_DEC_CS32     strlen("-2147483648")
#define LEN_DEC_CS64     strlen("-9223372036854775808")

#define LEN_DEC_CSU8     strlen("255")
#define LEN_DEC_CSU16    strlen("65535")
#define LEN_DEC_CSU32    strlen("4294967295")
#define LEN_DEC_CSU64    strlen("18446744073709551615")

#define SIZE_DEC_CS8     sizeof("-128")
#define SIZE_DEC_CS16    sizeof("-32768")
#define SIZE_DEC_CS32    sizeof("-2147483648")
#define SIZE_DEC_CS64    sizeof("-9223372036854775808")

#define SIZE_DEC_CSU8    sizeof("255")
#define SIZE_DEC_CSU16   sizeof("65535")
#define SIZE_DEC_CSU32   sizeof("4294967295")
#define SIZE_DEC_CSU64   sizeof("18446744073709551615")

#define BASE02      "01"
#define BASE08      "01234567"
#define BASE10      "0123456789"
#define BASE16      "0123456789ABCDEFabcdef"
#define BASENUM     "0123456789"
#define BASEA2Z     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define BASEa2z     "abcdefghijklmnopqrstuvwxyz"
#define BASE_       "!\"#$%&\'()*+,-./:;<=>\?@[\\]^_`{|}~"

typedef struct {
    char provider[3];
    char device_id[3];
}cs_mac;

typedef struct {
    char ip[4];
} cs_ip4;

#define letter2num(__char) ((cs_8)(__char>='0' && __char<='9') ? (__char-'0')\
                :((__char>='a' && __char<='f') ? (__char-'a'+10)\
                :((__char>='A' && __char<='F') ? ((__char-'A'+10)):(0))))

/*
 * _base_mac_2: ffff-ffff-ffff
 * _base_mac_3: ff-ff-ff-ff-ff-ff
 * _base_mac_4: ff:ff:ff:ff:ff:ff
 *
 * */

enum {
    _base_bin = 2,
    _base_oct = 8,
    _base_dec = 10,
    _base_hex = 16,
    _base_mac_2,
    _base_mac_3,
    _base_mac_4,
    _base_ip,
    _base_max,
};

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

typedef bool is_type(char _letter);

bool is_letter(char _letter);
bool is_upper(char _letter);
bool is_lower(char _letter);
bool is_bin(char _letter);
bool is_oct(char _letter);
bool is_dec(char _letter);
bool is_hex(char _letter);
bool is_sym(char _letter);
bool in_base(char _letter, char * baseset, int base_len);

/*
 * 有效的mac地址字符串，自动支持如下格式：
 * ff:ff:ff:ff:ff:ff
 * ff-ff-ff-ff-ff-ff
 * ffff-ffff-ffff
 *
 * */

bool is_mac(char * _mac);

/*
 * 有效的ip4地址字符串，支持类似下面的地址：
 * 255.255.255.255
 * 255.100.10.0
 * 1.1.1.1
 *
 * */

bool is_ip4(char * _ip);

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

bool is_csu8(char * _num);
bool is_csu16(char * _num);
bool is_csu32(char * _num);
bool is_csu64(char * _num);

bool is_cs8(char * _num);
bool is_cs16(char * _num);
bool is_cs32(char * _num);
bool is_cs64(char * _num);

/*
 * 字符串中字符有效性：
 * _str_is_type：字符串中字符是否符合type
 * _str_in_base：字符串中字符是否在字符序列中
 * _array_is_type：字符数组中字符是否符合type
 * _array_in_base：字符数组中字符是否在字符序列中
 *
 * */

bool _str_is_type(char * _str, is_type * is_type);
bool _str_in_base(char * _str, char * baseset, int base_len);
bool _array_is_type(char * _array, int len, is_type * is_type);
bool _array_in_base(char * _array, int len, char * baseset, int base_len);

/*
 * 类型转化
 * str2int:
 * str2mac:字符串格式转cs_mac
 * str2ip4:字符串格式转cs_ip4
 *
 * mac2str:     cs_mac转字符串格式，无需释放内存
 * ip42str:     cs_ip4转字符串格式，无需释放内存
 * cs82str:     cs_8转字符串格式，  无需释放内存
 * cs162str:    cs_16转字符串格式， 无需释放内存
 * cs322str:    cs_32转字符串格式， 无需释放内存
 * cs642str:    cs_64转字符串格式， 无需释放内存
 * csu82str:    cs_u8转字符串格式， 无需释放内存
 * csu162str:   cs_u16转字符串格式，无需释放内存
 * csu322str:   cs_u32转字符串格式，无需释放内存
 * csu642str:   cs_u64转字符串格式，无需释放内存
 *
 * */
cs_mac str2mac(char * _str);
cs_ip4  str2ip4(char * _str);

cs_u8  str2csu8(char * str);
cs_u16 str2csu16(char * str);
cs_u32 str2csu32(char * str);
cs_u64 str2csu64(char * str);
cs_8   str2cs8(char * str);
cs_16  str2cs16(char * str);
cs_32  str2cs32(char * str);
cs_64  str2cs64(char * str);

char * ip42str(cs_ip4 __cs_ip4);
char * mac2str(cs_mac _mac, int base_);

char * cs82str(cs_8 _num);
char * cs162str(cs_16 _num);
char * cs322str(cs_32 _num);
char * cs642str(cs_64 _num);
char * csu82str(cs_u8 _num);
char * csu162str(cs_u16 _num);
char * csu322str(cs_u32 _num);
char * csu642str(cs_u64 _num);

#define __p_index(__p, __index) (unsigned char)(((char *)(&__p))[__index])
#define __mac2str(__str, __mac) do{sprintf(__str, "%02x:%02x:%02x:%02x:%02x:%02x", \
        __p_index(__mac, 0), __p_index(__mac, 1), __p_index(__mac, 2),\
        __p_index(__mac, 3), __p_index(__mac, 4), __p_index(__mac, 5));}while(0)
#define __ip42str(__str, __ip4) do{sprintf(__str, "%d.%d.%d.%d", \
        __p_index(__ip4, 0), __p_index(__ip4, 1), __p_index(__ip4, 2), __p_index(__ip4, 3));}while(0)

#endif
