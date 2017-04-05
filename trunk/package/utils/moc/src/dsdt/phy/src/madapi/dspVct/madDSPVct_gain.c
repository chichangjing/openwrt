#include <madCopyright.h>

/***********************************************************************
* madDSPVct_gain.c
*
* DESCRIPTION:
*       API's for DSP VCT (Gain based) after gaigabit link is established.
*       The function is no pending mode. There is wait loops in implementaion.
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

/* to call PENDING mode of madDspVct_gain. */
/* to use "madDSPVct_sm.c" */

/*#include <madDSPVctApi.h> */
/*#include <madDSPVct.h> */

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

static unsigned short gain_len_table_gain0[64] =
{ 1, 2, 4, 5, 7, 8,10,11,12,14,15,16,18,19,20,21,
 23,24,25,26,27,28,29,30,32,33,34,35,36,37,38,39,
 40,41,41,42,43,44,45,46,47,48,49,49,50,51,52,53,
 54,54,55,56,57,57,58,59,60,60,61,62,63,63,63,63};
static unsigned short gain_len_table_gain1[64] =
{ 18, 22, 25, 29, 32, 35, 39, 42, 45, 50, 53, 55, 60, 62, 65, 67,
  69, 71, 73, 75, 77, 79, 81, 83, 85, 86, 88, 90, 91, 93, 95, 96,
  98, 99,101,102,103,105,106,108,109,110,111,113,114,115,116,118,
 119,120,121,122,123,124,126,127,128,129,130,130,130,130,130,130};
static unsigned short gain_len_table_gain2[48] =
{ 62, 65, 67, 70, 72, 75, 77, 79, 81, 84, 86, 88, 90, 91, 93, 95,
  97, 99,100,102,104,105,107,108,110,111,113,114,116,117,118,120,
 121,122,124,125,126,127,129,130,130,130,130,130,130,130,130,130};

/**********************************************************************
* madDSPvct_gain
*
* DESCRIPTION:
*       This API performs the DSP virtual cable test on Gain-based)
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

MAD_STATUS madDspVct_gain
(
   IN MAD_DEV *dev, 
   IN  MAD_LPORT       port,
   OUT MAD_DSP_VCT_GAIN_RESULT_STRUCT *madDspVctResultP
)
{

  int i = 0, j=0;
  int count;
		
  unsigned short data;
  MAD_STATUS  retVal;

  MAD_U8 hwPort;

  MAD_CRITIC_INFO(("@@@ Start madDspVct_gain (hold, and no pending mode) collect data \n"));

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
    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_DSP_VCT_TYPE2))
    {
        MAD_DBG_ERROR(("DSP VCT type2 not supported.\n"));
        return MAD_API_DSPVCT_NOT_SUPPORT;
    }

    /* check link up */

	/* read data */
  for(j=0; j<10; j++)  /* 10 times reading */
  {
    for(i=0; i<4; i++)  /* 4 pair */
	{
      if((retVal = madHwWritePagedPhyReg(dev, hwPort,0xff, 16,0x1118+i)) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading from paged phy reg failed.\n"));
       return retVal;
	  }
		
/*      MAD_CRITIC_INFO(("@@@ setting done, wait reg 16_0xff = 0x8000 \n")); */
      count = MAX_DSP_VCT_WAIT_COUNT;
      while (1)
	  {
        if((retVal = madHwReadPagedPhyReg(dev, hwPort ,0xff, 16, &data)) != MAD_OK)
		{
          MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
          return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
		}
        if( (data&0x8000)==0x8000) break; 
	    madWait(DSP_VCT_WAIT_TIME);
	    if (count--==0) return MAD_FAIL;
	  }
	
      if((retVal = madHwReadPagedPhyReg(dev, port ,0xff, 18, &madDspVctResultP->regGainPair[i][j])) != MAD_OK)
	  {
        MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
	  }
  if((retVal = madHwReadPagedPhyReg(dev, port ,0xff, 23, &data)) != MAD_OK)
  {
    MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
    return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }
      MAD_DBG_ERROR(("Reading page 0xff phy 18 data %d in pair %d %d times effcient %d.\n", madDspVctResultP->regGainPair[i][j], i, j,data&3));

	}
  }

  /* read Gain efficiant */	
  if((retVal = madHwReadPagedPhyReg(dev, port ,0xfc, 23, &madDspVctResultP->reg23_252)) != MAD_OK)
  {
    MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
    return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }

  madDspVctResultP->reg23_252 &= 3; 

  for(i=0; i<4; i++)  /* 4 pair */
  {
    int dataL = 0;
    for(j=0; j<10; j++)  /* 10 times avarage */
      dataL += madDspVctResultP->regGainPair[i][j];
	dataL /= 10;
   if( madDspVctResultP->reg23_252 ==0)
   {
	 if(dataL<=49)
       madDspVctResultP->regLen_pair[i] = 0;
     else
       madDspVctResultP->regLen_pair[i] = gain_len_table_gain0[dataL-49];
   }
   else if( madDspVctResultP->reg23_252 ==1)
   {
	 if(dataL<=20)
       madDspVctResultP->regLen_pair[i] = 0;
     else
       madDspVctResultP->regLen_pair[i] = gain_len_table_gain1[dataL-20];
   }
   else if( madDspVctResultP->reg23_252 ==2)
   {
	 if(dataL<=29)
       madDspVctResultP->regLen_pair[i] = 0;
     else
       madDspVctResultP->regLen_pair[i] = gain_len_table_gain2[dataL-29];
   }
   else
     madDspVctResultP->regLen_pair[i] = 130;
  }
  return MAD_OK;
}
 

