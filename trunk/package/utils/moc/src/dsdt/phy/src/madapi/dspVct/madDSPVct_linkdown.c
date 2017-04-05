#include <madCopyright.h>

/***********************************************************************
* madDSPvct_linkdown.c
*
* DESCRIPTION:
*       API's for DSP VCT with link down. The function is no pending mode. 
*       There is wait loops in implementaion.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
************************************************************************/
#include <madApi.h>
#include <madApiInternal.h>
#include <madHwCntl.h>
#include <madDrvPhyRegs.h>
#include <madDrvConfig.h>

/* to call PENDING mode of madDSPvct_linkdown. */
/* to use "madDSPVct_linkdown__sm.c" */

#define MAX_DSP_VCT_WAIT_COUNT 50
#define DSP_VCT_WAIT_TIME 1000
static void madWait (int waitTime)
{
volatile int count=waitTime*100000;
    while (count--);
}

#ifndef ABS
#define ABS(x) ((x>=0)?x:-x)
#endif


MAD_STATUS testChannel(MAD_DEV *dev, MAD_U8 hwPort, char * sw, int lp, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP)
{
  unsigned short data;
  MAD_STATUS  retVal = MAD_OK;
  int count;

  int i = 0;
  int j = 0;
  int l = 0;
  
		
  for(i = 0; i <= 3; i++)
  {
	madDspVctResultP->regLen_21[0][i] = 0;
  }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0x15, 8,0xc9)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0x7,21,0x2640)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Writing from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwReadPagedPhyReg(dev, hwPort,0x7,21,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
	data &= 0x7fff;
	data |= 0x8000;
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0x7,21,data)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Writing from paged phy reg failed.\n"));
      return retVal;
    }
  count = MAX_DSP_VCT_WAIT_COUNT;
  while (1)
  {
      if((retVal = madHwReadPagedPhyReg(dev, hwPort ,7, 21, &data)) != MAD_OK)
	  {
       MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
       return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
	  }
      if( (data&0x0800)==0) break; 
	  madWait(DSP_VCT_WAIT_TIME);
	  if (count--==0) return MAD_FAIL;
  }
    if((retVal = madHwReadPagedPhyReg(dev, hwPort,0x7,20,&madDspVctResultP->reg20_7[0])) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwReadPagedPhyReg(dev, hwPort,0x15,1,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    madDspVctResultP->regLen_21[0][0] = data & 0xff;
    madDspVctResultP->regLen_21[0][1] = (data>>8)&0xff;
    if((retVal = madHwReadPagedPhyReg(dev, hwPort,0x15,2,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    madDspVctResultP->regLen_21[0][2] = data & 0xff;
    madDspVctResultP->regLen_21[0][3] = (data>>8)&0xff;

	MAD_DBG_ERROR(("@@@ madDspVctResultP->regLen_21[0][0] %02x, [1] %02x, [2] %02x, [3] %02x \n",
		madDspVctResultP->regLen_21[0][0],
		madDspVctResultP->regLen_21[0][1],
		madDspVctResultP->regLen_21[0][2],
		madDspVctResultP->regLen_21[0][3]
		));


		

/************************************ 1 done  **************************/

/************************************ 2 start  **************************/

  for(i = 0; i <= 3; i++)
  {
	madDspVctResultP->regLen_21[1][i] = 0;
  }
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x15, 8,0x12c)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x7,21,0x2640)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Writing from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0x7,21,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
	data &= 0x7fff;
	data |= 0x8000;
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x7,21,data)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Writing from paged phy reg failed.\n"));
      return retVal;
    }
  count = MAX_DSP_VCT_WAIT_COUNT;
  while (1)
  {
      if((retVal = madHwReadPagedPhyReg(dev, hwPort ,7, 21, &data)) != MAD_OK)
	  {
       MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
       return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
	  }
      if( (data&0x0800)==0) break; 
	  madWait(DSP_VCT_WAIT_TIME);
	  if (count--==0) return MAD_FAIL;
  }
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0x7,20,&madDspVctResultP->reg20_7[1])) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0x15,1,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    madDspVctResultP->regLen_21[1][0] = data & 0xff;
    madDspVctResultP->regLen_21[1][1] = (data>>8)&0xff;
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0x15,2,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    madDspVctResultP->regLen_21[1][2] = data & 0xff;
    madDspVctResultP->regLen_21[1][3] = (data>>8)&0xff;

	MAD_DBG_ERROR(("@@@ madDspVctResultP->regLen_21[1][0] %02x, [1] %02x, [2] %02x, [3] %02x\n",
		madDspVctResultP->regLen_21[1][0],
		madDspVctResultP->regLen_21[1][1],
		madDspVctResultP->regLen_21[1][2],
		madDspVctResultP->regLen_21[1][3],
         madDspVctResultP->regLen_21[0] 
		));

  return retVal;
						
}

 
/**********************************************************************
* madDSPvct_linkdown
*
* DESCRIPTION:
*       This API performs the DSP virtual cable test with link down. 
*       It starts DSP VCT and collects all DSP VCT data.
*       The supported VCT are
*           accuracy cable length,
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - logical port number.
*
* OUTPUTS:
*       DSPVctResult - The data and status of DSP VCT.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*    
*
***********************************************************************/

MAD_STATUS madDspVct_linkdown
(
   IN MAD_DEV *dev, 
   IN  MAD_LPORT       port,
   OUT MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP
)
{
  int k = 0;
  int l = 0;
  char sw[64];		
  MAD_STATUS  retVal;

  MAD_U8 hwPort;

  MAD_CRITIC_INFO(("@@@ Start madDspVct_linkdown (hold, and no pending mode) collect data \n"));

    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY(dev, MAD_PHY_DSP_VCT) ||
        !MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("DSP VCT not supported.\n"));
        return MAD_API_DSPVCT_NOT_SUPPORT;
    }

  for(k = 0; k <1; k++)
  {
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xfc, 24,0x9555)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xfc, 25,0x9575)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 17,0xdc0c)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 16,0x2159)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 17,0x0088)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 16,0x215d)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 17,0x8906)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 16,0x215b)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 17,0x2148)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 16,0x2144)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 17,0x8034)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 16,0x2141)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xfb, 8,0x3606)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0, 16,0x3070)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev, hwPort,0, 0,0x9140)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }

    retVal = testChannel(dev, hwPort, sw, k, madDspVctResultP);

  }

  MAD_DBG_INFO(("Exception ! File Syntax Error or Runtime Error!\n"));
  return retVal;
}

