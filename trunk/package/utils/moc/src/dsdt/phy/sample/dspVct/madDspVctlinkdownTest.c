#include <madCopyright.h>
/********************************************************************************
* madDspVctLinkdownTest.c
*
* DESCRIPTION:
*       DSP VCT link down test function (madDspVctLinkdownTest)
*       The test function to call API <madDspVct_linkdown> to start and get DSP VCT data.
*       Based on the data the test function processes and calculate to get VCT 
*       results. Now the results include: 
*       acuracy cable length.
*
* DEPENDENCIES:
*       MAD Driver should be loaded and Phys should be enabled
*       before running sample test functions as specified in
*       sample\Initialization\madApiInit.c.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include <madSample.h>
#include <madDSPVctApi.h>
#include <madDSPVct.h>

#include <math.h>

#define MAX_DSP_VCT_WAIT_COUNT 500
#define DSP_VCT_WAIT_TIME 1000
static void madWait (int waitTime)
{
volatile int count=waitTime*100000;
    while (count--);
}

#ifndef ABS
#define ABS(x) ((x>=0)?x:-x)
#endif

static double var_double[4] = {0.0, 0.0, 0.0, 0.0};
static int var_Stat[4]  = {0, 0, 0, 0};
static double len[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; 
static double aveL[4] = {0.0, 0.0, 0.0, 0.0}; 

extern MAD_STATUS madDspVct_linkdown(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP);
extern MAD_STATUS madDspVct_linkdown_sm(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP);

MAD_STATUS madDspVctLinkdownApp(MAD_DEV *dev, int port)
{

  int i = 0;
  int j = 0;
  int l = 0;
  int k = 0;
  int m = 0;
  double length = 0.00;
  double len1, len2, reportLen, tmp;
  int reportStat;
#ifdef MAD_SM_VCT
  int count;
#endif		
  MAD_STATUS  retVal;

  MAD_DSP_VCT_RESULT_STRUCT madDspVctResult;
  MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP=&madDspVctResult;

  MSG_PRINT(("!!! Start madDspVctdLinkDown \n"));

  madDspVctResult.resultValid = 0;
#ifndef MAD_SM_VCT
    retVal = madDspVct_linkdown(dev, port, &madDspVctResult);
#else
    /*
     *    Start and get Cable Test Result
    */
	count = MAX_DSP_VCT_WAIT_COUNT;
    do
    {
      if(((retVal = madDspVct_linkdown_sm(dev,port, &madDspVctResult)) != MAD_OK)
//           && ((retVal&MAD_RUN_ST_MASK)!=MAD_VCT_AGAIN)
           && ((retVal&MAD_RUN_ST_MASK)!=MAD_PENDING))
	  {
        if(retVal == MAD_API_DSPVCT_NOT_SUPPORT )
          MSG_PRINT(("madDspVct_linkdown_sm return Failed. The device does not support DSP VCT!\n"));
        else
          MSG_PRINT(("madDspVct_linkdown_sm return Failed. Check Link down!\n"));

        return retVal;
	  }
      if ((retVal&MAD_RUN_ST_MASK)==MAD_PENDING)
	  {
	    madWait(DSP_VCT_WAIT_TIME);
	    if (count--==0) return MAD_FAIL;
	  }
    } while ((retVal&MAD_RUN_ST_MASK)==MAD_PENDING);


#endif

    if(retVal != MAD_OK)
    {
      MSG_PRINT(("madDspVct_linkdown_sm failed to collect data.\n"));
      return retVal;
    }

  for(i = 0; i <= 3; i++)
  {
	var_double[i] = 0.0;
	var_Stat[i] = 0;
//	madDspVctResultP->regLen_21[0][i] = 0;
//	madDspVctResultP->regLen_21[1][i] = 0;
  }

  for(i = 0; i <= 3; i++)
  {
	var_Stat[i] = (madDspVctResultP->reg20_7[0] & (0xF<<(i*4))) >> (i*4);
	if(var_Stat[i] > 0)
		var_double[i] = madDspVctResultP->regLen_21[0][i] * 0.815;
	else
		var_double[i] = -1.0;
  }
  MSG_PRINT(("madDspVctResultP->reg20_7[0]=%x var_double[0]=%f var_double[1]=%f var_double[2]=%f var_double[3]=%f\n",
	    madDspVctResultP->reg20_7[0], var_double[0], var_double[1], var_double[2],var_double[3]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[0][0]=0x%x var_double[0]: %f \n", madDspVctResultP->regLen_21[0][0], var_double[0]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[0][1]=0x%x var_double[1]: %f \n", madDspVctResultP->regLen_21[0][1], var_double[1]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[0][2]=0x%x var_double[2]: %f \n", madDspVctResultP->regLen_21[0][2], var_double[2]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[0][3]=0x%x var_double[3]: %f \n", madDspVctResultP->regLen_21[0][3], var_double[3]));
		
		
  for(i = 0; i <= 2; i++)
  {
    for (j = i+1; j<=3; j++)
	{
	  if (var_double[j] > var_double[i])
	  {
		tmp = var_double[i];
		var_double[i] = var_double[j];
		var_double[j] = tmp;
	  }
	} 
  } 
	MSG_PRINT(("!!! 1 var_double[0]: %f \n", var_double[0]));
	MSG_PRINT(("!!! 1 var_double[1]: %f \n", var_double[1]));
	MSG_PRINT(("!!! 1 var_double[2]: %f \n", var_double[2]));
	MSG_PRINT(("!!! 1 var_double[3]: %f \n", var_double[3]));

  if (var_double[1] != -1.0)
  {
	if ((ABS(var_double[0] - var_double[1]) < 5.0) && 
		(ABS(var_double[1] - var_double[2]) < 5.0))
	{
	  len1 = (var_double[0] + var_double[1] + var_double[2])/3;
	}
	else if ((ABS(var_double[1] - var_double[2]) < 5.0) &&
		     (ABS(var_double[2] - var_double[3]) < 5.0)) 
	{
	  len1 = (var_double[1] + var_double[2] + var_double[3])/3;
	}
	else 
	{
      for(i = 0; i <= 3; i++)
	  {
		if ((var_double[i] != -1) && (var_double[i] > 70.0))
		{
			len1 = var_double[i];
			goto test1;
		}
		else
		  len1 = -1.0;
	  }
	}
  }
  else
  {
    for(i = 2; i <= 3; i++)
	{
      if ((var_double[i] != -1) && (var_double[i] > 70.0))
	  {
		len1 = var_double[i];
		goto test1;
	  }
	  else
	  {
		len1 = -1.0;
	  }
	}
  }
		
test1:
  MAD_CRITIC_INFO((" len1 = %f\n", len1));

/************************************ 1 done  **************************/

/************************************ 2 start  **************************/

  for(i = 0; i <= 3; i++)
  {
	var_double[i] = 0.0;
	var_Stat[i] = 0;
//	madDspVctResultP->regLen_21[1][i] = 0;
  }
  for(i = 0; i <= 3; i++)
  {
	var_Stat[i] = ( madDspVctResultP->reg20_7[1] & (0xF<<(i*4)) ) >> (i*4);
	if (var_Stat[i] > 0 )
	{
		var_double[i] = madDspVctResultP->regLen_21[1][i] * 0.815;
	}
	else
	{
		var_double[i] = -1;
	}
  }	
	MSG_PRINT(("^^^@@@ reg20_7[1]=0x%x var_double: %f %f %f %f\n", madDspVctResultP->reg20_7[1],
	        var_double[0], var_double[1], var_double[2], var_double[3]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[1][0]=0x%x var_double[0]: %f \n", madDspVctResultP->regLen_21[1][0], var_double[0]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[1][1]=0x%x var_double[1]: %f \n", madDspVctResultP->regLen_21[1][1], var_double[1]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[1][2]=0x%x var_double[2]: %f \n", madDspVctResultP->regLen_21[1][2], var_double[2]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[1][3]=0x%x var_double[3]: %f \n", madDspVctResultP->regLen_21[1][3], var_double[3]));

  {
    for(j = i+1; j <= 3; j++)
	{
	  if (var_double[j] > var_double[i] )
	  {
		tmp = var_double[i];
		var_double[i] = var_double[j];
		var_double[j] = tmp;
	  }
	} 
  }

  if(var_double[1] != -1.0) 
  {
	if ((ABS(var_double[0] - var_double[1]) < 5.0) && 
		(ABS(var_double[1] - var_double[2]) < 5.0))
	  len2 = (var_double[0] + var_double[1] + var_double[2])/3;
	else if ((ABS(var_double[1] - var_double[2]) < 5.0) &&
	         (ABS(var_double[2] - var_double[3]) < 5.0))
	  len2 = (var_double[1] + var_double[2] + var_double[3])/3;
	else 
	  len2 = -1.0;
  }
  else
  {
			len2 = -1.0;
  }
		
  MAD_CRITIC_INFO(("len2 = %f\n",len2));
	
  if ((len1 == -1.0) && (len2 == -1.0)) 
  {
	MAD_CRITIC_INFO(("INVALID\n"));
  }
  else if (len1 == -1.0) 
  {
	reportLen = len2;
	reportStat = madDspVctResultP->reg20_7[1];
  }
  else if (len2 == -1.0) 
  {
	reportLen = len1;
	reportStat = madDspVctResultP->reg20_7[0];
  }
  else
  {
	if (len1 > 50.0 )
	{
		reportLen = len1;
		reportStat = madDspVctResultP->reg20_7[0];
	}
	else
	{
		reportLen = len2;
		reportStat = madDspVctResultP->reg20_7[1];
	}	
  }
  MAD_CRITIC_INFO(("@@@ reportLen =  %f reportStat 0x%x\n", reportLen, reportStat));
						
  return MAD_OK;
}
 
/********************************************************************************
* madDspVctLinkdownTest
*
* DESCRIPTION:
*       The function test all DSP VCT Linkdown API. It call API madDstVct_linkdown.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - logical port number.
*
*
*******************************************************************************/

MAD_STATUS madDspVctLinkdownTest(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;
//	MAD_DSP_VCT_RESULT *DSPVctResult=(MAD_DSP_VCT_RESULT *)data;
	MAD_U32 testMask=1;

    if (dev == 0)
    {
        MSG_PRINT(("MAD driver is not initialized\n"));
        return MAD_FAIL;
    }

    /*
     *	Start and get DSP VCT Test Result
    */
		if((status = madDspVctLinkdownApp(dev,port)) != MAD_OK)
		{
	      MSG_PRINT(("madDSPvct return Failed\n"));
		  testMADDisplayStatus(status);
		  return status;
		}

		MSG_PRINT(("DSP VCT  Test Result for Port %i\n",(int)port));
//		displayDSPVCT(type&testMask, DSPVctResult);
    return MAD_OK;
}

