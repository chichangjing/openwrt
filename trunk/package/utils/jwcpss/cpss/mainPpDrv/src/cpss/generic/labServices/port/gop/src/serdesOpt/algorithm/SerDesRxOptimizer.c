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
* SerdesRxOptimizer.c
*
* DESCRIPTION:
*       Serdes optimizer API implementation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
******************************************************************************/


#include <common/siliconIf/mvSiliconIf.h>
#include <serdesOpt/private/gtSkipList.h>
#include <serdesOpt/SerdesRxOptimizer.h>
#include <serdesOpt/private/serdesOptPrivate.h>
#include <serdesOpt/SerdesRxOptDeviceIf.h>

MV_SOPT_OS_TIME_WK_AFTER_FUNC hwsSOptOsTimerWkFuncPtr = NULL;
MV_SOPT_OS_MEM_SET_FUNC       hwsSOptOsMemSetFuncPtr = NULL;
MV_SOPT_OS_FREE_FUNC          hwsSOptOsFreeFuncPtr = NULL;
MV_SOPT_OS_MALLOC_FUNC        hwsSOptOsMallocFuncPtr = NULL;
MV_SOPT_OS_MEM_CPY_FUNC       hwsSOptOsMemCpyFuncPtr = NULL;
MV_SOPT_OS_IO_PRINTF_FUNC     hwsSOptOsPrintfFuncPtr = NULL;
MV_SOPT_OS_IO_SPRINTF_FUNC    hwsSOptOsSprintfFuncPtr = NULL;
MV_SOPT_OS_SIG_SEM_BIN_CREATE_FUNC  hwsSOptOsSemBCreateFuncPtr = NULL;
MV_SOPT_OS_SIG_SEM_DELETE_FUNC  hwsSOptOsSemDelFuncPtr = NULL;
MV_SOPT_OS_SIG_SEM_WAIT_FUNC  hwsSOptOsSemWaitFuncPtr = NULL;
MV_SOPT_OS_SIG_SEM_SIGNAL_FUNC  hwsSOptOsSemSignalFuncPtr = NULL;
MV_SOPT_OS_RAND_FUNC            hwsSOptOsRandFuncPtr = NULL;

static MV_SERDES_OPT_MODE           SerdesTestMode = AccurAware;
static MV_SERDES_OPT_REPORT_TYPE    SerdesTestReportType = Silence;
static GT_U32 prbsTestTimeout = 100;

/* keep all info needed for serdes RX optimization */
static STRUCT_SL_HEAD *rxLanes;
static GT_U32 rxLanesNum;

/* keep info for PRBS running only */
static STRUCT_SL_HEAD *txLanes;
static GT_U32 txLanesNum;

/* algorithm input data */
static FFE_CFG      ffeConfig;
static DFE_CFG      dfeConfig;
static GT_U32       samplerStart;
static GT_U32       sqlch;

static GT_U32 maxLoopNum = 2;
static GT_32 samplerWindFailCritarea = 5;
static GT_U32 windFailRCritarea = 1;
static GT_U32 windFailCCritarea = 2;
static GT_U32 smoothFactorDfe = 2;
static GT_U32 smoothFactorSampler = 2;
static GT_U32 samplerCenterDistance = 30;
static GT_U32 volatgeBerRiskCriteria = 1;

static void runMode1(MV_LANE_DATA *curLane);
static void runMode2(MV_LANE_DATA *curLane);

/*******************************************************************************
* genCompareNodes
*
* DESCRIPTION:
*       Compares two id generator structs data, and returns one of the following
*       results: GT_SL_EQUAL / GT_SL_SMALLER / GT_SL_GREATER.
*
* INPUTS:
*       data1   - A pointer to the first struct to be compared.
*       data2   - A pointer to the second struct to be compared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_SL_EQUAL / GT_SL_SMALLER / GT_SL_GREATER.
*
* COMMENTS:
*       For full declaration of the compare function see:
*       common/gtSkipList.h
*
*******************************************************************************/
static GT_SL_COMP_RES genCompareNodes
(
    IN MV_LANE_DATA *data1,
    IN MV_LANE_DATA *data2
)
{
    /* Check the null cases     */
    if((data1 == NULL) && (data2 == NULL))
        return GT_SL_EQUAL;
    if((data1 == NULL) && (data2 != NULL))
        return GT_SL_SMALLER;
    if((data1 != NULL) && (data2 == NULL))
        return GT_SL_GREATER;

    /* Both data's are not NULL */
    if ((data1->laneNum == data2->laneNum) &&
        (data1->devNum == data2->devNum) &&
        (data1->groupNum == data2 ->groupNum))
        return GT_SL_EQUAL;

    if (data1->laneNum < data2->laneNum)
        return GT_SL_SMALLER;

    return GT_SL_GREATER;
}

/*******************************************************************************
* mvSerdesTunningSystemInit
*
* DESCRIPTION:
*       Init environment variables for tunning algorithm.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningSystemInit
(
    MV_SERDES_OPT_OS_FUNC_PTR *funcPtr
)
{
    if(NULL == funcPtr)
    {
        return 1;
    }

    if((NULL == funcPtr->osTimerWkPtr)
       || (NULL == funcPtr->osMemSetPtr)
       || (NULL == funcPtr->osFreePtr)
       || (NULL == funcPtr->osMallocPtr)
       || (NULL == funcPtr->osPrintfPtr)
       || (NULL == funcPtr->osMemCpyPtr)
       || (NULL == funcPtr->osSprintfPtr)
       || (NULL == funcPtr->osSemBCreatePtr)
       || (NULL == funcPtr->osSemDelPtr)
       || (NULL == funcPtr->osSemWaitPtr)
       || (NULL == funcPtr->osSemSignalPtr))
    {
        return 1;
    }

    hwsSOptOsTimerWkFuncPtr = funcPtr->osTimerWkPtr;
    hwsSOptOsMemSetFuncPtr  = funcPtr->osMemSetPtr;
    hwsSOptOsFreeFuncPtr    = funcPtr->osFreePtr;
    hwsSOptOsMallocFuncPtr  = funcPtr->osMallocPtr;
    hwsSOptOsPrintfFuncPtr  = funcPtr->osPrintfPtr;
    hwsSOptOsMemCpyFuncPtr  = funcPtr->osMemCpyPtr;
    hwsSOptOsSprintfFuncPtr = funcPtr->osSprintfPtr;
    hwsSOptOsSemBCreateFuncPtr = funcPtr->osSemBCreatePtr;
    hwsSOptOsSemDelFuncPtr = funcPtr->osSemDelPtr;
    hwsSOptOsSemWaitFuncPtr = funcPtr->osSemWaitPtr;
    hwsSOptOsSemSignalFuncPtr = funcPtr->osSemSignalPtr;
    hwsSOptOsRandFuncPtr = funcPtr->osRandPtr;

    rxLanes = skipInit((GT_INTFUNCPTR)genCompareNodes);
    txLanes = skipInit((GT_INTFUNCPTR)genCompareNodes);

    if ((rxLanes == 0) || (txLanes == 0))
    {
        return 1;
    }
    rxLanesNum = 0;
    txLanesNum = 0;

    /* Set initial Sampler, FFE, DFE, sampler: 0x5A, Sqlch=0) */
    ffeConfig.ffeCMaxValue = FFE_C_MAX;
    ffeConfig.ffeCMinValue = FFE_C_MIN;
    ffeConfig.ffeRMaxValue = FFE_R_MAX;
    ffeConfig.ffeRMinValue = FFE_R_MIN;

    dfeConfig.dfeMaxValue = DFE_MAX;
    dfeConfig.dfeMinValue = DFE_MIN;

    samplerStart = SAMPLER_START;
    sqlch = SQLCH_VAL;

    return 0;
}

/*******************************************************************************
* mvSerdesTunningSystemClose
*
* DESCRIPTION:
*       Free all resources allocated by tunning algorithm.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
int mvSerdesTunningSystemClose
(
    void
)
{
    GT_UINTPTR iter;
    MV_LANE_DATA *curLane;

    /* stop transmit prbs on all tested lanes and set defaults */
    iter = 0;
    while((curLane = skipGetNext(txLanes, &iter)) != NULL)
    {
        curLane->accIf.prbsStopCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum);
    }
    iter = 0;
    while((curLane = skipGetNext(rxLanes, &iter)) != NULL)
    {
        curLane->accIf.prbsStopCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum);
    }

    if (skipDestroy(rxLanes, hwsSOptOsFreeFuncPtr) != 0)
        return 1;

    if (skipDestroy(txLanes, hwsSOptOsFreeFuncPtr) != 0)
        return 1;

    rxLanesNum = 0;
    txLanesNum = 0;

    return 0;
}

/*******************************************************************************
* mvSerdesTunningRxLaneRegister
*
* DESCRIPTION:
*       Configure all RX lane related information for tunning
*       (include access functions pointers).
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*       prbsType  - PRBS type (7, 15, 23, 31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningRxLaneRegister
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 laneNum,
    MV_PRBS_TYPE prbsType
)
{
    MV_LANE_DATA *curLane;
    STRUCT_SL_NODE *slNode;

    curLane = (MV_LANE_DATA *)hwsSOptOsMallocFuncPtr(sizeof(MV_LANE_DATA));

    if (curLane == 0)
    {
        return 1;
    }
    hwsSOptOsMemSetFuncPtr(curLane, 0, sizeof(MV_LANE_DATA));
    curLane->devNum = devNum;
    curLane->laneNum = laneNum;
    curLane->groupNum = portGroup;
    curLane->prbsCode = prbsType;

    if (mvSerdesLaneRegistration(devNum, &(curLane->accIf)) != 0)
        return 1;

    if (skipAddAndGetNode(rxLanes, curLane, &slNode) != 0)
        return 1;

    /* Save current Sampler/FFE/DFE/Sqlch values */
    curLane->accIf.saveRestoreCfgFunc(devNum, portGroup,laneNum, 0);

    rxLanesNum++;
    return 0;
}

/*******************************************************************************
* mvSerdesTunningRxLaneUnregister
*
* DESCRIPTION:
*       Remove RX lane related information from tunning algorithm
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningRxLaneUnregister
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 laneNum
)
{
    MV_LANE_DATA lane, *curLane;

    lane.devNum = devNum;
    lane.laneNum = laneNum;
    lane.groupNum = portGroup;

    curLane = skipDelete(rxLanes, &lane);
    if (curLane == 0)
        return 1;

    hwsSOptOsFreeFuncPtr(curLane);
    rxLanesNum--;

    return 0;
}

/*******************************************************************************
* mvSerdesTunningTxLaneRegister
*
* DESCRIPTION:
*       Configure all TX lane related information to run PRBS
*       (include functions pointers).
*
* INPUTS:
*       devNum    - Device Number
*       laneNum   - Lane Number.
*       prbsType  - PRBS type (7, 15, 23, 31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningTxLaneRegister
(
    GT_U8 devNum,
    GT_U32 portGroup,
    GT_U32 laneNum,
    MV_PRBS_TYPE prbsType
)
{
    MV_LANE_DATA *curLane;
    STRUCT_SL_NODE *slNode;

    curLane = (MV_LANE_DATA *)hwsSOptOsMallocFuncPtr(sizeof(MV_LANE_DATA));

    if (curLane == 0)
    {
        return 1;
    }
    hwsSOptOsMemSetFuncPtr(curLane, 0, sizeof(MV_LANE_DATA));
    curLane->devNum = devNum;
    curLane->laneNum = laneNum;
    curLane->groupNum = portGroup;
    curLane->prbsCode = prbsType;

    if (mvSerdesLaneRegistration(devNum, &(curLane->accIf)) != 0)
        return 1;

    if (skipAddAndGetNode(txLanes, curLane, &slNode) != 0)
        return 1;

    /* Save current Sampler/FFE/DFE/Sqlch values */
    curLane->accIf.saveRestoreCfgFunc(devNum, portGroup, laneNum, 0);

    txLanesNum++;
    return 0;
}

/*******************************************************************************
* mvSerdesTunningTxLaneUnregister
*
* DESCRIPTION:
*       Remove TX lane related information from tunning algorithm
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningTxLaneUnregister
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 laneNum
)
{
    MV_LANE_DATA lane, *curLane;

    lane.devNum = devNum;
    lane.laneNum = laneNum;
    lane.groupNum = portGroup;

    curLane = skipDelete(txLanes, &lane);
    if (curLane == 0)
        return 1;

    hwsSOptOsFreeFuncPtr(curLane);
    txLanesNum--;

    return 0;
}


/*******************************************************************************
* mvSerdesTunningStartTx
*
* DESCRIPTION:
*       Run PRBS on TX lanes configured.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void mvSerdesTunningStartTx()
{
    GT_UINTPTR iter;
    MV_LANE_DATA *slNode;

    iter = 0;
    while((slNode = skipGetNext(txLanes, &iter)) != NULL)
    {
        /* generate PRBS on TX lane */
        slNode->accIf.prbsConfigFunc(slNode->devNum, slNode->groupNum, slNode->laneNum, slNode->prbsCode);
    }
}

/*******************************************************************************
* mvSerdesTunningStopTx
*
* DESCRIPTION:
*       Stop PRBS transmit on TX lanes registered.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void mvSerdesTunningStopTx()
{

    GT_UINTPTR iter;
    MV_LANE_DATA *slNode;

    iter = 0;
    while((slNode = skipGetNext(txLanes, &iter)) != NULL)
    {
        /* stop generate PRBS on TX lane */
        slNode->accIf.prbsStopCfgFunc(slNode->devNum, slNode->groupNum, slNode->laneNum);
    }
}


/*******************************************************************************
* mvSerdesTunningStart
*
* DESCRIPTION:
*       Run serdes parameters tunning algorithm (on all RX lanes).
*
* INPUTS:
*       prbsTime - duration of the PRBS test during suggested setup verification [ms].
*       testMode - run different algorithm variations
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - tunning success on all RX lanes
*       1  - tunning failed
*
*******************************************************************************/
int mvSerdesTunningStart
(
    MV_SERDES_OPT_MODE testMode,
    GT_U32  prbsTime
)
{
    MV_LANE_DATA *curLane;
    GT_UINTPTR iter;

    if (skipIsEmpty(rxLanes))
    {
        return 1;
    }
    iter = 0;
    SerdesTestMode = testMode;
    prbsTestTimeout = prbsTime;

    switch (testMode)
    {
    case AccurTimeAware:
        break;
    case AccurAware:
        while((curLane = skipGetNext(rxLanes, &iter)) != NULL)
        {
            runMode1(curLane);
        }
        break;
    case AccurAwareShort:
        while((curLane = skipGetNext(rxLanes, &iter)) != NULL)
        {
            runMode2(curLane);
        }
        break;
    case TimeAware:
        break;
    }
    return 0;
}

/*******************************************************************************
* mvSerdesTunningReport
*
* DESCRIPTION:
*       Return serdes optimal parameters.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Test result parameters.
*
*******************************************************************************/
MV_SERDES_OPT_RESULTS mvSerdesTunningReport
(
    GT_U8 devNum,
    GT_U32 portGroup,
    GT_U32 laneNum
)
{
    MV_SERDES_OPT_RESULTS report;
    MV_LANE_DATA lane, *curLane;

    hwsSOptOsMemSetFuncPtr(&report, 0, sizeof(report));

    lane.devNum = devNum;
    lane.groupNum = portGroup;
    lane.laneNum = laneNum;

    curLane = skipGetFirstData(rxLanes, &lane);
    if (curLane != 0)
    {
        report = curLane->result;
    }

    return report;
}

/*******************************************************************************
* mvSerdesTunningReportTypeSet
*
* DESCRIPTION:
*       Set report type.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Test result parameters.
*
*******************************************************************************/
void mvSerdesTunningReportTypeSet
(
    MV_SERDES_OPT_REPORT_TYPE  type
)
{
    SerdesTestReportType = type;
}


/* local functions */
static void arrWindow (GT_U32 *arr,GT_U32 arrSize, GT_U32 *minInd, GT_U32 *maxInd)
{
    GT_U32 i, maxValInd;

    maxValInd = 0;

    for(i = 0 ; i < arrSize; i++)
    {
        if(arr[i] > arr[maxValInd])
        {
            maxValInd = i;
            *minInd = maxValInd;
        }
        else if(arr[i] == arr[maxValInd])
        {
            *maxInd = i;
        }
    }
    if (*minInd > *maxInd)
    {
        *maxInd = *minInd;
    }
}

static int samplerTapScan(MV_LANE_DATA *curLane)
{
    int samplerWindow;
    GT_U32 posSamplerCenter, negSamplerCenter;
    GT_U32 calCfgData;
    GT_U32 posR, negR;
    GT_U32 counter;

    samplerWindow = 0;
    posSamplerCenter = samplerStart;
    negSamplerCenter = samplerStart;

    /* look for positive side of sampler window = positive range */
    for (posR = 0; posR < (32 - 8); posR++)
    {
        calCfgData = samplerStart + posR;
        curLane->accIf.samplerCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, calCfgData);
        curLane->accIf.prbsConfigFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, curLane->prbsCode);
        hwsSOptOsTimerWkFuncPtr(prbsTestTimeout);
        counter = curLane->accIf.getErrorsFunc(curLane->devNum,  curLane->groupNum,curLane->laneNum);
        if(counter <= 1)
        {
            samplerWindow++;
            curLane->result.prbsNotLocked = GT_FALSE;
        }
        else
        {
            /* always 1 enough */
            posSamplerCenter = samplerStart + posR;
            break;
        }
    }

    /* look for negative side of sampler window = negative range */
    for (negR = 0; negR < (32-8); negR++)
    {
        calCfgData = samplerStart - negR;
        curLane->accIf.samplerCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, calCfgData);
        curLane->accIf.prbsConfigFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, curLane->prbsCode);
        hwsSOptOsTimerWkFuncPtr(prbsTestTimeout);
        counter = curLane->accIf.getErrorsFunc(curLane->devNum,  curLane->groupNum, curLane->laneNum);
        if(counter <= 1)
        {
            samplerWindow++;
            curLane->result.prbsNotLocked = GT_FALSE;
        }
        else
        {
            /* always 1 enough */
            negSamplerCenter = samplerStart - negR;
            break;
        }
    }

    curLane->accIf.samplerCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, samplerStart);

    curLane->result.samplerCenter = (posSamplerCenter + negSamplerCenter) / 2;

    if (samplerWindow > 0)
    {
        curLane->result.samplerWindow = samplerWindow - 1;
    }
    else
    {
        curLane->result.samplerWindow = 0;
    }

    return 0;
}

static GT_32 ffeScanCurrent(MV_LANE_DATA *curLane, GT_U32 resCur, GT_U32 capCur)
{
    curLane->accIf.setFfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, resCur, capCur);
    curLane->accIf.prbsConfigFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, curLane->prbsCode);
    hwsSOptOsTimerWkFuncPtr(prbsTestTimeout);
    return curLane->accIf.getErrorsFunc(curLane->devNum,  curLane->groupNum, curLane->laneNum);
}

static void printCurArray(GT_U32 *arrPtr, GT_U32 arrStart, GT_U32 arrEnd)
{
    GT_U32 i;

    for (i = arrStart; i <= arrEnd; i++)
    {
        hwsSOptOsPrintfFuncPtr("\n\t%d\t%d", i, arrPtr[i]);
    }
}

static void runMode2(MV_LANE_DATA *curLane)
{
    GT_U32 capCur, bestC;
    GT_U32 dfeList[DFE_MAX+1];
    GT_U32 dfeBestCList[DFE_MAX+1][FFE_C_MAX+1];
    GT_U32 dfeSamplerList[DFE_MAX+1];
    GT_U32 dfeSamplerListSmoot[DFE_MAX+1];
    GT_U32 vector[DFE_MAX+1];
    GT_U32 dfeCur;
    GT_U32 ffeRCur, bestR;
    GT_U32 counter;
    GT_U32 signal;
    GT_U32 minIndx, maxIndx, tmp, indx;
    GT_U32 minTmp, maxTmp;
    GT_U32 k;
    GT_BOOL shortReach = GT_FALSE;
    GT_U32 dfeLoopNum;
    GT_U32 maxCVal;



    curLane->result.prbsNotLocked = GT_TRUE;
    /* we are successful oriented, for any problem set tunningDone to 0 */
    curLane->tunningDone = 1;
    /*Set initial Sampler, FFE, DFE, PE (FFE 0x33f, DFE: 0x15 (common suite value), sampler: 0x5A, Sqlch=0) */
    curLane->accIf.setDfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, 0x15);
    curLane->accIf.setFfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, 3, 0xf);
    curLane->accIf.samplerCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, SAMPLER_START);
    curLane->accIf.setSqlchValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, 0);
    /* read register 0x78 RecReg0, bit 11 must be 0, if get 1 set default and return */
    curLane->accIf.checkSignalDetectFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, &signal);

    if (signal == 1)
    {
        curLane->accIf.saveRestoreCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, 1);
        curLane->result.signalDetectFail = GT_TRUE;
        curLane->tunningDone = 0;
        return;
    }
        hwsSOptOsMemSetFuncPtr(vector, 0, sizeof(vector));
        hwsSOptOsMemSetFuncPtr(dfeList, 0, sizeof(dfeList));
        hwsSOptOsMemSetFuncPtr(dfeSamplerList, 0, sizeof(dfeList));
        hwsSOptOsMemSetFuncPtr(dfeBestCList, 0, sizeof(GT_U32) * (DFE_MAX+1) * (FFE_C_MAX+1) );

        /* Phase1: Fitted attenuation and ILD initial optimization */
        /* Set C =0xF (static at this phase) */
        capCur = 0xf;

        for (dfeCur = dfeConfig.dfeMinValue; dfeCur <= dfeConfig.dfeMaxValue; dfeCur++)
        {
            /* set DFE to HW */
            curLane->accIf.setDfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, dfeCur);
            for (ffeRCur = ffeConfig.ffeRMinValue; ffeRCur <= ffeConfig.ffeRMaxValue; ffeRCur++)
            {
                /* set FFE to HW : use capCur and ffeRCur */
                counter = ffeScanCurrent(curLane, ffeRCur, capCur);
                if(counter == 0)
                {
                    vector[ffeRCur]++;
                    dfeList[dfeCur]++;
                    curLane->result.prbsNotLocked = GT_FALSE;
                }
            }
        }
        /* Find the R with largest number of valid DFE taps */
        bestR = 0;tmp = 0;
        arrWindow(vector, ffeConfig.ffeRMaxValue+1, &bestR, &tmp);
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\nDevice %d, Lane %d", curLane->devNum, curLane->laneNum);
            hwsSOptOsPrintfFuncPtr("\nPhase 1::FFE R vector");
            printCurArray(vector, ffeConfig.ffeRMinValue, ffeConfig.ffeRMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 1::DFE vector");
            printCurArray(dfeList, dfeConfig.dfeMinValue, dfeConfig.dfeMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 1::Best FFE R is %d.", bestR);
        }
        if (bestR == 0)
        {
            curLane->tunningDone = 0;
            return;
        }
        if (vector[FFE_R_MAX] == DFE_MAX)
        {
            /* short reach */
            shortReach = GT_TRUE;
            if (SerdesTestReportType == Verbose)
                hwsSOptOsPrintfFuncPtr("\nPhase 1:: set short reach");
        }
        /* smooth best R */
        minIndx = bestR;
        maxIndx = tmp;
        minTmp = 0;
        for (k = 0; (bestR) && (k < smoothFactorDfe); k++)
        {
            for (indx = minIndx; indx <= maxIndx; indx++)
            {
                if (vector[indx] == 0)
                {
                    continue;
                }
                vector[indx] -= 1;
            }
            arrWindow(vector, ffeConfig.ffeRMaxValue+1, &minTmp, &maxTmp);
            if (minIndx > minTmp)
            {
                minIndx = minTmp;
            }
            if (maxIndx < maxTmp)
            {
                maxIndx = maxTmp;
            }
        }
        if (minIndx != maxIndx)
        {
            minIndx += (maxIndx - minIndx)/2;
        }

        bestR = minIndx;
        /* If R_window < Min_R_Window ?  Warning (Fitted attenuation) */
        if (windFailRCritarea > bestR)
        {
            curLane->result.ffeRFittedAttenuation = GT_TRUE;
        }
        curLane->result.ffeRBestVal = bestR;
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\nPhase 1::Best FFE R after smoothing is %d.", bestR);
        }

        /* Phase2: Fitted attenuation and ILD initial optimization */
        hwsSOptOsMemSetFuncPtr(vector, 0, sizeof(vector));
        for (dfeCur = dfeConfig.dfeMinValue; dfeCur <= dfeConfig.dfeMaxValue; dfeCur++)
        {
            if (dfeList[dfeCur] == 0)
            {
                /* no valid R value for this DFE value was found in phase 1 */
                /* mark this dfe in dfe list [0-31] like not valid */
                continue;
            }
            dfeList[dfeCur] = 0;
            /* set DFE to HW */
            curLane->accIf.setDfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, dfeCur);

            for (capCur = ffeConfig.ffeCMinValue; capCur <= ffeConfig.ffeCMaxValue; capCur++)
            {
                /* set FFE to HW : use capCur and bestR */
                counter = ffeScanCurrent(curLane, curLane->result.ffeRBestVal, capCur);
                if(counter == 0)
                {
                    vector[capCur]++;
                    dfeList[dfeCur]++;
                    dfeBestCList[dfeCur][capCur]++;
                    curLane->result.prbsNotLocked = GT_FALSE;
                }
            }
        }
        /* Find C with largest number of valid DFE taps (prefer highest C); */
        bestC = 0; tmp = 0;
        arrWindow(vector, ffeConfig.ffeCMaxValue+1, &tmp, &bestC);

        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\n\nPhase 2::FFE C vector");
            printCurArray(vector, ffeConfig.ffeCMinValue, ffeConfig.ffeCMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 2::DFE vector");
            printCurArray(dfeList, dfeConfig.dfeMinValue, dfeConfig.dfeMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 2::Best FFE C is %d.", bestC);
            hwsSOptOsPrintfFuncPtr("\nPhase 2::Max FFE C is %d.", vector[bestC]);
        }
        /* smooth best C */
        minIndx = tmp;
        maxIndx = bestC;
        maxCVal = vector[bestC];

        for (k = 0; (bestC) && (k < smoothFactorDfe); k++)
        {
            for (indx = minIndx; indx <= maxIndx; indx++)
            {
                if (vector[indx] == 0)
                {
                    continue;
                }
                vector[indx] -= 1;
            }
            arrWindow(vector, ffeConfig.ffeCMaxValue+1, &minTmp, &maxTmp);
            if (minIndx > minTmp)
            {
                minIndx = minTmp;
            }
            if (maxIndx < maxTmp)
            {
                maxIndx = maxTmp;
            }
        }
        bestC = maxIndx;
        if (shortReach == GT_TRUE)
        {
            if (SerdesTestReportType == Verbose)
            {
                hwsSOptOsPrintfFuncPtr("\n\nSmooth best C: minIndx is %d, maxIndx is %d", minIndx, maxIndx);
            }
            if (minIndx != maxIndx)
            {
                minIndx += (maxIndx - minIndx)/2;
            }
            bestC = minIndx;
        }
        /* If C_window < Min_C_Window ?  Warning (Fitted attenuation) */
        if (bestC < windFailCCritarea)
        {
            curLane->result.ffeCFittedAttenuation = GT_TRUE;
        }
        curLane->result.ffeCBestVal = bestC;
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\nPhase 2::Best FFE C after smoothing is %d.", bestC);
        }

        /* List Best DFE values (that have max and max-1 C windows as long as >1) with largest C range */
        /* Phase 3: ILD optimization: */
        /* Set FFE R,C according to results from Phase #1 & 2; use bestC and bestR */
        hwsSOptOsMemSetFuncPtr(vector, 0, sizeof(vector));
        curLane->accIf.setFfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, curLane->result.ffeRBestVal, curLane->result.ffeCBestVal);

        /* find DFE max value */
        minIndx = 0; maxIndx = 0;
        arrWindow(dfeList, dfeConfig.dfeMaxValue+1, &minIndx, &maxIndx);
        /* DFE wind = maxI - minI +1 < FailCtr - report warning: Voltage BER risk */
        if ((maxIndx - minIndx + 1) < volatgeBerRiskCriteria)
        {
            curLane->result.volatgeBerRisk = GT_TRUE;
        }
        /* look for middle DFE in the dfeSamplerList */
        /* find DFE max value */
        minIndx = 0; maxIndx = 0;
        arrWindow(dfeList, dfeConfig.dfeMaxValue+1, &minIndx, &maxIndx);
        tmp = maxCVal / 5;
        if (tmp < 2)
        {
            dfeLoopNum = 2;
        }
        else
        {
            dfeLoopNum = tmp;
        }
        /* smoothing best DFE */
        hwsSOptOsMemCpyFuncPtr(dfeSamplerListSmoot, dfeList, sizeof(dfeList));
        for (k = 0; k < dfeLoopNum; k++)
        {
            for (indx = minIndx; indx <= maxIndx; indx++)
            {
                if (dfeSamplerListSmoot[indx] == 0)
                {
                    continue;
                }
                dfeSamplerListSmoot[indx] -= 1;
            }
            arrWindow(dfeSamplerListSmoot, dfeConfig.dfeMaxValue+1, &minTmp, &maxTmp);
            if (minIndx > minTmp)
            {
                minIndx = minTmp;
            }
            if (maxIndx < maxTmp)
            {
                maxIndx = maxTmp;
            }
        }
        if (minIndx != maxIndx)
        {
            minIndx += (maxIndx - minIndx)/2;
        }

        /* set best DFE */
        curLane->result.dfeBestVal = minIndx;
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\n\nResult::DFE is %d", curLane->result.dfeBestVal);
            hwsSOptOsPrintfFuncPtr("\nResult::FFE R is %d", curLane->result.ffeRBestVal);
            hwsSOptOsPrintfFuncPtr("\nResult::FFE C is %d", curLane->result.ffeCBestVal);
        }

        /*If sampler window < 5 taps ? report warning: BER risk */
        if (curLane->result.samplerWindow < samplerWindFailCritarea)
        {
            /* report warning: Time BER risk */
            curLane->result.timeBerRisk = GT_TRUE;
        }
}

static void runMode1(MV_LANE_DATA *curLane)
{
    GT_U32 capCur, bestC;
    GT_U32 dfeList[DFE_MAX+1];
    GT_U32 dfeBestCList[DFE_MAX+1][FFE_C_MAX+1];
    GT_U32 dfeSamplerList[DFE_MAX+1];
    GT_U32 dfeSamplerListSmoot[DFE_MAX+1];
    GT_U32 vector[DFE_MAX+1];
    GT_U32 dfeCur;
    GT_U32 ffeRCur, bestR;
    GT_U32 counter;
    GT_U32 delta;
    GT_U32 signal;
    GT_U32 minIndx, maxIndx, tmp, indx;
    GT_U32 minTmp, maxTmp;
    int wideWind;
    GT_U32 iterNum, k;
    GT_BOOL shortReach = GT_FALSE;
    GT_U32 dfeLoopNum;


    curLane->result.prbsNotLocked = GT_TRUE;
    /* we are successful oriented, for any problem set tunningDone to 0 */
    curLane->tunningDone = 1;
    /*Set initial Sampler, FFE, DFE, PE (FFE 0x33f, DFE: 0x15 (common suite value), sampler: 0x5A, Sqlch=0) */
    curLane->accIf.setDfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, 0x15);
    curLane->accIf.setFfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, 3, 0xf);
    curLane->accIf.samplerCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, SAMPLER_START);
    curLane->accIf.setSqlchValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, 0);
    /* read register 0x78 RecReg0, bit 11 must be 0, if get 1 set default and return */
    curLane->accIf.checkSignalDetectFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, &signal);

    if (signal == 1)
    {
        curLane->accIf.saveRestoreCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, 1);
        curLane->result.signalDetectFail = GT_TRUE;
        curLane->tunningDone = 0;
        return;
    }

    iterNum = 0;
    do
    {
        hwsSOptOsMemSetFuncPtr(vector, 0, sizeof(vector));
        hwsSOptOsMemSetFuncPtr(dfeList, 0, sizeof(dfeList));
        hwsSOptOsMemSetFuncPtr(dfeSamplerList, 0, sizeof(dfeList));
        hwsSOptOsMemSetFuncPtr(dfeBestCList, 0, sizeof(GT_U32) * (DFE_MAX+1) * (FFE_C_MAX+1) );

        /* Phase1: Fitted attenuation and ILD initial optimization */
        /* Set C =0xF (static at this phase) */
        capCur = 0xf;

        for (dfeCur = dfeConfig.dfeMinValue; dfeCur <= dfeConfig.dfeMaxValue; dfeCur++)
        {
            /* set DFE to HW */
            curLane->accIf.setDfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, dfeCur);
            for (ffeRCur = ffeConfig.ffeRMinValue; ffeRCur <= ffeConfig.ffeRMaxValue; ffeRCur++)
            {
                /* set FFE to HW : use capCur and ffeRCur */
                counter = ffeScanCurrent(curLane, ffeRCur, capCur);
                if(counter == 0)
                {
                    vector[ffeRCur]++;
                    dfeList[dfeCur]++;
                    curLane->result.prbsNotLocked = GT_FALSE;
                }
            }
        }
        /* Find the R with largest number of valid DFE taps */
        bestR = 0;tmp = 0;
        arrWindow(vector, ffeConfig.ffeRMaxValue+1, &bestR, &tmp);
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\nDevice %d, Lane %d", curLane->devNum, curLane->laneNum);
            hwsSOptOsPrintfFuncPtr("\nPhase 1::FFE R vector");
            printCurArray(vector, ffeConfig.ffeRMinValue, ffeConfig.ffeRMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 1::DFE vector");
            printCurArray(dfeList, dfeConfig.dfeMinValue, dfeConfig.dfeMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 1::Best FFE R is %d.", bestR);
        }
        if (bestR == 0)
        {
            curLane->tunningDone = 0;
            return;
        }
        if (vector[FFE_R_MAX] == DFE_MAX)
        {
            /* short reach */
            shortReach = GT_TRUE;
            if (SerdesTestReportType == Verbose)
                hwsSOptOsPrintfFuncPtr("\nPhase 1:: set short reach");
        }
        /* smooth best R */
        minIndx = bestR;
        maxIndx = tmp;
        for (k = 0; (bestR) && (k < smoothFactorDfe); k++)
        {
            for (indx = minIndx; indx <= maxIndx; indx++)
            {
                if (vector[indx] == 0)
                {
                    continue;
                }
                vector[indx] -= 1;
            }
            arrWindow(vector, ffeConfig.ffeRMaxValue+1, &minTmp, &maxTmp);
            if (minIndx > minTmp)
            {
                minIndx = minTmp;
            }
            if (maxIndx < maxTmp)
            {
                maxIndx = maxTmp;
            }
        }
        if (minIndx != maxIndx)
        {
            minIndx += (maxIndx - minIndx)/2;
        }

        bestR = minIndx;
        /* If R_window < Min_R_Window ?  Warning (Fitted attenuation) */
        if (windFailRCritarea > bestR)
        {
            curLane->result.ffeRFittedAttenuation = GT_TRUE;
        }
        curLane->result.ffeRBestVal = bestR;
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\nPhase 1::Best FFE R after smoothing is %d.", bestR);
        }

        /* Phase2: Fitted attenuation and ILD initial optimization */
        hwsSOptOsMemSetFuncPtr(vector, 0, sizeof(vector));
        for (dfeCur = dfeConfig.dfeMinValue; dfeCur <= dfeConfig.dfeMaxValue; dfeCur++)
        {
            if (dfeList[dfeCur] == 0)
            {
                /* no valid R value for this DFE value was found in phase 1 */
                /* mark this dfe in dfe list [0-31] like not valid */
                continue;
            }
            dfeList[dfeCur] = 0;
            /* set DFE to HW */
            curLane->accIf.setDfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, dfeCur);

            for (capCur = ffeConfig.ffeCMinValue; capCur <= ffeConfig.ffeCMaxValue; capCur++)
            {
                /* set FFE to HW : use capCur and bestR */
                counter = ffeScanCurrent(curLane, curLane->result.ffeRBestVal, capCur);
                if(counter == 0)
                {
                    vector[capCur]++;
                    dfeList[dfeCur]++;
                    dfeBestCList[dfeCur][capCur]++;
                    curLane->result.prbsNotLocked = GT_FALSE;
                }
            }
        }
        /* Find C with largest number of valid DFE taps (prefer highest C); */
        bestC = 0; tmp = 0;
        arrWindow(vector, ffeConfig.ffeCMaxValue+1, &tmp, &bestC);
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\n\nPhase 2::FFE C vector");
            printCurArray(vector, ffeConfig.ffeCMinValue, ffeConfig.ffeCMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 2::DFE vector");
            printCurArray(dfeList, dfeConfig.dfeMinValue, dfeConfig.dfeMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 2::Best FFE C is %d.", bestC);
        }
        /* smooth best C */
        minIndx = tmp;
        maxIndx = bestC;
        for (k = 0; (bestC) && (k < smoothFactorDfe); k++)
        {
            for (indx = minIndx; indx <= maxIndx; indx++)
            {
                if (vector[indx] == 0)
                {
                    continue;
                }
                vector[indx] -= 1;
            }
            arrWindow(vector, ffeConfig.ffeCMaxValue+1, &minTmp, &maxTmp);
            if (minIndx > minTmp)
            {
                minIndx = minTmp;
            }
            if (maxIndx < maxTmp)
            {
                maxIndx = maxTmp;
            }
        }
        bestC = maxIndx;
        if (shortReach == GT_TRUE)
        {
            if (SerdesTestReportType == Verbose)
            {
                hwsSOptOsPrintfFuncPtr("\n\nSmooth best C: minIndx is %d, maxIndx is %d", minIndx, maxIndx);
            }
            if (minIndx != maxIndx)
            {
                minIndx += (maxIndx - minIndx)/2;
            }
            bestC = minIndx;
        }
        /* If C_window < Min_C_Window ?  Warning (Fitted attenuation) */
        if (bestC < windFailCCritarea)
        {
            curLane->result.ffeCFittedAttenuation = GT_TRUE;
        }
        curLane->result.ffeCBestVal = bestC;
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\nPhase 2::Best FFE C after smoothing is %d.", bestC);
        }

        /* List Best DFE values (that have max and max-1 C windows as long as >1) with largest C range */
        /* Phase 3: ILD/sampler optimization: */
        /* Set FFE R,C according to results from Phase #1 & 2; use bestC and bestR */
        hwsSOptOsMemSetFuncPtr(vector, 0, sizeof(vector));
        curLane->accIf.setFfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, curLane->result.ffeRBestVal, curLane->result.ffeCBestVal);

        /* find DFE max value */
        minIndx = 0; maxIndx = 0;
        arrWindow(dfeList, dfeConfig.dfeMaxValue+1, &minIndx, &maxIndx);
        /* DFE wind = maxI - minI +1 < FailCtr - report warning: Voltage BER risk */
        if ((maxIndx - minIndx + 1) < volatgeBerRiskCriteria)
        {
            curLane->result.volatgeBerRisk = GT_TRUE;
        }

        /* Run Sampler scan for the list of max DFE values from Phase 2 */
        wideWind= 0;
        for (dfeCur = minIndx; dfeCur <= maxIndx; dfeCur++)
        {
            /* set DFE to HW */
            if ((dfeList[dfeCur] == 0) || (dfeBestCList[dfeCur][curLane->result.ffeCBestVal] == 0))
                continue;

            curLane->accIf.setDfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, dfeCur);
            samplerTapScan(curLane);
            /* Select the DFE that has widest Sampler  window and centralize the sampler. */
            dfeSamplerList[dfeCur] = curLane->result.samplerWindow;
            vector[dfeCur] = curLane->result.samplerCenter;
            if (wideWind <= curLane->result.samplerWindow)
            {
                wideWind = curLane->result.samplerWindow;
                vector[dfeCur] = curLane->result.samplerCenter;
                dfeSamplerList[dfeCur] = wideWind;
            }
        }
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\n\nPhase 3::DFE sampler vector");
            printCurArray(dfeSamplerList, dfeConfig.dfeMinValue, dfeConfig.dfeMaxValue);
            hwsSOptOsPrintfFuncPtr("\nPhase 3::Sampler center vector");
            printCurArray(vector, dfeConfig.dfeMinValue, dfeConfig.dfeMaxValue);
        }

        /* look for middle DFE in the dfeSamplerList */
        /* find DFE max value */
        minIndx = 0; maxIndx = 0;
        arrWindow(dfeSamplerList, dfeConfig.dfeMaxValue+1, &minIndx, &maxIndx);
        tmp = dfeSamplerList[maxIndx] / 5;
        if (tmp < 2)
        {
            dfeLoopNum = smoothFactorSampler;
        }
        else
        {
            dfeLoopNum = tmp;
        }
        /* smoothing best DFE */
        hwsSOptOsMemCpyFuncPtr(dfeSamplerListSmoot, dfeSamplerList, sizeof(dfeSamplerList));
        for (k = 0; k < dfeLoopNum; k++)
        {
            for (indx = minIndx; indx <= maxIndx; indx++)
            {
                if (dfeSamplerListSmoot[indx] == 0)
                {
                    continue;
                }
                dfeSamplerListSmoot[indx] -= 1;
            }
            arrWindow(dfeSamplerListSmoot, dfeConfig.dfeMaxValue+1, &minTmp, &maxTmp);
            if (minIndx > minTmp)
            {
                minIndx = minTmp;
            }
            if (maxIndx < maxTmp)
            {
                maxIndx = maxTmp;
            }
        }
        if (minIndx != maxIndx)
        {
            minIndx += (maxIndx - minIndx)/2;
        }

        /* set best DFE */
        curLane->result.dfeBestVal = minIndx;
        curLane->result.samplerCenter = vector[minIndx];
        curLane->result.samplerWindow = dfeSamplerList[minIndx];
        if (SerdesTestReportType == Verbose)
        {
            hwsSOptOsPrintfFuncPtr("\n\nResult::DFE is %d", curLane->result.dfeBestVal);
            hwsSOptOsPrintfFuncPtr("\nResult::FFE R is %d", curLane->result.ffeRBestVal);
            hwsSOptOsPrintfFuncPtr("\nResult::FFE C is %d", curLane->result.ffeCBestVal);
            hwsSOptOsPrintfFuncPtr("\nResult::samplerCenter is 0x%x", curLane->result.samplerCenter);
            hwsSOptOsPrintfFuncPtr("\nResult::samplerWindow is %d.\n", curLane->result.samplerWindow);
        }

        /*If sampler window < 5 taps ? report warning: BER risk */
        if (curLane->result.samplerWindow < samplerWindFailCritarea)
        {
            /* report warning: Time BER risk */
            curLane->result.timeBerRisk = GT_TRUE;
        }

        /* Phase 4: optimization */
        /* if the sampler center delta from the original starting point (0x5A) is too big
        (> 30% of valid sampler window from phase3) it means the scan is still not optimal; */
        if (vector[minIndx] > SAMPLER_START)
        {
            delta = vector[minIndx] - SAMPLER_START;
        }
        else
        {
            delta = SAMPLER_START - vector[minIndx];
        }

        /* repeat phase 1,2,3 one more time, starting with the DFE and sampler from this iteration */
        /* Max. 2 iterations total */
        curLane->accIf.setDfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, curLane->result.dfeBestVal);
        curLane->accIf.setFfeValueFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, curLane->result.ffeRBestVal, curLane->result.ffeCBestVal);
        curLane->accIf.samplerCfgFunc(curLane->devNum, curLane->groupNum, curLane->laneNum, curLane->result.samplerCenter);
        iterNum++;
    } while((iterNum < maxLoopNum) && (((delta*100)/SAMPLER_START) > samplerCenterDistance));

    if ((iterNum == maxLoopNum) && (((delta*100)/SAMPLER_START) > samplerCenterDistance))
    {
        curLane->tunningDone = 0;
    }
}

/* input parameters config functions */
int dfeInfoConfig(GT_U32 dfeMin, GT_U32 dfeMax)
{
    if (dfeMax > DFE_MAX)
    {
        return 1;
    }
    dfeConfig.dfeMinValue = dfeMin;
    dfeConfig.dfeMaxValue = dfeMax;
    return 0;
}

int ffeInfoConfig(GT_U32 ffeRMin, GT_U32 ffeRMax, GT_U32 ffeCMin, GT_U32 ffeCMax)
{
    if ((ffeRMax > FFE_R_MAX) || (ffeCMax > FFE_C_MAX))
    {
        return 1;
    }
    ffeConfig.ffeCMaxValue = ffeCMax;
    ffeConfig.ffeCMinValue = ffeCMin;
    ffeConfig.ffeRMaxValue = ffeRMax;
    ffeConfig.ffeRMinValue = ffeRMin;
    return 0;
}

void addInfoConfig(GT_U32 sampStart, GT_U32 sqlchVal)
{
    samplerStart = sampStart;
    sqlch = sqlchVal;
}

void mode1ParamsCfg(GT_U32 iterNum,
                    GT_U32 crit,
                    GT_32  rCrit,
                    GT_32  cCrit,
                    GT_U32 smoothDfe,
                    GT_U32 smoothSampler,
                    GT_U32 centerDist,
                    GT_U32 berRiskCrit)
{
    maxLoopNum = iterNum;
    samplerWindFailCritarea = crit;
    windFailRCritarea = rCrit;
    windFailCCritarea = cCrit;
    smoothFactorDfe = smoothDfe;
    smoothFactorSampler = smoothSampler;
    samplerCenterDistance = centerDist;
    volatgeBerRiskCriteria = berRiskCrit;
}
