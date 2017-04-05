#include "string.h"
#include "stdio.h"
#include "jws_convert.h"

#define NULL_STRING(x) ((NULL == x) || ('\0' == *x))

/**
 * convert hex to dec
 * @param  hex_digit input hex number character
 * @return           hex nember if ture, -1 if false
 */
static char convert_hex2dec(char hex_digit)
{
	if(('0' <= hex_digit) && (hex_digit <= '9'))
		return (hex_digit - '0');

	if(('a' <= hex_digit) && (hex_digit <= 'f'))
		return (hex_digit - 'a' + 10);

	if(('A' <= hex_digit) && (hex_digit <= 'F'))
		return (hex_digit - 'A' + 10);

	return -1;
}

/**
 * convert string to array number
 * @param  pStr     input string (ex."33-44-66-33")
 * @param  pArray   output array number (ex. char pArray[4] value is {0x33.0x44,0x66,0x33})
 * @param  arrayLen hex array length. (ex. length is 4)
 * @return          [description]
 */
RETSTATUS convert_str2array(const char *pStr, char *pArray, int arrayLen)
{
	short hiDigit;
	short loDigit;
	short tempDigit;
	int index;

	if(NULL_STRING(pStr))
		return ERROR_CONVERSION_NO_VALUE;

	if(NULL == pArray)
		return ERROR_CONVERSION_NO_BUFF;

	for(index = 0; index < arrayLen; index++)
	{
		hiDigit = convert_hex2dec(*(pStr++));

		if (('\0' != *pStr) && ('-' != *pStr))
		{
			loDigit = convert_hex2dec(*(pStr++));
		}
		else
		{
			loDigit = hiDigit;
			hiDigit = 0;
		}

		if ((0 > hiDigit) || (0 > loDigit))
		{
			return ERROR_CONVERSION_INCORRECT_TYPE;
		}

		tempDigit = (hiDigit << 4) + loDigit;
		if ((0 > tempDigit) || (tempDigit > 255))
		{
			return ERROR_CONVERSION_INCORRECT_TYPE;
		}

		if ((index < (arrayLen - 1)) && ('-' != *pStr))
		{
			return ERROR_CONVERSION_INCORRECT_TYPE;
		}

		if (index < arrayLen - 1)
		{
			if ('-' != *pStr)
				return ERROR_CONVERSION_ILLEGAL_VALUE;
		}
		else
		{
			if (0 != *pStr)
				return ERROR_CONVERSION_ILLEGAL_VALUE;
		}

		pStr++;

		pArray[index] = (char) tempDigit;

	}

	return ERROR_CONVERSION_CORRECT;

}

/**
 * convert array number to ascii string
 * @param  pArray   input array number (ex. char *pArray[3]={0x44,0x56,0x77})
 * @param  arrayLen array number length (ex. length is 3)
 * @param  pStr     output ascii string (ex. "44-56-77")
 * @param  strLen   string length (ex. length is 8)
 * @return          [description]
 */
RETSTATUS convert_array2str(const char *pArray, const int arrayLen, char *pStr, int strLen)
{
	int index;

   	if (NULL == pArray) 
   		return ERROR_CONVERSION_NO_VALUE;

   	if (NULL == pStr) 
   		return ERROR_CONVERSION_NO_BUFF;

   	if (arrayLen*3 > strLen)
   		return ERROR_CONVERSION_OVERFLOW_BUFF;

   	for(index = 0; index < arrayLen; index++)
   	{
   		sprintf(pStr, "%2.2x",(unsigned char ) pArray[index]);

   		if(index >= arrayLen - 1)
   		{
   			continue;
   		}
   		else
   		{
   			pStr = pStr + 2;
   			sprintf(pStr, "-");
   			pStr++;
   		}
   		
   	}
   
   	return ERROR_CONVERSION_CORRECT;
}

/**
 * count the number of char in string
 * @param  str [description]
 * @param  ch  [description]
 * @return     [description]
 */
int count_char(char *str, char ch)
{
	int count = 0;
	while(*str)
	{
		if (*str++ == ch)
		{
			count++;
		}
	}

	return count;
}