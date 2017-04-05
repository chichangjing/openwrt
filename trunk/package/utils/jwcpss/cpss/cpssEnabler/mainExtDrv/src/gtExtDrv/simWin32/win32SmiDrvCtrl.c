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
* gtSmiDrvCtrl.c
*
* DESCRIPTION:
*       API implementation for SMI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 1.2 $
*******************************************************************************/

/*Includes*/

#include <gtExtDrv/drivers/gtSmiDrvCtrl.h>

#include <private/88e6218/gt88e6218Reg.h> 
#include <private/88e6218/gtCore.h>

#define GT_REG_ETHER_SMI_REG (UNIMAC_BASE_REG_ADDR | UNIMAC_SMI_REG)
#define SMI_TIMEOUT_COUNTER  1000

/* Access to registers (Read/Write) */
#define GT_REG_WRITE(reg, data) *(volatile GT_U32*)(reg) = data
#define GT_REG_READ(reg)        *(volatile GT_U32*)(reg)

extern int osPrintf(const char* format, ...);

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
    register GT_U32 timeOut;
    register GT_U32 smiReg;

    /* not busy */
    smiReg =  (devSlvId << 16) | (SMI_OP_CODE_BIT_READ) | (regAddr << 21);
    
    /* first check that it is not busy */
    for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
    {
        if ((GT_REG_READ(GT_REG_ETHER_SMI_REG) & SMI_STAT_BUSY) == 0)
            break;
        
        if (0 == timeOut)
            return GT_FAIL;
    }
    
    GT_REG_WRITE(GT_REG_ETHER_SMI_REG, smiReg);
    
    for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
    {
        smiReg = GT_REG_READ(GT_REG_ETHER_SMI_REG);
        
        if (smiReg & SMI_STAT_READ_VALID)
            break;
        
        if (0 == timeOut)
            return GT_FAIL;
    }
    
    *value = (GT_U32)(smiReg & 0xffff);    
    
    return GT_OK;
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
    register GT_U32     timeOut;
    GT_U32              smiReg;
    
    smiReg = (devSlvId << 16) | (regAddr << 21) | (value & 0xffff);    

    /* first check that it is not busy */   
    for (timeOut = SMI_TIMEOUT_COUNTER; ; timeOut--)
    {
        if ((GT_REG_READ(GT_REG_ETHER_SMI_REG) & SMI_STAT_BUSY) == 0)
            break;
            
        if (timeOut == 0)
            return GT_FAIL;
    }
    
    /* not busy */
    GT_REG_WRITE(GT_REG_ETHER_SMI_REG, smiReg);   
    
    return GT_OK;
}



#define DEBUG_ONLY
#ifdef  DEBUG_ONLY

#include <stdio.h>
GT_STATUS smidwr
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS rc;
    rc = extDrvDirectSmiWriteReg(devSlvId,regAddr,value);
    /*printf*/ osPrintf("write Direct SMI (%08lX) address -> %08lX data -> %08lX \n",
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
    rc = extDrvDirectSmiReadReg(devSlvId,regAddr,&value);
    /*printf*/ osPrintf("read Direct SMI (%08lX) address -> %08lX data -> %08lX \n",
           devSlvId,regAddr,value);

    return rc;
}

#endif /* DEBUG_ONLY */


