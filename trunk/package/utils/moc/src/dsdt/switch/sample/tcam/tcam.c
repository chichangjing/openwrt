#include <Copyright.h>
/********************************************************************************
* tcam.c
*
* DESCRIPTION:
*      iHow to use TCAM API functions
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msSample.h"
#include "gtHwCntl.h"

static void testDisplayStatus(GT_STATUS status)
{
    switch(status)
    {
        case GT_OK :
            MSG_PRINT(("Passed.\n"));
            break;
        case GT_FAIL :
            MSG_PRINT(("Failed.\n"));
            break;
        case GT_BAD_PARAM :
            MSG_PRINT(("Bad Parameter.\n"));
            break;
        case GT_NOT_SUPPORTED :
            MSG_PRINT(("Not Supported.\n"));
            break;
        case GT_NOT_FOUND :
            MSG_PRINT(("Not Found.\n"));
            break;
        case GT_NO_MORE :
            MSG_PRINT(("No more Item.\n"));
            break;
        case GT_NO_SUCH :
            MSG_PRINT(("No Such Item.\n"));
            break;
        default:
            MSG_PRINT(("Failed.\n"));
            break;
    }
}

/* sample TCAM */
void getTcamFrameHd(GT_U8 destAddr[], GT_U8 srcAddr[], GT_U16 *tag, 
        GT_U16 *pri, GT_U16 *vid, GT_U16 *ethType, GT_TCAM_FRM_HD *tcamHrData)
{
  MSG_PRINT(("Get TCAM Frame header.\n"));

  memcpy(destAddr, tcamHrData->paraFrmHd.destAddr, 6);
  memcpy(srcAddr, tcamHrData->paraFrmHd.srcAddr, 6);
  *tag = tcamHrData->paraFrmHd.tag;
  *pri = (tcamHrData->paraFrmHd.priVid>>12)&0xf;
  *vid = tcamHrData->paraFrmHd.priVid&0xfff;
  *ethType = tcamHrData->paraFrmHd.ethType;
}

void setTcamFrameHd(GT_U8 destAddr[], GT_U8 srcAddr[], GT_U16 tag, 
GT_U16 pri, GT_U16 vid, GT_U16 ethType, GT_TCAM_FRM_HD *tcamHrData)
{
  MSG_PRINT(("Set TCAM Frame header.\n"));

  memcpy(tcamHrData->paraFrmHd.destAddr, destAddr, 6);
  memcpy(tcamHrData->paraFrmHd.srcAddr, srcAddr, 6);
  tcamHrData->paraFrmHd.tag = tag;
  tcamHrData->paraFrmHd.priVid = (pri<<12)|(vid&0xfff);
  tcamHrData->paraFrmHd.ethType = ethType;
}

static void displayTcamFrameHd(GT_TCAM_FRM_HD *tcamHrData)
{
  MSG_PRINT(("TCAM Frame header.\n"));

  MSG_PRINT(("Dest address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
  tcamHrData->paraFrmHd.destAddr[0], tcamHrData->paraFrmHd.destAddr[1],
  tcamHrData->paraFrmHd.destAddr[2], tcamHrData->paraFrmHd.destAddr[3],
  tcamHrData->paraFrmHd.destAddr[4], tcamHrData->paraFrmHd.destAddr[5]
  ));
  MSG_PRINT(("Src address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
  tcamHrData->paraFrmHd.srcAddr[0], tcamHrData->paraFrmHd.srcAddr[1],
  tcamHrData->paraFrmHd.srcAddr[2], tcamHrData->paraFrmHd.srcAddr[3],
  tcamHrData->paraFrmHd.srcAddr[4], tcamHrData->paraFrmHd.srcAddr[5]
  ));
  MSG_PRINT(("Tag: %x\n", tcamHrData->paraFrmHd.tag));
  MSG_PRINT(("PRI: %x\n", (tcamHrData->paraFrmHd.priVid&0xf000)>>12));
  MSG_PRINT(("VID: %x\n", tcamHrData->paraFrmHd.priVid&0xfff));
  MSG_PRINT(("ether type: %x\n", tcamHrData->paraFrmHd.ethType));

}

/* show=0: no display.
   show=1: display basic parameters.
   show=2: display frame data parameters. 
   show=4: display frame raw data. 
   show=0xf: display all parameters. */
void displayTcamData(GT_QD_DEV *dev, GT_TCAM_DATA *tcamData, int show)
{
  int i; 
  if(!show)
    return;
  MSG_PRINT(("TCAM data.\n"));

#if 1
  if(show&4)
  {
    MSG_PRINT(("\nFirst part of TCAM Frame\n"));
    for(i=0; i<28; i++)
    {
      if(i%14==0)
      MSG_PRINT(("\nframeOctet[%02d]: %04x ", i, tcamData->rawFrmData[0].frame0.frame[i]));
      else
      MSG_PRINT(("%04x ", tcamData->rawFrmData[0].frame0.frame[i]));
    }
    for(i=0; i<28; i++)
    {
      if(i%8==0)
      MSG_PRINT(("\nframeOctet[%02d]: %04x ", i, tcamData->rawFrmData[0].frame1.frame[i]));
      else
      MSG_PRINT(("%04x ", tcamData->rawFrmData[0].frame1.frame[i]));
    }
    for(i=0; i<28; i++)
    {
      if(i%8==0)
      MSG_PRINT(("\nframeOctet[%02d]: %04x ", i, tcamData->rawFrmData[0].frame2.frame[i]));
      else
      MSG_PRINT(("%04x ", tcamData->rawFrmData[0].frame2.frame[i]));
    }
    for(i=0; i<6; i++)
    {
      if(i%8==0)
      MSG_PRINT(("\nframeOctet[%02d]: %04x ", i, tcamData->rawFrmData[1].frame3.frame[i]));
      else
      MSG_PRINT(("%04x ", tcamData->rawFrmData[1].frame3.frame[i]));
    }

	if(dev->is96Frame==1)
   {
    MSG_PRINT(("\nSecond part of TCAM Frame\n"));
    for(i=0; i<28; i++)
    {
      if(i%8==0)
      MSG_PRINT(("\nframeOctet[%02d]: %04x ", i, tcamData->rawFrmData[1].frame0.frame[i]));
      else
      MSG_PRINT(("%04x ", tcamData->rawFrmData[1].frame0.frame[i]));
    }
    for(i=0; i<28; i++)
    {
      if(i%8==0)
      MSG_PRINT(("\nframeOctet[%02d]: %04x ", i, tcamData->rawFrmData[1].frame1.frame[i]));
      else
      MSG_PRINT(("%04x ", tcamData->rawFrmData[1].frame1.frame[i]));
    }
    for(i=0; i<6; i++)
    {
      if(i%8==0)
      MSG_PRINT(("\nframeOctet[%02d]: %04x ", i, tcamData->rawFrmData[1].frame3.frame[i]));
      else
      MSG_PRINT(("%04x ", tcamData->rawFrmData[1].frame3.frame[i]));
    }
    MSG_PRINT(("\n"));
   }
  }
#endif

  if(show&3)
  {
  MSG_PRINT(("frameType: %x frameTypeMask: %x\n", tcamData->frameType, tcamData->frameTypeMask));
  MSG_PRINT(("spv: %x spvMask: %x\n", tcamData->spv, tcamData->spvMask));
  MSG_PRINT(("ppri: %x ppriMask: %x\n", tcamData->ppri, tcamData->ppriMask));
  MSG_PRINT(("pvid: %x pvidMask: %x\n", tcamData->pvid, tcamData->pvidMask));

  displayTcamFrameHd((GT_TCAM_FRM_HD *)tcamData->frameOctet);
  }
#if 1
  if(show&2)
  {
    int end;
	if(dev->is96Frame==1)
		end = 96;
	else end = 48;

  for(i=0; i<end; i++)
    MSG_PRINT(("frameOctet[%d]: %x frameOctetMask[%d]: %x\n",  \
             i, tcamData->frameOctet[i], i, tcamData->frameOctetMask[i]));
  }
#endif

  if(show&1)
  {
  MSG_PRINT(("continu: %x\n", tcamData->continu));
  MSG_PRINT(("interrupt: %x\n", tcamData->interrupt));
  MSG_PRINT(("IncTcamCtr: %x\n", tcamData->IncTcamCtr));
  MSG_PRINT(("vidOverride: %x\n", tcamData->vidOverride));
  MSG_PRINT(("vidData: %x\n", tcamData->vidData));
  MSG_PRINT(("nextId: %x\n", tcamData->nextId));
  MSG_PRINT(("qpriOverride: %x\n", tcamData->qpriOverride));
  MSG_PRINT(("qpriData: %x\n", tcamData->qpriData));
  MSG_PRINT(("fpriOverride: %x\n", tcamData->fpriOverride));
  MSG_PRINT(("fpriData: %x\n", tcamData->fpriData));
  MSG_PRINT(("qpriAvbOverride: %x\n", tcamData->qpriAvbOverride));
  MSG_PRINT(("qpriAvbData: %x\n", tcamData->qpriAvbData));
  MSG_PRINT(("dpvOverride: %x\n", tcamData->dpvSF));
  MSG_PRINT(("dpvData: %x\n", tcamData->dpvData));
  MSG_PRINT(("factionOverride: %x\n", tcamData->factionOverride));
  MSG_PRINT(("factionData: %x\n", tcamData->factionData));
  MSG_PRINT(("ldBalanceOverride: %x\n", tcamData->ldBalanceOverride));
  MSG_PRINT(("ldBalanceData: %x\n", tcamData->ldBalanceData));
  MSG_PRINT(("debugPort: %x\n", tcamData->debugPort));
  MSG_PRINT(("highHit: %x\n", tcamData->highHit));
  MSG_PRINT(("lowHit: %x\n", tcamData->lowHit));
	/* after 6390 */
  MSG_PRINT(("dpvDataHi: %x\n",     tcamData->dpvDataHi));

  MSG_PRINT(("egActPoint: %x\n",     tcamData->egActPoint));
  MSG_PRINT(("unKnownFilter: %x\n",     tcamData->unKnownFilter));
  MSG_PRINT(("vp: %x\n",     tcamData->vp));
  MSG_PRINT(("colorMode: %x\n",     tcamData->colorMode));
  MSG_PRINT(("dpvMode: %x\n",     tcamData->dpvMode));

  MSG_PRINT(("DSCP: %x\n",     tcamData->DSCP));
  MSG_PRINT(("ldBalanceData1: %x\n",     tcamData->ldBalanceData1));

  MSG_PRINT(("factionData1: %x\n",     tcamData->factionData1));
  
  }
}


GT_U32 sampleTcam(GT_QD_DEV *dev)
{
  GT_STATUS status;
  GT_U32 testResults = 0;
  int i, j, port, endReg;

  GT_TCAM_DATA     tcamData;
  GT_TCAM_DATA     tmpTcamData;
  GT_U32        tcamPointer;
  TEST_API testAPI;

#define NumberOfEntry   4  /* 10 */
#define  Is96Frame   0    /*  1 */

  MSG_PRINT(("TCAM API test \n"));

  MSG_PRINT(("\n  TCAM API Flush all test \n"));

#if 0
    /*
     *  Tcam mode enable API
     */
extern GT_STATUS testPortU8Type(GT_QD_DEV *dev, TEST_API* api, int testLimit);
    testAPI.getFunc.u8 = (GT_API_GET_U8)gprtGetTcamMode;
    testAPI.setFunc.u8 = (GT_API_SET_U8)gprtSetTcamMode;
    if((status = testPortU8Type(dev,&testAPI, 3) != GT_OK))
    {
      MSG_PRINT(("gprtGetTcamMode failure returned "));
      testDisplayStatus(status);
      return status;
    }
    MSG_PRINT(("Tcam mode enable API test "));
    testDisplayStatus(status);
    MSG_PRINT(("\n"));
#endif

  for (port=0; port<dev->maxPorts; port++)
  {
/*    if(status = gprtSetTcamMode(dev, port, TCAM_MODE_DISABLE) != GT_OK) 
    if(status = gprtSetTcamMode(dev, port, TCAM_MODE_ENABLE_96) != GT_OK) */
    if(status = gprtSetTcamMode(dev, port, TCAM_MODE_ENABLE_48) != GT_OK) 
    {
      MSG_PRINT(("gprtSetTcamMode return failed"));
      return status;
    }
  }

  status = GT_OK;
  if((status = gtcamFlushAll(dev)) != GT_OK)
  {
    MSG_PRINT(("gtcamFlushAll returned "));
    testDisplayStatus(status);
    return status;
  }
#if 1
  for(i=0; i<NumberOfEntry; i+=2)
  {
    tcamPointer = i;
    dev->is96Frame = Is96Frame;
	if(dev->is96Frame)
		endReg = 96;
	else
		endReg = 48;

    MSG_PRINT(("gtcamReadTCAMData tcam entry: %d \n", (int)tcamPointer));
    if((status = gtcamReadTCAMData(dev, tcamPointer, &tmpTcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamReadTCAMData returned \n"));
        testDisplayStatus(status);
        return status;
    }
    displayTcamData(dev, &tmpTcamData, 0xf);
  }
#endif
#if 0
  MSG_PRINT(("\n  TCAM API Purge and read test \n"));
  for(i=0; i<3; i++)
  {
    tcamPointer = i;
    memset((char *)&tcamData, 0x55+i, sizeof(GT_TCAM_DATA));
    dev->is96Frame = Is96Frame;
    MSG_PRINT(("gtcamPurgyEntry entry: %d \n", (int)tcamPointer));

    if((status = gtcamPurgyEntry(dev, tcamPointer, &tcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamPurgyEntry returned "));
        testDisplayStatus(status);
        return status;
    }

    MSG_PRINT(("gtcamReadTCAMData tcam entry: %d \n", (int)tcamPointer));
    if((status = gtcamReadTCAMData(dev, tcamPointer, &tmpTcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamReadTCAMData returned \n"));
        testDisplayStatus(status);
        return status;
    }
    displayTcamData(dev, &tmpTcamData, 1);
	MSG_PRINT(("Expected tcam data: \n"));
    displayTcamData(dev, &tcamData, 1);
  }
#endif
#if 1
    MSG_PRINT(("TCAM API Load and read test \n"));
    /* fill Tcam data */
  for(i=4; i<NumberOfEntry+4; i+=2)
  {
    tcamData.frameType = 0x3;
    tcamData.frameTypeMask = 0x3;
    tcamData.spv = 0x7ff;
    tcamData.spvMask = 0x7ff;
    tcamData.ppri = 0x9;
    tcamData.ppriMask = 0x9;
    tcamData. pvid = 0xb;
    tcamData.pvidMask = 0xb;

    for(j=0; j<endReg; j++)
    {
      tcamData.frameOctet[j] = 10+j;
      tcamData.frameOctetMask[j] = 80+j;
    }

#if 1
    {
      GT_TCAM_FRM_HD *tcamHrData = (GT_TCAM_FRM_HD *)tcamData.frameOctet;
      GT_U8 destAddr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
      GT_U8 srcAddr[6] = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd};
      GT_U16 tag = 0x3456;
      GT_U16 pri = 0xa;
      GT_U16 vid = 0x123;
      GT_U16 ethType = 0x88f7;

      setTcamFrameHd(destAddr, srcAddr, tag, pri, vid, ethType, tcamHrData);
    }
#endif
    tcamData.continu = 0x1;
    tcamData.interrupt = 0x52;
    tcamData.IncTcamCtr = 0x53;
    tcamData.vidData = 0x55;
    tcamData.nextId = 0x56;
    tcamData.qpriData = 0x58;

    tcamData.qpriAvbData = 0x5b;
    tcamData.dpvData = 0x5d;
    tcamData.factionData = 0x5f;
    tcamData.ldBalanceData = 0x61;
    tcamData.debugPort = 0x62;
    tcamData.highHit = 0x63;
    tcamData.lowHit = 0x64;
	/* after 6390 */
    tcamData.dpvDataHi=0xaaaa;

    tcamData.egActPoint=0x2a;
    tcamData.unKnownFilter=2;
    tcamData.vp=1;
    tcamData.colorMode=2;
    tcamData.dpvMode=2;

    tcamData.DSCP=0x2a;
    tcamData.ldBalanceData1=2;

    tcamData.factionData1=0x2aaa;
    
    tcamPointer = i;
    MSG_PRINT(("TCAM API Load test for entry %d\n", (int)tcamPointer));			
	
    if((status = gtcamLoadEntry(dev, tcamPointer, &tcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamLoadEntry returned "));
        testDisplayStatus(status);
        return status;
    }
#if 0
  }
  for(i=4; i<NumberOfEntry+4; i+=2)
  {
    tcamPointer = i;
#endif
    MSG_PRINT(("TCAM API read test for entry %d\n", (int)tcamPointer));
    if((status = gtcamReadTCAMData(dev, tcamPointer, &tcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamReadTCAMData returned "));
        testDisplayStatus(status);
        return status;
    }
    displayTcamData(dev, &tcamData, 7);
  }
#endif

  tcamPointer = 0;

  {
    GT_U32        last_tcamPointer;
    do {
      MSG_PRINT(("TCAM API Get next test start entry %d \n", (int)tcamPointer));
      last_tcamPointer =  tcamPointer;
      if((status = gtcamGetNextTCAMData(dev, &tcamPointer, &tcamData)) != GT_OK)
     {
        MSG_PRINT(("gtcamGetNextTCAMData returned "));
        testDisplayStatus(status);
        return status;
     }

      MSG_PRINT(("TCAM API Get next test next entry %d \n", (int)tcamPointer));
      displayTcamData(dev, &tcamData, 3);

      if(last_tcamPointer == tcamPointer)
	  {
        MSG_PRINT(("TCAM API Get next test get last entry %d \n", (int)tcamPointer));
         break;
	  }
    } while (tcamPointer!=0);
  }


#if 1
    MSG_PRINT(("TCAM API Flush single and read test on entry %d \n", (int)tcamPointer));
    dev->is96Frame = 1;
    if((status = gtcamFlushEntry(dev, tcamPointer)) != GT_OK)
    {
        MSG_PRINT(("gtcamFlushEntry returned "));
        testDisplayStatus(status);
        return status;
    }
    if((status = gtcamReadTCAMData(dev, tcamPointer, &tcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamReadTCAMData returned "));
        testDisplayStatus(status);
        return status;
    }
    displayTcamData(dev, &tcamData, 7);

#endif



#if 0
  MSG_PRINT(("\n  TCAM API Purge and read then next \n"));
/*     displayTcamData(dev, &tcamData, 1); */
/*   for(i=0; i<NumberOfEntry; i++)  */
  for(i=0; i<3; i++)
  {
    tcamPointer = i;
/*     memset((char *)&tcamData, 0x55+i, sizeof(GT_TCAM_DATA)); */
    dev->is96Frame = Is96Frame;
    MSG_PRINT(("gtcamPurgyEntry entry: %d \n", (int)tcamPointer));

    if((status = gtcamPurgyEntry(dev, tcamPointer, &tcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamPurgyEntry returned "));
        testDisplayStatus(status);
        return status;
    }

    MSG_PRINT(("gtcamReadTCAMData tcam entry: %d \n", (int)tcamPointer));
    if((status = gtcamReadTCAMData(dev, tcamPointer, &tcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamReadTCAMData returned \n"));
        testDisplayStatus(status);
        return status;
    }
    displayTcamData(dev, &tcamData, 1);
  }
#endif

#if 1
    tcamPointer = 0;
    MSG_PRINT(("TCAM API Get next test start entry %d \n", (int)tcamPointer));

    if((status = gtcamGetNextTCAMData(dev, &tcamPointer, &tcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamGetNextTCAMData returned "));
        testDisplayStatus(status);
        return status;
    }

    MSG_PRINT(("TCAM API Get next test next entry %d \n", (int)tcamPointer));
    displayTcamData(dev, &tcamData, 3);
   
    tcamPointer = 4;
    MSG_PRINT(("TCAM API Get next test start entry %d \n", (int)tcamPointer));

    if((status = gtcamGetNextTCAMData(dev, &tcamPointer, &tcamData)) != GT_OK)
    {
        MSG_PRINT(("gtcamGetNextTCAMData returned "));
        testDisplayStatus(status);
        return status;
    }

    MSG_PRINT(("TCAM API Get next test next entry %d \n", (int)tcamPointer));
    displayTcamData(dev, &tcamData, 1);
   
#endif
    MSG_PRINT(("Tcam API test done "));
    testDisplayStatus(status);

    return status;
}

