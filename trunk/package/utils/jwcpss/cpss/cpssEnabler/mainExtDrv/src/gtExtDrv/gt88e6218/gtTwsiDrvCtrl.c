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
* gtTwsiDrvCtrl.c
*
* DESCRIPTION:
*       API implementation for TWSI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 6 $
*******************************************************************************/

/*Includes*/

#include <gtExtDrv/drivers/gtTwsiDrvCtrl.h>

#include <private/88e6218/gt88e6218Reg.h> 
#include <private/88e6218/gtCore.h>

/* typedefs */
typedef enum _i2cBusStatus 
{
    I2C_OK,
    I2C_BUSY,
    I2C_TIMEOUT,
    I2C_ERROR =0xff

} I2C_BUS_STATUS;

typedef enum _i2cfreq      
{
    
    _100KHZ = 100000,
    _400KHZ = 400000

} I2C_FREQ;

typedef enum _i2cTransType
{
    I2C_WRITE,
    I2C_READ

} I2C_TRANS_TYPE;

/* Describes i2c transaction */
typedef struct _i2cTrans
{
    GT_U8           devId;       /* I2c slave ID                              */ 
    I2C_TRANS_TYPE  transType;   /* Read / Write transaction                  */
    GT_U8           *pData;      /* Pointer to array of chars (address / data)*/
    GT_U8           len;         /* pData array size (in chars).              */
    GT_BOOL         stop;        /* Indicates if stop bit is needed in the end 
                                    of the transaction                        */ 
} I2C_TRANS;            

/* Function declerations */
static I2C_BUS_STATUS      mvI2cInit(IN I2C_FREQ freq , IN GT_U32 inClk);
static I2C_BUS_STATUS      mvI2cMasterTrans(IN I2C_TRANS* i2cTrans);
static I2C_BUS_STATUS      mvI2cWaitNotBusy();

/* defines */
#define I2C_TIMEOUT_VALUE   0x2000000 /* 1 Second at 500 Mhz */
#define TWSI_SLAVE_FREQ     _100KHZ
#define FFX_CORE_FREQ       133.33e6
#define MAX_TIMEOUT         2


#define PRESCALAR_MULTIPLE_VALUE               5
#define LBU_CLK_DIV_ADD_VALUE                  1


/* The macro I2C_BUS_BUSY returns a non-zero number if the i2c bus is busy 
   or zero if the i2c bus isn't busy */  
#define I2C_BUS_BUSY                                                           \
        (MVREGREAD_CHAR(I2C_BASE_REG_ADDR | I2C_STATUS) & (I2C_SR_BUSY))

/* The macro IS_RX_ACK returns a non-zero number if non-ACK bit is recieved 
   from slave or zero if recieving ACK bit from memory slave*/  
#define IS_RX_ACK                                                              \
        (MVREGREAD_CHAR(I2C_BASE_REG_ADDR | I2C_STATUS) & (RX_SR_ACK))


/*******************************************************************************
* extDrvDirectTwsiInitDriver
*
* DESCRIPTION:
*       Init the TWSI interface 
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
GT_STATUS extDrvDirectTwsiInitDriver
(
    GT_VOID
)
{
    I2C_BUS_STATUS  i2cStatus;  
    i2cStatus = mvI2cInit(TWSI_SLAVE_FREQ, FFX_CORE_FREQ);
    if (i2cStatus != I2C_OK) return GT_FAIL;

    return GT_OK;
}


/*******************************************************************************
* extDrvDirectTwsiWaitNotBusy
*
* DESCRIPTION:
*       Wait for TWSI interface not BUSY
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
GT_STATUS extDrvDirectTwsiWaitNotBusy
(
    GT_VOID
)
{
    I2C_BUS_STATUS  i2cStatus;  
    i2cStatus = mvI2cWaitNotBusy();
    if (i2cStatus != I2C_OK) return GT_FAIL;

    return GT_OK;
}



/*******************************************************************************
* extDrvDirectTwsiMasterReadTrans
*
* DESCRIPTION:
*       do TWSI interface Transaction 
*
* INPUTS:
*    devId - I2c slave ID                               
*    pData - Pointer to array of chars (address / data)
*    len   - pData array size (in chars).              
*    stop  - Indicates if stop bit is needed.
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
GT_STATUS extDrvDirectTwsiMasterReadTrans
(
    IN GT_U8           devId,       /* I2c slave ID                              */ 
    IN GT_U8           *pData,      /* Pointer to array of chars (address / data)*/
    IN GT_U8           len,         /* pData array size (in chars).              */
    IN GT_BOOL         stop         /* Indicates if stop bit is needed in the end  */
)
{
    I2C_TRANS       i2cTrans;
    I2C_BUS_STATUS  i2cStatus;  

    i2cTrans.devId = devId;
    i2cTrans.len   = len;
    i2cTrans.pData = pData;
    i2cTrans.stop  = stop;
    i2cTrans.transType =  I2C_READ;
    i2cStatus = mvI2cMasterTrans(&i2cTrans);

    if (i2cStatus != I2C_OK) return GT_FAIL;

    return GT_OK;
}



/*******************************************************************************
* extDrvDirectTwsiMasterWriteTrans
*
* DESCRIPTION:
*       do TWSI interface Transaction 
*
* INPUTS:
*    devId - I2c slave ID                               
*    pData - Pointer to array of chars (address / data)
*    len   - pData array size (in chars).              
*    stop  - Indicates if stop bit is needed.
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
GT_STATUS extDrvDirectTwsiMasterWriteTrans
(
    IN GT_U8           devId,       /* I2c slave ID                              */ 
    IN GT_U8           *pData,      /* Pointer to array of chars (address / data)*/
    IN GT_U8           len,         /* pData array size (in chars).              */
    IN GT_BOOL         stop         /* Indicates if stop bit is needed in the end  */
)
{
    I2C_TRANS       i2cTrans;
    I2C_BUS_STATUS  i2cStatus;  

    i2cTrans.devId = devId;
    i2cTrans.len   = len;
    i2cTrans.pData = pData;
    i2cTrans.stop  = stop;
    i2cTrans.transType =  I2C_WRITE;
    i2cStatus = mvI2cMasterTrans(&i2cTrans);

    if (i2cStatus != I2C_OK) return GT_FAIL;

    return GT_OK;
}


/*******************************************************************************
* mvI2cInit - Initializes the I2C bus mechanism.
*
* DESCRIPTION:
*       This function initialize the I2C mechanism and must be called before 
*       any attemp to use the I2C bus.
*       The function calculates the parameters needed for the I2C's clock 
*       prescalar register, configs the i2c global control register to work in 
*       i2c mode with mem_sel, and starts the global I2cBaseAddr parameter with
*       the base register address value of the i2c unit.
*
* INPUT:
*       freq - The desired frequency. See I2C_FREQ typdef enumerator in mvI2c.h. 
*       inClk - The system's core clock.
*       i2cRegBase - The base registers address of i2c unit.
*
* OUTPUT:
*       Load clock prescalar value into PRER register.
*       Load the global control register.
*       Load i2c register base address into global variable i2cBaseAddr.
*       
*       
* RETURN:
*       I2C_ERROR - For insert uncorrect inputs.
*       I2C_BUSY  - If the i2c bus is busy.
*       I2C_OK    - For success in the initialization phase. 
* 
* NOTE: 
*       This function have to be called before starting any transaction on the
*       i2c bus.
*******************************************************************************/
static I2C_BUS_STATUS mvI2cInit(I2C_FREQ freq , GT_U32 inClk)
{
    unsigned int   i2cTimeOut=0;
    unsigned short clkPrescalar = 0;
    unsigned int pClkDiv;
    
    /* Checking for correct inputs */
    if ( (freq == 0) || (inClk == 0) ) 
    {
        return I2C_ERROR;
    }
    
    /*Set <CLKDiv> to SYSCLK/2*/
    MV_SET_REG_BITS(LBU_BASE_REG_ADDR+LBU_CLK_DIV,BIT0);

    MV_RESET_REG_BITS(GPIO_BASE_REG_ADDR + GPIO_SELECT_8_15 , BIT15 | BIT14 |
                      BIT13 | BIT12);
    
    /* Calculating clock prescalar register value and write it */
    /* I2C_REG_READ(LBU_CLK_DIV , &pClkDiv);//From original Driver!!*/
    
    /*Changed By Amit*/
    MV_REG_READ(LBU_BASE_REG_ADDR | LBU_CLK_DIV, &pClkDiv);
    
    clkPrescalar = (inClk) / ( (pClkDiv + LBU_CLK_DIV_ADD_VALUE) * 
                               (PRESCALAR_MULTIPLE_VALUE) * (freq) );

    MV_REG_WRITE_SHORT(I2C_BASE_REG_ADDR | I2C_PRER , clkPrescalar);
    /* Loading i2c global control register */
    MV_REG_WRITE_CHAR(I2C_BASE_REG_ADDR | I2C_GLOBAL_CTRL ,
                       (I2C_GCR_MODE_CTRL_I2C | I2C_GCR_MEM_SEL ));

    /*Checking if the i2c bus is busy */
    while(I2C_BUS_BUSY)
    {
        /* Checking i2c time out */
        if(i2cTimeOut++ == I2C_TIMEOUT_VALUE)
            return I2C_TIMEOUT;
    }      

    return I2C_OK; 
}


/*******************************************************************************
* mvI2cWaitNotBusy - Wait for i2c bus free
*
* DESCRIPTION:
*       This function wait for i2c bus free
*
* INPUT:
*        None,
*
* OUTPUT:
*        None,
*       
* RETURN:
*       I2C_ERROR - For insert uncorrect inputs.
*       I2C_BUSY  - If the i2c bus is busy.
*       I2C_OK    - For success in the initialization phase. 
* 
* NOTE: 
*       This function have to be called before starting any transaction on the
*       i2c bus.
*******************************************************************************/
static I2C_BUS_STATUS mvI2cWaitNotBusy()
{
    unsigned int i2cTimeOut=0;

    /*Checking if the i2c bus is busy */
    while(I2C_BUS_BUSY)
    {
        /* Checking i2c time out */
        if(i2cTimeOut++ == I2C_TIMEOUT_VALUE)
            return I2C_TIMEOUT;
    }      

    return I2C_OK; 
}

/*******************************************************************************
* mvI2cMasterTrans- Generates transaction on i2c bus.
*
* DESCRIPTION:
*       This function performs transaction on i2c bus according to the I2C_TRANS
*       typdef enumerator fields, which have to be filled by user. 
*       The function generates start bit on bus, transmits the slave ID with 
*       relevant operation (read/write), writes or reads data, and generates 
*       stop bit if nessecery. 
*
* INPUT:
*       *pI2cTrans - Pointer to I2C_TRANS typdef enumerator - See in mvI2c.h.     
*
* OUTPUT:
*       Start/Stop bit is set.
*       Data is recieved/transmited from/on i2c bus.
*
* RETURN:
*       I2C_ERROR   - For insert uncorrect inputs.
*       I2C_BUSY    - If the i2c bus is busy.
*       I2C_OK      - For success in the asking transaction.
*       I2C_TIMEOUT - If timeout was over when waiting for Rx ack.
*              
*******************************************************************************/
static I2C_BUS_STATUS mvI2cMasterTrans(I2C_TRANS *pI2cTrans)
{

    unsigned int    timeout,offset;
    unsigned char   devIdAndOperation;

    if ((pI2cTrans->transType == I2C_WRITE) && (I2C_BUS_BUSY)) 
    {
        return I2C_BUSY;
    }
     
    /* Setting start bit */
    MV_SET_REG_BITS(I2C_BASE_REG_ADDR | I2C_CTRL , I2C_CR_START);
    
    /* Preparing slave ID + operation (read/write) */
    devIdAndOperation = ((pI2cTrans->devId<<1) | (pI2cTrans->transType));
    
    /* Writing Slave ID + operation (read/write) to data register */ 
    MV_REG_WRITE_CHAR(I2C_BASE_REG_ADDR | I2C_DATA , devIdAndOperation);
    
    /* Waiting for RX acknowledge */
    timeout = 0;
    while (IS_RX_ACK)
    {
        /* Checking i2c time out */
        if(timeout++ == I2C_TIMEOUT_VALUE)
            return I2C_TIMEOUT;
        
        return I2C_OK;
    }
    
    /* Writing / Reading data */
    for (offset = 0 ; offset < (pI2cTrans->len) ; offset++) 
    {
        switch (pI2cTrans->transType) 
        {
            case I2C_WRITE: /* Write case */
            
                /* The last phase of writing data transaction includes setting 
                   stop bit before writing the final data */
                if ((pI2cTrans->stop == GT_TRUE) && (offset == pI2cTrans->len - 1))
                {   
                    /* Setting stop bit */
                    MV_SET_REG_BITS(I2C_BASE_REG_ADDR | I2C_CTRL , I2C_CR_STOP); 

                }                   
                MV_REG_WRITE_CHAR(I2C_BASE_REG_ADDR | I2C_DATA , (*(pI2cTrans->pData + offset)));

                /* Waiting for RX acknowledge */
                timeout = 0;
                while(IS_RX_ACK)
                {
                    /* Checking i2c time out */
                    if(timeout++ == I2C_TIMEOUT_VALUE)
                        return I2C_TIMEOUT;
                }      
                
                break;

            case I2C_READ:
                
                /* The last phase of reading data transaction includes setting 
                   stop bit before reading the final data */
                if (offset == pI2cTrans->len - 1)
                {   
                    /* Setting stop bit */
                    MV_SET_REG_BITS(I2C_BASE_REG_ADDR | I2C_CTRL , I2C_CR_STOP);
                    
                }   
                /* Reading data from slave into pData array */
                MV_REG_READ_CHAR(I2C_BASE_REG_ADDR | I2C_DATA , (pI2cTrans->pData + offset));
                
                /* Waiting for RX acknowledge */
                timeout = 0;
                while(IS_RX_ACK)
                {
                    /* Checking i2c time out */
                    if(timeout++ == I2C_TIMEOUT_VALUE)
                        return I2C_TIMEOUT;

                } 
                break;

        }
    }

    return I2C_OK;
}



