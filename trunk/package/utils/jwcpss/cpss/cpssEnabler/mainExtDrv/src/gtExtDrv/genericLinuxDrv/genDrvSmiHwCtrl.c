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
* genDrvSmiStub.c
*
* DESCRIPTION:
*       Stub implementation for SMI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 6 $
*******************************************************************************/

/*Includes*/

#include <gtExtDrv/drivers/gtSmiDrvCtrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include "kerneldrv/include/presteraSmiGlob.h"

/* file descriptor returnd by openning the PP device driver */
extern GT_32 gtPpFd;

#define STUB_FAIL printf("stub function %s returning MV_NOT_SUPPORTED\n", \
                         __FUNCTION__) ; return GT_FAIL

#ifdef SMI_DBG
static GT_U32 smiDbgPrint = 0;
extern GT_U32 smiDbgPrintEn(IN GT_U32  enable)
{
    GT_U32  oldState = smiDbgPrint;
    smiDbgPrint = enable;

    return oldState;
}
#endif

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
*       GT_OK      - on success
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
  SMI_REG smiReg;

  smiReg.slvId   = devSlvId;
  smiReg.regAddr = regAddr;
  smiReg.value   = value;

#ifdef SMI_DBG  
  if(smiDbgPrint)
  {
      printf("hwIfSmiWriteReg:slvId=0x%x,regAddr=0x%x,value=0x%x\n", 
             smiReg.slvId, smiReg.regAddr, smiReg.value);
  }
#endif

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_WRITEREG, &smiReg))
  {
    fprintf(stderr, "Fail to write reg 0x%lx errno(%s)\n",
            smiReg.regAddr, strerror(errno));
    return GT_FAIL;
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
  SMI_REG smiReg;
  
  smiReg.slvId   = devSlvId;
  smiReg.regAddr = regAddr;
  
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_READREG, &smiReg))
  {
    fprintf(stderr, "Fail to read reg 0x%lx errno(%s)\n",
            smiReg.regAddr, strerror(errno));
    return GT_FAIL;
  }
  
  *dataPtr = smiReg.value;
  
#ifdef SMI_DBG
    if(smiDbgPrint)
    {
        printf("hwIfSmiReadReg:slvId=0x%x,regAddr=0x%x,value=0x%x\n", 
               smiReg.slvId, smiReg.regAddr, *dataPtr);
    }
#endif

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
  retVal = hwIfSmiWriteReg(devSlvId, regAddr, value);
  
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

  retVal = hwIfSmiReadReg(devSlvId, regAddr, dataPtr);  
  return retVal;
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
    return retVal;
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
    STUB_FAIL;
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
    STUB_FAIL;
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
    SMI_REG_RAM_STC smiRegRam;

    smiRegRam.devSlvId = devSlvId;
    smiRegRam.addr     = addr;
    smiRegRam.dataArr  = dataArr;
    smiRegRam.arrLen   = arrLen;

    if (ioctl (gtPpFd, PRESTERA_SMI_IOC_READREGRAM, &smiRegRam))
    {
        fprintf(stderr, "Fail to read reg Ram 0x%lx errno(%s)\n",
                        smiRegRam.addr, strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
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
*       arrLen   - The size of addrArr/dataArr.
*       dataArr  - An array containing the data to be written.
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
    SMI_REG_RAM_STC smiRegRam;

    smiRegRam.devSlvId = devSlvId;
    smiRegRam.addr     = addr;
    smiRegRam.dataArr  = dataArr;
    smiRegRam.arrLen   = arrLen;

    if (ioctl (gtPpFd, PRESTERA_SMI_IOC_WRITEREGRAM, &smiRegRam))
    {
        fprintf(stderr, "Fail to write reg Ram 0x%lx errno(%s)\n",
                        smiRegRam.addr, strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;

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
    GT_U32      i;  /* iterator */
    GT_STATUS   rc = GT_OK; /* return code */

    for(i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiTaskReadReg(devSlvId,addrArr[i],&dataArr[i]);
        if(rc != GT_OK)
            break;
    }

    return rc;
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
    GT_U32      i;  /* iterator */
    GT_STATUS   rc = GT_OK; /* return code */

    for(i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiTaskWriteReg(devSlvId,addrArr[i],dataArr[i]);
        if(rc != GT_OK)
            break;
    }

    return rc;
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

#ifdef XCAT_DRV
  if (instance != 0x10) /* we only deal with the first device */
    return GT_FAIL;
  instance = 0;
#endif

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

