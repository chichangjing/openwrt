#include "acRand.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "acType.h"

/*
 * 以秒级的当前时间作为随机种子，1s内突发随机数有可能重复
 * sRand：随机值0～RAND_MAX
 * sRandBool：随机true或者false
 * sRandInt：随机数0～_max,包含_max
 * sRandFloat：随机浮点数
 * sRandChar(str)：字符串中随机一个字母，不包含‘\0’
 * sRandObj(obj)：数组中一个随机对象
 *
 * */

AC_U32 sRand(void){
	S_SRAND;
	return	rand();
}
AC_BOOL sRandBool(void){
	return	sRand()%2;
}
AC_U32 sRandInt(AC_U32 max){
	return	(AC_U32)(sRand()/(_RAND_MAX+0.1)*(max+1));
}
float sRandFloat(void){
	return	sRand()/(_RAND_MAX+0.1);
}

/*
 * 以毫秒级的当前时间作为随机种子，精度更高，突发随机数不重复
 * msRand：随机值0～RAND_MAX
 * msRandBool：随机true或者false
 * msRandInt：随机数0～_max,包含_max
 * msRandFloat：随机浮点数
 * msRandChar(str)：字符串中随机一个字母，不包含‘\0’
 * msRandObj(obj)：数组中一个随机对象
 * msRandRange(min, max)
 * */

AC_U32	msRand(void){
	MS_SRAND;
	return	rand();
}
AC_BOOL msRandBool(void){
	return	msRand()%2;
}
AC_U32	msRandInt(AC_U32 max){
	return	(AC_U32)(msRand()/(_RAND_MAX+0.1)*(max+1));
}
float msRandFloat(void){
	return	msRand()/(_RAND_MAX+0.1);
}
char msRandChar(char * str){
    return str[msRandInt(strlen(str))];
}
/*
 * 生成整形数据
 * randLetter：大写字母，小写字母，数字，符号
 * randUpper：大写字母 A~Z
 * randLower：小写字母 a~z
 * randNumber：数字 0~9
 * randMac地址, 6字节长，0x000000000000~0xffffffffffff
 * randIpv4：ip4地址, 4字节长，0x00000000~0xffffffff
 *
 * */

char randLetter(void){
    return '!'+msRandInt('~'-'!');
}

char randUpper(void){
    return 'A'+msRandInt('Z'-'A');
}

char randLower(void){
    return 'a'+msRandInt('z'-'a');
}

char randNumber(void){
    return '0'+msRandInt('9'-'0');
}

AC_MAC randMac(void){
    AC_U64 mac = randAC_U64();
    return *(AC_MAC*)(&mac);
}

AC_IPV4 randIpv4(void){
    AC_U32 ip4 = randAC_U32();
    return *(AC_IPV4 *)(&(ip4));
}

/*
 * 生成整形数据
 * randAc8：  8位长， 有符号整数，-128～127
 * randAc16： 16位长，有符号整数，-32768～32767
 * randAc32： 32位长，有符号整数，-2147483648～2147483647
 * randAc64： 64位长，有符号整数，-9223372036854775808～9223372036854775807
 * randAcU8： 8位长， 无符号整数，0～255
 * randAcU16：16位长，无符号整数，0～65535
 * randAcU32：32位长，无符号整数，0～4294967295
 * randAcU64：64位长，无符号整数，0～18446744073709551615
 *
 * */

RANDFIXTYPE(AC_8);
RANDFIXTYPE(AC_16);
RANDFIXTYPE(AC_32);
RANDFIXTYPE(AC_64);

RANDFIXTYPE(AC_U8);
RANDFIXTYPE(AC_U16);
RANDFIXTYPE(AC_U32);
RANDFIXTYPE(AC_U64);
