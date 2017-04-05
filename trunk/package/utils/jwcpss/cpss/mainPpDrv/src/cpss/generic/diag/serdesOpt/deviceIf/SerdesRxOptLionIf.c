/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* SerdesRxOptLionIf.h
*
* DESCRIPTION:
*       Serdes LionB device interface implemenation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/
#include <serdesOpt/SerdesRxOptimizer.h>
#include <serdesOpt/private/serdesOptPrivate.h>
#include <serdesOpt/private/SerdesRxOptAppIf.h>

#define CAL_CFG_REG_OFFSET      (0x28)
#define TRANSMITTER_REG1_OFFSET (0x54)
#define SQL_CFG_REG_OFFSET      (0x78)
#define FFE_CFG_REG_OFFSET      (0x8c)
#define DFE_CFG_REG_OFFSET      (0x98)
#define RESET_CFG_REG_OFFSET    (0x16C)
#define PHY_TEST_REG0_OFFSET    (0x178)
#define PHY_TEST_DATA5_OFFSET   (0x194)
#define PRBS_ERROR_MSB_OFFSET   (0x1a4)
#define PRBS_COUNTER_OFFSET     (0x1a0)

typedef struct
{
    GT_U32 dfeRegValue;
    GT_U32 ffeRegValue;
    GT_U32 samplerRegValue;
    GT_U32 sqlchRegValue;

}MV_LANE_REPOS;

static MV_LANE_REPOS serdesTestLaneCfg[4][12];

/* Basic address of Serdes Internal Register File (LP Serdes) */
static GT_U32 getAddressOfLane(GT_U32 laneNum)
{
    return (0x09800200 + laneNum * 0x400);
}

static GT_U32 getPrbsCode(MV_PRBS_TYPE prbsType)
{
    int prbsCode;

    switch (prbsType)
    {
    case PRBS_7:
        prbsCode = 0x80; /* prbs 7 */
        break;
    case PRBS_15:
        prbsCode = 0x82; /* prbs 15 */
        break;
    case PRBS_23:
        prbsCode = 0x83; /* prbs 23 */
        break;
    case PRBS_31:
        prbsCode = 0x84; /* prbs 31 */
        break;
    default :
        prbsCode = 0x80; /* prbs 7 */
        break;
    }

    return prbsCode;
}

/* configure Transmitter Emphasis & Amplitude parameters */
static void transmitterCfg(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum)
{
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    genRegisterSet(devNum, portGroup, baseAddr + TRANSMITTER_REG1_OFFSET, 0x7F2D, 0);
}

/* generate PRBS per lane */
static void prbsConfig(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, MV_PRBS_TYPE prbsCode)
{
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    /* generate prbs */
    genRegisterSet(devNum, portGroup,baseAddr + PHY_TEST_REG0_OFFSET, 0, 0);
    genRegisterSet(devNum, portGroup,baseAddr + PHY_TEST_DATA5_OFFSET,getPrbsCode(prbsCode), 0);
    genRegisterSet(devNum, portGroup,baseAddr + PHY_TEST_REG0_OFFSET, 0x800E, 0);
    return;
}

static GT_U32 getErrors(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum)
{
    GT_U32 tmp, errCnt1, errCnt2;
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    /* check prbs lock, if 0 , do not read counters  */
    genRegisterGet(devNum, portGroup,baseAddr + PHY_TEST_REG0_OFFSET+4, &tmp, 0);
    
    if (((tmp >> 14) & 1) == 0)
    {
        return 0xffffffff;
    }
    /* performing two readings to approve real 0 error */
    genRegisterGet(devNum, portGroup, baseAddr + PRBS_ERROR_MSB_OFFSET+4, &tmp, 0); 
    genRegisterGet(devNum, portGroup, baseAddr + PRBS_ERROR_MSB_OFFSET, &errCnt1, 0);
    errCnt1 = tmp + 0x1000 * errCnt1; 
    
    genRegisterGet(devNum, portGroup, baseAddr + PRBS_ERROR_MSB_OFFSET+4, &tmp, 0);
    genRegisterGet(devNum, portGroup, baseAddr + PRBS_ERROR_MSB_OFFSET, &errCnt2, 0);          
    errCnt2 = tmp + 0x1000 * errCnt2; 

    return(errCnt1 + errCnt2);      
}

/* Approving that the PRBS is running on the tested lane */
static GT_U32 approvLaneConnect(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, 
                               GT_U32 dfe, GT_U32 ffe, GT_U32 prbsCode)
{
    GT_U32 prbsCount;
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    ffe = ffe;

    /* Setting initial DFE value for this specific test */
    genRegisterSet(devNum, portGroup,baseAddr + DFE_CFG_REG_OFFSET, dfe, 0);

    /* Setting initial FFE value for this specific test */
    genRegisterSet(devNum, portGroup,baseAddr + FFE_CFG_REG_OFFSET, 317, 0);

    /* generate prbs */
    prbsConfig(devNum, portGroup, laneNum, prbsCode);

    /* read counter */
    genRegisterGet(devNum,  portGroup, baseAddr + PRBS_COUNTER_OFFSET, &prbsCount, 0);          
    return prbsCount;
}

static void setDfeValue(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, GT_U32 dfe)
{
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    /* Setting initial DFE value for this specific test */
    genRegisterSet(devNum, portGroup,baseAddr + DFE_CFG_REG_OFFSET, dfe, 0);
}

static void setFfeValue(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, GT_U32 ffeR, GT_U32 ffeC)
{
    GT_U32 ffeVal;
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    /* Setting initial DFE value for this specific test */
    /* FFE = I*100 + R*10 + C */
    ffeVal = 0x300 + ffeR*0x10 + ffeC;        
    genRegisterSet(devNum, portGroup,baseAddr + FFE_CFG_REG_OFFSET, ffeVal, 0);
}

static void saplerValSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, GT_U32 data)
{
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    data = 0x40 + (data << 8) + (1 << 15);  /* bit 15 - manual position */
    genRegisterSet(devNum, portGroup,baseAddr + CAL_CFG_REG_OFFSET, data, 0);
    /* reset bit 6 only */
    genRegisterSet(devNum, portGroup,baseAddr + RESET_CFG_REG_OFFSET, 0x40, 0x40);
    genRegisterSet(devNum, portGroup,baseAddr + RESET_CFG_REG_OFFSET, 0x0, 0x40);
}

static void samplerCfg(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, GT_U32 data)
{
    GT_U32 curVal;
    unsigned int tmp1, tmp2;
    unsigned i;

    GT_U32 baseAddr = getAddressOfLane(laneNum);

    /* get current sampler value */
    genRegisterGet(devNum, portGroup,baseAddr + CAL_CFG_REG_OFFSET, &curVal, 0);
    
    curVal = (curVal >> 8) & 0x7f;
    if (curVal == data)
    {
        return;
    }

    /* if delta between data and current sampler value > 5 (serdes definition) */
    /* change sampler value with step = 5 */
    if ( curVal > data)
    {
        tmp1 = curVal - data;
        tmp2 = curVal;
        for (i = 0; i < (tmp1 / 5); i++)
        {
            tmp2 -= 5; 
            saplerValSet(devNum, portGroup, laneNum, tmp2);

        }
    }
    else
    {
        tmp1 = data - curVal;
        tmp2 = curVal;
        for (i = 0; i < (tmp1 / 5); i++)
        {
            tmp2 += 5; 
            saplerValSet(devNum, portGroup, laneNum, tmp2);
            
        }

    }
    /* set data */
    saplerValSet(devNum, portGroup, laneNum, data);

}

static void saveRestoreLaneCfg(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, int mode)
{
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    /* 0 - to save data, 1 - to restore prev. saved data */
    if (mode == 0)
    {
        /* Save current Sampler/FFE/DFE/Sqlch values  */
        /* TBD: list of registers to store / restore */
        genRegisterGet(devNum,  portGroup, baseAddr + DFE_CFG_REG_OFFSET, &serdesTestLaneCfg[devNum][laneNum].dfeRegValue, 0);
        genRegisterGet(devNum,  portGroup, baseAddr + FFE_CFG_REG_OFFSET, &serdesTestLaneCfg[devNum][laneNum].ffeRegValue, 0);
        genRegisterGet(devNum,  portGroup, baseAddr + CAL_CFG_REG_OFFSET, &serdesTestLaneCfg[devNum][laneNum].samplerRegValue, 0);
        genRegisterGet(devNum,  portGroup, baseAddr + SQL_CFG_REG_OFFSET, &serdesTestLaneCfg[devNum][laneNum].sqlchRegValue, 0);

    }
    else
    {
        /* restore*/
        genRegisterSet(devNum, portGroup,baseAddr + DFE_CFG_REG_OFFSET, serdesTestLaneCfg[devNum][laneNum].dfeRegValue,0);
        genRegisterSet(devNum, portGroup,baseAddr + FFE_CFG_REG_OFFSET, serdesTestLaneCfg[devNum][laneNum].ffeRegValue,0);
        genRegisterSet(devNum, portGroup,baseAddr + CAL_CFG_REG_OFFSET, serdesTestLaneCfg[devNum][laneNum].samplerRegValue,0);
        genRegisterSet(devNum, portGroup,baseAddr + SQL_CFG_REG_OFFSET, serdesTestLaneCfg[devNum][laneNum].sqlchRegValue,0);
    }

    return;
}

/* configure Transmitter Emphasis & Amplitude parameters */
static void setSqlchValue(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, GT_U32 data)
{
    GT_U32 baseAddr = getAddressOfLane(laneNum);

    genRegisterSet(devNum, portGroup,baseAddr + SQL_CFG_REG_OFFSET, data, 0xF);
}

static void checkSignalDetect(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum, GT_U32 *signal)
{
    GT_U32 baseAddr = getAddressOfLane(laneNum);
    GT_U32 data;
    
    if (signal != 0)
    {
        genRegisterGet(devNum, portGroup, baseAddr + SQL_CFG_REG_OFFSET, &data, 0);
        *signal = (data >> 11) & 1;
    }
}

static void prbsStopConfig(GT_U8 devNum, GT_U32 portGroup, GT_U32 laneNum)
{
    GT_U32 baseAddr = getAddressOfLane(laneNum);
    
    /* stop prbs */
    genRegisterSet(devNum, portGroup,baseAddr + PHY_TEST_REG0_OFFSET, 0xE, 0);
}



void  lionSerdesFuncRegistration(MV_ACCESS_FUNCS *accIf)
{
    accIf->prbsConfigFunc = prbsConfig;
    accIf->getErrorsFunc = getErrors;
    accIf->transmitterCfgFunc =  transmitterCfg;
    accIf->approvLaneConnectFunc = approvLaneConnect;
    accIf->setDfeValueFunc = setDfeValue;
    accIf->setFfeValueFunc = setFfeValue;
    accIf->samplerCfgFunc = samplerCfg;
    accIf->saveRestoreCfgFunc = saveRestoreLaneCfg;
    accIf->setSqlchValueFunc = setSqlchValue;
    accIf->checkSignalDetectFunc = checkSignalDetect;
    accIf->prbsStopCfgFunc = prbsStopConfig;
}

