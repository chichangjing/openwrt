/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCnc.c
*
* DESCRIPTION:
*       CPSS DxCh Centralized Counters (CNC) API.
*
* FILE REVISION NUMBER:
*       $Revision: 65 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/cnc/private/prvCpssDxChCncLog.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/cnc/private/prvCpssDxChCnc.h>
#include <cpssCommon/private/prvCpssMath.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrg.h>

/* address of register increments: */
/* wraparound indexes per client */
#define CNC_WRAPAROUND_STATUS_ADDR_INCREMENT_CNS  0x00000100
/* 2k-counter block */
#define CNC_COUNTER_BLOCK_ADDR_INCREMENT_CNS      0x00004000

/* check that CNC block upload operation is supported,      */
/* not supported if PCI is not available or that workaround */
/* for FDB AU messages problem is used.                     */
#define PRV_CNC_BLOCK_UPLOAD_SUPPORT_CHECK_MAC(_dev)                    \
    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(_dev) == 0) ||                \
       (PRV_CPSS_DXCH_PP_MAC(_dev)->errata.                             \
                   info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.    \
                       enabled == GT_TRUE))                             \
    {                                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);                                        \
    }

/* The macro checks the _format parameter validity for Lion and        */
/* above devices. For other devices it assigns the _format to the      */
/* alone value supported for these devices.                            */
/* Such behavior of the macro makes the _format value passed by the    */
/* the caller not relevant for not Lion devices.                       */
#define PRV_CNC_COUNTER_FORMAT_CHECK_MAC(_devNum, _format)              \
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(_devNum))                   \
    {                                                                   \
        if ((GT_U32)_format                                             \
            > (GT_U32)CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E)            \
        {                                                               \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                        \
        }                                                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        _format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;                \
    }


/*******************************************************************************
* prvCpssDxChCncCounterSwToHw
*
* DESCRIPTION:
*   The function converts the SW counter to 64-bit HW format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       format         - CNC counter HW format,
*                        relevant only for Lion and above
*       swCounterPtr   - (pointer to) CNC Counter in SW format
*
* OUTPUTS:
*       hwCounterArr   - (pointer to) CNC Counter in HW format
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncCounterSwToHw
(
    IN   CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN   CPSS_DXCH_CNC_COUNTER_STC         *swCounterPtr,
    OUT  GT_U32                            hwCounterArr[]
)
{
    switch (format)
    {
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E:
            /* Packets counter: 29 bits, Byte Count counter: 35 bits */
            hwCounterArr[0] =
                (swCounterPtr->packetCount.l[0] & 0x1FFFFFFF)
                | (swCounterPtr->byteCount.l[1] << 29);
            hwCounterArr[1] = swCounterPtr->byteCount.l[0];
            break;

        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E:
            /* Packets counter: 27 bits, Byte Count counter: 37 bits */
            hwCounterArr[0] =
                (swCounterPtr->packetCount.l[0] & 0x07FFFFFF)
                | (swCounterPtr->byteCount.l[1] << 27);
            hwCounterArr[1] = swCounterPtr->byteCount.l[0];
            break;

        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E:
            /* Packets counter: 37 bits, Byte Count counter: 27 bits */
            hwCounterArr[0] = swCounterPtr->packetCount.l[0];
            hwCounterArr[1] =
                (swCounterPtr->byteCount.l[0] & 0x07FFFFFF)
                | (swCounterPtr->packetCount.l[1] << 27);
            break;

        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncCounterHwToSw
*
* DESCRIPTION:
*   The function converts the 64-bit HW counter to SW format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       format        - CNC counter HW format,
*                       relevant only for Lion and above
*       hwCounterArr   - (pointer to) CNC Counter in HW format
*
* OUTPUTS:
*       swCounterPtr   - (pointer to) CNC Counter in SW format
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncCounterHwToSw
(
    IN   CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN   GT_U32                            hwCounterArr[],
    OUT  CPSS_DXCH_CNC_COUNTER_STC         *swCounterPtr
)
{
    switch (format)
    {
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E:
            /* Packets counter: 29 bits, Byte Count counter: 35 bits */
            swCounterPtr->packetCount.l[0] =
                (hwCounterArr[0] & 0x1FFFFFFF);
            swCounterPtr->packetCount.l[1] = 0;
            swCounterPtr->byteCount.l[0] = hwCounterArr[1];
            swCounterPtr->byteCount.l[1] =
                ((hwCounterArr[0] >> 29) & 0x07);
            break;

        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E:
            /* Packets counter: 27 bits, Byte Count counter: 37 bits */
            swCounterPtr->packetCount.l[0] =
                (hwCounterArr[0] & 0x07FFFFFF);
            swCounterPtr->packetCount.l[1] = 0;
            swCounterPtr->byteCount.l[0] = hwCounterArr[1];
            swCounterPtr->byteCount.l[1] =
                ((hwCounterArr[0] >> 27) & 0x1F);
            break;

        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E:
            /* Packets counter: 37 bits, Byte Count counter: 27 bits */
            swCounterPtr->packetCount.l[0] = hwCounterArr[0];
            swCounterPtr->packetCount.l[1] =
                ((hwCounterArr[1] >> 27) & 0x1F);
            swCounterPtr->byteCount.l[0] =
                (hwCounterArr[1] & 0x07FFFFFF);
            swCounterPtr->byteCount.l[1] = 0;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncPortGroupCounterValueAverageSet
*
* DESCRIPTION:
*   The function sets the average counter value.
*   The get function must sum the values per port groups
*   and the calculated result must be as given.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       regAddr          - address of 64-bit memory to set the average value
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncPortGroupCounterValueAverageSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            regAddr,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;               /* return code                */
    GT_U32    data[2];          /* registers value            */
    GT_U16    portGroupNum;     /* number of port groups      */
    GT_U16    portGroupIndex;   /* index of port group in BMP */
    GT_U32    portGroupId;      /* the port group Id - support multi port groups device */
    CPSS_DXCH_CNC_COUNTER_STC   counter; /* work counter      */
    GT_U16    pktCntrMod;       /* packetCouner % portGroupNum*/
    GT_U16    byteCntrMod;      /* byteCouner % portGroupNum  */
    GT_U64    w64;              /* work variable              */
    GT_U32      numDuplicatedUnits;     /* support duplicated units in the core */
    GT_PORT_GROUPS_BMP  tmp_portGroupsBmp;  /*temp port groups bmp */ /* support duplicated units in the core */
    GT_BOOL isAdditionalRegDupNeeded;   /* support duplicated units in the core */
    PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC  additionalRegDup; /* support duplicated units in the core */
    GT_U32  dupUnit;/* support duplicated units in the core */
    GT_U32  origRegAddr;/* support duplicated units in the core */


    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CNC_COUNTER_FORMAT_CHECK_MAC(devNum, format);
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    /* calculate number of port groups */
    portGroupNum = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        portGroupNum ++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    if (portGroupNum == 0)
    {
        /* nothing to do */
        return GT_OK;
    }

    /* support duplications of the unit in the device (per core) */
    rc = prvCpssHwPpPortGroupNumDuplicatedUnitsGet(devNum,
            PRV_CPSS_DXCH_UNIT_CNC_0_E/*use CNC 0 as representative to all CNC units in single core*/,
            &numDuplicatedUnits);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    portGroupNum *= (GT_U16)(1/*orig*/ +
                    numDuplicatedUnits/*additional*/);

    /* calculate average counter value */
    pktCntrMod = prvCpssMathMod64By16(
        counterPtr->packetCount, portGroupNum);
    byteCntrMod = prvCpssMathMod64By16(
        counterPtr->byteCount, portGroupNum);
    counter.packetCount = prvCpssMathDiv64By16(
        counterPtr->packetCount, portGroupNum);
    counter.byteCount = prvCpssMathDiv64By16(
        counterPtr->byteCount, portGroupNum);

    /* add one to average */
    w64.l[1] = 0;
    w64.l[0] = 1;
    counter.packetCount = prvCpssMathAdd64(
        counter.packetCount, w64);
    counter.byteCount = prvCpssMathAdd64(
        counter.byteCount, w64);

    /* to avoid compiler warning */
    data[0] = 0;
    data[1] = 0;

    origRegAddr = regAddr;

    /* loop on all port groups to get the counter value from */
    portGroupIndex = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        for(dupUnit = 0; dupUnit < (1 + numDuplicatedUnits) ; dupUnit++)
        {
            if (portGroupIndex == pktCntrMod)
            {
                /* subtract one from average back */
                counter.packetCount = prvCpssMathSub64(
                    counter.packetCount, w64);
            }

            if (portGroupIndex == byteCntrMod)
            {
                /* subtract one from average back */
                counter.byteCount = prvCpssMathSub64(
                    counter.byteCount, w64);
            }

            if ((portGroupIndex == 0)
                || (portGroupIndex == pktCntrMod)
                || (portGroupIndex == byteCntrMod))
            {
                /* convert counter from SW to HW format       */
                /* this conversion done at the first loop and */
                /* any time when "counter" updated            */
                rc = prvCpssDxChCncCounterSwToHw(
                    format, &counter, data);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            if(dupUnit != 0 &&
               GT_TRUE ==
                   prvCpssDuplicatedMultiPortGroupsGet(devNum,portGroupId,origRegAddr,&tmp_portGroupsBmp,
                       &isAdditionalRegDupNeeded,&additionalRegDup))
            {
                if(isAdditionalRegDupNeeded == GT_TRUE)
                {
                    PRV_CPSS_MUST_DIRECT_CALL_TO_DRIVER_INDICATION_CNS
                   /* we need to replace the address of the accessing to the
                      one of the duplicated unit */
                   regAddr = additionalRegDup.additionalAddressesArr[dupUnit-1];
                }
            }

            PRV_CPSS_MUST_DIRECT_CALL_TO_DRIVER_INDICATION_CNS
            rc = prvCpssDrvHwPpPortGroupWriteRam(
                devNum, portGroupId, regAddr, 2/* words*/, data);
            if (rc != GT_OK)
            {
                return rc;
            }

            portGroupIndex ++;
        }

        regAddr = origRegAddr;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncPortGroupCounterValueSummingGet
*
* DESCRIPTION:
*   The function gets the counter contents summing per port groups
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       regAddr          - address of 64-bit memory to get the sum value
*                          (APPLICABLE RANGES: DxCh3, xCat, xCat3 0..2047;
*                                              Lion 0..511;
*                                              xCat2 0..255)
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr        - (pointer to) counter contents
*                           The result counter contains the sum of counters
*                           read from all specified port groups.
*                           It contains more bits than in HW.
*                           For example the sum of 4 35-bit values may be
*                           37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncPortGroupCounterValueSummingGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            regAddr,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    data[2];          /* field value       */
    GT_U32    portGroupId;      /* port group Id     */
    CPSS_DXCH_CNC_COUNTER_STC  counter; /* work counter */
    GT_U32      numDuplicatedUnits;     /* support duplicated units in the core */
    GT_PORT_GROUPS_BMP  tmp_portGroupsBmp;  /*temp port groups bmp */ /* support duplicated units in the core */
    GT_BOOL isAdditionalRegDupNeeded;   /* support duplicated units in the core */
    PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC  additionalRegDup; /* support duplicated units in the core */
    GT_U32  dupUnit;/* support duplicated units in the core */
    GT_U32  origRegAddr;/* support duplicated units in the core */

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CNC_COUNTER_FORMAT_CHECK_MAC(devNum, format);
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    cpssOsMemSet(counterPtr, 0, sizeof(CPSS_DXCH_CNC_COUNTER_STC));

    /* support duplications of the unit in the device (per core) */
    rc = prvCpssHwPpPortGroupNumDuplicatedUnitsGet(devNum,
            PRV_CPSS_DXCH_UNIT_CNC_0_E/*use CNC 0 as representative to all CNC units in single core*/,
            &numDuplicatedUnits);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    origRegAddr = regAddr;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        for(dupUnit = 0; dupUnit < (1 + numDuplicatedUnits) ; dupUnit++)
        {
            if(dupUnit != 0 &&
               GT_TRUE ==
                   prvCpssDuplicatedMultiPortGroupsGet(devNum,portGroupId,origRegAddr,&tmp_portGroupsBmp,
                       &isAdditionalRegDupNeeded,&additionalRegDup))
            {
                if(isAdditionalRegDupNeeded == GT_TRUE)
                {
                    PRV_CPSS_MUST_DIRECT_CALL_TO_DRIVER_INDICATION_CNS
                   /* we need to replace the address of the accessing to the
                      one of the duplicated unit */
                   regAddr = additionalRegDup.additionalAddressesArr[dupUnit-1];
                }
            }

            PRV_CPSS_MUST_DIRECT_CALL_TO_DRIVER_INDICATION_CNS
            rc = prvCpssDrvHwPpPortGroupReadRam(
                devNum, portGroupId, regAddr, 2/*words*/, data);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssDxChCncCounterHwToSw(
                format, data, &counter);
            if (rc != GT_OK)
            {
                return rc;
            }

            counterPtr->packetCount =
                prvCpssMathAdd64(
                    counterPtr->packetCount, counter.packetCount);

            counterPtr->byteCount =
                prvCpssMathAdd64(
                    counterPtr->byteCount, counter.byteCount);
        }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncEArchBlockUnitGet
*
* DESCRIPTION:
*   The function calculates unit and relative block by block number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       blockNum       - CNC block number
* OUTPUTS:
*       uninNumPtr     - (pointer to) number of CNC unit.
*       blockInUnitPtr - (pointer to) number of block in CNC unit.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncEArchBlockUnitGet
(
    IN   GT_U8                     devNum,
    IN   GT_U32                    blockNum,
    OUT  GT_U32                    *uninNumPtr,
    OUT  GT_U32                    *blockInUnitPtr
)
{
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *uninNumPtr     = blockNum / PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlocks;
    *blockInUnitPtr = blockNum % PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlocks;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncEArchBlockClientToCfgAddr
*
* DESCRIPTION:
*   The function returns address and bit mask for register configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       client         - CNC client
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
* OUTPUTS:
*       regAddrPtr    - (pointer to) address of the block configuration register.
*       bitMaskPtr    - (pointer to) bit mask of the block configuration.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCncEArchBlockClientToCfgAddr
(
    IN   GT_U8                     devNum,
    IN   CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN   GT_U32                    blockNum,
    OUT  GT_U32                    *regAddrPtr,
    OUT  GT_U32                    *bitMaskPtr
)
{
    GT_U32      unitNum;     /* number of CNC unit          */
    GT_U32      blockInUnit; /* number of block in CNC unit */

    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);

    if (GT_OK != prvCpssDxChCncEArchBlockUnitGet(
        devNum, blockNum, &unitNum, &blockInUnit))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *regAddrPtr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).perBlockRegs.
        clientEnable.CNCBlockConfigReg0[blockInUnit];

    /* for PCL and TTI mask contains all parallel sublookup clients */
    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_TTI_E:                             *bitMaskPtr = BIT_0  ; break;
        case CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_0_E:                  *bitMaskPtr = BIT_0  ; break;
        case CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_1_E:                  *bitMaskPtr = BIT_1  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E:            *bitMaskPtr = BIT_2  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_0_E: *bitMaskPtr = BIT_2  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_1_E: *bitMaskPtr = BIT_3  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_2_E: *bitMaskPtr = BIT_4  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_3_E: *bitMaskPtr = BIT_5  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E:          *bitMaskPtr = BIT_6  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_0_E: *bitMaskPtr = BIT_6  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_1_E: *bitMaskPtr = BIT_7  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_2_E: *bitMaskPtr = BIT_8  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_3_E: *bitMaskPtr = BIT_9  ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E:            *bitMaskPtr = BIT_10 ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_0_E: *bitMaskPtr = BIT_10 ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_1_E: *bitMaskPtr = BIT_11 ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_2_E: *bitMaskPtr = BIT_12 ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_3_E: *bitMaskPtr = BIT_13 ; break;
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E:               *bitMaskPtr = BIT_14 ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_SRC_EPORT_E:               *bitMaskPtr = BIT_15 ; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E:          *bitMaskPtr = BIT_16 ; break;
        case CPSS_DXCH_CNC_CLIENT_PACKET_TYPE_PASS_DROP_E:           *bitMaskPtr = BIT_17 ; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E:           *bitMaskPtr = BIT_18 ; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E:          *bitMaskPtr = BIT_19 ; break;
        case CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E:                *bitMaskPtr = BIT_20 ; break;
        case CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E:                    *bitMaskPtr = BIT_21 ; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_TRG_EPORT_E:                *bitMaskPtr = BIT_22 ; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E:                      *bitMaskPtr = BIT_23 ; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_0_E:           *bitMaskPtr = BIT_23 ; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_1_E:           *bitMaskPtr = BIT_24 ; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_2_E:           *bitMaskPtr = BIT_25 ; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_3_E:           *bitMaskPtr = BIT_26 ; break;
        case CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E:
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.trafficManager.supported == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            *bitMaskPtr = BIT_27 ;
            break;
        case CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_2_E:
            if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            *bitMaskPtr = BIT_28 ;
            break;
        case CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_3_E:
            if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            *bitMaskPtr = BIT_29 ;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncBlockClientToCountMode
*
* DESCRIPTION:
*   The function returns address and start bit and number of bits for 'count mode' field.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
* OUTPUTS:
*       regAddrPtr    - (pointer to) address of the block count mode register.
*       startBitPtr   - (pointer to) start bit of the field.
*       numBitsPtr    - (pointer to) number of bits in the field.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncBlockClientToCountMode
(
    IN   GT_U8                     devNum,
    IN   GT_U32                    blockNum,
    OUT  GT_U32                    *regAddrPtr,
    OUT  GT_U32                    *startBitPtr,
    OUT  GT_U32                    *numBitsPtr
)
{
    GT_U32    unitNum;       /* number of CNC unit          */
    GT_U32    blockInUnit;   /* number of block in CNC unit */
    GT_U32    regAddr;       /* register address  */
    GT_U32    offset;        /* field Offset      */
    GT_U32    index;         /* register index    */
    GT_U32    numOfBits = 2; /* number of bits in the field */
    GT_U32    sip5_20Supported = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

#ifdef GM_USED
    /* the GM currently not supports this new format */
    sip5_20Supported = 0;
#endif /*GM_USED*/


    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (GT_OK != prvCpssDxChCncEArchBlockUnitGet(
            devNum, blockNum, &unitNum, &blockInUnit))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (!sip5_20Supported)
        {
            regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).perBlockRegs.
                clientEnable.CNCBlockConfigReg0[blockInUnit];

            offset = 30;
        }
        else
        {
            numOfBits = 3; /* 3 bits each field */
            index  = blockInUnit / 8;/* 8 fields in register */
            offset = (blockInUnit % 8) * numOfBits;

            regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).perBlockRegs.
                entryMode.CNCBlocksCounterEntryModeRegister[index];
        }
    }
    else
    {
        /* Not eArch devices */

        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockClientCfgAddrBase
            + (blockNum * 0x100);

        offset = 12;
    }

    *regAddrPtr = regAddr;
    *startBitPtr = offset;
    *numBitsPtr = numOfBits;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncBlockClientToCfgAddr
*
* DESCRIPTION:
*   The function converts the client to HW base address of configuration register.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       client         - CNC client
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
* OUTPUTS:
*       cfgRegPtr    - (pointer to) address of the block configuration register.
*       bitOffsetPtr - (pointer to) bit offset of the block configuration.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCncBlockClientToCfgAddr
(
    IN   GT_U8                     devNum,
    IN   CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN   GT_U32                    blockNum,
    OUT  GT_U32                    *cfgRegPtr,
    OUT  GT_U32                    *bitOffsetPtr
)
{
    GT_U32      index = 0;  /* client HW index */

    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E:      index = 0; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E:   index = 1; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E: index = 2; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E:   index = 3; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E: index = 4; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E:  index = 5; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E: index = 6; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E:             index = 7; break;
        case CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E:       index = 8; break;
        case CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E:           index = 9; break;
        case CPSS_DXCH_CNC_CLIENT_TTI_E:                    index = 10; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* Lion, xCat2   */
        /* per block registers base */
        *cfgRegPtr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                cncRegs.blockClientCfgAddrBase
           + (CNC_BLOCK_CLIENT_CFG_ADDR_INCREMENT_CNS * (blockNum));

        /* client enable bit */
        *bitOffsetPtr = index;
    }
    else
    {
        /* DxCh3, xCat, xCat3         */
        /* block + client register */
        *cfgRegPtr = PRV_CPSS_DXCH_CNC_CLIENT_CFG_ADDR_MAC(
            devNum, blockNum, index);

        /* bits 12:0 for even, bits 26:13 for odd */
        *bitOffsetPtr = (index & 1) ? 13 : 0;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncClientCountModeRegGet
*
* DESCRIPTION:
*   The function calculates register address, bit offset and L2-mode
*   pattern for CNC client count mode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       client         - CNC client
*
* OUTPUTS:
*       regAddrPtr       - (pointer to) register address.
*       bitOffPtr        - (pointer to) bit offset in register.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncClientCountModeRegGet
(
    IN   GT_U8                     devNum,
    IN   CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT  GT_U32                    *regAddrPtr,
    OUT  GT_U32                    *bitOffPtr
)
{
    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).countingModeConfig;
            }
            else
            {
                *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            }
            *bitOffPtr = 3;
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_0_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_1_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_2_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_3_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* no case break */
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).countingModeConfig;
            }
            else
            {
                *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            }
            *bitOffPtr = 0;
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_0_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_1_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_2_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_3_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* no case break */
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).countingModeConfig;
            }
            else
            {
                *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            }
            *bitOffPtr = 1;
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_0_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_1_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_2_E:
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_3_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* no case break */
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).countingModeConfig;
            }
            else
            {
                *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            }
            *bitOffPtr = 2;
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                        preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
                *bitOffPtr = 13;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
                *bitOffPtr = 14;
            }
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.CNCModes.CNCModes;
                *bitOffPtr = 2;
            }
            else
            {
                if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
                {
                    *regAddrPtr =
                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.cncCountMode;
                }
                else
                {
                    *regAddrPtr =
                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                            peripheralAccess.cncModes.modesRegister;
                }
                *bitOffPtr = 2;
            }
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.CNCModes.CNCModes;
                *bitOffPtr = 0;
            }
            else
            {
                if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
                {
                    *regAddrPtr =
                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.cncCountMode;
                }
                else
                {
                    *regAddrPtr =
                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                            peripheralAccess.cncModes.modesRegister;
                }
                *bitOffPtr = 0;
            }
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_0_E:
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_1_E:
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_2_E:
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_3_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* no case break */
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
                *bitOffPtr = 10;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
                *bitOffPtr = 25;
            }
            break;
        case CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
                *bitOffPtr = 0;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
                *bitOffPtr = 0;
            }
            break;
        case CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
                *bitOffPtr = 1;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
                *bitOffPtr = 1;
            }
            break;
        case CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_2_E:
        case CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_3_E:
            if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* no case break */
        case CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_0_E:
        case CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_1_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* no case break */
        case CPSS_DXCH_CNC_CLIENT_TTI_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
                *bitOffPtr = 4;
            }
            else if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
                *bitOffPtr = 4;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
                *bitOffPtr = 4;
            }
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_TRG_EPORT_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr =
                    PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
                *bitOffPtr = 2;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_SRC_EPORT_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).countingModeConfig;
                *bitOffPtr = 5;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_DXCH_CNC_CLIENT_PACKET_TYPE_PASS_DROP_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                *regAddrPtr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                    preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
                *bitOffPtr = 13;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        /* L3/L2 mode not relevant for CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E client */
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChCncBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
{
    return cpssDxChCncPortGroupBlockClientEnableSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, client, updateEnable);
}

/*******************************************************************************
* cpssDxChCncBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncBlockClientEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, client, updateEnable));

    rc = internal_cpssDxChCncBlockClientEnableSet(devNum, blockNum, client, updateEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, client, updateEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
    return cpssDxChCncPortGroupBlockClientEnableGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, client, updateEnablePtr);
}

/*******************************************************************************
* cpssDxChCncBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncBlockClientEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, client, updateEnablePtr));

    rc = internal_cpssDxChCncBlockClientEnableGet(devNum, blockNum, client, updateEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, client, updateEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*       indexRangesBmp   - the counter index ranges bitmap
*                         DxCh3, xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
)
{
    return cpssDxChCncPortGroupBlockClientRangesSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, client, indexRangesBmp);
}

/*******************************************************************************
* cpssDxChCncBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*       indexRangesBmp   - the counter index ranges bitmap
*                         DxCh3, xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncBlockClientRangesSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, client, indexRangesBmp));

    rc = internal_cpssDxChCncBlockClientRangesSet(devNum, blockNum, client, indexRangesBmp);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, client, indexRangesBmp));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) the counter index ranges bitmap
*                         DxCh3, xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U64                    *indexRangesBmpPtr
)
{
    return cpssDxChCncPortGroupBlockClientRangesGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, client, indexRangesBmpPtr);
}

/*******************************************************************************
* cpssDxChCncBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) the counter index ranges bitmap
*                         DxCh3, xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U64                    *indexRangesBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncBlockClientRangesGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, client, indexRangesBmpPtr));

    rc = internal_cpssDxChCncBlockClientRangesGet(devNum, blockNum, client, indexRangesBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, client, indexRangesBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortClientEnableSet
*
* DESCRIPTION:
*   The function sets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_PHYSICAL_PORT_NUM      portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   enable
)
{
    GT_U32 regAddr;          /* register address  */
    GT_U32 fieldOffset;      /* field Offset      */
    GT_U32 value;            /* field value       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

        portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).L2L3IngrVLANCountingEnable[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncl2l3IngressVlanCountEnable;
        fieldOffset = (localPort != CPSS_CPU_PORT_NUM_CNS) ? localPort : 31;
    }

    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_INGRESS_SRC_EPORT_E:
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* no break; */
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E: break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    value = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssHwPpPortGroupSetRegField(
        devNum, portGroupId ,regAddr, fieldOffset, 1/*fieldLength*/, value);
}

/*******************************************************************************
* cpssDxChCncPortClientEnableSet
*
* DESCRIPTION:
*   The function sets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_PHYSICAL_PORT_NUM      portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortClientEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, client, enable));

    rc = internal_cpssDxChCncPortClientEnableSet(devNum, portNum, client, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, client, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortClientEnableGet
*
* DESCRIPTION:
*   The function gets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PHYSICAL_PORT_NUM      portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *enablePtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    regAddr;          /* register address  */
    GT_U32    fieldOffset;      /* field Offset      */
    GT_U32    value;            /* field value       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

        portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).L2L3IngrVLANCountingEnable[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncl2l3IngressVlanCountEnable;
        fieldOffset = (localPort != CPSS_CPU_PORT_NUM_CNS) ? localPort : 31;
    }

    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_INGRESS_SRC_EPORT_E:
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* no break; */
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E: break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpPortGroupGetRegField(
        devNum, portGroupId , regAddr, fieldOffset, 1/*fieldLength*/, &value);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortClientEnableGet
*
* DESCRIPTION:
*   The function gets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PHYSICAL_PORT_NUM      portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortClientEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, client, enablePtr));

    rc = internal_cpssDxChCncPortClientEnableGet(devNum, portNum, client, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, client, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
*
* DESCRIPTION:
*   The function enables or disables counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_U32 data;             /* data              */
    GT_U32 regAddr;          /* register address  */
    GT_U32 fieldOffset;      /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
        fieldOffset = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
        fieldOffset = 15;
    }

    data = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssHwPpSetRegField(
        devNum, regAddr, fieldOffset, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
*
* DESCRIPTION:
*   The function enables or disables counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
*
* DESCRIPTION:
*   The function gets status of counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    data;             /* data              */
    GT_U32    regAddr;          /* register address  */
    GT_U32 fieldOffset;         /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
        fieldOffset = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
        fieldOffset = 15;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, fieldOffset, 1/*fieldLength*/, &data);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
*
* DESCRIPTION:
*   The function gets status of counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncVlanClientIndexModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode
*   for the Vlan CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanClient         - VLAN client
*                            relevant clients:
*                            CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E
*                            CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E
*       indexMode          - index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncVlanClientIndexModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          vlanClient,
    IN  CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT indexMode
)
{
    GT_U32 data;             /* data               */
    GT_U32 regAddr;          /* register address   */
    GT_U32 offset;           /* offset in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (vlanClient)
    {
        case CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E:
            regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                    preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
            offset = 17;
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E:
            regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.CNCModes.CNCModes;
            offset = 10;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (indexMode)
    {
        case CPSS_DXCH_CNC_VLAN_INDEX_MODE_ORIGINAL_VID_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_VLAN_INDEX_MODE_EVID_E:
            data = 1;
            break;
        case CPSS_DXCH_CNC_VLAN_INDEX_MODE_TAG1_VID_E:
            data = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(
        devNum, regAddr, offset, 2/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncVlanClientIndexModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode
*   for the Vlan CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanClient         - VLAN client
*                            relevant clients:
*                            CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E
*                            CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E
*       indexMode          - index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncVlanClientIndexModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          vlanClient,
    IN  CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT indexMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncVlanClientIndexModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanClient, indexMode));

    rc = internal_cpssDxChCncVlanClientIndexModeSet(devNum, vlanClient, indexMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanClient, indexMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncVlanClientIndexModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode
*   for the Vlan CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanClient         - VLAN client
*                            relevant clients:
*                            CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E
*                            CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E
*
* OUTPUTS:
*       indexModePtr       - (pointer to)index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - enexpected HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncVlanClientIndexModeGet
(
    IN   GT_U8                             devNum,
    IN   CPSS_DXCH_CNC_CLIENT_ENT          vlanClient,
    OUT  CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT *indexModePtr
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */
    GT_U32    offset;           /* offset in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(indexModePtr);

    switch (vlanClient)
    {
        case CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E:
            regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                    preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
            offset = 17;
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E:
            regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.CNCModes.CNCModes;
            offset = 10;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, offset, 2/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (data)
    {
        case 0:
             *indexModePtr = CPSS_DXCH_CNC_VLAN_INDEX_MODE_ORIGINAL_VID_E;
            break;
        case 1:
            *indexModePtr = CPSS_DXCH_CNC_VLAN_INDEX_MODE_EVID_E;
            break;
        case 2:
            *indexModePtr = CPSS_DXCH_CNC_VLAN_INDEX_MODE_TAG1_VID_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncVlanClientIndexModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode
*   for the Vlan CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanClient         - VLAN client
*                            relevant clients:
*                            CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E
*                            CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E
*
* OUTPUTS:
*       indexModePtr       - (pointer to)index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - enexpected HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncVlanClientIndexModeGet
(
    IN   GT_U8                             devNum,
    IN   CPSS_DXCH_CNC_CLIENT_ENT          vlanClient,
    OUT  CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT *indexModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncVlanClientIndexModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vlanClient, indexModePtr));

    rc = internal_cpssDxChCncVlanClientIndexModeGet(devNum, vlanClient, indexModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vlanClient, indexModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPacketTypePassDropToCpuModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode of To CPU packets
*   for the Packet Type Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       toCpuMode        - to CPU packets index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPacketTypePassDropToCpuModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT toCpuMode
)
{
    GT_U32 data;             /* data              */
    GT_U32 regAddr;          /* register address  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;

    switch (toCpuMode)
    {
        case CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_CPU_CODE_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_SRC_PORT_E:
            data = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 16/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncPacketTypePassDropToCpuModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode of To CPU packets
*   for the Packet Type Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       toCpuMode        - to CPU packets index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPacketTypePassDropToCpuModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT toCpuMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPacketTypePassDropToCpuModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, toCpuMode));

    rc = internal_cpssDxChCncPacketTypePassDropToCpuModeSet(devNum, toCpuMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, toCpuMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPacketTypePassDropToCpuModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode of To CPU packets
*   for the Packet Type Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       toCpuModePtr        - to CPU packets index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPacketTypePassDropToCpuModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT *toCpuModePtr
)
{
    GT_STATUS rc;            /* return code       */
    GT_U32    data;          /* data              */
    GT_U32    regAddr;       /* register address  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(toCpuModePtr);

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 16/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *toCpuModePtr =
        (data == 0)
            ? CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_CPU_CODE_E
            : CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_SRC_PORT_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPacketTypePassDropToCpuModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode of To CPU packets
*   for the Packet Type Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       toCpuModePtr        - to CPU packets index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPacketTypePassDropToCpuModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT *toCpuModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPacketTypePassDropToCpuModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, toCpuModePtr));

    rc = internal_cpssDxChCncPacketTypePassDropToCpuModeGet(devNum, toCpuModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, toCpuModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncTmClientIndexModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode
*   for the Traffic manager CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2, Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       indexMode          - index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncTmClientIndexModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_CNC_TM_INDEX_MODE_ENT indexMode
)
{
    GT_U32 data;             /* data              */
    GT_U32 regAddr;          /* register address  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_TM_DROP_MAC(devNum).globalConfigs;

    switch (indexMode)
    {
        case CPSS_DXCH_CNC_TM_INDEX_MODE_0_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_TM_INDEX_MODE_1_E:
            data = 1;
            break;
        case CPSS_DXCH_CNC_TM_INDEX_MODE_2_E:
            data = 2;
            break;
        case CPSS_DXCH_CNC_TM_INDEX_MODE_3_E:
            data = 3;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 0/*fieldOffset*/, 2/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncTmClientIndexModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode
*   for the Traffic manager CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2, Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2, Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       indexMode          - index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncTmClientIndexModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_CNC_TM_INDEX_MODE_ENT indexMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncTmClientIndexModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, indexMode));

    rc = internal_cpssDxChCncTmClientIndexModeSet(devNum, indexMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, indexMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncTmClientIndexModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode
*   for the Traffic manager CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2, Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       indexModePtr       - (pointer to)index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncTmClientIndexModeGet
(
    IN   GT_U8                           devNum,
    OUT  CPSS_DXCH_CNC_TM_INDEX_MODE_ENT *indexModePtr
)
{
    GT_STATUS rc;            /* return code       */
    GT_U32    data;          /* data              */
    GT_U32    regAddr;       /* register address  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(indexModePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TM_DROP_MAC(devNum).globalConfigs;

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 0/*fieldOffset*/, 2/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (data)
    {
        case 0:
            *indexModePtr = CPSS_DXCH_CNC_TM_INDEX_MODE_0_E;
            break;
        case 1:
            *indexModePtr = CPSS_DXCH_CNC_TM_INDEX_MODE_1_E;
            break;
        case 2:
            *indexModePtr = CPSS_DXCH_CNC_TM_INDEX_MODE_2_E;
            break;
        case 3:
            *indexModePtr = CPSS_DXCH_CNC_TM_INDEX_MODE_3_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /*never occurs*/
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncTmClientIndexModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode
*   for the Traffic manager CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2, Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2, Bobcat3..
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       indexModePtr       - (pointer to)index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncTmClientIndexModeGet
(
    IN   GT_U8                           devNum,
    OUT  CPSS_DXCH_CNC_TM_INDEX_MODE_ENT *indexModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncTmClientIndexModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, indexModePtr));

    rc = internal_cpssDxChCncTmClientIndexModeGet(devNum, indexModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, indexModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncClientByteCountModeSet
*
* DESCRIPTION:
*   The function sets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*       countMode        - count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncClientByteCountModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    IN  CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */
    GT_U32    bitOff;           /* offset of mode bit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    rc = prvCpssDxChCncClientCountModeRegGet(
        devNum, client, &regAddr, &bitOff);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (countMode)
    {
        case CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E: data = 0; break;
        case CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E: data = 1; break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(
        devNum, regAddr, bitOff, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncClientByteCountModeSet
*
* DESCRIPTION:
*   The function sets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*       countMode        - count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncClientByteCountModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    IN  CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncClientByteCountModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, client, countMode));

    rc = internal_cpssDxChCncClientByteCountModeSet(devNum, client, countMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, client, countMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncClientByteCountModeGet
*
* DESCRIPTION:
*   The function gets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*
* OUTPUTS:
*       countModePtr     - (pointer to) count mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncClientByteCountModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    OUT CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT *countModePtr
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */
    GT_U32    bitOff;           /* offset of mode bit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(countModePtr);

    rc = prvCpssDxChCncClientCountModeRegGet(
        devNum, client, &regAddr, &bitOff);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, bitOff, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *countModePtr = (data == 0)
        ? CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E
        : CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncClientByteCountModeGet
*
* DESCRIPTION:
*   The function gets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*
* OUTPUTS:
*       countModePtr     - (pointer to) count mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncClientByteCountModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    OUT CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT *countModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncClientByteCountModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, client, countModePtr));

    rc = internal_cpssDxChCncClientByteCountModeGet(devNum, client, countModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, client, countModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncEgressVlanDropCountModeSet
*
* DESCRIPTION:
*   The function sets Egress VLAN Drop counting mode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       mode             - Egress VLAN Drop counting mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncEgressVlanDropCountModeSet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    rc = prvCpssDxChCncClientCountModeRegGet(
           devNum, CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E, &regAddr, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (mode)
    {
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E:
            data = 1;
            break;
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E:
            data = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 4/*fieldOffset*/, 2/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeSet
*
* DESCRIPTION:
*   The function sets Egress VLAN Drop counting mode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       mode             - Egress VLAN Drop counting mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressVlanDropCountModeSet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncEgressVlanDropCountModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChCncEgressVlanDropCountModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncEgressVlanDropCountModeGet
*
* DESCRIPTION:
*   The function gets Egress VLAN Drop counting mode.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       modePtr             - (pointer to) Egress VLAN Drop counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - on reserved value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncEgressVlanDropCountModeGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT *modePtr
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    rc = prvCpssDxChCncClientCountModeRegGet(
           devNum, CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E, &regAddr, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 4/*fieldOffset*/, 2/*fieldLength*/, &data);

    switch (data)
    {
        case 0:
            *modePtr =
                CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;
            break;
        case 1:
            *modePtr =
                CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E;
            break;
        case 2:
            *modePtr =CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeGet
*
* DESCRIPTION:
*   The function gets Egress VLAN Drop counting mode.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       modePtr             - (pointer to) Egress VLAN Drop counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - on reserved value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressVlanDropCountModeGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncEgressVlanDropCountModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChCncEgressVlanDropCountModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterClearByReadEnableSet
*
* DESCRIPTION:
*   The function enable/disables clear by read mode of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterClearByReadEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */
    GT_U32    i;                /* loop index         */
    GT_STATUS rc;               /* return code        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    data = (enable == GT_FALSE) ? 0 : 1;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2; Caelum; Bobcat3 */
        for (i = 0; (i < PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncUnits); i++)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, i).globalRegs.CNCGlobalConfigReg;
            rc = prvCpssHwPpSetRegField(
                devNum, regAddr, 1/*fieldOffset*/, 1/*fieldLength*/, data);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }

    /* Not eArch devices */

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 1/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableSet
*
* DESCRIPTION:
*   The function enable/disables clear by read mode of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterClearByReadEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChCncCounterClearByReadEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterClearByReadEnableGet
*
* DESCRIPTION:
*   The function gets clear by read mode status of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterClearByReadEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    regAddr;          /* register address  */
    GT_U32    data;             /* field value       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, 0).globalRegs.CNCGlobalConfigReg;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 1/*fieldOffset*/, 1/*fieldLength*/, &data);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableGet
*
* DESCRIPTION:
*   The function gets clear by read mode status of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterClearByReadEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChCncCounterClearByReadEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterClearByReadValueSet
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterClearByReadValueSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_U32    regAddr;          /* register address           */
    GT_U32    i;                /* loop index         */
    GT_STATUS rc;               /* return code        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2; Caelum; Bobcat3 */
        for (i = 0; (i < PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncUnits); i++)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, i).globalRegs.CNCClearByReadValueRegWord0;
            rc = prvCpssDxChCncPortGroupCounterValueAverageSet(
                devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                regAddr, format, counterPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }

    /* Not eArch devices */

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.clearByReadValueWord0;

    return prvCpssDxChCncPortGroupCounterValueAverageSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueSet
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadValueSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterClearByReadValueSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, format, counterPtr));

    rc = internal_cpssDxChCncCounterClearByReadValueSet(devNum, format, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, format, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterClearByReadValueGet
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterClearByReadValueGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_U32    regAddr;          /* register address  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, 0).globalRegs.CNCClearByReadValueRegWord0;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.clearByReadValueWord0;
    }

    return prvCpssDxChCncPortGroupCounterValueSummingGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueGet
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadValueGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterClearByReadValueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, format, counterPtr));

    rc = internal_cpssDxChCncCounterClearByReadValueGet(devNum, format, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, format, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterWraparoundEnableSet
*
* DESCRIPTION:
*   The function enables/disables wraparound for all CNC counters
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterWraparoundEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */
    GT_U32    i;                /* loop index         */
    GT_STATUS rc;               /* return code        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    data = (enable == GT_FALSE) ? 0 : 1;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2; Caelum; Bobcat3 */
        for (i = 0; (i < PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncUnits); i++)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, i).globalRegs.CNCGlobalConfigReg;
            rc = prvCpssHwPpSetRegField(
                devNum, regAddr, 0/*fieldOffset*/, 1/*fieldLength*/, data);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }

    /* Not eArch devices */

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 0/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableSet
*
* DESCRIPTION:
*   The function enables/disables wraparound for all CNC counters
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterWraparoundEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChCncCounterWraparoundEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterWraparoundEnableGet
*
* DESCRIPTION:
*   The function gets status of wraparound for all CNC counters
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterWraparoundEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    regAddr;          /* register address  */
    GT_U32    data;             /* field value       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, 0).globalRegs.CNCGlobalConfigReg;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 0/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableGet
*
* DESCRIPTION:
*   The function gets status of wraparound for all CNC counters
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterWraparoundEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChCncCounterWraparoundEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       blockNum     - CNC block number
*                      valid range see in datasheet of specific device.
*       indexNumPtr  - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr  - (pointer to) actual size of array of indexes
*       indexesArr[] - (pointer to) array of indexes of counters wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterWraparoundIndexesGet
(
    IN    GT_U8    devNum,
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
)
{
    return cpssDxChCncPortGroupCounterWraparoundIndexesGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, indexNumPtr,
        NULL /*portGroupIdArr*/, indexesArr);
}

/*******************************************************************************
* cpssDxChCncCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       blockNum     - CNC block number
*                      valid range see in datasheet of specific device.
*       indexNumPtr  - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr  - (pointer to) actual size of array of indexes
*       indexesArr[] - (pointer to) array of indexes of counters wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundIndexesGet
(
    IN    GT_U8    devNum,
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterWraparoundIndexesGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, indexNumPtr, indexesArr));

    rc = internal_cpssDxChCncCounterWraparoundIndexesGet(devNum, blockNum, indexNumPtr, indexesArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, indexNumPtr, indexesArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    return cpssDxChCncPortGroupCounterSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, index, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, index, format, counterPtr));

    rc = internal_cpssDxChCncCounterSet(devNum, blockNum, index, format, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, index, format, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    return cpssDxChCncPortGroupCounterGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, index, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, index, format, counterPtr));

    rc = internal_cpssDxChCncCounterGet(devNum, blockNum, index, format, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, index, format, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
)
{
    return cpssDxChCncPortGroupBlockUploadTrigger(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, blockNum);
}

/*******************************************************************************
* cpssDxChCncBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncBlockUploadTrigger);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum));

    rc = internal_cpssDxChCncBlockUploadTrigger(devNum, blockNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                     *inProcessBlocksBmpPtr
)
{
    return cpssDxChCncPortGroupBlockUploadInProcessGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, inProcessBlocksBmpPtr);
}

/*******************************************************************************
* cpssDxChCncBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                     *inProcessBlocksBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncBlockUploadInProcessGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, inProcessBlocksBmpPtr));

    rc = internal_cpssDxChCncBlockUploadInProcessGet(devNum, inProcessBlocksBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, inProcessBlocksBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       portGroupId                - the port group Id. to support multi-port-groups device.
*       initOutCounters       - GT_TRUE - override the memory contents
*                             - GT_FALSE - add to memory contents
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_NO_MORE            - the action succeeded and there are no more waiting
*                               CNC counter value
*       GT_NOT_READY          - Upload started after upload trigger or
*                               continued after queue rewind but yet
*                               not paused due to queue full and yet not finished.
*                               The part of queue memory does not contain uploaded counters yet.
*                               No counters retrieved.
*       GT_FAIL               - on failure
*       GT_BAD_PARAM          - on wrong devNum
*       GT_BAD_PTR            - one of the parameters is NULL pointer
*       GT_BAD_STATE          - if the previous FU messages were not
*                               retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
static GT_STATUS cncUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_U32                            portGroupId,
    IN     GT_BOOL                           initOutCounters,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    GT_STATUS                   rc;            /* return code                   */
    PRV_CPSS_AU_DESC_STC        *descPtr;    /* pointer to the current descriptor */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;  /* pointer to the descriptors DB
                                                 of the device                  */
    GT_U32                     *auMemPtr;/* pointer to the SDMA memory , to start of current message */

    GT_U32                      ii;            /* iterator                      */
    GT_U32                      cncMaxCnt;     /* number of CNC messages        */
    GT_U32                      word[2];       /* work data                     */
    CPSS_DXCH_CNC_COUNTER_STC   cuppPortGroupCounter;/* current counter value for multi-port-groups support */
    GT_U32                      auMessageNumWords;  /* number of words in AU/FU message */
    GT_U32                      auMessageNumBytes;  /* number of bytes in AU/FU message */
    GT_U32                      firstCncWordIdx;    /* AU memory index of current CNC counter's first word */
    GT_PORT_GROUPS_BMP          portGroupsBmp; /* check bitmap of port groups */
    GT_PORT_GROUPS_BMP          fullPortGroupsBmp; /* full queue bitmap of port groups */
    GT_U32                      inProcessBlocksBmp; /* bitmap of blocks being uploaded */

    /* check that the FUQ initialized */
    PRV_CPSS_FUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

    auMessageNumWords = PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.auMessageNumOfWords;
    auMessageNumBytes = 4 * auMessageNumWords;

    descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);

    /* the pointer to start of 'next message to handle'  */
    auMemPtr = (GT_U32 *)(descCtrlPtr->blockAddr + (auMessageNumBytes * descCtrlPtr->currDescIdx));

    cncMaxCnt = *numOfCounterValuesPtr;

    *numOfCounterValuesPtr = 0;

    portGroupsBmp =
        PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum)
        ? (GT_U32)(1 << portGroupId) : CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    rc = cpssDxChBrgFdbPortGroupQueueFullGet(
        devNum, portGroupsBmp, CPSS_DXCH_FDB_QUEUE_TYPE_FU_E, &fullPortGroupsBmp);
    if (rc != GT_OK)
    {
        return rc;
    }
    if ((fullPortGroupsBmp == CPSS_PORT_GROUP_UNAWARE_MODE_CNS) ||
        (portGroupsBmp & fullPortGroupsBmp) != 0)
    {
        /* upload paused due to queue is full                  */
        /* queue contains ready portion of counters to retrieve */
    }
    else
    {
        /* queue is not full. Check if upload finished */
        rc = cpssDxChCncPortGroupBlockUploadInProcessGet(
            devNum, portGroupsBmp, &inProcessBlocksBmp);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (inProcessBlocksBmp != 0)
        {
            /* block upload yet not finished and not paused */
            /* the queue contents yet not valid             */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
        }
    }

    /* read number of AU descriptors */
    for (ii = 0; (ii < cncMaxCnt); ii++)
    {
        if (descCtrlPtr->unreadCncCounters == 0)
        {
            *numOfCounterValuesPtr = ii;
            return /* it's not error for log */ GT_NO_MORE;
        }

        if(auMessageNumWords == 8)
        {
            /* The 8 word AU message includes 4 CNC counters.
              Each CNC counter use two 32 bit words.
              The descCtrlPtr->unreadCncCounters is count down variable
              that starts from number of CNC counters in a block (2048, 512 or 256).
              First uploaded counter has descCtrlPtr->unreadCncCounters % 4 = 0.
              Second one has descCtrlPtr->unreadCncCounters % 4 = 3.
              Third one has descCtrlPtr->unreadCncCounters % 4 = 2.
              Fourth one has descCtrlPtr->unreadCncCounters % 4 = 1.
              Use simple equation to get index of current counter in AU message:
               - index = (4 - (descCtrlPtr->unreadCncCounters % 4)) % 4  */
            firstCncWordIdx = ((4 - (descCtrlPtr->unreadCncCounters & 3)) & 3) * 2;
        }
        else
        {
            /* The 4 word AU message includes 2 CNC counters.
              Each CNC counter use two 32 bit words.
              First uploaded counter has even value of descCtrlPtr->unreadCncCounters.
              Second one has odd value of descCtrlPtr->unreadCncCounters. */
            firstCncWordIdx = (descCtrlPtr->unreadCncCounters & 1) * 2;
        }

        if (initOutCounters != GT_FALSE)
        {
            /* start with clean counter value */
            cpssOsMemSet(
                &(counterValuesPtr[ii]), 0,
                sizeof(CPSS_DXCH_CNC_COUNTER_STC));
        }

        /* swap HW values to get correct endian order of the CNC counter */
        word[0] = CPSS_32BIT_LE(auMemPtr[firstCncWordIdx]);
        firstCncWordIdx++;
        word[1] = CPSS_32BIT_LE(auMemPtr[firstCncWordIdx]);

        rc = prvCpssDxChCncCounterHwToSw(
            format, word, &cuppPortGroupCounter);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* summary the counter from all the port groups */
        counterValuesPtr[ii].byteCount =
            prvCpssMathAdd64(counterValuesPtr[ii].byteCount,
                cuppPortGroupCounter.byteCount);

        counterValuesPtr[ii].packetCount =
            prvCpssMathAdd64(counterValuesPtr[ii].packetCount,
                cuppPortGroupCounter.packetCount);

        descCtrlPtr->unreadCncCounters --;

        /* check if last CNC counters of AU message was read by check
           number of last AU word that was read - firstCncWordIdx. */
        if (firstCncWordIdx < (auMessageNumWords - 1))
        {
            /* this is not last CNC counter in a AU message.
               need to read next CNC counter in current AU message memory. */
            continue;
        }

        /* erase memory to be compatible with FU messages upload.
           point descPtr to the current AU descriptor in the queue */
        descPtr = (PRV_CPSS_AU_DESC_STC*)auMemPtr;
        AU_DESC_RESET_MAC(descPtr);

        /* move to next AU message memory */
        descCtrlPtr->currDescIdx = ((descCtrlPtr->currDescIdx + 1) %
                                   descCtrlPtr->blockSize);
        auMemPtr = (GT_U32 *)(descCtrlPtr->blockAddr + (auMessageNumBytes * descCtrlPtr->currDescIdx));

        /* reached the end of AU block */
        if (descCtrlPtr->currDescIdx == 0)
        {
            rc = prvCpssDxChFullQueueRewind(
                devNum, portGroupId, GT_TRUE /*isFuQueue*/, NULL /*appDescCtrlPtr*/);
            /* ii yet not incremenred by loop header */
            *numOfCounterValuesPtr = (ii + 1);
            /* on succesfull rewind GT_OK */
            return rc;
        }
    }

    *numOfCounterValuesPtr = cncMaxCnt;
    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChCncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  CNC counter value
*       GT_NOT_READY             - Upload started after upload trigger or
*                                  continued after queue rewind but yet
*                                  not paused due to queue full and yet not finished.
*                                  The part of queue memory does not contain uploaded counters yet.
*                                  No counters retrieved.
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                                  retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncUploadedBlockGet
(
    IN     GT_U8                             devNum,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    return cpssDxChCncPortGroupUploadedBlockGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        numOfCounterValuesPtr, format, counterValuesPtr);
}

/*******************************************************************************
* cpssDxChCncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  CNC counter value
*       GT_NOT_READY             - Upload started after upload trigger or
*                                  continued after queue rewind but yet
*                                  not paused due to queue full and yet not finished.
*                                  The part of queue memory does not contain uploaded counters yet.
*                                  No counters retrieved.
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                                  retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncUploadedBlockGet
(
    IN     GT_U8                             devNum,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncUploadedBlockGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, numOfCounterValuesPtr, format, counterValuesPtr));

    rc = internal_cpssDxChCncUploadedBlockGet(devNum, numOfCounterValuesPtr, format, counterValuesPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, numOfCounterValuesPtr, format, counterValuesPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCountingEnableSet
*
* DESCRIPTION:
*   The function enables counting on selected cnc unit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*       enable  - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCountingEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    IN  GT_BOOL                                 enable
)
{
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */
    GT_U32 bitOffset;   /* bit offset in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    switch (cncUnit)
    {
        case CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
            }
            bitOffset = 3;
            break;
        case CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).countingModeConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            }
            bitOffset = 4;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    data = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssHwPpSetRegField(
        devNum, regAddr, bitOffset/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncCountingEnableSet
*
* DESCRIPTION:
*   The function enables counting on selected cnc unit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*       enable  - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCountingEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCountingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cncUnit, enable));

    rc = internal_cpssDxChCncCountingEnableSet(devNum, cncUnit, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cncUnit, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCountingEnableGet
*
* DESCRIPTION:
*   The function gets enable counting status on selected cnc unit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable
*                     GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCountingEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS rc;       /* return code            */
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */
    GT_U32 bitOffset;   /* bit offset in register */

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    switch (cncUnit)
    {
        case CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
            }
            bitOffset = 3;
            break;
        case CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).countingModeConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            }
            bitOffset = 4;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, bitOffset/*fieldOffset*/, 1/*fieldLength*/, &data);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCountingEnableGet
*
* DESCRIPTION:
*   The function gets enable counting status on selected cnc unit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable
*                     GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCountingEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCountingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cncUnit, enablePtr));

    rc = internal_cpssDxChCncCountingEnableGet(devNum, cncUnit, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cncUnit, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
{
    return cpssDxChCncPortGroupCounterFormatSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, format);
}

/*******************************************************************************
* cpssDxChCncCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterFormatSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, format));

    rc = internal_cpssDxChCncCounterFormatSet(devNum, blockNum, format);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, format));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
{
    return cpssDxChCncPortGroupCounterFormatGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, formatPtr);
}

/*******************************************************************************
* cpssDxChCncCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCounterFormatGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, blockNum, formatPtr));

    rc = internal_cpssDxChCncCounterFormatGet(devNum, blockNum, formatPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, blockNum, formatPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncEgressQueueClientModeSet
*
* DESCRIPTION:
*   The function sets Egress Queue client counting mode
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number
*       mode            - Egress Queue client counting mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncEgressQueueClientModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT mode
)
{
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.CNCModes.CNCModes;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.peripheralAccess.cncModes.modesRegister;
    }

    switch (mode)
    {
        case CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E:
            data = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 8/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncEgressQueueClientModeSet
*
* DESCRIPTION:
*   The function sets Egress Queue client counting mode
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number
*       mode            - Egress Queue client counting mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressQueueClientModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncEgressQueueClientModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChCncEgressQueueClientModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncEgressQueueClientModeGet
*
* DESCRIPTION:
*   The function gets Egress Queue client counting mode
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       modePtr         - (pointer to) Egress Queue client counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncEgressQueueClientModeGet
(
    IN  GT_U8                                      devNum,
    OUT CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT *modePtr
)
{
    GT_STATUS rc;       /* return code            */
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).peripheralAccess.CNCModes.CNCModes;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.peripheralAccess.cncModes.modesRegister;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 8/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr =
        (data == 0)
            ? CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E
            : CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncEgressQueueClientModeGet
*
* DESCRIPTION:
*   The function gets Egress Queue client counting mode
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       modePtr         - (pointer to) Egress Queue client counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressQueueClientModeGet
(
    IN  GT_U8                                      devNum,
    OUT CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncEgressQueueClientModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChCncEgressQueueClientModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCpuAccessStrictPriorityEnableSet
*
* DESCRIPTION:
*   The function enables strict priority of CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       enable          - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCpuAccessStrictPriorityEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */
    GT_U32    i;                /* loop index         */
    GT_STATUS rc;               /* return code        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    data = (enable == GT_FALSE) ? 0 : 1;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2; Caelum; Bobcat3 */
        for (i = 0; (i < PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncUnits); i++)
        {
            regAddr =
                PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, i).globalRegs.CNCGlobalConfigReg;
            rc = prvCpssHwPpSetRegField(
                devNum, regAddr, 10/*fieldOffset*/, 1/*fieldLength*/, data);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }

    /* Not eArch devices */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    return prvCpssHwPpSetRegField(
        devNum, regAddr, 10/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncCpuAccessStrictPriorityEnableSet
*
* DESCRIPTION:
*   The function enables strict priority of CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       enable          - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCpuAccessStrictPriorityEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCpuAccessStrictPriorityEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChCncCpuAccessStrictPriorityEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncCpuAccessStrictPriorityEnableGet
*
* DESCRIPTION:
*   The function gets enable status of strict priority of
*   CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncCpuAccessStrictPriorityEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;       /* return code            */
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, 0).globalRegs.CNCGlobalConfigReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;
    }

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 10/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCpuAccessStrictPriorityEnableGet
*
* DESCRIPTION:
*   The function gets enable status of strict priority of
*   CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCpuAccessStrictPriorityEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncCpuAccessStrictPriorityEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChCncCpuAccessStrictPriorityEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
{
    GT_STATUS   rc;           /* return code                  */
    GT_U32      data;         /* data                         */
    GT_U32      regAddr;      /* register address             */
    GT_U32      fieldOffset;  /* field Offset                 */
    GT_U32      portGroupId;  /* the port group Id            */
    GT_U32      mask;         /* field bit mask               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        if (prvCpssDxChCncEArchBlockClientToCfgAddr(
            devNum, client, blockNum, &regAddr, &mask) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        data = (updateEnable == GT_FALSE) ? 0 : mask;

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssHwPpPortGroupWriteRegBitMask(
                devNum, portGroupId, regAddr, mask, data);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* Not eArch devices */

    if (prvCpssDxChCncBlockClientToCfgAddr(
        devNum, client, blockNum, &regAddr, &fieldOffset) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    data = (updateEnable == GT_FALSE) ? 0 : 1;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr,
            fieldOffset, 1/*fieldLength*/, data);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupBlockClientEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, client, updateEnable));

    rc = internal_cpssDxChCncPortGroupBlockClientEnableSet(devNum, portGroupsBmp, blockNum, client, updateEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, client, updateEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
    GT_STATUS rc;             /* return code        */
    GT_U32    data;           /* data               */
    GT_U32    regAddr;        /* register address   */
    GT_U32    fieldOffset;    /* field Offset       */
    GT_U32    portGroupId;    /* the port group Id  */
    GT_U32    mask;           /* field bit mask     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(updateEnablePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        if (prvCpssDxChCncEArchBlockClientToCfgAddr(
            devNum, client, blockNum, &regAddr, &mask) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssHwPpPortGroupReadRegister(
            devNum, portGroupId, regAddr, &data);
        if (rc != GT_OK)
        {
            return rc;
        }

        *updateEnablePtr = ((data & mask) == 0) ? GT_FALSE : GT_TRUE;

        return GT_OK;
    }

    /* Not eArch devices */
    if (prvCpssDxChCncBlockClientToCfgAddr(
        devNum, client, blockNum, &regAddr, &fieldOffset) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpPortGroupGetRegField(
        devNum, portGroupId,
        regAddr, fieldOffset, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *updateEnablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupBlockClientEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, client, updateEnablePtr));

    rc = internal_cpssDxChCncPortGroupBlockClientEnableGet(devNum, portGroupsBmp, blockNum, client, updateEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, client, updateEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*       indexRangesBmp   - bitmap of counter index ranges
*                         DxCh3, xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
)
{
    GT_STATUS rc;            /* return code                 */
    GT_U32    regAddr;       /* register address            */
    GT_U32    fieldOffset;   /* field Offset                */
    GT_U32    portGroupId;   /* the port group Id           */
    GT_U32    unitNum;       /* number of CNC unit          */
    GT_U32    blockInUnit;   /* number of block in CNC unit */
    GT_U32    i;             /* loop index                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        if (GT_OK != prvCpssDxChCncEArchBlockUnitGet(
            devNum, blockNum, &unitNum, &blockInUnit))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            for (i = 0; (i < 2); i++)
            {
                regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).perBlockRegs.
                    rangeCount.CNCBlockRangeCountEnable[i][blockInUnit];

                rc = prvCpssHwPpPortGroupWriteRegister(
                    devNum, portGroupId, regAddr, indexRangesBmp.l[i]);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* Not eArch devices */

    if (prvCpssDxChCncBlockClientToCfgAddr(
        devNum, client, blockNum, &regAddr, &fieldOffset) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (! PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* DxCh3, xCat, xCat3 - 10 ranges of 2048 counters */

        /* check that indexRangesBmp does not contain bits 10-31 */
        if ((indexRangesBmp.l[0] > 0x3FF) || (indexRangesBmp.l[1] != 0))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr,
            (fieldOffset + 1), 10/*fieldLength*/, indexRangesBmp.l[0]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* Lion, xCat2, Lion2 - 64 ranges                   */
    /* The ranges mapped to this block for all clients  */

    /* The block configuration registers 1-2            */
    regAddr += 4;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupWriteRam(
        devNum, portGroupId, regAddr,
        2/*words*/, indexRangesBmp.l);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*       indexRangesBmp   - bitmap of counter index ranges
*                         DxCh3, xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupBlockClientRangesSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, client, indexRangesBmp));

    rc = internal_cpssDxChCncPortGroupBlockClientRangesSet(devNum, portGroupsBmp, blockNum, client, indexRangesBmp);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, client, indexRangesBmp));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap counter index ranges
*                         DxCh3, xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U64                    *indexRangesBmpPtr
)
{
    GT_STATUS rc;            /* return code                 */
    GT_U32  regAddr;         /* register address            */
    GT_U32  fieldOffset;     /* field Offset                */
    GT_U32  portGroupId;     /* the port group Id           */
    GT_U32  unitNum;         /* number of CNC unit          */
    GT_U32  blockInUnit;     /* number of block in CNC unit */
    GT_U32  i;               /* loop index                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(indexRangesBmpPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        if (GT_OK != prvCpssDxChCncEArchBlockUnitGet(
            devNum, blockNum, &unitNum, &blockInUnit))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        for (i = 0; (i < 2); i++)
        {
            regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).perBlockRegs.
                rangeCount.CNCBlockRangeCountEnable[i][blockInUnit];

            rc = prvCpssHwPpPortGroupReadRegister(
                devNum, portGroupId, regAddr, &(indexRangesBmpPtr->l[i]));
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }

    /* Not eArch devices */

    if (prvCpssDxChCncBlockClientToCfgAddr(
        devNum, client, blockNum, &regAddr, &fieldOffset) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (! PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* DxCh3, xCat, xCat3 - 10 ranges of 2048 counters */

        indexRangesBmpPtr->l[1] = 0;

        return prvCpssHwPpPortGroupGetRegField(
            devNum, portGroupId, regAddr,
            (fieldOffset + 1), 10/*fieldLength*/,
            &(indexRangesBmpPtr->l[0]));
    }

    /* Lion, xCat2, Lion2 - 64 ranges                   */
    /* The ranges mapped to this block for all clients  */

    /* The block configuration registers 1-2            */
    regAddr += 4;


    return prvCpssHwPpPortGroupReadRam(
        devNum, portGroupId, regAddr,
        2/*words*/, indexRangesBmpPtr->l);
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap counter index ranges
*                         DxCh3, xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U64                    *indexRangesBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupBlockClientRangesGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, client, indexRangesBmpPtr));

    rc = internal_cpssDxChCncPortGroupBlockClientRangesGet(devNum, portGroupsBmp, blockNum, client, indexRangesBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, client, indexRangesBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       indexNumPtr     - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr     - (pointer to) actual size of array of indexes
*       portGroupIdArr[]- (pointer to) array of port group Ids of
*                         counters wrapped around.
*                         The NULL pointer supported.
*       indexesArr[]    - (pointer to) array of indexes of counters
*                         wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupCounterWraparoundIndexesGet
(
    IN    GT_U8                   devNum,
    IN     GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN    GT_U32                  blockNum,
    INOUT GT_U32                  *indexNumPtr,
    OUT   GT_U32                  portGroupIdArr[],
    OUT   GT_U32                  indexesArr[]
)
{
    GT_STATUS rc;               /* return code                 */
    GT_U32    regAddr;          /* register address            */
    GT_U32    data;             /* HW data                     */
    GT_U32    i;                /* loop index                  */
    GT_U32    j;                /* loop index                  */
    GT_U32    maxIndexes;       /* index  array size           */
    GT_U32    portGroupId;      /*the port group Id            */
    GT_U32    unitNum;          /* number of CNC unit          */
    GT_U32    blockInUnit;      /* number of block in CNC unit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    CPSS_NULL_PTR_CHECK_MAC(indexNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(indexesArr);

    if (*indexNumPtr == 0)
    {
        return GT_OK;
    }

    maxIndexes = *indexNumPtr;

    *indexNumPtr = 0;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        if (GT_OK != prvCpssDxChCncEArchBlockUnitGet(
            devNum, blockNum, &unitNum, &blockInUnit))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            for (i = 0; (i < 4); i++)
            {
                regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).perBlockRegs.
                    wraparound.CNCBlockWraparoundStatusReg[blockInUnit][i];

                rc = prvCpssHwPpPortGroupReadRegister(
                    devNum, portGroupId, regAddr, &data);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (j = 0; (j < 2); j++, data = (data >> 16))
                {
                    if (data & 1)
                    {
                        indexesArr[*indexNumPtr] = (data >> 1) & 0x0FFF;
                        if (portGroupIdArr != NULL)
                        {
                            portGroupIdArr[*indexNumPtr] = portGroupId;
                        }

                        (*indexNumPtr) ++;

                        if (*indexNumPtr == maxIndexes)
                        {
                            return GT_OK;
                        }
                    }
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* Not eArch devices */

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockWraparoundStatusAddrBase
        + (CNC_BLOCK_CLIENT_CFG_ADDR_INCREMENT_CNS * blockNum);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        for (i = 0; (i < 4); i++)
        {
            rc = prvCpssHwPpPortGroupReadRegister(
                devNum, portGroupId, (regAddr + (i * 4)), &data);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (j = 0; (j < 2); j++, data = (data >> 16))
            {
                if (data & 1)
                {
                    indexesArr[*indexNumPtr] = (data >> 1) & 0x0FFF;
                    if (portGroupIdArr != NULL)
                    {
                        portGroupIdArr[*indexNumPtr] = portGroupId;
                    }

                    (*indexNumPtr) ++;

                    if (*indexNumPtr == maxIndexes)
                    {
                        return GT_OK;
                    }
                }
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       indexNumPtr     - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr     - (pointer to) actual size of array of indexes
*       portGroupIdArr[]- (pointer to) array of port group Ids of
*                         counters wrapped around.
*                         The NULL pointer supported.
*       indexesArr[]    - (pointer to) array of indexes of counters
*                         wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterWraparoundIndexesGet
(
    IN    GT_U8                   devNum,
    IN     GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN    GT_U32                  blockNum,
    INOUT GT_U32                  *indexNumPtr,
    OUT   GT_U32                  portGroupIdArr[],
    OUT   GT_U32                  indexesArr[]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupCounterWraparoundIndexesGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, indexNumPtr, portGroupIdArr, indexesArr));

    rc = internal_cpssDxChCncPortGroupCounterWraparoundIndexesGet(devNum, portGroupsBmp, blockNum, indexNumPtr, portGroupIdArr, indexesArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, indexNumPtr, portGroupIdArr, indexesArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_U32    regAddr;          /* register address           */
    GT_U32    unitNum;          /* number of CNC unit          */
    GT_U32    blockInUnit;      /* number of block in CNC unit */

    /* other parameters checked in called function */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    PRV_CPSS_DXCH_CNC_COUNTER_INDEX_VALID_CHECK_MAC(devNum, index);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        if (GT_OK != prvCpssDxChCncEArchBlockUnitGet(
            devNum, blockNum, &unitNum, &blockInUnit))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).perBlockRegs.
            memory.baseAddress[blockInUnit] + (index * 8);
    }
    else
    {
        /* Not eArch devices */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockCountersAddrBase
            + (blockNum * CNC_COUNTER_BLOCK_ADDR_INCREMENT_CNS) + (index * 8);
    }

    return prvCpssDxChCncPortGroupCounterValueAverageSet(
        devNum, portGroupsBmp,
        regAddr, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, index, format, counterPtr));

    rc = internal_cpssDxChCncPortGroupCounterSet(devNum, portGroupsBmp, blockNum, index, format, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, index, format, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_U32    regAddr;          /* register address  */
    GT_U32    unitNum;          /* number of CNC unit          */
    GT_U32    blockInUnit;      /* number of block in CNC unit */

    /* other parameters checked in called function */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    PRV_CPSS_DXCH_CNC_COUNTER_INDEX_VALID_CHECK_MAC(devNum, index);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        if (GT_OK != prvCpssDxChCncEArchBlockUnitGet(
            devNum, blockNum, &unitNum, &blockInUnit))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).perBlockRegs.
            memory.baseAddress[blockInUnit] + (index * 8);
    }
    else
    {
        /* Not eArch devices */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockCountersAddrBase
            + (blockNum * CNC_COUNTER_BLOCK_ADDR_INCREMENT_CNS) + (index * 8);
    }

    return prvCpssDxChCncPortGroupCounterValueSummingGet(
        devNum, portGroupsBmp,
        regAddr, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, index, format, counterPtr));

    rc = internal_cpssDxChCncPortGroupCounterGet(devNum, portGroupsBmp, blockNum, index, format, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, index, format, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChCncPortGroupBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncPortGroupBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum
)
{
    GT_STATUS              rc;               /* return code                       */
    GT_U32                 regAddr;          /* register address                  */
    GT_BOOL                actionCompleted;  /* Action Trigger Flag               */
    GT_BOOL                uploadEnable;     /* FU upload Enable                  */
    CPSS_FDB_ACTION_MODE_ENT    mode;        /* FDB action mode                   */
    PRV_CPSS_AU_DESC_STC        *descPtr;    /*pointer to the current descriptor */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;/* pointer to the descriptors DB
                                                 of the device                    */
    GT_U32                     *auMemPtr;/* pointer to the SDMA memory , to start of current message */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32                      auMessageNumBytes;
    GT_U32                      inProcessBlocksBmp; /* bitmap of being upload blocks */
    GT_U32                      unitNum = 0;        /* number of CNC unit            */
    GT_U32                      blockInUnit = 0;    /* number of block in CNC unit   */
    GT_U32                      pipeId;    /* support multi-pipe device */
    GT_U32                      fieldValue;/* field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    PRV_CNC_BLOCK_UPLOAD_SUPPORT_CHECK_MAC(devNum);

    /* the feartue needs separated AU and FU message queues */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    auMessageNumBytes = 4 * PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.auMessageNumOfWords;

    /* check that there is not not-finished FDB upload */
    rc = cpssDxChBrgFdbTrigActionStatusGet(devNum, &actionCompleted);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChBrgFdbUploadEnableGet(devNum, &uploadEnable);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChBrgFdbActionModeGet(devNum, &mode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((uploadEnable == GT_TRUE) && (actionCompleted == GT_FALSE)
        && (mode == CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    rc = cpssDxChCncPortGroupBlockUploadInProcessGet(
        devNum, portGroupsBmp, &inProcessBlocksBmp);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (inProcessBlocksBmp != 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* loop on all port groups to check the configuration */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* used separate FU queue if enabled */
        descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);

        /* check there are no FU messages in the queue */
        if (descCtrlPtr->unreadCncCounters == 0)
        {
            /* check that the FUQ initialized */
            PRV_CPSS_FUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

            /* the pointer to start of 'next message to handle'  */
            auMemPtr = (GT_U32 *)(descCtrlPtr->blockAddr + (auMessageNumBytes * descCtrlPtr->currDescIdx));

            /* pointer to the current "retrieved" descriptor */
            descPtr = (PRV_CPSS_AU_DESC_STC*)auMemPtr;
            /* found valid FU message in the queue */
            if (AU_DESC_IS_NOT_VALID(descPtr) != GT_TRUE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        if (GT_OK != prvCpssDxChCncEArchBlockUnitGet(
            devNum, blockNum, &unitNum, &blockInUnit))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }


        if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.cnc.cncUploadUseDfxUnit == GT_TRUE)
        {
            /* the CNC in bobcat3 must not be triggered in parallel on the 2 pipes
               because both pipe use the same MG unit with the same FUQ
            */
            CPSS_TBD_BOOKMARK_BOBCAT3

            regAddr =
                PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                              DFXServerUnitsBC2SpecificRegs.deviceCtrl16;

            /* bits 18,19,20 */
            /*20 rf_cnc_pipe_select */
            /*19 pipe1_rf_cnc_unit_dump_select */
            /*18 pipe0_rf_cnc_unit_dump_select */
            pipeId = 0;
            fieldValue = (unitNum << 0)/* pipe 0 */ |
                         (unitNum << 1)/* pipe 1 */ |
                         (pipeId  << 2);/* select the pipeId */

            rc = prvCpssDrvHwPpResetAndInitControllerSetRegField(
                devNum, regAddr,
                18/*fieldOffset*/, 3/*fieldLength*/, fieldValue);
        }
        else
        {
            /*<MG_IP> MG_IP/MG Registers/Global Control/General Configurations*/
            /* Bit 28 - CNC Dump Selector.Dumping is done from CNC0 / CNC1 */
            regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.generalConfigurations;

            rc = prvCpssHwPpSetRegField(
               devNum, regAddr, 28, 1, unitNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* loop on all port groups to trigger the upload */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* set indication (if needed) about FUQ used by CNC and not by the FDB */
        rc = prvCpssDxChFuqOwnerSet(devNum , portGroupId ,GT_TRUE/* CNC , not FDB */);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* increase amount of queued counters */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].unreadCncCounters +=
            PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlockNumEntries;

        /* trigger CNC counters upload */
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, unitNum).
                globalRegs.CNCFastDumpTriggerReg;

            /* the CNC in bobcat3 must not be triggered in parallel on the 2 pipes
               because both pipe use the same MG unit with the same FUQ
            */
            CPSS_TBD_BOOKMARK_BOBCAT3
            PRV_CPSS_MUST_DIRECT_CALL_TO_DRIVER_INDICATION_CNS

            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, portGroupId,regAddr,
                blockInUnit/*fieldOffset*/, 1/*fieldLength*/, 1);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockUploadTriggers;
            rc = prvCpssHwPpPortGroupSetRegField(
                devNum, portGroupId,regAddr,
                blockNum/*fieldOffset*/, 1/*fieldLength*/, 1);
        }
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncPortGroupBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupBlockUploadTrigger);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum));

    rc = internal_cpssDxChCncPortGroupBlockUploadTrigger(devNum, portGroupsBmp, blockNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          Checks all active port groups of the bitmap.
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    OUT GT_U32                      *inProcessBlocksBmpPtr
)
{
    GT_STATUS   rc;               /* return code       */
    GT_U32      regAddr;          /* register address  */
    GT_U32      regData;          /* register data     */
    /* support multi-port-groups device */
    GT_U32      portGroupId;      /* the port group Id */
    GT_U32      i;                /* loop index        */
    GT_U32      numDuplicatedUnits;     /* support duplicated units in the core */
    GT_PORT_GROUPS_BMP  tmp_portGroupsBmp;  /*temp port groups bmp */ /* support duplicated units in the core */
    GT_BOOL isAdditionalRegDupNeeded;   /* support duplicated units in the core */
    PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC  additionalRegDup; /* support duplicated units in the core */
    GT_U32  dupUnit;/* support duplicated units in the core */
    GT_U32  origRegAddr;/* support duplicated units in the core */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(inProcessBlocksBmpPtr);
    PRV_CNC_BLOCK_UPLOAD_SUPPORT_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2, Caelum, Bobcat3 */

        /* clear before accomulating */
        for (i = 0; (i < PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncUnits); i++)
        {
            if ((i & 1) == 0)
            {
                inProcessBlocksBmpPtr[i / 2] = 0;
            }
        }

        /* support duplications of the unit in the device (per core) */
        rc = prvCpssHwPpPortGroupNumDuplicatedUnitsGet(devNum,
                PRV_CPSS_DXCH_UNIT_CNC_0_E/*use CNC 0 as representative to all CNC units in single core*/,
                &numDuplicatedUnits);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            for(dupUnit = 0; dupUnit < (1 + numDuplicatedUnits) ; dupUnit++)
            {
                for (i = 0; (i < PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncUnits); i++)
                {
                    regAddr = PRV_DXCH_REG1_UNIT_CNC_MAC(devNum, i).
                        globalRegs.CNCFastDumpTriggerReg;

                    origRegAddr = regAddr;
                    if(dupUnit != 0 &&
                       GT_TRUE ==
                           prvCpssDuplicatedMultiPortGroupsGet(devNum,portGroupId,origRegAddr,&tmp_portGroupsBmp,
                               &isAdditionalRegDupNeeded,&additionalRegDup))
                    {
                        if(isAdditionalRegDupNeeded == GT_TRUE)
                        {
                            PRV_CPSS_MUST_DIRECT_CALL_TO_DRIVER_INDICATION_CNS
                           /* we need to replace the address of the accessing to the
                              one of the duplicated unit */
                           regAddr = additionalRegDup.additionalAddressesArr[dupUnit-1];
                        }
                    }

                    PRV_CPSS_MUST_DIRECT_CALL_TO_DRIVER_INDICATION_CNS
                    rc = prvCpssDrvHwPpPortGroupReadRegister(
                        devNum, portGroupId, regAddr, &regData);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    inProcessBlocksBmpPtr[i / 2] |=
                        ((regData & 0xFFFF) << ((i == 0) ? 0 : 16));

                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* Not eArch devices */

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockUploadTriggers;

    *inProcessBlocksBmpPtr = 0x0;

    /* loop on all port groups to set the same configuration */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupGetRegField(
            devNum, portGroupId, regAddr,
            0/*fieldOffset*/, 12/*fieldLength*/,
            &regData);
        if (rc != GT_OK)
        {
            return rc;
        }
        *inProcessBlocksBmpPtr |= regData;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          Checks all active port groups of the bitmap.
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    OUT GT_U32                      *inProcessBlocksBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupBlockUploadInProcessGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, inProcessBlocksBmpPtr));

    rc = internal_cpssDxChCncPortGroupBlockUploadInProcessGet(devNum, portGroupsBmp, inProcessBlocksBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, inProcessBlocksBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncPortGroupBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncPortGroupUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncPortGroupUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       portGroupsBmp         - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  CNC counter value
*       GT_NOT_READY             - Upload started after upload trigger or
*                                  continued after queue rewind but yet
*                                  not paused due to queue full and yet not finished.
*                                  The part of queue memory does not contain uploaded counters yet.
*                                  No counters retrieved.
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                                  retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_PORT_GROUPS_BMP                portGroupsBmp,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    GT_STATUS   rc; /* return code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  portGroupNumOfCounters;/* number of counters requested and actually */
                                   /* retrieved for the current port group. */
    GT_U32  minNumOfCounters;      /* the minimum number of counters read per port */
                                   /* group from all port groups.   */
    GT_BOOL firstPortGroup;        /* is first Port Group the first */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(numOfCounterValuesPtr);
    PRV_CNC_COUNTER_FORMAT_CHECK_MAC(devNum, format);
    CPSS_NULL_PTR_CHECK_MAC(counterValuesPtr);
    PRV_CNC_BLOCK_UPLOAD_SUPPORT_CHECK_MAC(devNum);

    if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        rc = GT_OK;

        minNumOfCounters = *numOfCounterValuesPtr;

        firstPortGroup = GT_TRUE;

        /* sum the counters from all the port groups */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            portGroupNumOfCounters = *numOfCounterValuesPtr;
            /* all port groups have the same number of counters */
            rc = cncUploadedBlockGet(
                devNum, portGroupId, firstPortGroup /*initOutCounters*/,
                format, &portGroupNumOfCounters, counterValuesPtr);

            if( portGroupNumOfCounters < minNumOfCounters )
            {
                minNumOfCounters = portGroupNumOfCounters;
            }

            if( GT_OK == rc || GT_NO_MORE == rc )
            {
                /* continute to next port group in case get number of requested counters */
                /* successfuly or this port group has no more counters */
            }
            else
            {
                return rc;
            }

            firstPortGroup = GT_FALSE;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        *numOfCounterValuesPtr = minNumOfCounters;

        return rc;
    }

    /* non multi-port-groups device */
    return cncUploadedBlockGet(
        devNum, CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,
        GT_TRUE /*initOutCounters*/, format,
        numOfCounterValuesPtr,counterValuesPtr);
}

/*******************************************************************************
* cpssDxChCncPortGroupUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncPortGroupBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncPortGroupUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncPortGroupUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       portGroupsBmp         - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  CNC counter value
*       GT_NOT_READY             - Upload started after upload trigger or
*                                  continued after queue rewind but yet
*                                  not paused due to queue full and yet not finished.
*                                  The part of queue memory does not contain uploaded counters yet.
*                                  No counters retrieved.
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                                  retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_PORT_GROUPS_BMP                portGroupsBmp,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupUploadedBlockGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, numOfCounterValuesPtr, format, counterValuesPtr));

    rc = internal_cpssDxChCncPortGroupUploadedBlockGet(devNum, portGroupsBmp, numOfCounterValuesPtr, format, counterValuesPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, numOfCounterValuesPtr, format, counterValuesPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
{
    GT_STATUS rc;            /* return code                 */
    GT_U32    regAddr;       /* register address            */
    GT_U32    data;          /* HW data                     */
    GT_U32    offset;        /* HW data offset              */
    GT_U32    portGroupId;   /*the port group Id            */
    GT_U32    numBits;       /* number of bits for the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);

    switch (format)
    {
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E:
            data = 1;
            break;
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E:
            data = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get info about count mode */
    rc = prvCpssDxChCncBlockClientToCountMode(devNum, blockNum,&regAddr,&offset,&numBits);
    if (rc != GT_OK)
    {
        return rc;
    }


    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr,
            offset, numBits/*length*/, data);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupCounterFormatSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, format));

    rc = internal_cpssDxChCncPortGroupCounterFormatSet(devNum, portGroupsBmp, blockNum, format);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, format));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncPortGroupCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncPortGroupCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
{
    GT_STATUS rc;            /* return code                 */
    GT_U32    regAddr;       /* register address            */
    GT_U32    data;          /* HW data                     */
    GT_U32    offset;        /* HW data offset              */
    GT_U32    portGroupId;   /* the port group Id           */
    GT_U32    numBits;       /* number of bits for the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    CPSS_NULL_PTR_CHECK_MAC(formatPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* get info about count mode */
    rc = prvCpssDxChCncBlockClientToCountMode(devNum, blockNum,&regAddr,&offset,&numBits);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpPortGroupGetRegField(
        devNum, portGroupId, regAddr,
        offset, numBits, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (data)
    {
        case 0:
            *formatPtr = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;
            break;
        case 1:
            *formatPtr = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;
            break;
        case 2:
            *formatPtr = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncPortGroupCounterFormatGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, blockNum, formatPtr));

    rc = internal_cpssDxChCncPortGroupCounterFormatGet(devNum, portGroupsBmp, blockNum, formatPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, blockNum, formatPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCncOffsetForNatClientSet
*
* DESCRIPTION:
*      Set the CNC offset for NAT client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*       offset      - CNC offset for NAT client
*                     (APPLICABLE RANGES: 0..65535)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - on out off range offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The CNC clients ARP and NAT are muxed (since for a
*       given packet the user can access ARP entry or NAT entry).
*       This offset is added to the NAT when sending the pointer to the CNC.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncOffsetForNatClientSet
(
    IN   GT_U8    devNum,
    IN   GT_U32   offset
)
{
    GT_U32 regAddr;     /* the register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    if (offset > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

   regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).NATConfig;

   return prvCpssHwPpSetRegField(devNum,
                                 regAddr,
                                 0 /*offset*/,
                                 16 /*length*/,
                                 offset);
}

/*******************************************************************************
* cpssDxChCncOffsetForNatClientSet
*
* DESCRIPTION:
*      Set the CNC offset for NAT client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*       offset      - CNC offset for NAT client
*                     (APPLICABLE RANGES: 0..65535)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - on out off range offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The CNC clients ARP and NAT are muxed (since for a
*       given packet the user can access ARP entry or NAT entry).
*       This offset is added to the NAT when sending the pointer to the CNC.
*
*******************************************************************************/
GT_STATUS cpssDxChCncOffsetForNatClientSet
(
    IN   GT_U8    devNum,
    IN   GT_U32   offset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncOffsetForNatClientSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, offset));

    rc = internal_cpssDxChCncOffsetForNatClientSet(devNum, offset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, offset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChCncOffsetForNatClientGet
*
* DESCRIPTION:
*      Get the CNC offset for NAT client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       offsetPtr   - (pointer to) CNC offset for NAT client
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The CNC clients ARP and NAT are muxed (since for a
*      given packet the user can access ARP entry or NAT entry).
*      This offset is added to the NAT when sending the pointer to the CNC.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCncOffsetForNatClientGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *offsetPtr
)
{
    GT_STATUS   rc;     /* return code            */
    GT_U32      regAddr;/* the register address   */
    GT_U32      value;  /* the value from regiter */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);

    if(!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).NATConfig;

    rc = prvCpssHwPpGetRegField(devNum,
                                regAddr,
                                0/*fieldOffset*/,
                                16/*fieldLength*/,
                                &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *offsetPtr  = value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncOffsetForNatClientGet
*
* DESCRIPTION:
*      Get the CNC offset for NAT client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       offsetPtr   - (pointer to) CNC offset for NAT client
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The CNC clients ARP and NAT are muxed (since for a
*      given packet the user can access ARP entry or NAT entry).
*      This offset is added to the NAT when sending the pointer to the CNC.
*
*******************************************************************************/
GT_STATUS cpssDxChCncOffsetForNatClientGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *offsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCncOffsetForNatClientGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, offsetPtr));

    rc = internal_cpssDxChCncOffsetForNatClientGet(devNum, offsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, offsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


