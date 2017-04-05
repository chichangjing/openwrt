#include <stdlib.h>
#include "cs_rand.h"
#include "cs_type.h"

/*
 * 以秒级的当前时间作为随机种子，1s内突发随机数有可能重复
 * _s_rand：随机值0～RAND_MAX
 * _s_bool_rand：随机true或者false
 * _s_int_rand：随机数0～_max,包含_max
 * _s_float_rand：随机浮点数
 * _s_char_rand(str)：字符串中随机一个字母，不包含‘\0’
 * _s_obj_rand(obj)：数组中一个随机对象
 * */

int	_s_rand(void){
	_S_SRAND;
	return	rand();
}
bool _s_bool_rand(void){
	return	_s_rand()%2;
}
int _s_int_rand(int _max){
	return	(int)(_s_rand()/(_RAND_MAX+0.1)*(_max+1));
}
float _s_float_rand(void){
	return	_s_rand()/(_RAND_MAX+0.1);
}

/*
 * 以毫秒级的当前时间作为随机种子，精度更高，突发随机数不重复
 * _s_rand：随机值0～RAND_MAX
 * _s_bool_rand：随机true或者false
 * _s_int_rand：随机数0～_max,包含_max
 * _s_float_rand：随机浮点数
 * _ms_char_rand(str)：字符串中随机一个字母，不包含‘\0’
 * _ms_obj_rand(obj)：数组中一个随机对象
 * */

int	_ms_rand(void){
	_MS_SRAND;
	return	rand();
}
bool _ms_bool_rand(void){
	return	_ms_rand()%2;
}
int	_ms_int_rand(int _max){
	return	(int)(_ms_rand()/(_RAND_MAX+0.1)*(_max+1));
}
float _ms_float_rand(void){
	return	_ms_rand()/(_RAND_MAX+0.1);
}

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

char rand_letter(void){
    return '!'+_ms_int_rand('~'-'!');
    //return _ms_obj_rand(BASENUM""BASEA2Z""BASEa2z""BASE_);
}

char rand_upper(void){
    return 'A'+_ms_int_rand('Z'-'A');
}

char rand_lower(void){
    return 'a'+_ms_int_rand('z'-'a');
}

char rand_num(void){
    return '0'+_ms_int_rand('9'-'0');
}

char * rand_str(char * _str, int dlen, char * _src, int _slen){
    if(_str && dlen && _src && _slen)
        for(dlen--; dlen>=0; dlen--)
            _str[dlen] = *(_src+_ms_int_rand(_slen-1));
    return _str;
}

cs_mac rand_mac(void){
    cs_64 _mac = rand_cs_64();
    return *(cs_mac*)(&_mac);
}

cs_ip4 rand_ip4(void){
    cs_32 _ip4 = rand_cs_32();
    return *(cs_ip4 *)(&(_ip4));
}

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

__rand_fixtype(cs_8);
__rand_fixtype(cs_16);
__rand_fixtype(cs_32);
__rand_fixtype(cs_64);

__rand_fixtype(cs_u8);
__rand_fixtype(cs_u16);
__rand_fixtype(cs_u32);
__rand_fixtype(cs_u64);

#if 0
cs_8 rand_cs_8(void){
    __rand_fixchar(cs_8);
}
cs_16 rand_cs_16(void){
    __rand_fixchar(cs_16);
}
cs_32 rand_cs_32(void){
    __rand_fixchar(cs_32);
}
cs_64 rand_cs_64(void){
    __rand_fixchar(cs_64);
}
cs_u8 rand_cs_u8(void){
    __rand_fixchar(cs_u8);
}
cs_u16 rand_cs_u16(void){
    __rand_fixchar(cs_u16);
}
cs_u32 rand_cs_u32(void){
    __rand_fixchar(cs_u32);
}
cs_u64 rand_cs_u64(void){
    __rand_fixchar(cs_u64);
}
#endif
