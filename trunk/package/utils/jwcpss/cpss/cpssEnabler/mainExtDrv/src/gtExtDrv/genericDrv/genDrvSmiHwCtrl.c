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
* genDrvSmiHwCtrl.c
*
* DESCRIPTION:
*       API implementation for SMI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 3 $
*******************************************************************************/

/*Includes*/
#include <gtExtDrv/os/extDrvOs.h>
#include <gtExtDrv/drivers/pssBspApis.h>
#include <gtExtDrv/drivers/gtHwIfOp.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>

#if (defined CHX_FAMILY || defined SAL_FAMILY)
    #define DX_FAMILY
#endif /*(defined CHX_FAMILY || defined SAL_FAMILY)*/


#ifdef DXSX_FAMILY
  #undef DX_FAMILY
static GT_STATUS hwIfSmiMultichipRead(
    IN  GT_U32 devSlvId,
    IN  GT_U32 regAddr,
    OUT GT_U32 *valuePtr
);

static GT_STATUS hwIfSmiMultichipWrite(
    IN  GT_U32 devSlvId,
    IN  GT_U32 regAddr,
    OUT GT_U32 value
);
#endif


/* Constants */
#ifdef GT_SMI

/* Definition for Multi Address Mode */
#define QD_REG_SMI_COMMAND      0x0
#define QD_REG_SMI_DATA         0x1

/* Bit definition for QD_REG_SMI_COMMAND */
#define QD_SMI_BUSY             0x8000
#define QD_SMI_MODE             0x1000
#define QD_SMI_MODE_BIT         12
#define QD_SMI_OP_BIT           10
#define QD_SMI_OP_SIZE          2
#define QD_SMI_DEV_ADDR_BIT     5
#define QD_SMI_DEV_ADDR_SIZE    5
#define QD_SMI_REG_ADDR_BIT     0
#define QD_SMI_REG_ADDR_SIZE    5

#define QD_SMI_CLAUSE45         0
#define QD_SMI_CLAUSE22         1

#define QD_SMI_WRITE            0x01
#define QD_SMI_READ             0x02

#define QD_SMI_ACCESS_LOOP      1000
#define QD_SMI_TIMEOUT          2


/* speific for salsa/cheetah */
#define  SMI_WRITE_ADDRESS_MSB_REGISTER   (0x00)
#define  SMI_WRITE_ADDRESS_LSB_REGISTER   (0x01)
#define  SMI_WRITE_DATA_MSB_REGISTER      (0x02)
#define  SMI_WRITE_DATA_LSB_REGISTER      (0x03)

#define  SMI_READ_ADDRESS_MSB_REGISTER    (0x04)
#define  SMI_READ_ADDRESS_LSB_REGISTER    (0x05)
#define  SMI_READ_DATA_MSB_REGISTER       (0x06)
#define  SMI_READ_DATA_LSB_REGISTER       (0x07)

#define  SMI_STATUS_REGISTER              (0x1f)

#define SMI_STATUS_WRITE_DONE             (0x02)
#define SMI_STATUS_READ_READY             (0x01)

#define SMI_TIMEOUT_COUNTER               (1000)

#endif /* GT_SMI */

#ifdef GT_SMI
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
#endif /* DEBUG */

/* Prototype for extDrvSmiDevVendorIdGet */
GT_STATUS hwIfSmiTaskReadReg
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
);

#endif /* GT_SMI */

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
#ifdef GT_SMI
#if defined(DX_FAMILY)
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
#else /* !DX_FAMILY */
    return GT_NOT_SUPPORTED;
#endif

#else /* !GT_SMI*/
    return GT_NOT_SUPPORTED;
#endif
}

#ifndef CPU_EMULATED_BUS
#ifdef GT_SMI
/*****************************************************************************
* hwIfSmiIndirectRead
*
* DESCRIPTION:
*       This function reads data from a device in the secondary MII bus.
*
* INPUTS:
*       devSmiAddr  - The SMI Device id for PP.
*       regSmiAddr  - The SMI Device id for register of PP.
*       regAddr     - The SMI register address
*       valuePtr    - The storage where register date to be saved.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_TIMEOUT  - on timeout
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#if (!defined(DXSX_FAMILY)) && (defined(DX_FAMILY))
static GT_STATUS hwIfSmiIndirectRead (
    GT_U32  devSmiAddr,
    GT_U32  regSmiAddr,
    GT_U32  regAddr,
    GT_U32* valuePtr
)
{
#if defined(DX_FAMILY)

    GT_STATUS           rc;
    GT_U32              msb;
    GT_U32              lsb;
#ifdef  SMI_WAIT_FOR_STATUS_DONE
    register GT_U32     timeOut;
    GT_U32              stat;
#endif /* SMI_WAIT_FOR_STATUS_DONE */

    /* write addr to read */
    msb = regAddr >> 16;
    lsb = regAddr & 0xFFFF;
    rc = bspSmiWriteReg(devSmiAddr,0,SMI_READ_ADDRESS_MSB_REGISTER,msb);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = bspSmiWriteReg(devSmiAddr,0,SMI_READ_ADDRESS_LSB_REGISTER,lsb);
    if (rc != GT_OK)
    {
        return rc;
    }

#ifdef  SMI_WAIT_FOR_STATUS_DONE
    /* wait for read done */
    for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
    {
        rc = bspSmiReadReg(devSmiAddr, 0, SMI_STATUS_REGISTER, &stat);
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
    rc = bspSmiReadReg(devSmiAddr,0,SMI_READ_DATA_MSB_REGISTER,&msb);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = bspSmiReadReg(devSmiAddr,0,SMI_READ_DATA_LSB_REGISTER,&lsb);
    if (rc != GT_OK)
    {
        return rc;
    }

    *valuePtr = ((msb & 0xFFFF) << 16) | (lsb & 0xFFFF);

    return GT_OK;
    /* DX_FAMILY*/
#else
    return GT_NOT_SUPPORTED;
#endif /* !DX_FAMILY */
}

/*****************************************************************************
* hwIfSmiIndirectWrite
*
* DESCRIPTION:
*       This function writes data from a device in the secondary MII bus.
*
* INPUTS:
*       devSmiAddr  - The SMI Device id for PP.
*       regSmiAddr  - The SMI Device id for register of PP.
*       regAddr     - The SMI register address
*       value       - The register's value.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_TIMEOUT  - on timeout
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwIfSmiIndirectWrite (
    GT_U32  devSmiAddr,
    GT_U32  regSmiAddr,
    GT_U32  regAddr,
    GT_U32  value
)
{
#if defined(DX_FAMILY)

    GT_STATUS           rc;
    GT_U32              msb;
    GT_U32              lsb;
#ifdef  SMI_WAIT_FOR_STATUS_DONE
    register GT_U32     timeOut;
    GT_U32              stat;

    /* wait for write done */
    for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
    {
        rc = bspSmiReadReg(devSmiAddr, 0, SMI_STATUS_REGISTER, &stat);
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
    rc = bspSmiWriteReg(devSmiAddr,0,SMI_WRITE_ADDRESS_MSB_REGISTER,msb);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = bspSmiWriteReg(devSmiAddr,0,SMI_WRITE_ADDRESS_LSB_REGISTER,lsb);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* write data to write */
    msb = value >> 16;
    lsb = value & 0xFFFF;
    rc = bspSmiWriteReg(devSmiAddr,0,SMI_WRITE_DATA_MSB_REGISTER,msb);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = bspSmiWriteReg(devSmiAddr,0,SMI_WRITE_DATA_LSB_REGISTER,lsb);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
#else /* !DX_FAMILY */
    return GT_NOT_SUPPORTED;
#endif
}
#endif /* GT_SMI */
#endif /* CPU_EMULATED_BUS */

#endif /* DXSX_FAMILY */

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
#ifdef GT_SMI
    GT_STATUS             rc;
    bspSmiAccessMode_ENT  smiAccessMode; /* relevant for SOHO only */

    rc = bspSmiInitDriver(&smiAccessMode);

    return rc;

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
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
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*
*******************************************************************************/
GT_STATUS hwIfSmiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
#ifdef CPU_EMULATED_BUS
    GT_SMI_OPER_STC smiOp;
    GT_HW_IF_UNT    hwIf;
    GT_STATUS retVal;

    hwIf.smiOper = &smiOp;

    smiOp.opCode        = GT_WRITE_SMI_BUFFER_E;
    smiOp.smiDeviceAddr = devSlvId;
    smiOp.regAddress    = regAddr;
    smiOp.smiData       = value;

    retVal = extDrvHwIfOperExec(1, &hwIf, NULL);

    return retVal;
#else
#ifdef GT_SMI
#if defined(DX_FAMILY)

    return hwIfSmiIndirectWrite(devSlvId, 0, regAddr, value);
    /* DX_FAMILY */
#elif defined(DXSX_FAMILY)  /* DX_FAMILY */

    return hwIfSmiMultichipWrite(devSlvId, regAddr, value);

#else /* !DX_FAMILY */
    return GT_NOT_SUPPORTED;
#endif

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
#endif /* CPU_EMULATED_BUS */
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
    OUT GT_U32 *valuePtr
)
{
#ifdef CPU_EMULATED_BUS
    GT_STATUS       retVal;
    GT_SMI_OPER_STC smiOp;
    GT_HW_IF_UNT    hwIf;

    hwIf.smiOper = &smiOp;

    smiOp.opCode        = GT_READ_SMI_BUFFER_E;
    smiOp.smiDeviceAddr = devSlvId;
    smiOp.regAddress    = regAddr;

    retVal = extDrvHwIfOperExec(1, &hwIf, NULL);

    *valuePtr = smiOp.smiData;

    return retVal;
#else
#ifdef GT_SMI
#if defined(DX_FAMILY)

    return hwIfSmiIndirectRead(devSlvId, 0, regAddr, valuePtr);
    /* DX_FAMILY */
#elif defined(DXSX_FAMILY)  /* DX_FAMILY */

    return hwIfSmiMultichipRead(devSlvId, regAddr, valuePtr);

#else /* !DX_FAMILY */
    return GT_NOT_SUPPORTED;
#endif

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
#endif /* CPU_EMULATED_BUS */
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
#ifdef GT_SMI
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
#endif /* DEBUG */
    osSetIntLockUnlock(INTR_MODE_LOCK, (GT_32*)&intKey);
    retVal = hwIfSmiWriteReg(devSlvId, regAddr, value);
    osSetIntLockUnlock(INTR_MODE_UNLOCK, (GT_32*)&intKey);

    return retVal;
#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
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
#ifdef GT_SMI
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
#endif /* DEBUG */
    return retVal;

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
}


#if defined(GT_SMI) || defined(CPU_EMULATED_BUS)
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
#else
#ifdef GT_SMI
#if defined(DX_FAMILY)
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiIndirectWrite(devSlvId, 0, addrArr[i], dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
    /* DX_FAMILY */
#else /* !DX_FAMILY */
#ifdef DXSX_FAMILY
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiMultichipWrite(devSlvId, addrArr[i], dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }
#endif

    return GT_OK;

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
#else
#ifdef GT_SMI
#if defined(DX_FAMILY)
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiIndirectRead(devSlvId, 0, addrArr[i], &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
    /* DX_FAMILY */
#else /* !DX_FAMILY */
#ifdef DXSX_FAMILY
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiMultichipRead(devSlvId, addrArr[i], &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
#endif

    return GT_OK;

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
        for (opIdx = 0; opIdx < MAX_REG_CNT && i < arrLen; i++, opIdx++)
        {
            smiOp[opIdx].opCode        = GT_WRITE_SMI_BUFFER_E;
            smiOp[opIdx].smiDeviceAddr = devSlvId;
            smiOp[opIdx].regAddress    = addr;
            smiOp[opIdx].smiData       = dataArr[i];
            addr += 4;
        }

        retVal = extDrvHwIfOperExec(opIdx, &hwIf, NULL);
        if (GT_OK != retVal)
        {
            return retVal;
        }
    }

    return GT_OK;
#else
#ifdef GT_SMI
#if defined(DX_FAMILY)
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++, addr += 4)
    {
        rc = hwIfSmiIndirectWrite(devSlvId, 0, addr, dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
    /* DX_FAMILY */
#else /* !DX_FAMILY */
#ifdef DXSX_FAMILY
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++, addr += 4)
    {
        rc = hwIfSmiMultichipWrite(devSlvId, addr, dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }
#endif

    return GT_OK;
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
            ;
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
#else
#ifdef GT_SMI
#if defined(DX_FAMILY)
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++, addr += 4)
    {
        rc = hwIfSmiIndirectRead(devSlvId, 0, addr, &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
    /* DX_FAMILY */
#else /* !DX_FAMILY */
#ifdef DXSX_FAMILY
    GT_STATUS   rc;
    GT_U32      i;

    for (i = 0; i < arrLen; i++, addr += 4)
    {
        rc = hwIfSmiMultichipRead(devSlvId, addr, &dataArr[i]);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
#endif

    return GT_OK;

#endif

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
#endif /* CPU_EMULATED_BUS */
}
#endif /* defined(GT_SMI) || defined(CPU_EMULATED_BUS) */


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
#ifdef GT_SMI
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
#endif /* debug */

    retVal = hwIfSmiWriteReg(devSlvId, regAddr, value);

    return retVal;

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
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
#ifdef GT_SMI
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
#endif /* DEBUG */

    return retVal;

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
}

/*******************************************************************************
* extDrvDirectSmiRead
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
GT_STATUS extDrvDirectSmiReadReg
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *value
)
{
#ifdef GT_SMI
    return bspSmiReadReg(devSlvId,0,regAddr,value);

#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
}

/*******************************************************************************
* extDrvDirectSmiWriteReg
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*       dataPtr    - Data read from register.
*
* OUTPUTS:
*        None,
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvDirectSmiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
#ifdef GT_SMI
    return bspSmiWriteReg(devSlvId,0,regAddr,value);
#else /* !GT_SMI */
    return GT_NOT_SUPPORTED;
#endif /* GT_SMI */
}

#ifdef GT_SMI
#ifdef DEBUG
#include <stdio.h>

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

GT_STATUS smidwr
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS rc;
    rc = bspSmiWriteReg(devSlvId,0,regAddr,value);
    printf("write Direct SMI (%08lX) address -> %08lX data -> %08lX \n",
           devSlvId,regAddr,value);

    return rc;
}


GT_STATUS smidrd
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr
)
{
    GT_U32 value = 0;
    GT_STATUS rc;
    rc = bspSmiReadReg(devSlvId,0,regAddr,&value);
    printf("read Direct SMI (%08lX) address -> %08lX data -> %08lX \n",
           devSlvId,regAddr,value);

    return rc;
}

#endif /* DEBUG */
#endif /* GT_SMI */

/*******************************************************************************
* hwIfSmiMultichipWrite
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*       Supports SOHO single-chip, multi-cuip and DX protocols.
*
* INPUTS:
*       devSlvId - 16 MSB: SMI protocol type
*                  16 LSB: Slave Device ID
*       regAddr  - Register address to read from.
*       value    - Data write to register.
*
* OUTPUTS:
*        None,
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*******************************************************************************/
#ifdef DXSX_FAMILY
static GT_STATUS hwIfSmiMultichipWrite
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{

    GT_STATUS rc;
    GT_U16 smiMode;        /* SOHO Single / multi-chip  or DX*/
    GT_U32 regSmiDevId;
    GT_U32 regSmiRegAddr;
    volatile GT_U32 timeOut; /* in 100MS units */
    volatile GT_32  i;
    GT_U32 smiReg;
    GT_U32 msb;
    GT_U32 lsb;

    /* get internal info for register */
    regSmiRegAddr = regAddr & 0xffff;
    regSmiDevId   = regAddr >> 16;
    smiMode       = devSlvId >> 16;
    devSlvId &= 0xFFFF;    /* send only device addr to BSP */

    switch(smiMode)
    {
      case HW_IF_SMI_SOHO_DEVICE_DIRECT:     /******** SOHO DIRECT ********/
        rc = bspSmiWriteReg(devSlvId, regSmiDevId, regSmiRegAddr, value);
        break;

      case HW_IF_SMI_SOHO_DEVICE_INDIRECT:   /******* SOHO INDIRECT *******/
        /* first check that it is not busy */
        rc = bspSmiReadReg(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
        if (rc != GT_OK)
          return GT_FAIL;

        timeOut = QD_SMI_ACCESS_LOOP; /* initialize the loop count */

        if(smiReg & QD_SMI_BUSY)
        {
          for(i = 0 ; i < QD_SMI_TIMEOUT ; i++);
          do
          {
            if(timeOut-- < 1 )
              return GT_TIMEOUT;

            rc = bspSmiReadReg(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
            if (rc != GT_OK)
              return GT_FAIL;

          } while (smiReg & QD_SMI_BUSY);
        }

        /* the SMI is not busy, write SMI data register */
        rc = bspSmiWriteReg(devSlvId,0,QD_REG_SMI_DATA,value);
        if (rc != GT_OK)
          return GT_FAIL;

        /*   build data for SMI command register */
        smiReg = QD_SMI_BUSY                          |
                 (QD_SMI_CLAUSE22 << QD_SMI_MODE_BIT) |
                 (QD_SMI_WRITE << QD_SMI_OP_BIT)      |
                 (regSmiDevId << QD_SMI_DEV_ADDR_BIT) |
                 (regSmiRegAddr << QD_SMI_REG_ADDR_BIT);

        /* write SMI Command register */
        rc = bspSmiWriteReg(devSlvId, 0, QD_REG_SMI_COMMAND, smiReg);
        if (rc != GT_OK)
          return GT_FAIL;
        break;

      case HW_IF_SMI_DX_DEVICE:              /******** DX INDIRECT ********/

#ifdef  SMI_WAIT_FOR_STATUS_DONE

        /* wait for write done */
        for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
        {
          rc = bspSmiReadReg(devSlvId, 0, SMI_STATUS_REGISTER, &smiReg);
          if (rc != GT_OK)
            return rc;

          if ((smiReg & SMI_STATUS_WRITE_DONE) != 0)
            break;

          if (0 == timeOut)
            return GT_FAIL;
        }
#endif /* SMI_WAIT_FOR_STATUS_DONE */

        /* write addr to write */
        msb = regAddr >> 16;
        lsb = regAddr & 0xFFFF;
        rc = bspSmiWriteReg(devSlvId, 0, SMI_WRITE_ADDRESS_MSB_REGISTER, msb);
        if (rc != GT_OK)
          return rc;

        rc = bspSmiWriteReg(devSlvId, 0, SMI_WRITE_ADDRESS_LSB_REGISTER, lsb);
        if (rc != GT_OK)
          return rc;

        /* write data to write */
        msb = value >> 16;
        lsb = value & 0xFFFF;
        rc = bspSmiWriteReg(devSlvId, 0, SMI_WRITE_DATA_MSB_REGISTER, msb);
        if (rc != GT_OK)
          return rc;

        rc = bspSmiWriteReg(devSlvId, 0, SMI_WRITE_DATA_LSB_REGISTER, lsb);
        if (rc != GT_OK)
          return rc;
        break;

      default:
        return GT_NOT_SUPPORTED;
        break;
    }
  return rc;
}
#endif /* DXSX_FAMILY */

/*******************************************************************************
* hwIfSmiMultichipRead
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*       Supports SOHO single-chip, multi-cuip and DX protocols.
*
* INPUTS:
*       devSlvId - 16 MSB: SMI protocol type
*                  16 LSB: Slave Device ID
*       regAddr  - Register address to read from.
*
* OUTPUTS:
*        valuePtr  - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*******************************************************************************/
#ifdef DXSX_FAMILY
static GT_STATUS hwIfSmiMultichipRead
(
    IN  GT_U32 devSlvId,
    IN  GT_U32 regAddr,
    OUT GT_U32 *valuePtr
)
{

    GT_STATUS rc;
    GT_U16 smiMode;        /* SOHO Single / multi-chip  or DX*/
    GT_U32 regSmiDevId;
    GT_U32 regSmiRegAddr;
    volatile GT_U32 timeOut; /* in 100MS units */
    volatile GT_32  i;
    GT_U32 smiReg;
    GT_U32 msb;
    GT_U32 lsb;

    /* get internal info for register */
    regSmiRegAddr = regAddr & 0xffff;
    regSmiDevId   = regAddr >> 16;
    smiMode       = devSlvId >> 16;

    switch(smiMode)
    {
      case HW_IF_SMI_SOHO_DEVICE_DIRECT:     /******** SOHO DIRECT ********/
        devSlvId &= 0xFFFF;    /* send only device addr to BSP */
        rc = bspSmiReadReg(devSlvId, regSmiDevId, regSmiRegAddr, valuePtr);
        break;

      case HW_IF_SMI_SOHO_DEVICE_INDIRECT:   /******* SOHO INDIRECT *******/
        /* first check that it is not busy */
        rc = bspSmiReadReg(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
        if (rc != GT_OK)
          return GT_FAIL;

        timeOut = QD_SMI_ACCESS_LOOP; /* initialize the loop count */

        if(smiReg & QD_SMI_BUSY)
        {
          for(i = 0 ; i < QD_SMI_TIMEOUT ; i++);
          do
          {
            if(timeOut-- < 1 )
              return GT_TIMEOUT;

            rc = bspSmiReadReg(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
            if (rc != GT_OK)
              return GT_FAIL;

          } while (smiReg & QD_SMI_BUSY);
        }

    /* the SMI is not busy,
       build data for SMI command register */
        smiReg = QD_SMI_BUSY                          |
                 (QD_SMI_CLAUSE22 << QD_SMI_MODE_BIT) |
                 (QD_SMI_READ << QD_SMI_OP_BIT)       |
                 (regSmiDevId << QD_SMI_DEV_ADDR_BIT) |
                 (regSmiRegAddr << QD_SMI_REG_ADDR_BIT);

        /* write SMI Command register */
        rc = bspSmiWriteReg(devSlvId, 0, QD_REG_SMI_COMMAND, smiReg);
        if (rc != GT_OK)
          return GT_FAIL;

        /* initialize the loop count */
        timeOut = QD_SMI_ACCESS_LOOP;

        /* Read SMI command register to check BUSY bit */
        rc = bspSmiReadReg(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
        if (rc != GT_OK)
          return GT_FAIL;

        timeOut = QD_SMI_ACCESS_LOOP; /* initialize the loop count */

        if(smiReg & QD_SMI_BUSY)
        {
          for(i = 0 ; i < QD_SMI_TIMEOUT ; i++);
          do
          {
            if(timeOut-- < 1 )
              return GT_TIMEOUT;

            rc = bspSmiReadReg(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
            if (rc != GT_OK)
              return GT_FAIL;

          } while (smiReg & QD_SMI_BUSY);
        }

        /* The Data is ready in the SMI Data register */
        rc = bspSmiReadReg(devSlvId, 0, QD_REG_SMI_DATA, valuePtr);
        if (rc != GT_OK)
            return GT_FAIL;

        break;

      case HW_IF_SMI_DX_DEVICE:              /******** DX INDIRECT ********/
         /* write addr to write */
        msb = regAddr >> 16;
        lsb = regAddr & 0xFFFF;
        rc = bspSmiWriteReg(devSlvId, 0, SMI_READ_ADDRESS_MSB_REGISTER, msb);
        if (rc != GT_OK)
          return rc;

        rc = bspSmiWriteReg(devSlvId, 0, SMI_READ_ADDRESS_LSB_REGISTER, lsb);
        if (rc != GT_OK)
          return rc;

 #ifdef  SMI_WAIT_FOR_STATUS_DONE
        /* wait for write done */
        for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
        {
          rc = bspSmiReadReg(devSlvId, 0, SMI_STATUS_REGISTER, &smiReg);
          if (rc != GT_OK)
            return rc;

          if ((smiReg & SMI_STATUS_WRITE_DONE) != 0)
            break;

          if (0 == timeOut)
            return GT_FAIL;
        }
#endif /* SMI_WAIT_FOR_STATUS_DONE */

       /* write data to write */
        rc = bspSmiReadReg(devSlvId, 0, SMI_READ_DATA_MSB_REGISTER, &msb);
        if (rc != GT_OK)
          return rc;

        rc = bspSmiReadReg(devSlvId, 0, SMI_READ_DATA_LSB_REGISTER, &lsb);
        if (rc != GT_OK)
          return rc;

        *valuePtr = ((msb & 0xFFFF) << 16) | (lsb & 0xFFFF);
        break;

      default:
        return GT_NOT_SUPPORTED;
        break;
    }
  return rc;
}
#endif /* DXSX_FAMILY */

