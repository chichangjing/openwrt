/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoDxChPortUtils.c
*
* DESCRIPTION:
*       App demo DxCh PORT API.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/init/cpssInit.h>
#include <cpss/extServices/cpssExtServices.h>

#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

#include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>

/* include the external services */
#include <cmdShell/common/cmdExtServices.h> 

/*******************************************************************************
* util_appDemoDxChDeviceCoreClockGet
*
* DESCRIPTION:
*       Get core clock in Herz of device
*
* INPUTS:
*       dev         -   device id of ports
*
* OUTPUTS:
*       coreClockPtr - (pointer to)core clock in Herz
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS util_appDemoDxChDeviceCoreClockGet
(
    IN GT_U8          dev,
    OUT GT_U32       *coreClockPtr
)
{
    /* device must be active */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(coreClockPtr);

    switch (PRV_CPSS_PP_MAC(dev)->coreClock)
    {
        case 222: *coreClockPtr = 222222222; break;
        case 167: *coreClockPtr = 166666667; break;
        case 362: *coreClockPtr = 362500000; break;
        default: *coreClockPtr = PRV_CPSS_PP_MAC(dev)->coreClock * 1000000;
            break;
    }
    return GT_OK;
}


/*******************************************************************************
* util_appDemoDxChTimeStampsRateGet
*
* DESCRIPTION:
*       Get rate of events by VNT Time Stamps.
*
* INPUTS:
*       dev                 - device number
*       startTimeStamp      - start counting time stamp
*       endTimeStamp        - end counting time stamp
*       eventCount          - events count between end and start time stamps
*
* OUTPUTS:
*       none
*
* RETURNS:
*       events rate in events per second
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 util_appDemoDxChTimeStampsRateGet
(
    IN  GT_U8          dev,
    IN  GT_U32         startTimeStamp,
    IN  GT_U32         endTimeStamp,
    IN  GT_U32         eventCount
)
{
    GT_STATUS rc;           /* return code*/
    GT_U32    coreClock;    /* device core clock in HZ */
    double    diff;         /* difference between end and start time stamps */
    double    rate;         /* rate of events */
    GT_U32    rateOut;      /* integer value of rate */

    /* get core closk in HZ */
    rc = util_appDemoDxChDeviceCoreClockGet(dev,&coreClock);
    if (rc != GT_OK)
    {
        return 0xFFFFFFFF;
    }

    /* calculate difference between end and start */
    if (startTimeStamp > endTimeStamp)
    {
       /* take care of wraparound of end time */
       diff = 0xFFFFFFFFU;
       diff = diff + 1 + endTimeStamp - startTimeStamp;
    }
    else
       diff = endTimeStamp - startTimeStamp;

   /* time stamp is in device core clocks.
     event rate is number of events divided by time between them.
     the time is core clock period multiplied on number of core clocks.
     rate = eventCount / time, where time = diff * (1/coreClock) */
   rate = eventCount;
   rate = (rate * coreClock) / diff;

   /* convert float point value to unsigned integer */
   rateOut = (GT_U32)rate;

   return rateOut;
}


/*******************************************************************************
* util_appDemoDxChPortFWSRatesTableGet
*
* DESCRIPTION:
*       Calculates and stores RX/TX rate for all ports
*
* INPUTS:
*       dev            - device number
*       arrSize        - DB array size
*
* OUTPUTS:
*       rateDbPtr      - (pointer to) DB of rates
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS util_appDemoDxChPortFWSRatesTableGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 arrSize,
    OUT CMD_PORT_FWS_RATE_STC *rateDbPtr
)
{
    GT_U64      cntrValue;  /* value of counters */
    GT_PHYSICAL_PORT_NUM       portNum;              /* port number */
    GT_STATUS   rc;                   /* return code */
    GT_U32      startTimeStampRxArr[CPSS_MAX_PORTS_NUM_CNS]; /* VNT time stamp start for RX */
    GT_U32      startTimeStampTxArr[CPSS_MAX_PORTS_NUM_CNS]; /* VNT time stamp start for TX */
    GT_U32      endTimeStamp;   /* VNT time stamp start */
    GT_U32      rxRate;         /* VNT time stamp based RX rate */
    GT_U32      txRate;         /* VNT time stamp based TX rate */
    GT_U32      macPortNum;     /* MAC port number */

    cpssOsMemSet(rateDbPtr, 0, arrSize * sizeof(CMD_PORT_FWS_RATE_STC));
    cpssOsMemSet(startTimeStampRxArr, 0, sizeof(startTimeStampRxArr));
    cpssOsMemSet(startTimeStampTxArr, 0, sizeof(startTimeStampTxArr));

    portNum = 0;

    while (1)
    {
        if (portNum >= arrSize)
        {
            break;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portNum, PRV_CPSS_DXCH_PORT_TYPE_MAC_E, &macPortNum);
            if(rc != GT_OK)
            {
                portNum++;
                continue;
            }
        }
        else
        {
            if(PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) != GT_TRUE)
            {
                portNum++;
                continue;
            }
        }

        rc = cpssDxChMacCounterGet(devNum, portNum, CPSS_GOOD_UC_PKTS_RCV_E, &cntrValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChVntLastReadTimeStampGet(devNum, &startTimeStampRxArr[portNum]);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChMacCounterGet(devNum, portNum, CPSS_GOOD_UC_PKTS_SENT_E, &cntrValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChVntLastReadTimeStampGet(devNum, &startTimeStampTxArr[portNum]);
        if (rc != GT_OK)
        {
            return rc;
        }

        portNum++;
    }

    /* sleep */
    cpssOsTimerWkAfter(1000);

    portNum = 0;

    /* For device go over all available physical ports. */
    while (1)
    {
        if (portNum >= arrSize)
        {
            break;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portNum, PRV_CPSS_DXCH_PORT_TYPE_MAC_E, &macPortNum);
            if(rc != GT_OK)
            {
                portNum++;
                continue;
            }
        }
        else
        {
            if(PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) != GT_TRUE)
            {
                portNum++;
                continue;
            }
        }


        rc = cpssDxChMacCounterGet(devNum, portNum, CPSS_GOOD_UC_PKTS_RCV_E, &cntrValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChVntLastReadTimeStampGet(devNum, &endTimeStamp);
        if (rc != GT_OK)
        {
            return rc;
        }

        rxRate = util_appDemoDxChTimeStampsRateGet(devNum, startTimeStampRxArr[portNum], endTimeStamp, cntrValue.l[0]);

        rc = cpssDxChMacCounterGet(devNum, portNum, CPSS_GOOD_UC_PKTS_SENT_E, &cntrValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChVntLastReadTimeStampGet(devNum, &endTimeStamp);
        if (rc != GT_OK)
        {
            return rc;
        }

        txRate = util_appDemoDxChTimeStampsRateGet(devNum, startTimeStampTxArr[portNum], endTimeStamp, cntrValue.l[0]);

        rateDbPtr[portNum].rxPacketRate = rxRate;
        rateDbPtr[portNum].txPacketRate = txRate;

        portNum++;
    }

    return GT_OK;
}


/*******************************************************************************
* appDemoDxChPortRateGet
*
* DESCRIPTION:
*       This function calculates RX/TX packet rate on specific port.
*
* INPUTS:
*       rxPacketRate    - (pointer to) RX packet rate
*       txPacketRate    - (pointer to) TX packet rate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChPortPacketRateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *rxPacketRate,
    OUT GT_U32                  *txPacketRate
)
{
    GT_STATUS rc;
    static CMD_PORT_FWS_RATE_STC rateDb[CPSS_MAX_PORTS_NUM_CNS];

    CPSS_NULL_PTR_CHECK_MAC(rxPacketRate);
    CPSS_NULL_PTR_CHECK_MAC(txPacketRate);

    if(portNum >= CPSS_MAX_PORTS_NUM_CNS)
    {
        return GT_BAD_PARAM;
    }

    rc = util_appDemoDxChPortFWSRatesTableGet(devNum, CPSS_MAX_PORTS_NUM_CNS, rateDb);
    if(GT_OK != rc)
    {
        return rc;
    }

    *rxPacketRate = rateDb[portNum].rxPacketRate;
    *txPacketRate = rateDb[portNum].txPacketRate;

    return GT_OK;
}



