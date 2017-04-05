#include <madCopyright.h>
/********************************************************************************
* madSample.h
*
* DESCRIPTION:
*       Types definitions for Sample program
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __MAD_SAMPLE_H
#define __MAD_SAMPLE_H

#include "madApi.h"

#ifdef __cplusplus
extern "C" {
#endif

	
	/* #define SHOW_DEBUG_INFO    1  */
#undef SHOW_DEBUG_INFO  

#ifndef MAD_APP
 #ifdef __KERNEL__
  #include "linux/kernel.h" 
  #define MSG_PRINT(x) printk x
  #ifdef SHOW_DEBUG_INFO
    #define MSG_PRINT_DBG(x) printk x
  #else
    #define MSG_PRINT_DBG(x) printk x
  #endif
 #else
  #include "stdio.h" 
  #define MSG_PRINT(x) printf  x
  #ifdef SHOW_DEBUG_INFO
    #define MSG_PRINT_DBG(x) printf x
  #else
    #define MSG_PRINT_DBG(x)  printf x
  #endif
 #endif
#else
  #include "stdarg.h" 
  #include "stdio.h"
  #include "string.h"
  #include "stdlib.h"
  #define MSG_PRINT(x) printf x
  #ifdef SHOW_DEBUG_INFO
    #define MSG_PRINT_DBG(x) printf x
  #else
    #define MSG_PRINT_DBG(x) printf x
  #endif
#endif

#ifdef __KERNEL__
extern void * memcpy(void *,const void *,__kernel_size_t);
extern int memcmp(const void *,const void *,__kernel_size_t);
#endif
#undef USE_SEMAPHORE

#ifdef USE_SEMAPHORE
MAD_SEM madOsSemCreate(MAD_SEM_BEGIN_STATE state);
MAD_STATUS madOsSemDelete(MAD_SEM smid);
MAD_STATUS madOsSemWait(MAD_SEM smid, MAD_U32 timeOut);
MAD_STATUS madOsSemSignal(MAD_SEM smid);
#endif

MAD_BOOL bspSpecificReadMii ( MAD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                      unsigned int* value);
MAD_BOOL bspSpecificWriteMii ( MAD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                       unsigned int value);


extern void testMADDisplayStatus(MAD_STATUS status);


typedef MAD_U32 (*MAD_API_VOID) (MAD_DEV*);
typedef int (*MAD_CMP_FUNC) (void*, int, int);

typedef MAD_STATUS (*MAD_API_SET_BOOL) (MAD_DEV*, MAD_BOOL);
typedef MAD_STATUS (*MAD_API_GET_BOOL) (MAD_DEV*, MAD_BOOL*);

typedef MAD_STATUS (*MAD_API_SET_U8) (MAD_DEV*, MAD_U8);
typedef MAD_STATUS (*MAD_API_GET_U8) (MAD_DEV*, MAD_U8*);

typedef MAD_STATUS (*MAD_API_SET_U16) (MAD_DEV*, MAD_U16);
typedef MAD_STATUS (*MAD_API_GET_U16) (MAD_DEV*, MAD_U16*);

typedef MAD_STATUS (*MAD_API_SET_U32) (MAD_DEV*, MAD_U32);
typedef MAD_STATUS (*MAD_API_GET_U32) (MAD_DEV*, MAD_U32*);

typedef MAD_STATUS (*MAD_API_SET_PORT_BOOL) (MAD_DEV*, MAD_LPORT,MAD_BOOL);
typedef MAD_STATUS (*MAD_API_GET_PORT_BOOL) (MAD_DEV*, MAD_LPORT,MAD_BOOL*);

typedef MAD_STATUS (*MAD_API_SET_PORT_U16) (MAD_DEV*, MAD_LPORT,MAD_U16);
typedef MAD_STATUS (*MAD_API_GET_PORT_U16) (MAD_DEV*, MAD_LPORT,MAD_U16*);

typedef MAD_STATUS (*MAD_API_SET_PORT_U32) (MAD_DEV*, MAD_LPORT,MAD_U32);
typedef MAD_STATUS (*MAD_API_GET_PORT_U32) (MAD_DEV*, MAD_LPORT,MAD_U32*);

typedef MAD_STATUS (*MAD_API_SET_PORT_U8) (MAD_DEV*, MAD_LPORT,MAD_U8);
typedef MAD_STATUS (*MAD_API_GET_PORT_U8) (MAD_DEV*, MAD_LPORT,MAD_U8*);

typedef struct _TEST_API
{
    union
    {
        MAD_API_SET_BOOL bool;
        MAD_API_SET_U8 u8;
        MAD_API_SET_U16 u16;
        MAD_API_SET_U32 u32;
        MAD_API_SET_PORT_BOOL port_bool;
        MAD_API_SET_PORT_U8 port_u8;
        MAD_API_SET_PORT_U16 port_u16;
        MAD_API_SET_PORT_U32 port_u32;
    } setFunc;

    union
    {
        MAD_API_GET_BOOL bool;
        MAD_API_GET_U8 u8;
        MAD_API_GET_U16 u16;
        MAD_API_GET_U32 u32;
        MAD_API_GET_PORT_BOOL port_bool;
        MAD_API_GET_PORT_U8 port_u8;
        MAD_API_GET_PORT_U16 port_u16;
        MAD_API_GET_PORT_U32 port_u32;
    } getFunc;

}TEST_API;








#ifdef __cplusplus
}
#endif

#endif   /* __MAD_SAMPLE_H */

