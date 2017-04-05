#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains common types and defines across the Marvell
T-unit (copper) API driver (MTD).
********************************************************************/
#ifndef MTD_TYPES_H
#define MTD_TYPES_H

#define DB_TEST_ENVIRONMENT 0 /* set to 0 for host environment */
                              /* set to 1 to build sample code */
                              /* to interface to Marvell DB via SMI board */

#define C_LINKAGE 1 /* set to 1 if C compile/linkage on C files is desired*/

#if C_LINKAGE
#if defined __cplusplus
    extern "C" {
#endif
#endif

/* general */

#undef IN
#define IN
#undef OUT
#define OUT
#undef INOUT
#define INOUT

#define STATIC static

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef void  MTD_VOID;
typedef char  MTD_8;
typedef short MTD_16; 
typedef long  MTD_32;

typedef unsigned char  MTD_U8;
typedef unsigned short MTD_U16;
typedef unsigned long  MTD_U32;
typedef unsigned int   MTD_UINT; 
typedef int            MTD_INT;
typedef signed short   MTD_S16;

typedef unsigned long long  MTD_U64;

typedef enum {
    MTD_FALSE = 0,
    MTD_TRUE  = 1
} MTD_BOOL;

#define MTD_CONVERT_BOOL_TO_UINT(boolVar,uintVar) {(boolVar) ? (uintVar=1) : (uintVar=0);}
#define MTD_CONVERT_UINT_TO_BOOL(uintVar,boolVar) {(uintVar) ? (boolVar=MTD_TRUE) : (boolVar=MTD_FALSE);}
#define MTD_GET_BOOL_AS_BIT(boolVar) ((boolVar) ? 1 : 0)
#define MTD_GET_BIT_AS_BOOL(uintVar) ((uintVar) ? MTD_TRUE : MTD_FALSE)

typedef void     (*MTD_VOIDFUNCPTR) (void); /* ptr to function returning void */
typedef MTD_U32  (*MTD_INTFUNCPTR)  (void); /* ptr to function returning int  */


/* module state */
typedef enum {
    MTD_STATE_NONE = 0,    /* Uninitialized */
    MTD_STATE_ACTIVE        /* Started */
} MTD_STATE;


typedef MTD_U32 MTD_STATUS;

#define MTD_OK      0    /* Operation succeeded */
#define MTD_FAIL    1    /* Operation failed    */


/* bit definition */
#define MTD_BIT_0       0x0001
#define MTD_BIT_1       0x0002
#define MTD_BIT_2       0x0004
#define MTD_BIT_3       0x0008
#define MTD_BIT_4       0x0010
#define MTD_BIT_5       0x0020
#define MTD_BIT_6       0x0040
#define MTD_BIT_7       0x0080
#define MTD_BIT_8       0x0100
#define MTD_BIT_9       0x0200
#define MTD_BIT_10      0x0400
#define MTD_BIT_11      0x0800
#define MTD_BIT_12      0x1000
#define MTD_BIT_13      0x2000
#define MTD_BIT_14      0x4000
#define MTD_BIT_15      0x8000

/*#define MTD_DEBUG  comment out to remove all print code/data */

#ifdef MTD_DEBUG
typedef enum 
{
    MTD_DBG_OFF_LVL,
    MTD_DBG_ERR_LVL,
    MTD_DBG_ALL_LVL
} MTD_DBG_LEVEL;

void mtdDbgPrint(FILE *stream, MTD_DBG_LEVEL debug_level, char* format, ...);

#define MTD_DBG_ERROR(...) mtdDbgPrint(stderr, MTD_DBG_ERR_LVL, __VA_ARGS__) /* macro for error messages */
#define MTD_DBG_INFO(...)  mtdDbgPrint(stdout, MTD_DBG_ALL_LVL, __VA_ARGS__) /* macro for informational messages */

#else /* MTD_DEBUG not defined */

#define MTD_DBG_ERROR(...)
#define MTD_DBG_INFO(...)

#endif

typedef struct presteraInfo
{
	MTD_U32		devNum;
	MTD_U32		portGroup;
}PRESTERA_INFO;

#if C_LINKAGE
#if defined __cplusplus
}
#endif
#endif

#endif /* MTD_TYPES_H */

