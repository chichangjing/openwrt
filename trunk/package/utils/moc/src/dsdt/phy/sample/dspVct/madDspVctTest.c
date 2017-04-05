#include <madCopyright.h>
/********************************************************************************
* madDspVctTest.c
*
* DESCRIPTION:
*       DSP VCT test function (madDspVctTest)
*       The test function to call API <madDspVct> to start and get DSP VCT data.
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

extern MAD_STATUS madDspVct(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP);
extern MAD_STATUS madDspVct_sm(MAD_DEV *dev, int port, MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP);

MAD_STATUS madDspVctApp(MAD_DEV *dev, int port)
{

  int i = 0;
  int j = 0;
  int l = 0;
  int k = 0;
  int m = 0;
  double length = 0.00;
  double len1, len2, reportLen, tmp;
  double tmp1;
#ifdef MAD_SM_VCT
  int count;
#endif
  MAD_STATUS  retVal;

  MAD_DSP_VCT_RESULT_STRUCT madDspVctResult;
  MAD_DSP_VCT_RESULT_STRUCT *madDspVctResultP=&madDspVctResult;

  MSG_PRINT(("!!! Start madDspVctd \n"));
  for(k = 0; k <= 9; k++)
  {
    madDspVctResult.resultValid = 0;
#ifndef MAD_SM_VCT
    retVal = madDspVct(dev, port, &madDspVctResult);
#else
    /*
     *    Start and get Cable Test Result
    */
	count = MAX_DSP_VCT_WAIT_COUNT;
    do
    {
      if(((retVal = madDspVct_sm(dev,port, &madDspVctResult)) != MAD_OK)
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
      MSG_PRINT(("madDspVct_sm failed to collect data.\n"));
      return retVal;
    }

//	MSG_PRINT(("!!! k %d End to collecte data, and to calculate length\n", k));

	MSG_PRINT(("!!! k %d To calculate length on Gain data \n", k));
    for(l = 0; l <= 3; l++)
	{
//	  MSG_PRINT(k & " ")
      for(m = 0; m <= 9; m++)
	  {
//		MSG_PRINT(("!!! k %d madDspVctResultP->regGain[%d][%d] = %d \n", k, l, m, madDspVctResultP->regGain[l][m]));
		if(madDspVctResultP->reg23_252 == 0) 
		{
		  if(madDspVctResultP->regGain[l][m] > 36) 
		  {
			len[m] = 179.075 * log10(madDspVctResultP->regGain[l][m]) - 278.895;
		  } 
		  else 
		  {
		    len[m] = 0;
		  }
		}
		else if(madDspVctResultP->reg23_252 == 1)
		{
		  if(madDspVctResultP->regGain[l][m] > 13) 
		  {
		 	len[m] = 182.355 * log10(madDspVctResultP->regGain[l][m]) - 206.495;
		  } 
		  else 
		  {
		 	len[m] = 0;
		  }
		}
		else if(madDspVctResultP->reg23_252 == 2) 
		{
		  if(madDspVctResultP->regGain[l][m] > 12)
		  {
		 	len[m] = 184.91 * log10(madDspVctResultP->regGain[l][m]) - 201.01;
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

	/* len with threshould 1 */

    for(i=0; i <= 3; i++)
	{
	  var_double[i] = 0.0;
	  var_Stat[i] = 0;
	}				
   for(i = 0; i <= 3; i++)
	{
	  var_Stat[i] = ( madDspVctResultP->reg20_7[0] & (0xf<<(i*4)) ) >> (i*4);
	  if(var_Stat[i] > 0) 
	  {
		var_double[i] = madDspVctResultP->regLen_21[0][i] * 0.815;						
	  }
	}
	MSG_PRINT(("!!! k=%d madDspVctResultP->reg23_252=%d  length=%f madDspVctResultP->reg20_7[0]=0x%x\n",
		k, madDspVctResultP->reg23_252, length, madDspVctResultP->reg20_7[0]));

	MSG_PRINT(("!!! madDspVctResultP->regLen_21[0][0]=0x%x var_double[0]: %f \n", madDspVctResultP->regLen_21[0][0], var_double[0]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[0][1]=0x%x var_double[1]: %f \n", madDspVctResultP->regLen_21[0][1], var_double[1]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[0][2]=0x%x var_double[2]: %f \n", madDspVctResultP->regLen_21[0][2], var_double[2]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[0][3]=0x%x var_double[3]: %f \n", madDspVctResultP->regLen_21[0][3], var_double[3]));
		
    for(i = 0; i <= 2; i++)
	{
      for(j = i+1; j <= 3; j++)
	  {
	    if(var_double[j] > var_double[i]) 
		{
		  tmp = var_double[i];
		  var_double[i] = var_double[j];
		  var_double[j] = tmp;
		}
	  } 
	}
	if(var_double[0] > 70.0) 
	{
	  len1 = var_double[0];
	} 
	else 
	{
	  if(ABS(var_double[0] - var_double[1]) < 5.0) 
	  {
	    len1 = (var_double[0] + var_double[1])/2;
	  } 
	  else if(ABS(var_double[1] - var_double[2]) < 5.0) 
	  {
		len1 = (var_double[1] + var_double[2])/2;	
	  }
	}
	MSG_PRINT(("!!! with threshould 1 len =  %f", len1));
	MSG_PRINT(("\n"));

	/* len with threshould 2 */
		
    for(i = 0; i <= 3; i++)
	{
	  var_double[i] = 0.0;
	  var_Stat[i] = 0;
	}

    for(i = 0; i <= 3; i++)
	{
	  var_Stat[i] = ( madDspVctResultP->reg20_7[1] & (0xf<<(i*4)) ) >> (i*4);
	  if(var_Stat[i] > 0)
	  {
		var_double[i] = madDspVctResultP->regLen_21[1][i] * 0.815;						
	  }
	}
	MSG_PRINT(("!!! k=%d madDspVctResultP->reg23_252=%d  length=%f madDspVctResultP->reg20_7[1]=0x%x\n",
		k, madDspVctResultP->reg23_252, length, madDspVctResultP->reg20_7[1]));

	MSG_PRINT(("!!! madDspVctResultP->regLen_21[1][0]=0x%x var_double[0]: %f \n", madDspVctResultP->regLen_21[1][0], var_double[0]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[1][1]=0x%x var_double[1]: %f \n", madDspVctResultP->regLen_21[1][1], var_double[1]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[1][2]=0x%x var_double[2]: %f \n", madDspVctResultP->regLen_21[1][2], var_double[2]));
	MSG_PRINT(("!!! madDspVctResultP->regLen_21[1][3]=0x%x var_double[3]: %f \n", madDspVctResultP->regLen_21[1][3], var_double[3]));
		
    for(i = 0; i <= 2; i++)
	{
      for(j = i+1; j <= 3; j++)
	  {
	    if(var_double[j] > var_double[i]) 
		{
		  tmp = var_double[i];
		  var_double[i] = var_double[j];
		  var_double[j] = tmp;
		}
	  } 
	}
	if(var_double[0] > 70.0) 
	{
	  len2 = var_double[0];
	}
	else  
	{
	  if(ABS(var_double[0] - var_double[1]) < 5.0) 
	  {
		len2 = (var_double[0] + var_double[1])/2;
	  }
	  else if(ABS(var_double[1] - var_double[2]) < 5.0)
	  {
		len2 = (var_double[1] + var_double[2])/2;	
	  }
	}
	MSG_PRINT(("!!! with threshould 2 len =  %f", len2));

	if(length >= 60.0)
	{
	  if((len1 < length + 15.0) && ( length - 15.0 < len1))
	  {
		reportLen = len1;
	  }
	  else if((len2 < length + 15.0) && ( length - 15.0 < len2))
	  {
	 	reportLen = len2;
	  }
	  else 
	  {
		reportLen = length;
	  }
	}
	else
	{
	  if((len2 < length + 15.0) && ( length - 15.0 < len2))
	  {
	 	reportLen = len2;
	  }
	  else if((len1 < length + 15.0) && ( length - 15.0 < len1))
	  {
		reportLen = len1;
	  }
	  else 
	  {
		reportLen = length;
	  }
	}
	MSG_PRINT(("\n!!! reportLen =  %f\n", reportLen));
  }
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

MAD_STATUS madDspVctTest(MAD_DEV *dev,MAD_LPORT port)
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
		if((status = madDspVctApp(dev,port)) != MAD_OK)
		{
	      MSG_PRINT(("madDSPvct return Failed\n"));
		  testMADDisplayStatus(status);
		  return status;
		}

		MSG_PRINT(("DSP VCT  Test Result for Port %i\n",(int)port));
//		displayDSPVCT(type&testMask, DSPVctResult);
    return MAD_OK;
}

