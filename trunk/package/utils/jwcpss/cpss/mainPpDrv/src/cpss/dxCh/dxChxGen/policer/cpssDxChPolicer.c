/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPolicer.c
*
* DESCRIPTION:
*       Ingress Policing Engine function implementations.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxCh3/policer/private/prvCpssDxChPolicerLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/policer/cpssDxChPolicer.h>

/* number of Policer Counter sets */
#define PRV_CPSS_DXCH_POLICER_MAX_COUNTER_INDEX_CNS     16

/* check Policer Counter Index */
#define PRV_CPSS_DXCH_POLICER_COUNTER_INDEX_CHECK_MAC(_idx)     \
    if ((_idx) >= PRV_CPSS_DXCH_POLICER_MAX_COUNTER_INDEX_CNS)  \
    {                                                       \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                \
    }

/* number of Policer Table entries */
#define PRV_CPSS_DXCH_POLICER_MAX_INDEX_CNS             256

/* check Policer Table index */
#define PRV_CPSS_DXCH_POLICER_INDEX_CHECK_MAC(_idx)         \
    if ((_idx) >= PRV_CPSS_DXCH_POLICER_MAX_INDEX_CNS)      \
    {                                                       \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                \
    }


/* the size of Policer's table entry in words */
#define PRV_CPSS_DXCH_POLICER_TABLE_ENTRY_SIZE_CNS      2

/* the size of Policer Counter table entry in words */
#define PRV_CPSS_DXCH_POLICER_COUNTER_ENTRY_SIZE_CNS    2

/* number of rate type supported by DxCh Policer */
#define PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS         6

#define PRV_CPSS_DXCH_POLICER_PRE_INIT_VALUE_ARRAY_MAC {1,1,1,1,1,1}

/* flag for init was done */
static GT_BOOL  prvPolicerInitWasDone = GT_FALSE;

/* minimum and maximum value */
/*
rateMode |       rate (kbps)      |           cbs  (Bytes)    |
         | min      |      max    |     min    |        max   |
---------------------------------------------------------------
0        | 1        |    *1023    |     1      |   *(64k-1)   |
1        | 10       |    *1023    |     8      |   *(64k-1)   |
2        | 100      |    *1023    |     64     |   *(64k-1)   |
3        | 1k       |    *1023    |     512    |   *(64k-1)   |
4        | 10k      |    *1023    |     4K     |   *(64k-1)   |
5        | 100k     |    *1023    |     32K    |   *(64k-1)   |
*/

/*filled in init during runTime */
static GT_U32
    prvMeterMinRateValueArray[PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS] =
                    PRV_CPSS_DXCH_POLICER_PRE_INIT_VALUE_ARRAY_MAC;

/*filled in init during runTime */
static GT_U32
    prvMeterMaxCbsValueArray[PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS] =
                    PRV_CPSS_DXCH_POLICER_PRE_INIT_VALUE_ARRAY_MAC;

/*filled in init during runTime */
static GT_U32
    prvMeterMinCbsValueArray[PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS]=
                    PRV_CPSS_DXCH_POLICER_PRE_INIT_VALUE_ARRAY_MAC;


/*******************************************************************************
* the half granularity values are the values of Granularity of each range      *
* divided by 2 -- this is needed in order to achieve best accuracy in          *
* calculation of Hw CIR,CBS                                                    *
*******************************************************************************/

/* half granularity values of CIR (rate) values
-- filled in init during runTime */
static GT_U32
 prvMeterHalfGranularityRateValueArray[PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS]=
                    PRV_CPSS_DXCH_POLICER_PRE_INIT_VALUE_ARRAY_MAC;

/* half granularity values of CBS values
-- filled in init during runTime */
static GT_U32
  prvMeterHalfGranularityCbsValueArray[PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS]=
                    PRV_CPSS_DXCH_POLICER_PRE_INIT_VALUE_ARRAY_MAC;

/* the values of max rates that can achieve best accuracy in the ranges
-- filled in init during runTime */
static GT_U32
  prvMeterMaxRateDecrementHalfGranularityValueArray[PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS]=
                    PRV_CPSS_DXCH_POLICER_PRE_INIT_VALUE_ARRAY_MAC;

/*******************************************************************************
* cpssDxChPolicerRateCalcSw
*
* DESCRIPTION:
*       This routine calculate the : (SW)user rate , (SW)user cbs from
*           rateType , actual rate , actual cbs in Hw
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hwRateType -the rate type field read from Hw
*                       (bits 26..28 word 0 - tc entry)
*       hwRate - the rate field read from Hw
*                       (bits 16..25 word 0 - tc entry)
*       hwlCbs -  the cbs field read from Hw
*                       (bits 0..15 word 0 - tc entry)
*
* OUTPUTS:
*       userRatePtr - (pointer to)the rate that the user asked for (Kbps)
*       userCbsPtr  - (pointer to)the cbs that the user asked for (Bytes)
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cpssDxChPolicerRateCalcSw(
    IN  GT_U32      hwRateType,
    IN  GT_U32      hwRate,
    IN  GT_U32      hwlCbs,
    OUT GT_U32      *userRatePtr,
    OUT GT_U32      *userCbsPtr
)
{
    if(hwRateType >= PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    *userRatePtr = prvMeterMinRateValueArray[hwRateType] * hwRate;
    *userCbsPtr = prvMeterMinCbsValueArray[hwRateType] * hwlCbs;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerRateCalcHw
*
* DESCRIPTION:
*       This routine calculate the : rateType , actual rate , actual cbs
*       that result from user rate , user cbs
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       userRate - the rate that the user asked for (Kbps)
*       userCbs  - the cbs that the user asked for (Bytes)
*
* OUTPUTS:
*       rateTypePtr - (pointer to) the rate type field to write to Hw
*                       (bits 26..28 word 0 - tc entry)
*       actualRatePtr - (pointer to) the rate field to write to Hw
*                       (bits 16..25 word 0 - tc entry)
*       actualCbsPtr - (pointer to) the cbs field to write to Hw
*                       (bits 0..15 word 0 - tc entry)
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cpssDxChPolicerRateCalcHw(
    IN  GT_U32      userRate,
    IN  GT_U32      userCbs,
    OUT GT_U32      *rateTypePtr,
    OUT GT_U32      *actualRatePtr,
    OUT GT_U32      *actualCbsPtr
)
{
    GT_U32      ii;/*iterator*/

    for(ii = 0 ; ii < PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS; ii++)
    {
        if(userRate <
                prvMeterMaxRateDecrementHalfGranularityValueArray[ii])
        {
            *actualRatePtr =
                (userRate + prvMeterHalfGranularityRateValueArray[ii]) /
                    prvMeterMinRateValueArray[ii];
            break;
        }
    }

    if(ii == PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS)
    {
        /* the user rate is over the max supported */
        *actualRatePtr = 0x3FF;/* 10 bits */
        ii = PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS - 1;
    }

    *rateTypePtr = ii;

    if(userCbs > prvMeterMaxCbsValueArray[ii])
    {
        *actualCbsPtr = 0xFFFF;
    }
    else
    {
        *actualCbsPtr =
            (userCbs + prvMeterHalfGranularityCbsValueArray[ii]) /
                prvMeterMinCbsValueArray[ii] ;

        if((*actualCbsPtr) > 0xFFFF )
        {
            /* the user cbs is over the max supported in the selected range */
            *actualCbsPtr = 0xFFFF;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPolicerInit
*
* DESCRIPTION:
*       Init Traffic Conditioner facility on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - on illegal devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerInit
(
    IN  GT_U8       devNum
)
{
    GT_U32      ii;           /* loop iterator */
    GT_U32      maxRateValue; /* maximal rate for rate type */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);



    if(prvPolicerInitWasDone == GT_FALSE)
    {
        prvPolicerInitWasDone = GT_TRUE;

        for(ii = 0 ; ii < PRV_CPSS_DXCH_POLICER_RATE_TYPE_MAX_CNS ; ii++)
        {

            /* fill the min rate */
            prvMeterMinRateValueArray[ii] = (ii != 0) ?
                (10 * prvMeterMinRateValueArray[ii - 1])  :
                1;

            /* fill the max rate */
            maxRateValue =
                prvMeterMinRateValueArray[ii] * _1K;

            /* fill the min CBS */
            prvMeterMinCbsValueArray[ii] = (ii != 0) ?
                (8 * prvMeterMinCbsValueArray[ii - 1])  :
                1;

            /* fill the max CBS */
            prvMeterMaxCbsValueArray[ii] =
                prvMeterMinCbsValueArray[ii] * _64K;

            /* fill the rate half granularity */
            prvMeterHalfGranularityRateValueArray[ii] =
                prvMeterMinRateValueArray[ii] / 2;

            /* fill the CBS half granularity */
            prvMeterHalfGranularityCbsValueArray[ii] =
                prvMeterMinCbsValueArray[ii] / 2;

            /* fill the max decrement half granularity rate table */
            prvMeterMaxRateDecrementHalfGranularityValueArray[ii] =
                maxRateValue -
                prvMeterHalfGranularityRateValueArray[ii] ;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerInit
*
* DESCRIPTION:
*       Init Traffic Conditioner facility on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - on illegal devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerInit
(
    IN  GT_U8       devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChPolicerInit(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerPacketSizeModeSet
*
* DESCRIPTION:
*      Configure Policing mode for non tunnel terminated packets
*      The Policy engine provides the following modes to define packet size:
*       - Layer 1 metering. Packet size includes the
*         entire packet + IPG + preamble.
*       - Layer 2 metering. Packet size includes the entire packet,
*         including Layer 2 header and CRC.
*       - Layer 3 metering. Packet size includes Layer 3 information only,
*         excluding Layer 2 header and CRC.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - Policing mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerPacketSizeModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT        mode
)
{
    GT_U32      hwData;     /* data to write to register */
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    switch (mode)
    {
        case CPSS_POLICER_PACKET_SIZE_L3_ONLY_E:
            hwData = 2;
            break;
        case CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E:
            hwData = 1;
            break;
        case CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E:
            hwData = 0;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get address of Policers Global Configuration Rgister */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 2, hwData);
}

/*******************************************************************************
* cpssDxChPolicerPacketSizeModeSet
*
* DESCRIPTION:
*      Configure Policing mode for non tunnel terminated packets
*      The Policy engine provides the following modes to define packet size:
*       - Layer 1 metering. Packet size includes the
*         entire packet + IPG + preamble.
*       - Layer 2 metering. Packet size includes the entire packet,
*         including Layer 2 header and CRC.
*       - Layer 3 metering. Packet size includes Layer 3 information only,
*         excluding Layer 2 header and CRC.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - Policing mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPacketSizeModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT        mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerPacketSizeModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChPolicerPacketSizeModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerDropRedModeSet
*
* DESCRIPTION:
*      Set the type of the Policer drop action for red packets
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       dropRedMode - Red Drop mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or dropRedMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerDropRedModeSet
(
    IN  GT_U8                      devNum,
    IN  CPSS_DROP_MODE_TYPE_ENT    dropRedMode
)
{
    GT_U32      hwData;     /* data to write to register */
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    switch (dropRedMode)
    {
        case CPSS_DROP_MODE_SOFT_E:
            hwData = 0;
            break;
        case CPSS_DROP_MODE_HARD_E:
            hwData = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get address of Policers Global Configuration Rgister */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    return prvCpssHwPpSetRegField(devNum, regAddr, 2, 1, hwData);
}

/*******************************************************************************
* cpssDxChPolicerDropRedModeSet
*
* DESCRIPTION:
*      Set the type of the Policer drop action for red packets
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       dropRedMode - Red Drop mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or dropRedMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerDropRedModeSet
(
    IN  GT_U8                      devNum,
    IN  CPSS_DROP_MODE_TYPE_ENT    dropRedMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerDropRedModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropRedMode));

    rc = internal_cpssDxChPolicerDropRedModeSet(devNum, dropRedMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropRedMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerPacketSizeModeForTunnelTermSet
*
* DESCRIPTION:
*      Set the policing counting mode for tunnel terminated packets
*      The Policy engine provides the following modes to define packet size:
*       - Layer 1 metering. Packet size includes the
*         entire packet + IPG + preamble.
*       - Layer 2 metering. Packet size includes the entire packet,
*         including Layer 2 header and CRC.
*       - Passenger packet metering.
*         For IP-Over-x packets, the counting includes the passenger packet's
*         BC, excluding the tunnel header and the packets CRC
*         For, Ethernet-Over-x packets, the counting includes the passenger
*         packet's BC, excluding the tunnel header and if packet includes Two
*         CRC patterns (one for the inner packet and one for the outer packets)
*         also excluding the outer CRC
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - policing mode for tunnel terminated packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerPacketSizeModeForTunnelTermSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_POLICER_PACKET_SIZE_MODE_ENT   mode
)
{
    GT_U32      hwData;     /* data to write to register */
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    switch (mode)
    {
        case CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E:
            hwData = 2;
            break;
        case CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E:
            hwData = 1;
            break;
        case CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E:
            hwData = 0;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get address of Policers Global Configuration Rgister */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    return prvCpssHwPpSetRegField(devNum, regAddr, 7, 2, hwData);
}

/*******************************************************************************
* cpssDxChPolicerPacketSizeModeForTunnelTermSet
*
* DESCRIPTION:
*      Set the policing counting mode for tunnel terminated packets
*      The Policy engine provides the following modes to define packet size:
*       - Layer 1 metering. Packet size includes the
*         entire packet + IPG + preamble.
*       - Layer 2 metering. Packet size includes the entire packet,
*         including Layer 2 header and CRC.
*       - Passenger packet metering.
*         For IP-Over-x packets, the counting includes the passenger packet's
*         BC, excluding the tunnel header and the packets CRC
*         For, Ethernet-Over-x packets, the counting includes the passenger
*         packet's BC, excluding the tunnel header and if packet includes Two
*         CRC patterns (one for the inner packet and one for the outer packets)
*         also excluding the outer CRC
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - policing mode for tunnel terminated packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPacketSizeModeForTunnelTermSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_POLICER_PACKET_SIZE_MODE_ENT   mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerPacketSizeModeForTunnelTermSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChPolicerPacketSizeModeForTunnelTermSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerTokenBucketModeSet
*
* DESCRIPTION:
*      Set the policer token bucket mode and policer MRU.
*      To implement the token bucket algorithm, each of the 256 policers
*      incorporates a Bucket Size Counter (BucketSizeCnt).
*      This counter is incremented with tokens, according to the configured
*      policer rate (CIR) up to a maximal value of the configured
*      Policer Burst Size (CBS)
*      The Byte Count of each conforming packet is decremented from the counter.
*      When a new packet arrives, according to this configuration, the packet
*      conformance is checked according to one of the following modes:
*      - Strict Rate Limiter - If BucketSizeCnt > Packet's Byte Count the packet
*          is conforming else, it is out of profile.
*      - Loose Rate Limiter - If BucketSizeCnt > MRU the packet is conforming
*          else, it is out of profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - Policer Token Bucket mode.
*       mru         - Policer MRU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerTokenBucketModeSet
(
    IN GT_U8                      devNum,
    IN CPSS_POLICER_TB_MODE_ENT   mode,
    IN CPSS_POLICER_MRU_ENT       mru
)
{
    GT_U32      hwData;     /* data to write to register */
    GT_U32      tempHwData;
    GT_U32      regAddr;    /* register address */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    /* set Policer mode bit */
    switch (mode)
    {
        case CPSS_POLICER_TB_STRICT_E:
            tempHwData = 0;
            break;
        case CPSS_POLICER_TB_LOOSE_E:
            tempHwData = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* set mru bits */
    switch (mru)
    {
        case CPSS_POLICER_MRU_1536_E:
            hwData = 0;
            break;
        case CPSS_POLICER_MRU_2K_E:
            hwData = 1;
            break;
        case CPSS_POLICER_MRU_10K_E:
            hwData = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData |= (tempHwData << 2);

    /* get address of Policers Global Configuration Rgister */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    return prvCpssHwPpSetRegField(devNum, regAddr, 4, 3, hwData);
}

/*******************************************************************************
* cpssDxChPolicerTokenBucketModeSet
*
* DESCRIPTION:
*      Set the policer token bucket mode and policer MRU.
*      To implement the token bucket algorithm, each of the 256 policers
*      incorporates a Bucket Size Counter (BucketSizeCnt).
*      This counter is incremented with tokens, according to the configured
*      policer rate (CIR) up to a maximal value of the configured
*      Policer Burst Size (CBS)
*      The Byte Count of each conforming packet is decremented from the counter.
*      When a new packet arrives, according to this configuration, the packet
*      conformance is checked according to one of the following modes:
*      - Strict Rate Limiter - If BucketSizeCnt > Packet's Byte Count the packet
*          is conforming else, it is out of profile.
*      - Loose Rate Limiter - If BucketSizeCnt > MRU the packet is conforming
*          else, it is out of profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       mode        - Policer Token Bucket mode.
*       mru         - Policer MRU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vid
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerTokenBucketModeSet
(
    IN GT_U8                      devNum,
    IN CPSS_POLICER_TB_MODE_ENT   mode,
    IN CPSS_POLICER_MRU_ENT       mru
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerTokenBucketModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode, mru));

    rc = internal_cpssDxChPolicerTokenBucketModeSet(devNum, mode, mru);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode, mru));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerEntrySet
*
* DESCRIPTION:
*      Set Policer Entry configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*       entryPtr            - pointer to policer entry
*
* OUTPUTS:
*       tbParamsPtr         - pointer to actual policer token bucket parameters.
*                            The token bucket parameters are returned as output
*                            values. This is due to the hardware rate resolution,
*                            the exact rate or burst size requested may not be
*                            honored. The returned value gives the user the
*                            actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex or entry parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerEntrySet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            entryIndex,
    IN  CPSS_DXCH_POLICER_ENTRY_STC       *entryPtr,
    OUT CPSS_DXCH_POLICER_TB_PARAMS_STC   *tbParamsPtr
)
{
    GT_U32      hwData[PRV_CPSS_DXCH_POLICER_TABLE_ENTRY_SIZE_CNS];
    GT_STATUS   rc;             /* return status */
    GT_U32      hwRateType;     /* hw rate type value */
    GT_U32      hwCir;          /* hw rate value */
    GT_U32      hwCbs;          /* hw cbs value */
    GT_U32      tmpHwValue;     /* temporary value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_POLICER_INDEX_CHECK_MAC(entryIndex);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tbParamsPtr);
    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, entryPtr->qosProfile);
    PRV_CPSS_DXCH_POLICER_COUNTER_INDEX_CHECK_MAC(entryPtr->counterSetIndex);

    /* Word 0 */

    /* get hw values of rateType, policerRate and birstSize */
    rc = cpssDxChPolicerRateCalcHw(entryPtr->tbParams.cir,
                                   entryPtr->tbParams.cbs,
                                   &hwRateType,
                                   &hwCir,
                                   &hwCbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Policer Burst Size */
    hwData[0] = hwCbs;

    /* Policer Rate */
    hwData[0] |= (hwCir << 16);

    /* Policer Rate Type */
    hwData[0] |= (hwRateType << 26);

    /* Policer Color Mode */
    switch (entryPtr->meterColorMode)
    {
        case CPSS_POLICER_COLOR_BLIND_E:
            tmpHwValue = 0;
            break;
        case CPSS_POLICER_COLOR_AWARE_E:
            tmpHwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData[0] |= (tmpHwValue << 29);


    /* Word 1 */

    /* Policer Cmd */
    switch (entryPtr->cmd)
    {
        case CPSS_DXCH_POLICER_CMD_NONE_E:
            tmpHwValue = 0;
            break;
        case CPSS_DXCH_POLICER_CMD_DROP_RED_E:
            tmpHwValue = 1;
            break;
        case CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E:
            tmpHwValue = 2;
            break;
        case CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E:
            tmpHwValue = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData[1] = tmpHwValue;

    /* QoS Profile */
    hwData[1] |= (entryPtr->qosProfile << 2);

    /* Modify DSCP */
    switch (entryPtr->modifyDscp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            tmpHwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            tmpHwValue = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            tmpHwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData[1] |= (tmpHwValue << 9);

    /* Modify UP */
    switch (entryPtr->modifyUp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            tmpHwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            tmpHwValue = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            tmpHwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData[1] |= (tmpHwValue << 11);

    /* Policer Enable */
    tmpHwValue = BOOL2BIT_MAC(entryPtr->policerEnable);
    hwData[1] |= (tmpHwValue << 13);

    /* Policer Counting Enable */
    tmpHwValue = BOOL2BIT_MAC(entryPtr->counterEnable);
    hwData[1] |= (tmpHwValue << 14);

    /* Policer Counter Table Index */
    hwData[1] |= (entryPtr->counterSetIndex << 15);

    /* write Policer entry to Policer Table */
    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_POLICER_E,
                                    entryIndex,
                                    &hwData[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* translate hw values of burst count and Policer rate to sw */
    rc = cpssDxChPolicerRateCalcSw(hwRateType, hwCir, hwCbs,
                                   &tbParamsPtr->cir, &tbParamsPtr->cbs);
    return rc;
}

/*******************************************************************************
* cpssDxChPolicerEntrySet
*
* DESCRIPTION:
*      Set Policer Entry configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*       entryPtr            - pointer to policer entry
*
* OUTPUTS:
*       tbParamsPtr         - pointer to actual policer token bucket parameters.
*                            The token bucket parameters are returned as output
*                            values. This is due to the hardware rate resolution,
*                            the exact rate or burst size requested may not be
*                            honored. The returned value gives the user the
*                            actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex or entry parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEntrySet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            entryIndex,
    IN  CPSS_DXCH_POLICER_ENTRY_STC       *entryPtr,
    OUT CPSS_DXCH_POLICER_TB_PARAMS_STC   *tbParamsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, entryPtr, tbParamsPtr));

    rc = internal_cpssDxChPolicerEntrySet(devNum, entryIndex, entryPtr, tbParamsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, entryPtr, tbParamsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPolicerEntryGet
*
* DESCRIPTION:
*      Get Policer Entry parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*
* OUTPUTS:
*       entryPtr             - pointer to policer entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          entryIndex,
    OUT CPSS_DXCH_POLICER_ENTRY_STC     *entryPtr
)
{
    GT_U32      hwData[PRV_CPSS_DXCH_POLICER_TABLE_ENTRY_SIZE_CNS];
    GT_STATUS   rc;             /* return status */
    GT_U32      hwRateType;     /* hw rate type value */
    GT_U32      hwCir;          /* hw rate value */
    GT_U32      hwCbs;          /* hw cbs value */
    GT_U32      tmpHwValue;     /* temporary value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_POLICER_INDEX_CHECK_MAC(entryIndex);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    /* read Policer entry from Policer Table */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_TABLE_POLICER_E,
                                   entryIndex,
                                   &hwData[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Word 0 */

    /* Policer Burst size */
    hwCbs = (hwData[0] & 0xFFFF);

    /* Policer Rate */
    hwCir = ((hwData[0] >> 16) & 0x3FF);

    /* Policer Rate Type */
    hwRateType = ((hwData[0] >> 26) & 0x7);

    /* translate hw values of burst count and Policer rate to sw */
    rc = cpssDxChPolicerRateCalcSw(hwRateType, hwCir, hwCbs,
                                   &entryPtr->tbParams.cir,
                                   &entryPtr->tbParams.cbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Policer Color Mode */
    tmpHwValue = ((hwData[0] >> 29) & 0x1);

    entryPtr->meterColorMode = (tmpHwValue == 0) ? CPSS_POLICER_COLOR_BLIND_E :
        CPSS_POLICER_COLOR_AWARE_E;


    /* Word 1 */

    /* Policer Cmd */
    tmpHwValue = hwData[1] & 0x3;

    switch (tmpHwValue)
    {
        case 0:
            entryPtr->cmd = CPSS_DXCH_POLICER_CMD_NONE_E;
            break;
        case 1:
            entryPtr->cmd = CPSS_DXCH_POLICER_CMD_DROP_RED_E;
            break;
        case 2:
            entryPtr->cmd = CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E;
            break;
        case 3:
            entryPtr->cmd = CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* QoS Profile */
    entryPtr->qosProfile = ((hwData[1] >> 2) & 0x7F);

    /* Modify DSCP */
    tmpHwValue = ((hwData[1] >> 9) & 0x3);

    switch (tmpHwValue)
    {
        case 0:
            entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Modify UP */
    tmpHwValue = ((hwData[1] >> 11) & 0x3);

    switch (tmpHwValue)
    {
        case 0:
            entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case 1:
            entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        case 2:
            entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Policer Enable */
    tmpHwValue = ((hwData[1] >> 13) & 0x1);
    entryPtr->policerEnable = BIT2BOOL_MAC(tmpHwValue);

    /* Policer Counting Enable */
    tmpHwValue = ((hwData[1] >> 14) & 0x1);
    entryPtr->counterEnable = BIT2BOOL_MAC(tmpHwValue);

    /* Policer Counter Table Index */
    tmpHwValue = ((hwData[1] >> 15) & 0xF);
    entryPtr->counterSetIndex = tmpHwValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerEntryGet
*
* DESCRIPTION:
*      Get Policer Entry parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*
* OUTPUTS:
*       entryPtr             - pointer to policer entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          entryIndex,
    OUT CPSS_DXCH_POLICER_ENTRY_STC     *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, entryPtr));

    rc = internal_cpssDxChPolicerEntryGet(devNum, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerEntryInvalidate
*
* DESCRIPTION:
*      Invalidate Policer Entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerEntryInvalidate
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  entryIndex
)
{
    GT_U32      hwData[PRV_CPSS_DXCH_POLICER_TABLE_ENTRY_SIZE_CNS];
    GT_STATUS   rc;             /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_POLICER_INDEX_CHECK_MAC(entryIndex);

    /* reset all words in hwData */
    cpssOsMemSet(hwData, 0, sizeof(hwData));

    /* write Policer entry to Policer Table */
    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_POLICER_E,
                                    entryIndex,
                                    &hwData[0]);
    return rc;
}

/*******************************************************************************
* cpssDxChPolicerEntryInvalidate
*
* DESCRIPTION:
*      Invalidate Policer Entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - policer entry index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEntryInvalidate
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  entryIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerEntryInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex));

    rc = internal_cpssDxChPolicerEntryInvalidate(devNum, entryIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*      Calculate Token Bucket parameters in the Application format without
*      HW update.
*      The token bucket parameters are returned as output values. This is due to
*      the hardware rate resolution, the exact rate or burst size requested may
*      not be honored. The returned value gives the user the actual parameters
*      configured in the hardware.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       tbInParamsPtr   - pointer to Token bucket input parameters
*
* OUTPUTS:
*       tbOutParamsPtr  - pointer to Token bucket output paramters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerEntryMeterParamsCalculate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_POLICER_TB_PARAMS_STC        *tbInParamsPtr,
    OUT CPSS_DXCH_POLICER_TB_PARAMS_STC        *tbOutParamsPtr
)
{
    GT_STATUS   rc;             /* return status */
    GT_U32      hwRateType;     /* hw rate type value */
    GT_U32      hwCir;          /* hw rate value */
    GT_U32      hwCbs;          /* hw cbs value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(tbInParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(tbOutParamsPtr);

    /* calculate the hw value of cir and cbs */
    rc = cpssDxChPolicerRateCalcHw(tbInParamsPtr->cir,
                                   tbInParamsPtr->cbs,
                                   &hwRateType,
                                   &hwCir,
                                   &hwCbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPolicerRateCalcSw(hwRateType, hwCir, hwCbs,
                                   &tbOutParamsPtr->cir, &tbOutParamsPtr->cbs);
    return rc;
}

/*******************************************************************************
* cpssDxChPolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*      Calculate Token Bucket parameters in the Application format without
*      HW update.
*      The token bucket parameters are returned as output values. This is due to
*      the hardware rate resolution, the exact rate or burst size requested may
*      not be honored. The returned value gives the user the actual parameters
*      configured in the hardware.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       tbInParamsPtr   - pointer to Token bucket input parameters
*
* OUTPUTS:
*       tbOutParamsPtr  - pointer to Token bucket output paramters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEntryMeterParamsCalculate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_POLICER_TB_PARAMS_STC        *tbInParamsPtr,
    OUT CPSS_DXCH_POLICER_TB_PARAMS_STC        *tbOutParamsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerEntryMeterParamsCalculate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tbInParamsPtr, tbOutParamsPtr));

    rc = internal_cpssDxChPolicerEntryMeterParamsCalculate(devNum, tbInParamsPtr, tbOutParamsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tbInParamsPtr, tbOutParamsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerCountersGet
*
* DESCRIPTION:
*      Get Policer Counters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       countersPtr  - pointer to counters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or counterSetIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerCountersGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            counterSetIndex,
    OUT CPSS_DXCH_POLICER_COUNTERS_STC    *countersPtr

)
{
    GT_U32      hwData[PRV_CPSS_DXCH_POLICER_COUNTER_ENTRY_SIZE_CNS];
    GT_STATUS   rc;             /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_POLICER_COUNTER_INDEX_CHECK_MAC(counterSetIndex);
    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    /* read Policer Counters entry */
    rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
                                    counterSetIndex,
                                    &hwData[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    countersPtr->outOfProfileBytesCnt = hwData[0];
    countersPtr->inProfileBytesCnt = hwData[1];

    return rc;
}

/*******************************************************************************
* cpssDxChPolicerCountersGet
*
* DESCRIPTION:
*      Get Policer Counters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       countersPtr  - pointer to counters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or counterSetIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountersGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            counterSetIndex,
    OUT CPSS_DXCH_POLICER_COUNTERS_STC    *countersPtr

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerCountersGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counterSetIndex, countersPtr));

    rc = internal_cpssDxChPolicerCountersGet(devNum, counterSetIndex, countersPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counterSetIndex, countersPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPolicerCountersSet
*
* DESCRIPTION:
*      Set Policer Counters.
*      To reset counters use zero values.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index (APPLICABLE RANGES: 0..15)
*       countersPtr  - pointer to counters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or counterSetIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPolicerCountersSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          counterSetIndex,
    IN  CPSS_DXCH_POLICER_COUNTERS_STC  *countersPtr
)
{
    GT_U32      hwData[PRV_CPSS_DXCH_POLICER_COUNTER_ENTRY_SIZE_CNS];
    GT_STATUS   rc;             /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_POLICER_COUNTER_INDEX_CHECK_MAC(counterSetIndex);
    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    hwData[0] = countersPtr->outOfProfileBytesCnt;
    hwData[1] = countersPtr->inProfileBytesCnt;

    /* write Policer Counters entry */
    rc = prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
                                    counterSetIndex,
                                    &hwData[0]);
    return rc;
}

/*******************************************************************************
* cpssDxChPolicerCountersSet
*
* DESCRIPTION:
*      Set Policer Counters.
*      To reset counters use zero values.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index (APPLICABLE RANGES: 0..15)
*       countersPtr  - pointer to counters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or counterSetIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountersSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          counterSetIndex,
    IN  CPSS_DXCH_POLICER_COUNTERS_STC  *countersPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPolicerCountersSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counterSetIndex, countersPtr));

    rc = internal_cpssDxChPolicerCountersSet(devNum, counterSetIndex, countersPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counterSetIndex, countersPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


