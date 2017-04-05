#include <madCopyright.h>

/***********************************************************************
* madDSPVct_sm.c
*
* DESCRIPTION:
*       API's for DSP VCT. The function is pending mode.
*       There is no wait loops in implementaion. The applicaton program,
*       will process return <PENDING> signal. 
*       See sample /dspVct/madDspVctTest. 
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

typedef enum {
	DSPVCT_INIT_ST,
	DSPVCT_LINKUP,
    DSPVCT_LEN1_ST,
    DSPVCT_LEN2_ST,
    DSPVCT_DONE_ST
} MAD_DSPVCT_SM_STATE;

#define MAD_DSPVCT_GAIN_L_ID 4
#define MAD_DSPVCT_GAIN_M_ID 10

typedef struct {
  MAD_DSPVCT_SM_STATE  currentSt;

} MAD_DSPVCT_SM_CFG;

#if 0
typedef MAD_STATUS (*FMAD_DSPVCT_ACTION)(
                        MAD_DEV*   dev,
                        int port, 
                        MAD_DSPVCT_SM_CFG * dspVctSmCfg);

FMAD_DSPVCT_ACTION madDspVctAct [DSPVCT_DONE_ST+1] = {

};
#endif
static MAD_STATUS madDspVctAct_linkup(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP);
static MAD_STATUS madDspVctAct_len1(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP);
static MAD_STATUS madDspVctAct_len2(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP);
static MAD_STATUS madDspVctAct_done(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP);

static MAD_STATUS madDspVctAct_init(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP)
{
  madDspVctSmCfgP->currentSt=DSPVCT_INIT_ST;
  return madDspVctAct_linkup(dev, port, madDspVctResultP, madDspVctSmCfgP);
}


static MAD_STATUS madDspVctAct_linkup(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP)
{
  unsigned short data;
  MAD_STATUS  retVal;
  if(madDspVctSmCfgP->currentSt==DSPVCT_INIT_ST)
  {
	if((retVal = madHwWritePagedPhyReg(dev,port,0xfc, 24,0x9555)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xfc, 25,0x9575)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 17,0xdc0c)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 16,0x2159)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 17,0x0088)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 16,0x215d)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 17,0x8906)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 16,0x215b)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 17,0x2148)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 16,0x2144)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 17,0x8034)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xff, 16,0x2141)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0xfb, 8,0x3606)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0, 16,0x3070)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    if((retVal = madHwWritePagedPhyReg(dev,port,0, 0,0x9140)) != MAD_OK)
    {
      MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
      return retVal;
    }
    madDspVctSmCfgP->currentSt=DSPVCT_LINKUP;
  }

  if(madDspVctSmCfgP->currentSt==DSPVCT_LINKUP)
  {
	int i;
/*	MAD_CRITIC_INFO(("@@@ setting done, wait reg 17_0 = 0x400 \n")); */
        for(i=0; i <= 3; i++)
		{
	      madDspVctResultP->regLen_21[0][i] = 0;
		}				
        if((retVal = madHwWritePagedPhyReg(dev,port,0x15, 8,0xc9)) != MAD_OK)
		{
          MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
          return retVal;
		}
        if((retVal = madHwWritePagedPhyReg(dev,port,0x7,21,0x2640)) != MAD_OK)
		{
          MAD_DBG_ERROR(("Writing from paged phy reg failed.\n"));
          return retVal;
		}
        if((retVal = madHwReadPagedPhyReg(dev,port,0x7,21,&data)) != MAD_OK)
		{
          MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
          return retVal;
		}
	    data &= 0x7fff;
	    data |= 0x8000;
        if((retVal = madHwWritePagedPhyReg(dev,port,0x7,21,data)) != MAD_OK)
		{
          MAD_DBG_ERROR(("Writing from paged phy reg failed.\n"));
          return retVal;
		}
        madDspVctSmCfgP->currentSt=DSPVCT_LEN1_ST;
        return madDspVctAct_len1(dev, port, madDspVctResultP, madDspVctSmCfgP);
	 
  }
  else
  {
      MAD_DBG_ERROR(("MAD DSP VCT failed.\n"));
	  return MAD_FAIL;
  }
}

		
static MAD_STATUS madDspVctAct_len1(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP)
{
  int i = 0;
  unsigned short data;
  MAD_STATUS  retVal;
  if(madDspVctSmCfgP->currentSt==DSPVCT_LEN1_ST)
  {
    if((retVal = madHwReadPagedPhyReg(dev,port ,7, 21, &data)) != MAD_OK)
	{
      MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
      return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
	}
    if( (data&0x0800)==0x800) 
	{
      if((retVal = madHwReadPagedPhyReg(dev,port,0x7,20,&madDspVctResultP->reg20_7[0])) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
        return retVal;
	  }
      if((retVal = madHwReadPagedPhyReg(dev,port,0x15,1,&data)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
        return retVal;
	  }
      madDspVctResultP->regLen_21[0][0] = data & 0xff;
      madDspVctResultP->regLen_21[0][1] = (data>>8)&0xff;
      if((retVal = madHwReadPagedPhyReg(dev,port,0x15,2,&data)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
        return retVal;
	  }
      madDspVctResultP->regLen_21[0][2] = data & 0xff;
      madDspVctResultP->regLen_21[0][3] = (data>>8)&0xff;

/*  MAD_CRITIC_INFO(("@@@ to collect madDspVctResultP->regLen_21 1\n")); */

      for(i = 0; i <= 3; i++)
	  {
  	    madDspVctResultP->regLen_21[1][i] = 0;
	  }
      if((retVal = madHwWritePagedPhyReg(dev,port,0x15, 8,0x12c)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
        return retVal;
	  }
      if((retVal = madHwWritePagedPhyReg(dev,port,0x7,21,0x2640)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Writing from paged phy reg failed.\n"));
        return retVal;
	  }
      if((retVal = madHwReadPagedPhyReg(dev,port,0x7,21,&data)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
        return retVal;
	  }
	  data &= 0x7fff;
	  data |= 0x8000;
      if((retVal = madHwWritePagedPhyReg(dev,port,0x7,21,data)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Writing from paged phy reg failed.\n"));
        return retVal;
	  }

      madDspVctSmCfgP->currentSt=DSPVCT_LEN2_ST;
      return madDspVctAct_len2(dev, port, madDspVctResultP, madDspVctSmCfgP);
	}
	else
	{
      return MAD_PENDING;
	}
  }
  else
  {
      MAD_DBG_ERROR(("MAD DSP VCT failed.\n"));
	  return MAD_FAIL;
  }

}


static MAD_STATUS madDspVctAct_len2(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP)
{
  unsigned short data;
  MAD_STATUS  retVal;
  if(madDspVctSmCfgP->currentSt==DSPVCT_LEN2_ST)
  {
    if((retVal = madHwReadPagedPhyReg(dev,port ,7, 21, &data)) != MAD_OK)
	{
      MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
      return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
	}
    if( (data&0x0800)==0x800) 
	{
	  if((retVal = madHwReadPagedPhyReg(dev,port,0x7,20,&madDspVctResultP->reg20_7[1])) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
        return retVal;
	  }
      if((retVal = madHwReadPagedPhyReg(dev,port,0x15,1,&data)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
        return retVal;
	  }
      madDspVctResultP->regLen_21[1][0] = data & 0xff;
      madDspVctResultP->regLen_21[1][1] = (data>>8)&0xff;
      if((retVal = madHwReadPagedPhyReg(dev,port,0x15,2,&data)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
        return retVal;
	  }
      madDspVctResultP->regLen_21[1][2] = data & 0xff;
      madDspVctResultP->regLen_21[1][3] = (data>>8)&0xff;
      madDspVctSmCfgP->currentSt=DSPVCT_DONE_ST;
      return madDspVctAct_done(dev, port, madDspVctResultP, madDspVctSmCfgP);
	}
	else
	{
      return MAD_PENDING;
	}
  }
  else
  {
      MAD_DBG_ERROR(("MAD DSP VCT failed.\n"));
	  return MAD_FAIL;
  }

}

static MAD_STATUS madDspVctAct_done(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP, MAD_DSPVCT_SM_CFG *madDspVctSmCfgP)
{
  MAD_STATUS  retVal = MAD_OK;

  madDspVctSmCfgP->currentSt=DSPVCT_DONE_ST;
  madDspVctResultP->resultValid = 0;
 return retVal;
}

static  MAD_DSPVCT_SM_CFG madDspVctSmCfg;

static 
MAD_STATUS madDspVct_1680
(    
   IN  MAD_DEV          *dev,
   IN  MAD_U8           port,
   OUT MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP
)
{
  MAD_STATUS retVal;
  MAD_DSPVCT_SM_CFG *madDspVctSmCfgP = &madDspVctSmCfg;

  if(madDspVctResultP->resultValid==0)
  {
    MAD_CRITIC_INFO(("@@@ Start madDspVct_linkdown (pending mode) collect data \n"));
    madDspVctSmCfgP->currentSt=DSPVCT_INIT_ST;
    madDspVctResultP->resultValid = 1;
  }

  switch (madDspVctSmCfgP->currentSt)
  {
    case DSPVCT_INIT_ST:
      return madDspVctAct_init(dev, port, madDspVctResultP, madDspVctSmCfgP);
    case DSPVCT_LINKUP:
      return madDspVctAct_linkup(dev, port, madDspVctResultP, madDspVctSmCfgP);
    case DSPVCT_LEN1_ST:
      return madDspVctAct_len1(dev, port, madDspVctResultP, madDspVctSmCfgP);
    case DSPVCT_LEN2_ST:
      return madDspVctAct_len2(dev, port, madDspVctResultP, madDspVctSmCfgP);
    case DSPVCT_DONE_ST:
      return madDspVctAct_done(dev, port, madDspVctResultP, madDspVctSmCfgP);
	default:
      return MAD_FAIL;
  }

  return retVal;
}




/**********************************************************************
* madDspVct_linkdown
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
*       MAD_PENDING   - on pending
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*    
*
***********************************************************************/

MAD_STATUS madDspVct_linkdown_sm
(
   IN MAD_DEV *dev, 
   IN  MAD_LPORT       port,
   OUT MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP
)
{
  MAD_STATUS  retVal;

  MAD_U8 hwPort;


/* MAD_CRITIC_INFO(("@@@ Start madDspVct_linkdown (pending mode) collect data \n")); */

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

  do
  {
         if(((retVal=madDspVct_1680(dev,hwPort,madDspVctResultP)) != MAD_OK) &&
             ((retVal&MAD_RUN_ST_MASK)!=MAD_VCT_AGAIN) &&
             ((retVal&MAD_RUN_ST_MASK)!=MAD_PENDING))
         {
            MAD_DBG_ERROR(("Running DSP VCT failed.\n"));
            return (retVal | MAD_API_FAIL_ALTVCT_RUN);
         }
         if ((retVal&MAD_RUN_ST_MASK)==MAD_PENDING)
         {
             return retVal;
         }
  } while ((retVal&MAD_RUN_ST_MASK)==MAD_VCT_AGAIN);



    /*
     * reset the phy
     */
   if((madHwPagedReset(dev,hwPort,0)) != MAD_OK)
    {
        MAD_DBG_ERROR(("Soft Reset failed.\n"));
        return (MAD_API_FAIL_SW_RESET);
    }

  return MAD_OK;
}
 

