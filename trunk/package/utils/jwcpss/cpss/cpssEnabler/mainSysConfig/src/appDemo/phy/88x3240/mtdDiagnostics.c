#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
higher-level functions using MDIO access to enable test modes,
loopbacks, and other diagnostic functions of the Marvell 88X3240 
ethernet PHY.
********************************************************************/
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdApiRegs.h"
#include "mtdDiagnostics.h"
#include "mtdAPIInternal.h"

MTD_STATUS mtdGetPhyRevision
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U8 *phyRev,
    OUT MTD_U8 *numPorts,
    OUT MTD_U8 *thisPort
)
{
    MTD_U16 temp, tryCounter, temp2;
    MTD_BOOL tunitReady;
    MTD_U16 revision,numports,thisport, readyBit;
    MTD_BOOL inDownloadMode, inLowPowerMode, registerExists, regReady;

    *phyRev = MTD_REV_UNK;
    *numPorts = 0;
    *thisPort = 0;

    /* check if T-unit firmware is up and running, and if so, easier to determine revision */
    ATTEMPT(mtdIsTunitResponsive(contextPtr,port,&tunitReady));

    /* check if T-unit is in waiting on download mode or low power mode if not ready */
    if (tunitReady == MTD_FALSE)
    {
        ATTEMPT(mtdIsPhyInMdioDownloadMode(contextPtr,port,&inDownloadMode));
        ATTEMPT(mtdIsTunitInLowPowerMode(contextPtr,port,&inLowPowerMode))
    }

    if (tunitReady == MTD_TRUE || inLowPowerMode == MTD_TRUE)
    {
        /* read revision from 1.C04E.3:0 */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_TUNIT_PHY_REV_INFO_REG,&temp))
        ATTEMPT(mtdHwGetRegFieldFromWord(temp,0,4,&revision));

        /* if it read 0, then it's an early version of firmware and chip revision must be QZ2 */
        if (revision == 0)
        {
            /* early version of the firmware which only ran on QZ2, in which case use internal version */
            ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_TUNIT_IEEE_PMA_DEVID2,0,4,&revision));

            /* has to be QZ2 */
            if (revision == MTD_REV_QZ2)
            {
                *numPorts = 4; /* QZ2 is a quad chip */
                *phyRev = MTD_REV_QZ2;
                return MTD_OK;
            }
            else
            {
                /* this is an error, should never happen*/
                return MTD_FAIL;
            }
        }
        else
        {
            /* it's post-A0 firmware (after 1.0.17), and revision and other info is in 1.C04E */

            /* read other info from 1.C04E */
            ATTEMPT(mtdHwGetRegFieldFromWord(temp,4,3,&numports));
            *numPorts = (MTD_U8)numports;
            ATTEMPT(mtdHwGetRegFieldFromWord(temp,7,3,&thisport));
            *thisPort = (MTD_U8)thisport;
            *phyRev = (MTD_U8)revision;
            return MTD_OK;
        }
    }

    /* firmware is not running and has never run if we got here, better be in download mode */
    if (inDownloadMode == MTD_TRUE)
    {
        /* try and figure out chip revision assuming firmware never loaded/ran */
        ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_TUNIT_IEEE_PMA_DEVID2,0,4,&revision));

        /* NOTE: this logic is invalid for DZ1 chips */
        if (revision >= MTD_REV_DZ2)
        {
            *numPorts = 2; /* DZ2, DA0 are a dual-port chips */
            *phyRev = (MTD_U8)revision;
            return MTD_OK;
        }

        /* could be QZ2 or QA0 */
        if (revision >= MTD_REV_QZ2 && revision < MTD_REV_DZ2)
        {
            *numPorts = 4; /* QZ2, QA0 are both quad chips */
            *phyRev = (MTD_U8)revision;
        }

        /* now test the chip to see if it's really QZ2 or actually it's QA0 */
        /* internal rev id is the same for these two chips */
        if (revision == MTD_REV_QZ2)
        {
            /* remove internal reset */
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,3,0xD801,5,1,1));

            /* wait until it's ready */
            regReady = MTD_FALSE;
            tryCounter = 0;
            while(regReady == MTD_FALSE && tryCounter++ < 10)
            {
                ATTEMPT(mtdWait(contextPtr,1)); /* timeout is set to 10 ms */
                ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,3,0xD007,6,1,&readyBit));
                if (readyBit == 1)
                {
                    regReady = MTD_TRUE;
                }
            }

            if (regReady == MTD_FALSE)
            {
                /* timed out, can't tell for sure what rev this is */
                *numPorts = 0;
                *phyRev = MTD_REV_UNK;
                return MTD_FAIL;
            }

            /* perform test */
            registerExists = MTD_FALSE;
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,3,0x8EC6,&temp));
            ATTEMPT(mtdHwXmdioWrite(contextPtr,port,3,0x8EC6,0xA5A5));
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,3,0x8EC6,&temp2));

            /* put back internal reset */
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,3,0xD801,5,1,0));

            if (temp == 0 && temp2 == 0xA5A5)
            {
                registerExists = MTD_TRUE;
            }


            /* overwrite choice if test passed */
#if 0 /* logic which would be needed to differentiate between DZ1 and DZ2 */           
            if (registerExists == MTD_TRUE && *phyRev == MTD_REV_DZ1)
            {
                *phyRev = MTD_REV_DA0; /* this is actually DZ2 */
                return MTD_OK;
            }
#endif            

            if (registerExists == MTD_TRUE && *phyRev == MTD_REV_QZ2)
            {
                *phyRev = MTD_REV_QA0; /* this is actually QA0 */
                return MTD_OK;
            }
        }

        return MTD_OK; /* it was one of the first two if got here */
    }

    /* got here, this is a failure */
    return MTD_FAIL;
}

MTD_STATUS mtdGetTunitBISTStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *bistStatus
)
{
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_TUNIT_BIST_STATUS_REG,bistStatus));    

    return MTD_OK;
}

MTD_STATUS mtdGetTunitFirmwareMode
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *tunitMode,
    OUT MTD_U16 *tunitState
)
{
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_TUNIT_XG_EXT_STATUS,8,4,tunitMode));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_TUNIT_XG_EXT_STATUS,0,8,tunitState));
    
    return MTD_OK;
}

MTD_STATUS mtdSetPMATestModes
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 testMode, 
    IN MTD_U16 testFrequency
)
{
    MTD_U16 freqTemp;
    
    if (testMode == MTD_PMA_TESTMODE_4)
    {
        freqTemp = ((testFrequency>>10)&7); /* get bits 12:10 */

        if (freqTemp == 0 || freqTemp == 3 || freqTemp == 7)
        {
            return MTD_FAIL; /* an invalid frequency was selected */
        }
        else
        {
            /* one of the good frequencies was selected */
            testMode |= (freqTemp << 10); /* with bits 12:10 */
            ATTEMPT(mtdHwXmdioWrite(contextPtr,port,1,132,testMode));
        }
    }
    else if ((/*testMode >= MTD_PMA_TESTMODE_OFF && always true, because testMode unsigned */
              testMode <= MTD_PMA_TESTMODE_3) ||
             (testMode >= MTD_PMA_TESTMODE_5 &&
              testMode <= MTD_PMA_TESTMODE_7))
    {
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,1,132,testMode));
    }
    else
    {
        return MTD_FAIL;
    }

    return MTD_OK;
}


MTD_STATUS mtdGetPMATestMode
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *testMode,
    OUT MTD_U16 *testFrequency
)
{
    MTD_U16 valRead;
    
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,132,&valRead));
    *testMode = valRead & 0xE000;
    *testFrequency = valRead & 0x1C00;

    return MTD_OK;
}


MTD_STATUS mtdGet10GBTSNROperatingMargin
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_S16 snr_4chan[]
)
{
    MTD_U16 readVal;

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,133,&readVal));
    snr_4chan[0] = readVal ^ 0x8000; /* Channel  A, converted to 2's complement */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,134,&readVal));
    snr_4chan[1] = readVal ^ 0x8000; /* Channel  B, converted to 2's complement */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,135,&readVal));
    snr_4chan[2] = readVal ^ 0x8000; /* Channel  C, converted to 2's complement */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,136,&readVal));
    snr_4chan[3] = readVal ^ 0x8000; /* Channel  D, converted to 2's complement */
    
    return MTD_OK;
}


MTD_STATUS mtdGet10GBTSNRMinimumMargin
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_S16 snr_4chan[]
)
{
    MTD_U16 readVal;

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,137,&readVal));
    snr_4chan[0] = readVal ^ 0x8000; /* Channel  A, converted to 2's complement */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,138,&readVal));
    snr_4chan[1] = readVal ^ 0x8000; /* Channel  B, converted to 2's complement */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,139,&readVal));
    snr_4chan[2] = readVal ^ 0x8000; /* Channel  C, converted to 2's complement */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,140,&readVal));
    snr_4chan[3] = readVal ^ 0x8000; /* Channel  D, converted to 2's complement */
    
    return MTD_OK;
}



/********************************Cable Diagnostics ****************************/

MTD_STATUS mtdRun_CableDiagnostics
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port, 
    IN MTD_BOOL breakLink,
    IN MTD_BOOL disableInterPairShort
)
{
    MTD_U16 regVal;
    
    /* check if test is already running */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC00D,&regVal));

    if ((regVal & (1<<15)) && (regVal & (1<<11)))
    {
        return MTD_FAIL; /* test is running */
    }

    /* test isn't running, write the request */
    regVal = (1<<15);
    if (disableInterPairShort)
    {
        regVal |= (1<<13);
    }
    if (breakLink)
    {
        regVal |= (1<<12);
    }

    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,1,0xC00D,regVal));    
    
    return MTD_OK;
}

MTD_STATUS mtdGet_CableDiagnostics_Status
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *testDone
)
{
    MTD_U16 regVal;
    
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC00D,&regVal));

    if (regVal & (1<<11))
    {
        *testDone = MTD_FALSE;
    }
    else
    {
        *testDone = MTD_TRUE;
    }

    return MTD_OK;
}

MTD_STATUS mtdGet_CableDiagnostics_Results
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port, 
    MTD_U8 terminationCode[],
    MTD_U8 length[]
)
{
    MTD_U16 regVal,i,mask,regVal2;
    
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC00D,&regVal));    
    
    if (regVal & (1<<11))
    {
        return MTD_FAIL; /* run, but not finished */
    }

    /* finished, so get results */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC00E,&regVal));

    for(i=0,mask=0x000F; i<4; i++)
    {
        terminationCode[i] = (MTD_U8)((regVal & mask)>>(4*i));
        mask <<= 4;
    }

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC00F,&regVal));
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC010,&regVal2));
    for(i=0,mask=0x00FF; i<2; i++)
    {
        length[i] = (MTD_U8)((regVal & mask)>>(8*i));
        length[i+2] = (MTD_U8)((regVal2 & mask)>>(8*i)); 
        mask <<= 8;
    }

    /* return PHY to normal operating mode */
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,1,0xC00D,0));        
    
    return MTD_OK;
}


/********************* Echo Tests ***********************************/
MTD_STATUS mtdStart10GEchoTest
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port
)
{
    MTD_U16 regVal1, regVal2;
    
    /* check if a test was already requested and is still in progress */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC00B,&regVal1));
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC00C,&regVal2));

    if ((regVal1 & (1<<2)) &&
        (!(regVal2 & (1<<14))))
    {
        return MTD_FAIL; /* test requested and in progress still, can't request another */
    }

    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,1,0xC00B,2,1,1));    
    
    return MTD_OK;
}

MTD_STATUS mtdIs10GEchoTestDone
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_BOOL *testDone
)
{
    MTD_U16 regVal;
    
    /* check if test was even requested, and if not return MTD_FAIL */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,0xC00B,&regVal));

    if (!(regVal & (1<<2)))
    {
        return MTD_FAIL;
    }

    ATTEMPT(mtdGetTunitBISTStatus(contextPtr,port,&regVal));
    
    if (regVal & MTD_BIST_ECHO_TEST_DONE)
    {
        *testDone = MTD_TRUE;
    }
    else
    {
        *testDone = MTD_FALSE;
    }

    return MTD_OK;    
}

MTD_STATUS mtdGet10GEchoTestResults
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 results[]
)
{
    MTD_BOOL done;
    MTD_U16 i;
    
    /* make sure it was requested and is done */
    ATTEMPT(mtdIs10GEchoTestDone(contextPtr,port,&done));

    if (!done)
    {
        return MTD_FAIL;
    }

    /* read the results */
    for (i=0xC013; i < 0xC027; i++)
    {
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,1,i,&results[i-0xC013]));
    }

    /* take it out of the echo test mode */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,1,0xC00B,2,1,0));

    return MTD_OK;
}

MTD_STATUS mtdGetAmbientNoise
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 noiseResults[]
)
{
    MTD_BOOL testDone = MTD_FALSE;
    MTD_U16 results[20],i;
    
    /* force off transmitter */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,1,9,0,1,1));

    ATTEMPT(mtdStart10GEchoTest(contextPtr,port));

    while(testDone == MTD_FALSE)
    {
        ATTEMPT(mtdIs10GEchoTestDone(contextPtr,port,&testDone));
        ATTEMPT(mtdWait(contextPtr,500)); /* better not to poll constantly */
    }

    /* read results and return PHY to normal operation */
    ATTEMPT(mtdGet10GEchoTestResults(contextPtr,port,results));
    for(i=0; i < 4; i++)
    {
        noiseResults[i] = results[i+16];
    }    
    
    /* allow transmitter to be turned back on */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,1,9,0,1,0));
    
    return MTD_OK;
}

/***** BER COUNTER *****/
MTD_STATUS mtdReadBERCount
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *count
)
{
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,3,33,8,6,count));

    return MTD_OK;
}

MTD_STATUS mtdIsLPInfoValid
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_BOOL *validFlag
)
{
    MTD_U16 regVal;
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,1,129,0,1,&regVal));

    if (regVal)
    {
        *validFlag = MTD_TRUE;
    }
    else
    {
        *validFlag = MTD_FALSE;
    }
    
    return MTD_OK;
}

/***** PBO - NEAR END and FAR END *****/
MTD_STATUS mtdReadNearEndPBO
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *pboValue
)
{
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,1,131,10,3,pboValue));
    
    return MTD_OK;
}

MTD_STATUS mtdReadFarEndPBO
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *pboValue
)
{
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,1,131,13,3,pboValue));
    
    return MTD_OK;
}



MTD_STATUS mtdGetSerdesEyeStatistics
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 curSelLane,
    OUT MTD_U32 eyeDataUp[][MTD_EYEDIAGRAM_NCOLS],
    OUT MTD_U32 eyeDataDn[][MTD_EYEDIAGRAM_NCOLS],
    OUT MTD_U16 *eyeArea,
    OUT MTD_U16 *eyeWidth,
    OUT MTD_U16 *eyeHeight
)
{
    MTD_U16 sampleNumPow2 = 20; /* sampleNum = 1<<sampleNumPow2 */
    MTD_U16 DFEdata = 0; /*reg for calculate h0*/
    MTD_U16 reg_8xBE = 0; /*register value for center phase location*/

    MTD_U16 /* regComm_00 = 0, */regLane_00 = 0;
    MTD_U16 tmp, tmpAddr;
    MTD_U16 spdSet, cnt;
    MTD_U16 eyeXmax, eyeXmin, eyeZmax, eyeZmin;
    MTD_U16 i, j;

#if 0 /* uncomment this if we need to plot eye diagram */
    MTD_U16 n, idx=0;
    MTD_U32 raw_topHalf_2eye[(MTD_EYEDIAGRAM_NROWS + 1) / 2][MTD_EYEDIAGRAM_NCOLS] = {0};
    MTD_U32 raw_bottomHalf_2eye[(MTD_EYEDIAGRAM_NROWS + 1) / 2][MTD_EYEDIAGRAM_NCOLS] = {0};
    MTD_U32 measEye[MTD_EYEDIAGRAM_NQ] = {0}, pMyXData[MTD_EYEDIAGRAM_NQ] = {0}, pMyYData[MTD_EYEDIAGRAM_NQ] = {0}, pMyZData[MTD_EYEDIAGRAM_NQ] = {0} ;
#endif

    tmpAddr = 0x8000 + curSelLane*MTD_XFI_LANEOFFSET;

    /*track register 0x80BE, best VEO location*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0xBE, &tmp));
    reg_8xBE = tmp & 0x7f;

    /*calculate h0*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x84,&tmp));
    DFEdata = ((tmp & 0x7F00) >> 8); /* //[14:8] */

    /*0. read reg $V.$CL00.1:0($CL=90)and $V.$PL00.3:0($PL=80)*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,0x9000,&tmp));
    /* regComm_00 = tmp & 0x3; */ /* //get last 2 bits */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x00,&tmp));
    regLane_00 = tmp & 0xF; /* //get last 4 bits */
    if (regLane_00 > 10) /* //0xA */
    {
        /* //out << "\nOP_Freq[3:0] is > 0xA; not in the table range, measurement aborted."; */
        return MTD_FAIL;
    }

    /*set reg$PL15.11:4
    //eye monitor speed  setting 0: $V.$PL15.11:4 = 0x0;
    //eye monitor speed  setting 1: $V.$PL15.11:4 = 0x1;
    //eye monitor speed  setting 2: $V.$PL15.11:4 = 0x16;
    //eye monitor speed  setting 3: $V.$PL15.11:4 = 0x27;*/
    spdSet = 0; /*&spdSet = Comm_Lane_freq[regComm_00, regLane_00];*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x15,&tmp));
    /*speed set*/
    if (spdSet == 0)
    {
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x15,(tmp & 0xF00F)));
    }
    else if (spdSet == 1)
    {
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x15,(tmp & 0xF00F)));
    }
    else if (spdSet == 2)
    {
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x15,((tmp & 0xF00F) | 0x150)));
    }
    else if (spdSet == 3)
    {
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x15,((tmp & 0xF00F) | 0x260)));
    }

    /*!!! need set phase to zero*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,(tmp & 0xFFC0)));


    /*1. enable eye monitor
    //addr: phy, dev, reg
    //$V.$PL10.15 = 0x1*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,(tmp | 0x8000)));
    mtdWait(contextPtr,1000);
    cnt = 0;
    while (1)
    {
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,&tmp));
        if ((tmp & 0x8000) == 0x8000)
            break;
        mtdWait(contextPtr,1000);
        cnt++;
        if (cnt > 4)
        {
            /*out << "\nEM enable failed. Measurement aborted.";
            //out << "\nPlease check if the selected lane is up.\n";*/
            return MTD_FAIL;;
        }
    }
    /*2. write sample count for calibration
    //Valid setting is 10 to 29
    //5'd10 = 1024 matched samples for each pattern
    //...
    //5'd20 = 1 million matched samples for each pattern
    //...
    //$V.$PL10.13:9 = 0x14 (20 in decimal) - 5bit used for the # of samples*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,((tmp & 0xC1FF) | 0x2800)));
    /*3. write eye amplitude $V.$PL11.10:6 = 0x19*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,((tmp & 0xF83F) | 0x0640)));
    /*X3240 no calibration for now*/



    /*-------------------- eye monitor measure -----------------------
    //1. set compare sample number*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,((tmp & 0xC1FF) | (sampleNumPow2 << 9))));

    /*2. first write top half $V.$PL11.11 = 0x1*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,(tmp & 0x8000)));
  /*   //top half --- loop on amplitude & phase, and measure the eye
    //Measure top half of the first eye and bottom half of the 2nd eye
    //assume first eye is even, second is odd */
    ATTEMPT(mtdDoMeasHalfEye(contextPtr, port, curSelLane, reg_8xBE, eyeDataUp));

    /*3. write bottom half $V.$PL11.11 = 0x0*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,(tmp & 0xF7FF)));
    /*bottom half
    //Measure bottom half of the first eye and top half of the 2nd eye*/
    ATTEMPT(mtdDoMeasHalfEye(contextPtr, port, curSelLane, reg_8xBE, eyeDataDn));

    /*4. write back eye amplitude $V.$PL11.10:6 = 0x19*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,((tmp & 0xF83F) | 0x640)));

    /*5. disable eye monitor $V.$PL10.15 = 0x0*/
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,&tmp));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x10,(tmp & 0x7FFF)));

    /*6. post processing of eye data to get eye statistics*/
    (*eyeArea) = 0;
    eyeXmin = MTD_EYEDIAGRAM_NCOLS/2; /* for eye width*/
    eyeXmax = 0;
    eyeZmin = MTD_EYEDIAGRAM_NROWS; /* for eye height */
    eyeZmax = 0;
    for (i = 0; i < (MTD_EYEDIAGRAM_NROWS + 1) / 2; i++) /* through all amplitudes 0-25*/
    {
        for (j = 0; j < 64; j++)  /* through 64 phases (first eye) */
        {
            if (eyeDataUp[i][j] == 0)
            {
                if (j < eyeXmin)
                {
                    eyeXmin = j;
                }
                else if (j > eyeXmax)
                {
                    eyeXmax = j;
                }
                if (i < eyeZmin)
                {
                    eyeZmin = i;
                }
                else if (i > eyeZmax)
                {
                    eyeZmax = i;
                }
                (*eyeArea)++;
            }
            if ((eyeDataDn[i][j] == 0) && (i < (MTD_EYEDIAGRAM_NROWS-1)/2))
            {
                if (j < eyeXmin)
                {
                    eyeXmin = j;
                }
                else if (j > eyeXmax)
                {
                    eyeXmax = j;
                }
                tmp = (MTD_EYEDIAGRAM_NROWS-1)-i;
                if (tmp < eyeZmin)
                {
                    eyeZmin = tmp;
                }
                else if (tmp > eyeZmax)
                {
                    eyeZmax = tmp;
                }
                (*eyeArea)++;
            }

        }
        for (j = 0; j < 64; j++) /*  // through 64 phases (second eye) */
        {
            if (eyeDataDn[i][j+64] == 0)
            {
                (*eyeArea)++;
            }
            if ((eyeDataUp[i][j+64] == 0) && (i < (MTD_EYEDIAGRAM_NROWS-1)/2))
            {
                (*eyeArea)++;
            }
        }
    }
    *eyeWidth = eyeXmax - eyeXmin;
    *eyeHeight = ((eyeZmax - eyeZmin - ((MTD_EYEDIAGRAM_NROWS-1)/2))*(DFEdata*250/64-249))/32;

#if 0 /*uncomment this part to get x/y/z values for eye diagram plot
    // prepare for eye diagram plot*/
    for (i = 0; i < (MTD_EYEDIAGRAM_NROWS + 1) / 2; i++) /* through all amplitudes 0-25*/
    {
        for (j = 0; j < 64; j++)  /* // through 64 phases (first eye) */
        {
            raw_topHalf_2eye[i][j] = eyeDataUp[i][j];
            raw_bottomHalf_2eye[i][j] = eyeDataDn[i][j];
        }
        for (j = 0; j < 64; j++)  /* // through 64 phases (second eye) */
        {
            raw_topHalf_2eye[i][j + 64] = eyeDataDn[i][j + 64];
            raw_bottomHalf_2eye[i][j + 64] = eyeDataUp[i][j + 64];
        }
    }
    for (i = 0; i < (MTD_EYEDIAGRAM_NROWS + 1) / 2; i++)
    {
        for (j = 0; j < MTD_EYEDIAGRAM_NCOLS; j++)
        {
            measEye[idx] = raw_topHalf_2eye[i][j];
            idx++;
        }
    }
    for (i = 1; i < (MTD_EYEDIAGRAM_NROWS + 1) / 2; i++)
    {
        for (j = 0; j < MTD_EYEDIAGRAM_NCOLS; j++)
        {
            measEye[idx] = raw_bottomHalf_2eye[(MTD_EYEDIAGRAM_NROWS-1)/2-i][j];
            idx++;
        }
    }
    (*eyeArea) = 0;
    eyeXmin = MTD_EYEDIAGRAM_NCOLS/2;
    eyeXmax = 0;
    eyeZmin = MTD_EYEDIAGRAM_NROWS;
    eyeZmax = 0;
    for (i = 0; i < MTD_EYEDIAGRAM_NROWS; i++)
    {
        for (j = 0; j < MTD_EYEDIAGRAM_NCOLS; j++)
        {
            n = (i * MTD_EYEDIAGRAM_NCOLS) + j;
            pMyXData[n] = j;
            pMyZData[n] = i;
            pMyYData[n] = measEye[n];
            if (measEye[n] == 0)
            {
                if (j < MTD_EYEDIAGRAM_NCOLS/2) /* // for eyeWidth */
                {
                    if (j < eyeXmin)
                    {
                        eyeXmin = j;
                    }
                    else if (j > eyeXmax)
                    {
                        eyeXmax = j;
                    }
                }
                if (i < eyeZmin)
                {
                    eyeZmin = i;
                }
                else if (i > eyeZmax)
                {
                    eyeZmax = i;
                }
                (*eyeArea)++;
            }
        }
    }
    *eyeWidth = eyeXmax - eyeXmin;
    *eyeHeight = ((eyeZmax - eyeZmin - ((MTD_EYEDIAGRAM_NROWS-1)/2))*(DFEdata*250/64-249))/32;
#endif

    return MTD_OK;
}
