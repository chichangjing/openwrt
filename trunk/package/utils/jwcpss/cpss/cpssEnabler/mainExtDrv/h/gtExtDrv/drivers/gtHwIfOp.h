
/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtHwIfOp.h
*
* DESCRIPTION:
*       API for HW interface.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*******************************************************************************/
#ifndef __gtHwIfOp
#define __gtHwIfOp

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtExtDrv/os/extDrvOs.h>

/***************************************************************/
/*  DATA TYPES                                                 */
/***************************************************************/

/* OP_CODE Enumeration */
typedef enum
{
  /* SMI operations */
  GT_READ_SMI_BUFFER_E       = 1,
  GT_WRITE_SMI_BUFFER_E,
  
  /* Future co-processor operations */
  GT_COPROC_READ_COUNTERS_E = 20,
  GT_COPROC_READ_MIBS_E,
  GT_COPROC_CLEAR_SRAM_E
  
} GT_OPER_OPCODE_ENT;

/* Descriptor definition for regular Co-Processor operations */
typedef struct GT_COPROC_OPER_STCT
{
  GT_OPER_OPCODE_ENT        opCode;
  GT_U32                    offsetInSram;
  GT_U32                    bufferPtr;
  GT_U32                    bufferSize;
  GT_U32                    opCodeExtention0;
  GT_U32                    opCodeExtention1;
} GT_COPROC_OPER_STC;

/* Descriptor definition for SMI transactions */
typedef struct GT_SMI_OPER_STCT
{
  GT_OPER_OPCODE_ENT        opCode;
  GT_U32                    smiDeviceAddr;
  GT_U32                    regAddress;
  GT_U32                    smiData;
  GT_U32                    opCodeExtention0;
  GT_U32                    opCodeExtention1;
} GT_SMI_OPER_STC;

/* */
typedef GT_VOID (*GT_CALL_BACK_FUN)
(
    GT_VOID     *param
);

/*  */
typedef struct GT_HW_IF_NON_BLOCK_CB_STCT
{
  GT_CALL_BACK_FUN          cbRoutine;
  GT_VOID                   *cbParam;
} GT_HW_IF_NON_BLOCK_CB_STC;

/* This union is the basic data structure used to send\receive information 
    to\from the application layers (PSS-BSP) */
typedef union
{
  GT_COPROC_OPER_STC        *coProc;
  GT_SMI_OPER_STC           *smiOper;
} GT_HW_IF_UNT;


/*******************************************************************************
* extDrvHwIfOperExec
*
* DESCRIPTION:
*       This routine performs co-processor operations according to data in the 
*       operationLength entries. The routine can be blocking or non-blocking.
*
* INPUTS:
*       arrayLength     - Array length.
*       operationArr    - Array of operations to be perform by the co-processor.
*       nonBlockCbPtr   - A struct containing the data needed for non blocking
*                         operation (callback routine and parameter). If NULL,
*                         the routine call is blocking.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvHwIfOperExec
(
    IN   GT_U32                         arrayLength,
    IN   GT_HW_IF_UNT                   operationArr[],
    IN   GT_HW_IF_NON_BLOCK_CB_STC      *nonBlockCbPtr
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtHwIfOp */
