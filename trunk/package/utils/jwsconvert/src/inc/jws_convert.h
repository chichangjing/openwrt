#ifndef __JWS_CONVERT_H
#define __JWS_CONVERT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
	ERROR_CONVERSION_CORRECT,
	ERROR_CONVERSION_INCORRECT_TYPE,
	ERROR_CONVERSION_ILLEGAL_VALUE,
	ERROR_CONVERSION_NO_VALUE,
	ERROR_CONVERSION_NO_BUFF,
	ERROR_CONVERSION_OVERFLOW_BUFF,
}RETSTATUS;

RETSTATUS convert_str2array(const char *pStr, char *pArray, int arrayLen);
RETSTATUS convert_array2str(const char *pArray, int arrayLen, 
							char *pStr, int strLen);
int count_char(char *str, char ch);

int init_power_a(void);
int init_power_b(void);
int init_alarm_k(void);
int init_alarm_led(void);
int init_ring_led(void);
int init_run_led(void);

int power_a_get(int *value);
int power_b_get(int *value);
int alarm_k_set(int value);
int alarm_led_set(int value);
int ring_led_set(int value);
int run_led_set(int value);
    

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __JWS_CONVERT_H */
