/*******************************************************************
 *
 *    DESCRIPTION:mvTwsiDrv.h
 *
 *    AUTHOR: OferR.R
 *
 *    HISTORY:
 *
 *******************************************************************/

/** public data **/
#define TWSI_BASE_REG_ADDR         0x8000C000

/* I2C/TWSI Register offsets */

#define TWSI_PRESCALAR_REG         0x0100    /* Clock prescalar register       */
#define TWSI_GLOBAL_CTRL_REG       0x0104    /* Global control register        */
#define TWSI_CONTROL_REG           0x0110    /* I2c control register           */
#define TWSI_STATUS_REG            0x011C    /* CSU status register            */
#define TWSI_INTERRUPT_MASK_REG    0x0120    /* Interrupt mask register        */
#define TWSI_DATA_REG              0x0124    /* Virtual data register          */
#define TWSI_INTERRUPT_SRC_REG     0x012C    /* Interrupt source register      */
#define TWSI_DEVICE_RESET_REG			 0x0000    /* peripheral reset register			 */
#define TWSI_CLK_DIVIDER_REG			 0x0004    /* peripheral clock divider register */

/* Global control register */
#define TWSI_GCR_AC_SEL            BIT0
#define TWSI_GCR_MEM_SEL           BIT1
#define TWSI_GCR_INTERRUPT_MASK    BIT5
#define TWSI_GCR_MODE_CTRL_I2C     NO_BIT

#define TWSI_NO_BIT  (0)
#define TWSI_BIT0    (1<<0)
#define TWSI_BIT1		 (1<<1)
#define TWSI_BIT2		 (1<<2)
#define TWSI_BIT3		 (1<<3)
#define TWSI_BIT4		 (1<<4)
#define TWSI_BIT5		 (1<<5)
#define TWSI_BIT6		 (1<<6)
#define TWSI_BIT7		 (1<<7)

#define TWSI_WRITE	 0
#define TWSI_READ		 1
/* I2c control register */
#define TWSI_CR_STOP               TWSI_BIT0
#define TWSI_CR_START              TWSI_BIT1

#define TWSI_PERIPHERAL_RESET_REG	 TWSI_BASE_REG_ADDR
#define TWSI_PERIPHERAL_CLK_DIVIDER_REG  (TWSI_PERIPHERAL_RESET_REG + 0x04)
#define TWSI_PRESET    0x00   /* LBU peripheral reset - WO */

#define TWSI_PRESCALAR_MULTIPLE_VALUE 5
#define TWSI_CLK_DIV_ADD_VALUE        1


#define  mvInternalRegBaseAddr  (0x00000000)
/* gets register offset and bits: a 32bit value. It set to logic '1' in the  
   internal register the bits which given as an input example:
   MV_SET_REG_BITS(0x840,BIT3 | BIT24 | BIT30) - set bits: 3,24 and 30 to logic
   '1' in register 0x840 while the other bits stays as is. */
#define TWSI_SET_REG_BITS(regOffset,bits)                                        \
        ((*((volatile unsigned int*)(mvInternalRegBaseAddr |   \
        (regOffset)))) |= ((unsigned int)(bits)))

/* gets register offset and bits: a 32bit value. It set to logic '0' in the  
   internal register the bits which given as an input example:
   MV_RESET_REG_BITS(0x840,BIT3 | BIT24 | BIT30) - set bits: 3,24 and 30 to 
   logic '0' in register 0x840 while the other bits stays as is. */
#define TWSI_RESET_REG_BITS(regOffset,bits)                                      \
        ((*((volatile unsigned int*)(mvInternalRegBaseAddr |   \
        (regOffset)))) &= ~((unsigned int)(bits)))

#define TWSI_READ_CHAR(reg)	\
            (*(volatile unsigned char *) reg)    

#define TWSI_READ_REG(offset,pData) (*(pData) = TWSI_READ_CHAR(TWSI_BASE_REG_ADDR | offset))


#define TWSI_WRITE_CHAR(reg,value) (*(volatile unsigned char *) reg = value)
 

#define TWSI_WRITE_REG(offset,data) \
						TWSI_WRITE_CHAR((TWSI_BASE_REG_ADDR | offset),data)




#define TWSI_RX_SR_ACK             BIT3

/* typedefs */
typedef enum _i2cBusStatus
{
    TWSI_OK,
    TWSI_BUSY,
    TWSI_TIMEOUT,
    TWSI_ERROR =0xff

} TWSI_STATUS;

typedef enum _twsiFreq
{

    _100KHZ = 100000,
    _400KHZ = 400000

} TWSI_FREQ;


/** default settings **/
#define TWSI_TIMEOUT_VALUE   0x2000000 /* 1 Second at 500 Mhz */
#define TWSI_SLAVE_FREQ     _100KHZ
#define FFX_CORE_FREQ       133.33e6
#define MAX_TIMEOUT         2

/** public functions **/

/** private functions **/
