#ifndef STAND_ALONE
    #include <cpss/generic/cpssTypes.h>
    #include <cpss/extServices/private/prvCpssBindFunc.h>
    #include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#else
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <CpssInclude/cpssTypes.h>
    #define cpssOsSprintf sprintf
#endif

#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPATypeDef.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAGCDAlgo.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiterWS.h>


#define gcd prvCpssDxChDynPAGCDAlgo

#define MAX_PORTS_NUM_CNS PRV_CPSS_DXCH_PORT_NUM_CNS
#define INVALID_SPEED_CNS 0
#define INVALID_PORT_CNS  0xFFFF


#define MIN_INTERSLICE_GAP_OPTIMAL_DISTR_CNS    3

/* #define PRV_ALGO_VER_REQUIRED_BANDWIDTH */
#define PRV_ALGO_VER_SLICE_0_5G_WITH_DUMMY

typedef int (*compfn)(const void*, const void*);
typedef struct
{
    GT_U32      portIdx;
    GT_FLOAT64  speed;
    GT_U32      numberOfSlicesToAllocate;
    GT_U32      numberOfSlicesAllocated;
    GT_U32      interSliceGap;                   /* number of slices skipped from last slice occupied by this port */
    #ifdef SLICE_LIST_DEBUG
        GT_U32  sliceList[100];
    #endif
}TxQPortConfig_STC;

typedef struct
{
    GT_U32               portNum;
    GT_U32               totalSize;
    GT_FLOAT64           requiredBandWidth;
    GT_U32               highSpeedPortNum;
    GT_U32               highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS];
    GT_U32               freeSlicesArr[340];
    TxQPortConfig_STC  * arr;
}TxQPortConfigList_STC;





static int cmpPorts(const TxQPortConfig_STC *elem1, const TxQPortConfig_STC *elem2)
{
    /* sort speeds from high to low,
     * and then sort ports from high to low
     */
    if(elem1->speed < elem2->speed)
    {
        return 1;
    }
    if(elem1->speed > elem2->speed)
    {
        return -1;
    }
    if(elem1->portIdx < elem2->portIdx)
    {
        return -1;
    }
    if(elem1->portIdx > elem2->portIdx)
    {
        return +1;
    }

    return 0;
}


static void PortListInit
(
    INOUT TxQPortConfigList_STC * portList,
    IN    TxQPortConfig_STC  * portsConfigArray,
    IN    GT_U32 size
)
{
    GT_U32 portIdx;

    cpssOsMemSet(portsConfigArray,0,sizeof(TxQPortConfig_STC)*size);
    portList->arr  = &portsConfigArray[0];
    portList->totalSize = size;
    portList->highSpeedPortNum = 0;
    portList->portNum = 0;

    for (portIdx = 0 ; portIdx < sizeof(portList->highSpeedPortArr)/sizeof(portList->highSpeedPortArr[0]); portIdx++)
    {
        portList->highSpeedPortArr[portIdx] = INVALID_PORT_CNS;
    }
}

static GT_STATUS PortListSet
(
    TxQPortConfigList_STC * portList,
    IN GT_FLOAT64  *portsConfigArrayPtr,
    IN GT_U32  size
)
{
    GT_U32 portIdx;

    if (portList->totalSize < size)
    {
        return 1;
    }
    portList->requiredBandWidth = 0;
    for(portIdx = 0; portIdx < size; portIdx++)
    {
        portList->arr[portIdx].portIdx = portIdx;
        portList->arr[portIdx].speed = portsConfigArrayPtr[portIdx];
        portList->arr[portIdx].numberOfSlicesAllocated = 0;
        portList->arr[portIdx].interSliceGap = 0;

        if(portList->arr[portIdx].speed != INVALID_SPEED_CNS)
        {
            portList->requiredBandWidth += portList->arr[portIdx].speed;
        }
    }

    portList->portNum = size;
    return GT_OK;
}


static void PortListSortInDecreasingOrder(TxQPortConfigList_STC * portList)
{
    GT_U32 i;
    GT_U32 j;
    GT_U32 maxIdx;
    TxQPortConfig_STC tmp;
    TxQPortConfig_STC *portsConfigArray;

    portsConfigArray = portList->arr;
    /* sort */
    for (i= 0; i < portList->portNum -1; i++)
    {
        /* search for max */
        maxIdx = i;
        for (j = i+1; j< portList->portNum; j++)
        {
            if (cmpPorts(&portsConfigArray[j],&portsConfigArray[maxIdx]) < 0)
            {
                maxIdx = j;
            }
        }
        /* exchange with current */
        if (maxIdx != i)
        {
            tmp = portsConfigArray[i];
            portsConfigArray[i] = portsConfigArray[maxIdx];
            portsConfigArray[maxIdx] = tmp;
        }
    }

    /* count actual number of ports */
    for (i = 0 ; portList->arr[i].speed > 0; i++);
    /* set actual number of ports */
    portList->portNum = i;
}

static int calcNumberOfSlices
(
    IN  TxQPortConfigList_STC *portList,
    IN  GT_U32                 maxPipeSizeMbps,
        IN  GT_U32                 minSliceResolutionMpbs,
    OUT GT_FLOAT64            *sliceBandwidthPtr,
    OUT GT_U32                *numberOfSlicesPtr
)
{
    GT_FLOAT64        numberOfSlices;
    GT_FLOAT64        tmpSliceBandwidth;
    GT_U32        portIdx;
    TxQPortConfig_STC * portsConfigArray;

    portsConfigArray = portList->arr;

    #ifdef PRV_ALGO_VER_SLICE_0_5G_WITH_DUMMY
        tmpSliceBandwidth = minSliceResolutionMpbs;
        numberOfSlices = (GT_U32)((GT_FLOAT64)maxPipeSizeMbps/tmpSliceBandwidth);
    #endif
    #ifdef PRV_ALGO_VER_REQUIRED_BANDWIDTH
        tmpSliceBandwidth = portsConfigArray[0].speed;

        for(portIdx = 0; portIdx < portList->portNum ; portIdx++)
        {
            if((int)portsConfigArray[portIdx].speed != portsConfigArray[portIdx].speed)
            {
                tmpSliceBandwidth = 0.5;
                break;
            }
            tmpSliceBandwidth = (GT_FLOAT64)gcd((int)tmpSliceBandwidth,
                                            (int)portsConfigArray[portIdx].speed);
        }
        numberOfSlices = portList->requiredBandWidth/tmpSliceBandwidth;

        if((int)numberOfSlices != numberOfSlices)
        {
            return -1;
        }
    #endif
    *numberOfSlicesPtr = (GT_U32)numberOfSlices;

    for(portIdx = 0; portIdx < portList->portNum; portIdx++)
    {
        portsConfigArray[portIdx].numberOfSlicesToAllocate = (int)(portsConfigArray[portIdx].speed
                                                                / tmpSliceBandwidth);
        if (portsConfigArray[portIdx].numberOfSlicesToAllocate * tmpSliceBandwidth < portsConfigArray[portIdx].speed)
        {
            portsConfigArray[portIdx].numberOfSlicesToAllocate++;
        }
    }

    *sliceBandwidthPtr = tmpSliceBandwidth;
    return 0;
}


static GT_VOID InitPizzaArray
(
    INOUT  GT_U32          *pizzaArray,
    IN     GT_U32           pizzaArraySize
)
{
    GT_U32 sliceIdx;
    for(sliceIdx = 0; sliceIdx < pizzaArraySize; sliceIdx++)
    {
        pizzaArray[sliceIdx] = INVALID_PORT_CNS;
    }
}

#ifdef SLICE_LIST_DEBUG
    void SliceDistributionPrintHdr(FILE * fp, GT_U32 numberOfSlices)
    {
        GT_U32 sliceId;
        fprintf(fp,    "\n portN speed slN HS#:");
        for(sliceId = 0; sliceId < numberOfSlices; sliceId++)
        {
            fprintf(fp,     " %3d",sliceId);
        }
    }

    void SliceDistributionPrintPortHdr(FILE * fp, GT_U32 portIdx, GT_FLOAT64 speed)
    {
        fprintf(fp,    "\n %5d %4.1fg", portIdx,speed);
    }

    void SliceDistributionPrintPizza(FILE * fp, GT_U32 numberOfSlices, GT_U32 numberOfHighSpeedPorts, GT_U32 *pizzaArray)
    {
        GT_U32 sliceId;
        fprintf(fp,    " %3d %3d:", numberOfSlices,numberOfHighSpeedPorts);
        for(sliceId = 0; sliceId < numberOfSlices; sliceId++)
        {
            char PortStr[10];
            if (pizzaArray[sliceId] != INVALID_PORT_CNS)
            {
                cpssOsSprintf(PortStr,"%3d", pizzaArray[sliceId]);
            }
            else
            {
                cpssOsSprintf(PortStr,"  *");
            }
            fprintf(fp," %s", PortStr);
        }
    }
#endif

static GT_STATUS configPizza_OptimalDistribution
(
    IN  TxQPortConfigList_STC * portList,
    IN  GT_U32           maxPipeSizeMbps,
    IN  GT_U32           minSliceResolutionMpbs,
    IN  GT_U32           highSpeedPortThresh,          /* on average distance between slices */
    IN  GT_U32           pizzaArraySize,
    OUT GT_U32          *pizzaArray,
    OUT GT_U32          *numberOfSlicesPtr
)
{
    GT_U32  idx;
    GT_U32  sliceIdx;
    GT_U32  portIdx;
    GT_FLOAT64   sliceBandwidth;
    GT_U32  portNum2Process;
    GT_U32  interSliceGap;
    GT_U32  numberOfSlices;
    GT_U32  numberOfFreeSlices;


    InitPizzaArray(pizzaArray,pizzaArraySize);

    if(calcNumberOfSlices(portList,maxPipeSizeMbps,minSliceResolutionMpbs,/*OUT*/&sliceBandwidth, &numberOfSlices) != 0)
    {
        return 1;
    }
    /* cpssOsPrintf("\nsliceBandwidth=%.2f", sliceBandwidth); */

    if (numberOfSlices > pizzaArraySize)
    {
        return 1;
    }
    *numberOfSlicesPtr = numberOfSlices;

    /*--------------------------------------*/
    /* calculate number of high speed ports */
    /*--------------------------------------*/
    portList->highSpeedPortNum = 0;
    if (portList->portNum <= CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS)  /* theretically only high speed ports */
    {
        if (portList->arr[0].numberOfSlicesToAllocate > 1)
        {
            for (portList->highSpeedPortNum = 0 ; portList->highSpeedPortNum < portList->portNum; portList->highSpeedPortNum++)
            {
                interSliceGap = numberOfSlices / portList->arr[portList->highSpeedPortNum].numberOfSlicesToAllocate;
                if (interSliceGap >= highSpeedPortThresh ) /* gap 3 is accceptable : slice : 0 1 2 3  */
                {                                             /*                         port : 0 x x 0  */
                    break;
                }
            }
        }
    }
    else
    {
        if (portList->arr[portList->highSpeedPortNum].numberOfSlicesToAllocate > 1)
        {
            for (portList->highSpeedPortNum = 0 ; portList->highSpeedPortNum < CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS; portList->highSpeedPortNum++)
            {
                interSliceGap = numberOfSlices / portList->arr[portList->highSpeedPortNum].numberOfSlicesToAllocate;
                if (interSliceGap >= highSpeedPortThresh )
                {
                    break;
                }
            }
        }
    }
    /*------------------------------------------*/
    /* calculate inter slice gap  for each port */
    /*------------------------------------------*/
    for (portIdx = portList->highSpeedPortNum ; portIdx < portList->portNum ; portIdx++)
    {

        if (portList->portNum > portList->highSpeedPortNum)
        {
            interSliceGap = numberOfSlices / portList->arr[portIdx].numberOfSlicesToAllocate;
        }
        else /* just high speed ports present */
        {
            interSliceGap = 1;
        }
        portList->arr[portIdx].interSliceGap = interSliceGap;
    }

    #ifdef SLICE_LIST_DEBUG
        SliceDistributionPrintHdr(stdout,numberOfSlices);
    #endif

    numberOfFreeSlices = numberOfSlices;
    sliceIdx = 0;
    for (portIdx = portList->highSpeedPortNum ; portIdx < portList->portNum ; portIdx++)
    {
        portNum2Process = portList->arr[portIdx].portIdx;

        for (idx = 0 ; idx < portList->arr[portIdx].numberOfSlicesToAllocate; idx++)
        {
            while (pizzaArray[sliceIdx] != INVALID_PORT_CNS)  /* skip all used slices */
            {
                sliceIdx++;
                if (sliceIdx > numberOfSlices)
                {
                    sliceIdx = 0;
                }
            }
            pizzaArray[sliceIdx] = portNum2Process;
            numberOfFreeSlices --;
            #ifdef SLICE_LIST_DEBUG
                portList->arr[portIdx].sliceList[portList->arr[portIdx].numberOfSlicesAllocated] = sliceIdx;
            #endif
            portList->arr[portIdx].numberOfSlicesAllocated++;


            sliceIdx += portList->arr[portIdx].interSliceGap;
            if (sliceIdx >= numberOfSlices)
            {
                sliceIdx -= numberOfSlices;
            }
        }
        if (portList->arr[portIdx].numberOfSlicesAllocated != portList->arr[portIdx].numberOfSlicesToAllocate)
        {
            return 0xFFFF;
        }
        #ifdef SLICE_LIST_DEBUG
            SliceDistributionPrintPortHdr(stdout, portList->arr[portIdx].portIdx, portList->arr[portIdx].speed);
            SliceDistributionPrintPizza  (stdout, numberOfSlices, portList->highSpeedPortNum, pizzaArray);
        #endif
    }
    /*--------------------------------*/
    /* now carry for high speed ports */
    /*--------------------------------*/
    for (portIdx = 0 ; portIdx < portList->highSpeedPortNum ;  portIdx++)
    {
        GT_U32 startSliceIdx_Q10;    /* 10 Bits after point */
        /* build list of free slices */
        numberOfFreeSlices= 0;
        for (sliceIdx = 0; sliceIdx < numberOfSlices; sliceIdx++)
        {
            if (pizzaArray[sliceIdx] == INVALID_PORT_CNS)
            {
                portList->freeSlicesArr[numberOfFreeSlices++] = sliceIdx;
            }
        }
        portNum2Process = portList->arr[portIdx].portIdx;
        portList->highSpeedPortArr[portIdx] = portNum2Process;
        portList->arr[portIdx].interSliceGap = (numberOfFreeSlices<<10)/portList->arr[portIdx].numberOfSlicesToAllocate;
        startSliceIdx_Q10 = 0;
        for (idx = 0 ; idx < portList->arr[portIdx].numberOfSlicesToAllocate; idx++)
        {
            sliceIdx = portList->freeSlicesArr[startSliceIdx_Q10>>10];
            pizzaArray[sliceIdx] = portNum2Process;
            #ifdef SLICE_LIST_DEBUG
                portList->arr[portIdx].sliceList[portList->arr[portIdx].numberOfSlicesAllocated] = sliceIdx;
            #endif
            portList->arr[portIdx].numberOfSlicesAllocated++;
            startSliceIdx_Q10 += portList->arr[portIdx].interSliceGap;  /* already Q10 */
        }
    }

    return GT_OK;
}

#define FIXP_MUL (1<<20)

GT_U32 U32_DiffAbs(GT_U32 a1, GT_U32 a2)
{
    if (a1 > a2)
    {
        return a1 -a2;
    }
    else
    {
        return a2-a1;
    }
}


GT_U32 q20_div(GT_U32 divisor, GT_U32 divider)
{
    GT_U32 a1 = divisor*FIXP_MUL;
    GT_U32 a2 = divider/2;
    GT_U32 rez;

    rez = (a1 + a2)/divider;
    return rez;
}

static GT_STATUS configPizza_apprAlgo
(
    IN  TxQPortConfigList_STC * portList,
    IN  GT_U32     maxPipeSizeMbps,
    IN  GT_U32     minSliceResolutionMpbs,
    IN  GT_U32     highSpeedPortThresh_Q20,
    IN  GT_U32     pizzaArraySize,
    OUT GT_U32    *pizzaArray,
    OUT GT_U32    *numberOfSlicesPtr
)
{
    GT_U32      placeIdx;
    GT_U32      sliceIdx;
    GT_U32      portIdx;
    GT_FLOAT64  sliceBandwidth;
    GT_U32      portNum2Process;
    GT_U32      numberOfSlices;
    GT_U32      numberOfFreeSlices;
    GT_U32      interSliceGap_Q20;


    InitPizzaArray(pizzaArray,pizzaArraySize);

    if(calcNumberOfSlices(portList,maxPipeSizeMbps,minSliceResolutionMpbs,/*OUT*/&sliceBandwidth, /*OUT*/&numberOfSlices) != 0)
    {
        return 1;
    }
    /* cpssOsPrintf("\nsliceBandwidth=%.2f", sliceBandwidth); */

    if (numberOfSlices > pizzaArraySize)
    {
        return 1;
    }
    *numberOfSlicesPtr = numberOfSlices;

    /*--------------------------------------*/
    /* calculate number of high speed ports */
    /*--------------------------------------*/
    portList->highSpeedPortNum = 0;
    if (portList->portNum <= CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS)  /* theretically only high speed ports */
    {
        if (portList->arr[0].numberOfSlicesToAllocate > 1)
        {
            for (portList->highSpeedPortNum = 0 ; portList->highSpeedPortNum < portList->portNum; portList->highSpeedPortNum++)
            {
                interSliceGap_Q20 = q20_div(numberOfSlices,portList->arr[portList->highSpeedPortNum].numberOfSlicesToAllocate);
                if (interSliceGap_Q20 >= highSpeedPortThresh_Q20)               /* gap 4 is accceptable : slice : 0 1 2 3 4 */
                {                                                               /*                         port : 0 x x x 0 */
                    break;
                }
            }
        }
    }
    else
    {
        if (portList->arr[portList->highSpeedPortNum].numberOfSlicesToAllocate > 1)
        {
            for (portList->highSpeedPortNum = 0 ; portList->highSpeedPortNum < CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS; portList->highSpeedPortNum++)
            {
                interSliceGap_Q20 = q20_div(numberOfSlices, portList->arr[portList->highSpeedPortNum].numberOfSlicesToAllocate);
                if (interSliceGap_Q20 >= highSpeedPortThresh_Q20) /* gap 4 is accceptable : slice : 0 1 2 3 4 */
                {                                                 /*                         port : 0 x x x 0 */
                    break;
                }
            }
        }
    }
    /*------------------------------------------*/
    /* calculate inter slice gap  for each port */
    /*------------------------------------------*/
    for (portIdx = portList->highSpeedPortNum ; portIdx < portList->portNum ; portIdx++)
    {

        if (portList->portNum > portList->highSpeedPortNum)
        {
            interSliceGap_Q20 = q20_div(numberOfSlices,portList->arr[portIdx].numberOfSlicesToAllocate);
        }
        else /* just high speed ports present */
        {
            interSliceGap_Q20 = 1*FIXP_MUL;
        }
        portList->arr[portIdx].interSliceGap = interSliceGap_Q20;
    }

    #ifdef SLICE_LIST_DEBUG
        SliceDistributionPrintHdr(stdout,numberOfSlices);
    #endif

    numberOfFreeSlices = numberOfSlices;
    for (portIdx = portList->highSpeedPortNum ; portIdx < portList->portNum ; portIdx++)
    {
        GT_U32 lastSliceIdx;
        GT_U32 prevPlace_q20;
        GT_U32 place_q20;
        GT_U32 dist_q20;

        portNum2Process = portList->arr[portIdx].portIdx;
        dist_q20        = portList->arr[portIdx].interSliceGap;
        /*-------------------------------*/
        /* find first empty slice        */
        /*-------------------------------*/
        for (sliceIdx = 0 ; pizzaArray[sliceIdx] != INVALID_PORT_CNS; sliceIdx++);
        pizzaArray[sliceIdx] = portNum2Process;
        portList->arr[portIdx].numberOfSlicesAllocated = 1;
        lastSliceIdx = sliceIdx;
        /*-------------------------------*/
        /* compute next ideal place      */
        /*-------------------------------*/
        prevPlace_q20 = sliceIdx*FIXP_MUL;
        place_q20     = prevPlace_q20 + dist_q20;

        for (placeIdx = 1 ; placeIdx < portList->arr[portIdx].numberOfSlicesToAllocate; placeIdx++)
        {
            GT_U32 idx;
            GT_U32 min_sliceIdx;
            GT_U32 min_dist_q20;
            GT_U32 prev_dist_q20;
            GT_U32 cur_dist_q20;
            /*-------------------------------------------------*/
            /* find FIRST empty place nearest to ideal place   */
            /* but greater than min distance (3)               */
            /*-------------------------------------------------*/
            /* a. build list of free slices                    */
            /*-------------------------------------------------*/
            numberOfFreeSlices= 0;
            for (sliceIdx = 0; sliceIdx < numberOfSlices; sliceIdx++)
            {
                if (pizzaArray[sliceIdx] == INVALID_PORT_CNS)
                {
                    portList->freeSlicesArr[numberOfFreeSlices++] = sliceIdx;
                }
            }
            min_sliceIdx = portList->freeSlicesArr[0];

            min_dist_q20 = prev_dist_q20 = U32_DiffAbs(min_sliceIdx*FIXP_MUL,place_q20);
            for (idx = 1 ; idx < numberOfFreeSlices; idx++)
            {
                sliceIdx = portList->freeSlicesArr[idx];
                if (sliceIdx - lastSliceIdx > MIN_INTERSLICE_GAP_OPTIMAL_DISTR_CNS)
                {
                    cur_dist_q20 = U32_DiffAbs(sliceIdx*FIXP_MUL,place_q20);
                    if (cur_dist_q20 < min_dist_q20)
                    {
                        min_dist_q20 = cur_dist_q20;
                        min_sliceIdx = sliceIdx;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            pizzaArray[min_sliceIdx] = portNum2Process;
            prevPlace_q20 = place_q20;
            place_q20     = prevPlace_q20 + dist_q20;
            portList->arr[portIdx].numberOfSlicesAllocated++;
            lastSliceIdx = min_sliceIdx;

        }
        if (portList->arr[portIdx].numberOfSlicesAllocated != portList->arr[portIdx].numberOfSlicesToAllocate)
        {
            return 0xFFFF;
        }
        #ifdef SLICE_LIST_DEBUG
            SliceDistributionPrintPortHdr(stdout, portList->arr[portIdx].portIdx, portList->arr[portIdx].speed);
            SliceDistributionPrintPizza  (stdout, numberOfSlices, portList->highSpeedPortNum, pizzaArray);
        #endif
    }
    /*--------------------------------*/
    /* now carry for high speed ports */
    /*--------------------------------*/
    for (portIdx = 0 ; portIdx < portList->highSpeedPortNum ;  portIdx++)
    {
        GT_U32 startSliceIdx_Q10;    /* 10 Bits after point */
        /* build list of free slices */
        numberOfFreeSlices= 0;
        for (sliceIdx = 0; sliceIdx < numberOfSlices; sliceIdx++)
        {
            if (pizzaArray[sliceIdx] == INVALID_PORT_CNS)
            {
                portList->freeSlicesArr[numberOfFreeSlices++] = sliceIdx;
            }
        }
        portNum2Process = portList->arr[portIdx].portIdx;
        portList->highSpeedPortArr[portIdx] = portNum2Process;
        portList->arr[portIdx].interSliceGap = (numberOfFreeSlices<<10)/portList->arr[portIdx].numberOfSlicesToAllocate;
        startSliceIdx_Q10 = 0;
        for (placeIdx = 0 ; placeIdx < portList->arr[portIdx].numberOfSlicesToAllocate; placeIdx++)
        {
            sliceIdx = portList->freeSlicesArr[startSliceIdx_Q10>>10];
            pizzaArray[sliceIdx] = portNum2Process;
            #ifdef SLICE_LIST_DEBUG
                portList->arr[portIdx].sliceList[placeIdx] = sliceIdx;
            #endif
            portList->arr[portIdx].numberOfSlicesAllocated++;
            startSliceIdx_Q10 += portList->arr[portIdx].interSliceGap;  /* already Q10 */
        }
    }
    return GT_OK;
}


#ifdef STAND_ALONE



    GT_STATUS CheckConfiguration
    (
        /*IN*/TxQPortConfigList_STC * portList,
        /*IN*/ GT_U32 *pizzaArray,
        /*IN*/ GT_U32  numberOfSlices
    )
    {
        GT_U32 portIdx;
        GT_U32 portNum;
        GT_U32 sliceIdx;
        GT_U32 firstSliceIdx;
        GT_U32 curSliceIdx;
        GT_U32 lastSliceIdx;
        GT_U32 nextSliceIdx;
        GT_U32 cnt;


        for(portIdx = 0; portIdx < portList->portNum; portIdx++)
        {
            portNum = portList->arr[portIdx].portIdx;
            /*----------------------------------*/
            /* check number of allocated slices */
            /*----------------------------------*/
            cnt = 0;
            for (sliceIdx = 0; sliceIdx < numberOfSlices ; sliceIdx++)
            {
                if (pizzaArray[sliceIdx] == portNum)
                {
                    cnt++;
                }
            }
            if (cnt != portList->arr[portIdx].numberOfSlicesToAllocate)
            {
                return 1;
            }
            if (portIdx >= portList->highSpeedPortNum)
            {
                /*-----------------------------------------*/
                /* check distance between allocated slices */
                /*-----------------------------------------*/
                if (portList->arr[portIdx].numberOfSlicesToAllocate > 1)
                {
                    nextSliceIdx = (GT_U32)-1;
                    firstSliceIdx = 0;
                    curSliceIdx   = 0;
                    lastSliceIdx  = 0;

                    for (cnt = 0; cnt < portList->arr[portIdx].numberOfSlicesToAllocate; cnt ++)
                    {
                        for (sliceIdx = nextSliceIdx + 1; sliceIdx < numberOfSlices ; sliceIdx++)
                        {
                            if (pizzaArray[sliceIdx] == portNum)
                            {
                                curSliceIdx = sliceIdx;
                                break;
                            }
                        }
                        if (cnt == 0)
                        {
                            firstSliceIdx = curSliceIdx;
                        }
                        for (sliceIdx = curSliceIdx+1; sliceIdx < numberOfSlices ; sliceIdx++)
                        {
                            if (pizzaArray[sliceIdx] == portNum)
                            {
                                nextSliceIdx = sliceIdx;
                                break;
                            }
                        }
                        if (cnt == portList->arr[portIdx].numberOfSlicesToAllocate -1)
                        {
                            lastSliceIdx = nextSliceIdx;
                        }
                        if (nextSliceIdx - curSliceIdx < MIN_INTERSLICE_GAP_OPTIMAL_DISTR_CNS)
                        {
                            return 1;
                        }
                    }
                    /*----------------------------------------------*/
                    /* check distance between last and first slices */
                    /*----------------------------------------------*/
                    if ( lastSliceIdx + MIN_INTERSLICE_GAP_OPTIMAL_DISTR_CNS >= numberOfSlices)
                    {
                        if (lastSliceIdx + MIN_INTERSLICE_GAP_OPTIMAL_DISTR_CNS - numberOfSlices > firstSliceIdx)
                        {
                            return 1;
                        }
                    }
                }
            }
        }
        return 0;
    }
#endif

GT_STATUS checkConditionForOptimalDistribution
(
    IN TxQPortConfigList_STC * portList,
    IN GT_U32                  maxPipeSizeMbps
)
{
    GT_32 portIdx;
    GT_U32 prevSpeed;
    GT_U32 curSpeed;

    if (portList->portNum == 0)
    {
        return GT_OK;
    }
    prevSpeed = (GT_U32)portList->arr[portList->portNum-1].speed;
    if (portList->portNum >= 2)
    {
        for (portIdx = (GT_32)portList->portNum-2; portIdx >= 0; portIdx --)
        {
            curSpeed = (GT_U32)portList->arr[portIdx].speed;
            if (curSpeed % prevSpeed != 0)
            {
                return/* this is not an error for LOG */GT_FAIL;
            }
            prevSpeed = curSpeed;
        }
    }
    if (maxPipeSizeMbps % prevSpeed != 0)
    {
        return/* this is not an error for LOG */GT_FAIL;
    }
    return GT_OK;
}

static TxQPortConfig_STC     portsConfigArray[MAX_PORTS_NUM_CNS];
static TxQPortConfigList_STC portList;
static GT_FLOAT64            txQPort64Bandwidth = 0;

GT_STATUS BuildTxQPizzaDistribution
(
    IN   GT_FLOAT64  *portsConfigArrayPtr,
    IN   GT_U32       portsConfigArraySize,
    IN   GT_U32       maxPipeSizeMbps,
    IN   GT_U32       minSliceResolutionMpbs,
    IN   PRV_CPSS_DXCH_HIGH_SPEED_PORT_THRESH_PARAMS_STC *highSpeedPortThreshPtr,
    IN   GT_U32       pizzaArraySize,
    OUT  GT_U32      *pizzaArray,
    OUT  GT_U32      *numberOfSlicesPtr,
    OUT  GT_U32      *numberOfHighSpeedPortsPtr,
    OUT  GT_U32       highSpeedPortsArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
)
{
    GT_U32  i;
    GT_STATUS rc;
    GT_U32  numberOfSlices = 0;

    PortListInit(&portList, &portsConfigArray[0],MAX_PORTS_NUM_CNS);

    if (portsConfigArrayPtr[64] != 0)
    {
        if (txQPort64Bandwidth != 0)
        {
            portsConfigArrayPtr[64] = txQPort64Bandwidth;
        }
    }

    PortListSet (&portList, portsConfigArrayPtr,portsConfigArraySize);


    if(portList.requiredBandWidth > maxPipeSizeMbps)
    {
        cpssOsPrintf("\nmaxPipeSize(%d) < totalBandwidth(%.2f)", maxPipeSizeMbps, portList.requiredBandWidth);
        return 1;
    }

    PortListSortInDecreasingOrder(&portList);


/*
    for(i = 0; i < MAX_PORTS_NUM_CNS; i++)
    {
        if(portsConfigArray[i].speed != INVALID_SPEED_CNS)
        {
            cpssOsPrintf("\nport %3d, speed %10.2f", portsConfigArray[i].number, portsConfigArray[i].speed);
        }
    }
*/
    if ( GT_OK == checkConditionForOptimalDistribution(&portList,maxPipeSizeMbps))
    {
        rc = configPizza_OptimalDistribution(&portList,maxPipeSizeMbps,minSliceResolutionMpbs, highSpeedPortThreshPtr->exactAlgoHighSpeedPortThresh, pizzaArraySize, /*OUT*/pizzaArray, /*OUT*/&numberOfSlices);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = configPizza_apprAlgo(&portList,maxPipeSizeMbps,minSliceResolutionMpbs, highSpeedPortThreshPtr->apprAlgoHighSpeedPortThresh_Q20,pizzaArraySize, /*OUT*/pizzaArray, /*OUT*/&numberOfSlices);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    *numberOfSlicesPtr = numberOfSlices;

    *numberOfHighSpeedPortsPtr = portList.highSpeedPortNum;
    for (i = 0 ; i < portList.highSpeedPortNum; i++)
    {
        highSpeedPortsArr[i] = portList.highSpeedPortArr[i];
    }
    #if 0  /* #ifdef STAND_ALONE */
        if (0 != CheckConfiguration(&portList,pizzaArray, /*OUT*/numberOfSlices))
        {
            CheckConfiguration(&portList,pizzaArray, /*OUT*/numberOfSlices);
            return 1;
        }
    #endif
    return GT_OK;
}


GT_STATUS setTxQ64BW(GT_U32 bw)
{
    if (bw == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (bw > 240)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    txQPort64Bandwidth = (GT_FLOAT64)bw;
    return GT_OK;
}

#ifdef STAND_ALONE


    #define MAX_NUM_OF_SLICES_CNS 340

    GT_U32  g_numberOfSlices;
    GT_U32  g_pizzaArray[MAX_NUM_OF_SLICES_CNS];
    GT_U32  g_numberOfHighSpeedPorts;
    GT_U32  g_highSpeedPortsArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS];

    int TestConfig(GT_U32 maxPipeSize, GT_FLOAT64 *portsConfigArrayPtr, GT_U32 portNum)
    {
        GT_U32 portN;
        GT_U32 sliceId;
        FILE * fp;
        GT_STATUS rc;
        GT_U32 minSliceResolutionMpbs;


        if ((fp=fopen("PizzaConf.txt","w"))==NULL)
        {
            perror("\nfailed to create file PizzaConf.txt");
            return -1;
        }
        fprintf(stdout,"\n portN speed slN HS#:");
        fprintf(fp,    "\n portN speed slN HS#:");
        for(sliceId = 0; sliceId < 240; sliceId++)
        {
            fprintf(stdout, " %3d",sliceId);
            fprintf(fp,     " %3d",sliceId);
        }


        for (portN = 1; portN <= portNum; portN++)
        {
            if (portsConfigArrayPtr[portN-1] == 0)
            {
                continue;
            }
            #ifndef SLICE_IDX_DEBUG
                fprintf(stdout,"\n %5d %4.1fg", portN-1,portsConfigArrayPtr[portN-1]);
                fprintf(fp,    "\n %5d %4.1fg", portN-1,portsConfigArrayPtr[portN-1]);
            #endif
            /*
            GT_STATUS prvBuildTxQPizzaDistribution
            (
                IN   GT_FLOAT64  *portsConfigArrayPtr,
                IN   GT_U32       portsConfigArraySize,
                IN   GT_U32       maxPipeSizeMbps,
                    IN   GT_U32       minSliceResolutionMpbs,
                IN   GT_U32       pizzaArraySize,
                OUT  GT_U32      *pizzaArray,
                OUT  GT_U32      *numberOfSlicesPtr,
                OUT  GT_U32      *numberOfHighSpeedPortsPtr,
                OUT  GT_U32       highSpeedPortsArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
            )
            */
            minSliceResolutionMpbs = 500;

            rc = prvBuildTxQPizzaDistribution(portsConfigArrayPtr,portN,
                                              maxPipeSize,
                                              minSliceResolutionMpbs,
                                            /*OUT*/MAX_NUM_OF_SLICES_CNS,&g_pizzaArray[0], &g_numberOfSlices,
                                            /*OUT*/&g_numberOfHighSpeedPorts,g_highSpeedPortsArr);
            if (rc != 0)
            {
                cpssOsPrintf("\n ---> ERROR : ");
                getchar();
                return -1;
            }
            #ifndef SLICE_IDX_DEBUG
                fprintf(stdout," %3d %3d:", g_numberOfSlices,g_numberOfHighSpeedPorts);
                fprintf(fp,    " %3d %3d:", g_numberOfSlices,g_numberOfHighSpeedPorts);
                for(sliceId = 0; sliceId < g_numberOfSlices; sliceId++)
                {
                    char PortStr[10];
                    if (g_pizzaArray[sliceId] != INVALID_PORT_CNS)
                    {
                        cpssOsSprintf(PortStr,"%3d", g_pizzaArray[sliceId]);
                    }
                    else
                    {
                        cpssOsSprintf(PortStr,"  *");
                    }
                    cpssOsPrintf(   " %s", PortStr);
                    fprintf(fp," %s", PortStr);

                }
                fflush(fp);
            #endif
        }
        fclose(fp);
        return 0;
    }




    GT_U32 buildListOfCombFullBW()
    {
        #define SPEED_N 5
        GT_U32 BW = 168;
        GT_U32 serdesNMax = 36;
        GT_U32 portNmax = 68;
        GT_U32 serdesNPerSpeed[SPEED_N] = {  4,  2,  1, 1,  1};
        GT_U32 speed          [SPEED_N] = { 40, 20, 10, 5,  1};
        GT_U32 k_40G,k_20G,k_10G,k_5G,k_1G;
        GT_U32 cnt;
        GT_U32 genPortN;
        GT_U32 serdesCnt;
        GT_U32 genBW;
        static GT_U32 list[100000][SPEED_N];

        cnt = 0;
        for (k_40G=0; k_40G <= 4; k_40G++)
        {
            for (k_20G=0; k_20G <= 12; k_20G++)
            {
                for (k_10G=0; k_10G <= 24; k_10G++)
                {
                    for (k_5G=0; k_5G <= 36; k_5G++)
                    {
                        for (k_1G=0; k_1G <= 48; k_1G++)
                        {
                            genPortN = k_40G + k_20G + k_10G + k_5G + k_1G;
                            if (1 < genPortN && genPortN<= portNmax)
                            {
                                serdesCnt = serdesNPerSpeed[0]*k_40G +
                                            serdesNPerSpeed[1]*k_20G +
                                            serdesNPerSpeed[2]*k_10G +
                                            serdesNPerSpeed[3]*k_5G  +
                                            serdesNPerSpeed[4]*k_1G;
                                if (serdesCnt<= serdesNMax)
                                {
                                    genBW = speed[0]*k_40G +
                                            speed[1]*k_20G +
                                            speed[2]*k_10G +
                                            speed[3]*k_5G  +
                                            speed[4]*k_1G;

                                    if (10 < genBW && genBW < BW)
                                    {
                                        list[cnt][0] = k_40G;
                                        list[cnt][1] = k_20G;
                                        list[cnt][2] = k_10G;
                                        list[cnt][3] = k_5G;
                                        list[cnt][4] = k_1G;
                                        cnt = cnt + 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return cnt;
    }


    static GT_FLOAT64 *portsConfigArrayPtr;
    static GT_FLOAT64 portsConfigArrayUsr[MAX_PORTS_NUM_CNS];





    int main(void)
    {
        int                option;
        GT_U32        i;
        //GT_U32  numberOfSlices;
        //GT_U32  pizzaArray[MAX_NUM_OF_SLICES_CNS];
        GT_U32  maxPipeSizeMbps;

        maxPipeSizeMbps = 168000;
        for(i = 0; i < MAX_PORTS_NUM_CNS; i++)
        {
            portsConfigArrayUsr[i] = INVALID_SPEED_CNS;
        }



        cpssOsPrintf ("\nEnter port configuration(255 for exit): ");
        cpssOsPrintf ("\n  1 - 50x1G + 7x10G          ");
        cpssOsPrintf ("\n  2 - 40x1G + 2x40G          ");
        cpssOsPrintf ("\n  3 - 10x1G + 4x2.5G + 6x10G + 1x40G");
        cpssOsPrintf ("\n  other - exit;");
        cpssOsPrintf ("\n-->");
        //scanf ("%d", &option);
        /*option = atoi(argv[1]);*/
        option = 8;

        switch(option)
        {
            case 1:
                for (i = 0 ; i < 50; i++)
                {
                    portsConfigArrayUsr[i] = 1.0;
                }
                for (i = 50; i < 57 ; i++)
                {
                    portsConfigArrayUsr[i] = 10.0;
                }
                portsConfigArrayPtr = portsConfigArrayUsr;
                break;

            case 2:
                for (i = 0 ; i < 40; i++)
                {
                    portsConfigArrayUsr[i] = 1.0;
                }
                portsConfigArrayUsr[41] = 40.0;
                portsConfigArrayUsr[42] = 40.0;
                portsConfigArrayPtr = portsConfigArrayUsr;
                break;

            case 3:
                for (i = 0 ; i < 10; i++)
                {
                    portsConfigArrayUsr[i] = 1.0;
                }
                for (i = 10 ; i < 14; i++)
                {
                    portsConfigArrayUsr[i] = 2.5;
                }
                for (i = 14 ; i < 20; i++)
                {
                    portsConfigArrayUsr[i] = 10;
                }
                portsConfigArrayUsr[40] = 40.0;
                portsConfigArrayPtr = portsConfigArrayUsr;
                break;

            case 4:
                maxPipeSizeMbps = 120000;
                for(i = 0; i < MAX_PORTS_NUM_CNS; i++)
                {
                    portsConfigArrayUsr[i] = INVALID_SPEED_CNS;
                }

                for (i = 0 ; i < 48; i++)
                {
                    portsConfigArrayUsr[i] = 1.0;
                }
                portsConfigArrayUsr[49] = 10.0;
                portsConfigArrayUsr[50] = 10.0;
                portsConfigArrayUsr[51] = 10.0;
                portsConfigArrayUsr[56] = 10.0;
                portsConfigArrayUsr[58] = 10.0;
                portsConfigArrayUsr[60] = 10.0;
                portsConfigArrayUsr[62] = 10.0;
                portsConfigArrayUsr[72] = 1.0;
                portsConfigArrayPtr = portsConfigArrayUsr;
                break;
            case 5:
                maxPipeSizeMbps = 120000;
                for(i = 0; i < 11; i++)
                {
                    portsConfigArrayUsr[i] = INVALID_SPEED_CNS;
                }
                for(i = 0; i < 11; i++)
                {
                    portsConfigArrayUsr[2*i]   = 10.0;
                    portsConfigArrayUsr[2*i+1] = 1.0;
                }
                portsConfigArrayPtr = portsConfigArrayUsr;
                break;
            case 6:   /* TM */
                maxPipeSizeMbps = 120000;
                portsConfigArrayUsr[64] = 48;
                portsConfigArrayUsr[73] = 1;
                portsConfigArrayPtr = portsConfigArrayUsr;
                break;
            case 7:
                maxPipeSizeMbps = 168000;
                portsConfigArrayUsr[0] = 40000.0;
                portsConfigArrayUsr[1] = 40000.0;
                portsConfigArrayUsr[2] = 20000.0;
                portsConfigArrayUsr[3] = 10000.0;
                portsConfigArrayUsr[4] = 10000.0;
                portsConfigArrayUsr[5] =  5000.0;
                portsConfigArrayUsr[6] =  5000.0;
                for (i = 7 ; i <= 15; i++)
                {
                    portsConfigArrayUsr[i] = 1000.0;
                }
                portsConfigArrayPtr = portsConfigArrayUsr;
                break;
            case 8:
                maxPipeSizeMbps = 168000;
                portsConfigArrayUsr[0] = 40000.0;
                for (i = 1 ; i <= 8; i++)
                {
                    portsConfigArrayUsr[i] = 10000.0;
                }
                for (i = 9 ; i <= 16; i++)
                {
                    portsConfigArrayUsr[i] = 1000.0;
                }

                portsConfigArrayPtr = portsConfigArrayUsr;
                break;

            default:
                cpssOsPrintf("\nNot supported option");
                return -1;
        }

        TestConfig(maxPipeSizeMbps,portsConfigArrayPtr,MAX_PORTS_NUM_CNS);

        return 0;
    }
#endif
