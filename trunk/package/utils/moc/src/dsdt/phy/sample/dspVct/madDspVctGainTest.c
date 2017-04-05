#include <madCopyright.h>
/********************************************************************************
* madDspVctGainTest.c
*
* DESCRIPTION:
*       DSP VCT Gain test function (madDspVctGainTest)
*       The test function to call API <madDspVct_gain> to start and get DSP VCT data.
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

extern MAD_STATUS madDspVct_gain(MAD_DEV *dev, MAD_LPORT port, MAD_DSP_VCT_GAIN_RESULT_STRUCT *madDspVctGainResultP);

MAD_STATUS madDspVctGainApp(MAD_DEV *dev, int port)
{
  int i = 0;
  int j = 0;
  int l = 0;
  int k = 0;
  int m = 0;
  int aveLen = 0;

  double length = 0.00;
  double tmp1;
#ifdef MAD_SM_VCT
  int count;
#endif
  MAD_STATUS  retVal;

  MAD_DSP_VCT_GAIN_RESULT_STRUCT madDspVctGainResult;
  MAD_DSP_VCT_GAIN_RESULT_STRUCT *madDspVctGainResultP=&madDspVctGainResult;

  MSG_PRINT(("!!! Start madDspVctd_gain \n"));

    madDspVctGainResult.resultValid = 0;
#if 1
    retVal = madDspVct_gain(dev, port, &madDspVctGainResult);
#else
    /*
     *    Start and get Cable Test Result
    */
	count = MAX_DSP_VCT_WAIT_COUNT;
    do
    {
      if(((retVal = madDspVct_sm(dev,port, &madDspVctGainResult)) != MAD_OK)
//           && ((retVal&MAD_RUN_ST_MASK)!=MAD_VCT_AGAIN)
           && ((retVal&MAD_RUN_ST_MASK)!=MAD_PENDING))
	  {
        if(retVal == MAD_API_DSPVCT_NOT_SUPPORT )
          MSG_PRINT(("madDspVct_sm return Failed. The device does not support DSP VCT!\n"));
        else
          MSG_PRINT(("madDspVct_sm return Failed. Check Link down!\n"));

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
      MSG_PRINT(("madDspVctgain failed to collect data.\n"));
      return retVal;
    }

//	MSG_PRINT(("!!! k %d End to collecte data, and to calculate length\n", k));

	MSG_PRINT(("!!! k %d To calculate length on Gain data \n", k));
    for(l = 0; l <= 3; l++)
	{
//	  MSG_PRINT(k & " ")
      for(m = 0; m <= 9; m++)
	  {
//		MSG_PRINT(("!!! k %d madDspVctGainResultP->regGain[%d][%d] = %d \n", k, l, m, madDspVctGainResultP->regGain[l][m]));
		if(madDspVctGainResultP->reg23_252 == 0) 
		{
		  if(madDspVctGainResultP->regGainPair[l][m] > 49) 
		  {
			len[m] = 180.99 * log10(madDspVctGainResultP->regGainPair[l][m]) - 306.805;
		  } 
		  else 
		  {
		    len[m] = 0;
		  }
		}
		else if(madDspVctGainResultP->reg23_252 == 1)
		{
		  if(madDspVctGainResultP->regGainPair[l][m] > 20) 
		  {
		 	len[m] = 191.10 * log10(madDspVctGainResultP->regGainPair[l][m]) - 234.06;
		  } 
		  else 
		  {
		 	len[m] = 0;
		  }
		}
		else if(madDspVctGainResultP->reg23_252 == 2) 
		{
		  if(madDspVctGainResultP->regGainPair[l][m] > 29)
		  {
		 	len[m] = 186.495 * log10(madDspVctGainResultP->regGainPair[l][m]) - 213.225;
		  }
		  else
		  {
			len[m] = 0;
		  }
		}
	  }
#if 0
      for(i = 0; i <= 9; i++)
	  {
		MSG_PRINT(("!!! len[%d] = %f \n", i, len[i]));
	  }
#endif
      for(i = 0; i <= 8; i++)
	  {
        for(j = i+1; j <= 9; j++)
		{
	      if(len[j] > len[i]) 
		  {
			tmp1 = len[i];
			len[i] = len[j];
			len[j] = tmp1;
		  }
		} 
	  }
	  aveL[l] = (len[2] + len[3] + len[4] + len[5] + len[6] + len[7])/6;
	  MSG_PRINT(("!!! aveL[%d] = %f\n", l, aveL[l]));
      aveLen = (madDspVctGainResultP->regLen_pair[0] +  
                madDspVctGainResultP->regLen_pair[1] +
                madDspVctGainResultP->regLen_pair[2] +
                madDspVctGainResultP->regLen_pair[3]) / 4;
	  MSG_PRINT(("!!! API get len = %d, %d, %d, %d. Averagy length = %d\n", 
		  madDspVctGainResultP->regLen_pair[0],
		  madDspVctGainResultP->regLen_pair[1],
		  madDspVctGainResultP->regLen_pair[2],
		  madDspVctGainResultP->regLen_pair[3],
		  aveLen));
	}
    /* End to collecte data */		

    for(i = 0; i <= 2; i++)
	{
      for(j = i+1; j <= 3; j++)
	  {
	    if(aveL[j] > aveL[i]) 
		{
		  tmp1 = aveL[i];
		  aveL[i] = aveL[j];
		  aveL[j] = tmp1;
		}
	  } 
	}
		
	length = (aveL[1] + aveL[2])/2;
	MSG_PRINT(("!!! Gain length = %f\n", length));
    /* End to get Gain data */		

  return MAD_OK;
}
 
/********************************************************************************
* madDspVctTest
*
* DESCRIPTION:
*       The function test all DSP VCT API. It call API madDstVct.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - logical port number.
*
*
*******************************************************************************/

MAD_STATUS madDspVctGainTest(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;
	MAD_U32 testMask=1;

    if (dev == 0)
    {
        MSG_PRINT(("MAD driver is not initialized\n"));
        return MAD_FAIL;
    }

    /*
     *	Start and get DSP VCT Test Result
    */
		if((status = madDspVctGainApp(dev,port)) != MAD_OK)
		{
	      MSG_PRINT(("madDSPvct_gain return Failed\n"));
		  testMADDisplayStatus(status);
		  return status;
		}

		MSG_PRINT(("DSP VCT Gain Test Result for Port %i\n",(int)port));
    return MAD_OK;
}

