#include <mvlPtpCopyright.h>

/********************************************************************************
* ptp_init.c
* 
* DESCRIPTION:
*       Functions to access platform layer by PTP in MacSec control program.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <mgdApi.h>
#include <msecApi.h>
#include <msec_defines.h>
#include <mgdApiInternal.h>
#include <ptp_1step.h>

MGD_DEV       *mgddev;

 /* 1-step PTP in 10G Phy 2242 */
#include <msecFunc.h>
/*******************************************************************************
* msecPtpWriteReg
*
* DESCRIPTION:
*       This routine write data Value to MGD MacSec register address.
*
* INPUTS:
*       port - port number.
*       addr - MGD MacSec register addr.
*       data - data value to be written
*   
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS msecPtpWriteReg 
(
    IN  MGD_DEV    *dev,
	IN  int        port,
    IN  unsigned short    addr,
    IN  unsigned long    data
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  if(msecWritePortReg(dev, port, addr, data)!=MGD_OK)
    retVal = DSDT_FAIL;
  return retVal;
}

/*******************************************************************************
* msecPtpReadReg
*
* DESCRIPTION:
*       This routine read data Value to MGD MacSec register address.
*
* INPUTS:
*       port - port number.
*       addr - MGD MacSec register addr.
*   
* OUTPUTS:
*       data - read data value
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS msecPtpReadReg 
(
    IN  MGD_DEV    *dev,
	IN  int        port,
    IN  unsigned short    addr,
    OUT  unsigned long    *data
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  if(msecReadPortReg(dev, port, addr, data)!=MGD_OK)
    retVal = DSDT_FAIL;
  if (DEBUG_MSEC_READ == 1)
		PTP_DBG_INFO("msecPtpReadReg: Addr %x Data %x\n", addr, (unsigned int)*data);
  return retVal;
}



unsigned int ptp_read_reg (int port, unsigned short addr) {
  DSDT_STATUS   retVal;
  unsigned long    data;
  retVal  = msecPtpReadReg(mgddev, port, addr, &data);
  if(retVal == DSDT_OK)
	return data;
  else
  {
    PTP_DBG_INFO("Reading from ptp reg (%x) failed.\n",addr);
	return 0;
  }
}

DSDT_STATUS ptp_write_reg (int port, unsigned short addr, unsigned int data) {
  DSDT_STATUS   retVal;
  retVal  = msecPtpWriteReg(mgddev, port, addr, data); 
  if(retVal == DSDT_OK)
      MGD_DBG_INFO("Wring to ptp reg (%x) success.\n",addr);
  else
      PTP_DBG_INFO("Wring to ptp reg (%x) failed.\n",addr);
  return retVal;
}


DSDT_STATUS ptp_dev_init(void * dev) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  if [catch {load MacSecTest.dll} rc] { */
/*	Error "Load MacSecTest.dll failed: rc=rc" */
/*	return -1 */
/*  } else { */
/*  	PTP_DBG_INFO("Load MacSecTest.dll success." */
/*  } */
/*  cyUsbInitDef */
  mgddev = (MGD_DEV *)dev;
  return retVal;
}  



unsigned int ptp_readcheck_reg (int port, unsigned short addr) {
  int trycount = 0;
  unsigned int val = ptp_read_reg (port, addr);
  unsigned int data =  ~val;
  while(data != val) {
    data = val;
    val = ptp_read_reg (port, addr);
    PTP_DBG_INFO ("try %d: read 0x%x from address 0x%x, old val 0x%x in port %d.\n", trycount, val, addr, data, port);
    trycount = trycount+1;
    if(trycount > 10) {
       PTP_DBG_INFO ("Error: too many tries!\n");
       break;
    }
  }
  return val;
}

DSDT_STATUS ptp_writecheck_reg(int port, unsigned short addr, unsigned int data) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int val = ~data;
  unsigned short  trycount = 0;
  while(data != val) {
    retVal = ptp_write_reg(port, addr, data);
    val = ptp_read_reg(port, addr);
/*    PTP_DBG_INFO ("try %d: wrote 0x%x to address 0x%x, read back 0x%x in port %d.\n",  */
/*		    trycount, data, addr, val, port); */
    trycount = trycount+1;
    if(trycount > 10) {
       PTP_DBG_INFO("Error: too many tries!\n");
       break;
    }
  }
  return retVal;
}

/* Set bits by ORing new value with old value */
DSDT_STATUS set_bits (int port, unsigned short addr, unsigned int data) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int val = ptp_read_reg(port, addr)|data;
  retVal = ptp_writecheck_reg(port, addr, val);
  return retVal;
}

/* Set bits by ANDing inverse of new value with old value */
DSDT_STATUS clear_bits (int port, unsigned short addr, unsigned int data) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int val = ptp_read_reg(port, addr)&~data;
  retVal = ptp_writecheck_reg(port, addr, val );
  return retVal;
}

