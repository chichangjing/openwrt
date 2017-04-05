/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
* gt64260UartDrv.c - UART interface driver.
*
* DESCRIPTION:
*       Enable the use of specific UART operations.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/*#include <prestera/os/gtTypes.h>*/
#include <gtExtDrv/drivers/gtUartDrv.h>
#include <gtOs/gtOsIo.h>

/* vxWorks specific includes */
#include <vxWorks.h>
#include <sioLib.h>
#include <sysLib.h>
#include <errno.h>
#include <drv/sio/st16552Sio.h>

/* Default stdout UART channel */
SIO_CHAN * sioChannel = (SIO_CHAN *)NULL; 

/* MACRO - checking for UART initalization fo PSS internal use */
#define EXT_DRV_UART_INIT_DONE()   \
  ( sioChannel != (SIO_CHAN *)NULL )

#define  NUMBER_OF_SENDING_TRYS  0xFFFFFF
/*******************************************************************************
* extDrvUartInit
*
* DESCRIPTION:
*       Setting the UART environment to work with PSS polling mode.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvUartInit ( void )
{
  
  /* reads the default stdout UART channel */  
  if ((sioChannel = sysSerialChanGet(0)) == (SIO_CHAN *)ERROR )
    return (GT_FAIL);
  
  /* Bind the tx buffer function to the global pointer used by osPrintSync */
  /* The use of binding function enable us to stay support with 
      users that won't like to use this feature. */
  osPrintSyncUartBindFunc = extDrvUartTxBuffer;
  
  return (GT_OK);

}

/*******************************************************************************
* extDrvUartTxBuffer
*
* DESCRIPTION:
*       Sending ASCII buffer to the UART port in pooling mode
*
* INPUTS:
*       buffer      - pointer to buffer containing the data to be send to the UART port.
*       buffSize    - The size of the buffer to be sent to the UART port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvUartTxBuffer
(
  char * buffer,
  GT_U32  buffSize
)
{
  int i, poolRetVal, dec;
  
  /* Checking if the UART port initilize to work with the PSS */
  if ( ! EXT_DRV_UART_INIT_DONE() )
    return (GT_FAIL);
    
  for (i=0; i<buffSize; i++)
  {
    /* re-setting delay value */
    dec = NUMBER_OF_SENDING_TRYS;
    do {
         /* Call function from the BSP sending charecter in polling mode */
         poolRetVal = (sioChannel->pDrvFuncs->pollOutput)(sioChannel, buffer[i]);
    } while ( (poolRetVal == EAGAIN) && (--dec) );

    /* On Success. */
    if ( poolRetVal == OK )
      continue;

    /* Fail to sent the char over the UART port */
    if ( dec == 0 )
      return (GT_FAIL);
  }
  
  return (GT_OK);
}

