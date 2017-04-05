#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and data that are strictly internal
for the proper functioning of the API on the Marvell 88X3240 
ethernet PHY.
********************************************************************/
#ifndef MTDINTRNL_H
#define MTDINTRNL_H

#include "mtdDiagnostics.h"

#if C_LINKAGE
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

/* Master-Slave Protocol Definitions
 MDIO Register to slave */
#define MAX_BUFF_SIZE_OUT_REG        1,49192 /* MDIO Registers used to communicate with slave code*/
#define ACTUAL_BUFF_SIZE_IN_REG      1,49193
#define COMMAND_REG                  1,49194  
#define WORDS_WRITTEN_REG            1,49195  
#define LOW_ADDRESS_REG              1,49196  
#define HIGH_ADDRESS_REG             1,49197  
#define DATA_REG                     1,49198
#define CHECKSUM_REG                 1,49199
#define WORDS_RCVD_REG               1,49200

/*  Host Commands*/
#define ERASE_FLASH_PROGRAM_AREA  (0x1)
#define FILL_BUFFER               (0x2)
/* #define WRITE_BUFFER              (0x3)*/
/* #define READ_BUFFER               (0x4)*/
/* #define ERASE_FLASH               (0x5)*/
#define WRITE_VERIFY_BUFFER       (0x6)
#define WRITE_BUFFER WRITE_VERIFY_BUFFER  /*  Always does a write/verify*/
#define VERIFY_FLASH (0x7)      /*  Reads flash and makes sure header and checksum*/
                               /*  and app checksum match what's stored in the*/
                               /*  flash header and app area*/


/*  Slave Responses*/
#define MTD_SLAVE_OK          (0x100)
#define MTD_SLAVE_ERR_CMD     (0x200)
#define MTD_SLAVE_FLASH_BUSY  (0x300)
#define MTD_SLAVE_VERIFY_ERR  (0x400)

/*  This macro is handy for calling a function when you want to test the*/
/*  return value and return MTD_FAIL, if the function returned MTD_FAIL,*/
/*  otherwise continue. Will set the return code to MTD_IO_ERROR.*/
#define TRY_IO(xFuncToTry) if (xFuncToTry == MTD_FAIL) {*errCode = MTD_IO_ERROR; return MTD_FAIL;}


MTD_STATUS mtdMdioFlashDownload(CTX_PTR_TYPE contextPtr, MTD_U16 port,
                                MTD_U8 data[],MTD_U32 size,MTD_U16 *errCode);

MTD_STATUS mtdMdioRamDownload(CTX_PTR_TYPE contextPtr,MTD_U8 data[],
                              MTD_U32 size, MTD_U16 port, MTD_U8 use_ram_checksum,
                              MTD_U16 *errCode); 

MTD_STATUS mtdParallelMdioRamDownload(CTX_PTR_TYPE contextPtr, MTD_U8 data[],
                               MTD_U32 size, MTD_U16 ports[],
                               MTD_U16 numPorts, MTD_U16 erroredPorts[], 
                               MTD_U8 use_ram_checksum,
                               MTD_U16 *errCode);

MTD_STATUS mtdParallelMdioFlashDownload(CTX_PTR_TYPE contextPtr, MTD_U8 data[],
                                 MTD_U32 size, 
                                 MTD_U16 ports[], MTD_U16 numPorts,
                                 MTD_U16 erroredPorts[],
                                 MTD_U16 *errCode);

typedef enum
{
    X3240_DEV = 0,
    MTDINVALIDDEV = 10
}DEV_ID;

typedef enum
{
    Z0 = 0,
    Z1 = 1,
    MTDINVALIDREV=8
}DEV_REV;

typedef struct
{
    DEV_ID devID;
    DEV_REV devRev;
}STRUCT_DEVICE_TYPE;

STRUCT_DEVICE_TYPE mtdGetDeviceType(CTX_PTR_TYPE contextPtr, MTD_U16 port);

typedef MTD_U32 MEM_SIZE_BYTES;

MEM_SIZE_BYTES mtdGetDevMemorySize(CTX_PTR_TYPE contextPtr, MTD_U16 port);

/******************************************************************************
MTD_STATUS mtdDoMeasHalfEye
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 curSelLane,
    IN MTD_U16 reg_8xBE,
    OUT MTD_U32 **data
)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    curSelLane - XFI lane number, 0 or 1
    reg_8xBE - center phase location

 Outputs:
    data - 26 x 128 array of 32-bit eye data (readout )

 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    The function is translated from the source code of Marvell's SMI Eye Diagram GUI.
    It is called by mtdGetSerdesEyeStatistics to read Serdes eye diagram data
    from register 30.8012 & 30.8013.

 Side effects:
    None

 Notes/Warnings:
    Eye data can only be collected from active Serdes lanes.
******************************************************************************/
MTD_STATUS mtdDoMeasHalfEye
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 curSelLane,
    IN MTD_U16 reg_8xBE,
    OUT MTD_U32 data[][MTD_EYEDIAGRAM_NCOLS]
);

#if C_LINKAGE
#if defined __cplusplus 
}
#endif 
#endif

#endif /* defined MTDINTRNL_H */
