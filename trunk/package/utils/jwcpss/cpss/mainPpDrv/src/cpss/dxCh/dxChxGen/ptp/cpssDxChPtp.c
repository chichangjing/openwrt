/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPtp.c
*
* DESCRIPTION:
*       CPSS DxCh Precision Time Protocol function implementations.
*
* FILE REVISION NUMBER:
*       $Revision: 43$
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE

/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>
#include <cpss/dxCh/dxChxGen/ptp/private/prvCpssDxChPtpLog.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>

/* number of Policer Counter sets */
#define PRV_CPSS_DXCH_POLICER_MAX_COUNTER_INDEX_CNS     16

/* temporary unused parameter */
#define PRV_TMP_PARAM_NOT_USED_MAC(_p) (void)(_p)

/* Converts packet PTP TS UDP Checksum Update Mode to hardware value */
#define PRV_CPSS_CONVERT_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_TO_HW_VAL_MAC(_val, _mode) \
    switch (_mode)                                                                  \
    {                                                                               \
        case CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_CLEAR_E:                     \
            _val = 0;                                                               \
            break;                                                                  \
        case CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_RECALCULATE_E:               \
            _val = 1;                                                               \
            break;                                                                  \
        case CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_KEEP_E:                      \
            _val = 3;                                                               \
            break;                                                                  \
        default:                                                                    \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                    \
    }

/* Converts PTP TS UDP Checksum Update Mode hardware value to SW value */
#define PRV_CPSS_CONVERT_HW_VAL_TO_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_MAC(_val, _mode) \
    switch (_val)                                                                   \
    {                                                                               \
        case 0:                                                                     \
            _mode = CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_CLEAR_E;              \
            break;                                                                  \
        case 1:                                                                     \
            _mode = CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_RECALCULATE_E;        \
            break;                                                                  \
        case 3:                                                                     \
            _mode = CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_KEEP_E;               \
            break;                                                                  \
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                               \
    }

/* Converts packet PTP Egress Exception Command to hardware value */
#define PRV_CPSS_CONVERT_PTP_EGRESS_EXCEPTION_CMD_TO_HW_VAL_MAC(_val, _cmd) \
    switch (_cmd)                                                           \
    {                                                                       \
        case CPSS_PACKET_CMD_FORWARD_E:                                     \
            _val = 0;                                                       \
            break;                                                          \
        case CPSS_PACKET_CMD_DROP_HARD_E:                                   \
            _val = 1;                                                       \
            break;                                                          \
        default:                                                            \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

/* Converts PTP Egress Exception Command  to hardware value */
#define PRV_CPSS_CONVERT_HW_VAL_TO_PTP_EGRESS_EXCEPTION_CMD_MAC(_val, _cmd) \
    switch (_val)                                                           \
    {                                                                       \
        case 0:                                                             \
            _cmd = CPSS_PACKET_CMD_FORWARD_E;                               \
            break;                                                          \
        case 1:                                                             \
            _cmd = CPSS_PACKET_CMD_DROP_HARD_E;                             \
            break;                                                          \
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);                                            \
    }

/* TAI Iterator LOOP Header */
#define PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(_dev, _taiIdPtr, _taiIterator)     \
    _taiIterator.state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_START_E;                   \
    while (1)                                                                            \
    {                                                                                    \
        {                                                                                \
            GT_STATUS __rc;                                                              \
            __rc = prvCpssDxChPtpNextTaiGet(_dev, _taiIdPtr, &_taiIterator);             \
            if (__rc != GT_OK)                                                           \
            {                                                                            \
                return __rc;                                                             \
            }                                                                            \
            if (_taiIterator.state == PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E)    \
            {                                                                            \
                break;                                                                   \
            }                                                                            \
        }

/* TAI Iterator LOOP footer */
#define PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(_dev, _taiIterator, _taiIdPtr)       \
    }

/* TAI Iterator Get Instanse and check that it is Single */
#define PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(_dev, _taiIdPtr, _taiIterator)     \
    _taiIterator.state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_START_E;                   \
    {                                                                                    \
        GT_STATUS __rc;                                                                  \
        __rc = prvCpssDxChPtpNextTaiGet(_dev, _taiIdPtr, &_taiIterator);                 \
        if (__rc != GT_OK)                                                               \
        {                                                                                \
            return __rc;                                                                 \
        }                                                                                \
        if (_taiIterator.state != PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E)     \
        {                                                                                \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                         \
        }                                                                                \
    }

/* port number to access the PTP ILKN registers DB */
#define PRV_CPSS_DXCH_ILKN_PTP_PORT_NUM_CNS 72

/* Get port MAC num if MAC type or ILKN indication */
#define PRV_CPSS_DXCH_PTP_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(_dev, _port, _portMacNum)              \
    {                                                                                               \
        GT_STATUS __rc;                                                                             \
        CPSS_DXCH_DETAILED_PORT_MAP_STC *_portMapShadowPtr;                                       \
        __rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(_dev, _port, &_portMapShadowPtr);          \
        if (__rc != GT_OK)                                                                          \
        {                                                                                           \
            return __rc;                                                                            \
        }                                                                                           \
        if (GT_FALSE == _portMapShadowPtr->valid)                                                   \
        {                                                                                           \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                                    \
        }                                                                                           \
        if (CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E == _portMapShadowPtr->portMap.mappingType)   \
        {                                                                                           \
            _portMacNum = PRV_CPSS_DXCH_ILKN_PTP_PORT_NUM_CNS;                                      \
            return GT_OK;                                                                           \
        }                                                                                           \
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(_dev, _port ,_portMacNum);                 \
    }

/*check pointer only for not single TAI devices*/
#define PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(_dev, _ptr)                                  \
    if ((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(_dev) != GT_FALSE)                             \
       && (PRV_CPSS_DXCH_PP_MAC(_dev)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE))           \
    {                                                                                               \
        CPSS_NULL_PTR_CHECK_MAC(_ptr);                                                              \
    }

/*
 * typedef: PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_ENT
 *
 *  Description:
 *     State of TAI Iteration Structure.
 *
 * Enumerations:
 *     PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_START_E      - Start - value for initialization
 *     PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E      - Valid TAI subunit identified
 *     PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E - Last Valid TAI subunit identified
 *                     typicaly checked in read functions that require the single TAI.
 *     PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E    - No TAI subunits more
 *
 */
typedef enum {
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_START_E,
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E,
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E,
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E
} PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_ENT;

/*
 * typedef: struct PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC
 *
 * Description: Structure for TAI Iteration.
 *
 * Fields:
 *  state          - TAI Iterator state
 *  gopNumber      - GOP Number
 *  taiNumber      - TAI Number
 *  portGroupId    - port group Id
 *
 */
typedef struct{
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_ENT      state;
    GT_U32                                        gopNumber;
    GT_U32                                        taiNumber;
    GT_U32                                        portGroupId;
} PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC;

/*******************************************************************************
* prvCpssDxChPtpNextTaiGet
*
* DESCRIPTION:
*       Gets next TAI subunit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Id.
*                         For single TAI devices taiIdPtr ignored,
*                         iterator state evaluation is START_E => VALID_LAST => NO_MORE.
*       iteratorPtr     - (pointer to) TAI iterator
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPtpNextTaiGet
(
    IN    GT_U8                               devNum,
    IN    CPSS_DXCH_PTP_TAI_ID_STC            *taiIdPtr,
    INOUT PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC  *iteratorPtr
)
{
    GT_STATUS rc;                        /* return code                  */
    GT_U32    gopsAmount;                /* amount of GOPs               */
    GT_U32    globalGop;                 /* Global TAI GOP index         */
    GT_U32    bmp;                       /* work port group bitmap       */
    GT_U32    i;                         /* loop index                   */
    GT_BOOL   isPortGroupLast;           /* is Port Group Last           */
    GT_U32    portMacNum;                /* MAC number                   */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance != GT_FALSE)
    {
        switch (iteratorPtr->state)
        {
            case PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_START_E:
                iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E;
                iteratorPtr->gopNumber = 0;
                iteratorPtr->taiNumber = 0;
                rc = prvCpssPpConfigPortGroupFirstActiveGet(
                    devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, &(iteratorPtr->portGroupId));
                if (rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                return GT_OK;
            case PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E:
                iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
                return GT_OK;
            default:
                iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* temporary for Bobcat3 the same that for Bobcat2 */
    gopsAmount = 10; /* BobCat2 amount of GOPs + ILKN */
    globalGop  = 9; /* BobCat2 Global TAI ILKN index */

    switch (iteratorPtr->state)
    {
        case PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_START_E:
            /* calculate iteratorPtr->taiNumber */
            switch (taiIdPtr->taiNumber)
            {
                case CPSS_DXCH_PTP_TAI_NUMBER_0_E:
                    iteratorPtr->taiNumber = 0;
                    break;
                case CPSS_DXCH_PTP_TAI_NUMBER_1_E:
                    iteratorPtr->taiNumber = 1;
                    break;
                case CPSS_DXCH_PTP_TAI_NUMBER_ALL_E:
                    iteratorPtr->taiNumber = 0;
                    break;
                default:
                    iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* calculate iteratorPtr->gopNumber */
            switch (taiIdPtr->taiInstance)
            {
                case CPSS_DXCH_PTP_TAI_INSTANCE_ALL_E:
                    iteratorPtr->gopNumber = 0;
                    rc = prvCpssPpConfigPortGroupFirstActiveGet(
                        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, &(iteratorPtr->portGroupId));
                    if (rc != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                    iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E;
                    break;
                case CPSS_DXCH_PTP_TAI_INSTANCE_GLOBAL_E:
                    iteratorPtr->gopNumber = globalGop;
                    if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
                    {
                        /* the last port group */
                        bmp = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp;
                        if (bmp == 0)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* must not occure */
                        }
                        /* look for the biggest non-zero bit */
                        for (i = 31; ((bmp & (1 << i)) == 0); i--) {};
                        iteratorPtr->portGroupId = i;

                    }
                    else
                    {
                        iteratorPtr->portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
                    }
                    break;
                case CPSS_DXCH_PTP_TAI_INSTANCE_PORT_E:
                    /* state will be overriden below after successfull port checking */
                    iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
                    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,taiIdPtr->portNum,portMacNum);
                    rc = prvCpssDxChHwPpPortToGopConvert(
                        devNum, portMacNum,
                        &(iteratorPtr->portGroupId), &(iteratorPtr->gopNumber));
                    if (rc != GT_OK)
                    {
                        iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
                        return rc;
                    }
                    break;
                default:
                    iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* calculate iteratorPtr->state */
            if (((taiIdPtr->taiInstance == CPSS_DXCH_PTP_TAI_INSTANCE_ALL_E)
                 && (gopsAmount > 1))
                || (taiIdPtr->taiNumber == CPSS_DXCH_PTP_TAI_NUMBER_ALL_E))
            {
                iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E;
            }
            else
            {
                iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E;
            }
            return GT_OK;

        case PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E:
            if (taiIdPtr->taiInstance != CPSS_DXCH_PTP_TAI_INSTANCE_ALL_E)
            {
                if ((taiIdPtr->taiNumber == CPSS_DXCH_PTP_TAI_NUMBER_ALL_E)
                    && (iteratorPtr->taiNumber == 0))
                {
                    iteratorPtr->taiNumber = 1;
                    iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E;
                    return GT_OK;
                }
                else
                {
                    iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            else /*(taiIdPtr->taiInstance == CPSS_DXCH_PTP_TAI_INSTANCE_ALL_E)*/
            {
                bmp = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp;
                if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
                {
                    /* check is the port group last */
                    isPortGroupLast =
                        ((bmp & (0xFFFFFFFF << (iteratorPtr->portGroupId + 1))) == 0)
                            ? GT_TRUE : GT_FALSE;
                }
                else
                {
                    isPortGroupLast = GT_TRUE;
                }

                /* next TAI number */
                if (taiIdPtr->taiNumber == CPSS_DXCH_PTP_TAI_NUMBER_ALL_E)
                {
                    if (iteratorPtr->taiNumber == 0)
                    {
                        iteratorPtr->taiNumber = 1;
                        iteratorPtr->state =
                            (((iteratorPtr->gopNumber + 1) >= gopsAmount)
                                && (isPortGroupLast == GT_TRUE))
                                    ? PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E
                                    : PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E;
                        return GT_OK;
                    }
                    else
                    {
                        iteratorPtr->taiNumber = 0;
                    }
                }
                else
                {
                    iteratorPtr->taiNumber =
                        (taiIdPtr->taiNumber != CPSS_DXCH_PTP_TAI_NUMBER_1_E) ? 0 : 1;
                }

                /* next GOP number */
                iteratorPtr->gopNumber ++;
                if ((iteratorPtr->gopNumber + 1) < gopsAmount)
                {
                    iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E;
                    return GT_OK;
                }
                if ((iteratorPtr->gopNumber + 1) == gopsAmount)
                {
                    if (isPortGroupLast == GT_TRUE)
                    {
                        iteratorPtr->state =
                            (taiIdPtr->taiNumber != CPSS_DXCH_PTP_TAI_NUMBER_ALL_E)
                                ? PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E
                                : PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E;
                        return GT_OK;
                    }
                    else
                    {
                        /* next port group */
                        /* look for the next non-zero bit */
                        for (i = (iteratorPtr->portGroupId + 1);
                              ((bmp & (1 << i)) == 0); i++) {};
                        iteratorPtr->portGroupId = i;
                        iteratorPtr->gopNumber = 0;
                        iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_E;
                        return GT_OK;
                    }
                }
                /* error state */
                iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_VALID_LAST_E:
            iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
            return GT_OK;
        case PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E:
        default:
            iteratorPtr->state = PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STATE_NO_MORE_E;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /*return GT_OK;*/
}

/*******************************************************************************
* internal_cpssDxChPtpEtherTypeSet
*
* DESCRIPTION:
*       Configure ethertype0/ethertype1 of PTP over Ethernet packets.
*       A packet is identified as PTP over Ethernet if its EtherType matches
*       one of the configured values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index.
*                         (APPLICABLE RANGES: 0..1.)
*       etherType       - PTP EtherType0 or EtherType1 according to the index.
*                         (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_OUT_OF_RANGE          - on wrong etherType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherTypeIndex,
    IN GT_U32   etherType
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    if (etherTypeIndex > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (etherType > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPEtherTypes;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEthertypes;
    }

    fieldOffset = (etherTypeIndex * 16);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 16,
                                     etherType);
}

/*******************************************************************************
* cpssDxChPtpEtherTypeSet
*
* DESCRIPTION:
*       Configure ethertype0/ethertype1 of PTP over Ethernet packets.
*       A packet is identified as PTP over Ethernet if its EtherType matches
*       one of the configured values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index.
*                         (APPLICABLE RANGES: 0..1.)
*       etherType       - PTP EtherType0 or EtherType1 according to the index.
*                         (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_OUT_OF_RANGE          - on wrong etherType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherTypeIndex,
    IN GT_U32   etherType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEtherTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherTypeIndex, etherType));

    rc = internal_cpssDxChPtpEtherTypeSet(devNum, etherTypeIndex, etherType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherTypeIndex, etherType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpEtherTypeGet
*
* DESCRIPTION:
*       Get ethertypes of PTP over Ethernet packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       etherTypePtr    - (pointer to) PTP EtherType0 or EtherType1 according
*                          to the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEtherTypeGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   etherTypeIndex,
    OUT GT_U32   *etherTypePtr
)
{
    GT_U32      regAddr;        /* register address                 */
    GT_U32      fieldOffset;    /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    if (etherTypeIndex > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPEtherTypes;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEthertypes;
    }

    fieldOffset = (etherTypeIndex * 16);

    return prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 16,
                                     etherTypePtr);
}

/*******************************************************************************
* cpssDxChPtpEtherTypeGet
*
* DESCRIPTION:
*       Get ethertypes of PTP over Ethernet packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       etherTypePtr    - (pointer to) PTP EtherType0 or EtherType1 according
*                          to the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEtherTypeGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   etherTypeIndex,
    OUT GT_U32   *etherTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEtherTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherTypeIndex, etherTypePtr));

    rc = internal_cpssDxChPtpEtherTypeGet(devNum, etherTypeIndex, etherTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherTypeIndex, etherTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpUdpDestPortsSet
*
* DESCRIPTION:
*       Configure UDP destination port0/port1 of PTP over UDP packets.
*       A packet is identified as PTP over UDP if it is a UDP packet, whose
*       destination port matches one of the configured ports.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index.
*                         (APPLICABLE RANGES: 0..1.)
*       udpPortNum      - UDP port1/port0 number according to the index,
*                         (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_OUT_OF_RANGE          - on wrong udpPortNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpUdpDestPortsSet
(
    IN GT_U8    devNum,
    IN GT_U32   udpPortIndex,
    IN GT_U32   udpPortNum
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    if (udpPortIndex > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (udpPortNum > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPOverUDPDestinationPorts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpUdpDstPorts;
    }

    fieldOffset = (udpPortIndex * 16);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 16,
                                     udpPortNum);
}

/*******************************************************************************
* cpssDxChPtpUdpDestPortsSet
*
* DESCRIPTION:
*       Configure UDP destination port0/port1 of PTP over UDP packets.
*       A packet is identified as PTP over UDP if it is a UDP packet, whose
*       destination port matches one of the configured ports.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index.
*                         (APPLICABLE RANGES: 0..1.)
*       udpPortNum      - UDP port1/port0 number according to the index,
*                         (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_OUT_OF_RANGE          - on wrong udpPortNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpDestPortsSet
(
    IN GT_U8    devNum,
    IN GT_U32   udpPortIndex,
    IN GT_U32   udpPortNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpUdpDestPortsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, udpPortIndex, udpPortNum));

    rc = internal_cpssDxChPtpUdpDestPortsSet(devNum, udpPortIndex, udpPortNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, udpPortIndex, udpPortNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpUdpDestPortsGet
*
* DESCRIPTION:
*       Get UDP destination port0/port1 of PTP over UDP packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       udpPortNumPtr     - (pointer to) UDP port0/port1 number, according
*                           to the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpUdpDestPortsGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   udpPortIndex,
    OUT GT_U32   *udpPortNumPtr
)
{
    GT_U32      regAddr;        /* register address                 */
    GT_U32      fieldOffset;    /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(udpPortNumPtr);

    if (udpPortIndex > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPOverUDPDestinationPorts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpUdpDstPorts;
    }

    fieldOffset = (udpPortIndex * 16);

    return prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 16,
                                     udpPortNumPtr);
}

/*******************************************************************************
* cpssDxChPtpUdpDestPortsGet
*
* DESCRIPTION:
*       Get UDP destination port0/port1 of PTP over UDP packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       udpPortNumPtr     - (pointer to) UDP port0/port1 number, according
*                           to the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpDestPortsGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   udpPortIndex,
    OUT GT_U32   *udpPortNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpUdpDestPortsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, udpPortIndex, udpPortNumPtr));

    rc = internal_cpssDxChPtpUdpDestPortsGet(devNum, udpPortIndex, udpPortNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, udpPortIndex, udpPortNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpMessageTypeCmdSet
*
* DESCRIPTION:
*       Configure packet command per PTP message type.
*       The message type is extracted from the PTP header.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       command         - assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpMessageTypeCmdSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   domainIndex,
    IN GT_U32                   messageType,
    IN CPSS_PACKET_CMD_ENT      command
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    hwValue;          /* value to write to hw             */
    PRV_CPSS_DXCH_TABLE_ENT tableType;/* table type */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    if (messageType > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(hwValue, command);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */

        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

        if (domainIndex > 4)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
        {
            tableType = PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E;
            /* continues messageType 3 bits command for each of 5 domains */
            fieldOffset = (15 * messageType) + (3 * domainIndex) ;
        }
        else
        {
            tableType = PRV_CPSS_DXCH_LION3_TABLE_TTI_PTP_COMMAND_E;
            /* for each messageType 3 bits command for each of 5 domains an one reserved bit */
            fieldOffset = (16 * messageType) + (3 * domainIndex) ;
        }

        return prvCpssDxChWriteTableEntryField(
            devNum, tableType,
            portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
            fieldOffset, 3 /*fieldLength*/, hwValue);
    }

    fieldOffset = (messageType % 10) * 3;

    if (messageType / 10)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ttiRegs.ptpPacketCmdCfg1Reg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ttiRegs.ptpPacketCmdCfg0Reg;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 3, hwValue);
}

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdSet
*
* DESCRIPTION:
*       Configure packet command per PTP message type.
*       The message type is extracted from the PTP header.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       command         - assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeCmdSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   domainIndex,
    IN GT_U32                   messageType,
    IN CPSS_PACKET_CMD_ENT      command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpMessageTypeCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainIndex, messageType, command));

    rc = internal_cpssDxChPtpMessageTypeCmdSet(devNum, portNum, domainIndex, messageType, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainIndex, messageType, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpMessageTypeCmdGet
*
* DESCRIPTION:
*       Get packet command per PTP message type.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       commandPtr      - (pointer to) assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpMessageTypeCmdGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_U32                   domainIndex,
    IN  GT_U32                   messageType,
    OUT CPSS_PACKET_CMD_ENT      *commandPtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    hwValue;          /* value to read from hw            */
    GT_U32    rc;               /* return code                      */
    PRV_CPSS_DXCH_TABLE_ENT tableType;/* table type */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    if (messageType > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */

        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

        if (domainIndex > 4)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
        {
            tableType = PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E;
            /* continues messageType 3 bits command for each of 5 domains */
            fieldOffset = (15 * messageType) + (3 * domainIndex) ;
        }
        else
        {
            tableType = PRV_CPSS_DXCH_LION3_TABLE_TTI_PTP_COMMAND_E;
            /* for each messageType 3 bits command for each of 5 domains an one reserved bit */
            fieldOffset = (16 * messageType) + (3 * domainIndex) ;
        }

        rc = prvCpssDxChReadTableEntryField(
            devNum, tableType,
            portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
            fieldOffset, 3 /*fieldLength*/, &hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*commandPtr, hwValue);

        return GT_OK;
    }

    fieldOffset = (messageType % 10) * 3;

    if (messageType / 10)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ttiRegs.ptpPacketCmdCfg1Reg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ttiRegs.ptpPacketCmdCfg0Reg;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 3, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*commandPtr, hwValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdGet
*
* DESCRIPTION:
*       Get packet command per PTP message type.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       commandPtr      - (pointer to) assigned command to the packet.
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeCmdGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_U32                   domainIndex,
    IN  GT_U32                   messageType,
    OUT CPSS_PACKET_CMD_ENT      *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpMessageTypeCmdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainIndex, messageType, commandPtr));

    rc = internal_cpssDxChPtpMessageTypeCmdGet(devNum, portNum, domainIndex, messageType, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainIndex, messageType, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpCpuCodeBaseSet
*
* DESCRIPTION:
*       Set CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       cpuCode         - The base of CPU code assigned to PTP packets mirrored
*                         or trapped to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpCpuCodeBaseSet
(
    IN GT_U8                        devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT     cpuCode
)
{
    GT_U32                                  regAddr;    /* register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode; /* DSA code */
    GT_STATUS                               rc;         /* return code */
    GT_U32                                  offset;     /* bit offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode, &dsaCpuCode);
    if( rc != GT_OK )
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPExceptionsAndCPUCodeConfig;
        offset  = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpPacketCmdCfg1Reg;
        offset  = 18;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 8, (GT_U32)dsaCpuCode);
}

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseSet
*
* DESCRIPTION:
*       Set CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       cpuCode         - The base of CPU code assigned to PTP packets mirrored
*                         or trapped to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpCpuCodeBaseSet
(
    IN GT_U8                        devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT     cpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpCpuCodeBaseSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCode));

    rc = internal_cpssDxChPtpCpuCodeBaseSet(devNum, cpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpCpuCodeBaseGet
*
* DESCRIPTION:
*       Get CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
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
*       cpuCodePtr      - (pointer to) The base of CPU code assigned to PTP
*                           packets mirrored or trapped to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpCpuCodeBaseGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT     *cpuCodePtr
)
{

    GT_U32                                  regAddr;    /* register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode; /* DSA code */
    GT_U32                                  tempCode;   /* dsa code */
    GT_STATUS                               rc;         /* return code */
    GT_U32                                  offset;     /* bit offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPExceptionsAndCPUCodeConfig;
        offset  = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpPacketCmdCfg1Reg;
        offset  = 18;
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, 8, &tempCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    dsaCpuCode = (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)tempCode;

    return prvCpssDxChNetIfDsaToCpuCode(dsaCpuCode,cpuCodePtr);
}

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseGet
*
* DESCRIPTION:
*       Get CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType
*       (where message type is taken from PTP header).
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
*       cpuCodePtr      - (pointer to) The base of CPU code assigned to PTP
*                           packets mirrored or trapped to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpCpuCodeBaseGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT     *cpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpCpuCodeBaseGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCodePtr));

    rc = internal_cpssDxChPtpCpuCodeBaseGet(devNum, cpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TAI_ID_STC                 *taiIdPtr,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */

        PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

        switch (function)
        {
            case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E:
                hwValue = 0;
                break;
            case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_FREQUENCE_UPDATE_E:
                hwValue = 1;
                break;
            case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E:
                hwValue = 2;
                break;
            case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_DECREMENT_E:
                hwValue = 3;
                break;
            case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E:
                hwValue = 4;
                break;
            case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_INCREMENT_E:
                hwValue = 5;
                break;
            case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_DECREMENT_E:
                hwValue = 6;
                break;
            case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_NOP_E:
                hwValue = 7;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr, 2, 3, hwValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

        return GT_OK;
    }

    return cpssDxChPtpPortGroupTodCounterFunctionSet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            function);
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TAI_ID_STC                 *taiIdPtr,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTodCounterFunctionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, taiIdPtr, function));

    rc = internal_cpssDxChPtpTodCounterFunctionSet(devNum, direction, taiIdPtr, function);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, taiIdPtr, function));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter
*                          functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */
        PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
        CPSS_NULL_PTR_CHECK_MAC(functionPtr);
        PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr, 2, 3, &hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        switch (hwValue)
        {
            case 0:
                *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;
                break;
            case 1:
                *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_FREQUENCE_UPDATE_E;
                break;
            case 2:
                *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E;
                break;
            case 3:
                *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_DECREMENT_E;
                break;
            case 4:
                *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E;
                break;
            case 5:
                *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_INCREMENT_E;
                break;
            case 6:
                *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GRACEFULLY_DECREMENT_E;
                break;
            case 7:
                *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_NOP_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /*never occurs*/
        }
        return GT_OK;
    }

    return cpssDxChPtpPortGroupTodCounterFunctionGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            functionPtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter
*                          functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTodCounterFunctionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, taiIdPtr, functionPtr));

    rc = internal_cpssDxChPtpTodCounterFunctionGet(devNum, direction, taiIdPtr, functionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, taiIdPtr, functionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTodCounterFunctionTriggerSet
(
    IN GT_U8                       devNum,
    IN CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC   *taiIdPtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */

        PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

        PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

            /* toggle required for trigger */
            /* unset trigger bit */
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr, 0, 1, 0);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* set trigger bit */
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr, 0, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

        return GT_OK;
    }

    return cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction);
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionTriggerSet
(
    IN GT_U8                       devNum,
    IN CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC   *taiIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTodCounterFunctionTriggerSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, taiIdPtr));

    rc = internal_cpssDxChPtpTodCounterFunctionTriggerSet(devNum, direction, taiIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, taiIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTodCounterFunctionTriggerGet
(
    IN GT_U8                       devNum,
    IN CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC   *taiIdPtr,
    OUT GT_BOOL                    *enablePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* BobCat2 and above devices */
        PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
        CPSS_NULL_PTR_CHECK_MAC(enablePtr);
        PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr, 0, 1, &hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        *enablePtr = BIT2BOOL_MAC(hwValue);

        return GT_OK;
    }

    return cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            enablePtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx
*                         (APPLICABLE DEVICES: Lion; xCat2; Lion2.)
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterFunctionTriggerGet
(
    IN GT_U8                       devNum,
    IN CPSS_PORT_DIRECTION_ENT     direction,
    IN  CPSS_DXCH_PTP_TAI_ID_STC   *taiIdPtr,
    OUT GT_BOOL                    *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTodCounterFunctionTriggerGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, taiIdPtr, enablePtr));

    rc = internal_cpssDxChPtpTodCounterFunctionTriggerGet(devNum, direction, taiIdPtr, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, taiIdPtr, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - (pointer to) TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
{
    return cpssDxChPtpPortGroupTodCounterShadowSet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            todCounterPtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - (pointer to) TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTodCounterShadowSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, todCounterPtr));

    rc = internal_cpssDxChPtpTodCounterShadowSet(devNum, direction, todCounterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, todCounterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    return cpssDxChPtpPortGroupTodCounterShadowGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            direction,
                                            todCounterPtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTodCounterShadowGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, todCounterPtr));

    rc = internal_cpssDxChPtpTodCounterShadowGet(devNum, direction, todCounterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, todCounterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTodCounterGet
*
* DESCRIPTION:
*       Get TOD time stamping counter value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTodCounterGet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    return cpssDxChPtpPortGroupTodCounterGet(devNum,
                                             CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                             direction,
                                             todCounterPtr);
}

/*******************************************************************************
* cpssDxChPtpTodCounterGet
*
* DESCRIPTION:
*       Get TOD time stamping counter value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTodCounterGet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTodCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, todCounterPtr));

    rc = internal_cpssDxChPtpTodCounterGet(devNum, direction, todCounterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, todCounterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable port for PTP time stamping.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       portNum         - port number
*                         Note: For Ingress time stamping the trigger is based
*                               on the local source port.
*                               For Egress time stamping the trigger is based
*                               on the local target port.
*       enable          - GT_TRUE - The port is enabled for ingress/egress
*                                   time stamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   time stamping.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortTimeStampEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    portGroupId;      /* the port group Id - support
                                            multi-port-groups device */
    GT_U32     localPort;        /* local port - support
                                            multi-port-groups device */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    fieldOffset = OFFSET_TO_BIT_MAC(localPort);
    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            ttiRegs.ptpTimeStampPortEnReg[OFFSET_TO_WORD_MAC(localPort)];

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                                                portGroupId,
                                                regAddr,
                                                fieldOffset,
                                                1,
                                                hwData);

        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            haRegs.ptpTimeStampPortEnReg[OFFSET_TO_WORD_MAC(localPort)];

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                fieldOffset, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable port for PTP time stamping.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       portNum         - port number
*                         Note: For Ingress time stamping the trigger is based
*                               on the local source port.
*                               For Egress time stamping the trigger is based
*                               on the local target port.
*       enable          - GT_TRUE - The port is enabled for ingress/egress
*                                   time stamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   time stamping.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTimeStampEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortTimeStampEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, portNum, enable));

    rc = internal_cpssDxChPtpPortTimeStampEnableSet(devNum, direction, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortTimeStampEnableGet
*
* DESCRIPTION:
*       Get port PTP time stamping status (enabled/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       portNum         - port number
*                         Note: For Ingress time stamping the trigger is based
*                               on the local source port.
*                               For Egress time stamping the trigger is based
*                               on the local target port.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The port is enabled for
*                                               ingress/egress time stamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   time stamping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortTimeStampEnableGet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    portGroupId;      /* the port group Id - support
                                            multi-port-groups device */
    GT_U32     localPort;        /* local port - support
                                            multi-port-groups device */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    fieldOffset = OFFSET_TO_BIT_MAC(localPort);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            ttiRegs.ptpTimeStampPortEnReg[OFFSET_TO_WORD_MAC(localPort)];

            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                        fieldOffset, 1, &hwData);
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                haRegs.ptpTimeStampPortEnReg[OFFSET_TO_WORD_MAC(localPort)];

            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                        fieldOffset, 1, &hwData);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableGet
*
* DESCRIPTION:
*       Get port PTP time stamping status (enabled/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       portNum         - port number
*                         Note: For Ingress time stamping the trigger is based
*                               on the local source port.
*                               For Egress time stamping the trigger is based
*                               on the local target port.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The port is enabled for
*                                               ingress/egress time stamping.
*                         GT_FALSE - The port is disabled for ingress/egress
*                                   time stamping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTimeStampEnableGet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortTimeStampEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, portNum, enablePtr));

    rc = internal_cpssDxChPtpPortTimeStampEnableGet(devNum, direction, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpEthernetTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping over Ethernet packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       enable          - GT_TRUE - PTP Time stamping is enabled over Ethernet.
*                         GT_FALSE - PTP Time stamping is disabled over Ethernet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEthernetTimeStampEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_BOOL                  enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalConfig;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
        }

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping over Ethernet packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       enable          - GT_TRUE - PTP Time stamping is enabled over Ethernet.
*                         GT_FALSE - PTP Time stamping is disabled over Ethernet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEthernetTimeStampEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_BOOL                  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEthernetTimeStampEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, enable));

    rc = internal_cpssDxChPtpEthernetTimeStampEnableSet(devNum, direction, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpEthernetTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status over Ethernet packets (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled
*                                                over Ethernet.
*                         GT_FALSE - PTP Time stamping is disabled over Ethernet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEthernetTimeStampEnableGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                  *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status over Ethernet packets (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled
*                                                over Ethernet.
*                         GT_FALSE - PTP Time stamping is disabled over Ethernet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEthernetTimeStampEnableGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEthernetTimeStampEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, enablePtr));

    rc = internal_cpssDxChPtpEthernetTimeStampEnableGet(devNum, direction, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpUdpTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping over UDP packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       protocolStack   - types of IP to set: IPV4 or IPV6.
*       enable          - GT_TRUE - PTP Time stamping is enabled over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Time stamping is disabled over UDP (IPV4/IPV6).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpUdpTimeStampEnableSet
(
    IN  GT_U8                        devNum,
    IN  CPSS_PORT_DIRECTION_ENT      direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN  GT_BOOL                      enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* the offset of the field          */
    GT_U32    filedLength;      /* the length of the field          */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    hwData = BOOL2BIT_MAC(enable);
    filedLength = 1;

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            fieldOffset  = 1;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            fieldOffset = 2;
            break;
        case CPSS_IP_PROTOCOL_IPV4V6_E:
            fieldOffset = 1;
            filedLength = 2;
            hwData |= (hwData << 1);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset,
                                       filedLength, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalConfig;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
        }

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, filedLength, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping over UDP packets.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       protocolStack   - types of IP to set: IPV4 or IPV6.
*       enable          - GT_TRUE - PTP Time stamping is enabled over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Time stamping is disabled over UDP (IPV4/IPV6).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpTimeStampEnableSet
(
    IN  GT_U8                        devNum,
    IN  CPSS_PORT_DIRECTION_ENT      direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN  GT_BOOL                      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpUdpTimeStampEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, protocolStack, enable));

    rc = internal_cpssDxChPtpUdpTimeStampEnableSet(devNum, direction, protocolStack, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, protocolStack, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpUdpTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status over UDP packets (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       protocolStack   - types of IP to set: IPV4 or IPV6.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled
*                                               over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Time stamping is disabled over UDP (IPV4/IPV6).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpUdpTimeStampEnableGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT      protocolStack,
    OUT GT_BOOL                         *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* the offset of the field          */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            fieldOffset  = 1;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            fieldOffset = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status over UDP packets (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       protocolStack   - types of IP to set: IPV4 or IPV6.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled
*                                               over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Time stamping is disabled over UDP (IPV4/IPV6).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpUdpTimeStampEnableGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    IN  CPSS_IP_PROTOCOL_STACK_ENT      protocolStack,
    OUT GT_BOOL                         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpUdpTimeStampEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, protocolStack, enablePtr));

    rc = internal_cpssDxChPtpUdpTimeStampEnableGet(devNum, direction, protocolStack, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, protocolStack, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpMessageTypeTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping for specific message type of the packet.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       enable          - GT_TRUE - PTP Time stamping is enabled for specific message type.
*                         GT_FALSE - PTP Time stamping is disabled for specific message type.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpMessageTypeTimeStampEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    IN  GT_BOOL                 enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* the offset of the field          */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (messageType > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fieldOffset = messageType + 16;
    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEnableTimeStamp;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpEnableTimeStamp;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpEnableTimeStamp;
        }

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping for specific message type of the packet.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       enable          - GT_TRUE - PTP Time stamping is enabled for specific message type.
*                         GT_FALSE - PTP Time stamping is disabled for specific message type.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeTimeStampEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpMessageTypeTimeStampEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, messageType, enable));

    rc = internal_cpssDxChPtpMessageTypeTimeStampEnableSet(devNum, direction, messageType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, messageType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpMessageTypeTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled for
*                                                specific message type.
*                         GT_FALSE - PTP Time stamping is disabled for specific
*                                    message type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpMessageTypeTimeStampEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* the offset of the field          */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (messageType > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEnableTimeStamp;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpEnableTimeStamp;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpEnableTimeStamp;
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fieldOffset = messageType + 16;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled for
*                                                specific message type.
*                         GT_FALSE - PTP Time stamping is disabled for specific
*                                    message type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpMessageTypeTimeStampEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  messageType,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpMessageTypeTimeStampEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, messageType, enablePtr));

    rc = internal_cpssDxChPtpMessageTypeTimeStampEnableGet(devNum, direction, messageType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, messageType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTransportSpecificCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping check of the packet's transport specific value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx, Rx or both directions
*       enable      - GT_TRUE - PTP Time stamping check of the packet's transport
*                               specific value is enabled.
*                     GT_FALSE - PTP Time stamping check of the packet's transport
*                                specific value is disabled.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTransportSpecificCheckEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalConfig;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
        }

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Time stamping check of the packet's transport specific value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx, Rx or both directions
*       enable      - GT_TRUE - PTP Time stamping check of the packet's transport
*                               specific value is enabled.
*                     GT_FALSE - PTP Time stamping check of the packet's transport
*                                specific value is disabled.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificCheckEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTransportSpecificCheckEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, enable));

    rc = internal_cpssDxChPtpTransportSpecificCheckEnableSet(devNum, direction, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpTransportSpecificCheckEnableGet
*
* DESCRIPTION:
*       Get status of PTP Time stamping check of the packet's transport specific
*       value (the check is enabled/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx or Rx
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - PTP Time stamping check of the
*                                  packet's transport specific value is enabled.
*                     GT_FALSE - PTP Time stamping check of the packet's transport
*                                specific value is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTransportSpecificCheckEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BOOL2BIT_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableGet
*
* DESCRIPTION:
*       Get status of PTP Time stamping check of the packet's transport specific
*       value (the check is enabled/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx or Rx
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - PTP Time stamping check of the
*                                  packet's transport specific value is enabled.
*                     GT_FALSE - PTP Time stamping check of the packet's transport
*                                specific value is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificCheckEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTransportSpecificCheckEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, enablePtr));

    rc = internal_cpssDxChPtpTransportSpecificCheckEnableGet(devNum, direction, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpTransportSpecificTimeStampEnableSet
*
* DESCRIPTION:
*       Enable PTP Time stamping for transport specific value of the packet.
*       Note: Relevant only if global configuration determines whether the
*       transport specific field should be checked by
*       cpssDxChPtpTransportSpecificCheckEnableSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                  - device number
*       direction               - Tx, Rx or both directions
*       transportSpecificVal    - transport specific value taken from PTP header
*                                 bits [7:4].
*                                 (APPLICABLE RANGES: 0..15.)
*       enable                  - GT_TRUE - PTP Time stamping is enabled for this
*                                           transport specific value.
*                                GT_FALSE - PTP Time stamping is disabled for this
*                                           transport specific value.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTransportSpecificTimeStampEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    IN  GT_BOOL                 enable
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (transportSpecificVal > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwData = BOOL2BIT_MAC(enable);


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEnableTimeStamp;

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, transportSpecificVal,
                                       1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpEnableTimeStamp;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpEnableTimeStamp;
        }

        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, transportSpecificVal,
                                       1, hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableSet
*
* DESCRIPTION:
*       Enable PTP Time stamping for transport specific value of the packet.
*       Note: Relevant only if global configuration determines whether the
*       transport specific field should be checked by
*       cpssDxChPtpTransportSpecificCheckEnableSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                  - device number
*       direction               - Tx, Rx or both directions
*       transportSpecificVal    - transport specific value taken from PTP header
*                                 bits [7:4].
*                                 (APPLICABLE RANGES: 0..15.)
*       enable                  - GT_TRUE - PTP Time stamping is enabled for this
*                                           transport specific value.
*                                GT_FALSE - PTP Time stamping is disabled for this
*                                           transport specific value.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificTimeStampEnableSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTransportSpecificTimeStampEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, transportSpecificVal, enable));

    rc = internal_cpssDxChPtpTransportSpecificTimeStampEnableSet(devNum, direction, transportSpecificVal, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, transportSpecificVal, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpTransportSpecificTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       transportSpecificVal    - transport specific value taken from PTP header
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled for
*                                                this transport specific value.
*                          GT_FALSE - PTP Time stamping is disabled for this
*                                     transport specific value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTransportSpecificTimeStampEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (transportSpecificVal > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpEnableTimeStamp;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpEnableTimeStamp;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpEnableTimeStamp;
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, transportSpecificVal,
                                   1, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Time stamping status for specific message type of the packet
*       (enable/disabled).
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       transportSpecificVal    - transport specific value taken from PTP header
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Time stamping is enabled for
*                                                this transport specific value.
*                          GT_FALSE - PTP Time stamping is disabled for this
*                                     transport specific value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or
*                                  direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTransportSpecificTimeStampEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  transportSpecificVal,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTransportSpecificTimeStampEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, transportSpecificVal, enablePtr));

    rc = internal_cpssDxChPtpTransportSpecificTimeStampEnableGet(devNum, direction, transportSpecificVal, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, transportSpecificVal, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpTimestampEntryGet
*
* DESCRIPTION:
*       Read current Timestamp entry from the timestamp FIFO queue.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
* OUTPUTS:
*       entryPtr        - (pointer to) timestamp entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  Timestamp entries
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTimestampEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC   *entryPtr
)
{
    GT_U32    regAddr;          /* register address of LSB word     */
    GT_U32    regAddr1;         /* register address of MSB word     */
    GT_U32    hwData0 = 0x0;    /* data to read from hw             */
    GT_U32    hwData1 = 0x0;    /* data to read from hw             */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    activePortGroupsBmp; /* bitmap of active Port Groups  */
    GT_BOOL   entryFound;       /* flag for finding an entry        */
    GT_U32    portGroupsCheckedBmp; /* port groups bitmap that have
                                       been checked this time       */
    GT_U32    rc = GT_OK;       /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    cpssOsMemSet(entryPtr, 0, sizeof(*entryPtr));

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalFifoCurEntry[0];
            regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalFifoCurEntry[1];
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddr  = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalFifoCurEntry[0];
                regAddr1 = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalFifoCurEntry[1];
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalFifoCurEntry[0];
                regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalFifoCurEntry[1];
            }

            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    entryFound = GT_FALSE;
    portGroupsCheckedBmp = 0x0;

    /* get the current port group ID to serve */
    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        portGroupId = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
    }
    else
    {
        portGroupId =
            PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.ptpNextPortGroupToServe;
    }


    while (entryFound == GT_FALSE)
    {
        /* start with bmp of all active port groups */
        activePortGroupsBmp = PRV_CPSS_PP_MAC(devNum)->
                                    portGroupsInfo.activePortGroupsBmp;

        /* look for port group to query */
        while(0 == (activePortGroupsBmp & (1 << portGroupId)))
        {
            if(portGroupId > PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup)
            {
                portGroupId = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
                break;
            }

            portGroupId++;
        }


        /* Read the Lsb bits of the message */
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &hwData0);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Read the Msb bits of the message */
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr1, &hwData1);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* check if it is the last entry in this port group */
        if (((hwData0 & 0x3FFFFFF) == 0x3FFFFFF) & ((hwData1 & 0x3FFFFFFF) == 0x3FFFFFFF))
        {
            if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
            {
                break;
            }

            portGroupsCheckedBmp |= (1 << portGroupId);

            if (portGroupsCheckedBmp == activePortGroupsBmp)
            {
                /* getting here means, that all active port groups were checked
                   in current round and no entries were found */
                break;
            }

            portGroupId++;
        }
        else
        {
            entryFound = GT_TRUE;
        }
    }

    if (entryFound == GT_TRUE)
    {
        entryPtr->portNum =
            (GT_U8)PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(
                                    devNum, portGroupId, (hwData0 & 0x3f));
        entryPtr->messageType = ((hwData0 >> 6) & 0xf);
        entryPtr->sequenceId = ((hwData0 >> 10) & 0xffff);
        entryPtr->timeStampVal = (hwData1 & 0x3fffffff);

        /* save the info about the next port group to be served */
        PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.secureBreachNextPortGroupToServe =
            portGroupId + 1;
    }
    else
    {
        rc = GT_NO_MORE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPtpTimestampEntryGet
*
* DESCRIPTION:
*       Read current Timestamp entry from the timestamp FIFO queue.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
* OUTPUTS:
*       entryPtr        - (pointer to) timestamp entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  Timestamp entries
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTimestampEntryGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC   *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTimestampEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, direction, entryPtr));

    rc = internal_cpssDxChPtpTimestampEntryGet(devNum, direction, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, direction, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/******************************************************************************/
/******************************************************************************/
/******* start of functions with portGroupsBmp parameter **********************/
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
* internal_cpssDxChPtpPortGroupTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*                                  or function
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortGroupTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_GROUPS_BMP                       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (function)
    {
        case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E:
            hwData = 1;
            break;
        case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E:
            hwData = 2;
            break;
        case CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E:
            hwData = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, 4, 2, hwData);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalConfig;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
        }

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, 4, 2, hwData);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet()
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*                                  or function
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_GROUPS_BMP                       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortGroupTodCounterFunctionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, function));

    rc = internal_cpssDxChPtpPortGroupTodCounterFunctionSet(devNum, portGroupsBmp, direction, function);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, function));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortGroupTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortGroupTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwData;           /* data to write to hw              */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(functionPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.
                ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                ptpGlobalConfig;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                            regAddr, 4, 2, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }


    switch (hwData)
    {
        case 0:
            *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_UPDATE_E;
            break;
        case 1:
            *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_INCREMENT_E;
            break;
        case 2:
            *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_CAPTURE_E;
            break;
        case 3:
            *functionPtr = CPSS_DXCH_PTP_TOD_COUNTER_FUNC_GENERATE_E;
            break;
        default:
            /* should never get here */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an
*       external interface.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortGroupTodCounterFunctionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, functionPtr));

    rc = internal_cpssDxChPtpPortGroupTodCounterFunctionGet(devNum, portGroupsBmp, direction, functionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, functionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT  direction
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_BOOL   isTriggerOn;   /* trigger status flag: GT_TRUE - trigger is on */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    /* Check trigger status for Rx/Tx or both directions */

    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        /* read trigger status */
        rc = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(
                                                devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                CPSS_PORT_DIRECTION_RX_E,
                                                &isTriggerOn);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Check if trigger was already cleared (action finished) */
        if (isTriggerOn == GT_TRUE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        /* read trigger status */
        rc = cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(
                                                devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                CPSS_PORT_DIRECTION_TX_E,
                                                &isTriggerOn);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Check if trigger was already cleared (TOD function finished) */
        if (isTriggerOn == GT_TRUE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    /* Set the trigger */

    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpGlobalConfig;

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, 6, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpGlobalConfig;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpGlobalConfig;
        }

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, 6, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionTriggerSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT  direction
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortGroupTodCounterFunctionTriggerSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction));

    rc = internal_cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(devNum, portGroupsBmp, direction);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.
                ptpGlobalConfig;
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                ptpGlobalConfig;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check that all port groups are ready */
    rc = prvCpssPortGroupBusyWait(devNum, portGroupsBmp, regAddr, 6,
                                      GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        rc = GT_OK;
        /* not all port groups has the bit with value 0 */
        *enablePtr = GT_TRUE;
    }
    else
    {
        *enablePtr = GT_FALSE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterFunctionTriggerGet
(
    IN GT_U8                    devNum,
    IN GT_PORT_GROUPS_BMP       portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortGroupTodCounterFunctionTriggerGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, enablePtr));

    rc = internal_cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(devNum, portGroupsBmp, direction, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpPortGroupTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - (pointer to) TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortGroupTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
{
    GT_U32    regAddr;          /* register address                 */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    i;                /* loop iterator                    */
    GT_U32    nanoSecHwVal;     /* hw value of nanoseconds          */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(todCounterPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
        case CPSS_PORT_DIRECTION_TX_E:
        case CPSS_PORT_DIRECTION_BOTH_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Check if TOD nanoseconds errata is enabled */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E))
    {
        /* nanoseconds value should be multiplies of 20ns */
        nanoSecHwVal = (todCounterPtr->nanoSeconds / 20);
        nanoSecHwVal *= 20;
    }
    else
    {
        nanoSecHwVal = todCounterPtr->nanoSeconds;
    }


    if ((direction == CPSS_PORT_DIRECTION_RX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpTodCntrShadowNanoSeconds;

            /* Write nanoseconds */
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                      regAddr, nanoSecHwVal);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Write two words of seconds */
            for (i = 0; i < 2; i++)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ptpTodCntrShadowSeconds[i];

                rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                          regAddr, todCounterPtr->seconds.l[i]);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    if ((direction == CPSS_PORT_DIRECTION_TX_E) ||
        (direction == CPSS_PORT_DIRECTION_BOTH_E))
    {
        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpTodCntrShadowNanoSeconds;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpTodCntrShadowNanoSeconds;
            }

            /* Write nanoseconds */
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                      regAddr, nanoSecHwVal);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Write two words of seconds */
            for (i = 0; i < 2; i++)
            {
                if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                {
                    regAddr = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpTodCntrShadowSeconds[i];
                }
                else
                {
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ptpTodCntrShadowSeconds[i];
                }

                rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                          regAddr, todCounterPtr->seconds.l[i]);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - (pointer to) TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortGroupTodCounterShadowSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, todCounterPtr));

    rc = internal_cpssDxChPtpPortGroupTodCounterShadowSet(devNum, portGroupsBmp, direction, todCounterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, todCounterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortGroupTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortGroupTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_U32    regAddrSec;       /* register address                 */
    GT_U32    regAddrNanoSec;   /* register address                 */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(todCounterPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddrNanoSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTodCntrShadowNanoSeconds;
            regAddrSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTodCntrShadowSeconds[0];
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddrNanoSec = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpTodCntrShadowNanoSeconds;
                regAddrSec     = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpTodCntrShadowSeconds[0];
            }
            else
            {
                regAddrNanoSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    haRegs.ptpTodCntrShadowNanoSeconds;
                regAddrSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    haRegs.ptpTodCntrShadowSeconds[0];
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    /* Read nanoseconds */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrNanoSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->nanoSeconds = hwData;

    /* Read two words of seconds */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->seconds.l[0] = hwData;
    regAddrSec += 4;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->seconds.l[1] = hwData;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD time stamping shadow counter values.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortGroupTodCounterShadowGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, todCounterPtr));

    rc = internal_cpssDxChPtpPortGroupTodCounterShadowGet(devNum, portGroupsBmp, direction, todCounterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, todCounterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortGroupTodCounterGet
*
* DESCRIPTION:
*       Get TOD time stamping counter value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortGroupTodCounterGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_U32    regAddrSec;       /* register address                 */
    GT_U32    regAddrNanoSec;   /* register address                 */
    GT_U32    hwData;           /* data to read from hw             */
    GT_U32    portGroupId;      /* the port group Id - support multi-
                                                port-groups device  */
    GT_U32    rc;               /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(todCounterPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            regAddrNanoSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTodCntrNanoSeconds;
            regAddrSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.ptpTodCntrSeconds[0];
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                regAddrNanoSec = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpTodCntrNanoSeconds;
                regAddrSec     = PRV_DXCH_REG1_UNIT_ETS_MAC(devNum).ptpTodCntrSeconds[0];
            }
            else
            {
                regAddrNanoSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    haRegs.ptpTodCntrNanoSeconds;
                regAddrSec = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    haRegs.ptpTodCntrSeconds[0];
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    /* Read nanoseconds */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrNanoSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->nanoSeconds = hwData;

    /* Read two words of seconds */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->seconds.l[0] = hwData;
    regAddrSec += 4;

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddrSec, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    todCounterPtr->seconds.l[1] = hwData;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortGroupTodCounterGet
*
* DESCRIPTION:
*       Get TOD time stamping counter value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portGroupsBmp or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortGroupTodCounterGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortGroupTodCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, direction, todCounterPtr));

    rc = internal_cpssDxChPtpPortGroupTodCounterGet(devNum, portGroupsBmp, direction, todCounterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, direction, todCounterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiClockModeSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Clock Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockMode       - clock interface using mode.
*                         Output mode relevant to Global TAI Instance only.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiClockModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT        clockMode
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    switch (clockMode)
    {
        case CPSS_DXCH_PTP_TAI_CLOCK_MODE_DISABLED_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_PTP_TAI_CLOCK_MODE_OUTPUT_E:
            hwValue = 1;
            break;
        case CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_E:
            hwValue = 2;
            break;
        case CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_ADJUST_E:
            hwValue = 3;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 9, 2, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockModeSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Clock Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockMode       - clock interface using mode.
*                         Output mode relevant to Global TAI Instance only.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT        clockMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiClockModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, clockMode));

    rc = internal_cpssDxChPtpTaiClockModeSet(devNum, taiIdPtr, clockMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, clockMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiClockModeGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Clock Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockModePtr    - (pointer to) clock interface using mode.
*                         Output mode relevant to Global TAI Instance only.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiClockModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT        *clockModePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;      /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(clockModePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 9, 2, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hwValue)
    {
        case 0:
            *clockModePtr = CPSS_DXCH_PTP_TAI_CLOCK_MODE_DISABLED_E;
            break;
        case 1:
            *clockModePtr = CPSS_DXCH_PTP_TAI_CLOCK_MODE_OUTPUT_E;
            break;
        case 2:
            *clockModePtr = CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_E;
            break;
        case 3:
            *clockModePtr = CPSS_DXCH_PTP_TAI_CLOCK_MODE_INPUT_ADJUST_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /*never occurs*/
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockModeGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Clock Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockModePtr    - (pointer to) clock interface using mode.
*                         Output mode relevant to Global TAI Instance only.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT CPSS_DXCH_PTP_TAI_CLOCK_MODE_ENT        *clockModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiClockModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, clockModePtr));

    rc = internal_cpssDxChPtpTaiClockModeGet(devNum, taiIdPtr, clockModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, clockModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiInternalClockGenerateEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Internal Clock Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          - Internal Clock Generate Enable.
*                         GT_TRUE - the internal clock generator
*                         generates a clock signal
*                         GT_FALSE - the internal clock not generated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiInternalClockGenerateEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue = BOOL2BIT_MAC(enable);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 8, 1, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInternalClockGenerateEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Internal Clock Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          - Internal Clock Generate Enable.
*                         GT_TRUE - the internal clock generator
*                         generates a clock signal
*                         GT_FALSE - the internal clock not generated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInternalClockGenerateEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiInternalClockGenerateEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enable));

    rc = internal_cpssDxChPtpTaiInternalClockGenerateEnableSet(devNum, taiIdPtr, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiInternalClockGenerateEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Internal Clock Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) Internal Clock Generate Enable.
*                         GT_TRUE - the internal clock generator
*                         generates a clock
*                         GT_FALSE - the internal clock not generated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiInternalClockGenerateEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 8, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInternalClockGenerateEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Internal Clock Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) Internal Clock Generate Enable.
*                         GT_TRUE - the internal clock generator
*                         generates a clock
*                         GT_FALSE - the internal clock not generated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInternalClockGenerateEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiInternalClockGenerateEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enablePtr));

    rc = internal_cpssDxChPtpTaiInternalClockGenerateEnableGet(devNum, taiIdPtr, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) PTP Clock Adjust Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - PTP PClock Drift Adjust Enable.
*                        GT_TRUE - the PClk is affected by the <Drift Adjustment> or not.
*                        GT_FALSE - the PClk reflects the frequency of the free running TOD,
*                        without any frequency adjustments.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue = BOOL2BIT_MAC(enable);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 7, 1, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) PTP Clock Adjust Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - PTP PClock Drift Adjust Enable.
*                        GT_TRUE - the PClk is affected by the <Drift Adjustment> or not.
*                        GT_FALSE - the PClk reflects the frequency of the free running TOD,
*                        without any frequency adjustments.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enable));

    rc = internal_cpssDxChPtpTaiPtpPClockDriftAdjustEnableSet(devNum, taiIdPtr, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) PTP Clock Adjust Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) PTP PClock Drift Adjust Enable.
*                        GT_TRUE - the PClk is affected by the <Drift Adjustment> or not.
*                        GT_FALSE - the PClk reflects the frequency of the free running TOD,
*                        without any frequency adjustments.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 7, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) PTP Clock Adjust Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) PTP PClock Drift Adjust Enable.
*                        GT_TRUE - the PClk is affected by the <Drift Adjustment> or not.
*                        GT_FALSE - the PClk reflects the frequency of the free running TOD,
*                        without any frequency adjustments.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enablePtr));

    rc = internal_cpssDxChPtpTaiPtpPClockDriftAdjustEnableGet(devNum, taiIdPtr, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiCaptureOverrideEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Capture Override Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - Capture Override Enable.
*                        When both TOD Capture registers are valid and capture
*                        trigger arrives:
*                        GT_TRUE  - the TOD Capture Value0 is overwritten.
*                        GT_FALSE - the TOD Capture Value0 unchanged.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiCaptureOverrideEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue = BOOL2BIT_MAC(enable);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 6, 1, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiCaptureOverrideEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Capture Override Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - Capture Override Enable.
*                        When both TOD Capture registers are valid and capture
*                        trigger arrives:
*                        GT_TRUE  - the TOD Capture Value0 is overwritten.
*                        GT_FALSE - the TOD Capture Value0 unchanged.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiCaptureOverrideEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiCaptureOverrideEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enable));

    rc = internal_cpssDxChPtpTaiCaptureOverrideEnableSet(devNum, taiIdPtr, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiCaptureOverrideEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Capture Override Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) Capture Override Enable.
*                        When both TOD Capture registers are valid and capture
*                        trigger arrives:
*                        GT_TRUE  - the TOD Capture Value0 is overwritten.
*                        GT_FALSE - the TOD Capture Value0 unchanged.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiCaptureOverrideEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 6, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiCaptureOverrideEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Capture Override Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) Capture Override Enable.
*                        When both TOD Capture registers are valid and capture
*                        trigger arrives:
*                        GT_TRUE  - the TOD Capture Value0 is overwritten.
*                        GT_FALSE - the TOD Capture Value0 unchanged.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiCaptureOverrideEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiCaptureOverrideEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enablePtr));

    rc = internal_cpssDxChPtpTaiCaptureOverrideEnableGet(devNum, taiIdPtr, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiInputTriggersCountEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Count Input Triggers Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - Count Input Triggers Enable.
*                        GT_TRUE  - each incoming trigger is counted.
*                        GT_FALSE - incoming triggers not counted.
*                        see cpssDxChPtpTaiIncomingTriggerCounterSet/Get
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiInputTriggersCountEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue = BOOL2BIT_MAC(enable);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 5, 1, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInputTriggersCountEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Count Input Triggers Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable         - Count Input Triggers Enable.
*                        GT_TRUE  - each incoming trigger is counted.
*                        GT_FALSE - incoming triggers not counted.
*                        see cpssDxChPtpTaiIncomingTriggerCounterSet/Get
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInputTriggersCountEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_BOOL                                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiInputTriggersCountEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enable));

    rc = internal_cpssDxChPtpTaiInputTriggersCountEnableSet(devNum, taiIdPtr, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiInputTriggersCountEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Count Input Triggers Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) Count Input Triggers Enable.
*                        GT_TRUE  - each incoming trigger is counted.
*                        GT_FALSE - incoming triggers not counted.
*                        see cpssDxChPtpTaiIncomingTriggerCounterSet/Get
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiInputTriggersCountEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 5, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInputTriggersCountEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Count Input Triggers Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr      - (pointer to) Count Input Triggers Enable.
*                        GT_TRUE  - each incoming trigger is counted.
*                        GT_FALSE - incoming triggers not counted.
*                        see cpssDxChPtpTaiIncomingTriggerCounterSet/Get
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInputTriggersCountEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiInputTriggersCountEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enablePtr));

    rc = internal_cpssDxChPtpTaiInputTriggersCountEnableGet(devNum, taiIdPtr, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiExternalPulseWidthSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Pulse Width.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       extPulseWidth  - External Pulse Width.
*                        width of the pulse in nanoseconds of generated output trigger.
*                       (APPLICABLE RANGES: 0..2^28-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on bad extPulseWidth
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiExternalPulseWidthSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_U32                                  extPulseWidth
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    if (extPulseWidth >= BIT_28)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig1;
        hwValue = (extPulseWidth & 0xFFFF);
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 0, 16, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig2;
        hwValue = ((extPulseWidth >> 16) & 0x0FFF);
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 0, 12, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiExternalPulseWidthSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Pulse Width.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       extPulseWidth  - External Pulse Width.
*                        width of the pulse in nanoseconds of generated output trigger.
*                       (APPLICABLE RANGES: 0..2^28-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on bad extPulseWidth
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiExternalPulseWidthSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    IN  GT_U32                                  extPulseWidth
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiExternalPulseWidthSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, extPulseWidth));

    rc = internal_cpssDxChPtpTaiExternalPulseWidthSet(devNum, taiIdPtr, extPulseWidth);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, extPulseWidth));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiExternalPulseWidthGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Pulse Width
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       extPulseWidthPtr -  (pointer to) External Pulse Width.
*                           width of the pulse in nanoseconds of generated output trigger.
*                           (APPLICABLE RANGES: 0..2^28-1.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiExternalPulseWidthGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_U32                                  *extPulseWidthPtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(extPulseWidthPtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig1;
    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 0, 16, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    *extPulseWidthPtr = hwValue;

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig2;
    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 0, 12, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    *extPulseWidthPtr |= (hwValue << 16);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiExternalPulseWidthGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Pulse Width
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       taiIdPtr       - (pointer to) TAI Units identification.
*                        Single TAI unit must be specified.
*                        For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                        (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       extPulseWidthPtr -  (pointer to) External Pulse Width.
*                           width of the pulse in nanoseconds of generated output trigger.
*                           (APPLICABLE RANGES: 0..2^28-1.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiExternalPulseWidthGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC                *taiIdPtr,
    OUT GT_U32                                  *extPulseWidthPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiExternalPulseWidthGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, extPulseWidthPtr));

    rc = internal_cpssDxChPtpTaiExternalPulseWidthGet(devNum, taiIdPtr, extPulseWidthPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, extPulseWidthPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPtpTaiTodValueRegisterAddressGet
*
* DESCRIPTION:
*       Get TOD values register addresses.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       gopNumber       - GOP Number
*       taiNumber       - TAI Number
*       readUse         - GT_TRUE - read access, GT_FALSE - wrire access
*       todValueType    - type of TOD value.
*                         Valid types are for both read and write:
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E.
*                         Valid types are for read only:
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE0_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE1_E.
*
* OUTPUTS:
*       regAddrArr      - (pointer to) array of register addresses
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*
* COMMENTS:
*       This API does not activate any triggers, other APIs does it
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPtpTaiTodValueRegisterAddressGet
(
    IN  GT_U8                                 devNum,
    IN  GT_U32                                gopNumber,
    IN  GT_U32                                taiNumber,
    IN  GT_BOOL                               readUse,
    IN  CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT        todValueType,
    OUT GT_U32                                regAddrArr[7]
)
{
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance != GT_FALSE)
    {
        if ((gopNumber != 0) || (taiNumber != 0))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    switch (todValueType)
    {
        case CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E:
            regAddrArr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).triggerGenerationTODSecHigh;
            regAddrArr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).triggerGenerationTODSecMed;
            regAddrArr[2] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).triggerGenerationTODSecLow;
            regAddrArr[3] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).triggerGenerationTODNanoHigh;
            regAddrArr[4] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).triggerGenerationTODNanoLow;
            regAddrArr[5] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).triggerGenerationTODFracHigh;
            regAddrArr[6] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).triggerGenerationTODFracLow;
            break;
        case CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E:
            regAddrArr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).generateFunctionMaskSecHigh;
            regAddrArr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).generateFunctionMaskSecMed;
            regAddrArr[2] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).generateFunctionMaskSecLow;
            regAddrArr[3] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).generateFunctionMaskNanoHigh;
            regAddrArr[4] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).generateFunctionMaskNanoLow;
            regAddrArr[5] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).generateFunctionMaskFracHigh;
            regAddrArr[6] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).generateFunctionMaskFracLow;
            break;
        case CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E:
            regAddrArr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeLoadValueSecHigh;
            regAddrArr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeLoadValueSecMed;
            regAddrArr[2] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeLoadValueSecLow;
            regAddrArr[3] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeLoadValueNanoHigh;
            regAddrArr[4] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeLoadValueNanoLow;
            regAddrArr[5] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeLoadValueFracHigh;
            regAddrArr[6] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeLoadValueFracLow;
            break;
        case CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE0_E:
            if (readUse == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            regAddrArr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue0SecHigh;
            regAddrArr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue0SecMed;
            regAddrArr[2] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue0SecLow;
            regAddrArr[3] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue0NanoHigh;
            regAddrArr[4] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue0NanoLow;
            regAddrArr[5] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue0FracHigh;
            regAddrArr[6] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue0FracLow;
            break;
        case CPSS_DXCH_PTP_TAI_TOD_TYPE_CAPTURE_VALUE1_E:
            if (readUse == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            regAddrArr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue1SecHigh;
            regAddrArr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue1SecMed;
            regAddrArr[2] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue1SecLow;
            regAddrArr[3] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue1NanoHigh;
            regAddrArr[4] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue1NanoLow;
            regAddrArr[5] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue1FracHigh;
            regAddrArr[6] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, gopNumber, taiNumber).timeCaptureValue1FracLow;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiTodSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD values.
*       The TOD will be updated by triggering an appropriate function.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todValueType    - type of TOD value.
*                         Valid types are
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E.
*       todValuePtr     - (pointer to) TOD value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This API does not activate any triggers, other APIs does it
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiTodSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT        todValueType,
    IN  CPSS_DXCH_PTP_TOD_COUNT_STC           *todValuePtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[7];     /* register address */
    GT_U32                             hwValue[7];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(todValuePtr);

    hwValue[0] = todValuePtr->seconds.l[1] & 0xFFFF;
    hwValue[1] = (todValuePtr->seconds.l[0] >> 16) & 0xFFFF;
    hwValue[2] = todValuePtr->seconds.l[0] & 0xFFFF;
    hwValue[3] = (todValuePtr->nanoSeconds >> 16) & 0xFFFF;
    hwValue[4] = todValuePtr->nanoSeconds & 0xFFFF;
    hwValue[5] = (todValuePtr->fracNanoSeconds >> 16) & 0xFFFF;
    hwValue[6] = todValuePtr->fracNanoSeconds & 0xFFFF;

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        rc = prvCpssDxChPtpTaiTodValueRegisterAddressGet(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber,
            GT_FALSE /*readUse*/, todValueType, regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (i = 0; (i < 7); i++)
        {
            rc = prvCpssDrvHwPpSetRegField(
                devNum, regAddr[i], 0, 16, hwValue[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD values.
*       The TOD will be updated by triggering an appropriate function.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todValueType    - type of TOD value.
*                         Valid types are
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_MASK_E,
*                         CPSS_DXCH_PTP_TAI_TOD_TYPE_LOAD_VALUE_E.
*       todValuePtr     - (pointer to) TOD value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This API does not activate any triggers, other APIs does it
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT        todValueType,
    IN  CPSS_DXCH_PTP_TOD_COUNT_STC           *todValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiTodSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, todValueType, todValuePtr));

    rc = internal_cpssDxChPtpTaiTodSet(devNum, taiIdPtr, todValueType, todValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, todValueType, todValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiTodGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD values.
*       The TOD was captured by triggering an appropriate function.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todValueType    - type of TOD value.
*
* OUTPUTS:
*       todValuePtr     - (pointer to) TOD value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiTodGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT        todValueType,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC           *todValuePtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[7];     /* register address */
    GT_U32                             hwValue[7];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(todValuePtr);

    cpssOsMemSet(todValuePtr, 0, sizeof(*todValuePtr));

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    rc = prvCpssDxChPtpTaiTodValueRegisterAddressGet(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber,
        GT_TRUE /*readUse*/, todValueType, regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0; (i < 7); i++)
    {
        rc = prvCpssDrvHwPpGetRegField(
            devNum, regAddr[i], 0, 16, &(hwValue[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    todValuePtr->seconds.l[1]    = hwValue[0];
    todValuePtr->seconds.l[0]    = ((hwValue[1] << 16) | hwValue[2]);
    todValuePtr->nanoSeconds     = ((hwValue[3] << 16) | hwValue[4]);
    todValuePtr->fracNanoSeconds = ((hwValue[5] << 16) | hwValue[6]);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD values.
*       The TOD was captured by triggering an appropriate function.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todValueType    - type of TOD value.
*
* OUTPUTS:
*       todValuePtr     - (pointer to) TOD value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_TYPE_ENT        todValueType,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC           *todValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiTodGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, todValueType, todValuePtr));

    rc = internal_cpssDxChPtpTaiTodGet(devNum, taiIdPtr, todValueType, todValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, todValueType, todValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiOutputTriggerEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Output Trigger Generation Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          -  enable output trigger generation.
*                          GT_TRUE  - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       use cpssDxChPtpTaiTodSet with
*       CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E and
*       the time to trigger generation.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiOutputTriggerEnableSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_BOOL                               enable
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue = BOOL2BIT_MAC(enable);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 1, 1, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiOutputTriggerEnableSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Output Trigger Generation Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          -  enable output trigger generation.
*                          GT_TRUE  - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       use cpssDxChPtpTaiTodSet with
*       CPSS_DXCH_PTP_TAI_TOD_TYPE_TRIGGER_GENERATION_E and
*       the time to trigger generation.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiOutputTriggerEnableSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_BOOL                               enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiOutputTriggerEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enable));

    rc = internal_cpssDxChPtpTaiOutputTriggerEnableSet(devNum, taiIdPtr, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiOutputTriggerEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Output Trigger Generation Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) enable output trigger generation.
*                         GT_TRUE  - enable GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiOutputTriggerEnableGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_BOOL                               *enablePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCntrFunctionConfig0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 1, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiOutputTriggerEnableGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Output Trigger Generation Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to) enable output trigger generation.
*                         GT_TRUE  - enable GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiOutputTriggerEnableGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_BOOL                               *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiOutputTriggerEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enablePtr));

    rc = internal_cpssDxChPtpTaiOutputTriggerEnableGet(devNum, taiIdPtr, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiTodStepSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todStepPtr      - (pointer to) TOD Step.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiTodStepSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_STEP_STC    *todStepPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[4];     /* register address */
    GT_U32                             hwValue[4];     /* HW data  */
    GT_U32                             i;              /* loop index       */
    GT_U32                             firstRegIdx;    /* index of first register for configuration */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(todStepPtr);

    if(!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        hwValue[0] = (todStepPtr->nanoSeconds >> 16) & 0xFFFF;
        firstRegIdx = 0;
    }
    else
    {
        /* only 16 bits are used for nanoseconds starting from Caelum (BobK) */
        if (todStepPtr->nanoSeconds >= BIT_16)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        firstRegIdx = 1;
    }

    hwValue[1] = todStepPtr->nanoSeconds & 0xFFFF;
    hwValue[2] = (todStepPtr->fracNanoSeconds >> 16) & 0xFFFF;
    hwValue[3] = todStepPtr->fracNanoSeconds & 0xFFFF;

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).TODStepNanoConfigHigh;
        regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).TODStepNanoConfigLow;
        regAddr[2] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).TODStepFracConfigHigh;
        regAddr[3] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).TODStepFracConfigLow;

        for (i = firstRegIdx; (i < 4); i++)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr[i], 0, 16, hwValue[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodStepSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       todStepPtr      - (pointer to) TOD Step.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodStepSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    IN  CPSS_DXCH_PTP_TAI_TOD_STEP_STC    *todStepPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiTodStepSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, todStepPtr));

    rc = internal_cpssDxChPtpTaiTodStepSet(devNum, taiIdPtr, todStepPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, todStepPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiTodStepGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       todStepPtr      - (pointer to) TOD Step.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiTodStepGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    OUT CPSS_DXCH_PTP_TAI_TOD_STEP_STC    *todStepPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[4];     /* register address */
    GT_U32                             hwValue[4];     /* HW data  */
    GT_U32                             i;              /* loop index       */
    GT_U32                             firstRegIdx;    /* index of first register for configuration */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(todStepPtr);

    cpssOsMemSet(todStepPtr, 0, sizeof(*todStepPtr));

    if(!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        firstRegIdx = 0;
    }
    else
    {
        /* only 16 bits are used for nanoseconds starting from Caelum (BobK) */
        firstRegIdx = 1;
        hwValue[0]  = 0;
    }

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).TODStepNanoConfigHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).TODStepNanoConfigLow;
    regAddr[2] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).TODStepFracConfigHigh;
    regAddr[3] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).TODStepFracConfigLow;

    for (i = firstRegIdx; (i < 4); i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr[i], 0, 16, &(hwValue[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    todStepPtr->nanoSeconds     = ((hwValue[0] << 16) | hwValue[1]);
    todStepPtr->fracNanoSeconds = ((hwValue[2] << 16) | hwValue[3]);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodStepGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       todStepPtr      - (pointer to) TOD Step.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodStepGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    OUT CPSS_DXCH_PTP_TAI_TOD_STEP_STC    *todStepPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiTodStepGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, todStepPtr));

    rc = internal_cpssDxChPtpTaiTodStepGet(devNum, taiIdPtr, todStepPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, todStepPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPulseDelaySet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Trigger Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds delay.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPulseDelaySet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue[0] = (nanoSeconds >> 16) & 0xFFFF;
    hwValue[1] = nanoSeconds & 0xFFFF;

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).
                timeAdjustmentPropagationDelayConfigHigh;
        regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).
                timeAdjustmentPropagationDelayConfigLow;

        for (i = 0; (i < 2); i++)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr[i], 0, 16, hwValue[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPulseDelaySet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Trigger Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds delay.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPulseDelaySet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPulseDelaySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, nanoSeconds));

    rc = internal_cpssDxChPtpTaiPulseDelaySet(devNum, taiIdPtr, nanoSeconds);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, nanoSeconds));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPulseDelayGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Trigger Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPulseDelayGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(nanoSecondsPtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).
            timeAdjustmentPropagationDelayConfigHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).
            timeAdjustmentPropagationDelayConfigLow;

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr[i], 0, 16, &(hwValue[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *nanoSecondsPtr = ((hwValue[0] << 16) | hwValue[1]);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPulseDelayGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Trigger Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPulseDelayGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPulseDelayGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, nanoSecondsPtr));

    rc = internal_cpssDxChPtpTaiPulseDelayGet(devNum, taiIdPtr, nanoSecondsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, nanoSecondsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiClockDelaySet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Clock Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds delay.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiClockDelaySet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue[0] = (nanoSeconds >> 16) & 0xFFFF;
    hwValue[1] = nanoSeconds & 0xFFFF;

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).
                externalClockPropagationDelayConfigHigh;
        regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).
                externalClockPropagationDelayConfigLow;

        for (i = 0; (i < 2); i++)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr[i], 0, 16, hwValue[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockDelaySet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) External Clock Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds delay.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockDelaySet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiClockDelaySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, nanoSeconds));

    rc = internal_cpssDxChPtpTaiClockDelaySet(devNum, taiIdPtr, nanoSeconds);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, nanoSeconds));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiClockDelayGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Clock Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiClockDelayGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(nanoSecondsPtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).
            externalClockPropagationDelayConfigHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).
            externalClockPropagationDelayConfigLow;

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr[i], 0, 16, &(hwValue[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *nanoSecondsPtr = ((hwValue[0] << 16) | hwValue[1]);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockDelayGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) External Clock Propagation Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockDelayGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiClockDelayGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, nanoSecondsPtr));

    rc = internal_cpssDxChPtpTaiClockDelayGet(devNum, taiIdPtr, nanoSecondsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, nanoSecondsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiFractionalNanosecondDriftSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Fractional Nanosecond Drift.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       fracNanoSecond  - (pointer to) fractional nano seconds drift.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiFractionalNanosecondDriftSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_32                                 fracNanoSecond
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue[0] = (fracNanoSecond >> 16) & 0xFFFF;
    hwValue[1] = fracNanoSecond & 0xFFFF;

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).driftAdjustmentConfigHigh;
        regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).driftAdjustmentConfigLow;

        for (i = 0; (i < 2); i++)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr[i], 0, 16, hwValue[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiFractionalNanosecondDriftSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Fractional Nanosecond Drift.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       fracNanoSecond  - (pointer to) fractional nano seconds drift.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiFractionalNanosecondDriftSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_32                                 fracNanoSecond
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiFractionalNanosecondDriftSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, fracNanoSecond));

    rc = internal_cpssDxChPtpTaiFractionalNanosecondDriftSet(devNum, taiIdPtr, fracNanoSecond);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, fracNanoSecond));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiFractionalNanosecondDriftGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Fractional Nanosecond Drift.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       fracNanoSecondPtr   - (pointer to) fractional nano seconds drift.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiFractionalNanosecondDriftGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_32                                 *fracNanoSecondPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(fracNanoSecondPtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).driftAdjustmentConfigHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).driftAdjustmentConfigLow;

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr[i], 0, 16, &(hwValue[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *fracNanoSecondPtr = ((hwValue[0] << 16) | hwValue[1]);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiFractionalNanosecondDriftGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Fractional Nanosecond Drift.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       fracNanoSecondPtr   - (pointer to) fractional nano seconds drift.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiFractionalNanosecondDriftGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_32                                 *fracNanoSecondPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiFractionalNanosecondDriftGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, fracNanoSecondPtr));

    rc = internal_cpssDxChPtpTaiFractionalNanosecondDriftGet(devNum, taiIdPtr, fracNanoSecondPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, fracNanoSecondPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPClockCycleSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) PClock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds cycle.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPClockCycleSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    hwValue[0] = (nanoSeconds >> 16) & 0xFFFF;
    hwValue[1] = nanoSeconds & 0xFFFF;

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).PClkClockCycleConfigHigh;
        regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).PClkClockCycleConfigLow;

        for (i = 0; (i < 2); i++)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr[i], 0, 16, hwValue[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPClockCycleSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) PClock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       nanoSeconds     - nano seconds cycle.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPClockCycleSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                nanoSeconds
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPClockCycleSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, nanoSeconds));

    rc = internal_cpssDxChPtpTaiPClockCycleSet(devNum, taiIdPtr, nanoSeconds);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, nanoSeconds));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPClockCycleGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) PClock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds cycle.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPClockCycleGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(nanoSecondsPtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).PClkClockCycleConfigHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).PClkClockCycleConfigLow;

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr[i], 0, 16, &(hwValue[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *nanoSecondsPtr = ((hwValue[0] << 16) | hwValue[1]);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPClockCycleGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) PClock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       nanoSecondsPtr  - (pointer to) nano seconds cycle.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPClockCycleGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *nanoSecondsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPClockCycleGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, nanoSecondsPtr));

    rc = internal_cpssDxChPtpTaiPClockCycleGet(devNum, taiIdPtr, nanoSecondsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, nanoSecondsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiClockCycleSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Clock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       seconds         - seconds part of cycle.
*                         (APPLICABLE RANGES: 0..3.)
*       nanoSeconds     - nano seconds part of cycle.
*                         (APPLICABLE RANGES: 0..2^28-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiClockCycleSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                seconds,
    IN  GT_U32                                nanoSeconds
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    if (seconds >= BIT_2)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (nanoSeconds >= BIT_30)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwValue[0] = ((nanoSeconds >> 16) & 0x3FFF) | ((seconds & 3) << 14);
    hwValue[1] = nanoSeconds & 0xFFFF;

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).clockCycleConfigHigh;
        regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).clockCycleConfigLow;

        for (i = 0; (i < 2); i++)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr[i], 0, 16, hwValue[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockCycleSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Clock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       seconds         - seconds part of cycle.
*                         (APPLICABLE RANGES: 0..3.)
*       nanoSeconds     - nano seconds part of cycle.
*                         (APPLICABLE RANGES: 0..2^28-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockCycleSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                seconds,
    IN  GT_U32                                nanoSeconds
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiClockCycleSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, seconds, nanoSeconds));

    rc = internal_cpssDxChPtpTaiClockCycleSet(devNum, taiIdPtr, seconds, nanoSeconds);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, seconds, nanoSeconds));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiClockCycleGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Clock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       secondsPtr      - (pointer to) seconds part of cycle.
*                         (APPLICABLE RANGES: 0..3.)
*       nanoSecondsPtr  - (pointer to) nano seconds part of cycle.
*                         (APPLICABLE RANGES: 0..2^28-1.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiClockCycleGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *secondsPtr,
    OUT GT_U32                                *nanoSecondsPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(secondsPtr);
    CPSS_NULL_PTR_CHECK_MAC(nanoSecondsPtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).clockCycleConfigHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).clockCycleConfigLow;

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr[i], 0, 16, &(hwValue[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *secondsPtr     = ((hwValue[0] >> 14) & 3);
    *nanoSecondsPtr = (((hwValue[0] & 0x3FFF) << 16) | hwValue[1]);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiClockCycleGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Clock Cycle.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       secondsPtr      - (pointer to) seconds part of cycle.
*                         (APPLICABLE RANGES: 0..3.)
*       nanoSecondsPtr  - (pointer to) nano seconds part of cycle.
*                         (APPLICABLE RANGES: 0..2^28-1.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiClockCycleGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *secondsPtr,
    OUT GT_U32                                *nanoSecondsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiClockCycleGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, secondsPtr, nanoSecondsPtr));

    rc = internal_cpssDxChPtpTaiClockCycleGet(devNum, taiIdPtr, secondsPtr, nanoSecondsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, secondsPtr, nanoSecondsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiTodCaptureStatusSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD Capture Status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       captureIndex    - Capture Index.
*                         (APPLICABLE RANGES: 0..1.)
*       valid           - TOD Capture is valid.
*                         GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiTodCaptureStatusSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                captureIndex,
    IN  GT_BOOL                               valid
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    /* field became read only starting from Bobcat2 B0 */
    if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    if (captureIndex > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwValue = BOOL2BIT_MAC(valid);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCaptureStatus;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, captureIndex, 1, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodCaptureStatusSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) TOD Capture Status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       captureIndex    - Capture Index.
*                         (APPLICABLE RANGES: 0..1.)
*       valid           - TOD Capture is valid.
*                         GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodCaptureStatusSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                captureIndex,
    IN  GT_BOOL                               valid
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiTodCaptureStatusSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, captureIndex, valid));

    rc = internal_cpssDxChPtpTaiTodCaptureStatusSet(devNum, taiIdPtr, captureIndex, valid);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, captureIndex, valid));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiTodCaptureStatusGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Capture Status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       captureIndex    - Capture Index.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       validPtr         - (pointer to)Capture is valid.
*                          GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiTodCaptureStatusGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                captureIndex,
    OUT GT_BOOL                               *validPtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);

    if (captureIndex > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeCaptureStatus;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, captureIndex, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *validPtr = BIT2BOOL_MAC(hwValue);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodCaptureStatusGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Capture Status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       captureIndex    - Capture Index.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       validPtr         - (pointer to)Capture is valid.
*                          GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodCaptureStatusGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                captureIndex,
    OUT GT_BOOL                               *validPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiTodCaptureStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, captureIndex, validPtr));

    rc = internal_cpssDxChPtpTaiTodCaptureStatusGet(devNum, taiIdPtr, captureIndex, validPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, captureIndex, validPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiTodUpdateCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Update Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       valuePtr        - (pointer to)TOD Update Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counter is Clear On Read. Value incremented at each
*       TOD Update/Increment/Decrement function.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiTodUpdateCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *valuePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).timeUpdateCntr;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 0, 16, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *valuePtr = hwValue;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiTodUpdateCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) TOD Update Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       valuePtr        - (pointer to)TOD Update Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counter is Clear On Read. Value incremented at each
*       TOD Update/Increment/Decrement function.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodUpdateCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiTodUpdateCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, valuePtr));

    rc = internal_cpssDxChPtpTaiTodUpdateCounterGet(devNum, taiIdPtr, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiIncomingTriggerCounterSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Incoming Trigger Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       value           - value to set.
*                         (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiIncomingTriggerCounterSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                value
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    if (value >= BIT_8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwValue = value;

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).captureTriggerCntr;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 0, 8, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiIncomingTriggerCounterSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Incoming Trigger Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       value           - value to set.
*                         (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiIncomingTriggerCounterSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiIncomingTriggerCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, value));

    rc = internal_cpssDxChPtpTaiIncomingTriggerCounterSet(devNum, taiIdPtr, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiIncomingTriggerCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Incoming Trigger Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       valuePtr        - (pointer to)value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiIncomingTriggerCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *valuePtr
)
{
    GT_STATUS                          rc;          /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator; /* TAI iterator     */
    GT_U32                             regAddr;     /* register address */
    GT_U32                             hwValue;     /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).captureTriggerCntr;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 0, 8, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *valuePtr = hwValue;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiIncomingTriggerCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Incoming Trigger Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       valuePtr        - (pointer to)value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiIncomingTriggerCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiIncomingTriggerCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, valuePtr));

    rc = internal_cpssDxChPtpTaiIncomingTriggerCounterGet(devNum, taiIdPtr, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiIncomingClockCounterSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Incoming Clock Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          - enable counting.
*                         GT_TRUE - enable, GT_FALSE - disable.
*       value           - value to set.
*                         (APPLICABLE RANGES: 0..2^16-1.)
*                         (APPLICABLE DEVICES: Bobcat2.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter became Read Only and Clear on Read starting from Caelum devices.
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiIncomingClockCounterSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_BOOL                               enable,
    IN  GT_U32                                value
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    /* the value parameter is ignored starting from Caelum devices */
    if ((value >= BIT_16) && (!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).
                incomingClockInCountingConfigHigh;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr, 0, 1,
                (BOOL2BIT_MAC(enable)));
        if (rc != GT_OK)
        {
            return rc;
        }

        /* the value parameter is ignored starting from Caelum devices */
        if(!PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
                devNum, taiIterator.gopNumber, taiIterator.taiNumber).
                    incomingClockInCountingConfigLow;

            rc = prvCpssDrvHwPpPortGroupSetRegField(
                    devNum, taiIterator.portGroupId, regAddr, 0, 16, value);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiIncomingClockCounterSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Incoming Clock Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       enable          - enable counting.
*                         GT_TRUE - enable, GT_FALSE - disable.
*       value           - value to set.
*                         (APPLICABLE RANGES: 0..2^16-1.)
*                         (APPLICABLE DEVICES: Bobcat2.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter became Read Only and Clear on Read starting from Caelum devices.
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiIncomingClockCounterSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_BOOL                               enable,
    IN  GT_U32                                value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiIncomingClockCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enable, value));

    rc = internal_cpssDxChPtpTaiIncomingClockCounterSet(devNum, taiIdPtr, enable, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enable, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiIncomingClockCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Incoming Clock Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to)enable counting.
*                         GT_TRUE - enable, GT_FALSE - disable.
*       valuePtr        - (pointer to)value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiIncomingClockCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_BOOL                               *enablePtr,
    OUT GT_U32                                *valuePtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr;        /* register address */
    GT_U32                             hwValue;        /* HW data  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).
            incomingClockInCountingConfigHigh;
    rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr, 0, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr  = BIT2BOOL_MAC(hwValue);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).
            incomingClockInCountingConfigLow;

    return prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr, 0, 16, valuePtr);
}

/*******************************************************************************
* cpssDxChPtpTaiIncomingClockCounterGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Incoming Clock Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       enablePtr       - (pointer to)enable counting.
*                         GT_TRUE - enable, GT_FALSE - disable.
*       valuePtr        - (pointer to)value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiIncomingClockCounterGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_BOOL                               *enablePtr,
    OUT GT_U32                                *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiIncomingClockCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, enablePtr, valuePtr));

    rc = internal_cpssDxChPtpTaiIncomingClockCounterGet(devNum, taiIdPtr, enablePtr, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, enablePtr, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiFrequencyDriftThesholdsSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Frequency Drift Thesholds.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       interruptThreshold - the drift value threshold causing interrupt.
*                            Notifies about the clock frequency out of sync.
*                           (APPLICABLE RANGES: 0..2^24-1.)
*       adjustThreshold    - the drift value threshold that triggers
*                            the adjustment logic. Prevents minor jitters
*                            from affecting the drift adjustment.
*                           (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiFrequencyDriftThesholdsSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                interruptThreshold,
    IN  GT_U32                                adjustThreshold
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    if (interruptThreshold >= BIT_24)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (adjustThreshold >= BIT_8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwValue[0] = ((interruptThreshold >> 8) & 0xFFFF);
    hwValue[1] = (((interruptThreshold & 0xFF) << 8) | (adjustThreshold & 0xFF));

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).driftThresholdConfigHigh;
        regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).driftThresholdConfigLow;

        for (i = 0; (i < 2); i++)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
                devNum, taiIterator.portGroupId, regAddr[i], 0, 16, hwValue[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiFrequencyDriftThesholdsSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Frequency Drift Thesholds.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       interruptThreshold - the drift value threshold causing interrupt.
*                            Notifies about the clock frequency out of sync.
*                           (APPLICABLE RANGES: 0..2^24-1.)
*       adjustThreshold    - the drift value threshold that triggers
*                            the adjustment logic. Prevents minor jitters
*                            from affecting the drift adjustment.
*                           (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiFrequencyDriftThesholdsSet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    IN  GT_U32                                interruptThreshold,
    IN  GT_U32                                adjustThreshold
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiFrequencyDriftThesholdsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, interruptThreshold, adjustThreshold));

    rc = internal_cpssDxChPtpTaiFrequencyDriftThesholdsSet(devNum, taiIdPtr, interruptThreshold, adjustThreshold);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, interruptThreshold, adjustThreshold));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiFrequencyDriftThesholdsGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Frequency Drift Thesholds.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       interruptThresholdPtr - (pointer to)the drift value threshold causing interrupt.
*                               Notifies about the clock frequency out of sync.
*                               (APPLICABLE RANGES: 0..2^24-1.)
*       adjustThresholdPtr    - (pointer to)the drift value threshold that triggers
*                               the adjustment logic. Prevents minor jitters
*                               from affecting the drift adjustment.
*                               (APPLICABLE RANGES: 0..255.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiFrequencyDriftThesholdsGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *interruptThresholdPtr,
    OUT GT_U32                                *adjustThresholdPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr[2];     /* register address */
    GT_U32                             hwValue[2];     /* HW data  */
    GT_U32                             i;              /* loop index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(interruptThresholdPtr);
    CPSS_NULL_PTR_CHECK_MAC(adjustThresholdPtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).driftThresholdConfigHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).driftThresholdConfigLow;

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, taiIterator.portGroupId, regAddr[i], 0, 16, &(hwValue[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *interruptThresholdPtr  = (hwValue[0] << 8) | ((hwValue[1] >> 8) & 0xFF);
    *adjustThresholdPtr     = (hwValue[1] & 0xFF);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiFrequencyDriftThesholdsGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Frequency Drift Thesholds.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       interruptThresholdPtr - (pointer to)the drift value threshold causing interrupt.
*                               Notifies about the clock frequency out of sync.
*                               (APPLICABLE RANGES: 0..2^24-1.)
*       adjustThresholdPtr    - (pointer to)the drift value threshold that triggers
*                               the adjustment logic. Prevents minor jitters
*                               from affecting the drift adjustment.
*                               (APPLICABLE RANGES: 0..255.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiFrequencyDriftThesholdsGet
(
    IN  GT_U8                                 devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC              *taiIdPtr,
    OUT GT_U32                                *interruptThresholdPtr,
    OUT GT_U32                                *adjustThresholdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiFrequencyDriftThesholdsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, interruptThresholdPtr, adjustThresholdPtr));

    rc = internal_cpssDxChPtpTaiFrequencyDriftThesholdsGet(devNum, taiIdPtr, interruptThresholdPtr, adjustThresholdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, interruptThresholdPtr, adjustThresholdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiGracefulStepSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Graceful Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       gracefulStep    - Graceful Step in nanoseconds.
*                         (APPLICABLE RANGES: 0..31.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on wrong gracefulStep
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiGracefulStepSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    IN  GT_U32                             gracefulStep
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);

    if (gracefulStep >= BIT_5)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_START_MAC(devNum, taiIdPtr, taiIterator)
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
            devNum, taiIterator.gopNumber, taiIterator.taiNumber).TAICtrlReg0;

        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, taiIterator.portGroupId, regAddr, 2, 5, gracefulStep);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_LOOP_END_MAC(devNum, taiIdPtr, taiIterator)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiGracefulStepSet
*
* DESCRIPTION:
*       Set TAI (Time Application Interface) Graceful Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       gracefulStep    - Graceful Step in nanoseconds.
*                         (APPLICABLE RANGES: 0..31.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on wrong gracefulStep
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiGracefulStepSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    IN  GT_U32                             gracefulStep
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiGracefulStepSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, gracefulStep));

    rc = internal_cpssDxChPtpTaiGracefulStepSet(devNum, taiIdPtr, gracefulStep);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, gracefulStep));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiGracefulStepGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Graceful Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       gracefulStepPtr - (pointer to) Graceful Step in nanoseconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiGracefulStepGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    OUT GT_U32                            *gracefulStepPtr
)
{
    GT_STATUS                          rc;             /* return code      */
    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_STC taiIterator;    /* TAI iterator     */
    GT_U32                             regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_NOT_SINGLE_TAI_PTR_CHECK_MAC(devNum, taiIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(gracefulStepPtr);

    PRV_CPSS_DXCH_PTP_TAI_ITERATOR_GET_SINGLE_MAC(devNum, taiIdPtr, taiIterator);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(
        devNum, taiIterator.gopNumber, taiIterator.taiNumber).TAICtrlReg0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, taiIterator.portGroupId, regAddr, 2, 5, gracefulStepPtr);

    return rc;

}

/*******************************************************************************
* cpssDxChPtpTaiGracefulStepGet
*
* DESCRIPTION:
*       Get TAI (Time Application Interface) Graceful Step.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       taiIdPtr        - (pointer to) TAI Units identification.
*                         Single TAI unit must be specified.
*                         For devices that have single TAI unit (Caelum) taiIdPtr is ignored.
*                         (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       gracefulStepPtr - (pointer to) Graceful Step in nanoseconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiGracefulStepGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_PTP_TAI_ID_STC          *taiIdPtr,
    OUT GT_U32                            *gracefulStepPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiGracefulStepGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiIdPtr, gracefulStepPtr));

    rc = internal_cpssDxChPtpTaiGracefulStepGet(devNum, taiIdPtr, gracefulStepPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiIdPtr, gracefulStepPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsuControlSet
*
* DESCRIPTION:
*       Set TSU (Time Stamp Unit) Control.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       controlPtr      - (pointer to) control structure.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsuControlSet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    IN  CPSS_DXCH_PTP_TSU_CONTROL_STC     *controlPtr
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  hwData;         /* HW data          */
    GT_U32  hwMask;         /* HW mask          */
    GT_U32  portGroupId;    /* portGroupId      */
    GT_U32  localPort;      /* local port       */
    GT_U32  portMacNum;     /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(controlPtr);

    if (controlPtr->rxTaiSelect > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwMask = 0;
    hwData = 0;

    hwData |= BOOL2BIT_MAC(controlPtr->unitEnable);
    hwMask |= 1;

    hwData |= (controlPtr->rxTaiSelect << 5);
    hwMask |= (1 << 5);

    hwData |= (BOOL2BIT_MAC(controlPtr->tsQueOverrideEnable) << 6);
    hwMask |= (1 << 6);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPGeneralCtrl;

    return prvCpssDrvHwPpPortGroupWriteRegBitMask(
        devNum, portGroupId, regAddr, hwMask, hwData);
}

/*******************************************************************************
* cpssDxChPtpTsuControlSet
*
* DESCRIPTION:
*       Set TSU (Time Stamp Unit) Control.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       controlPtr      - (pointer to) control structure.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuControlSet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    IN  CPSS_DXCH_PTP_TSU_CONTROL_STC     *controlPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsuControlSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, controlPtr));

    rc = internal_cpssDxChPtpTsuControlSet(devNum, portNum, controlPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, controlPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsuControlGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) Control.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*
* OUTPUTS:
*       controlPtr      - (pointer to) control structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsuControlGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    OUT CPSS_DXCH_PTP_TSU_CONTROL_STC     *controlPtr
)
{
    GT_STATUS   rc;             /* return code      */
    GT_U32      regAddr;        /* register address */
    GT_U32      hwData;         /* HW data          */
    GT_U32      portGroupId;    /* Port Group Id    */
    GT_U32      localPort;      /* local port       */
    GT_U32      portMacNum;     /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(controlPtr);

    cpssOsMemSet(controlPtr, 0, sizeof(*controlPtr));

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPGeneralCtrl;

    rc = prvCpssDrvHwPpPortGroupReadRegister(
        devNum, portGroupId, regAddr, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    controlPtr->unitEnable = BIT2BOOL_MAC((hwData & 1));

    controlPtr->rxTaiSelect = ((hwData >> 5) & 1);

    controlPtr->tsQueOverrideEnable = BIT2BOOL_MAC(((hwData >> 6) & 1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsuControlGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) Control.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*
* OUTPUTS:
*       controlPtr      - (pointer to) control structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuControlGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    OUT CPSS_DXCH_PTP_TSU_CONTROL_STC     *controlPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsuControlGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, controlPtr));

    rc = internal_cpssDxChPtpTsuControlGet(devNum, portNum, controlPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, controlPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsuTxTimestampQueueRead
*
* DESCRIPTION:
*       Read TSU (Time Stamp Unit) TX Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       queueNum        - queue number.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       entryPtr        - (pointer to) TX Timestamp Queue Entry structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Entry deleted from the Queue by reading it.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsuTxTimestampQueueRead
(
    IN  GT_U8                                              devNum,
    IN  GT_PHYSICAL_PORT_NUM                               portNum,
    IN  GT_U32                                             queueNum,
    OUT CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC     *entryPtr
)
{
    GT_STATUS   rc;             /* return code      */
    GT_U32      regAddr[3];     /* register address */
    GT_U32      hwData[3];      /* HW data          */
    GT_U32      i;              /* loop index       */
    GT_U32      portGroupId;    /* Port Group Id    */
    GT_U32      localPort;      /* local port       */
    GT_U32      portMacNum;     /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    cpssOsMemSet(entryPtr, 0, sizeof(*entryPtr));

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    switch (queueNum)
    {
        case 0:
            regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPTXTimestampQueue0Reg0;
            regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPTXTimestampQueue0Reg1;
            regAddr[2] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPTXTimestampQueue0Reg2;
            break;
        case 1:
            regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPTXTimestampQueue1Reg0;
            regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPTXTimestampQueue1Reg1;
            regAddr[2] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPTXTimestampQueue1Reg2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for (i = 0; (i < 3); i++)
    {
        rc = prvCpssDrvHwPpPortGroupReadRegister(
            devNum, portGroupId, regAddr[i], &(hwData[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    entryPtr->entryValid     = BIT2BOOL_MAC((hwData[0] & 1));
    entryPtr->entryId        = ((hwData[0] >> 1) & 0x3FF);
    entryPtr->taiSelect      = ((hwData[0] >> 11) & 1);
    entryPtr->todUpdateFlag  = ((hwData[0] >> 12) & 1);
    entryPtr->timestamp      =
        ((hwData[0] >> 13) & 7)
        | ((hwData[1] & 0xFFFF) << 3)
        | ((hwData[2] & 0x1FFF) << 19);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsuTxTimestampQueueRead
*
* DESCRIPTION:
*       Read TSU (Time Stamp Unit) TX Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       queueNum        - queue number.
*                         (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       entryPtr        - (pointer to) TX Timestamp Queue Entry structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Entry deleted from the Queue by reading it.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuTxTimestampQueueRead
(
    IN  GT_U8                                              devNum,
    IN  GT_PHYSICAL_PORT_NUM                               portNum,
    IN  GT_U32                                             queueNum,
    OUT CPSS_DXCH_PTP_TSU_TX_TIMESTAMP_QUEUE_ENTRY_STC     *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsuTxTimestampQueueRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, queueNum, entryPtr));

    rc = internal_cpssDxChPtpTsuTxTimestampQueueRead(devNum, portNum, queueNum, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, queueNum, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsuCountersClear
*
* DESCRIPTION:
*       Clear All TSU (Time Stamp Unit) Packet counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.

* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsuCountersClear
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  portGroupId;    /* Port Group Id    */
    GT_U32  localPort;      /* local port       */
    GT_U32  portMacNum;     /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPGeneralCtrl;

    return prvCpssDrvHwPpPortGroupSetRegField(
        devNum, portGroupId, regAddr, 4/*offset*/, 1/*length*/, 1);
}

/*******************************************************************************
* cpssDxChPtpTsuCountersClear
*
* DESCRIPTION:
*       Clear All TSU (Time Stamp Unit) Packet counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.

* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuCountersClear
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsuCountersClear);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum));

    rc = internal_cpssDxChPtpTsuCountersClear(devNum, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsuPacketCouterGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) packet counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       counterType     - counter type (see enum)
*
* OUTPUTS:
*       valuePtr        - (pointer to) counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsuPacketCouterGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    IN  CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT counterType,
    OUT GT_U32                                    *valuePtr
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  portGroupId;    /* Port Group Id    */
    GT_U32  localPort;      /* local port       */
    GT_U32  portMacNum;     /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    switch (counterType)
    {
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_TOTAL_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).totalPTPPktsCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V1_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPv1PktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_PTP_V2_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).PTPv2PktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_Y1731_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).Y1731PktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TS_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).NTPTsPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_RX_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).NTPReceivePktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NTP_TX_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).NTPTransmitPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_WAMP_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).WAMPPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_NONE_ACTION_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).noneActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_FORWARD_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).forwardActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_DROP_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).dropActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).captureActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_TIME_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).addTimeActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_CORR_TIME_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).addCorrectedTimeActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_TIME_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).captureAddTimeActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_CORR_TIME_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).captureAddCorrectedTimeActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ADD_INGRESS_TIME_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).addIngrTimeActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_ADD_INGRESS_TIME_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).captureAddIngrTimeActionPktCntr;
            break;
        case CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_CAPTURE_INGRESS_TIME_E:
            regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).captureIngrTimeActionPktCntr;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDrvHwPpPortGroupGetRegField(
        devNum, portGroupId, regAddr, 0/*offset*/, 8/*length*/, valuePtr);
}

/*******************************************************************************
* cpssDxChPtpTsuPacketCouterGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) packet counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       counterType     - counter type (see enum)
*
* OUTPUTS:
*       valuePtr        - (pointer to) counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuPacketCouterGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    IN  CPSS_DXCH_PTP_TSU_PACKET_COUNTER_TYPE_ENT counterType,
    OUT GT_U32                                    *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsuPacketCouterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, counterType, valuePtr));

    rc = internal_cpssDxChPtpTsuPacketCouterGet(devNum, portNum, counterType, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, counterType, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsuNtpTimeOffsetSet
*
* DESCRIPTION:
*       Set TSU (Time Stamp Unit) NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       ntpTimeOffset   - NTP PTP Time Offset measured in seconds.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsuNtpTimeOffsetSet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    IN  GT_U32                            ntpTimeOffset
)
{
    GT_STATUS   rc;             /* return code      */
    GT_U32      regAddr[2];     /* register address */
    GT_U32      hwData[2];      /* HW data          */
    GT_U32      i;              /* loop index       */
    GT_U32      portGroupId;    /* Port Group Id    */
    GT_U32      localPort;      /* local port       */
    GT_U32      portMacNum;     /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).NTPPTPOffsetHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).NTPPTPOffsetLow;

    hwData[0] = (ntpTimeOffset & 0xFFFF);
    hwData[1] = ((ntpTimeOffset >> 16) & 0xFFFF);

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr[i], 0, 16, hwData[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsuNtpTimeOffsetSet
*
* DESCRIPTION:
*       Set TSU (Time Stamp Unit) NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*       ntpTimeOffset   - NTP PTP Time Offset measured in seconds.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuNtpTimeOffsetSet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    IN  GT_U32                            ntpTimeOffset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsuNtpTimeOffsetSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ntpTimeOffset));

    rc = internal_cpssDxChPtpTsuNtpTimeOffsetSet(devNum, portNum, ntpTimeOffset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ntpTimeOffset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsuNtpTimeOffsetGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number.
*
* OUTPUTS:
*       ntpTimeOffsetPtr - (pointer to) NTP PTP Time Offset measured in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsuNtpTimeOffsetGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    OUT GT_U32                            *ntpTimeOffsetPtr
)
{
    GT_STATUS   rc;             /* return code      */
    GT_U32      regAddr[2];     /* register address */
    GT_U32      hwData[2];      /* HW data          */
    GT_U32      i;              /* loop index       */
    GT_U32      portGroupId;    /* Port Group Id    */
    GT_U32      localPort;      /* local port       */
    GT_U32      portMacNum;     /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PTP_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(ntpTimeOffsetPtr);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    regAddr[0] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).NTPPTPOffsetHigh;
    regAddr[1] = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, localPort).NTPPTPOffsetLow;

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, portGroupId, regAddr[i], 0, 16, &(hwData[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *ntpTimeOffsetPtr = ((hwData[1] << 16) | hwData[0]);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsuNtpTimeOffsetGet
*
* DESCRIPTION:
*       Get TSU (Time Stamp Unit) NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number.
*
* OUTPUTS:
*       ntpTimeOffsetPtr - (pointer to) NTP PTP Time Offset measured in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsuNtpTimeOffsetGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    OUT GT_U32                            *ntpTimeOffsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsuNtpTimeOffsetGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ntpTimeOffsetPtr));

    rc = internal_cpssDxChPtpTsuNtpTimeOffsetGet(devNum, portNum, ntpTimeOffsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ntpTimeOffsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsTagGlobalCfgSet
*
* DESCRIPTION:
*       Set Timestamp Global Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       tsTagGlobalCfgPtr   - (pointer to) TS Tag Global Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsTagGlobalCfgSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *tsTagGlobalCfgPtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwData;           /* HW data                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(tsTagGlobalCfgPtr);

    if (tsTagGlobalCfgPtr->tsTagEtherType > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (tsTagGlobalCfgPtr->hybridTsTagEtherType > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* TTI data */
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.timestampEtherTypes;

    hwData = tsTagGlobalCfgPtr->tsTagEtherType
        | (tsTagGlobalCfgPtr->hybridTsTagEtherType << 16);

    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.timestampConfigs;

    hwData = BOOL2BIT_MAC(tsTagGlobalCfgPtr->tsTagParseEnable)
        | (BOOL2BIT_MAC(tsTagGlobalCfgPtr->hybridTsTagParseEnable) << 1);

    rc = prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 0, 2, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* HA data */
    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).PTPTimestampTagEthertype;

    hwData = tsTagGlobalCfgPtr->hybridTsTagEtherType
        | (tsTagGlobalCfgPtr->tsTagEtherType << 16);

    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChPtpTsTagGlobalCfgSet
*
* DESCRIPTION:
*       Set Timestamp Global Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       tsTagGlobalCfgPtr   - (pointer to) TS Tag Global Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsTagGlobalCfgSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *tsTagGlobalCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsTagGlobalCfgSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tsTagGlobalCfgPtr));

    rc = internal_cpssDxChPtpTsTagGlobalCfgSet(devNum, tsTagGlobalCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tsTagGlobalCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsTagGlobalCfgGet
*
* DESCRIPTION:
*       Get Timestamp Global Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       tsTagGlobalCfgPtr   - (pointer to) TS Tag Global Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsTagGlobalCfgGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *tsTagGlobalCfgPtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwData;           /* HW data                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(tsTagGlobalCfgPtr);

    cpssOsMemSet(tsTagGlobalCfgPtr, 0, sizeof(*tsTagGlobalCfgPtr));

    /* TTI data - the HA copy of the same Ethertypes ignored */
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.timestampEtherTypes;

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    tsTagGlobalCfgPtr->tsTagEtherType       = (hwData & 0xFFFF);
    tsTagGlobalCfgPtr->hybridTsTagEtherType = ((hwData >> 16) & 0xFFFF);


    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.timestampConfigs;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 0, 2, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    tsTagGlobalCfgPtr->tsTagParseEnable       = BIT2BOOL_MAC((hwData & 1));
    tsTagGlobalCfgPtr->hybridTsTagParseEnable = BIT2BOOL_MAC(((hwData >> 1) & 1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsTagGlobalCfgGet
*
* DESCRIPTION:
*       Get Timestamp Global Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       tsTagGlobalCfgPtr   - (pointer to) TS Tag Global Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsTagGlobalCfgGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_PTP_TS_TAG_GLOBAL_CFG_STC *tsTagGlobalCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsTagGlobalCfgGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tsTagGlobalCfgPtr));

    rc = internal_cpssDxChPtpTsTagGlobalCfgGet(devNum, tsTagGlobalCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tsTagGlobalCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsTagPortCfgSet
*
* DESCRIPTION:
*       Set Timestamp Port Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number.
*       tsTagPortCfgPtr   - (pointer to) TS Tag Port Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsTagPortCfgSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC   *tsTagPortCfgPtr
)
{
    GT_STATUS rc;               /* return code                      */
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    regIndex;         /* index of register                */
    GT_U32    hwValue;          /* HW value                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(tsTagPortCfgPtr);

    hwValue = BOOL2BIT_MAC(tsTagPortCfgPtr->tsPiggyBackEnable);
    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(
            devNum,
            PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
            portNum,
            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
            240,
            1 /*fieldLength*/,
            hwValue);
    }
    else
    {
        regIndex    = (portNum / 32);
        fieldOffset = (portNum % 32);
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.piggyBackTSConfigs[regIndex];
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 1, hwValue);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    switch (tsTagPortCfgPtr->tsTagMode)
    {
        case CPSS_DXCH_PTP_TS_TAG_MODE_NONE_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_PTP_TS_TAG_MODE_ALL_NON_EXTENDED_E:
            hwValue = 1;
            break;
        case CPSS_DXCH_PTP_TS_TAG_MODE_ALL_EXTENDED_E:
            hwValue = 2;
            break;
        case CPSS_DXCH_PTP_TS_TAG_MODE_NON_EXTENDED_E:
            hwValue = 3;
            break;
        case CPSS_DXCH_PTP_TS_TAG_MODE_EXTENDED_E:
            hwValue = 4;
            break;
        case CPSS_DXCH_PTP_TS_TAG_MODE_PIGGYBACK_E:
            hwValue = 5;
            break;
        case CPSS_DXCH_PTP_TS_TAG_MODE_HYBRID_E:
            hwValue = 6;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        35, 3 /*fieldLength*/, hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwValue = BOOL2BIT_MAC(tsTagPortCfgPtr->tsReceptionEnable);

    rc = prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        39, 1 /*fieldLength*/, hwValue);
    return rc;
}

/*******************************************************************************
* cpssDxChPtpTsTagPortCfgSet
*
* DESCRIPTION:
*       Set Timestamp Port Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number.
*       tsTagPortCfgPtr   - (pointer to) TS Tag Port Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsTagPortCfgSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC   *tsTagPortCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsTagPortCfgSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tsTagPortCfgPtr));

    rc = internal_cpssDxChPtpTsTagPortCfgSet(devNum, portNum, tsTagPortCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tsTagPortCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsTagPortCfgGet
*
* DESCRIPTION:
*       Get Timestamp Port Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number.
*
* OUTPUTS:
*       tsTagPortCfgPtr   - (pointer to) TS Tag Port Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsTagPortCfgGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC   *tsTagPortCfgPtr
)
{
    GT_STATUS rc;               /* return code                      */
    GT_U32    regAddr;          /* register address                 */
    GT_U32    fieldOffset;      /* field offset inside register     */
    GT_U32    regIndex;         /* index of register                */
    GT_U32    hwValue;          /* HW value                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(tsTagPortCfgPtr);

    cpssOsMemSet(tsTagPortCfgPtr, 0, sizeof(*tsTagPortCfgPtr));

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChReadTableEntryField(
            devNum,
            PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
            portNum,
            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
            240,
            1 /*fieldLength*/,
            &hwValue);
    }
    else
    {
        regIndex    = (portNum / 32);
        fieldOffset = (portNum % 32);
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.piggyBackTSConfigs[regIndex];

        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &hwValue);
    }
    if (rc != GT_OK)
    {
        return rc;
    }
    tsTagPortCfgPtr->tsPiggyBackEnable = BIT2BOOL_MAC(hwValue);

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        35, 3 /*fieldLength*/, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hwValue)
    {
        default:
        case 0:
            tsTagPortCfgPtr->tsTagMode = CPSS_DXCH_PTP_TS_TAG_MODE_NONE_E;
            break;
        case 1:
            tsTagPortCfgPtr->tsTagMode = CPSS_DXCH_PTP_TS_TAG_MODE_ALL_NON_EXTENDED_E;
            break;
        case 2:
            tsTagPortCfgPtr->tsTagMode = CPSS_DXCH_PTP_TS_TAG_MODE_ALL_EXTENDED_E;
            break;
        case 3:
            tsTagPortCfgPtr->tsTagMode = CPSS_DXCH_PTP_TS_TAG_MODE_NON_EXTENDED_E;
            break;
        case 4:
            tsTagPortCfgPtr->tsTagMode = CPSS_DXCH_PTP_TS_TAG_MODE_EXTENDED_E;
            break;
        case 5:
            tsTagPortCfgPtr->tsTagMode = CPSS_DXCH_PTP_TS_TAG_MODE_PIGGYBACK_E;
            break;
        case 6:
            tsTagPortCfgPtr->tsTagMode = CPSS_DXCH_PTP_TS_TAG_MODE_HYBRID_E;
            break;
    }

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        39, 1 /*fieldLength*/, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    tsTagPortCfgPtr->tsReceptionEnable = BIT2BOOL_MAC(hwValue);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsTagPortCfgGet
*
* DESCRIPTION:
*       Get Timestamp Port Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number.
*
* OUTPUTS:
*       tsTagPortCfgPtr   - (pointer to) TS Tag Port Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsTagPortCfgGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT CPSS_DXCH_PTP_TS_TAG_PORT_CFG_STC   *tsTagPortCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsTagPortCfgGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tsTagPortCfgPtr));

    rc = internal_cpssDxChPtpTsTagPortCfgGet(devNum, portNum, tsTagPortCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tsTagPortCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayIngressPortDelaySet
*
* DESCRIPTION:
*       Set Ingress Port Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       ingressDelayCorr    - Ingress Port Delay Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayIngressPortDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_32                   ingressDelayCorr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    if ((ingressDelayCorr < -1000000000) || (ingressDelayCorr > 1000000000))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        0/*fieldOffset*/, 32 /*fieldLength*/, (GT_U32)ingressDelayCorr);
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressPortDelaySet
*
* DESCRIPTION:
*       Set Ingress Port Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       ingressDelayCorr    - Ingress Port Delay Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressPortDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_32                   ingressDelayCorr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayIngressPortDelaySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ingressDelayCorr));

    rc = internal_cpssDxChPtpTsDelayIngressPortDelaySet(devNum, portNum, ingressDelayCorr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ingressDelayCorr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayIngressPortDelayGet
*
* DESCRIPTION:
*       Get Ingress Port Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*
* OUTPUTS:
*       ingressDelayCorrPtr  - (pointer to) Ingress Port Delay Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayIngressPortDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_32                   *ingressDelayCorrPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(ingressDelayCorrPtr);

    return prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        0/*fieldOffset*/, 32 /*fieldLength*/, (GT_U32*)ingressDelayCorrPtr);
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressPortDelayGet
*
* DESCRIPTION:
*       Get Ingress Port Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*
* OUTPUTS:
*       ingressDelayCorrPtr  - (pointer to) Ingress Port Delay Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressPortDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_32                   *ingressDelayCorrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayIngressPortDelayGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ingressDelayCorrPtr));

    rc = internal_cpssDxChPtpTsDelayIngressPortDelayGet(devNum, portNum, ingressDelayCorrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ingressDelayCorrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayIngressLinkDelaySet
*
* DESCRIPTION:
*       Set Ingress Link Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile
*                             (APPLICABLE RANGES: 0..8.)
*       ingressDelay        - Ingress Port Delay.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayIngressLinkDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    IN  GT_32                   ingressDelay
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    if (domainProfile > 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((ingressDelay < -1000000000) || (ingressDelay > 1000000000))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        (32 + (32 * domainProfile))/*fieldOffset*/, 32 /*fieldLength*/, (GT_U32)ingressDelay);
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressLinkDelaySet
*
* DESCRIPTION:
*       Set Ingress Link Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile
*                             (APPLICABLE RANGES: 0..8.)
*       ingressDelay        - Ingress Port Delay.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressLinkDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    IN  GT_32                   ingressDelay
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayIngressLinkDelaySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainProfile, ingressDelay));

    rc = internal_cpssDxChPtpTsDelayIngressLinkDelaySet(devNum, portNum, domainProfile, ingressDelay);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainProfile, ingressDelay));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayIngressLinkDelayGet
*
* DESCRIPTION:
*       Get Ingress Link Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*       domainProfile        - domain profile
*                              (APPLICABLE RANGES: 0..8.)
*
* OUTPUTS:
*       ingressDelayPtr      - (pointer to) Ingress Port Delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayIngressLinkDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    OUT GT_32                   *ingressDelayPtr
)
{
    GT_STATUS rc;       /* return code */
    GT_U32    hwVal;    /* HW value    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(ingressDelayPtr);

    if (domainProfile > 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        (32 + (32 * domainProfile))/*fieldOffset*/, 32 /*fieldLength*/, &hwVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    *ingressDelayPtr = (GT_32)hwVal;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressLinkDelayGet
*
* DESCRIPTION:
*       Get Ingress Link Delay.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*       domainProfile        - domain profile
*                              (APPLICABLE RANGES: 0..8.)
*
* OUTPUTS:
*       ingressDelayPtr      - (pointer to) Ingress Port Delay.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressLinkDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    OUT GT_32                   *ingressDelayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayIngressLinkDelayGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainProfile, ingressDelayPtr));

    rc = internal_cpssDxChPtpTsDelayIngressLinkDelayGet(devNum, portNum, domainProfile, ingressDelayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainProfile, ingressDelayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressPipeDelaySet
*
* DESCRIPTION:
*       Set Egress Pipe Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrPipeDelayCorr    - Egress Pipe Delay Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressPipeDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_32                   egrPipeDelayCorr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    if ((egrPipeDelayCorr < -1000000000) || (egrPipeDelayCorr > 1000000000))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        2/*fieldOffset*/, 30 /*fieldLength*/, (GT_U32)egrPipeDelayCorr);
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressPipeDelaySet
*
* DESCRIPTION:
*       Set Egress Pipe Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrPipeDelayCorr    - Egress Pipe Delay Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressPipeDelaySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_32                   egrPipeDelayCorr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressPipeDelaySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egrPipeDelayCorr));

    rc = internal_cpssDxChPtpTsDelayEgressPipeDelaySet(devNum, portNum, egrPipeDelayCorr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egrPipeDelayCorr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressPipeDelayGet
*
* DESCRIPTION:
*       Get Egress Pipe Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*
* OUTPUTS:
*       egrPipeDelayCorrPtr  - (pointer to) Egress Pipe Delay Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressPipeDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_32                   *egrPipeDelayCorrPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(egrPipeDelayCorrPtr);

    return prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        2/*fieldOffset*/, 30 /*fieldLength*/, (GT_U32*)egrPipeDelayCorrPtr);
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressPipeDelayGet
*
* DESCRIPTION:
*       Get Egress Pipe Delay Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*
* OUTPUTS:
*       egrPipeDelayCorrPtr  - (pointer to) Egress Pipe Delay Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressPipeDelayGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_32                   *egrPipeDelayCorrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressPipeDelayGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egrPipeDelayCorrPtr));

    rc = internal_cpssDxChPtpTsDelayEgressPipeDelayGet(devNum, portNum, egrPipeDelayCorrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egrPipeDelayCorrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet
*
* DESCRIPTION:
*       Set Egress Asymmetry Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       egrAsymmetryCorr    - Egress Asymmetry Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    IN  GT_32                   egrAsymmetryCorr
)
{
    GT_U32    fieldOffset;      /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    if (domainProfile > 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((egrAsymmetryCorr < -1000000000) || (egrAsymmetryCorr > 1000000000))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    fieldOffset = 32 + (32 * domainProfile);

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        fieldOffset, 32 /*fieldLength*/, (GT_U32)egrAsymmetryCorr);
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet
*
* DESCRIPTION:
*       Set Egress Asymmetry Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       egrAsymmetryCorr    - Egress Asymmetry Correction.
*                             (APPLICABLE RANGES: -10^9..10^9-1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    IN  GT_32                   egrAsymmetryCorr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainProfile, egrAsymmetryCorr));

    rc = internal_cpssDxChPtpTsDelayEgressAsymmetryCorrectionSet(devNum, portNum, domainProfile, egrAsymmetryCorr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainProfile, egrAsymmetryCorr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet
*
* DESCRIPTION:
*       Get Egress Asymmetry Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*       domainProfile        - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*
* OUTPUTS:
*       egrAsymmetryCorrPtr  - (pointer to) Egress Asymmetry Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    OUT GT_32                   *egrAsymmetryCorrPtr
)
{
    GT_U32    fieldOffset;      /* field offset inside register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(egrAsymmetryCorrPtr);

    if (domainProfile > 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fieldOffset = 32 + (32 * domainProfile);

    return prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        fieldOffset, 32 /*fieldLength*/, (GT_U32*)egrAsymmetryCorrPtr);
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet
*
* DESCRIPTION:
*       Get Egress Asymmetry Correction.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       portNum              - port number.
*       domainProfile        - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*
* OUTPUTS:
*       egrAsymmetryCorrPtr  - (pointer to) Egress Asymmetry Correction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  domainProfile,
    OUT GT_32                   *egrAsymmetryCorrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainProfile, egrAsymmetryCorrPtr));

    rc = internal_cpssDxChPtpTsDelayEgressAsymmetryCorrectionGet(devNum, portNum, domainProfile, egrAsymmetryCorrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainProfile, egrAsymmetryCorrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet
*
* DESCRIPTION:
*       Set Ingress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       ingrCorrFldPBEnable - Ingress Correction Field Piggyback Enable.
*                             GT_TRUE  - Correction field of PTP header contains
*                                        ingress timestamp.
*                             GT_FALSE - Correction field of PTP header reserved.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 ingrCorrFldPBEnable
)
{
    GT_U32              hwData; /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    hwData = BOOL2BIT_MAC(ingrCorrFldPBEnable);

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        320 /*fieldOffset*/, 1 /*fieldLength*/, hwData);
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet
*
* DESCRIPTION:
*       Set Ingress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       ingrCorrFldPBEnable - Ingress Correction Field Piggyback Enable.
*                             GT_TRUE  - Correction field of PTP header contains
*                                        ingress timestamp.
*                             GT_FALSE - Correction field of PTP header reserved.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 ingrCorrFldPBEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ingrCorrFldPBEnable));

    rc = internal_cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableSet(devNum, portNum, ingrCorrFldPBEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ingrCorrFldPBEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet
*
* DESCRIPTION:
*       Get Ingress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*
* OUTPUTS:
*       ingrCorrFldPBEnablePtr - (pointer to) Ingress Correction Field Piggyback Enable.
*                                GT_TRUE  - Correction field of PTP header contains
*                                           ingress timestamp.
*                                GT_FALSE - Correction field of PTP header reserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *ingrCorrFldPBEnablePtr
)
{
    GT_U32              hwData; /* HW data     */
    GT_STATUS           rc;     /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(ingrCorrFldPBEnablePtr);

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        320 /*fieldOffset*/, 1 /*fieldLength*/, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *ingrCorrFldPBEnablePtr = BIT2BOOL_MAC(hwData);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet
*
* DESCRIPTION:
*       Get Ingress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*
* OUTPUTS:
*       ingrCorrFldPBEnablePtr - (pointer to) Ingress Correction Field Piggyback Enable.
*                                GT_TRUE  - Correction field of PTP header contains
*                                           ingress timestamp.
*                                GT_FALSE - Correction field of PTP header reserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *ingrCorrFldPBEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ingrCorrFldPBEnablePtr));

    rc = internal_cpssDxChPtpTsDelayIngressCorrFieldPiggybackEnableGet(devNum, portNum, ingrCorrFldPBEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ingrCorrFldPBEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet
*
* DESCRIPTION:
*       Set Egress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrCorrFldPBEnable  - Egress Correction Field Piggyback Enable.
*                             GT_TRUE  - Correction field of PTP header contains
*                                        ingress timestamp.
*                             GT_FALSE - Correction field of PTP header reserved.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 egrCorrFldPBEnable
)
{
    GT_U32              hwData; /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    hwData = BOOL2BIT_MAC(egrCorrFldPBEnable);

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        320 /*fieldOffset*/, 1 /*fieldLength*/, hwData);
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet
*
* DESCRIPTION:
*       Set Egress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrCorrFldPBEnable  - Egress Correction Field Piggyback Enable.
*                             GT_TRUE  - Correction field of PTP header contains
*                                        ingress timestamp.
*                             GT_FALSE - Correction field of PTP header reserved.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 egrCorrFldPBEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egrCorrFldPBEnable));

    rc = internal_cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableSet(devNum, portNum, egrCorrFldPBEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egrCorrFldPBEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet
*
* DESCRIPTION:
*       Get Egress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*
* OUTPUTS:
*       egrCorrFldPBEnablePtr  - (pointer to) Egress Correction Field Piggyback Enable.
*                                GT_TRUE  - Correction field of PTP header contains
*                                           ingress timestamp.
*                                GT_FALSE - Correction field of PTP header reserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *egrCorrFldPBEnablePtr
)
{
    GT_U32              hwData; /* HW data     */
    GT_STATUS           rc;     /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(egrCorrFldPBEnablePtr);

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        320 /*fieldOffset*/, 1 /*fieldLength*/, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *egrCorrFldPBEnablePtr = BIT2BOOL_MAC(hwData);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet
*
* DESCRIPTION:
*       Get Egress Correction Field Piggyback Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*
* OUTPUTS:
*       egrCorrFldPBEnablePtr  - (pointer to) Egress Correction Field Piggyback Enable.
*                                GT_TRUE  - Correction field of PTP header contains
*                                           ingress timestamp.
*                                GT_FALSE - Correction field of PTP header reserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *egrCorrFldPBEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egrCorrFldPBEnablePtr));

    rc = internal_cpssDxChPtpTsDelayEgressCorrFieldPiggybackEnableGet(devNum, portNum, egrCorrFldPBEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egrCorrFldPBEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet
*
* DESCRIPTION:
*       Set Egress Time Correction TAI Select Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*       egrTimeCorrTaiSelMode  - Egress Time Correction TAI Select Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    IN  CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT   egrTimeCorrTaiSelMode
)
{
    GT_U32              hwData; /* HW data     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    switch (egrTimeCorrTaiSelMode)
    {
        case CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_INGRESS_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_EGRESS_E:
            hwData = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        0 /*fieldOffset*/, 1 /*fieldLength*/, hwData);
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet
*
* DESCRIPTION:
*       Set Egress Time Correction TAI Select Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       portNum                - port number.
*       egrTimeCorrTaiSelMode  - Egress Time Correction TAI Select Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    IN  CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT   egrTimeCorrTaiSelMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egrTimeCorrTaiSelMode));

    rc = internal_cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeSet(devNum, portNum, egrTimeCorrTaiSelMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egrTimeCorrTaiSelMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet
*
* DESCRIPTION:
*       Get Egress Time Correction TAI Select Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                    - device number
*       portNum                   - port number.
*
* OUTPUTS:
*       egrTimeCorrTaiSelModePtr  - (pointer to)Egress Time Correction TAI Select Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    OUT CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT   *egrTimeCorrTaiSelModePtr
)
{
    GT_U32              hwData; /* HW data     */
    GT_STATUS           rc;     /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(egrTimeCorrTaiSelModePtr);

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        0 /*fieldOffset*/, 1 /*fieldLength*/, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *egrTimeCorrTaiSelModePtr =
        (hwData == 0)
            ? CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_INGRESS_E
            : CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_EGRESS_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet
*
* DESCRIPTION:
*       Get Egress Time Correction TAI Select Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                    - device number
*       portNum                   - port number.
*
* OUTPUTS:
*       egrTimeCorrTaiSelModePtr  - (pointer to)Egress Time Correction TAI Select Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PHYSICAL_PORT_NUM                      portNum,
    OUT CPSS_DXCH_PTP_TS_DELAY_TAI_SEL_MODE_ENT   *egrTimeCorrTaiSelModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egrTimeCorrTaiSelModePtr));

    rc = internal_cpssDxChPtpTsDelayEgressTimeCorrTaiSelectModeGet(devNum, portNum, egrTimeCorrTaiSelModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egrTimeCorrTaiSelModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressTimestampTaiSelectSet
*
* DESCRIPTION:
*       Set Egress Timestamp TAI Select.
*
* APPLICABLE DEVICES:
*        Bobcat2; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrTsTaiNum         - Egress Timestamp TAI Number.
*                             (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressTimestampTaiSelectSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  egrTsTaiNum
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_CAELUM_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    if (egrTsTaiNum > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        1 /*fieldOffset*/, 1 /*fieldLength*/, egrTsTaiNum);
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimestampTaiSelectSet
*
* DESCRIPTION:
*       Set Egress Timestamp TAI Select.
*
* APPLICABLE DEVICES:
*        Bobcat2; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       egrTsTaiNum         - Egress Timestamp TAI Number.
*                             (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressTimestampTaiSelectSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  egrTsTaiNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressTimestampTaiSelectSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egrTsTaiNum));

    rc = internal_cpssDxChPtpTsDelayEgressTimestampTaiSelectSet(devNum, portNum, egrTsTaiNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egrTsTaiNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDelayEgressTimestampTaiSelectGet
*
* DESCRIPTION:
*       Get Egress Timestamp TAI Select.
*
* APPLICABLE DEVICES:
*        Bobcat2; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*
* OUTPUTS:
*       egrTsTaiNumPtr  - (pointer to)Egress Timestamp TAI Number.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDelayEgressTimestampTaiSelectGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *egrTsTaiNumPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_CAELUM_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(egrTsTaiNumPtr);

    return prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        portNum, PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        1 /*fieldOffset*/, 1 /*fieldLength*/, egrTsTaiNumPtr);
}

/*******************************************************************************
* cpssDxChPtpTsDelayEgressTimestampTaiSelectGet
*
* DESCRIPTION:
*       Get Egress Timestamp TAI Select.
*
* APPLICABLE DEVICES:
*        Bobcat2; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number.
*
* OUTPUTS:
*       egrTsTaiNumPtr  - (pointer to)Egress Timestamp TAI Number.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDelayEgressTimestampTaiSelectGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *egrTsTaiNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDelayEgressTimestampTaiSelectGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, egrTsTaiNumPtr));

    rc = internal_cpssDxChPtpTsDelayEgressTimestampTaiSelectGet(devNum, portNum, egrTsTaiNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, egrTsTaiNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsCfgTableSet
*
* DESCRIPTION:
*       Set Timestamp Configuration Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - entry index.
*                             (APPLICABLE RANGES: 0..127.)
*       entryPtr            - (pointer to)Timestamp Configuration Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsCfgTableSet
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         entryIndex,
    IN  CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *entryPtr
)
{
    GT_U32     hwData;   /* HW data                 */
    GT_U32     hwEntry;  /* HW formated table entry */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    if (entryIndex > 127)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwEntry = 0;

    switch (entryPtr->tsMode)
    {
        case CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E:
            hwData = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwEntry |= hwData;

    if (entryPtr->tsMode == CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E)
    {
        if (entryPtr->offsetProfile >= BIT_7)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        hwEntry |= (entryPtr->offsetProfile << 1);

        hwEntry |= (BOOL2BIT_MAC(entryPtr->OE) << 8);
    }

    if (entryPtr->tsMode == CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E)
    {
        switch (entryPtr->tsAction)
        {
            case CPSS_DXCH_PTP_TS_ACTION_NONE_E:
                hwData = 0;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_DROP_E:
                hwData = 2;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E:
                hwData = 3;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E:
                hwData = 4;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E:
                hwData = 5;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E:
                hwData = 6;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E:
                hwData = 7;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E:
                hwData = 8;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_INGRESS_TIME_E:
                hwData = 9;
                break;
            case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_INGRESS_TIME_E:
                hwData = 10;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        hwEntry |= (hwData << 1);

        switch (entryPtr->packetFormat)
        {
            case CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E:
                hwData = 0;
                break;
            case CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E:
                hwData = 1;
                break;
            case CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E:
                hwData = 2;
                break;
            case CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TS_E:
                hwData = 3;
                break;
            case CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_RX_E:
                hwData = 4;
                break;
            case CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TX_E:
                hwData = 5;
                break;
            case CPSS_DXCH_PTP_TS_PACKET_TYPE_WAMP_E:
                hwData = 6;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        hwEntry |= (hwData << 5);

        switch (entryPtr->ptpTransport)
        {
            case CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E:
                hwData = 0;
                break;
            case CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV4_E:
                hwData = 1;
                break;
            case CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV6_E:
                hwData = 2;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        hwEntry |= (hwData << 8);
    }

    if (entryPtr->ptpMessageType >= BIT_4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (entryPtr->domain > 4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        if (entryPtr->offset >= BIT_8)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        hwEntry |= (entryPtr->offset << 10);

        hwEntry |= (entryPtr->ptpMessageType << 18);

        hwEntry |= (entryPtr->domain << 22);

        hwEntry |= (BOOL2BIT_MAC(entryPtr->ingrLinkDelayEnable) << 25);

        hwEntry |= (BOOL2BIT_MAC(entryPtr->packetDispatchingEnable) << 26);
    }
    else
    {
        if (entryPtr->offset >= BIT_7)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        hwEntry |= (entryPtr->offset << 10);

        hwEntry |= (entryPtr->ptpMessageType << 17);

        hwEntry |= (entryPtr->domain << 21);

        hwEntry |= (BOOL2BIT_MAC(entryPtr->ingrLinkDelayEnable) << 24);

        hwEntry |= (BOOL2BIT_MAC(entryPtr->packetDispatchingEnable) << 25);
    }

    return prvCpssDxChWriteTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_TIMESTAMP_CFG_E,
        entryIndex, &hwEntry);
}

/*******************************************************************************
* cpssDxChPtpTsCfgTableSet
*
* DESCRIPTION:
*       Set Timestamp Configuration Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - entry index.
*                             (APPLICABLE RANGES: 0..127.)
*       entryPtr            - (pointer to)Timestamp Configuration Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsCfgTableSet
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         entryIndex,
    IN  CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsCfgTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, entryPtr));

    rc = internal_cpssDxChPtpTsCfgTableSet(devNum, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsCfgTableGet
*
* DESCRIPTION:
*       Get Timestamp Configuration Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - entry index.
*                             (APPLICABLE RANGES: 0..127.)
*
* OUTPUTS:
*       entryPtr            - (pointer to)Timestamp Configuration Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsCfgTableGet
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         entryIndex,
    OUT CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *entryPtr
)
{
    GT_U32     hwEntry;  /* HW formated table entry */
    GT_U32     hwData;   /* HW data                 */
    GT_STATUS  rc;       /* return code             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    cpssOsMemSet(entryPtr, 0, sizeof(*entryPtr));

    if (entryIndex > 127)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChReadTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_TIMESTAMP_CFG_E,
        entryIndex, &hwEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwData = hwEntry & 1;

    switch (hwData)
    {
        case 0:
            entryPtr->tsMode = CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E;
            break;
        case 1:
            entryPtr->tsMode = CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* never occurs */
    }

    if (entryPtr->tsMode == CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_MODIFY_TAG_E)
    {
        entryPtr->offsetProfile = ((hwEntry >> 1) & 0x7F);
        hwData                  = ((hwEntry >> 8) & 1);
        entryPtr->OE            = BIT2BOOL_MAC(hwData);
    }

    if (entryPtr->tsMode == CPSS_DXCH_PTP_TS_TIMESTAMPING_MODE_DO_ACTION_E)
    {
        hwData = ((hwEntry >> 1) & 0xF);

        switch (hwData)
        {
            default:
            case 0:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_NONE_E;
                break;
            case 2:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_DROP_E;
                break;
            case 3:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E;
                break;
            case 4:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E;
                break;
            case 5:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E;
                break;
            case 6:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E;
                break;
            case 7:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E;
                break;
            case 8:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_INGRESS_TIME_E;
                break;
            case 9:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_INGRESS_TIME_E;
                break;
            case 10:
                entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_INGRESS_TIME_E;
                break;
        }

        hwData = ((hwEntry >> 5) & 0x7);

        switch (hwData)
        {
            case 0:
                entryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E;
                break;
            case 1:
                entryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E;
                break;
            case 2:
                entryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
                break;
            case 3:
                entryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TS_E;
                break;
            case 4:
                entryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_RX_E;
                break;
            case 5:
                entryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TX_E;
                break;
            case 6:
                entryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_WAMP_E;
                break;
            case 7:
                entryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_RESERVED_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG); /* on reseved value 7 */
        }

        hwData = ((hwEntry >> 8) & 0x3);

        switch (hwData)
        {
            case 0:
                entryPtr->ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_ETHERNET_E;
                break;
            case 1:
                entryPtr->ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV4_E;
                break;
            case 2:
                entryPtr->ptpTransport = CPSS_DXCH_PTP_TRANSPORT_TYPE_UDP_IPV6_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG); /* on reserved value 3 */
        }
    }
    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        entryPtr->offset                  = ((hwEntry >> 10) & 0xFF);

        entryPtr->ptpMessageType          = ((hwEntry >> 18) & 0xF);

        entryPtr->domain                  = ((hwEntry >> 22) & 0x7);

        hwData                            = ((hwEntry >> 25) & 1);

        entryPtr->ingrLinkDelayEnable     = BIT2BOOL_MAC(hwData);

        hwData                            = ((hwEntry >> 26) & 1);

        entryPtr->packetDispatchingEnable = BIT2BOOL_MAC(hwData);
    }
    else
    {
        entryPtr->offset                  = ((hwEntry >> 10) & 0x7F);

        entryPtr->ptpMessageType          = ((hwEntry >> 17) & 0xF);

        entryPtr->domain                  = ((hwEntry >> 21) & 0x7);

        hwData                            = ((hwEntry >> 24) & 1);

        entryPtr->ingrLinkDelayEnable     = BIT2BOOL_MAC(hwData);

        hwData                            = ((hwEntry >> 25) & 1);

        entryPtr->packetDispatchingEnable = BIT2BOOL_MAC(hwData);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsCfgTableGet
*
* DESCRIPTION:
*       Get Timestamp Configuration Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       entryIndex          - entry index.
*                             (APPLICABLE RANGES: 0..127.)
*
* OUTPUTS:
*       entryPtr            - (pointer to)Timestamp Configuration Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsCfgTableGet
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         entryIndex,
    OUT CPSS_DXCH_PTP_TS_CFG_ENTRY_STC *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsCfgTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, entryPtr));

    rc = internal_cpssDxChPtpTsCfgTableGet(devNum, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsLocalActionTableSet
*
* DESCRIPTION:
*       Set Timestamp Local Action Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       messageType         - message type
*                             (APPLICABLE RANGES: 0..15.)
*       entryPtr            - (pointer to)Timestamp Local Action Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsLocalActionTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainProfile,
    IN  GT_U32                                  messageType,
    IN  CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *entryPtr
)
{
    GT_U32     hwEntry;     /* HW formated table entry */
    GT_U32     hwData;      /* HW data                 */
    GT_U32     entryIndex;  /* table entry index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    if (domainProfile > 4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (messageType > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwEntry = 0;

    entryIndex = (portNum * 128) + (messageType * 8) + domainProfile;

    switch (entryPtr->tsAction)
    {
        case CPSS_DXCH_PTP_TS_ACTION_NONE_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PTP_TS_ACTION_DROP_E:
            hwData = 2;
            break;
        case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E:
            hwData = 3;
            break;
        case CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E:
            hwData = 4;
            break;
        case CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E:
            hwData = 5;
            break;
        case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E:
            hwData = 6;
            break;
        case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E:
            hwData = 7;
            break;
        case CPSS_DXCH_PTP_TS_ACTION_CAPTURE_INGRESS_TIME_E:
            hwData = 10;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwEntry |= (hwData << 1);

    hwData   = BOOL2BIT_MAC(entryPtr->ingrLinkDelayEnable);

    hwEntry |= hwData;

    hwData   = BOOL2BIT_MAC(entryPtr->packetDispatchingEnable);

    hwEntry |= (hwData << 5);

    return prvCpssDxChWriteTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_LOCAL_ACTION_E,
        entryIndex, &hwEntry);
}

/*******************************************************************************
* cpssDxChPtpTsLocalActionTableSet
*
* DESCRIPTION:
*       Set Timestamp Local Action Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       messageType         - message type
*                             (APPLICABLE RANGES: 0..15.)
*       entryPtr            - (pointer to)Timestamp Local Action Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsLocalActionTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainProfile,
    IN  GT_U32                                  messageType,
    IN  CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsLocalActionTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainProfile, messageType, entryPtr));

    rc = internal_cpssDxChPtpTsLocalActionTableSet(devNum, portNum, domainProfile, messageType, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainProfile, messageType, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsLocalActionTableGet
*
* DESCRIPTION:
*       Get Timestamp Local Action Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       messageType         - message type
*                             (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       entryPtr            - (pointer to)Timestamp Local Action Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsLocalActionTableGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainProfile,
    IN  GT_U32                                  messageType,
    OUT CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *entryPtr
)
{
    GT_U32     hwEntry;     /* HW formated table entry */
    GT_U32     hwData;      /* HW data                 */
    GT_STATUS  rc;          /* return code             */
    GT_U32     entryIndex;  /* table entry index       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    cpssOsMemSet(entryPtr, 0, sizeof(*entryPtr));

    if (domainProfile > 4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (messageType > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    entryIndex = (portNum * 128) + (messageType * 8) + domainProfile;

    rc = prvCpssDxChReadTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_LOCAL_ACTION_E,
        entryIndex, &hwEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwData = ((hwEntry >> 1) & 0xF);

    switch (hwData)
    {
        case 0:
            entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_NONE_E;
            break;
        case 2:
            entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_DROP_E;
            break;
        case 3:
            entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_E;
            break;
        case 4:
            entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_TIME_E;
            break;
        case 5:
            entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_ADD_CORRECTED_TIME_E;
            break;
        case 6:
            entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_TIME_E;
            break;
        case 7:
            entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_ADD_CORRECTED_TIME_E;
            break;
        case 10:
            entryPtr->tsAction = CPSS_DXCH_PTP_TS_ACTION_CAPTURE_INGRESS_TIME_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    hwData = (hwEntry & 1);
    entryPtr->ingrLinkDelayEnable = BIT2BOOL_MAC(hwData);

    hwData = ((hwEntry >> 5) & 1);
    entryPtr->packetDispatchingEnable = BIT2BOOL_MAC(hwData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsLocalActionTableGet
*
* DESCRIPTION:
*       Get Timestamp Local Action Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainProfile       - domain profile.
*                             (APPLICABLE RANGES: 0..8.)
*       messageType         - message type
*                             (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       entryPtr            - (pointer to)Timestamp Local Action Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsLocalActionTableGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainProfile,
    IN  GT_U32                                  messageType,
    OUT CPSS_DXCH_PTP_TS_LOCAL_ACTION_ENTRY_STC *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsLocalActionTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainProfile, messageType, entryPtr));

    rc = internal_cpssDxChPtpTsLocalActionTableGet(devNum, portNum, domainProfile, messageType, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainProfile, messageType, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsUdpChecksumUpdateModeSet
*
* DESCRIPTION:
*       Set Timestamp UDP Checksum Update Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       udpCsUpdModePtr     - (pointer to)Timestamp UDP Checksum Update Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsUdpChecksumUpdateModeSet
(
    IN  GT_U8                                         devNum,
    IN  CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *udpCsUpdModePtr
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwData;           /* HW data                      */
    GT_U32    hwValue;          /* HW value                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(udpCsUpdModePtr);

    hwData = 0;

    PRV_CPSS_CONVERT_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_TO_HW_VAL_MAC(
        hwValue, udpCsUpdModePtr->ptpIpv4Mode);
    hwData |= hwValue;

    PRV_CPSS_CONVERT_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_TO_HW_VAL_MAC(
        hwValue, udpCsUpdModePtr->ptpIpv6Mode);
    hwData |= (hwValue << 2);

    PRV_CPSS_CONVERT_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_TO_HW_VAL_MAC(
        hwValue, udpCsUpdModePtr->ntpIpv4Mode);
    hwData |= (hwValue << 4);

    PRV_CPSS_CONVERT_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_TO_HW_VAL_MAC(
        hwValue, udpCsUpdModePtr->ntpIpv6Mode);
    hwData |= (hwValue << 6);

    PRV_CPSS_CONVERT_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_TO_HW_VAL_MAC(
        hwValue, udpCsUpdModePtr->wampIpv4Mode);
    hwData |= (hwValue << 8);

    PRV_CPSS_CONVERT_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_TO_HW_VAL_MAC(
        hwValue, udpCsUpdModePtr->wampIpv6Mode);
    hwData |= (hwValue << 10);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).timestampingUDPChecksumMode;

    return prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
}

/*******************************************************************************
* cpssDxChPtpTsUdpChecksumUpdateModeSet
*
* DESCRIPTION:
*       Set Timestamp UDP Checksum Update Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       udpCsUpdModePtr     - (pointer to)Timestamp UDP Checksum Update Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsUdpChecksumUpdateModeSet
(
    IN  GT_U8                                         devNum,
    IN  CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *udpCsUpdModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsUdpChecksumUpdateModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, udpCsUpdModePtr));

    rc = internal_cpssDxChPtpTsUdpChecksumUpdateModeSet(devNum, udpCsUpdModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, udpCsUpdModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsUdpChecksumUpdateModeGet
*
* DESCRIPTION:
*       Get Timestamp UDP Checksum Update Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       udpCsUpdModePtr     - (pointer to)Timestamp UDP Checksum Update Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsUdpChecksumUpdateModeGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *udpCsUpdModePtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwData;           /* HW data                      */
    GT_U32    hwValue;          /* HW value                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(udpCsUpdModePtr);

    cpssOsMemSet(udpCsUpdModePtr, 0, sizeof(*udpCsUpdModePtr));

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).timestampingUDPChecksumMode;

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwValue = (hwData & 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_MAC(
        hwValue, udpCsUpdModePtr->ptpIpv4Mode);

    hwValue = ((hwData >> 2) & 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_MAC(
        hwValue, udpCsUpdModePtr->ptpIpv6Mode);

    hwValue = ((hwData >> 4) & 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_MAC(
        hwValue, udpCsUpdModePtr->ntpIpv4Mode);

    hwValue = ((hwData >> 6) & 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_MAC(
        hwValue, udpCsUpdModePtr->ntpIpv6Mode);

    hwValue = ((hwData >> 8) & 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_MAC(
        hwValue, udpCsUpdModePtr->wampIpv4Mode);

    hwValue = ((hwData >> 10) & 3);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_MAC(
        hwValue, udpCsUpdModePtr->wampIpv6Mode);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsUdpChecksumUpdateModeGet
*
* DESCRIPTION:
*       Get Timestamp UDP Checksum Update Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       udpCsUpdModePtr     - (pointer to)Timestamp UDP Checksum Update Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsUdpChecksumUpdateModeGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_PTP_TS_UDP_CHECKSUM_UPDATE_MODE_STC *udpCsUpdModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsUdpChecksumUpdateModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, udpCsUpdModePtr));

    rc = internal_cpssDxChPtpTsUdpChecksumUpdateModeGet(devNum, udpCsUpdModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, udpCsUpdModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpOverEthernetEnableSet
*
* DESCRIPTION:
*       Set enable PTP over Ethernet packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpOverEthernetEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_STATUS rc;               /* return code                      */
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwValue;          /* HW value                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPConfigs;
    hwValue = BOOL2BIT_MAC(enable);

    rc = prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 0, 1, hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPtpOverEthernetEnableSet
*
* DESCRIPTION:
*       Set enable PTP over Ethernet packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpOverEthernetEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpOverEthernetEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPtpOverEthernetEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpOverEthernetEnableGet
*
* DESCRIPTION:
*       Get enable PTP over Ethernet packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpOverEthernetEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;               /* return code                      */
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwValue;          /* HW value                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPConfigs;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 0, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpOverEthernetEnableGet
*
* DESCRIPTION:
*       Get enable PTP over Ethernet packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpOverEthernetEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpOverEthernetEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPtpOverEthernetEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpOverUdpEnableSet
*
* DESCRIPTION:
*       Set enable PTP over UDP packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpOverUdpEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_STATUS rc;               /* return code                      */
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwValue;          /* HW value                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPConfigs;
    hwValue = BOOL2BIT_MAC(enable);

    rc = prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 1, 1, hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPtpOverUdpEnableSet
*
* DESCRIPTION:
*       Set enable PTP over UDP packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE - enable, GT_FALSE - disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpOverUdpEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpOverUdpEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPtpOverUdpEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpOverUdpEnableGet
*
* DESCRIPTION:
*       Get enable PTP over UDP packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpOverUdpEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;               /* return code                      */
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwValue;          /* HW value                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPConfigs;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 1, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpOverUdpEnableGet
*
* DESCRIPTION:
*       Get enable PTP over UDP packet.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpOverUdpEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpOverUdpEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPtpOverUdpEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpDomainModeSet
*
* DESCRIPTION:
*       Set PTP domain mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain number.
*                        (APPLICABLE RANGES: 0..3.)
*       domainMode     - domain mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpDomainModeSet
(
    IN  GT_U8                                  devNum,
    IN  GT_U32                                 domainIndex,
    IN  CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT  domainMode
)
{
    GT_STATUS rc;               /* return code                      */
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwValue;          /* HW value                         */
    GT_U32    offset;           /* field offset                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (domainIndex > 3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (domainMode)
    {
        case CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_DISABLE_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V1_E:
            hwValue = 1;
            break;
        case CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V2_E:
            hwValue = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPConfigs;

    offset = 2 + (domainIndex * 2);

    rc = prvCpssDrvHwPpSetRegField(
        devNum, regAddr, offset, 2, hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPtpDomainModeSet
*
* DESCRIPTION:
*       Set PTP domain mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain number.
*                        (APPLICABLE RANGES: 0..3.)
*       domainMode     - domain mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainModeSet
(
    IN  GT_U8                                  devNum,
    IN  GT_U32                                 domainIndex,
    IN  CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT  domainMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpDomainModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, domainIndex, domainMode));

    rc = internal_cpssDxChPtpDomainModeSet(devNum, domainIndex, domainMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, domainIndex, domainMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpDomainModeGet
*
* DESCRIPTION:
*       Get PTP domain mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainModePtr   - (pointer to) domain mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpDomainModeGet
(
    IN  GT_U8                                  devNum,
    IN  GT_U32                                 domainIndex,
    OUT CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT  *domainModePtr
)
{
    GT_STATUS rc;               /* return code                      */
    GT_U32    regAddr;          /* register address                 */
    GT_U32    hwValue;          /* HW value                         */
    GT_U32    offset;           /* field offset                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(domainModePtr);

    if (domainIndex > 3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPConfigs;

    offset = 2 + (domainIndex * 2);

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, offset, 2, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hwValue)
    {
        default:
        case 0:
            *domainModePtr = CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_DISABLE_E;
            break;
        case 1:
            *domainModePtr = CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V1_E;
            break;
        case 2:
            *domainModePtr = CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_PTP_V2_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpDomainModeGet
*
* DESCRIPTION:
*       Get PTP domain mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainModePtr   - (pointer to) domain mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainModeGet
(
    IN  GT_U8                                  devNum,
    IN  GT_U32                                 domainIndex,
    OUT CPSS_DXCH_PTP_INGRESS_DOMAIN_MODE_ENT  *domainModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpDomainModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, domainIndex, domainModePtr));

    rc = internal_cpssDxChPtpDomainModeGet(devNum, domainIndex, domainModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, domainIndex, domainModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpDomainV1IdSet
*
* DESCRIPTION:
*       Set PTP V1 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain index.
*                        (APPLICABLE RANGES: 0..3.)
*       domainIdArr    - domain Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V1 Header contains 128-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpDomainV1IdSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    IN  GT_U32       domainIdArr[4]
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    i;                /* loop index                   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(domainIdArr);

    if (domainIndex > 3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for (i = 0; (i < 4); i++)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.
                _1588V1DomainDomainNumber[i][domainIndex];
        rc = prvCpssDrvHwPpWriteRegister(
            devNum, regAddr, domainIdArr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpDomainV1IdSet
*
* DESCRIPTION:
*       Set PTP V1 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain index.
*                        (APPLICABLE RANGES: 0..3.)
*       domainIdArr    - domain Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V1 Header contains 128-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainV1IdSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    IN  GT_U32       domainIdArr[4]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpDomainV1IdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, domainIndex, domainIdArr));

    rc = internal_cpssDxChPtpDomainV1IdSet(devNum, domainIndex, domainIdArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, domainIndex, domainIdArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpDomainV1IdGet
*
* DESCRIPTION:
*       Get PTP V1 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainIdArr     - domain Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V1 Header contains 128-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpDomainV1IdGet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    OUT GT_U32       domainIdArr[4]
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    i;                /* loop index                   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(domainIdArr);

    if (domainIndex > 3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for (i = 0; (i < 4); i++)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.
                _1588V1DomainDomainNumber[i][domainIndex];
        rc = prvCpssDrvHwPpReadRegister(
            devNum, regAddr, &(domainIdArr[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpDomainV1IdGet
*
* DESCRIPTION:
*       Get PTP V1 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainIdArr     - domain Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V1 Header contains 128-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainV1IdGet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    OUT GT_U32       domainIdArr[4]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpDomainV1IdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, domainIndex, domainIdArr));

    rc = internal_cpssDxChPtpDomainV1IdGet(devNum, domainIndex, domainIdArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, domainIndex, domainIdArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpDomainV2IdSet
*
* DESCRIPTION:
*       Set PTP V2 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain index.
*                        (APPLICABLE RANGES: 0..3.)
*       domainId       - domain Id
*                        (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V2 Header contains 8-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpDomainV2IdSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    IN  GT_U32       domainId
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    offset;           /* field offset                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (domainIndex > 3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (domainId > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP._1588V2DomainDomainNumber;
    offset = (domainIndex * 8);

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, offset, 8, domainId);
}

/*******************************************************************************
* cpssDxChPtpDomainV2IdSet
*
* DESCRIPTION:
*       Set PTP V2 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       domainIndex    - domain index.
*                        (APPLICABLE RANGES: 0..3.)
*       domainId       - domain Id
*                        (APPLICABLE RANGES: 0..255.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V2 Header contains 8-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainV2IdSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    IN  GT_U32       domainId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpDomainV2IdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, domainIndex, domainId));

    rc = internal_cpssDxChPtpDomainV2IdSet(devNum, domainIndex, domainId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, domainIndex, domainId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpDomainV2IdGet
*
* DESCRIPTION:
*       Get PTP V2 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainIdPtr     - (pointer to) domain Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V2 Header contains 8-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpDomainV2IdGet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    OUT GT_U32       *domainIdPtr
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    offset;           /* field offset                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(domainIdPtr);

    if (domainIndex > 3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP._1588V2DomainDomainNumber;
    offset = (domainIndex * 8);

    return prvCpssDrvHwPpGetRegField(
        devNum, regAddr, offset, 8, domainIdPtr);
}

/*******************************************************************************
* cpssDxChPtpDomainV2IdGet
*
* DESCRIPTION:
*       Get PTP V2 domain Id.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index.
*                         (APPLICABLE RANGES: 0..3.)
*
* OUTPUTS:
*       domainIdPtr     - (pointer to) domain Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP V2 Header contains 8-bit domain Id.
*       4 domain Id values mapped to domainIndex 0-3
*       all other domain Id values mapped to default domain
*
*******************************************************************************/
GT_STATUS cpssDxChPtpDomainV2IdGet
(
    IN  GT_U8        devNum,
    IN  GT_U32       domainIndex,
    OUT GT_U32       *domainIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpDomainV2IdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, domainIndex, domainIdPtr));

    rc = internal_cpssDxChPtpDomainV2IdGet(devNum, domainIndex, domainIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, domainIndex, domainIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpEgressDomainTableSet
*
* DESCRIPTION:
*       Set Egress Domain Table Entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainIndex         - domain index.
*                             (APPLICABLE RANGES: 0..4.)
*       entryPtr            - (pointer to) Domain Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEgressDomainTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainIndex,
    IN  CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC   *entryPtr
)
{
    GT_U32     hwEntryArr[2];  /* HW formated table entry */
    GT_U32     hwData;         /* HW data                 */
    GT_U32     entryIndex;     /* table entry index       */
    GT_U32     portFactor;     /* port factor */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    if (domainIndex > 4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    portFactor = (1+PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(devNum));/*128/256/512*/
    entryIndex = (domainIndex * portFactor) + portNum;

    hwEntryArr[0] = 0;
    hwEntryArr[1] = 0;

    hwData   = BOOL2BIT_MAC(entryPtr->ptpOverEhernetTsEnable);
    hwEntryArr[0] |= hwData;

    hwData   = BOOL2BIT_MAC(entryPtr->ptpOverUdpIpv4TsEnable);
    hwEntryArr[0] |= (hwData << 1);

    hwData   = BOOL2BIT_MAC(entryPtr->ptpOverUdpIpv6TsEnable);
    hwEntryArr[0] |= (hwData << 2);

    if (entryPtr->messageTypeTsEnableBmp > BIT_16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwEntryArr[0] |= (entryPtr->messageTypeTsEnableBmp << 3);

    if (entryPtr->transportSpecificTsEnableBmp > BIT_16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwEntryArr[0] |= ((entryPtr->transportSpecificTsEnableBmp & 0x1FFF) << 19);
    hwEntryArr[1] |= (entryPtr->transportSpecificTsEnableBmp >> 13);

    return prvCpssDxChWriteTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_HA_PTP_DOMAIN_E,
        entryIndex, hwEntryArr);
}

/*******************************************************************************
* cpssDxChPtpEgressDomainTableSet
*
* DESCRIPTION:
*       Set Egress Domain Table Entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainIndex         - domain index.
*                             (APPLICABLE RANGES: 0..4.)
*       entryPtr            - (pointer to) Domain Table Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressDomainTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainIndex,
    IN  CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC   *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEgressDomainTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainIndex, entryPtr));

    rc = internal_cpssDxChPtpEgressDomainTableSet(devNum, portNum, domainIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpEgressDomainTableGet
*
* DESCRIPTION:
*       Get Egress Domain Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainIndex         - domain index.
*                             (APPLICABLE RANGES: 0..4.)
*
* OUTPUTS:
*       entryPtr            - (pointer to) Egress Domain Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEgressDomainTableGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainIndex,
    OUT CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC   *entryPtr
)
{
    GT_STATUS  rc;             /* return code             */
    GT_U32     hwEntryArr[2];  /* HW formated table entry */
    GT_U32     hwData;         /* HW data                 */
    GT_U32     entryIndex;     /* table entry index       */
    GT_U32     portFactor;     /* port factor */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    cpssOsMemSet(entryPtr, 0, sizeof(*entryPtr));

    if (domainIndex > 4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    portFactor = (1+PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(devNum));/*128/256/512*/
    entryIndex = (domainIndex * portFactor) + portNum;

    rc = prvCpssDxChReadTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_HA_PTP_DOMAIN_E,
        entryIndex, hwEntryArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwData = (hwEntryArr[0] & 1);
    entryPtr->ptpOverEhernetTsEnable = BIT2BOOL_MAC(hwData);

    hwData = ((hwEntryArr[0] >> 1) & 1);
    entryPtr->ptpOverUdpIpv4TsEnable = BIT2BOOL_MAC(hwData);

    hwData = ((hwEntryArr[0] >> 2) & 1);
    entryPtr->ptpOverUdpIpv6TsEnable = BIT2BOOL_MAC(hwData);

    entryPtr->messageTypeTsEnableBmp = ((hwEntryArr[0] >> 3) & 0xFFFF);

    entryPtr->transportSpecificTsEnableBmp =
        ((hwEntryArr[0] >> 19) & 0x1FFF) | ((hwEntryArr[1] & 7) << 13);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpEgressDomainTableGet
*
* DESCRIPTION:
*       Get Egress Domain Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number.
*       domainIndex         - domain index.
*                             (APPLICABLE RANGES: 0..4.)
*
* OUTPUTS:
*       entryPtr            - (pointer to) Egress Domain Table Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressDomainTableGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_U32                                  domainIndex,
    OUT CPSS_DXCH_PTP_EGRESS_DOMAIN_ENTRY_STC   *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEgressDomainTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, domainIndex, entryPtr));

    rc = internal_cpssDxChPtpEgressDomainTableGet(devNum, portNum, domainIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, domainIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpIngressExceptionCfgSet
*
* DESCRIPTION:
*       Set PTP packet Ingress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       ingrExceptionCfgPtr  - (pointer to) PTP packet Ingress Exception Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpIngressExceptionCfgSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC   *ingrExceptionCfgPtr
)
{
    GT_U32                                  regAddr;    /* register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode; /* DSA code */
    GT_STATUS                               rc;         /* return code */
    GT_U32                                  hwData;     /* HW data     */
    GT_U32                                  hwMask;     /* HW mask     */
    GT_U32                                  hwCmd;      /* HW command  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(ingrExceptionCfgPtr);

    hwMask = 0;
    hwData = 0;
    dsaCpuCode = 0;

    hwMask |= 1;
    if (ingrExceptionCfgPtr->ptpExceptionCommandEnable != GT_FALSE)
    {
        PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(
            hwCmd, ingrExceptionCfgPtr->ptpExceptionCommand);
        hwData |= (hwCmd << 1) | 1;
        hwMask |= (7 << 1);

        rc = prvCpssDxChNetIfCpuToDsaCode(
            ingrExceptionCfgPtr->ptpExceptionCpuCode, &dsaCpuCode);
        if (rc != GT_OK)
        {
            if ((ingrExceptionCfgPtr->ptpExceptionCommand == CPSS_PACKET_CMD_TRAP_TO_CPU_E)
                || (ingrExceptionCfgPtr->ptpExceptionCommand == CPSS_PACKET_CMD_MIRROR_TO_CPU_E))
            {
                return rc;
            }
        }
        else
        {
            hwData |= (dsaCpuCode << 4);
            hwMask |= (0xFF << 4);
        }
    }

    hwData |= (BOOL2BIT_MAC(ingrExceptionCfgPtr->ptpVersionCheckEnable) << 22);
    hwMask |= (1 << 22);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPExceptionsAndCPUCodeConfig;

    return prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, hwMask, hwData);
}

/*******************************************************************************
* cpssDxChPtpIngressExceptionCfgSet
*
* DESCRIPTION:
*       Set PTP packet Ingress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       ingrExceptionCfgPtr  - (pointer to) PTP packet Ingress Exception Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressExceptionCfgSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC   *ingrExceptionCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpIngressExceptionCfgSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ingrExceptionCfgPtr));

    rc = internal_cpssDxChPtpIngressExceptionCfgSet(devNum, ingrExceptionCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ingrExceptionCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpIngressExceptionCfgGet
*
* DESCRIPTION:
*       Get PTP packet Ingress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       ingrExceptionCfgPtr  - (pointer to) PTP packet Ingress Exception Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpIngressExceptionCfgGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC   *ingrExceptionCfgPtr
)
{
    GT_U32                                  regAddr;    /* register address */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode; /* DSA code */
    GT_STATUS                               rc;         /* return code */
    GT_U32                                  hwData;     /* HW data     */
    GT_U32                                  hwMask;     /* HW mask     */
    GT_U32                                  hwCmd;      /* HW command  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(ingrExceptionCfgPtr);

    cpssOsMemSet(ingrExceptionCfgPtr, 0, sizeof(*ingrExceptionCfgPtr));
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPExceptionsAndCPUCodeConfig;
    hwMask = 0x00400FFF;

    rc = prvCpssDrvHwPpReadRegBitMask(devNum, regAddr, hwMask, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    ingrExceptionCfgPtr->ptpExceptionCommandEnable = BIT2BOOL_MAC((hwData & 1));
    ingrExceptionCfgPtr->ptpVersionCheckEnable = BIT2BOOL_MAC(((hwData >> 22) & 1));

    if (ingrExceptionCfgPtr->ptpVersionCheckEnable != GT_FALSE)
    {
        PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(
            ingrExceptionCfgPtr->ptpExceptionCommand, ((hwData >> 1) & 7));

        hwCmd = (hwData >> 4) & 0xFF;
        dsaCpuCode = (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)(hwCmd);

        rc = prvCpssDxChNetIfDsaToCpuCode(
            dsaCpuCode, &(ingrExceptionCfgPtr->ptpExceptionCpuCode));
        if (rc != GT_OK)
        {
            if ((ingrExceptionCfgPtr->ptpExceptionCommand == CPSS_PACKET_CMD_TRAP_TO_CPU_E)
                || (ingrExceptionCfgPtr->ptpExceptionCommand == CPSS_PACKET_CMD_MIRROR_TO_CPU_E))
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpIngressExceptionCfgGet
*
* DESCRIPTION:
*       Get PTP packet Ingress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       ingrExceptionCfgPtr  - (pointer to) PTP packet Ingress Exception Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressExceptionCfgGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PTP_INGRESS_EXCEPTION_CFG_STC   *ingrExceptionCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpIngressExceptionCfgGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ingrExceptionCfgPtr));

    rc = internal_cpssDxChPtpIngressExceptionCfgGet(devNum, ingrExceptionCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ingrExceptionCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpIngressPacketCheckingModeSet
*
* DESCRIPTION:
*       Set PTP packet Ingress Checking Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*       checkingMode    - PTP packet ingress checking mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpIngressPacketCheckingModeSet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   domainIndex,
    IN  GT_U32                                   messageType,
    IN  CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT  checkingMode
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwData;           /* HW data                      */
    GT_U32    offset;           /* field offset                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (domainIndex > 4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (messageType > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr =
        PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.
            PTPExceptionCheckingModeDomain[domainIndex];

    offset = 2 * messageType;

    switch (checkingMode)
    {
        case CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BASIC_E:
            hwData = 0;
            break;
        case CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_TRANS_CLK_E:
            hwData = 1;
            break;
        case CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_PIGGY_BACK_E:
            hwData = 2;
            break;
        case CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BOUNDRY_CLK_E:
            hwData = 3;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDrvHwPpSetRegField(
        devNum, regAddr, offset, 2, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChPtpIngressPacketCheckingModeSet
*
* DESCRIPTION:
*       Set PTP packet Ingress Checking Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*       checkingMode    - PTP packet ingress checking mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressPacketCheckingModeSet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   domainIndex,
    IN  GT_U32                                   messageType,
    IN  CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT  checkingMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpIngressPacketCheckingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, domainIndex, messageType, checkingMode));

    rc = internal_cpssDxChPtpIngressPacketCheckingModeSet(devNum, domainIndex, messageType, checkingMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, domainIndex, messageType, checkingMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpIngressPacketCheckingModeGet
*
* DESCRIPTION:
*       Get PTP packet Ingress Checking Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       checkingModePtr - (pointer to)PTP packet ingress checking mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpIngressPacketCheckingModeGet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   domainIndex,
    IN  GT_U32                                   messageType,
    OUT CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT  *checkingModePtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwData;           /* HW data                      */
    GT_U32    offset;           /* field offset                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(checkingModePtr);

    if (domainIndex > 4)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (messageType > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr =
        PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.
            PTPExceptionCheckingModeDomain[domainIndex];

    offset = 2 * messageType;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, offset, 2, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hwData)
    {
        default:
        case 0:
            *checkingModePtr = CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BASIC_E;
            break;
        case 1:
            *checkingModePtr = CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_TRANS_CLK_E;
            break;
        case 2:
            *checkingModePtr = CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_PIGGY_BACK_E;
            break;
        case 3:
            *checkingModePtr = CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_BOUNDRY_CLK_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpIngressPacketCheckingModeGet
*
* DESCRIPTION:
*       Get PTP packet Ingress Checking Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       domainIndex     - domain index
*                         (APPLICABLE RANGES: 0..4.)
*       messageType     - message type
*                         (APPLICABLE RANGES: 0..15.)
*
* OUTPUTS:
*       checkingModePtr - (pointer to)PTP packet ingress checking mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressPacketCheckingModeGet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   domainIndex,
    IN  GT_U32                                   messageType,
    OUT CPSS_DXCH_PTP_INGRESS_CHECKING_MODE_ENT  *checkingModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpIngressPacketCheckingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, domainIndex, messageType, checkingModePtr));

    rc = internal_cpssDxChPtpIngressPacketCheckingModeGet(devNum, domainIndex, messageType, checkingModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, domainIndex, messageType, checkingModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpIngressExceptionCounterGet
*
* DESCRIPTION:
*       Get PTP Ingress Exception packet Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       counterPtr  - (pointer to) PTP Ingress Exception packet Counter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counter is Clear On Read.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpIngressExceptionCounterGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *counterPtr
)
{
    GT_U32    regAddr;          /* register address                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).PTP.PTPExceptionsCntr;

    return prvCpssPortGroupsCounterSummary(
        devNum, regAddr,
        0 /*fieldOffset*/, 8 /*fieldLength*/,
        counterPtr, NULL /*counter64bitValuePtr*/);
}

/*******************************************************************************
* cpssDxChPtpIngressExceptionCounterGet
*
* DESCRIPTION:
*       Get PTP Ingress Exception packet Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       counterPtr  - (pointer to) PTP Ingress Exception packet Counter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counter is Clear On Read.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpIngressExceptionCounterGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *counterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpIngressExceptionCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, counterPtr));

    rc = internal_cpssDxChPtpIngressExceptionCounterGet(devNum, counterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, counterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpEgressExceptionCfgSet
*
* DESCRIPTION:
*       Set PTP packet Egress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       egrExceptionCfgPtr  - (pointer to) PTP packet Egress Exception Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEgressExceptionCfgSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC   *egrExceptionCfgPtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwData;           /* HW data                      */
    GT_U32    hwValue;          /* HW value                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(egrExceptionCfgPtr);

    hwData = 0;

    PRV_CPSS_CONVERT_PTP_EGRESS_EXCEPTION_CMD_TO_HW_VAL_MAC(
        hwValue, egrExceptionCfgPtr->invalidPtpPktCmd);
    hwData |= hwValue;

    PRV_CPSS_CONVERT_PTP_EGRESS_EXCEPTION_CMD_TO_HW_VAL_MAC(
        hwValue, egrExceptionCfgPtr->invalidInPiggybackPktCmd);
    hwData |= (hwValue << 1);

    PRV_CPSS_CONVERT_PTP_EGRESS_EXCEPTION_CMD_TO_HW_VAL_MAC(
        hwValue, egrExceptionCfgPtr->invalidOutPiggybackPktCmd);
    hwData |= (hwValue << 2);

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).PTPAndTimestampingExceptionConfig;

    rc = prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 0, 3, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_CONVERT_PTP_EGRESS_EXCEPTION_CMD_TO_HW_VAL_MAC(
        hwValue, egrExceptionCfgPtr->invalidTsPktCmd);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKPTPConfig;

    rc = prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 1, 1, hwValue);
    return rc;
}

/*******************************************************************************
* cpssDxChPtpEgressExceptionCfgSet
*
* DESCRIPTION:
*       Set PTP packet Egress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       egrExceptionCfgPtr  - (pointer to) PTP packet Egress Exception Configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressExceptionCfgSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC   *egrExceptionCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEgressExceptionCfgSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, egrExceptionCfgPtr));

    rc = internal_cpssDxChPtpEgressExceptionCfgSet(devNum, egrExceptionCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, egrExceptionCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpEgressExceptionCfgGet
*
* DESCRIPTION:
*       Get PTP packet Egress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       egrExceptionCfgPtr  - (pointer to) PTP packet Egress Exception Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEgressExceptionCfgGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC   *egrExceptionCfgPtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwData;           /* HW data                      */
    GT_U32    hwValue;          /* HW value                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(egrExceptionCfgPtr);

    cpssOsMemSet(egrExceptionCfgPtr, 0, sizeof(*egrExceptionCfgPtr));

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).PTPAndTimestampingExceptionConfig;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 0, 3, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwValue = (hwData & 1);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_EGRESS_EXCEPTION_CMD_MAC(
        hwValue, egrExceptionCfgPtr->invalidPtpPktCmd);

    hwValue = ((hwData >> 1) & 1);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_EGRESS_EXCEPTION_CMD_MAC(
        hwValue, egrExceptionCfgPtr->invalidInPiggybackPktCmd);

    hwValue = ((hwData >> 2) & 1);
    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_EGRESS_EXCEPTION_CMD_MAC(
        hwValue, egrExceptionCfgPtr->invalidOutPiggybackPktCmd);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKPTPConfig;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 1, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_CONVERT_HW_VAL_TO_PTP_EGRESS_EXCEPTION_CMD_MAC(
        hwValue, egrExceptionCfgPtr->invalidTsPktCmd);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpEgressExceptionCfgGet
*
* DESCRIPTION:
*       Get PTP packet Egress Exception Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       egrExceptionCfgPtr  - (pointer to) PTP packet Egress Exception Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressExceptionCfgGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_PTP_EGRESS_EXCEPTION_CFG_STC   *egrExceptionCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEgressExceptionCfgGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, egrExceptionCfgPtr));

    rc = internal_cpssDxChPtpEgressExceptionCfgGet(devNum, egrExceptionCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, egrExceptionCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpEgressExceptionCountersGet
*
* DESCRIPTION:
*       Get PTP packet Egress Exception Counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       egrExceptionCntPtr  - (pointer to) PTP packet Egress Exception Counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are Clear On Read.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpEgressExceptionCountersGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC   *egrExceptionCntPtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    regAddr;          /* register address             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(egrExceptionCntPtr);

    cpssOsMemSet(egrExceptionCntPtr, 0, sizeof(*egrExceptionCntPtr));

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKPTPInvalidTimestampCntr;

    rc = prvCpssPortGroupsCounterSummary(
        devNum, regAddr,
        0 /*fieldOffset*/, 8 /*fieldLength*/,
        &(egrExceptionCntPtr->invalidTsPktCnt), NULL /*counter64bitValuePtr*/);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).invalidPTPHeaderCntr;

    rc = prvCpssPortGroupsCounterSummary(
        devNum, regAddr,
        0 /*fieldOffset*/, 8 /*fieldLength*/,
        &(egrExceptionCntPtr->invalidPtpPktCnt), NULL /*counter64bitValuePtr*/);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).invalidPTPIncomingPiggybackCntr;

    rc = prvCpssPortGroupsCounterSummary(
        devNum, regAddr,
        0 /*fieldOffset*/, 8 /*fieldLength*/,
        &(egrExceptionCntPtr->invalidInPiggybackPktCnt), NULL /*counter64bitValuePtr*/);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).invalidPTPOutgoingPiggybackCntr;

    rc = prvCpssPortGroupsCounterSummary(
        devNum, regAddr,
        0 /*fieldOffset*/, 8 /*fieldLength*/,
        &(egrExceptionCntPtr->invalidOutPiggybackPktCnt), NULL /*counter64bitValuePtr*/);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpEgressExceptionCountersGet
*
* DESCRIPTION:
*       Get PTP packet Egress Exception Counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       egrExceptionCntPtr  - (pointer to) PTP packet Egress Exception Counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are Clear On Read.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpEgressExceptionCountersGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_PTP_EGRESS_EXCEPTION_COUNTERS_STC   *egrExceptionCntPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpEgressExceptionCountersGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, egrExceptionCntPtr));

    rc = internal_cpssDxChPtpEgressExceptionCountersGet(devNum, egrExceptionCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, egrExceptionCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPtpTsIngressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Ingress Timestamp Queue Entry in HW format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*       egressQueue          - GT_TRUE - egress Queue, GT_FALSE - ingress Queue
*
* OUTPUTS:
*       hwDataPtr            - (pointer to) buffer with the row entry.
*                              3 words for ingress queue
*                              2 words for ingress queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPtpTsIngressTimestampQueueEntryRead
(
    IN  GT_U8    devNum,
    IN  GT_U32   queueNum,
    IN  GT_BOOL  egressQueue,
    OUT GT_U32   *hwDataPtr
)
{
    GT_STATUS rc;               /* return code              */
    GT_U32    queueIndex;       /* queueIndex               */
    GT_U32    regAddr;          /* register address         */
    GT_U32    i;                /* loop index               */
    GT_U32    wordsNum;         /* num of queue entry words */

    queueIndex = ((BOOL2BIT_MAC(egressQueue) * 2) + queueNum);
    wordsNum   = ((egressQueue == GT_FALSE) ? 3 : 2);

    for (i = 0; (i < wordsNum); i++)
    {
        switch (i)
        {
            default:
            case 0:
                regAddr =
                    PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).
                    timestampQueueEntryWord0[queueIndex];
                break;
            case 1:
                regAddr =
                    PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).
                    timestampQueueEntryWord1[queueIndex];
                break;
            case 2: /* ingress only */
                regAddr =
                    PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).
                    timestampQueueEntryWord2[queueIndex];
                break;
        }

        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &(hwDataPtr[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPtpTsIngressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Ingress Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       tsQueueEntryPtr      - (pointer to) Ingress Timestamp Queue Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Queue read causes deleting the entry from the queue.
*       The next read will retrieve the next entry.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsIngressTimestampQueueEntryRead
(
    IN  GT_U8                                                devNum,
    IN  GT_U32                                               queueNum,
    OUT CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC   *tsQueueEntryPtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    hwEntry[3];       /* HW Entry                     */
    GT_U32    hwValue;          /* HW Value                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(tsQueueEntryPtr);

    cpssOsMemSet(tsQueueEntryPtr, 0, sizeof(*tsQueueEntryPtr));

    rc = prvCpssDxChPtpTsIngressTimestampQueueEntryRead(
        devNum, queueNum, GT_FALSE /*egressQueue*/, hwEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwValue = (hwEntry[0] & 1);
    tsQueueEntryPtr->entryValid = BIT2BOOL_MAC(hwValue);

    hwValue = ((hwEntry[0] >> 1) & 1);
    tsQueueEntryPtr->isPtpExeption = BIT2BOOL_MAC(hwValue);

    hwValue = ((hwEntry[0] >> 2) & 7);
    switch (hwValue)
    {
        case 0:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E;
            break;
        case 1:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E;
            break;
        case 2:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
            break;
        case 3:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TS_E;
            break;
        case 4:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_RX_E;
            break;
        case 5:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TX_E;
            break;
        case 6:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_WAMP_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    tsQueueEntryPtr->taiSelect = ((hwEntry[0] >> 5) & 1);

    tsQueueEntryPtr->todUpdateFlag = ((hwEntry[0] >> 6) & 1);

    tsQueueEntryPtr->messageType = ((hwEntry[0] >> 7) & 0xF);

    tsQueueEntryPtr->domainNum = ((hwEntry[0] >> 11) & 0xFF);

    tsQueueEntryPtr->sequenceId =
        ((hwEntry[0] >> 19) & 0x1FFF) | ((hwEntry[1] & 0x7) << 13);

    tsQueueEntryPtr->timestamp =
        ((hwEntry[1] >> 3) & 0x1FFFFFFF) | ((hwEntry[2] & 0x7) << 29);

    tsQueueEntryPtr->portNum = ((hwEntry[2] >> 3) & 0xFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsIngressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Ingress Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       tsQueueEntryPtr      - (pointer to) Ingress Timestamp Queue Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Queue read causes deleting the entry from the queue.
*       The next read will retrieve the next entry.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsIngressTimestampQueueEntryRead
(
    IN  GT_U8                                                devNum,
    IN  GT_U32                                               queueNum,
    OUT CPSS_DXCH_PTP_TS_INGRESS_TIMESTAMP_QUEUE_ENTRY_STC   *tsQueueEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsIngressTimestampQueueEntryRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueNum, tsQueueEntryPtr));

    rc = internal_cpssDxChPtpTsIngressTimestampQueueEntryRead(devNum, queueNum, tsQueueEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueNum, tsQueueEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsEgressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Egress Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       tsQueueEntryPtr      - (pointer to) Egress Timestamp Queue Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Queue read causes deleting the entry from the queue.
*       The next read will retrieve the next entry.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsEgressTimestampQueueEntryRead
(
    IN  GT_U8                                                devNum,
    IN  GT_U32                                               queueNum,
    OUT CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC    *tsQueueEntryPtr
)
{
    GT_STATUS rc;               /* return code                  */
    GT_U32    hwEntry[2];       /* HW Entry                     */
    GT_U32    hwValue;          /* HW Value                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(tsQueueEntryPtr);

    cpssOsMemSet(tsQueueEntryPtr, 0, sizeof(*tsQueueEntryPtr));

    rc = prvCpssDxChPtpTsIngressTimestampQueueEntryRead(
        devNum, queueNum, GT_TRUE /*egressQueue*/, hwEntry);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwValue = (hwEntry[0] & 1);
    tsQueueEntryPtr->entryValid = BIT2BOOL_MAC(hwValue);

    hwValue = ((hwEntry[0] >> 1) & 1);
    tsQueueEntryPtr->isPtpExeption = BIT2BOOL_MAC(hwValue);

    hwValue = ((hwEntry[0] >> 2) & 7);
    switch (hwValue)
    {
        case 0:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V2_E;
            break;
        case 1:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_PTP_V1_E;
            break;
        case 2:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_Y1731_E;
            break;
        case 3:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TS_E;
            break;
        case 4:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_RX_E;
            break;
        case 5:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_NTP_TX_E;
            break;
        case 6:
            tsQueueEntryPtr->packetFormat = CPSS_DXCH_PTP_TS_PACKET_TYPE_WAMP_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    tsQueueEntryPtr->messageType = ((hwEntry[0] >> 5) & 0xF);

    tsQueueEntryPtr->domainNum = ((hwEntry[0] >> 9) & 0xFF);

    tsQueueEntryPtr->sequenceId =
        ((hwEntry[0] >> 17) & 0x7FFF) | ((hwEntry[1] & 0x1) << 15);

    tsQueueEntryPtr->queueEntryId = ((hwEntry[1] >> 1) & 0x3FF);

    tsQueueEntryPtr->portNum = ((hwEntry[1] >> 11) & 0xFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsEgressTimestampQueueEntryRead
*
* DESCRIPTION:
*       Read Egress Timestamp Queue Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       queueNum             - Queue Number
*                              (APPLICABLE RANGES: 0..1.)
*
* OUTPUTS:
*       tsQueueEntryPtr      - (pointer to) Egress Timestamp Queue Entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Queue read causes deleting the entry from the queue.
*       The next read will retrieve the next entry.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsEgressTimestampQueueEntryRead
(
    IN  GT_U8                                                devNum,
    IN  GT_U32                                               queueNum,
    OUT CPSS_DXCH_PTP_TS_EGRESS_TIMESTAMP_QUEUE_ENTRY_STC    *tsQueueEntryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsEgressTimestampQueueEntryRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueNum, tsQueueEntryPtr));

    rc = internal_cpssDxChPtpTsEgressTimestampQueueEntryRead(devNum, queueNum, tsQueueEntryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueNum, tsQueueEntryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsMessageTypeToQueueIdMapSet
*
* DESCRIPTION:
*       Set PTP Message Type To Queue Id Map.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       idMapBmp       - PTP Message Type To Queue Id Map.
*                        16 bits, bit per message type
*                        0 - queue0, 1 - queue1
*                        (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsMessageTypeToQueueIdMapSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       idMapBmp
)
{
    GT_U32    regAddr;          /* register address         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (idMapBmp > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).timestampQueueMsgType;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 0, 16, idMapBmp);
}

/*******************************************************************************
* cpssDxChPtpTsMessageTypeToQueueIdMapSet
*
* DESCRIPTION:
*       Set PTP Message Type To Queue Id Map.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       idMapBmp       - PTP Message Type To Queue Id Map.
*                        16 bits, bit per message type
*                        0 - queue0, 1 - queue1
*                        (APPLICABLE RANGES: 0..0xFFFF.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsMessageTypeToQueueIdMapSet
(
    IN  GT_U8        devNum,
    IN  GT_U32       idMapBmp
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsMessageTypeToQueueIdMapSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, idMapBmp));

    rc = internal_cpssDxChPtpTsMessageTypeToQueueIdMapSet(devNum, idMapBmp);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, idMapBmp));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsMessageTypeToQueueIdMapGet
*
* DESCRIPTION:
*       Get PTP Message Type To Queue Id Map.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       idMapBmpPtr    - (pointer to) PTP Message Type To Queue Id Map.
*                        16 bits, bit per message type
*                        0 - queue0, 1 - queue1
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsMessageTypeToQueueIdMapGet
(
    IN  GT_U8        devNum,
    OUT GT_U32       *idMapBmpPtr
)
{
    GT_U32    regAddr;          /* register address         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(idMapBmpPtr);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).timestampQueueMsgType;

    return prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 0, 16, idMapBmpPtr);
}

/*******************************************************************************
* cpssDxChPtpTsMessageTypeToQueueIdMapGet
*
* DESCRIPTION:
*       Get PTP Message Type To Queue Id Map.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       idMapBmpPtr    - (pointer to) PTP Message Type To Queue Id Map.
*                        16 bits, bit per message type
*                        0 - queue0, 1 - queue1
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsMessageTypeToQueueIdMapGet
(
    IN  GT_U8        devNum,
    OUT GT_U32       *idMapBmpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsMessageTypeToQueueIdMapGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, idMapBmpPtr));

    rc = internal_cpssDxChPtpTsMessageTypeToQueueIdMapGet(devNum, idMapBmpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, idMapBmpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsQueuesEntryOverrideEnableSet
*
* DESCRIPTION:
*       Set Timestamp Queues Override Enable.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       enable          - when the TS Queue is full the new timestamp
*                         GT_TRUE  - overrides an oldest Queue entry.
*                         GT_FALSE - not queued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       cpssDxChPtpTsuControlSet should configure the same value per port
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsQueuesEntryOverrideEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_U32    regAddr;          /* register address         */
    GT_U32    hwValue;          /* HW Value                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    hwValue = BOOL2BIT_MAC(enable);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKPTPConfig;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 0, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPtpTsQueuesEntryOverrideEnableSet
*
* DESCRIPTION:
*       Set Timestamp Queues Override Enable.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       enable          - when the TS Queue is full the new timestamp
*                         GT_TRUE  - overrides an oldest Queue entry.
*                         GT_FALSE - not queued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       cpssDxChPtpTsuControlSet should configure the same value per port
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsQueuesEntryOverrideEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsQueuesEntryOverrideEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPtpTsQueuesEntryOverrideEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsQueuesEntryOverrideEnableGet
*
* DESCRIPTION:
*       Get Timestamp Queues Override Enable.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to)when the TS Queue is full the new timestamp
*                         GT_TRUE  - overrides an oldest Queue entry.
*                         GT_FALSE - not queued.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsQueuesEntryOverrideEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32    regAddr;          /* register address          */
    GT_U32    hwValue;          /* HW Value                  */
    GT_STATUS rc;               /* return code               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKPTPConfig;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 0, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPtpTsQueuesEntryOverrideEnableGet
*
* DESCRIPTION:
*       Get Timestamp Queues Override Enable.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to)when the TS Queue is full the new timestamp
*                         GT_TRUE  - overrides an oldest Queue entry.
*                         GT_FALSE - not queued.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsQueuesEntryOverrideEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsQueuesEntryOverrideEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPtpTsQueuesEntryOverrideEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsQueuesSizeSet
*
* DESCRIPTION:
*       Set Timestamp Queues Size.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       queueSize       - The size of each PTP Queue.
*                         (APPLICABLE RANGES: 0..256.)
*                         If Timestamp Queues Override is Enable,
*                         the size should be (maximal - 2) == 254.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsQueuesSizeSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      queueSize
)
{
    GT_U32    regAddr;          /* register address         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (queueSize > 256)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKPTPConfig;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 2, 9, queueSize);
}

/*******************************************************************************
* cpssDxChPtpTsQueuesSizeSet
*
* DESCRIPTION:
*       Set Timestamp Queues Size.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       queueSize       - The size of each PTP Queue.
*                         (APPLICABLE RANGES: 0..256.)
*                         If Timestamp Queues Override is Enable,
*                         the size should be (maximal - 2) == 254.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsQueuesSizeSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      queueSize
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsQueuesSizeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueSize));

    rc = internal_cpssDxChPtpTsQueuesSizeSet(devNum, queueSize);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueSize));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsQueuesSizeGet
*
* DESCRIPTION:
*       Get Timestamp Queues Size.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       queueSizePtr    - (pointer to)The size of each PTP Queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsQueuesSizeGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *queueSizePtr
)
{
    GT_U32    regAddr;          /* register address         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(queueSizePtr);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKPTPConfig;

    return prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 2, 9, queueSizePtr);
}

/*******************************************************************************
* cpssDxChPtpTsQueuesSizeGet
*
* DESCRIPTION:
*       Get Timestamp Queues Size.
*       Related to all 2 ingress and 2 egress Timestamp Queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       queueSizePtr    - (pointer to)The size of each PTP Queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsQueuesSizeGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *queueSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsQueuesSizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueSizePtr));

    rc = internal_cpssDxChPtpTsQueuesSizeGet(devNum, queueSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDebugQueuesEntryIdsClear
*
* DESCRIPTION:
*       Set Timestamp Queues Current Entry Ids Clear.
*       For Debug only.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDebugQueuesEntryIdsClear
(
    IN  GT_U8       devNum
)
{
    GT_U32    regAddr;          /* register address         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).timestampQueueEntryIDClear;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 0, 1, 1);
}

/*******************************************************************************
* cpssDxChPtpTsDebugQueuesEntryIdsClear
*
* DESCRIPTION:
*       Set Timestamp Queues Current Entry Ids Clear.
*       For Debug only.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDebugQueuesEntryIdsClear
(
    IN  GT_U8       devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDebugQueuesEntryIdsClear);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChPtpTsDebugQueuesEntryIdsClear(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsDebugQueuesEntryIdsGet
*
* DESCRIPTION:
*       Get Timestamp Queues Current Entry IDs.
*       For Debug only.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       queueEntryId0Ptr    - (pointer to)The PTP Queue0 current entry Id.
*       queueEntryId1Ptr    - (pointer to)The PTP Queue1 current entry Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsDebugQueuesEntryIdsGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *queueEntryId0Ptr,
    OUT GT_U32      *queueEntryId1Ptr
)
{
    GT_U32    regAddr;          /* register address         */
    GT_U32    hwValue;          /* HW Value                  */
    GT_STATUS rc;               /* return code               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(queueEntryId0Ptr);
    CPSS_NULL_PTR_CHECK_MAC(queueEntryId1Ptr);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).timestampQueueEntryID;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 0, 20, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *queueEntryId0Ptr = (hwValue & 0x3FF);
    *queueEntryId1Ptr = ((hwValue >> 10) & 0x3FF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTsDebugQueuesEntryIdsGet
*
* DESCRIPTION:
*       Get Timestamp Queues Current Entry IDs.
*       For Debug only.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       queueEntryId0Ptr    - (pointer to)The PTP Queue0 current entry Id.
*       queueEntryId1Ptr    - (pointer to)The PTP Queue1 current entry Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsDebugQueuesEntryIdsGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *queueEntryId0Ptr,
    OUT GT_U32      *queueEntryId1Ptr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsDebugQueuesEntryIdsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueEntryId0Ptr, queueEntryId1Ptr));

    rc = internal_cpssDxChPtpTsDebugQueuesEntryIdsGet(devNum, queueEntryId0Ptr, queueEntryId1Ptr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueEntryId0Ptr, queueEntryId1Ptr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTsNtpTimeOffsetSet
*
* DESCRIPTION:
*       Set TS NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       ntpTimeOffset   - NTP PTP Time Offset measured in seconds.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The same value should be configured per port.
*       See cpssDxChPtpTsuNtpTimeOffsetSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsNtpTimeOffsetSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ntpTimeOffset
)
{
    GT_U32    regAddr;          /* register address         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).PTPNTPOffset;

    return prvCpssDrvHwPpWriteRegister(
        devNum, regAddr, ntpTimeOffset);
}

/*******************************************************************************
* cpssDxChPtpTsNtpTimeOffsetSet
*
* DESCRIPTION:
*       Set TS NTP Time Offset.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       ntpTimeOffset   - NTP PTP Time Offset measured in seconds.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The same value should be configured per port.
*       See cpssDxChPtpTsuNtpTimeOffsetSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsNtpTimeOffsetSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ntpTimeOffset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsNtpTimeOffsetSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ntpTimeOffset));

    rc = internal_cpssDxChPtpTsNtpTimeOffsetSet(devNum, ntpTimeOffset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ntpTimeOffset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPtpTsNtpTimeOffsetGet
*
* DESCRIPTION:
*       Get TS NTP Time Offset.
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
*       ntpTimeOffsetPtr - (pointer to) NTP PTP Time Offset measured in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTsNtpTimeOffsetGet
(
    IN  GT_U8                             devNum,
    OUT GT_U32                            *ntpTimeOffsetPtr
)
{
    GT_U32    regAddr;          /* register address         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(ntpTimeOffsetPtr);

    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).PTPNTPOffset;

    return prvCpssDrvHwPpReadRegister(
        devNum, regAddr, ntpTimeOffsetPtr);
}

/*******************************************************************************
* cpssDxChPtpTsNtpTimeOffsetGet
*
* DESCRIPTION:
*       Get TS NTP Time Offset.
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
*       ntpTimeOffsetPtr - (pointer to) NTP PTP Time Offset measured in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTsNtpTimeOffsetGet
(
    IN  GT_U8                             devNum,
    OUT GT_U32                            *ntpTimeOffsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTsNtpTimeOffsetGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ntpTimeOffsetPtr));

    rc = internal_cpssDxChPtpTsNtpTimeOffsetGet(devNum, ntpTimeOffsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ntpTimeOffsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPtpPulseIterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP pulse interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       pulseInterfaceDirection - PTP pulse interface direction.
*       taiNumber               - TAI number selection.
*                                (relevant for PTP output pulse)
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP pulse interface used for generating/receiving discrete pulses
*       that trigger a time-related operation such as Update or Capture.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPtpPulseIterfaceSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      pulseInterfaceDirection,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT            taiNumber
)
{
    GT_U32    regAddr;          /* register address             */
    GT_STATUS rc;               /* return code                  */
    GT_U32    hwValue;          /* HW value                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    rc = prvCpssDrvHwPpResetAndInitControllerReadReg(devNum, regAddr, &hwValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(pulseInterfaceDirection)
    {
        case CPSS_DIRECTION_INGRESS_E:
            hwValue |= (1 << 28);
            /* this bit[22] must be 0 when listening external pulses  */
            /* Switching value 0 to 1 in this bit triggers            */
            /* sending pulses to external devices - implemented in    */
            /* cpssDxChPtpTaiTodCounterFunctionAllTriggerSet          */
            hwValue &= ~(1 << 22);
            break;
        case CPSS_DIRECTION_EGRESS_E:
            hwValue &= ~(1 << 28);
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
            {
                switch(taiNumber)
                {
                    case CPSS_DXCH_PTP_TAI_NUMBER_0_E:
                        hwValue &= ~(1 << 23);
                        break;
                    case CPSS_DXCH_PTP_TAI_NUMBER_1_E:
                        hwValue |=  (1 << 23);
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    return prvCpssDrvHwPpResetAndInitControllerWriteReg(devNum, regAddr, hwValue);
}

/*******************************************************************************
* cpssDxChPtpTaiPtpPulseIterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP pulse interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number.
*       pulseInterfaceDirection - PTP pulse interface direction.
*       taiNumber               - TAI number selection.
*                                (relevant for PTP output pulse)
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP pulse interface used for generating/receiving discrete pulses
*       that trigger a time-related operation such as Update or Capture.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpPulseIterfaceSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DIRECTION_ENT                      pulseInterfaceDirection,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT            taiNumber
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPtpPulseIterfaceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pulseInterfaceDirection, taiNumber));

    rc = internal_cpssDxChPtpTaiPtpPulseIterfaceSet(devNum, pulseInterfaceDirection, taiNumber);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pulseInterfaceDirection, taiNumber));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPtpPulseIterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP pulse interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       pulseInterfaceDirectionPtr  - (pointer to) PTP pulse interface direction.
*       taiNumberPtr                - (pointer to) TAI number selection.
*                                     (relevant for PTP output pulse)
*                                     For devices that have single TAI unit (Caelum) parameter ignored.
*                                     (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP pulse interface used for generating/receiving discrete pulses
*       that trigger a time-related operation such as Update or Capture.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPtpPulseIterfaceGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DIRECTION_ENT                   *pulseInterfaceDirectionPtr,
    OUT CPSS_DXCH_PTP_TAI_NUMBER_ENT         *taiNumberPtr
)
{
    GT_U32    regAddr;          /* register address             */
    GT_STATUS rc;               /* return code                  */
    GT_U32    hwValue;          /* HW value                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );
    CPSS_NULL_PTR_CHECK_MAC(pulseInterfaceDirectionPtr);
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
    {
        CPSS_NULL_PTR_CHECK_MAC(taiNumberPtr);
    }

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    rc = prvCpssDrvHwPpResetAndInitControllerReadReg(devNum, regAddr, &hwValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (((hwValue >> 28) & 0x1) == 0)
    {
        *pulseInterfaceDirectionPtr = CPSS_DIRECTION_EGRESS_E;
    }
    else
    {
        *pulseInterfaceDirectionPtr = CPSS_DIRECTION_INGRESS_E;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
    {
        if(((hwValue >> 23) & 0x1) == 0)
        {
            *taiNumberPtr = CPSS_DXCH_PTP_TAI_NUMBER_0_E;
        }
        else
        {
            *taiNumberPtr = CPSS_DXCH_PTP_TAI_NUMBER_1_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpPulseIterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP pulse interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       pulseInterfaceDirectionPtr  - (pointer to) PTP pulse interface direction.
*       taiNumberPtr                - (pointer to) TAI number selection.
*                                     (relevant for PTP output pulse)
*                                     For devices that have single TAI unit (Caelum) parameter ignored.
*                                     (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP pulse interface used for generating/receiving discrete pulses
*       that trigger a time-related operation such as Update or Capture.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpPulseIterfaceGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DIRECTION_ENT                   *pulseInterfaceDirectionPtr,
    OUT CPSS_DXCH_PTP_TAI_NUMBER_ENT         *taiNumberPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPtpPulseIterfaceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pulseInterfaceDirectionPtr, taiNumberPtr));

    rc = internal_cpssDxChPtpTaiPtpPulseIterfaceGet(devNum, pulseInterfaceDirectionPtr, taiNumberPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pulseInterfaceDirectionPtr, taiNumberPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPtpClockInterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP clock interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI Number.
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockInterfaceDirection - PTP clock interface direction.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP clock interface used for generating/receiving a periodic pulse
*       such as 1 PPS signal.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPtpClockInterfaceSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT         taiNumber,
    IN  CPSS_DIRECTION_ENT                   clockInterfaceDirection
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    fieldValue;       /* register field value         */
    GT_U32    bitOffset;        /* field bit offset             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    switch(clockInterfaceDirection)
    {
        case CPSS_DIRECTION_INGRESS_E:
            fieldValue = 1;
            break;
        case CPSS_DIRECTION_EGRESS_E:
            fieldValue = 0;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
    {
        switch(taiNumber)
        {
            case CPSS_DXCH_PTP_TAI_NUMBER_0_E:
                bitOffset = 24;
                break;
            case CPSS_DXCH_PTP_TAI_NUMBER_1_E:
                bitOffset = 25;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        bitOffset = 24;
    }

    return prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum, regAddr,
                                                           bitOffset, 1, fieldValue);
}

/*******************************************************************************
* cpssDxChPtpTaiPtpClockInterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP clock interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI Number.
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockInterfaceDirection - PTP clock interface direction.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP clock interface used for generating/receiving a periodic pulse
*       such as 1 PPS signal.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpClockInterfaceSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT         taiNumber,
    IN  CPSS_DIRECTION_ENT                   clockInterfaceDirection
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPtpClockInterfaceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiNumber, clockInterfaceDirection));

    rc = internal_cpssDxChPtpTaiPtpClockInterfaceSet(devNum, taiNumber, clockInterfaceDirection);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiNumber, clockInterfaceDirection));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPtpClockInterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP clock interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI number.
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockInterfaceDirectionPtr   - (pointer to) PTP clock interface direction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP clock interface used for generating/receiving a periodic pulse
*       such as 1 PPS signal.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPtpClockInterfaceGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT         taiNumber,
    OUT CPSS_DIRECTION_ENT                   *clockInterfaceDirectionPtr
)
{
    GT_U32    regAddr;          /* register address             */
    GT_STATUS rc;               /* return code                  */
    GT_U32    hwValue;          /* HW value                     */
    GT_U32    bitOffset;        /* field bit offset             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );
    CPSS_NULL_PTR_CHECK_MAC(clockInterfaceDirectionPtr);

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    rc = prvCpssDrvHwPpResetAndInitControllerReadReg(devNum, regAddr, &hwValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
    {
        switch(taiNumber)
        {
            case CPSS_DXCH_PTP_TAI_NUMBER_0_E:
                bitOffset = 24;
                break;
            case CPSS_DXCH_PTP_TAI_NUMBER_1_E:
                bitOffset = 25;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        bitOffset = 24;
    }

    if(((hwValue >> bitOffset) & 0x1) == 0)
    {
        *clockInterfaceDirectionPtr = CPSS_DIRECTION_EGRESS_E;
    }
    else
    {
        *clockInterfaceDirectionPtr = CPSS_DIRECTION_INGRESS_E;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPtpClockInterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP clock interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI number.
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockInterfaceDirectionPtr   - (pointer to) PTP clock interface direction.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PTP clock interface used for generating/receiving a periodic pulse
*       such as 1 PPS signal.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPtpClockInterfaceGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT         taiNumber,
    OUT CPSS_DIRECTION_ENT                   *clockInterfaceDirectionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPtpClockInterfaceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiNumber, clockInterfaceDirectionPtr));

    rc = internal_cpssDxChPtpTaiPtpClockInterfaceGet(devNum, taiNumber, clockInterfaceDirectionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiNumber, clockInterfaceDirectionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPClkOutputInterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP PClk or Sync-E recovered clock output interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       pclkRcvrClkMode     - clock output interface mode.
*       taiNumber           - TAI number - used as clock’s source for PClk output mode.
*                             (relevant for "PTP" mode)
*                             For devices that have single TAI unit (Caelum) parameter ignored.
*                             (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPClkOutputInterfaceSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT    pclkRcvrClkMode,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT                taiNumber
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    fieldValue;       /* register field value         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl10;

    switch(pclkRcvrClkMode)
    {
        case CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_PCLK_E:
            fieldValue = 0;
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
            {
                switch(taiNumber)
                {
                    case CPSS_DXCH_PTP_TAI_NUMBER_0_E:
                        break;
                    case CPSS_DXCH_PTP_TAI_NUMBER_1_E:
                        fieldValue |= (1 << 1);
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            break;
        case CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_RCVR_CLK_E:
            fieldValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum, regAddr,
                                                           14, 2, fieldValue);
}

/*******************************************************************************
* cpssDxChPtpTaiPClkOutputInterfaceSet
*
* DESCRIPTION:
*       Set TAI PTP PClk or Sync-E recovered clock output interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       pclkRcvrClkMode     - clock output interface mode.
*       taiNumber           - TAI number - used as clock’s source for PClk output mode.
*                             (relevant for "PTP" mode)
*                             For devices that have single TAI unit (Caelum) parameter ignored.
*                             (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPClkOutputInterfaceSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT    pclkRcvrClkMode,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT                taiNumber
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPClkOutputInterfaceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pclkRcvrClkMode, taiNumber));

    rc = internal_cpssDxChPtpTaiPClkOutputInterfaceSet(devNum, pclkRcvrClkMode, taiNumber);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pclkRcvrClkMode, taiNumber));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiPClkOutputInterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP Pclock or Sync-E recovered clock output interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       pclkRcvrClkModePtr  - (pointer to) clock output interface mode.
*       taiNumberPtr        - (pointer to) TAI number - used as clock’s source for PClk output mode.
*                             (relevant for "PTP" mode)
*                             For devices that have single TAI unit (Caelum) parameter ignored.
*                             (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiPClkOutputInterfaceGet
(
    IN  GT_U8                                       devNum,
    OUT CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT    *pclkRcvrClkModePtr,
    OUT CPSS_DXCH_PTP_TAI_NUMBER_ENT                *taiNumberPtr
)
{
    GT_U32    regAddr;          /* register address             */
    GT_STATUS rc;               /* return code                  */
    GT_U32    fieldValue;       /* register field value         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );
    CPSS_NULL_PTR_CHECK_MAC(pclkRcvrClkModePtr);
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
    {
        CPSS_NULL_PTR_CHECK_MAC(taiNumberPtr);
    }

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl10;

    rc = prvCpssDrvHwPpResetAndInitControllerGetRegField(devNum, regAddr,
                                                         14, 2, &fieldValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    if ((fieldValue & 0x1) == 0)
    {
        *pclkRcvrClkModePtr = CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_PCLK_E;
    }
    else
    {
        *pclkRcvrClkModePtr = CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_RCVR_CLK_E;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
    {
        if(((fieldValue >> 1) & 0x1) == 0)
        {
            *taiNumberPtr = CPSS_DXCH_PTP_TAI_NUMBER_0_E;
        }
        else
        {
            *taiNumberPtr = CPSS_DXCH_PTP_TAI_NUMBER_1_E;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiPClkOutputInterfaceGet
*
* DESCRIPTION:
*       Get TAI PTP Pclock or Sync-E recovered clock output interface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       pclkRcvrClkModePtr  - (pointer to) clock output interface mode.
*       taiNumberPtr        - (pointer to) TAI number - used as clock’s source for PClk output mode.
*                             (relevant for "PTP" mode)
*                             For devices that have single TAI unit (Caelum) parameter ignored.
*                             (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiPClkOutputInterfaceGet
(
    IN  GT_U8                                       devNum,
    OUT CPSS_DXCH_PTP_TAI_PCLK_RCVR_CLK_MODE_ENT    *pclkRcvrClkModePtr,
    OUT CPSS_DXCH_PTP_TAI_NUMBER_ENT                *taiNumberPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiPClkOutputInterfaceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pclkRcvrClkModePtr, taiNumberPtr));

    rc = internal_cpssDxChPtpTaiPClkOutputInterfaceGet(devNum, pclkRcvrClkModePtr, taiNumberPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pclkRcvrClkModePtr, taiNumberPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiInputClockSelectSet
*
* DESCRIPTION:
*       Configures input TAI clock's selection.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       taiNumber           - TAI number.
*                             For devices that have single TAI unit (Caelum) parameter ignored.
*                             (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockSelect         - input clock selection mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiInputClockSelectSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber,
    IN  CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT  clockSelect
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    fieldValue;       /* register field value         */
    GT_U32    bitOffset;        /* field bit offset             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
    {
        switch(taiNumber)
        {
            case CPSS_DXCH_PTP_TAI_NUMBER_0_E:
                bitOffset = 26;
                break;
            case CPSS_DXCH_PTP_TAI_NUMBER_1_E:
                bitOffset = 27;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        bitOffset = 26;
    }

    switch(clockSelect)
    {
        case CPSS_DXCH_PTP_TAI_CLOCK_SELECT_PTP_PLL_E:
            fieldValue = 0;
            break;
        case CPSS_DXCH_PTP_TAI_CLOCK_SELECT_CORE_PLL_E:
            fieldValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum, regAddr,
                                                           bitOffset, 1, fieldValue);
}

/*******************************************************************************
* cpssDxChPtpTaiInputClockSelectSet
*
* DESCRIPTION:
*       Configures input TAI clock's selection.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       taiNumber           - TAI number.
*                             For devices that have single TAI unit (Caelum) parameter ignored.
*                             (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*       clockSelect         - input clock selection mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInputClockSelectSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber,
    IN  CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT  clockSelect
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiInputClockSelectSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiNumber, clockSelect));

    rc = internal_cpssDxChPtpTaiInputClockSelectSet(devNum, taiNumber, clockSelect);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiNumber, clockSelect));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiInputClockSelectGet
*
* DESCRIPTION:
*       Get input TAI clock's selection.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI number.
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockSelectPtr          - (pointer to) input clock selection mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiInputClockSelectGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber,
    OUT CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT  *clockSelectPtr
)
{
    GT_U32    regAddr;          /* register address             */
    GT_STATUS rc;               /* return code                  */
    GT_U32    fieldValue;       /* register field value         */
    GT_U32    bitOffset;        /* field bit offset             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );
    CPSS_NULL_PTR_CHECK_MAC(clockSelectPtr);

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_tai.supportSingleInstance == GT_FALSE)
    {
        switch(taiNumber)
        {
            case CPSS_DXCH_PTP_TAI_NUMBER_0_E:
                bitOffset = 26;
                break;
            case CPSS_DXCH_PTP_TAI_NUMBER_1_E:
                bitOffset = 27;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        bitOffset = 26;
    }

    rc = prvCpssDrvHwPpResetAndInitControllerGetRegField(devNum, regAddr,
                                                         bitOffset, 1, &fieldValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(fieldValue == 0)
    {
        *clockSelectPtr = CPSS_DXCH_PTP_TAI_CLOCK_SELECT_PTP_PLL_E;
    }
    else
    {
        *clockSelectPtr = CPSS_DXCH_PTP_TAI_CLOCK_SELECT_CORE_PLL_E;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpTaiInputClockSelectGet
*
* DESCRIPTION:
*       Get input TAI clock's selection.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       taiNumber               - TAI number.
*                                 For devices that have single TAI unit (Caelum) parameter ignored.
*                                 (APPLICABLE DEVICES: Bobcat2; Bobcat3.)
*
* OUTPUTS:
*       clockSelectPtr          - (pointer to) input clock selection mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiInputClockSelectGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PTP_TAI_NUMBER_ENT        taiNumber,
    OUT CPSS_DXCH_PTP_TAI_CLOCK_SELECT_ENT  *clockSelectPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiInputClockSelectGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, taiNumber, clockSelectPtr));

    rc = internal_cpssDxChPtpTaiInputClockSelectGet(devNum, taiNumber, clockSelectPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, taiNumber, clockSelectPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpTaiTodCounterFunctionAllTriggerSet
*
* DESCRIPTION:
*       Triggeres TOD update operation for all TAI units.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpTaiTodCounterFunctionAllTriggerSet
(
    IN GT_U8                    devNum
)
{
    GT_U32    regAddr;          /* register address             */
    GT_STATUS rc;               /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );

    /* Bobcat2; Caelum; Bobcat3 */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        /* current GM doesn't support DFX unit so far */
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
        DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    /* toggle required for trigger */
    /* unset trigger bit */
    rc = prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum, regAddr, 22, 1, 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set trigger bit */
    return prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum, regAddr, 22, 1, 1);
}

/*******************************************************************************
* cpssDxChPtpTaiTodCounterFunctionAllTriggerSet
*
* DESCRIPTION:
*       Triggeres TOD update operation for all TAI units.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpTaiTodCounterFunctionAllTriggerSet
(
    IN GT_U8                    devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpTaiTodCounterFunctionAllTriggerSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChPtpTaiTodCounterFunctionAllTriggerSet(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPtpReset
*
* DESCRIPTION:
*       Reset/unreset PTP unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       resetTxUnit - GT_TRUE - reset PTP trasmit unit,
*                     GT_FALSE - unreset PTP trasmit unit
*                     For Bobcat2 related to both transmit and receive units
*       resetRxUnit - GT_TRUE - reset PTP receive unit
*                     GT_FALSE - unreset PTP receive unit
*                     (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPtpReset
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         resetTxUnit,
    IN  GT_BOOL                         resetRxUnit
)
{
    GT_U32      portMacNum;     /* port MAC number */
    GT_U32      regAddr;        /* register address */
    GT_U32      resetMask;      /* reset bits mask */
    GT_U32      resetData;      /* reset bits data */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(
        devNum, portNum, portMacNum);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, portMacNum).PTPGeneralCtrl;

    resetData = 0;
    /* Any PP: bit1 - PTP reset */
    resetMask = 2;
    if (resetTxUnit == GT_FALSE)
    {
        resetData |= 2;
    }
    if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        /* Caelum bit13 - PTP Rx reset */
        resetMask |= 0x2000;
        if (resetRxUnit == GT_FALSE)
        {
            resetData |= 0x2000;
        }
    }

    /* Set PTP unit software reset bit [1:1] [13:13] */
    return prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, resetMask, resetData);
}

/*******************************************************************************
* internal_cpssDxChPtpPortUnitResetSet
*
* DESCRIPTION:
*       Reset/unreset Port PTP unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       resetTxUnit - GT_TRUE - reset PTP trasmit unit,
*                     GT_FALSE - unreset PTP trasmit unit
*                     For Bobcat2 related to both transmit and receive units
*       resetRxUnit - GT_TRUE - reset PTP receive unit
*                     GT_FALSE - unreset PTP receive unit
*                     (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortUnitResetSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         resetTxUnit,
    IN  GT_BOOL                         resetRxUnit
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );

    return prvCpssDxChPortPtpReset(devNum,portNum,resetTxUnit,resetRxUnit);
}

/*******************************************************************************
* cpssDxChPtpPortUnitResetSet
*
* DESCRIPTION:
*       Reset/unreset Port PTP unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       resetTxUnit - GT_TRUE - reset PTP trasmit unit,
*                     GT_FALSE - unreset PTP trasmit unit
*                     For Bobcat2 related to both transmit and receive units
*       resetRxUnit - GT_TRUE - reset PTP receive unit
*                     GT_FALSE - unreset PTP receive unit
*                     (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortUnitResetSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         resetTxUnit,
    IN  GT_BOOL                         resetRxUnit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortUnitResetSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, resetTxUnit, resetRxUnit));

    rc = internal_cpssDxChPtpPortUnitResetSet(
        devNum, portNum, resetTxUnit, resetRxUnit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC(
        (funcId, rc, devNum, portNum, resetTxUnit, resetRxUnit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortUnitResetGet
*
* DESCRIPTION:
*       Get Reset/unreset Port PTP unit state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       resetTxUnitPtr - (pointer to)
*                        GT_TRUE - reset PTP trasmit unit,
*                        GT_FALSE - unreset PTP trasmit unit
*                        For Bobcat2 related to both transmit and receive units
*       resetRxUnitPtr - (pointer to)
*                        GT_TRUE - reset PTP receive unit
*                        GT_FALSE - unreset PTP receive unit
*                        (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortUnitResetGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_BOOL                         *resetTxUnitPtr,
    OUT GT_BOOL                         *resetRxUnitPtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      portMacNum;     /* port MAC number */
    GT_U32      regAddr;        /* register address */
    GT_U32      data;           /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E  );

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(
        devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(resetTxUnitPtr);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, portMacNum).PTPGeneralCtrl;

    /* Get PTP unit software reset bit [1:1] [13:13] */
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Any PP bit1 - PTP Tx reset */
    *resetTxUnitPtr = (data & 2) ? GT_FALSE : GT_TRUE;
    if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        /* Caelum bit13 - PTP Rx reset */
        CPSS_NULL_PTR_CHECK_MAC(resetRxUnitPtr);
        *resetRxUnitPtr = (data & 0x2000) ? GT_FALSE : GT_TRUE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPtpPortUnitResetGet
*
* DESCRIPTION:
*       Get Reset/unreset Port PTP unit state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       resetTxUnitPtr - (pointer to)
*                        GT_TRUE - reset PTP trasmit unit,
*                        GT_FALSE - unreset PTP trasmit unit
*                        For Bobcat2 related to both transmit and receive units
*       resetRxUnitPtr - (pointer to)
*                        GT_TRUE - reset PTP receive unit
*                        GT_FALSE - unreset PTP receive unit
*                        (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortUnitResetGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_BOOL                         *resetTxUnitPtr,
    OUT GT_BOOL                         *resetRxUnitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortUnitResetGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, resetTxUnitPtr, resetRxUnitPtr));

    rc = internal_cpssDxChPtpPortUnitResetGet(
        devNum, portNum, resetTxUnitPtr, resetRxUnitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC(
        (funcId, rc, devNum, portNum, resetTxUnitPtr, resetRxUnitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortTxPipeStatusDelaySet
*
* DESCRIPTION:
*       Set PTP Tx Pipe Status Delay.
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       delay       - delay of empty egress pipe (in core clock cycles).
*                     (APPLICABLE RANGE: 0..0xFFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_OUT_OF_RANGE   - on out of range parameter value
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortTxPipeStatusDelaySet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                          delay
)
{
    GT_U32      portMacNum;     /* port MAC number */
    GT_U32      regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E );

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(
        devNum, portNum, portMacNum);

    if (delay >= BIT_16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, portMacNum).txPipeStatusDelay;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 0, 16, delay);
}

/*******************************************************************************
* cpssDxChPtpPortTxPipeStatusDelaySet
*
* DESCRIPTION:
*       Set PTP Tx Pipe Status Delay.
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       delay       - delay of empty egress pipe (in core clock cycles).
*                     (APPLICABLE RANGE: 0..0xFFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_OUT_OF_RANGE   - on out of range parameter value
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTxPipeStatusDelaySet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                          delay
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortTxPipeStatusDelaySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, delay));

    rc = internal_cpssDxChPtpPortTxPipeStatusDelaySet(
        devNum, portNum, delay);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC(
        (funcId, rc, devNum, portNum, delay));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPtpPortTxPipeStatusDelayGet
*
* DESCRIPTION:
*       Get PTP Tx Pipe Status Delay.
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       delayPtr    - (pointer to)delay of empty egress pipe (in core clock cycles).
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPtpPortTxPipeStatusDelayGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_U32                          *delayPtr
)
{
    GT_U32      portMacNum;     /* port MAC number */
    GT_U32      regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E );

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(
        devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(delayPtr);

    regAddr = PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(devNum, portMacNum).txPipeStatusDelay;

    return prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 0, 16, delayPtr);
}

/*******************************************************************************
* cpssDxChPtpPortTxPipeStatusDelayGet
*
* DESCRIPTION:
*       Get PTP Tx Pipe Status Delay.
*
* APPLICABLE DEVICES:
*        Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       delayPtr    - (pointer to)delay of empty egress pipe (in core clock cycles).
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPtpPortTxPipeStatusDelayGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_U32                          *delayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPtpPortTxPipeStatusDelayGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, delayPtr));

    rc = internal_cpssDxChPtpPortTxPipeStatusDelayGet(
        devNum, portNum, delayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC(
        (funcId, rc, devNum, portNum, delayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


