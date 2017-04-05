/*
 * (c), Copyright 2009-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief platform undependent wrapper interface to  platform dependent os functions
 *
 * @file tm_os_interface.h
 *
 * $Revision: 2.0 $
 */
#ifndef TM_OS_INTERFACE_H
#define TM_OS_INTERFACE_H


/* */
void *tm_malloc(unsigned int size);
void  tm_free(void *ptr);
void *tm_memset(void *s, int c, unsigned int n);
void *tm_memcpy(void *dest, const void *src, unsigned int n);


double tm_round(double x);
float  tm_roundf(float x);
int    tm_ceil(float x);
float  tm_fabs_fl(float x);
double tm_fabs_db(double x);
int    tm_abs(int x);



#define TM_MAX_STR_SIZE	1024

int tm_printf(const char *format, ...);


#endif   /* TM_OS_INTERFACE_H */

