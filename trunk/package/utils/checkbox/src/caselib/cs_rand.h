#ifndef __CS_RAND_H__
#define __CS_RAND_H__

#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include "cs_type.h"

/* RAND_MAX=0x7fffffff */

#define _RAND_MAX RAND_MAX

/*
 * 以秒级的当前时间作为随机种子，1s内突发随机数有可能重复
 * _s_rand：随机值0～RAND_MAX
 * _s_bool_rand：随机true或者false
 * _s_int_rand：随机数0～_max,包含_max
 * _s_float_rand：随机浮点数
 * _s_char_rand(str)：字符串中随机一个字母，不包含‘\0’
 * _s_obj_rand(obj)：数组中一个随机对象
 *
 * */

#define _S_SRAND	do{srand((unsigned)time(NULL));}while(0)

int		_s_rand(void);
bool	_s_bool_rand(void);
int 	_s_int_rand(int _max);
float 	_s_float_rand(void);

#define _s_range_rand(__min, __max) (__min+_s_int_rand(__max-__min))
#define _s_char_rand(str)   str[_s_int_rand(strlen(str)-1)]
#define _s_obj_rand(obj)    obj[_s_int_rand(sizeof(obj)/sizeof(obj[0])-1)]

/*
 * 以毫秒级的当前时间作为随机种子，精度更高，突发随机数不重复
 * _s_rand：随机值0～RAND_MAX
 * _s_bool_rand：随机true或者false
 * _s_int_rand：随机数0～_max,包含_max
 * _s_float_rand：随机浮点数
 * _ms_char_rand(str)：字符串中随机一个字母，不包含‘\0’
 * _ms_obj_rand(obj)：数组中一个随机对象
 *
 * */

#define _MS_SRAND	do{struct timeval tv; gettimeofday(&tv, NULL); srand(tv.tv_usec + tv.tv_sec);}while(0)

int 	_ms_rand(void);
bool 	_ms_bool_rand(void);
int 	_ms_int_rand(int _max);
float 	_ms_float_rand(void);

#define _ms_range_rand(__min, __max) (__min+_ms_int_rand(__max-__min))
#define _ms_char_rand(str)  str[_ms_int_rand(strlen(str)-1)]
#define _ms_obj_rand(obj)   obj[_ms_int_rand(sizeof(obj)/sizeof(obj[0])-1)]

/*
 * 生成整形数据
 * rand_letter：大写字母，小写字母，数字，符号
 * rand_upper：大写字母 A~Z
 * rand_lower：小写字母 a~z
 * rand_num：数字 0~9
 * rand_str：从指定字符集合中生成指定长度字符串
 * rand_mac：mac地址, 6字节长，0x000000000000~0xffffffffffff
 * rand_ip4：ip4地址, 4字节长，0x00000000~0xffffffff
 *
 * */

char rand_letter(void);
char rand_upper(void);
char rand_lower(void);
char rand_num(void);
char * rand_str(char * _str, int dlen, char * _src, int _slen);
#define _rand_str(str, src) rand_str(str, (sizeof(str)-1), src, (strlen(src)))
cs_mac  rand_mac(void);
cs_ip4  rand_ip4(void);

/*
 * 生成整形数据
 * rand_cs_8：  8位长， 有符号整数，-128～127
 * rand_cs_16： 16位长，有符号整数，-32768～32767
 * rand_cs_32： 32位长，有符号整数，-2147483648～2147483647
 * rand_cs_64： 64位长，有符号整数，-9223372036854775808～9223372036854775807
 * rand_cs_u8： 8位长， 无符号整数，0～255
 * rand_cs_u16：16位长，无符号整数，0～65535
 * rand_cs_u32：32位长，无符号整数，0～4294967295
 * rand_cs_u64：64位长，无符号整数，0～18446744073709551615
 *
 * */

#define __rand_fixchar(__type)    do{char src[sizeof(__type)]; char len = 0;\
                                    for(;len<sizeof(__type);len++)\
                                    src[len] = _ms_int_rand(0xff);\
                                    return *(__type *)(src);}while(0)
#define __rand_fixtype(__type) __type rand_##__type(void){__rand_fixchar(__type);}

cs_8    rand_cs_8(void);
cs_16   rand_cs_16(void);
cs_32   rand_cs_32(void);
cs_64   rand_cs_64(void);
cs_u8   rand_cs_u8(void);
cs_u16  rand_cs_u16(void);
cs_u32  rand_cs_u32(void);
cs_u64  rand_cs_u64(void);

#endif
