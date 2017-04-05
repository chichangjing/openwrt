#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitDrv.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPATypeDef.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiterWS.h>


GT_STATUS BuildPizzaDistributionWithDummySlice
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
    GT_STATUS   rc;
    GT_U32      i;
    GT_U32      totalBandwidth = 0;
    GT_U32      totalDummyPorts;
    GT_U32      dummyPortIdx;
    static  GT_U32  dummyPortNumMap[256];


    if (maxPipeSize == 0) /* nothing to configure */
    {
        goto nothing_to_configure;
    }
    for (i = 0 ; i < sizeof(dummyPortNumMap)/sizeof(dummyPortNumMap[0]); i++)
    {
        dummyPortNumMap[i] = PA_INVALID_PORT_CNS;
    }
    /*-----------------------------------------*
     *  calclulate bw to be allocated          *
     *-----------------------------------------*/
    totalBandwidth = 0;
    for (i = 0 ; i < size ; i++)
    {
        totalBandwidth += (GT_U32)(portsConfigArrayPtr[i]);
    }
    /*-----------------------------------------*
     *  compute number of dummy ports          *
     *-----------------------------------------*/
    if ((maxPipeSize - totalBandwidth) % minSliceResolution != 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* problem with resolution, somthing wrong in the system  */
    }
    totalDummyPorts = (maxPipeSize - totalBandwidth)/minSliceResolution;
    /*------------------------------------------------*
     *  Select dummy ports to be the ports with 0 BW  *
     *   set thir BW as minSliceResolution            *
     *------------------------------------------------*/
    if (totalDummyPorts > 0)
    {
        dummyPortIdx = 0;
        for (i = 0 ; i < size ; i++)
        {
            if (portsConfigArrayPtr[i] == 0)   /* set it to be dummy */
            {
                portsConfigArrayPtr[i] = minSliceResolution;
                dummyPortNumMap[i] = i; /* set  map : port i is valid , its value - i -- port number */
                dummyPortIdx++;
                if (dummyPortIdx == totalDummyPorts)
                {
                    break;
                }
            }
        }
    }
    /*-----------------------------------------*
     *  compute pizza                          *
     *-----------------------------------------*/
    rc = BuildPizzaDistribution(IN   portsConfigArrayPtr,
                                IN   size,
                                IN   maxPipeSize,
                                IN   minSliceResolution,
                                IN   highSpeedPortThreshPtr,
                                IN   pizzaArraySize,
                                OUT  pizzaArray,
                                OUT  numberOfSlicesPtr,
                                OUT  highSpeedPortNumPtr,
                                OUT  highSpeedPortArr);

    if (rc != GT_OK)
    {
        return rc;
    }
    /*-----------------------------------------*
     *  Make dymmu ports' slices to be invalid *
     *-----------------------------------------*/
    if (totalDummyPorts > 0)
    {
        GT_U32 sliceIdx;
        GT_U32 portNum;
        for (sliceIdx = 0 ; sliceIdx < *numberOfSlicesPtr ; sliceIdx++)
        {
            portNum = pizzaArray[sliceIdx];
            if (dummyPortNumMap[portNum] == portNum) /* really dummy port */
            {
                pizzaArray[sliceIdx] = PA_INVALID_PORT_CNS;
            }
        }
    }


    return rc;
nothing_to_configure:
    *numberOfSlicesPtr = 0;
    for (i = 0; i < pizzaArraySize ; i++)
    {
        pizzaArray[i] = PA_INVALID_PORT_CNS;
    }
    return GT_OK;
}




