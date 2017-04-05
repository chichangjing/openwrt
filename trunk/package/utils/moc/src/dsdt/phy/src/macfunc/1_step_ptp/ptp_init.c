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

#include <madApi.h>
#include <msec_defines.h>
#include <madApiInternal.h>
#include <ptp_1step.h>


/* #define ONE_STEP_PTP_IN_FPGA 1 */
#undef ONE_STEP_PTP_IN_FPGA  

#ifndef ONE_STEP_PTP_IN_FPGA  
MAD_DEV       *maddev;
#endif

#define TRY_NUMBER  30

#ifdef ONE_STEP_PTP_IN_FPGA
static void mad_delay (int delay)
{
#ifndef __KERNEL__
    volatile int count=delay*1000;
#else
/*    volatile int count=delay*100000; */
    volatile int count=delay*1000;
#endif
    while (count--);
}

 #ifndef __KERNEL__
  #if defined(LINUX)
  #else
    #include <windows.h>
    #include "USB.h" 
    extern unsigned int  USBPort;
    extern HANDLE pDeviceHandle; 
  #endif
 #else  /* in Kernel */
  #ifndef MVL_FFXAV  /* for Firebat */
    extern void madSmiInit(void);
    extern int madSmiRead (unsigned int smiPair, unsigned int smiAddr, unsigned int reg, unsigned int* value);
    extern int madSmiWrite (unsigned int smiPair, unsigned int smiAddr, unsigned int reg, unsigned int value);
    #define defaultPair   2
  #else /* FirefoxAv */
    extern MAD_DEV  *madDev;
    #define defaultPair   0
    void madSmiInit(void)
	{
	}

    int madSmiRead (unsigned int smiPair, unsigned int smiAddr, unsigned int reg, unsigned int* value)
	{
      /*    if(madDev->fmadReadMii(madDev,(unsigned long)(smiAddr+0x10),(unsigned long)reg, value) != MAD_TRUE) */
      if(madDev->fmadReadMii(madDev,(unsigned long)(smiAddr),(unsigned long)reg, value) != MAD_TRUE)
	  { 
        return -1;
	  }
      return 0;
	}

    int madSmiWrite (unsigned int smiPair, unsigned int smiAddr, unsigned int reg, unsigned int value)
	{
      /*    if(madDev->fmadWriteMii(madDev,(unsigned long)(smiAddr+0x10),(unsigned long)reg, value) != MAD_TRUE) */
      if(madDev->fmadWriteMii(madDev,(unsigned long)(smiAddr),(unsigned long)reg, value) != MAD_TRUE)
	  {
        return -1;
	  }
      return 0;

	}

  #endif
 #endif /* end in Kernel */
 #if defined(LINUX)
   #include <linux/autoconf.h>
   #ifdef CONFIG_ARCH_FFXAV
     #define DefaultPhy 0x10
     #define DefaultDev 0x10
   #else
     #define DefaultPhy 0x0
     #define DefaultDev 0x0
   #endif 
 #else /* not LINUX */
  #define DefaultPhy 0x0
  #define DefaultDev 0x0
 #endif 

/*
 * read mii register
 */ 
static unsigned short  msecPtpReadRegister (int devAddr, int port, unsigned short MIIReg)
{
	unsigned int data;
	unsigned short _data;
	unsigned int phyAddr = DefaultPhy; 

	phyAddr += port;
#ifdef __KERNEL__
     madSmiRead (defaultPair, phyAddr,  MIIReg, &data);
#else
    readRegister(pDeviceHandle,USBPort,devAddr+port, MIIReg, &data);
#endif
	_data = data;
/*	PTP_DBG_INFO(("Read port %x phyaddr %x dev %x reg %x :data %x:%x\n", USBPort,phyAddr,devAddr, MIIReg, data, *value)); */
	return _data;
}

/*
 * write mii register
 */ 
static void  msecPtpWriteRegister (int devAddr, int port, unsigned short MIIReg, unsigned short value)
{
	unsigned int phyAddr = DefaultPhy; 

	phyAddr += port;
#ifdef __KERNEL__
/*printk("msecPtpWriteRegister dev 0x%x port 0x%x, reg 0x%x, data 0x%04x\n", phyAddr, port, MIIReg, value); */
     madSmiWrite (defaultPair, phyAddr,  MIIReg, value);
#else
/*printf("msecPtpWriteRegister dev 0x%x port 0x%x, reg 0x%x, data 0x%04x\n", devAddr, port, MIIReg, value); */
	writeRegister(pDeviceHandle,USBPort,devAddr+port,MIIReg,value);
#endif
	return;
}

/* Wrapper to workaround MDIO flakiness  */
void msecPtpWriteRegister_wrap (int port, unsigned short reg, unsigned short data) {
  unsigned short val = ~data;
  int trycount = 0;

  if(reg == 0 || reg == 3) {
    msecPtpWriteRegister(DefaultDev, port, reg, data);
  } else {
    while(val != data) {
      if(trycount > 1) {
         PTP_DBG_INFO( ("msecPtpWriteRegister_wrap warning: write data 0x%x to reg reg but read back 0x%x; trying again\n", data, val));
      }
      msecPtpWriteRegister (DefaultDev, port, reg, data);
      mad_delay(10);
      val = msecPtpReadRegister (DefaultDev, port, reg);
      mad_delay(10);
      /*PTP_DBG_INFO( ("msecPtpWriteRegister_wrap try trycount: reg 0x%x read back 0x%x" data val]); */
      trycount = trycount+1;
      if(trycount > TRY_NUMBER) {
         PTP_DBG_INFO( ("msecPtpWriteRegister_wrap error: too many tries!\n"));
         break;
      }
    }
  }
}

/* write a 1540 register */
unsigned int ptp_read_reg (int port, unsigned short addr) {
  unsigned short data_low, data_high;
  unsigned int data;
  msecPtpWriteRegister (DefaultDev, port, 0, addr);
  mad_delay(50);
  data_low = msecPtpReadRegister (DefaultDev, port, 2);
  mad_delay(5);
  data_high = msecPtpReadRegister (DefaultDev, port, 3);
  data = data_high;
  mad_delay(5);
  /*PTP_DBG_INFO(("ptp_read_reg 0x%x 0x%x" addr (data_high << 16 | data_low] ] */
  data = ((data << 16) | (data_low)));
#ifdef __KERNEL__
/*printk("ptp_read_reg 0x%x data_high 0x%x, data_low 0x%x, data 0x%08x\n", addr, data_high, data_low, data); */
#endif
return (data);
}

DSDT_STATUS ptp_write_reg (int port, unsigned short addr, unsigned int data) {
  DSDT_STATUS   retVal = DSDT_OK;
  /*PTP_DBG_INFO(("ptp_write_reg 0x%x 0x%x" addr data]	 */
#ifdef __KERNEL__
/* printk("ptp_write_reg 0x%x, data 0x%08x\n", addr, data);  */
#endif
  msecPtpWriteRegister(DefaultDev, port, 1, addr);
  mad_delay(5);
  msecPtpWriteRegister(DefaultDev, port, 2, (data & 0xFFFF));
  mad_delay(5);
  msecPtpWriteRegister(DefaultDev, port, 3, (data >> 16));
  mad_delay(5);
  return retVal;
}
/* end of ONE_STEP_PTP_IN_FPGA */

#else  /* 1-step PTP in Phy 1548P */
#include <madHwCntl.h>
#include <msecFunc.h>
/*******************************************************************************
* msecPtpWriteReg
*
* DESCRIPTION:
*       This routine write data Value to MAD MacSec register address.
*
* INPUTS:
*       port - port number.
*       addr - MAD MacSec register addr.
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
    IN  MAD_DEV    *dev,
	IN  int        port,
    IN  unsigned short    addr,
    IN  unsigned long    data
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  int phyaddr;
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_MACSEC_CAPABLE))
  {
      MAD_DBG_ERROR(("Macsec does not support.\n")); 
      return MAD_API_MACSEC_NOT_SUPPORT;
  }
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_1STEP_PTP_CAPABLE))
  {
      MAD_DBG_ERROR(("Macsec does not support.\n")); 
      return MAD_API_PTP_NOT_SUPPORT;
  }

  /* page number 11 and register 0~3 are defined for indirect access to MACsec address space:
     reg 0 - ReadAddr
     reg 1 - WriteAddr
     reg 2 - Data low 16 bits
     reg 3 - Data high 16 bits
    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);
  */
  phyaddr = port;
        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,16,1,addr))
            != DSDT_OK)
        {
            PTP_DBG_INFO(("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,1));
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }



        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,16,2,(unsigned short)(data & 0xFFFF)))
            != DSDT_OK)
        {
            PTP_DBG_INFO(("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,2));
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }



        if((retVal = madHwWritePagedPhyReg(dev,phyaddr,16,3,(unsigned short)(data >> 16)))
            != DSDT_OK)
        {
            PTP_DBG_INFO(("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,3));
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

if (DEBUG_MSEC_WRITE == 1)
		PTP_DBG_INFO(("msecPtpWriteReg: Addr %x Data %x\n", addr, (unsigned int)data));

  return retVal;
}

/*******************************************************************************
* msecPtpReadReg
*
* DESCRIPTION:
*       This routine read data Value to MAD MacSec register address.
*
* INPUTS:
*       port - port number.
*       addr - MAD MacSec register addr.
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
    IN  MAD_DEV    *dev,
	IN  int        port,
    IN  unsigned short    addr,
    OUT  unsigned long    *data
)
{
  DSDT_STATUS   retVal;
  unsigned short data_low, data_high;
  int phyaddr;
  /* do MDC/MDIO write to read addr */
  /* do MDC/MDIO CPU write to DataL (data[15:0]) */
  /* do MDC/MDIO CPU write to DataH (data[31:16]) */

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_MACSEC_CAPABLE))
  {
      MAD_DBG_ERROR(("Macsec does not support.\n")); 
      return MAD_API_MACSEC_NOT_SUPPORT;
  }
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_1STEP_PTP_CAPABLE))
  {
      MAD_DBG_ERROR(("Macsec does not support.\n")); 
      return MAD_API_PTP_NOT_SUPPORT;
  }


  /* put read results in data */
/* page number 11 and register 0~3 are defined for indirect access to MACsec address space:
     reg 0 - ReadAddr
     reg 1 - WriteAddr
     reg 2 - Data low 16 bits
     reg 3 - Data high 16 bits
  */
 /*phyaddr = dev->msecPtp_ctrl.port_num; */
 phyaddr = port;
       if((retVal = madHwWritePagedPhyReg(dev,phyaddr,16,0,addr))
            != DSDT_OK)
        {
            PTP_DBG_INFO(("Writing to paged phy reg (%i,%i) failed.\n",phyaddr,0));
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

        if((retVal = madHwReadPagedPhyReg(dev,phyaddr,16,2,&data_low))
            != DSDT_OK)
        {
            PTP_DBG_INFO(("Reading to paged phy reg (%i,%i) failed.\n",phyaddr,2));
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        if((retVal = madHwReadPagedPhyReg(dev,phyaddr,16,3,&data_high))
            != DSDT_OK)
        {
            PTP_DBG_INFO(("Reading to paged phy reg (%i,%i) failed.\n",phyaddr,3));
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }


  *data = (data_high << 16) + (data_low & 0xFFFF);
if (DEBUG_MSEC_READ == 1)
		PTP_DBG_INFO(("msecPtpReadReg: Addr %x Data %x\n", addr, (unsigned int)*data));
  return retVal;
}



unsigned int ptp_read_reg (int port, unsigned short addr) {
  DSDT_STATUS   retVal;
  unsigned long    data;
  retVal  = msecPtpReadReg(maddev, port, addr, &data);
  if(retVal == DSDT_OK)
	return data;
  else
  {
    PTP_DBG_INFO(("Reading from ptp reg (%x) failed.\n",addr));
	return 0;
  }
}

DSDT_STATUS ptp_write_reg (int port, unsigned short addr, unsigned int data) {
  DSDT_STATUS   retVal;
  retVal  = msecPtpWriteReg(maddev, port, addr, data); 
  if(retVal == DSDT_OK)
  {
      PTP_DBG_INFO(("Wring to ptp reg (%x) success.\n",addr));
  }
  else
  {
      PTP_DBG_INFO(("Wring to ptp reg (%x) failed.\n",addr));
  }
  return retVal;
}


#endif

DSDT_STATUS ptp_dev_init(void * dev) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  if [catch {load MacSecTest.dll} rc] { */
/*	Error "Load MacSecTest.dll failed: rc=rc" */
/*	return -1 */
/*  } else { */
/*  	PTP_DBG_INFO(("Load MacSecTest.dll success." */
/*  } */
/*  cyUsbInitDef */
#ifdef ONE_STEP_PTP_IN_FPGA
#ifdef __KERNEL__
  madSmiInit();
#endif 
#else
  maddev = (MAD_DEV *)dev;
#endif
  /* set configuration for TAI trig out */
  if(((MAD_DEV *)dev)->deviceId==MAD_88E1548)
  {
    int i;
    for(i=0; i<4; i++)
    {
      if((retVal = madHwWritePagedPhyReg(dev,i,3,16,0x5777))!= DSDT_OK)
      {
        PTP_DBG_INFO(("Writing to paged phy reg (%i,%i_%i) failed.\n",i,16,3));
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }
/*      if((retVal = madHwWritePagedPhyReg(dev,i,18,27,0x200a))!= DSDT_OK) */
      if((retVal = madHwWritePagedPhyReg(dev,i,18,27,0x201a))!= DSDT_OK)
      {
        PTP_DBG_INFO(("Writing to paged phy reg (%i,%i_%i) failed.\n",i,27,18));
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }
    }
       /* test only. to power down port 2 and 3 */
       /* retVal = mdSysSetPhyEnable(dev, 2, 0);  
          retVal = mdSysSetPhyEnable(dev, 3, 0);   */
      if((retVal = madHwWritePagedPhyReg(dev,2,0,0,0x1940))!= MAD_OK)
        {
            PTP_DBG_INFO(("Writing to paged phy reg (%i,%i) failed.\n",2,0));
        }
       if((retVal = madHwWritePagedPhyReg(dev,3,0,0,0x1940))!= MAD_OK)
        {
            PTP_DBG_INFO(("Writing to paged phy reg (%i,%i) failed.\n",3,0));
        }
  }

  return retVal;
}  



unsigned int ptp_readcheck_reg (int port, unsigned short addr) {
  int trycount = 0;
  unsigned int val = ptp_read_reg (port, addr);
  unsigned int data =  ~val;
  while(data != val) {
    data = val;
    val = ptp_read_reg (port, addr);
    PTP_DBG_INFO( ("try %d: read 0x%x from address 0x%x, old val 0x%x in port %d.\n", trycount, val, addr, data, port));
    trycount = trycount+1;
    if(trycount > TRY_NUMBER) {
       PTP_DBG_INFO( ("Error: too many tries!\n"));
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
/*    PTP_DBG_INFO( ("try %d: wrote 0x%x to address 0x%x, read back 0x%x in port %d.\n",  */
/*		    trycount, data, addr, val, port)); */
    trycount = trycount+1;
    if(trycount > TRY_NUMBER) {
       PTP_DBG_INFO(("Error: too many tries!\n"));
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

