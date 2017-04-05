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
* gtSmiHwCtrl.c
*
* DESCRIPTION:
*       API implementation for SMI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 9 $
*******************************************************************************/

/*Includes*/

#include <gtExtDrv/drivers/gtSmiDrvCtrl.h>
#include <gtExtDrv/drivers/gtPciDrv.h>
#include <gtExtDrv/drivers/gtSmiDrv.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>

#if (defined CHX_FAMILY || defined SAL_FAMILY)
    #define DX_FAMILY
#endif /*(defined CHX_FAMILY || defined SAL_FAMILY)*/

/*#define DEBUG*/
#ifdef DEBUG
extern int taskSuspend(int tid);
extern int logMsg(char*,int,int,int,int,int,int);
extern int intContext(void);
extern int wdbUserEvtPost
(
    char * event              /* event string to send */
);
static int logHwReadWriteFlag = 0;
int logHwReadWriteSet() {logHwReadWriteFlag = 1; return 0;}
int logHwReadWriteUnSet() {logHwReadWriteFlag = 0; return 0;}
static void postIt(GT_U32 pp, GT_U8 *op, GT_U32 add, GT_U32 val)
{
    char buffer[128];
    osSprintf(buffer, "PP %d %s ADD-0x%08X VAL-0x%08X", pp, op, add, val);
    wdbUserEvtPost(buffer);
}
static GT_U32 checkaddr = 0xBAD0ADD0;
#endif

/*
    the Cheetah has erratum :
    FEr #26: CPU SMI Read/Write Status Indications are not functional
    Relevant for: 98DX250/260/270 and 98DX166/166R/246

#define SMI_WAIT_FOR_STATUS_DONE
*/


/*******************************************************************************
* hwIfSmiInitDriver
*
* DESCRIPTION:
*       Init the SMI interface
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiInitDriver
(
    GT_VOID
)
{
    return GT_OK;
}


/*******************************************************************************
* hwIfSmiWriteReg
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    register GT_STATUS  rc;
    GT_U32              msb;
    GT_U32              lsb;
#ifdef  SMI_WAIT_FOR_STATUS_DONE
    register GT_U32     timeOut;
    GT_U32              stat;

    /* wait for write done */
    for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
    {
        rc = extDrvDirectSmiReadReg(devSlvId, SMI_STATUS_REGISTER, &stat);
        if (rc != GT_OK)
        {
            return rc;
        }

        if ((stat & SMI_STATUS_WRITE_DONE) != 0)
        {
            break;
        }

        if (0 == timeOut)
        {
            return GT_FAIL;
        }
    }
#endif /* SMI_WAIT_FOR_STATUS_DONE */

    /* write addr to write */
    msb = regAddr >> 16;
    lsb = regAddr & 0xFFFF;
    rc = extDrvDirectSmiWriteReg(devSlvId,SMI_WRITE_ADDRESS_MSB_REGISTER,msb);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = extDrvDirectSmiWriteReg(devSlvId,SMI_WRITE_ADDRESS_LSB_REGISTER,lsb);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* write data to write */
    msb = value >> 16;
    lsb = value & 0xFFFF;
    rc = extDrvDirectSmiWriteReg(devSlvId,SMI_WRITE_DATA_MSB_REGISTER,msb);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = extDrvDirectSmiWriteReg(devSlvId,SMI_WRITE_DATA_LSB_REGISTER,lsb);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* hwIfSmiReadReg
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiReadReg
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    register GT_STATUS  rc;
    GT_U32              msb;
    GT_U32              lsb;
#ifdef  SMI_WAIT_FOR_STATUS_DONE
    register GT_U32     timeOut;
    GT_U32              stat;
#endif /* SMI_WAIT_FOR_STATUS_DONE */

    /* write addr to read */
    msb = regAddr >> 16;
    lsb = regAddr & 0xFFFF;
    rc = extDrvDirectSmiWriteReg(devSlvId,SMI_READ_ADDRESS_MSB_REGISTER,msb);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = extDrvDirectSmiWriteReg(devSlvId,SMI_READ_ADDRESS_LSB_REGISTER,lsb);
    if (rc != GT_OK)
    {
        return rc;
    }

#ifdef  SMI_WAIT_FOR_STATUS_DONE
    /* wait for read done */
    for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
    {
        rc = extDrvDirectSmiReadReg(devSlvId, SMI_STATUS_REGISTER, &stat);
        if (rc != GT_OK)
        {
            return rc;
        }

        if ((stat & SMI_STATUS_READ_READY) != 0)
        {
            break;
        }

        if (0 == timeOut)
        {
            return GT_FAIL;
        }
    }
#endif /* SMI_WAIT_FOR_STATUS_DONE */

    /* read data */
    rc = extDrvDirectSmiReadReg(devSlvId,SMI_READ_DATA_MSB_REGISTER,&msb);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = extDrvDirectSmiReadReg(devSlvId,SMI_READ_DATA_LSB_REGISTER,&lsb);
    if (rc != GT_OK)
    {
        return rc;
    }

    *dataPtr = ((msb & 0xFFFF) << 16) | (lsb & 0xFFFF);

    return GT_OK;
}

/*******************************************************************************
* hwIfSmiTaskWriteReg
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiTaskWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS retVal;
    GT_U32 intKey;
    intKey = 0;

#ifdef DEBUG
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiTaskWriteReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)value,
               0,0,0,0);
        taskSuspend(0);
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "W", regAddr, value);
    }
#endif
    osSetIntLockUnlock(INTR_MODE_LOCK, (GT_32*)&intKey);
    retVal = hwIfSmiWriteReg(devSlvId, regAddr, value);
    osSetIntLockUnlock(INTR_MODE_UNLOCK, (GT_32*)&intKey);

    return retVal;
}

/*******************************************************************************
* hwIfSmiTaskReadReg
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiTaskReadReg
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    GT_STATUS retVal;
    GT_U32 intKey;
    intKey = 0;

    osSetIntLockUnlock(INTR_MODE_LOCK, (GT_32*)&intKey);
    retVal = hwIfSmiReadReg(devSlvId, regAddr, dataPtr);
    osSetIntLockUnlock(INTR_MODE_UNLOCK, (GT_32*)&intKey);

#ifdef DEBUG
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiTaskReadReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)*dataPtr,
               0,0,0,0);
        taskSuspend(0);
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "R", regAddr, *dataPtr);
    }
#endif
    return retVal;
}


/*******************************************************************************
* hwIfSmiRegVecWrite
*
* DESCRIPTION:
*       Writes SMI register vector from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addrArr  - Array of addresses to write to.
*       dataArr  - An array containing the data to be written.
*       arrLen   - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*
*******************************************************************************/
static GT_STATUS hwIfSmiRegVecWrite
(
    IN GT_U32       devSlvId,
    IN GT_U32       addrArr[],
    IN GT_U32       dataArr[],
    IN GT_U32       arrLen
)
{
#ifdef CPU_EMULATED_BUS
    GT_SMI_OPER_STC smiOp[MAX_REG_CNT];
    GT_HW_IF_UNT    hwIf;
    GT_STATUS       retVal;
    GT_U32          i, opIdx;

    hwIf.smiOper = smiOp;
    i = 0;

    while (i < arrLen)
    {
        for (opIdx = 0; opIdx < MAX_REG_CNT && i < arrLen; i++, opIdx++)
        {
            smiOp[opIdx].opCode        = GT_WRITE_SMI_BUFFER_E;
            smiOp[opIdx].smiDeviceAddr = devSlvId;
            smiOp[opIdx].regAddress    = addrArr[i];
            smiOp[opIdx].smiData       = dataArr[i];
        }

        retVal = extDrvHwIfOperExec(opIdx, &hwIf, NULL);
        if (GT_OK != retVal)
        {
            return retVal;
        }
    }

    return GT_OK;
#else /* !CPU_EMULATED_BUS */
#ifdef GT_SMI
#if defined(DX_FAMILY)
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiWriteReg(devSlvId, addrArr[i], dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
    /* DX_FAMILY */
#else /* !DX_FAMILY */
    return GT_NOT_SUPPORTED;
#endif

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
#endif /* CPU_EMULATED_BUS */
}


/*******************************************************************************
* hwIfSmiRegVecRead
*
* DESCRIPTION:
*       Reads SMI register vector from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addrArr  - Array of addresses to write to.
*       arrLen   - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr  - An array containing the data to be written.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwIfSmiRegVecRead
(
    IN GT_U32       devSlvId,
    IN GT_U32       addrArr[],
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
)
{
#ifdef CPU_EMULATED_BUS
    GT_STATUS       retVal;
    GT_SMI_OPER_STC smiOp[MAX_REG_CNT];
    GT_HW_IF_UNT    hwIf;
    GT_U32          i, j, opIdx;

    hwIf.smiOper = smiOp;
    i = 0;

    while (i < arrLen)
    {
        for (opIdx = 0; opIdx < MAX_REG_CNT && i < arrLen; i++, opIdx++)
        {
            smiOp[opIdx].opCode        = GT_READ_SMI_BUFFER_E;
            smiOp[opIdx].smiDeviceAddr = devSlvId;
            smiOp[opIdx].regAddress    = addrArr[i];
        }

        retVal = extDrvHwIfOperExec(opIdx, &hwIf, NULL);
        if (GT_OK != retVal)
        {
            return retVal;
        }

        for (opIdx--, j = i-1; ; opIdx--, j--)
        {
            dataArr[j] = smiOp[opIdx].smiData;

            if (0 == opIdx)
            {
                break;
            }
        }
    }

    return GT_OK;
#else /* !CPU_EMULATED_BUS */
#ifdef GT_SMI
#if defined(DX_FAMILY)
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiReadReg(devSlvId, addrArr[i], &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
    /* DX_FAMILY */
#else /* !DX_FAMILY */
    return GT_NOT_SUPPORTED;
#endif

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
#endif /* CPU_EMULATED_BUS */
}


/*******************************************************************************
* hwIfSmiRegRamWrite
*
* DESCRIPTION:
*       Writes a memory map (contiguous memory) using SMI from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addr     - Register address to start write the reading.
*       dataArr  - An array containing the data to be written.
*       arrLen   - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK     - on success
*       GT_ERROR  - on hardware error
*
* COMMENTS:
*
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*
*******************************************************************************/
static GT_STATUS hwIfSmiRegRamWrite
(
    IN GT_U32       devSlvId,
    IN GT_U32       addr,
    IN GT_U32       dataArr[],
    IN GT_U32       arrLen
)
{
#ifdef CPU_EMULATED_BUS
    GT_SMI_OPER_STC smiOp[MAX_REG_CNT];
    GT_HW_IF_UNT    hwIf;
    GT_STATUS       retVal;
    GT_U32          i, opIdx;

    hwIf.smiOper = smiOp;
    i = 0;

    while (i < arrLen)
    {
        for (opIdx = 0;
             opIdx < MAX_REG_CNT && i < arrLen;
             i++, opIdx++, addr += 4)
        {
            smiOp[opIdx].opCode        = GT_WRITE_SMI_BUFFER_E;
            smiOp[opIdx].smiDeviceAddr = devSlvId;
            smiOp[opIdx].regAddress    = addr;
            smiOp[opIdx].smiData       = dataArr[i];
        }

        retVal = extDrvHwIfOperExec(opIdx, &hwIf, NULL);
        if (GT_OK != retVal)
        {
            return retVal;
        }
    }

    return GT_OK;
#else /* !CPU_EMULATED_BUS */
#ifdef GT_SMI
#if defined(DX_FAMILY)
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++, addr += 4)
    {
        rc = hwIfSmiWriteReg(devSlvId, addr, dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
    /* DX_FAMILY */
#else /* !DX_FAMILY */
    return GT_NOT_SUPPORTED;
#endif

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
#endif /* CPU_EMULATED_BUS */
}


/*******************************************************************************
* hwIfSmiRegRamRead
*
* DESCRIPTION:
*       Reads a memory map (contiguous memory) using SMI from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addr     - Register address to start write the writing.
*       arrLen   - The length of dataArr (the number of registers to read)
*
* OUTPUTS:
*       dataArr  - An array containing the data to be written.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwIfSmiRegRamRead
(
    IN GT_U32       devSlvId,
    IN GT_U32       addr,
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
)
{
#ifdef CPU_EMULATED_BUS
    GT_STATUS       retVal;
    GT_SMI_OPER_STC smiOp[MAX_REG_CNT];
    GT_HW_IF_UNT    hwIf;
    GT_U32          i, j, opIdx;

    hwIf.smiOper = smiOp;
    i = 0;

    while (i < arrLen)
    {
        for (opIdx = 0;
             opIdx < MAX_REG_CNT && i < arrLen;
             i++, opIdx++, addr += 4)
        {
            smiOp[opIdx].opCode        = GT_READ_SMI_BUFFER_E;
            smiOp[opIdx].smiDeviceAddr = devSlvId;
            smiOp[opIdx].regAddress    = addr;
        }

        retVal = extDrvHwIfOperExec(opIdx, &hwIf, NULL);
        if (GT_OK != retVal)
        {
            return retVal;
        }

        for (opIdx--, j = i-1; ; opIdx--, j--)
        {
            dataArr[j] = smiOp[opIdx].smiData;

            if (0 == opIdx)
            {
                break;
            }
        }
    }

    return GT_OK;
#else /* !CPU_EMULATED_BUS */
#ifdef GT_SMI
#if defined(DX_FAMILY)
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++, addr += 4)
    {
        rc = hwIfSmiReadReg(devSlvId, addr, &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
    /* DX_FAMILY */
#else /* !DX_FAMILY */
    return GT_NOT_SUPPORTED;
#endif

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
#endif /* CPU_EMULATED_BUS */
}


/*******************************************************************************
* hwIfSmiTskRegVecWrite
*
* DESCRIPTION:
*       Writes SMI register vector from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addrArr  - Array of addresses to write to.
*       dataArr  - An array containing the data to be written.
*       arrLen   - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiTskRegVecWrite
(
    IN GT_U32       devSlvId,
    IN GT_U32       addrArr[],
    IN GT_U32       dataArr[],
    IN GT_U32       arrLen
)
{
#ifdef GT_SMI
    GT_STATUS   retVal;
    GT_32       intKey;

    intKey = 0;

#ifdef DEBUG
    {
        GT_U32 i;

        for (i = 0; i < arrLen; i++)
        {
            if (checkaddr == addrArr[i])
            {
                logMsg("hwIfSmiTskRegVecWrite address 0x%08x, 0x%08x\n",
                       (int)addrArr[i], (int)dataArr[i],
                       0,0,0,0);
                taskSuspend(0);
            }

            if (logHwReadWriteFlag)
            {
                postIt(devSlvId, "W", addrArr[i], dataArr[i]);
            }
        }
    }
#endif /* DEBUG */

    osSetIntLockUnlock(INTR_MODE_LOCK, &intKey);
    retVal = hwIfSmiRegVecWrite(devSlvId, addrArr, dataArr, arrLen);
    osSetIntLockUnlock(INTR_MODE_UNLOCK, &intKey);

    return retVal;
#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
}


/*******************************************************************************
* hwIfSmiTskRegVecRead
*
* DESCRIPTION:
*       Reads SMI register vector from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addrArr  - Array of addresses to write to.
*       arrLen   - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr  - An array containing the data to be written.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiTskRegVecRead
(
    IN GT_U32       devSlvId,
    IN GT_U32       addrArr[],
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
)
{
#ifdef GT_SMI
    GT_STATUS   retVal;
    GT_32       intKey;

    intKey = 0;

    osSetIntLockUnlock(INTR_MODE_LOCK, &intKey);
    retVal = hwIfSmiRegVecRead(devSlvId, addrArr, dataArr, arrLen);
    osSetIntLockUnlock(INTR_MODE_UNLOCK, &intKey);

#ifdef DEBUG
    {
        GT_U32 i;

        for (i = 0; i < arrLen; i++)
        {
            if (checkaddr == addrArr[i])
            {
                logMsg("hwIfSmiTskRegVecRead address 0x%08x, 0x%08x\n",
                       (int)addrArr[i], (int)dataArr[i],
                       0,0,0,0);
                taskSuspend(0);
            }

            if (logHwReadWriteFlag)
            {
                postIt(devSlvId, "R", addrArr[i], dataArr[i]);
            }
        }
    }
#endif /* DEBUG */
    return retVal;

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
}


/*******************************************************************************
* hwIfSmiTskRegRamWrite
*
* DESCRIPTION:
*       Writes a memory map (contiguous memory) using SMI from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addr     - Register address to start write the reading.
*       dataArr  - An array containing the data to be written.
*       arrLen   - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*       - register address is incremented in 4 byte per register
*
*******************************************************************************/
GT_STATUS hwIfSmiTskRegRamWrite
(
    IN GT_U32       devSlvId,
    IN GT_U32       addr,
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
)
{
#ifdef GT_SMI
    GT_STATUS   retVal;
    GT_32       intKey;

    intKey = 0;

#ifdef DEBUG
    {
        GT_U32  i;
        GT_U32  tempAddr;

        tempAddr = addr;

        for (i = 0; i < arrLen; i++, tempAddr += 4)
        {
            if (checkaddr == tempAddr)
            {
                logMsg("hwIfSmiTskRegRamWrite address 0x%08x, 0x%08x\n",
                       (int)tempAddr, (int)dataArr[i],
                       0,0,0,0);
                taskSuspend(0);
            }

            if (logHwReadWriteFlag)
            {
                postIt(devSlvId, "W", tempAddr, dataArr[i]);
            }
        }
    }
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiTaskWriteReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)value,
               0,0,0,0);
        taskSuspend(0);
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "W", regAddr, value);
    }
#endif /* DEBUG */
    osSetIntLockUnlock(INTR_MODE_LOCK, &intKey);
    retVal = hwIfSmiRegRamWrite(devSlvId, addr, dataArr, arrLen);
    osSetIntLockUnlock(INTR_MODE_UNLOCK, &intKey);

    return retVal;
#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
}


/*******************************************************************************
* hwIfSmiTskRegRamRead
*
* DESCRIPTION:
*       Reads a memory map (contiguous memory) using SMI from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addr     - Register address to start write the writing.
*       arrLen   - The length of dataArr (the number of registers to read)
*
* OUTPUTS:
*       dataArr  - An array containing the data to be written.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*       - register address is incremented in 4 byte per register
*
*******************************************************************************/
GT_STATUS hwIfSmiTskRegRamRead
(
    IN GT_U32       devSlvId,
    IN GT_U32       addr,
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
)
{
#ifdef GT_SMI
    GT_STATUS   retVal;
    GT_32       intKey;

    intKey = 0;

    osSetIntLockUnlock(INTR_MODE_LOCK, &intKey);
    retVal = hwIfSmiRegRamRead(devSlvId, addr, dataArr, arrLen);
    osSetIntLockUnlock(INTR_MODE_UNLOCK, &intKey);

#ifdef DEBUG
    {
        GT_U32 i;
        GT_U32  tempAddr;

        tempAddr = addr;

        for (i = 0; i < arrLen; i++, tempAddr += 4)
        {
            if (checkaddr == tempAddr)
            {
                logMsg("hwIfSmiTskRegRamRead address 0x%08x, 0x%08x\n",
                       (int)tempAddr, (int)dataArr[i],
                       0,0,0,0);
                taskSuspend(0);
            }

            if (logHwReadWriteFlag)
            {
                postIt(devSlvId, "R", tempAddr, dataArr[i]);
            }
        }
    }
#endif /* DEBUG */
    return retVal;

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
}


/*******************************************************************************
* hwIfSmiInterruptWriteReg
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiInterruptWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS retVal;

#ifdef DEBUG
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiInterruptWriteReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)value,
               0,0,0,0);
        if (! intContext())
        {
            taskSuspend(0);
        }
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "W", regAddr, value);
    }
#endif

    retVal = hwIfSmiWriteReg(devSlvId, regAddr, value);

    return retVal;
}

/*******************************************************************************
* hwIfSmiInterruptReadReg
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiInterruptReadReg
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    GT_STATUS retVal;

    retVal = hwIfSmiReadReg(devSlvId, regAddr, dataPtr);

#ifdef DEBUG
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiInterruptReadReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)*dataPtr,
               0,0,0,0);
        if (!intContext())
        {
            taskSuspend(0);
        }
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "W", regAddr, *dataPtr);
    }
#endif
    return retVal;
}

/*******************************************************************************
* extDrvSmiDevVendorIdGet
*
* DESCRIPTION:
*       This routine returns vendor Id of the given device.
*
* INPUTS:
*       instance - The requested device instance.
*
* OUTPUTS:
*       vendorId <- The device vendor Id.
*       devId    <- The device Id.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvSmiDevVendorIdGet
(
    OUT GT_U16  *vendorId,
    OUT GT_U16  *devId,
    IN  GT_U32  instance
)
{
    GT_STATUS   retVal;
    GT_U32      vendor;
    GT_U32      device;

    /* Prestera is little endian */
    retVal = hwIfSmiTaskReadReg(instance, 0x00000050, &vendor);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    retVal = hwIfSmiTaskReadReg(instance, 0x0000004C, &device);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    *vendorId   = vendor & 0xffff;
    *devId      = (device>>4) & 0xffff;

    return GT_OK;
}

#ifdef DEBUG
int traceAddress(GT_U32 add)
{
    checkaddr = add;
    return 0;
}

int untraceAddress(GT_U32 add)
{
    checkaddr = 0xBAD0ADD0;
    return 0;
}
#endif


