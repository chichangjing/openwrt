// ----------------------------------------------------------------------
//  Copyright (c) 2008-2010 Marvell Semiconductor
//  All Rights Reserved
// ----------------------------------------------------------------------

/// \file MacSecTest.cpp : Defines the entry point for the DLL application.

#include "madSample.h"
#include "msecApi.h"
#include "msecFunc.h"
#include "mx_api_helper.h"
#include "msecTest.h"

extern int madSmiIfReadRegister (MAD_DEV     *dev, unsigned int smiAddr, 
                    unsigned int reg, unsigned short * value);
extern int madSmiIfWriteRegister (MAD_DEV     *dev, unsigned int smiAddr, 
                    unsigned int reg, unsigned short value);



void force_macsec_rst (MAD_DEV *ctrl)
{
  madSmiIfWriteRegister (ctrl, 0, 22, 253);
  madSmiIfWriteRegister (ctrl, 0, 13, 0xc000);
  madSmiIfWriteRegister (ctrl, 0, 13, 0x8000);
  if((ctrl->deviceId==MAD_88E1680)||(ctrl->deviceId==MAD_88E1680M))
  {
  madSmiIfWriteRegister (ctrl, 4, 22, 253);
  madSmiIfWriteRegister (ctrl, 4, 13, 0xc000);
  madSmiIfWriteRegister (ctrl, 5, 13, 0x8000);
  }
}


#if 0

#define MAX_MAD_DEVICES	1
static MAD_DEV	maddev[MAX_MAD_DEVICES];
static MAD_U32		nQDDevices = 0;
char * madGetDeviceName ( MAD_DEVICE_ID deviceId)
{

	switch (deviceId)
	{
        case MAD_88E10X0: return ("MAD_88E10X0 ");   
        case MAD_88E10X0S: return ("MAD_88E10X0S ");   
        case MAD_88E1011: return ("MAD_88E1011 ");   
		case MAD_88E104X: return ("MAD_88E104X ");
		case MAD_88E1111: return ("MAD_88E1111/MAD_88E1115 ");
		case MAD_88E1112: return ("MAD_88E1112 ");
		case MAD_88E1116: return ("MAD_88E1116/MAD_88E1116R ");
		case MAD_88E114X: return ("MAD_88E114X ");
		case MAD_88E1149: return ("MAD_88E1149 ");
		case MAD_88E1149R: return ("MAD_88E1149R ");
		case MAD_88E1181: return ("MAD_88E1181 ");
		case MAD_88E3016: return ("MAD_88E3015/MAD_88E3016/MAD_88E3018 ");
		case MAD_88E1121: return ("MAD_88E1121/MAD_88E1121R ");
		case MAD_88E3082: return ("MAD_88E3082/MAD_88E3083 ");
		case MAD_88E1240: return ("MAD_88E1240 ");
		case MAD_88E1340: return ("MAD_88E1340L ");
		case MAD_88E1340M: return ("MAD_88E1340M ");
		case MAD_88E1510: return ("MAD_88E1510 ");
		case MAD_88E1540: return ("MAD_88E1540 ");
		case MAD_88E1548: return ("MAD_88E1548 ");
		case MAD_88E1680: return ("MAD_88E1680 ");
        case MAD_88E1680M: return ("MAD_88E1680M ");
        case MAD_SW1680: return ("MAD_SW1680 ");
		default : return (" No-name ");
	}
} ;

/*
 * The QD device pointer.
 * A constant pointer to the one and only QD device.
 */
MAD_DEV       	*mad_dev = &maddev[0];


MAD_STATUS madStart(MAD_DEV* dev, int usbPort)
{
	MAD_STATUS status = MAD_FAIL;
	MAD_SYS_CONFIG   cfg;
#ifdef MAD_HEADER_MODE
	int	i;
#endif

	memset((char*)&cfg,0,sizeof(MAD_SYS_CONFIG));
	memset((char*)dev,0,sizeof(MAD_DEV));

	cfg.BSPFunctions.readMii   = bspSpecificReadMii;
	cfg.BSPFunctions.writeMii  = bspSpecificWriteMii;
	cfg.BSPFunctions.semCreate = NULL;
	cfg.BSPFunctions.semDelete = NULL;
	cfg.BSPFunctions.semTake   = NULL;
	cfg.BSPFunctions.semGive   = NULL;

	dev->appData = (void *)usbPort;
	cfg.smiBaseAddr = 0;  /* Set SMI Address */

	if((status=mdLoadDriver(&cfg, dev)) != MAD_OK)
	{
		return status;
	}

	MSG_PRINT(("Device Name   : %s\n", madGetDeviceName(dev->deviceId)));
	MSG_PRINT(("Device ID     : 0x%x\n",dev->deviceId));
	MSG_PRINT(("Revision      : 0x%x\n",dev->revision));
	MSG_PRINT(("Base Reg Addr : 0x%x\n",dev->baseRegAddr));
	MSG_PRINT(("No of Ports   : %d\n",dev->numOfPorts));
	MSG_PRINT(("USB port		 : %d\n",dev->appData));

	MSG_PRINT(("MAD has been started.\n"));

	return MAD_OK;
}

void madClose(void) 
{
	if (mad_dev->devEnabled)
		mdUnloadDriver(mad_dev);
}

#else

extern MAD_DEV       	*mad_dev;
#endif


MAD_DEV *macsec_api_init (int usbport)
{
  int    status=0;

  /* initialize usb connection */
//  status = usbInit(0);
  if (status != 0)
  {
    printf("Open USB Device error, status %d\n",status);
    return NULL;
  }
  printf("Open USB Device OK\n");

  madStart(mad_dev,usbport);

  return mad_dev;
}




MAD_STATUS msec_clear_mac_stats(MAD_DEV *dev, int mn)
{
  MAD_STATUS   retVal;
  unsigned short i;

  /* read/clear mac stats */
  for (i = (mn * 0x20 + 0x1000); i<(mn * 0x20 + 0x101f); i++)
  {
    retVal = msecWriteReg (dev, 0, i, 0);
    if ((dev->deviceId==MAD_88E1680)||(dev->deviceId==MAD_88E1680M))
      retVal = msecWriteReg (dev, 4, i, 0);
  }
  return retVal;
}

MAD_STATUS msec_clear_macsec_stats (MAD_DEV *dev, int portnum) 
{
  MAD_STATUS   retVal;
  unsigned short i;

  for (i=(0x1200 + 0x60 * portnum); i<(0x1260 + 0x60 * portnum); i++)
  {
    retVal = msecWriteReg (dev, portnum/4, i, 0);
  }
  return retVal;
}

MAD_STATUS msec_wrchk_reg (MAD_DEV *dev, int port, unsigned short rg, unsigned int data)
{
  MAD_STATUS   retVal;
  MAD_U32 chk;

  retVal = msecWriteReg (dev, port/4, rg, data);
  retVal =  msecReadReg (dev, port/4, rg, &chk);
  if (chk!=data) 
  {
    printf("ERROR -- Addr %x Read back %x, expected %x", rg, chk,  data);
  }
  return retVal;
}

MAD_STATUS msec_clear_tables (MAD_DEV *dev, int portnum)
{
  MAD_STATUS   retVal;
  int side, e, w;
  MAD_U16 addr;

  /* clear ELU */
  /* for {set i [expr $portnum * 0x400 + 0x80]} {$i<[expr $portnum * 0x400 + 0x0bd]} {incr i} {cpu_write $devp $i 0} */

  side = 0;
  for (e =0; e<8; e++)
  {
    //set cval [list]
    for (w=0; w<5; w++)
	{
      addr = portnum * 0x400 + 0x80 + 0x80 * side + e*8 + w;
      /* puts [format "Addr: %x" $addr] */
      retVal = msec_wrchk_reg (dev, portnum, addr, 0);
    }
  }

  /* clear ILU */
  /* for {set i [expr $portnum * 0x400 + 0x100]} {$i<[expr $portnum * 0x400 + 0x128]} {incr i} {cpu_write $devp $i 0} */
  side = 1;
  for (e=0; e<8; e++)
  {
    //set cval [list]
    addr = portnum * 0x400 + 0x80 + 0x80 * side + e*8 + 7;
    retVal = msecWriteReg (dev, portnum/4, addr, 0);
    /*for {set w 0} {$w<8} {incr w} {
    #  set addr [expr $portnum * 0x400 + 0x80 + 0x80 * $side + $e*8 + $w]
      #puts [format "Addr: %x" $addr]
      #wrchk_reg $addr 0
    #}  */
  }
  return retVal;
}

/* ported from ghinit, add duplex control */
/* speed 0: 10, 1: 100, 2: 1000. */
/* duplex MADT_TRUE: duplex, MAD_FALSE: haflex */
MAD_STATUS msec_shinit (MAD_DEV *dev, int speed, MAD_BOOL duplex)
{
  MAD_STATUS   retVal;
  MAD_U32   data;
  MAD_U16   addr;
  int portnum;

  printf(" start msec_shinit\n");

  dev->msec_ctrl.dev_num = 1;    /* indicates the device number (0 if only one) when multiple devices are present on SVB.*/
  dev->msec_ctrl.port_num= 0;   /* Indicates which port (0 to 4) is requesting CPU */
  dev->msec_ctrl.prev_addr = 0;  /* < Prev write address */
  dev->msec_ctrl.prev_dataL = 0; /* < Prev dataL value */
  dev->msec_ctrl.msec_rev;  /* 1340 revision */
  dev->msec_ctrl.msec_rev=check_testchip_revision(dev,0);

  if ((dev->deviceId==MAD_88E1680)||(dev->deviceId==MAD_88E1680M))
  {
    dev->msec_ctrl_1.dev_num = 2;    /* indicates the device number (0 if only one) when multiple devices are present on SVB.*/
    dev->msec_ctrl_1.port_num= 0;   /* Indicates which port (0 to 4) is requesting CPU */
    dev->msec_ctrl_1.prev_addr = 0;  /* < Prev write address */
    dev->msec_ctrl_1.prev_dataL = 0; /* < Prev dataL value */
    dev->msec_ctrl_1.msec_rev;  /* 1340 revision */
    dev->msec_ctrl_1.msec_rev=check_testchip_revision(dev,4);
  }

  switch (speed)
  {
  case 0: speed =10;
  case 1: speed =100;
  case 2: speed =1000;
  default: speed =1000;
  }

  /* fix for soft failure */
  for ( portnum=0; portnum<dev->numOfPorts; portnum++)
 {
	 addr = 0x1d6;
	 data = 0x2406;
    retVal = msecWritePortReg (dev, portnum, addr, data);
	 addr = 0x1da;
    retVal = msecReadPortReg (dev, portnum, addr, &data);
	 addr = 0x1d6;
	 data = 0x2006;
    retVal = msecWritePortReg (dev, portnum, addr, data);
  }
  msec_force_macsec_rst (dev, 0);
  if ((dev->deviceId==MAD_88E1680)||(dev->deviceId==MAD_88E1680M))
    msec_force_macsec_rst (dev, 4);

 for ( portnum=0; portnum<dev->numOfPorts; portnum++)
 {
   printf ("Port port %d init\n", portnum);
  set_dev_ctrl_port_num (dev, portnum);
  msec_set_phy_speed_duplex(dev, portnum, speed, MAD_TRUE);

  /* clear macsec stats */
   msec_clear_macsec_stats (dev, portnum);
   msec_clear_mac_stats (dev, portnum);

  /* take macsec out of reset */
	 addr = 0x1e;
	 data = 1;
    retVal = msecWritePortReg (dev, portnum, addr, data);
	 addr = 0x3;
	 data = 7;
    retVal = msecWritePortReg (dev, portnum, addr, data);
	 addr = 0x1e;
	 data = 0;
    retVal = msecWritePortReg (dev, portnum, addr, data);
	 addr = 0x3;
	 data = 0;
    retVal = msecWritePortReg (dev, portnum, addr, data);
   msec_clear_tables (dev, portnum);
  /*glbl_mac_cfg $portnum $speed */
  msec_set_macsec_speed_duplex(dev, portnum, speed, MAD_TRUE);
/*  set_macsec_speed_duplex $devp $speed $duplex */
 }

  /* sanity check */
    retVal = msecReadReg (dev, 0, 0x400, &data);
//printf("^^^^ read 0x400 sanity check is %x \n", data);

  /* enable mac clear on read */
    retVal = msecWriteReg (dev, 0, 0x110a, 0x31);
    retVal = msecReadReg (dev, 0, 0x110a, &data);
//printf("^^^^ read 0x110a is %x (0x31)\n", data);

  /* sanity check */
    retVal = msecReadReg (dev, 4, 0x400, &data);
//printf("^^^^ read 0x400 sanity check is %x \n", data);

  if ((dev->deviceId==MAD_88E1680)||(dev->deviceId==MAD_88E1680M))
  {
  /* sanity check */
    retVal = msecReadReg (dev, 4, 0x400, &data);
//printf("^^^^ read 0x400 sanity check is %x \n", data);

  /* enable mac clear on read */
    retVal = msecWriteReg (dev, 4, 0x110a, 0x31);
    retVal = msecReadReg (dev, 4, 0x110a, &data);
//printf("^^^^ read 0x110a is %x (0x31)\n", data);

  /* sanity check */
    retVal = msecReadReg (dev, 4, 0x400, &data);
//printf("^^^^ read 0x400 sanity check is %x \n", data);
  }
	
  return MAD_OK;
}





MAD_STATUS macsec_start (MAD_DEV *dev, int speed)
{
  MAD_STATUS   retVal=MAD_OK;
 
  retVal = msec_fixed (dev);
  printf("!!!!!!!!!!!!!!!!!! start msec_start \n");

  retVal = msec_shinit (dev, speed, MAD_TRUE);

  /* initialze maxwell AE core */
  retVal = msec_maxwell_ae_start_init (dev);
  return retVal;
}


