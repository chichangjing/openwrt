/*******************************************************************
 *
 *    DESCRIPTION: mvTwsiDrv.c
 *								lower level driver to TWSI 
 *									(Two Wire Serial Interface) bus
 *    AUTHOR: OferR.R
 *
 *    HISTORY:
 *
 *******************************************************************/

/** include files **/
#include "marvell.h"
#include "mvTwsiDrv.h"
/** local definitions **/
#define TWSI_BUS_BUSY  \
				(TWSI_READ_CHAR(TWSI_BASE_REG_ADDR | TWSI_STATUS_REG) & BIT2)
/* The macro IS_RX_ACK returns a non-zero number if non-ACK bit is recieved 
   from slave or zero if recieving ACK bit from memory slave*/  
#define TWSI_IS_RX_ACK                                                              \
        (TWSI_READ_CHAR(TWSI_BASE_REG_ADDR | TWSI_STATUS_REG) & BIT3)


/** default settings **/

/** internal functions **/

/** public functions **/

/*
 *  FUNCTION: mvTwsiInit
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS: 
 *
 */
TWSI_STATUS mvTwsiInit(TWSI_FREQ freq , unsigned int inClk)
{
	unsigned short clkPrescalar = 0;
/*    unsigned int   i2cTimeOut=0;*/
  unsigned int pClkDiv;
	unsigned int twsiDelay = 0;



	if ((freq == 0)||(inClk ==0))
		return TWSI_ERROR;

  TWSI_READ_REG(TWSI_PERIPHERAL_CLK_DIVIDER_REG, &pClkDiv);

	/*Set <CLKDiv> to SYSCLK/2*/
  TWSI_SET_REG_BITS(TWSI_PERIPHERAL_CLK_DIVIDER_REG, BIT0);

  TWSI_RESET_REG_BITS(GPIO_BASE_ADRS + GPIO_SELECT_15_8 , BIT15 | BIT14 |
                    BIT13 | BIT12);

  /* Calculating clock prescalar register value and write it */
  clkPrescalar = (inClk) / ( (pClkDiv + TWSI_CLK_DIV_ADD_VALUE) * 
                               (TWSI_PRESCALAR_MULTIPLE_VALUE) * (freq));

  TWSI_WRITE_REG (TWSI_CLK_DIVIDER_REG, clkPrescalar);
  
  /* Loading i2c global control register with default value 0x02*/
	TWSI_WRITE_REG (TWSI_GLOBAL_CTRL_REG, 0x02);	
	/*Checking if the i2c bus is busy */
	while (TWSI_BUS_BUSY)
	{
		/* Checking i2c time out */
		if (++twsiDelay == TWSI_TIMEOUT_VALUE)
			return TWSI_TIMEOUT;
	}

	return TWSI_OK;
}

/*
 *  FUNCTION: mvTwsiRead
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: Read number of nytes (given by len) from slave device over the 
 *  TWSI bus
 *
 *  RETURNS:TWSI_OK - on successful completion
 *          TWSI_ERROR - if transaction failed
 *          TWSI_TIMEOUT - if the TWSI bus is busy longer then delay permited     
 *
 */
TWSI_STATUS mvTwsiRead(unsigned char deviceId, unsigned int len, unsigned char *data)
{
 unsigned char deviceIdAndCommand;
 unsigned int offset;
 unsigned int twsiDelay = 0;

	
	/* read TWSI status register cheack the TWSIBusy bit is IDLE */
	while (TWSI_BUS_BUSY)
	{
		/* Checking TWSI time out */
		if (twsiDelay == TWSI_TIMEOUT_VALUE)
			return TWSI_TIMEOUT;
	}
	/* write the TWSI control register to set the start bit */
	TWSI_WRITE_REG(TWSI_CONTROL_REG,TWSI_CR_START);

	/* write the 7-bit TWSI slave-id and R/W  bit to the TWSI data register */
	deviceIdAndCommand = ((deviceId << 1)|(TWSI_READ));
	TWSI_WRITE_REG(TWSI_DATA_REG, deviceIdAndCommand);

	/* write the 8-bit address to the TWSI data register */
	for (offset = 0; offset < len ; offset++)
	{
	/* write the 8-bit data to the TWSI data register */
		TWSI_READ_REG(TWSI_DATA_REG , (data + offset));
		/* continue writing 8-bit data register if using page write mode */
		if (offset == len-1)
		{
			/* before writing the last byte write the TWSI control register with the stop bit */
			TWSI_WRITE_REG(TWSI_CONTROL_REG,TWSI_CR_STOP);
		}
		TWSI_READ_REG(TWSI_DATA_REG ,( data+offset));
	
		/* wait for ack */
		while (TWSI_IS_RX_ACK)
		{
			/* Checking i2c time out */
			if (++twsiDelay == TWSI_TIMEOUT_VALUE)
				return TWSI_TIMEOUT;
		}
	}

	return TWSI_OK;
}

/*
 *  FUNCTION: mvTwsiWrite
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS: 
 *
 */
TWSI_STATUS mvTwsiWrite (unsigned char deviceId, unsigned int len , unsigned char *data)
{
	unsigned char deviceIdAndCommand;
	unsigned int twsiDelay = 0;
	unsigned int offset;
	/* read TWSI status register cheack the TWSIBusy bit is IDLE */
	while (TWSI_BUS_BUSY)
	{
		/* Checking TWSI time out */
		if (++twsiDelay == TWSI_TIMEOUT_VALUE)
			return TWSI_TIMEOUT;
	}
	/* write the TWSI control register to set the start bit */
	TWSI_WRITE_REG(TWSI_CONTROL_REG,TWSI_CR_START);

	/* write the 7-bit TWSI slave-id and R/W  bit to the TWSI data register */
	deviceIdAndCommand = ((deviceId << 1)|(TWSI_WRITE));
	TWSI_WRITE_REG(TWSI_DATA_REG, deviceIdAndCommand);

	/* write the 8-bit address to the TWSI data register */
	for (offset = 0; offset < len ; offset++)
	{
	/* write the 8-bit data to the TWSI data register */
		TWSI_WRITE_REG(TWSI_DATA_REG ,*(data+offset));
		/* continue writing 8-bit data register if using page write mode */
		if (offset == len-1)
		{
			/* before writing the last byte write the TWSI control register with the stop bit */
			TWSI_WRITE_REG(TWSI_CONTROL_REG,TWSI_CR_STOP);
		}
		TWSI_WRITE_REG(TWSI_DATA_REG ,(*data+offset));
	
		/* wait for ack */
		while (TWSI_IS_RX_ACK)
		{
			/* Checking i2c time out */
			if (++twsiDelay == TWSI_TIMEOUT_VALUE)
				return TWSI_TIMEOUT;
		}
	}

	return TWSI_OK;
}

/*
 *  FUNCTION: mvTwsiReset
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS: 
 *
 */
void mvTwsiReset (void)
{
	
}
/** private functions **/
