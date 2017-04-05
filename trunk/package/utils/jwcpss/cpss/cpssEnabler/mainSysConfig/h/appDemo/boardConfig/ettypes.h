/*****************************************************************************
 * ettypes.h       
 *
 * Copyright (c) 1993-2000, Spirent Communications of Calabasas, Inc.
 * Copyright (c) 1993-1999, Netcom Systems, Inc.
 ****************************************************************************/

#ifndef _etTypes_h_
#define _etTypes_h_

#if   defined WIN32

	#if   defined WIN32_DLL_COMPILE
		#if   defined __BORLANDC__
			#if   __BORLANDC__ > 0x0460
				#define DLL_EXPORT __declspec(dllexport) __stdcall
			#else
				#define DLL_EXPORT  _export __stdcall
			#endif
		#else
			#define DLL_EXPORT __declspec(dllexport) __stdcall
		#endif
	#else
		#if   defined WIN32_STATIC
			#define DLL_EXPORT __stdcall
		#elif   defined __BORLANDC__
			#if   __BORLANDC__ > 0x0460
				#define DLL_EXPORT __declspec(dllimport) __stdcall
			#else
				#define DLL_EXPORT  _import __stdcall
			#endif
		#else
			#define DLL_EXPORT __declspec(dllimport) __stdcall
		#endif
	#endif

#elif defined WIN16 || defined _Windows || defined _WINDOWS || defined _WINDLL

	#if !defined WIN16
		#define WIN16
	#endif
	#define DLL_EXPORT _export _far _pascal

#else

	#define DLL_EXPORT

#endif

#if defined WIN16 || defined WIN32
	#include <windows.h>
	
	#ifdef interface
		#undef interface
	#endif
#endif

/*****************************************************************************
	The definitions of TRUE and FALSE as shown below might be re-defined in
	another header file, but they should be identical to what is defined	below.
*****************************************************************************/
#ifndef TRUE
	#define TRUE		1
#endif
#ifndef FALSE
	#define FALSE		0
#endif

/* [FINDME][SergeyR]: removed */
/*typedef int BOOL;*/

/*--------------------------------------------------------------------*/
/*------------------- Utility Data Structures ------------------------*/
/*--------------------------------------------------------------------*/
#if defined WIN32
#define int64  __int64
#define uint64 unsigned __int64
#endif

#ifndef int64
#define int64 long long
#define uint64 unsigned long long
#endif

typedef union tagINT64_T {
    struct {
        unsigned long low;
        long      high;
    } i64;
    int64 int64_t;
} INT64_T;


typedef struct tagHubSlotPort {
	int	iHub;
	int	iSlot;
	int	iPort;
	} _HubSlotPort;

typedef struct tagU64 {
	unsigned long	high;
	unsigned long	low;
	} U64;

typedef struct tagU8
	{
	unsigned char uc;
	} U8;

typedef struct tagU16
	{
	unsigned short ui;
	} U16;

typedef struct tagU32
	{
	unsigned long ul;
	} U32;

typedef struct tagS8
	{
	char c;
	} S8;

typedef struct tagS16
	{
	short i;
	} S16;

typedef struct tagS32
	{
	long l;
	} S32;

typedef struct tagF32
	{
	float f;
	} F32;

typedef struct tagF64
	{
	double d;
	} F64;




typedef U8	UChar;
typedef U16	UShort;
typedef U16	UInt;
typedef U32	ULong;
typedef S8	Char;
typedef S16	Short;
typedef S16	Int;
typedef S32	Long;
typedef F32	Float;
typedef F64	Double;
/*typedef BOOL    long;*/

#define 	NSU64IsEq	0x01
#define 	NSU64IsNEq	0x02
#define 	NSU64IsGT	0x03
#define 	NSU64IsLT	0x04
#define 	NSU64IsGE	0x05
#define 	NSU64IsLE	0x06


#ifdef __cplusplus
extern "C" {
#endif
double DLL_EXPORT dU64ToDouble(const U64 *pu64Num);
void DLL_EXPORT u64AssignLowHigh(U64 *pu64Num, unsigned long ulLow, unsigned long ulHigh);
void DLL_EXPORT u64Copy(const U64 *pu64Src, U64 *pu64Dest);
long DLL_EXPORT u64IsEqZero(const U64 *pu64Num);
void DLL_EXPORT u64Add(U64 *pu64Sum, const U64 *pu64Num1, const U64 *pu64Num2);
void DLL_EXPORT u64Subtract(U64 *pu64Diff, const U64 *pu64Num1, const U64 *pu64Num2);
void DLL_EXPORT u64Multiply(U64 *pu64Product, const U64 *pu64Num1, const U64 *pu64Num2);
void DLL_EXPORT u64AddUl(U64 *pu64Sum, const U64 *pu64Num1, unsigned long lNum2);
void DLL_EXPORT u64SubtractUl(U64 *pu64Diff, const U64 *pu64Num1, unsigned long lNum2);
void DLL_EXPORT u64MultiplyUl(U64 *pu64Product, const U64 *pu64Num1, unsigned long lNum2);
double DLL_EXPORT dU64DividedByU64(const U64 *pu64Num1, const U64 *pu64Num2);

/* NS section */
int  DLL_EXPORT NSU64Copy    (U64 *pu64Dest, const U64 *pu64Src);
long DLL_EXPORT NSU64Compare (const unsigned short uiComparator, const U64 *pu64Val1, const U64 *pu64Val2);
int  DLL_EXPORT NSU64Add     (const U64 *pu64Val1, const U64 *pu64Val2, U64* pu64Sum);
int  DLL_EXPORT NSU64Subtract(const U64 *pu64Val1, const U64 *pu64Val2, U64 *pu64Diff);

/* New SMBU64 functions that do not include overflow checking and return the error code have been added */
int DLL_EXPORT NSU64Multiply(const U64* pu64Val1, const U64* pu64Val2, U64* pu64Product);
int DLL_EXPORT NSU64Divide(const U64* pu64Val1, const U64* pu64Val2, U64* pu64Div); 


#ifdef __cplusplus
}
#endif

#endif

/* New SMBU64 functions that include overflow checking and return the error code have been added */
/*
int DLL_EXPORT NSU64ToDouble(double *dDest, const U64 *pu64Src);
int DLL_EXPORT NSDoubleToU64(U64 *pu64Dest, const double *dSrc);
*/

