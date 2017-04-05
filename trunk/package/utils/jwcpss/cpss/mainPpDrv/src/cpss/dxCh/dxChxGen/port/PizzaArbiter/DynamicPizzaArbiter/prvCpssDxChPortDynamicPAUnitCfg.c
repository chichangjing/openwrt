#ifndef STAND_ALONE
    #include <cpss/generic/cpssTypes.h>
    #include <cpss/extServices/private/prvCpssBindFunc.h>
    #include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#else
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include "CpssInclude/cpssTypes.h"
#endif
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAGCDAlgo.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPATypeDef.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiterWS.h>

#define gcd prvCpssDxChDynPAGCDAlgo

#define PRV_TEST_NUMBER_ALLOCATED_SLICE 1
#ifdef STAND_ALONE
    #define PRV_PRINT_COUNTERS       1
#else
    #define PRV_PRINT_COUNTERS       0
#endif


#define MAX_PORTS_NUM_CNS PRV_CPSS_DXCH_PORT_NUM_CNS
#define INVALID_SPEED_CNS 0


typedef struct
{
    GT_U32      number;
    GT_FLOAT64	speed;
}PortsConfig_STC;


typedef enum PA_PORT_STATE_ENT{
     PA_PORT_STATE_NOT_STARTED_E = 0
    ,PA_PORT_STATE_IN_PROCCESS_E  = 1    
    ,PA_PORT_STATE_COMPLETED_E   = 2        
}PA_PORT_STATE_ENT;

static PortsConfig_STC   portsConfigArray [MAX_PORTS_NUM_CNS];
static PA_PORT_STATE_ENT portStateArr     [MAX_PORTS_NUM_CNS];
static GT_FLOAT64        idealDistArr     [MAX_PORTS_NUM_CNS];	/* ideal distance between slices */
static GT_FLOAT64        nextIdealPlaceArr[MAX_PORTS_NUM_CNS];	    /* next ideal place (not real, that shall be determined based on ideal */
static GT_FLOAT64        weightArr        [MAX_PORTS_NUM_CNS];
static GT_U32            numberOfSlicesToConfigureArr[MAX_PORTS_NUM_CNS];
static GT_U32            numberOfConfiguredSlicesArr[MAX_PORTS_NUM_CNS];

static GT_U32            numberOfPorts;
static GT_U32            portInProcessIdxArr   [MAX_PORTS_NUM_CNS];
static GT_FLOAT64        DistFromSliceArr      [MAX_PORTS_NUM_CNS];
static GT_FLOAT64        DistFromSliceWeightArr[MAX_PORTS_NUM_CNS];

#ifdef STAND_ALONE
    FILE * fpConf = NULL;
    #if (PRV_PRINT_COUNTERS == 1)
        FILE * fpCounters = NULL;
    #endif
#endif



int cmpPorts(const PortsConfig_STC *elem1, const PortsConfig_STC *elem2)
{
    /* sort speeds from high to low,
     * and then sort ports from high to low
     * inside prestore increased order of port numbers
     */
    if(elem1->speed < elem2->speed)
    {
        return +1;
    }
    if(elem1->speed > elem2->speed)
    {
        return -1;
    }
    if(elem1->number < elem2->number)
    {
        return  -1;
    }
    if(elem1->number > elem2->number)
    {
        return  +1;
    }

    return 0;
}


static void SortPortsInDecreasingOrder(PortsConfig_STC * portsConfigArray, GT_U32 size)
{
    GT_U32 i;
    GT_U32 j;
    GT_U32 maxIdx;
    PortsConfig_STC tmp;

    for (i= 0; i < size -1; i++)
    {
        /* search for max */
        maxIdx = i;
        for (j = i+1; j< size; j++)
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
}


static int calcNOS
(
    /* IN */ GT_FLOAT64   totalBandwidth,
    /* OUT*/ GT_FLOAT64  *sliceBandwidthPtr,
    /* OUT*/ GT_U32      *numberOfSlicesPtr
)
{
    GT_FLOAT64  tmpSliceBandwidth;
    GT_FLOAT64  numberOfSlicesFl;
    GT_U32      numberOfSlicesU32;
    GT_U32		i;

    tmpSliceBandwidth = portsConfigArray[0].speed;

    for(i = 0; i < numberOfPorts; i++)
    {
        if((int)portsConfigArray[i].speed != portsConfigArray[i].speed)
        {
            tmpSliceBandwidth = 0.5;
            break;
        }
        else
        {
            tmpSliceBandwidth = (GT_FLOAT64)gcd((int)tmpSliceBandwidth, 
                                            (int)portsConfigArray[i].speed);
        }
    }
    #ifdef STAND_ALONE
        tmpSliceBandwidth = 1.0;
    #endif

    numberOfSlicesFl  = totalBandwidth/tmpSliceBandwidth;
    /* cpssOsPrintf("\nnumberOfSlices=%.2f", *numberOfSlicesPtr); */
    if((int)numberOfSlicesFl != numberOfSlicesFl)
    {
        return -1;
    }

    numberOfSlicesU32 = (GT_U32)numberOfSlicesFl;
    *numberOfSlicesPtr = numberOfSlicesU32;

    for(i = 0; i < numberOfPorts; i++)
    {
        numberOfSlicesToConfigureArr[i] = (int)(portsConfigArray[i].speed 
                                                                / tmpSliceBandwidth);
        idealDistArr[i] = ((GT_FLOAT64)numberOfSlicesU32)/(GT_FLOAT64)numberOfSlicesToConfigureArr[i];
        weightArr[i]    = 1.0;
    }
    *sliceBandwidthPtr = tmpSliceBandwidth;

    return 0;
}

#define PA_INVALID_PORT_CNS  0xFFFF

#if 0 /* let remain the code for possible future use */
static GT_U32 advance2NextSlice
(
    GT_U32 sliceIdx,
    GT_U32 numberOfSlices,
    GT_U32 *sliceArr
)
{
    sliceIdx = sliceIdx + 1;
    if (sliceIdx >= numberOfSlices)
    {
        sliceIdx = 0;
    }
    while (sliceArr[sliceIdx] != PA_INVALID_PORT_CNS)
    {
        sliceIdx = sliceIdx + 1;
        if (sliceIdx >= numberOfSlices)
        {
            sliceIdx = 0;
        }
    }
    return sliceIdx;
}
#endif

static int configPizza
(
    /* IN */GT_U32	   numberOfSlices,
    /* OUT*/GT_U32	  *sliceArr
)
{
    GT_U32	    sliceIdx;
    GT_U32      portIdx;
    GT_U32      i;
    GT_U32      bestPort;
    GT_BOOL     portWasFound;
    GT_U32      numberOfPortsInProcess;
    GT_U32      feasiblePortNum;

    #if (PRV_PRINT_COUNTERS == 1)
        fprintf(fpCounters,"\nIdeal Dist :");
        for (portIdx = 0 ; portIdx < numberOfPorts; portIdx++)
        {
            fprintf(fpCounters," %20.16f",idealDistArr[portIdx]);
        }
        fflush(fpCounters);
    #endif

    for (sliceIdx = 0; sliceIdx < numberOfSlices; sliceIdx++)
    {
        if (sliceIdx == 21)
        {
            bestPort = PA_INVALID_PORT_CNS;
        }
        bestPort = PA_INVALID_PORT_CNS;
        portWasFound = GT_FALSE;      
        numberOfPortsInProcess = 0;

        /* idxList = find(portStateArr == PORT_STATE_IN_PROCEED); */
        for (portIdx = 0 ; portIdx < numberOfPorts; portIdx++)
        {
            if (portStateArr[portIdx] == PA_PORT_STATE_IN_PROCCESS_E)
            {
                portInProcessIdxArr[numberOfPortsInProcess] = portIdx;
                numberOfPortsInProcess++;
            }
        }
        if (numberOfPortsInProcess > 0)
        {
            /* find ports that have ideal place in the past              */
            /* among them select that that has weited distance maximal   */
            /* i.e. that in faraway in the past                          */
            /* DistFromSliceArr = sliceIdx - nextIdealPlaceArr(idxList); */
            /*  feasiblePortIdxList = find(DistFromSliceArr >= 0);       */
            feasiblePortNum = 0;
            for (i = 0 ; i < numberOfPortsInProcess; i++)
            {
                portIdx = portInProcessIdxArr[i];
                DistFromSliceArr[i]       = sliceIdx - nextIdealPlaceArr[portIdx];
                DistFromSliceWeightArr[i] = DistFromSliceArr[i]*weightArr[portIdx];
                if (DistFromSliceArr[i] >= 0)
                {
                    feasiblePortNum ++;
                }
            }
            /* DistFromSliceWeightArr = DistFromSliceArr(feasiblePortIdxList).*weightArr(feasiblePortIdxList);  */
            /* [~,farAwayPortIdx] = max(DistFromSliceWeightArr);                                                */
            if (feasiblePortNum > 0)
            {
                GT_FLOAT64 maxWeightedDistance;

                bestPort = PA_INVALID_PORT_CNS;
                maxWeightedDistance = -1;
                for (i = 0 ; i < numberOfPortsInProcess; i++)
                {
                    /* find the port that has greatest weighted distance */
                    /* i.e. that is most in the past                     */
                    portIdx = portInProcessIdxArr[i];
                    if (DistFromSliceArr[i] >= 0)
                    {
                        if (DistFromSliceWeightArr[i] > maxWeightedDistance)
                        {
                            maxWeightedDistance = DistFromSliceWeightArr[i];
                            bestPort = portIdx;
                        }
                    }
                }
                portWasFound = GT_TRUE;
            }
        }
        if (GT_FALSE == portWasFound)
        {
            /* if exists port that not started , take first */
            /* else take the port that is slosest in the future */
            /* firstNonProceedPortIdx = find(portStateArr == PORT_STATE_NOT_STARTED,1,'first'); */
            for (portIdx = 0 ; portIdx < numberOfPorts; portIdx++)
            {
                if (portStateArr[portIdx] == PA_PORT_STATE_NOT_STARTED_E)
                {
                    portStateArr[portIdx] = PA_PORT_STATE_IN_PROCCESS_E;  
                    nextIdealPlaceArr[portIdx] = sliceIdx;
                    bestPort = portIdx;
                    portWasFound = GT_TRUE;
                    break;
                }
            }
            /* no ports without slices , take the nearest        */
            /* i.e.  take the port that is closest in the future */
            if (GT_FALSE == portWasFound) 
            {
                GT_FLOAT64 minWeightedDistance;
                minWeightedDistance = numberOfSlices;
                bestPort = PA_INVALID_PORT_CNS;
                for (i = 0 ; i < numberOfPortsInProcess; i++)
                {
                    portIdx = portInProcessIdxArr[i];
                    if (DistFromSliceArr[i] < 0)
                    { 
                        if (-DistFromSliceWeightArr[i] < minWeightedDistance)
                        {
                            minWeightedDistance = -DistFromSliceWeightArr[i];
                            bestPort = portIdx;
                        }
                    }
                }
            }
        }

        if (bestPort == PA_INVALID_PORT_CNS)
        {
            cpssOsPrintf("\n--->FATAL ERROR:");
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        numberOfConfiguredSlicesArr[bestPort]++;
        nextIdealPlaceArr[bestPort] = nextIdealPlaceArr[bestPort] + idealDistArr[bestPort];                                        
        
        /*  move port from list of having several slices to list of */
        /*  ports having all slices                                 */
        if (numberOfConfiguredSlicesArr[bestPort] == numberOfSlicesToConfigureArr[bestPort])
        {
            portStateArr[bestPort] = PA_PORT_STATE_COMPLETED_E;  
        }
        sliceArr[sliceIdx] = portsConfigArray[bestPort].number;
        #if (PRV_PRINT_COUNTERS == 1)
            fprintf(fpCounters,"\n %9d :",sliceIdx);
            for (portIdx = 0 ; portIdx < numberOfPorts; portIdx++)
            {
                fprintf(fpCounters," %20.16f",nextIdealPlaceArr[portIdx]);
            }
            fflush(fpCounters);
        #endif
    }

    return GT_OK;
}

GT_U32 CalculateNumberOfPorts
(
    GT_VOID
)
{
    GT_U32 i;
    PortsConfig_STC * ptr;

    ptr = &portsConfigArray[0];
    for (i = 0 ; i < sizeof(portsConfigArray)/sizeof(portsConfigArray[0]); i++, ptr++)
    {
        if (ptr->speed == 0)
        {
            break;
        }
    }
    return i;
}


#define UNUSED_PARAMETER(x) x = x


GT_STATUS BuildPizzaDistribution
(
    IN   GT_FLOAT64  *portsConfigArrayPtr,
    IN   GT_U32       size,
    IN   GT_U32       maxPipeSize,
    IN   GT_U32       minSliceResolution,
    IN   PRV_CPSS_DXCH_HIGH_SPEED_PORT_THRESH_PARAMS_STC *highSpeedPortThreshPtr,
    IN   GT_U32       pizzaArraySize,
    OUT  GT_U32      *pizzaArray,
    OUT  GT_U32      *numberOfSlicesPtr,
    OUT  GT_U32      *highSpeedPortNumPtr,
    OUT  GT_U32       highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]

)
{
    GT_U32  i;
    GT_FLOAT64	totalBandwidth = 0,
                sliceBandwidth;
    GT_U32      numberOfSlices;


    UNUSED_PARAMETER(minSliceResolution);
    UNUSED_PARAMETER(highSpeedPortThreshPtr);
    UNUSED_PARAMETER(highSpeedPortArr);

    *highSpeedPortNumPtr = 0;


    if (maxPipeSize == 0) /* nothing to configure */
    {
        goto nothing_to_configure;
    }
    
    cpssOsMemSet(portsConfigArray,0,sizeof(portsConfigArray));
    for(i = 0; i < size; i++)
    {
        portsConfigArray[i].number = i;
        portsConfigArray[i].speed = portsConfigArrayPtr[i];
        numberOfConfiguredSlicesArr [i] = 0;
        numberOfSlicesToConfigureArr[i] = 0;
        idealDistArr     [i] = 0;
        nextIdealPlaceArr[i] = 0;

        if(portsConfigArray[i].speed != INVALID_SPEED_CNS)
        {
            totalBandwidth += portsConfigArrayPtr[i];
        }
    }

    if (totalBandwidth == 0)
    {
        goto nothing_to_configure;
    }

    if(maxPipeSize < totalBandwidth)
    {
        cpssOsPrintf("\nmaxPipeSize(%d) < totalBandwidth(%f)", maxPipeSize, totalBandwidth);
        return 1;
    }

    SortPortsInDecreasingOrder(portsConfigArray, MAX_PORTS_NUM_CNS);

    numberOfPorts = CalculateNumberOfPorts();
    if(calcNOS(totalBandwidth, &sliceBandwidth, &numberOfSlices) != 0)
    {
        return 1;
    }

    if (numberOfSlices > pizzaArraySize)
    {
        return 1;
    }
    for(i = 0; i < pizzaArraySize; i++)
    {
        pizzaArray[i] = PA_INVALID_PORT_CNS;
    }
    for (i = 0 ; i <  MAX_PORTS_NUM_CNS; i++)
    {
        portStateArr[i]     = PA_PORT_STATE_NOT_STARTED_E;
        DistFromSliceArr[i] = 0;
    }

    configPizza(numberOfSlices, pizzaArray);
    *numberOfSlicesPtr = numberOfSlices;

    #if (PRV_TEST_NUMBER_ALLOCATED_SLICE == 1)
        for (i = 0 ; portsConfigArray[i].speed != INVALID_SPEED_CNS ; i++)
        {
            if (numberOfSlicesToConfigureArr[i] != numberOfConfiguredSlicesArr[i])
            {
                cpssOsPrintf("\nPIZZA ALGORITHM port [idx %d] num = %d speed = %7.2f",i, portsConfigArray[i].number, portsConfigArray[i].speed);
                cpssOsPrintf("\n    number of slices to alloc  = %d",numberOfSlicesToConfigureArr[i]);
                cpssOsPrintf("\n    number of slices allocated = %d",numberOfConfiguredSlicesArr[i]);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
    #endif
    return 0;
nothing_to_configure:
    *numberOfSlicesPtr = 0;
    for (i = 0; i < pizzaArraySize ; i++)
    {
        pizzaArray[i] = PA_INVALID_PORT_CNS;
    }
    return GT_OK;
}

/*--------------------------------------------------------------*/
/*                                                              */
/*--------------------------------------------------------------*/
GT_STATUS TestGCD(void)
{
    GT_U32 k;
    GT_U32 i,j;
    GT_U32 i_val,j_val;
    GT_U32 a,b;
    GT_U32 g ;

    GT_U32 i_val_arr[] = { 2, 4, 8, 16, 32 };
    GT_U32 j_val_arr[] = { 3, 5, 7, 9, 11, 13, 15, 17, 19, 21  };
    for (k = 1; k < 100 ; k++)
    {
        for (i = 0; i < sizeof(i_val_arr)/sizeof(i_val_arr[0]) ; i++)
        {
            i_val = i_val_arr[i];
            for (j = 0; j < sizeof(j_val_arr)/sizeof(j_val_arr[0]) ; j++)
            {
                j_val = j_val_arr[j];
                a = k*i_val;
                b = k*j_val;
                g = gcd(a,b);
                if (g != k)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
            }
        }
    }
    return GT_OK;
}



int TestConfig(void)
{
    GT_STATUS rc;
    CPSS_OS_FILE fp;
    GT_U32 i;
    static GT_FLOAT64 portsConfigArrayUsr[MAX_PORTS_NUM_CNS];
    GT_FLOAT64 *portsConfigArrayPtr;
    GT_U32  maxPipeSize = 168;
    GT_U32  numberOfSlices;
    static GT_U32  pizzaArray[2000];
    GT_U32 highSpeedNumber;
    GT_U32 highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS];

    for (i = 0 ; i < 49; i++)
    {
        portsConfigArrayUsr[i] = 1.0;
    }
    portsConfigArrayUsr[49] = 10;
    portsConfigArrayUsr[50] = 10;
    portsConfigArrayPtr = portsConfigArrayUsr;



    rc = BuildPizzaDistribution(portsConfigArrayPtr,MAX_PORTS_NUM_CNS,
                                   maxPipeSize,
                                /*IN minSliceResolution*/1000,
                                /*highSpeedPortParamsPtr*/NULL,
                                /*IN pizzaArraySize*/2000,
                                /*OUT*/&pizzaArray[0], &numberOfSlices, 
                                /*OUT*/&highSpeedNumber, highSpeedPortArr);
    if (rc != GT_OK)
    {
        return -1;
    }

    if ((fp=cpssOsFopen("PizzaConf.txt","w"))==CPSS_OS_FILE_INVALID)
    { 
        cpssOsPrintf("\nfailed to create file PizzaConf.txt");
        return -1;
    }

    for(i = 0; i < numberOfSlices; i++)
    {
        /*if(pizzaArray[i] != 0xff) */
        {
            cpssOsPrintf("\n'pz_slice%d_map' => %d", i, pizzaArray[i]);
            cpssOsFprintf(fp,"\n'pz_slice%d_map' => %d", i, pizzaArray[i]);
        }
    }
    cpssOsPrintf("\n'pizza_slice_to_run' => %d", (int)numberOfSlices);
    cpssOsFprintf(fp,"\n'pizza_slice_to_run' => %d", (int)numberOfSlices);
    cpssOsFclose(fp);
    return 0;
}

#ifdef STAND_ALONE

    #define MAX_NUM_OF_SLICES_CNS 340

    static GT_FLOAT64 *portsConfigArrayPtr;

    static GT_FLOAT64 portsConfigArrayUsr[MAX_PORTS_NUM_CNS];


    GT_CHAR * ProcessFile_SkipBlank(GT_CHAR * line)
    {
        GT_U32 i;
        if (line == NULL)
        {
            return NULL;
        }

        for (i = 0 ; *line != 0; i++, line++)
        {
            if (*line != ' ' && *line != '\t')
            {
                return line;
            }
        }
        return line;
    }

    int ProcessFile_GetConfig(FILE * fp, GT_U32 * configNPtr, GT_U32 * pipeBW)
    {
        static GT_CHAR token[20];
        static GT_CHAR line[1000];
        char * ptr;
        GT_U32 portNum;
        GT_U32 portIdx;
        GT_U32 portN;
        GT_U32 BW;


        for(portNum = 0; portNum  < MAX_PORTS_NUM_CNS; portNum++)
        {
            portsConfigArrayUsr[portNum ] = INVALID_SPEED_CNS;
        }
        for (;;)
        {
            if (NULL == fgets(&line[0],sizeof(line),fp))
            {
                *configNPtr = (GT_U32)(~0);
                *pipeBW     = 0;
                return 0;
            }
            ptr = strchr(line,'\n');
            if (ptr != NULL)
            {
                *ptr = 0;
            }
            ptr = ProcessFile_SkipBlank(line);
            if (ptr != line+strlen(line))
            {
                break;
            }
        }

        #ifdef STAND_ALONE
            fprintf(fpConf,"\n%s",line);
            fflush(fpConf);
            #if (PRV_PRINT_COUNTERS == 1)
                fprintf(fpCounters,"\n%s",line);
                fflush(fpCounters);
            #endif
        #endif

        ptr = &line[0];
        ptr = strchr(ptr,',');   /* skip IN       */
        ptr = strchr(ptr+1,','); /* skip "conf"   */
        *configNPtr = atoi(ptr+1);
        ptr = strchr(ptr+1,','); /* skip confN    */
        ptr = strchr(ptr+1,','); /* skip BW */
        *pipeBW = atoi(ptr+1);
        ptr = strchr(ptr+1,','); /* skip "BW" */
        ptr = strchr(ptr+1,','); /* skip "portsN" */
        portN = atoi(ptr+1);
        ptr = strchr(ptr+1,','); /* skip portsN   */
        for (portIdx = 0 ; portIdx < portN; portIdx++)
        {
            ptr = strchr(ptr+1,','); /* skip "portsN" */
            portNum = atoi(ptr+1);
            ptr = strchr(ptr+1,':'); /* skip "portsN" */
            BW = atoi(ptr+1);
            portsConfigArrayUsr[portNum] = (GT_FLOAT64)BW;
        }


        return 0;

    }

    int ProcessFile_GetPizza(FILE * fp, GT_U32 configN2Check,/*OUT*/GT_U32 * numberOfSlices2CheckPtr,GT_U32 *pizzaArray2CheckArr)
    {
        static GT_CHAR token[20];
        static GT_CHAR line[1000];
        char * ptr;
        GT_U32 confN; 
        GT_U32 sliceIdx;
        GT_U32 portNum;

        if (NULL == fgets(&line[0],sizeof(line),fp))
        {
            printf("\n-->ERROR : OUT , corresponding to IN expected");
            return -1;
        }

        ptr = &line[0];
        ptr = strchr(ptr,',');   /* skip OUT       */
        ptr = strchr(ptr+1,','); /* skip "conf"   */
        confN = atoi(ptr+1);
        if (confN != configN2Check)
        {
            printf("\n-->ERROR : configuration is not compatible");
            return -1;
        }
        ptr = strchr(ptr+1,','); /* skip confN    */
        ptr = strchr(ptr+1,','); /* skip "sliceN" */
        *numberOfSlices2CheckPtr = atoi(ptr+1);
        ptr = strchr(ptr+1,','); /* skip sliceN   */
        for (sliceIdx = 0 ; sliceIdx < *numberOfSlices2CheckPtr; sliceIdx++)
        {
            ptr = strchr(ptr+1,','); /* skip "pizza" */
            portNum = atoi(ptr+1);
            pizzaArray2CheckArr[sliceIdx] = portNum;
        }
        return 0;
    }

    int ProcessFile_PizzaCompare
    (
        GT_U32 numberOfSlices1, GT_U32 *pizzaArray1,
        GT_U32 numberOfSlices2, GT_U32 *pizzaArray2
    )
    {
        GT_U32 sliceIdx;
        GT_U32 portNum1;
        GT_U32 portNum2;
        int rc = 0;

        if (numberOfSlices1 != numberOfSlices2)
        {
            printf("\n--->ERROR sliceNum1 = %3d, sliceNum2 = %3d",numberOfSlices1,numberOfSlices2);
            return -1;
        }
        for (sliceIdx = 0; sliceIdx < numberOfSlices1 ; sliceIdx++)
        {
            portNum1 = pizzaArray1[sliceIdx];
            portNum2 = pizzaArray2[sliceIdx];
            if (portNum1 != portNum2)
            {
                printf("\n--->ERROR sliceIdx = %3d, portNum1 = %3d portNum2 = %3d",sliceIdx,portNum1,portNum2);
                rc = -1;
            }
        }
        return rc;
    }

    void PrintResults(FILE * fp, int confIdx, GT_U32 sliceN, GT_U32 *sliceArr)
    {
        GT_U32 i;
        fprintf(fp,"\nOUT, conf, %5d, sliceN, %3d, pizza",confIdx,sliceN);    
        for (i = 0; i < sliceN ; i++)
        {
            fprintf(fp,", %3d",sliceArr[i]);
        }
        fflush(fp);
    }


    int ProcessFile(char * fileName)
    {
        int rc;
        FILE	*fp;       /* file pointer */

        GT_U32 configN;
        GT_U32 maxPipeSize = 168;
        GT_U32 highSpeedPortNum;
        static GT_U32  highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS];
        static GT_U32  pizzaArray[MAX_NUM_OF_SLICES_CNS];
        GT_U32 numberOfSlices;
        static GT_U32  pizzaArray2Check[MAX_NUM_OF_SLICES_CNS];
        GT_U32 numberOfSlices2Check;
        GT_U32 errorN_comb;
        GT_U32 configIdx;

        #ifdef STAND_ALONE
            fpConf = fopen("C-Conf.txt","wt");;
            #if (PRV_PRINT_COUNTERS == 1)
                fpCounters = fopen("DEBUG_counters","wt");
            #endif
        #endif

        /* to Read a file, from the beginning */ 
        if ((fp=fopen(fileName, "r"))==NULL)
        { 
            cpssOsPrintf("\nfailed to open file %s", fileName);
            return -1;
        }

        configIdx   = 0;
        errorN_comb = 0;
        for (;;)
        {
            rc = ProcessFile_GetConfig(fp, /*OUT*/&configN, &maxPipeSize);
            if (rc != 0)
            {
                break;
            }
            if (configN == (GT_U32)(~0))
            {
                break;
            }
            cpssOsPrintf(" %4d",configIdx);
            if (configN % 20 == 0)
            {
                cpssOsPrintf("\n");
            }

            memset(pizzaArray2Check,0xFF,sizeof(pizzaArray2Check));
            if (configN == 1)
            {
                configN = configN;
            }

            rc = ProcessFile_GetPizza(fp, configN,/*OUT*/&numberOfSlices2Check,&pizzaArray2Check[0]);
            if (rc != 0)
            {
                return rc;
            }

            if (configN == 10000)
            {
                configN = configN;
            }
            portsConfigArrayPtr = portsConfigArrayUsr;

            /* 
            GT_STATUS BuildPizzaDistribution
            (
                IN   GT_FLOAT64  *portsConfigArrayPtr,
                IN   GT_U32       size,
                IN   GT_U32       maxPipeSize,
                IN   GT_U32       minSliceResolution,
                IN   GT_U32       pizzaArraySize,
                OUT  GT_U32      *pizzaArray,
                OUT  GT_U32      *numberOfSlicesPtr,
                OUT  GT_U32      *highSpeedPortNumPtr,
                OUT  GT_U32       highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
            )
            */


            BuildPizzaDistribution( portsConfigArrayPtr
                                   ,MAX_PORTS_NUM_CNS
                                   ,maxPipeSize
                                   ,/*minSliceResolution */0
                                   ,MAX_NUM_OF_SLICES_CNS
                                    /*OUT*/,&pizzaArray[0], &numberOfSlices,
                                    /*OUT*/&highSpeedPortNum, highSpeedPortArr);
            rc = ProcessFile_PizzaCompare(numberOfSlices, &pizzaArray[0], numberOfSlices2Check, &pizzaArray2Check[0]);

            #ifdef STAND_ALONE
                PrintResults(fpConf,configN,numberOfSlices,&pizzaArray[0]);
                #if (PRV_PRINT_COUNTERS == 1)
                    PrintResults(fpCounters,configN,numberOfSlices,&pizzaArray[0]);
                #endif
            #endif
            configIdx++;
            if (rc != 0)
            {
                printf("\n combN = %d errorN_comb = %d\n",configIdx,errorN_comb);
                errorN_comb++;
                /* return rc; */
            }
        }
        
        #ifdef STAND_ALONE
            fprintf(fpConf,"\n combN = %d errorN_comb = %d\n",configIdx,errorN_comb);
            fclose(fpConf);
            #if (PRV_PRINT_COUNTERS == 1)
                fclose(fpCounters);
            #endif
        #endif
        fclose(fp);
        printf("\n combN = %d errorN_comb = %d rate = %g%%",configIdx,errorN_comb,100*(double)errorN_comb/(double)configIdx);
        printf("\nPrint any key...");
        getchar();
        return 0;

    }



    int main(int argc, char *argv[])
    {
        int     rc;
        GT_U32  i;
        GT_U32  numberOfSlices;
        GT_U32  pizzaArray[MAX_NUM_OF_SLICES_CNS];
        GT_U32  maxPipeSize = 168;
        GT_U32  highSpeedPortNum;
        GT_U32  highSpeedPortArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS];

        if(argc > 1)
        {
            rc = ProcessFile(argv[1]);
            return rc;
        }

        for(i = 0; i < MAX_PORTS_NUM_CNS; i++)
        {
            portsConfigArrayUsr[i] = INVALID_SPEED_CNS;
        }
        for (i = 0 ; i <= 47; i++)
        {
            portsConfigArrayUsr[i] = 1.0;
        }
        for (i = 48 ; i <= 53; i++)
        {
            portsConfigArrayUsr[i] = 10;
        }
        portsConfigArrayPtr = portsConfigArrayUsr;
        BuildPizzaDistribution(portsConfigArrayPtr,MAX_PORTS_NUM_CNS,maxPipeSize,0,
                                   /*OUT*/MAX_NUM_OF_SLICES_CNS,&pizzaArray[0], &numberOfSlices,
                                   /*OUT*/&highSpeedPortNum, highSpeedPortArr);



        return 0;
    }
#endif



