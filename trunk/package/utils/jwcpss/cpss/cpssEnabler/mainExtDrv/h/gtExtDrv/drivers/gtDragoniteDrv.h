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
* gtDragoniteDrv.h
*
* DESCRIPTION:
*       Includes Dragonite managment routines.
*
* DEPENDENCIES:
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __gtDragoniteDrvh
#define __gtDragoniteDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtExtDrv/os/extDrvOs.h>


#define DRAGONITE_FW_LOAD_ADDR_CNS      0
#define ITCM_DIR                        0
#define DTCM_DIR                        1
#define DRAGONITE_IOC_MAGIC             'd'
#define DRAGONITE_IOC_SETMEM_TYPE       _IOW(DRAGONITE_IOC_MAGIC, 0, bool)
#define DRAGONITE_IOC_UNRESET           _IOW(DRAGONITE_IOC_MAGIC, 1, bool)
#define DRAGONITE_IOC_SENDIRQ           _IOW(DRAGONITE_IOC_MAGIC, 2, bool)

#define DRAGONITE_DATA_MSG_LEN          15

/****************/
/*  RX / TX     */
/****************/
#define TX_MO_ADDR                      0x50
#define TX_MO_HOST_OWNERSHIP_CODE       0xA0
#define TX_MO_POE_OWNERSHIP_CODE        0x0A

#define RX_MO_ADDR                      0x100
#define RX_MO_HOST_OWNERSHIP_CODE       0xB0
#define RX_MO_POE_OWNERSHIP_CODE        0x0B

#define TX_BUF_ADDR                     0x54
#define RX_BUF_ADDR                     0x104

/****************/
/*  D E B U G   */
/****************/
#define TX_DEBUG_MO_ADDR                0x150
#define TX_DEBUG_MO_HOST_OWNERSHIP      0xC0
#define TX_DEBUG_MO_POE_OWNERSHIP       0x0C

#define RX_DEBUG_MO_ADDR                0x650
#define RX_DEBUG_MO_HOST_OWNERSHIP      0xD0
#define RX_DEBUG_MO_POE_OWNERSHIP       0x0D

#define TX_DEBUG_BUF_ADDR               0x154
#define RX_DEBUG_BUF_ADDR               0x654

/****************/
/*  Management  */
/****************/
#define HOST_PROT_VER_REG_ADDR          0x0
#define PROT_VER_REG_ADDR               0x4
#define MEM_INIT_VAL                    0xFF
#define HOST_INPUT_FROM_POE_REG_ADDR    0x10
#define HOST_OUTPUT_TO_POE_REG_ADDR     0x30
#define HOST_RST_CAUSE_REG_ADDR         0x34


/*******************************************************************************
* extDrvDragoniteSharedMemWrite
*
* DESCRIPTION:
*       Write a given buffer to the given address in shared memory of Dragonite 
*       microcontroller.
*
* INPUTS:
*       offset     - Offset from beginning of shared memory
*       buffer      - The buffer to be written.
*       length      - Length of buffer in bytes.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvDragoniteSharedMemWrite
(
    IN  GT_U32  offset,
    IN  GT_VOID *buffer,
    IN  GT_U32  length
);

/*******************************************************************************
* extDrvDragoniteSharedMemRead
*
* DESCRIPTION:
*       Read a data from the given address in shared memory of Dragonite microcontroller
*
* INPUTS:
*       offset     - Offset from beginning of shared memory
*       length     - Length of the memory block to read (in 
*                          bytes).
* OUTPUTS:
*       buffer  - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvDragoniteSharedMemRead
(
    IN  GT_U32  offset,
    IN  GT_U32  length,
    OUT GT_VOID *buffer
);

/*******************************************************************************
* extDrvDragoniteShMemBaseAddrGet
*
* DESCRIPTION:
*       Get start address of communication structure in DTCM
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dtcmPtr      - Pointer to beginning of DTCM, where
*                      communication structures must be placed
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvDragoniteShMemBaseAddrGet
(
    OUT  GT_U32  *dtcmPtr
);


/*******************************************************************************
* extDrvDragoniteFwCrcCheck
*
* DESCRIPTION:
*       This routine executes Dragonite firmware checksum test
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Mostly for debug purposes, when FW download executed by CPSS CRC check 
*       engaged automatically
*
*******************************************************************************/
GT_STATUS extDrvDragoniteFwCrcCheck
(
    GT_VOID
);

/*******************************************************************************
* extDrvDragoniteDriverInit
*
* DESCRIPTION:
*       Dragonite driver init.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvDragoniteDriverInit
(
    GT_VOID
);

/*******************************************************************************
* extDrvDragoniteUnresetSet
*
* DESCRIPTION:
*       Reset/Unreset Dragonite.
*
* INPUTS:
*       unresetEnable - GT_TRUE - set to un-reset state
*                       GT_FALSE - set to reset state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvDragoniteUnresetSet
(
    GT_BOOL unresetEnable
);


/*******************************************************************************
* extDrvDragoniteUnresetSet
*
* DESCRIPTION:
*       Reset/Unreset Dragonite.
*
* INPUTS:
*       memType - 0 - ITCM
*                 1 - DTCM
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvDragoniteMemoryTypeSet
(
    GT_U32 memType
);


/*******************************************************************************
* extDrvDragoniteFwDownload
*
* DESCRIPTION:
*       Download FW to instruction shared memory
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful
*       GT_BAD_PTR on NULL pointer, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS extDrvDragoniteFwDownload
(
    IN GT_VOID  *buf,
    GT_U32      size
);

/*******************************************************************************
* extDrvDragoniteRegisterRead
*
* DESCRIPTION:
*       Dragonite register read.
*
* INPUTS:
*       addr - register address to read from.
*
* OUTPUTS:
*       valuePtr - (pointer to) returned value
*
* RETURNS:
*       GT_OK if successful, or
*       GT_BAD_PTR - on NULL pointer
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvDragoniteRegisterRead
(
    IN  GT_U32 addr,
    OUT GT_U32 *valuePtr
);

/*******************************************************************************
* extDrvDragoniteRegisterWrite
*
* DESCRIPTION:
*       Dragonite register write.
*
* INPUTS:
*       addr - register address to write.
*       value - register value to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_BAD_PTR - on NULL pointer
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvDragoniteRegisterWrite
(
    IN  GT_U32 addr,
    IN  GT_U32 regValue
);

/*******************************************************************************
* extDrvDragoniteMsgWrite
*
* DESCRIPTION:
*       Dragonite message write.
*
* INPUTS:
*       msg - buffer to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_BAD_PTR - on NULL pointer
*       GT_NOT_READY - ownership problem
*       GT_NOT_INITIALIZED - driver not initialized
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvDragoniteMsgWrite
(
    IN GT_U8 msg[DRAGONITE_DATA_MSG_LEN]
);

/*******************************************************************************
* extDrvDragoniteMsgWrite
*
* DESCRIPTION:
*       Dragonite message write.
*
* INPUTS:
*       msg - buffer to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_BAD_PTR - on NULL pointer
*       GT_NOT_READY - ownership problem
*       GT_NOT_INITIALIZED - driver not initialized
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvDragoniteMsgRead
(
    OUT GT_U8 msg[DRAGONITE_DATA_MSG_LEN]
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtDragoniteDrvh */


