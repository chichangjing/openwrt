/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenPhyVct.c
*
* DESCRIPTION:
*       API definitions for CPSS Marvell Virtual Cable Tester functionality.
*
*
* FILE REVISION NUMBER:
*       $Revision: 26 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/phy/private/prvCpssPhy.h>
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h>
#include <cpss/generic/phy/cpssGenPhyVct.h>
#include <cpss/dxCh/dxChxGen/phy/private/prvCpssDxChPhyLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfoEnhanced.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*#define DEBUG_VCT*/
#define CPSS_LOG_IN_MODULE_ENABLE

#ifdef DEBUG_VCT
#define DBG_VCT_MAC(x)   osPrintf x
#else
#define DBG_VCT_MAC(x)
#endif

#define VCT_RETRY_COUNTER_CNS 1000

/* Amplitude threshold for cables shorter then 110m */
#define THRESHOLD_AMPLITUDE_VALUE_CNS 40

/* Amplitude threshold for cables longer then 110m */
#define THRESHOLD_AMPLITUDE_VALUE_FOR_110M_CABLE_CNS 20

/* 110m minimum possible measured length due to 10% possible deviation. */
/* (110 * 90% = 99 > 98).                                               */
#define MIN_110M_CABLE_LEN_DUE_TO_DEVIATION_CNS 98


/*******************************************************************************
 * External usage variables
 ******************************************************************************/
/* values for display register 31 values to accurate cable length for GE PHYs */
/* register 30 value = 18 to accurateCableLenArr[18]=0      */
/* register 30 value = 19 to accurateCableLenArr[19]=2      */
/* ...                                                      */
/* register 30 value = 97 to accurateCableLenArr[97]=134    */
static const GT_U16 accurateCableLenArr[] = {
                                 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                 0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
                                 4,   5,   6,   9,  13,  17,  20,  23,  27,  30,
                                33,  35,  38,  41,  43,  46,  48,  51,  53,  55,
                                58,  60,  62,  64,  66,  68,  70,  72,  73,  75,
                                77,  79,  80,  82,  84,  85,  87,  88,  90,  91,
                                93,  94,  96,  97,  98, 100, 101, 102, 104, 105,
                               106, 107, 109, 110, 111, 112, 113, 114, 116, 117,
                               118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
                               128, 129, 130, 131, 132, 133, 134, 134, 0  , 0
                               };
/* values for display register 21 values to accurate cable length for GE PHYs
   E1149R and E1240 */
/* register 21 value = 18 to accurateCableLenArr[18]=3      */
/* register 21 value = 19 to accurateCableLenArr[19]=5      */
/* ...                                                      */
/* register 21 value = 91 to accurateCableLenArr[93]=130    */
const GT_U16 accurateCableLenArr1340[] = {
                                 1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
                                 1,   1,   1,   1,   1,   1,   1,   1,   4,   7,
                                 11,  15,  19,  22,  25,  28,  31,  34,  37,  39,
                                 42,  45,  48,  51,  53,  55,  58,  60,  62,  64,
                                 66,  69,  71,  73,  75,  76,  77,  79,  81,  83,
                                 84,  86,  88,  89,  91,  92,  93,  94,  96,  97,
                                 98,  100, 102, 103, 104, 105, 106, 108, 109, 110,
                                 111, 112, 113, 114, 116, 117, 118, 119, 120, 121,
                                 122, 123, 124, 125, 126, 127, 128, 129, 130};

const GT_U16 accurateCableLenArr1240[] = {
                                 1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
                                 1,   1,   1,   1,   1,   1,   1,   1,   3,   5,
                                 7,  10,  13,  15,  18,  22,  26,  30,  33,  35,
                                38,  40,  43,  45,  48,  51,  53,  55,  58,  60,
                                63,  65,  68,  69,  70,  71,  73,  75,  77,  79,
                                80,  81,  82,  83,  85,  87,  88,  90,  91,  92,
                                93,  95,  97,  98, 100, 101, 102, 103, 105, 106,
                               107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
                               117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
                               127, 128, 129, 130};

const GT_U16 accurateCableLenArr1149r[] = {
                                 1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
                                 1,   1,   1,   1,   1,   4,   7,  11,  15,  19,
                                23,  27,  30,  33,  37,  40,  42,  45,  48,  50,
                                53,  57,  60,  62,  64,  66,  68,  70,  72,  75,
                                78,  80,  82,  84,  85,  87,  89,  90,  91,  93,
                                94,  95,  96,  97,  99, 100, 101, 103, 105, 107,
                               109, 110, 111, 113, 115, 116, 117, 119, 120, 121,
                               122, 123, 125, 127, 128, 129, 130};

const GT_U16 accurateCableLenArr1680LRevA0[][111] = {
    {
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        1,   2,   4,   5,   7,   8,  10,  11,  12,  14,
        15,  16,  18,  19,  20,  21,  23,  24,  25,  26,
        27,  28,  29,  30,  32,  33,  34,  35,  36,  37,
        38,  39,  40,  41,  41,  42,  43,  44,  45,  46,
        47,  48,  49,  49,  50,  51,  52,  53,  54,  54,
        55,  56,  57,  57,  58,  59,  60,  60,  61,  62,
        63
    },
    {
        14,   14,   14,   14,   14,   14,   14,   14,   14,   14,
        14,   14,   14,   14,   14,   14,   14,   14,   14,   14,
        14,   18,   22,   25,   29,   32,   35,   39,   42,   45,
        47,   50,   53,   55,   58,   60,   62,   65,   67,   69,
        71,   73,   75,   77,   79,   81,   83,   85,   86,   88,
        90,   91,   93,   95,   96,   98,   99,   101,  102,  103,
        105,  106,  108,  109,  110,  111,  113,  114,  115,  116,
        118,  119,  120,  121,  122,  123,  124,  126,  127,  128,
        129,  130
    },
    {
        60,   60,   60,   60,   60,   60,   60,   60,   60,   60,
        60,   60,   60,   60,   60,   60,   60,   60,   60,   60,
        60,   60,   60,   60,   60,   60,   60,   60,   60,   60,
        62,   65,   67,   70,   72,   75,   77,   79,   81,   84,
        86,   88,   90,   91,   93,   95,   97,   99,   100,  102,
        104,  105,  107,  108,  110,  111,  113,  114,  116,  117,
        118,  120,  121,  122,  124,  125,  126,  127,  129,  130
    }};

/*
 * typedef: struct VCT_INFO
 *
 * Description: VCT state machine's information per port.
 *
 * Enumerations:
 *      reg0Val   - Stores phy's reg 0  original value.
 *      reg17Val  - Stores phy's reg 17 original value.
 *      reg26Val  - Stores phy's reg 26 original value.
 *      phyType   - Stores this port's phy type.
 *      disableWa - If a WA is enabled (GT_TRUE) it should be disabled.
 *      vctState  - Stores the VCT current state.
 *      currentTxPair - current launching signal pair
 *      rxPairsRes    - database to hold results of all 4 RX pairs per each
 *                      TX pair
 * Comments:
 *          A seperate state machine would be held for each port.
 */
typedef struct
{
    GT_U16       reg0Val;
    GT_U16       reg17Val;
    GT_U16       reg26Val;
    CPSS_VCT_PHY_TYPES_ENT phyType;
    GT_BOOL      disableWa;
    CPSS_VCT_STATE vctState;
    GT_U16       currentTxPair;
    GT_U16       rxPairsRes[16];
}CPSS_VCT_INFO_STC;


static CPSS_VCT_INFO_STC *vctInfo = NULL;

static GT_STATUS prvGetErrGeCabLen(GT_U8 dev, GT_PHYSICAL_PORT_NUM port, GT_U8 dist, GT_U8 *lengthPtr);
static GT_STATUS prvGetErrFeCabLen(GT_U8 dev, GT_PHYSICAL_PORT_NUM port, GT_U8 dist, GT_U8 *lengthPtr);
static GT_STATUS prvGetErrGeCabLenNG(GT_U8 dev, GT_PHYSICAL_PORT_NUM port, GT_U32 dist, GT_U16  phyIdRegValue, GT_U8 *lengthPtr);
static GT_STATUS  prvPhySoftReset(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS  prvSetTdrWorkAround(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS  prvEnableTdrFe1WorkAround(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS  prvDisableTdrFe1WorkAround(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS  prvEnableTdrFe2WorkAround(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS  prvDisableTdrFe2WorkAround(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS  prvEnableTdrGeWorkAround(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS  prvDisableTdrGeWorkAround(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);

static GT_STATUS prvDoesDeviceSupportVct(GT_U8 dev, GT_PHYSICAL_PORT_NUM port,
                                      CPSS_VCT_PHY_TYPES_ENT* phyType,
                                      GT_U16 *phyIdRegValue);

static GT_STATUS prvGetExtendedCableStatus_Phy100M(GT_U8 dev, GT_PHYSICAL_PORT_NUM port,
                                CPSS_VCT_CABLE_EXTENDED_STATUS_STC *extendedCableStatusPtr);
static GT_STATUS prvGetExtendedCableStatus_Phy1000M(GT_U8 dev, GT_PHYSICAL_PORT_NUM port,
                                CPSS_VCT_CABLE_EXTENDED_STATUS_STC *extendedCableStatusPtr);
static GT_STATUS prvGetExtendedCableStatus_Phy1000MMP(GT_U8 dev, GT_PHYSICAL_PORT_NUM port,
                                                   CPSS_VCT_PHY_TYPES_ENT phyType,
                                CPSS_VCT_CABLE_EXTENDED_STATUS_STC *extendedCableStatusPtr);

static GT_STATUS prvGetExtendedCableStatus_Phy1000MMP_NG(GT_U8 dev, GT_PHYSICAL_PORT_NUM port,
                                                         GT_U16 phyIdRegValue,
                                CPSS_VCT_CABLE_EXTENDED_STATUS_STC *extendedCableStatusPtr);

static GT_STATUS prvVctReady_Phy100M(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS prvVctStarted_Phy100M(GT_U8 dev,GT_PHYSICAL_PORT_NUM port, CPSS_VCT_STATE  action,
                                    CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr);
static GT_STATUS prvVctRedoPhase2_Phy100M(GT_U8 dev,GT_PHYSICAL_PORT_NUM port,
                                       CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr);

static GT_STATUS prvVctReady_Phy1000M(GT_U8 dev, GT_PHYSICAL_PORT_NUM port);
static GT_STATUS prvVctStarted_Phy1000M(GT_U8 dev,GT_PHYSICAL_PORT_NUM port,
                                     CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr);

static GT_STATUS prvVctReady_Phy1000MMP(GT_U8 dev, GT_PHYSICAL_PORT_NUM port,
                                     CPSS_VCT_PHY_TYPES_ENT phyType,
                                     CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr);
static GT_STATUS prvVctStarted_Phy1000MMP(GT_U8 dev,GT_PHYSICAL_PORT_NUM port,
                                       CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr);


static GT_STATUS gtPrvSetTxPulseTwistedPair
(
    IN  GT_U8     dev,
    IN  GT_PHYSICAL_PORT_NUM     port,
    IN  GT_U16     channel
);

static GT_STATUS prvVctReady_Phy1000MMP_NG
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM           port,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
);
static GT_STATUS prvVctStarted_Phy1000MMP_NG
(
    IN  GT_U8  dev,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  GT_U16 phyIdRegValue,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
);

/*******************************************************************************
* internal_cpssVctCableExtendedStatusGet
*
* DESCRIPTION:
*       This routine returns the extended VCT status per MDI pair/channel pair.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       extendedCableStatusPtr - extended virtual cable diagnostic status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If PHY is CPSS_VCT_PHY_100M_E (FE) cableStatusPtr will have only 2 pairs
*       relevant. In this case:
*       swap - just pairSwap[0] relevant
*              vctExtendedCableStatus.pairSwap[1] = CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairSwap[2] = CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairSwap[3] = CPSS_VCT_NOT_APPLICABLE_E
*       polarity - just pairPolarity[0] relevant
*              vctExtendedCableStatus.pairPolarity[1] =CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairPolarity[2] =CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairPolarity[3] =CPSS_VCT_NOT_APPLICABLE_E
*       skew - not relevant
*              vctExtendedCableStatus.skew[0,1,2,3].IsValid = GT_FALSE
*       accurateCableLen - not relevant
*              vctExtendedCableStatus.accurateCableLen.isValid = GT_FALSE
*       twoPairDownShift - not relevant
*              extendedCableStatusPtr->twoPairDownShift =
*                                           CPSS_VCT_NOT_APPLICABLE_DOWNSHIFT_E;
*       The API doesn't perform VCT test, but retrieves extended info regarding
*       the above features, which has collected by: cpssVctCableStatusGet API.
*******************************************************************************/
static GT_STATUS internal_cpssVctCableExtendedStatusGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    OUT CPSS_VCT_CABLE_EXTENDED_STATUS_STC *extendedCableStatusPtr
)
{
    GT_U32                  i;              /* iterator*/
    CPSS_VCT_PHY_TYPES_ENT  phyType;        /* PHY type*/
    GT_STATUS               status;         /* return code */
    GT_BOOL                 autoNegStatus;  /* PP to PHY Auto - Negotiation status*/
    GT_U16                  phyIdRegValue;  /* PHY ID register value */
    GT_U32                  portMacNum;

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check port and device number validity */
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, port, portMacNum);

    /* check read function was bound */
    CPSS_NULL_PTR_CHECK_MAC(PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->
                                            genVctBindFunc.cpssPhyRegisterRead);
    /* check write function was bound */
    CPSS_NULL_PTR_CHECK_MAC(PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->
                                           genVctBindFunc.cpssPhyRegisterWrite);
    /* check extendedCableStatusPtr is not NULL */
    CPSS_NULL_PTR_CHECK_MAC(extendedCableStatusPtr);

    /* VCT test is not supported on 10Giga ports */
    if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType
                                                          >= PRV_CPSS_PORT_XG_E)
    {
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }


    /* set default values */
    extendedCableStatusPtr->vctExtendedCableStatus.isValid = GT_FALSE;
    extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[0] =
                                                 CPSS_VCT_NOT_APPLICABLE_SWAP_E;
    extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[1] =
                                                 CPSS_VCT_NOT_APPLICABLE_SWAP_E;
    extendedCableStatusPtr->vctExtendedCableStatus.pairSkew.isValid = GT_FALSE;
    extendedCableStatusPtr->twoPairDownShift = CPSS_VCT_NOT_APPLICABLE_DOWNSHIFT_E;
    for (i=0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
    {
        extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[i] =
                                             CPSS_VCT_NOT_APPLICABLE_POLARITY_E;
        extendedCableStatusPtr->vctExtendedCableStatus.pairSkew.skew[i] = 0;
        extendedCableStatusPtr->accurateCableLen.cableLen[i] = 0;
        extendedCableStatusPtr->accurateCableLen.isValid[i] = GT_FALSE;
    }

    /* gets the PHY type */
    status = prvDoesDeviceSupportVct(devNum, port, &phyType, &phyIdRegValue);
    if (status != GT_OK)
        return status;

    switch(phyType)
    {
        case CPSS_VCT_PHY_100M_E:
            status = prvGetExtendedCableStatus_Phy100M(devNum, port,
                                                           extendedCableStatusPtr);
            break;
        case CPSS_VCT_PHY_1000M_E:
        case CPSS_VCT_PHY_1000M_B_E:
            status = prvGetExtendedCableStatus_Phy1000M(devNum, port,
                                                           extendedCableStatusPtr);
            break;
        case CPSS_VCT_PHY_1000M_MP_E:
        case CPSS_VCT_PHY_1000M_MP_NO_FIBER_E:
        case CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E:
            /* for multi-page phy disable PPU (AutoNeg) before running VCT */
            /* check AutoNeg function was bound */
            autoNegStatus = (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->
                     genVctBindFunc.cpssPhyAutoNegotiationGet == NULL)
                     ? GT_FALSE : GT_TRUE;

            /* get AutoNeg status */
            if(autoNegStatus)
            {
                status = PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->
                    genVctBindFunc.cpssPhyAutoNegotiationGet(devNum, port,
                                                                &autoNegStatus);
            }
            /* in case AutoNeg enable, disable it*/
            if(autoNegStatus)
            {

               PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->genVctBindFunc.
                                  cpssPhyAutoNegotiationSet(devNum, port, GT_FALSE);
            }

            if(phyType == CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E)
            {
                status = prvGetExtendedCableStatus_Phy1000MMP_NG(devNum,
                                                                 port,
                                                                 phyIdRegValue,
                                                           extendedCableStatusPtr);
            }
            else
            {
                status = prvGetExtendedCableStatus_Phy1000MMP(devNum,
                                                              port, phyType,
                                                           extendedCableStatusPtr);
            }

            if(autoNegStatus)
            {
                /* Restore AutoNeg status */
                PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->genVctBindFunc.
                                   cpssPhyAutoNegotiationSet(devNum, port, GT_TRUE);
            }

            break;
        default:
            status = GT_FAIL;
            break;
    }

    return status;
}

/*******************************************************************************
* cpssVctCableExtendedStatusGet
*
* DESCRIPTION:
*       This routine returns the extended VCT status per MDI pair/channel pair.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       extendedCableStatusPtr - extended virtual cable diagnostic status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If PHY is CPSS_VCT_PHY_100M_E (FE) cableStatusPtr will have only 2 pairs
*       relevant. In this case:
*       swap - just pairSwap[0] relevant
*              vctExtendedCableStatus.pairSwap[1] = CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairSwap[2] = CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairSwap[3] = CPSS_VCT_NOT_APPLICABLE_E
*       polarity - just pairPolarity[0] relevant
*              vctExtendedCableStatus.pairPolarity[1] =CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairPolarity[2] =CPSS_VCT_NOT_APPLICABLE_E
*              vctExtendedCableStatus.pairPolarity[3] =CPSS_VCT_NOT_APPLICABLE_E
*       skew - not relevant
*              vctExtendedCableStatus.skew[0,1,2,3].IsValid = GT_FALSE
*       accurateCableLen - not relevant
*              vctExtendedCableStatus.accurateCableLen.isValid = GT_FALSE
*       twoPairDownShift - not relevant
*              extendedCableStatusPtr->twoPairDownShift =
*                                           CPSS_VCT_NOT_APPLICABLE_DOWNSHIFT_E;
*       The API doesn't perform VCT test, but retrieves extended info regarding
*       the above features, which has collected by: cpssVctCableStatusGet API.
*******************************************************************************/
GT_STATUS cpssVctCableExtendedStatusGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    OUT CPSS_VCT_CABLE_EXTENDED_STATUS_STC *extendedCableStatusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssVctCableExtendedStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, extendedCableStatusPtr));

    rc = internal_cpssVctCableExtendedStatusGet(devNum, port, extendedCableStatusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, extendedCableStatusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssVctCableStatusGet
*
* DESCRIPTION:
*       This routine asserts / aborts the state-machine based virtual cable test
*       (VCT) for the requested port. When test completes, it returns the the
*       status per MDI pair, otherwize it will return current test status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       vctAction - The VCT action to perform on this port.
*
* OUTPUTS:
*       cableStatusPtr - virtual cable diagnostic status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_READY     - when action is CPSS_VCT_GET_RES_E and the test is not
*                          completed yet.
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Scenario:
*       1)First call with CPSS_VCT_START_E to begin the test
*       2)Call the function with CPSS_VCT_GET_RES_E  and wait till GT_OK as
*         relult.
*         If GT_NOT_READY is returned, call the CPSS_VCT_GET_RES_E again after a
*         while in order to let the test be completed.
*
*******************************************************************************/
static GT_STATUS internal_cpssVctCableStatusGet
(
    IN  GT_U8                     dev,
    IN  GT_PHYSICAL_PORT_NUM      port,
    IN  CPSS_VCT_ACTION_ENT       vctAction,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{
    GT_U32       i;                 /* iterator*/
    GT_STATUS    status;            /* return code */
    GT_BOOL      autoNegStatus;     /* PP to PHY Auto - Negotiation status*/
    GT_U16       phyIdRegValue;     /* PHY ID register value */
    GT_U32 portMacNum;

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(dev) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check port and device number validity */
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(dev, port, portMacNum);

    /* check read function was bound */
    CPSS_NULL_PTR_CHECK_MAC(PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->
                                            genVctBindFunc.cpssPhyRegisterRead);
    /* check write function was bound */
    CPSS_NULL_PTR_CHECK_MAC(PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->
                                           genVctBindFunc.cpssPhyRegisterWrite);
    /* check cableStatusPtr is not NULL */
    CPSS_NULL_PTR_CHECK_MAC(cableStatusPtr);

    /* VCT test is not supported on 10Giga ports */
    if(PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType
                                                          >= PRV_CPSS_PORT_XG_E)
    {
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* check vctAction */
    switch (vctAction)
    {
        case CPSS_VCT_START_E:
        case CPSS_VCT_GET_RES_E:
        case CPSS_VCT_ABORT_E:
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    /* Allocate the memory in first call. */
    if (vctInfo == NULL)
    {
        vctInfo = (CPSS_VCT_INFO_STC*)cpssOsMalloc(sizeof(CPSS_VCT_INFO_STC) *
                                        (PRV_CPSS_PP_MAC(dev)->numOfPorts));
        if (vctInfo == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        for(i = 0; i < PRV_CPSS_PP_MAC(dev)->numOfPorts; i++)
        {
            vctInfo[i].phyType   = 0xffff;
            vctInfo[i].reg0Val   = 0;
            vctInfo[i].reg17Val  = 0;
            vctInfo[i].reg26Val  = 0;
            vctInfo[i].disableWa = GT_FALSE;
            vctInfo[i].vctState  = CPSS_VCT_READY_E;

        }
    }

    /* Handle the CPSS_VCT_ABORT_E action. */
    if (vctAction == CPSS_VCT_ABORT_E)
    {
        if(vctInfo[port].disableWa)
        {
            switch(vctInfo[port].phyType)
            {
            case CPSS_VCT_PHY_100M_E:

                if(vctInfo[port].vctState != CPSS_VCT_REDO_PHASE_2_E)
                {
                    status = prvDisableTdrFe1WorkAround(dev, port);
                }
                else
                {
                    status = prvDisableTdrFe1WorkAround(dev, port);
                    status = prvDisableTdrFe2WorkAround(dev, port);
                }
                break;
            case CPSS_VCT_PHY_1000M_E:
            case CPSS_VCT_PHY_1000M_B_E:
                status = prvDisableTdrGeWorkAround(dev, port);
                break;
            case CPSS_VCT_PHY_1000M_MP_E:
            default:
                break;
            }
        }

        if(vctInfo[port].phyType == CPSS_VCT_PHY_100M_E)
        {
            /* restore the original content of register 0 + software reset */
            if((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_CNTRL,
                            (GT_U16)(vctInfo[port].reg0Val | 0x8000))) != GT_OK)
                return status;
        }

        /* Test is aborted. Switch to READY state and reset DB.*/
        vctInfo[port].vctState  = CPSS_VCT_READY_E;
        vctInfo[port].reg0Val   = 0;
        vctInfo[port].reg17Val  = 0;
        vctInfo[port].reg26Val  = 0;
        vctInfo[port].phyType   = 0xffff;
        vctInfo[port].disableWa = GT_FALSE;

        /* Enable soft reset to the port */
        return prvPhySoftReset(dev, port);
    }

    switch(vctInfo[port].vctState)
    {
        case CPSS_VCT_READY_E:

            if(vctAction == CPSS_VCT_GET_RES_E)
              CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            /* Identify phy type and check if VCT is relevant */
            if ((status = prvDoesDeviceSupportVct(dev,
                                                  port,
                                                  &(vctInfo[port].phyType),
                                                  &phyIdRegValue))
                != GT_OK)
            {
                    return status;
            }

            /* reset PHY */
            prvPhySoftReset(dev, port);

            switch(vctInfo[port].phyType)
            {
                case CPSS_VCT_PHY_100M_E:
                    status = prvVctReady_Phy100M(dev, port);
                    break;
                case CPSS_VCT_PHY_1000M_E:
                    status = prvVctReady_Phy1000M(dev, port);
                    break;
                case CPSS_VCT_PHY_1000M_B_E:
                    prvSetTdrWorkAround(dev, port);
                    status = prvVctReady_Phy1000M(dev, port);
                    break;
                case CPSS_VCT_PHY_1000M_MP_E:
                case CPSS_VCT_PHY_1000M_MP_NO_FIBER_E:
                case CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E:
                    /* for multi-page phy disable PPU (AutoNeg) before running VCT */
                    /* check AutoNeg function was bound */
                    autoNegStatus = (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->
                             genVctBindFunc.cpssPhyAutoNegotiationGet == NULL)
                             ? GT_FALSE : GT_TRUE;

                    /* get AutoNeg status */
                    if(autoNegStatus)
                    {
                        status = PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->
                            genVctBindFunc.cpssPhyAutoNegotiationGet(dev, port,
                                                                        &autoNegStatus);
                    }
                    /* in case AutoNeg enable, disable it*/
                    if(autoNegStatus)
                    {

                       PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->genVctBindFunc.
                                         cpssPhyAutoNegotiationSet(dev, port, GT_FALSE);
                    }
                    /* select page 0 (cooper) */
                    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port,
                                                 PRV_CPSS_GE_PHY_EXT_ADDR, 0)) != GT_OK)
                        return status;

                    if(vctInfo[port].phyType == CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E)
                    {
                       status = prvVctReady_Phy1000MMP_NG(dev, port, cableStatusPtr);
                    }
                    else
                    {

                       status = prvVctReady_Phy1000MMP(dev, port, vctInfo[port].phyType,
                                                 cableStatusPtr);
                    }
                    if(autoNegStatus)
                    {
                        /* Restore AutoNeg status */
                        PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->genVctBindFunc.
                                          cpssPhyAutoNegotiationSet(dev, port, GT_TRUE);
                    }

                    break;

                default:
                    status = GT_FAIL;
                    break;
            }

            /* Set VCT state to STARTED */
            vctInfo[port].vctState  = CPSS_VCT_STARTED_E;
            vctInfo[port].disableWa = GT_TRUE;
            break;

        case CPSS_VCT_STARTED_E:

            if(vctAction == CPSS_VCT_START_E)
              CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            switch(vctInfo[port].phyType)
            {
                case CPSS_VCT_PHY_100M_E:
                    status = prvVctStarted_Phy100M(dev, port, CPSS_VCT_STARTED_E, cableStatusPtr);
                    break;
                case CPSS_VCT_PHY_1000M_E:
                case CPSS_VCT_PHY_1000M_B_E:
                    status = prvVctStarted_Phy1000M(dev, port, cableStatusPtr);
                    break;
                case CPSS_VCT_PHY_1000M_MP_E:
                case CPSS_VCT_PHY_1000M_MP_NO_FIBER_E:
                case CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E:
                    /* for multi-page phy disable PPU before running VCT */
                    /* for multi-page phy disable PPU (AutoNeg) before running VCT */
                    /* check AutoNeg function was bound */
                    autoNegStatus = (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->
                             genVctBindFunc.cpssPhyAutoNegotiationGet == NULL)
                             ? GT_FALSE : GT_TRUE;

                    /* get AutoNeg status */
                    if(autoNegStatus)
                    {
                        status = PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->
                            genVctBindFunc.cpssPhyAutoNegotiationGet(dev, port,
                                                                        &autoNegStatus);
                    }
                    /* in case AutoNeg enable, disable it*/
                    if(autoNegStatus)
                    {

                       PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->genVctBindFunc.
                                         cpssPhyAutoNegotiationSet(dev, port, GT_FALSE);
                    }
                    /* select page 0 (cooper) */
                    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port,
                                                 PRV_CPSS_GE_PHY_EXT_ADDR, 0)) != GT_OK)
                        return status;
                    if(vctInfo[port].phyType == CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E)
                    {
                        /* Get PHY id reg value  */
                        if ((status = prvDoesDeviceSupportVct(dev,
                                                              port,
                                                              NULL,
                                                              &phyIdRegValue))
                            != GT_OK)
                        {
                                return status;
                        }

                        status = prvVctStarted_Phy1000MMP_NG(dev, port,
                                                             phyIdRegValue, cableStatusPtr);
                    }
                    else
                    {

                       status = prvVctStarted_Phy1000MMP(dev, port, cableStatusPtr);
                    }

                    if(autoNegStatus)
                    {
                        /* Restore AutoNeg status */
                        PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->genVctBindFunc.
                                          cpssPhyAutoNegotiationSet(dev, port, GT_TRUE);
                    }

                    break;
                default:
                    status = GT_FAIL;
                    break;
            }

            break;

        case CPSS_VCT_REDO_PHASE_1_E:

            if(vctAction == CPSS_VCT_START_E)
              CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            switch(vctInfo[port].phyType)
            {
                case CPSS_VCT_PHY_100M_E:
                    status = prvVctStarted_Phy100M(dev, port, CPSS_VCT_REDO_PHASE_1_E, cableStatusPtr);
                    break;
                default:
                    status = GT_FAIL;
                    break;
            }

            break;

        case CPSS_VCT_REDO_PHASE_2_E:

            if(vctAction == CPSS_VCT_START_E)
              CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            switch(vctInfo[port].phyType)
            {
                case CPSS_VCT_PHY_100M_E:
                    status = prvVctRedoPhase2_Phy100M(dev,port,cableStatusPtr);
                    break;
                default:
                    status = GT_FAIL;
                    break;
            }

            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }


    if ((status == GT_OK) && (vctAction == CPSS_VCT_GET_RES_E))
    {
        /* Test has completed. Switch to READY state and reset DB.*/
        vctInfo[port].vctState  = CPSS_VCT_READY_E;
        vctInfo[port].reg0Val   = 0;
        vctInfo[port].reg17Val  = 0;
        vctInfo[port].reg26Val  = 0;
        vctInfo[port].phyType   = 0xffff;
        vctInfo[port].disableWa = GT_FALSE;

        /* Enable soft reset to the port */
        prvPhySoftReset(dev, port);

    }

    return status;
}

/*******************************************************************************
* cpssVctCableStatusGet
*
* DESCRIPTION:
*       This routine asserts / aborts the state-machine based virtual cable test
*       (VCT) for the requested port. When test completes, it returns the the
*       status per MDI pair, otherwize it will return current test status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       vctAction - The VCT action to perform on this port.
*
* OUTPUTS:
*       cableStatusPtr - virtual cable diagnostic status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_READY     - when action is CPSS_VCT_GET_RES_E and the test is not
*                          completed yet.
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Scenario:
*       1)First call with CPSS_VCT_START_E to begin the test
*       2)Call the function with CPSS_VCT_GET_RES_E  and wait till GT_OK as
*         relult.
*         If GT_NOT_READY is returned, call the CPSS_VCT_GET_RES_E again after a
*         while in order to let the test be completed.
*
*******************************************************************************/
GT_STATUS cpssVctCableStatusGet
(
    IN  GT_U8                     dev,
    IN  GT_PHYSICAL_PORT_NUM      port,
    IN  CPSS_VCT_ACTION_ENT       vctAction,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssVctCableStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, port, vctAction, cableStatusPtr));

    rc = internal_cpssVctCableStatusGet(dev, port, vctAction, cableStatusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, port, vctAction, cableStatusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvSetTdrWorkAround
*
* DESCRIPTION:
*       Configures TDR test workaround on specified port/device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvSetTdrWorkAround
(
    IN GT_U8 dev,
    IN GT_PHYSICAL_PORT_NUM port
)
{
    GT_BOOL status;

    status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29, 0x001e);
    if(status != GT_OK)
    {
        return status;
    }
    cpssOsTimerWkAfter(1);

    status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0xcc00);
    if(status != GT_OK)
    {
        return status;
    }
    cpssOsTimerWkAfter(1);

    status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0xc800);
    if(status != GT_OK)
    {
        return status;
    }
    cpssOsTimerWkAfter(1);

    status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0xc400);
    if(status != GT_OK)
    {
        return status;
    }
    cpssOsTimerWkAfter(1);

    status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0xc000);
    if(status != GT_OK)
    {
        return status;
    }
    cpssOsTimerWkAfter(1);

    status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0xc100);
    if(status != GT_OK)
    {
        return status;
    }
    cpssOsTimerWkAfter(1);

    return GT_OK;
}

/*******************************************************************************
* prvPhySoftReset
*
* DESCRIPTION:
*       Reset specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvPhySoftReset
(
    IN GT_U8 dev,
    IN GT_PHYSICAL_PORT_NUM port
)
{
    GT_U16  value;
    GT_BOOL status;

    status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_CNTRL, &value);
    if (status != GT_OK)
    {
        return status;
    }

    value |= 0x8000;

    status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_CNTRL, value);
    if(status != GT_OK)
    {
        return status;
    }

    do
    {
        status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_CNTRL, &value);
        if(status != GT_OK)
        {
            return status;
        }
#if defined (ASIC_SIMULATION)
        value &= (~ 0x8000) ;
#endif
    } while (value & 0x8000);

    return GT_OK;
}

/*******************************************************************************
* prvEnableTdrFe1WorkAround
*
* DESCRIPTION:
*       Enable 1st work around for FE PHYs before enabling TDR test
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Works on 10/100 FE interfaces.
*
*******************************************************************************/
static GT_STATUS  prvEnableTdrFe1WorkAround
(
    IN GT_U8 dev,
    IN GT_PHYSICAL_PORT_NUM port
)
{
    GT_U16      value;
    GT_STATUS   status;


    /* enable 1st work-around */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29, 3))
                                                    != GT_OK)
       return status;

    value = 0x6440;
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30,
                                                               value)) != GT_OK)
       return status;

    return GT_OK;
}
/*******************************************************************************
* disableTdrFe1WorkAround
*
* DESCRIPTION:
*       Disable 1st work around for FE PHYs after TDR test end
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Works on 10/100 FE interfaces.
*
*******************************************************************************/
static GT_STATUS  prvDisableTdrFe1WorkAround
(
    IN GT_U8 dev,
    IN GT_PHYSICAL_PORT_NUM port
)
{
    GT_STATUS status;


    /* disable 1st work-around */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29, 3))
                                                                       != GT_OK)
       return status;

    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0))
                                                                       != GT_OK)
       return status;

    return GT_OK;
}
/*******************************************************************************
* prvEnableTdrFe2WorkAround
*
* DESCRIPTION:
*       Enable 2nd work around for FE PHYs before enabling TDR test
*       needed just for when run TDR again if status of the test was short cable
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Works on 10/100 FE interfaces.
*
*******************************************************************************/
static GT_STATUS  prvEnableTdrFe2WorkAround
(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port
)
{
    GT_U16      value;
    GT_STATUS   status;


    /* enable 2nd work-around */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29, 0xA))
                                                                       != GT_OK)
       return status;

    value = 0x0002;
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30,
                                                               value)) != GT_OK)
       return status;

    return GT_OK;
}
/*******************************************************************************
* prvDisableTdrFe2WorkAround
*
* DESCRIPTION:
*       Disable 2nd work around for FE PHYs after TDR test end
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Works on 10/100 FE interfaces.
*
*******************************************************************************/
static GT_STATUS  prvDisableTdrFe2WorkAround(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port
)
{
    GT_STATUS status;


    /* disable 2nd work-around */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29, 0xA)) != GT_OK)
       return status;

    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0)) != GT_OK)
       return status;

    return GT_OK;
}
/*******************************************************************************
* prvEnableTdrGeWorkAround
*
* DESCRIPTION:
*       Enable work around for GE PHYs before enabling TDR test
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Works on 1G interfaces.
*
*******************************************************************************/
static GT_STATUS  prvEnableTdrGeWorkAround
(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port
)
{
    GT_STATUS   status;


    /* enable work-around */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29, 0x0018)) != GT_OK)
       return status;

    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0x00c2)) != GT_OK)
       return status;

    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0x00ca)) != GT_OK)
       return status;

    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0x00c2)) != GT_OK)
       return status;

    return GT_OK;
}
/*******************************************************************************
* prvDisableTdrGeWorkAround
*
* DESCRIPTION:
*       Disable work around for GE PHYs after TDR test end
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Works 1G interfaces.
*
*******************************************************************************/
static GT_STATUS  prvDisableTdrGeWorkAround(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port
)
{
    GT_STATUS status;


    /* disable work-around */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29, 0x0018)) !=
                                                                          GT_OK)
       return status;

    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG30, 0x0042)) != GT_OK)
       return status;

    return GT_OK;
}
/*******************************************************************************
* prvGetErrGeCabLen
*
* DESCRIPTION:
*       Convert hw value of GE PHY cabel fault distance to length in meter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       dist   - hw value of cabel fault distance
*
* OUTPUTS:
*       lengthPtr  - error cable length in meter
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       y = 0.8081x - 28.751
*
*******************************************************************************/
static GT_STATUS prvGetErrGeCabLen
(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port,
    IN  GT_U8 dist,
    OUT GT_U8 *lengthPtr
)
{
    GT_U32 mm;
    GT_32 offset;
    GT_STATUS   rc;

    rc = cpssVctLengthOffsetGet(dev, port, &offset);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((802 * dist) <= (28751 - offset))
    {
        mm = 0;
    }
    else
    {
        mm = (GT_U32) (802 * dist - 28751 + offset)/1000;
    }

    *lengthPtr = (GT_U8)mm;

    return GT_OK;
}
/*******************************************************************************
* prvGetErrGeCabLenNG
*
* DESCRIPTION:
*       Convert hw value of GE PHY cabel fault distance to length in meters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      dev    - physical device number
*      port   - physical port number
*      dist   - hw value of cabel fault distance
*      phyIdRegValue   - PHY ID register's value
*
* OUTPUTS:
*       lengthPtr  - error cable length in meter
*
* RETURNS:
*       None
*
* COMMENTS:
*   Calculation formula : y = 0.8049*X - 23 + PHY_type_specific_offset
*   Contact Marvell PHY representatives about value of PHY_type_specific_offset
*******************************************************************************/
static GT_STATUS prvGetErrGeCabLenNG
(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port,
    IN  GT_U32 dist,
    IN  GT_U16 phyIdRegValue,
    OUT GT_U8 *lengthPtr
)
{
    GT_32 mm; /* actual distance in meters, may be negative value */
    GT_32 offset;
    GT_STATUS   rc;

    rc = cpssVctLengthOffsetGet(dev, port, &offset);
    if (rc != GT_OK)
    {
        *lengthPtr = 0;
        return rc;
    }

    if((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1149R)
    {
        /* The PHY specific offset for 1149R is 6 */
        if ((8049 * dist) <= (GT_U32)(230000 - 60000 - offset))
        {
            mm = 0;
        }
        else
        {
            mm = (8049 * dist - 230000 + 60000 + offset)/10000;
        }
    }
    else if((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1680M){/* PHYs 88E1680M & 88E1680L have the same device version 0x2E */
        mm = (8049 * dist + offset)/10000;
    }
    else
    {
        /* The PHY specific offset for 1240 is 5.5 */
        if ((8049 * dist) <= (GT_U32)(230000 - 55000 - offset))
        {
            mm = 0;
        }
        else
        {
            mm = (8049 * dist - 230000 + 55000 + offset)/10000;
        }
    }

    *lengthPtr = (GT_U8)mm;

    return GT_OK;
}


/*******************************************************************************
* prvGetErrFeCabLen
*
* DESCRIPTION:
*       Convert hw value of FE PHY cabel fault distance to length in meter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       dist   - hw value of cabel fault distance
*
* OUTPUTS:
*       lengthPtr  - error cable length in meter
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       y = 0.7816x - 18.261
*
*******************************************************************************/
static GT_STATUS prvGetErrFeCabLen
(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port,
    IN  GT_U8 dist,
    OUT GT_U8 *lengthPtr
)
{
    GT_U32 mm;
    GT_32 offset;
    GT_STATUS   rc;

    rc = cpssVctLengthOffsetGet(dev, port, &offset);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((782 * dist) <= (18261 - offset))
    {
        mm = 0;
    }
    else
    {
        mm = (GT_U32) (782 * dist - 18261 + offset)/1000;
    }

    *lengthPtr = (GT_U8)mm;

    return GT_OK;
}

/*******************************************************************************
* prvDoesDeviceSupportVCT
*
* DESCRIPTION:
*       This routine check if the given device supports VCT.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - Device Number.
*       port   - Port Number.
*
* OUTPUTS:
*       phyTypePtr       - (pointer to) PHY type, may be NULL
*       phyIdRegValuePtr - (pointer to) PHY ID register value
*
* RETURNS:
*       GT_OK               - if the device supports VCT
*       GT_NOT_SUPPORTED    - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvDoesDeviceSupportVct
(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port,
    OUT CPSS_VCT_PHY_TYPES_ENT  *phyTypePtr,
    OUT GT_U16                  *phyIdRegValuePtr
)
{
    GT_STATUS status;
    GT_U16    value;


    /* read Marvel OUI */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_ID0, &value)) != GT_OK)
    {
        return status;
    }
    if (value != PRV_CPSS_MARVELL_OUI_MSB)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* read model number and revision */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_ID1, &value)) != GT_OK)
    {
        return status;
    }

    *phyIdRegValuePtr = value;

    if(phyTypePtr == NULL)
    {
        /* caller is not interesting in PHY type */
        return GT_OK;
    }

    *phyTypePtr = CPSS_VCT_PHY_1000M_E;
    status = GT_OK;

    switch(value & PRV_CPSS_PHY_MODEL_MASK)
    {
        case PRV_CPSS_DEV_E3082:
            *phyTypePtr = CPSS_VCT_PHY_100M_E;
            break;
        case PRV_CPSS_DEV_E104X:
            if ((value & PRV_CPSS_PHY_REV_MASK) < 0x3)
            {
                status = GT_NOT_SUPPORTED;
            }
            if ((value & PRV_CPSS_PHY_REV_MASK) == 0x3)
            {
                *phyTypePtr = CPSS_VCT_PHY_1000M_B_E;
            }
            break;
        case PRV_CPSS_DEV_E1112:
            *phyTypePtr = CPSS_VCT_PHY_1000M_MP_E;
            break;
        case PRV_CPSS_DEV_E1149:
            if ((value & PRV_CPSS_PHY_REV_MASK) >= 0x4)
            {
                /* C1 revision of E1149 */
                *phyTypePtr = CPSS_VCT_PHY_1000M_MP_NO_FIBER_E;
            }
            else if((value & PRV_CPSS_PHY_REV_MASK) == 0)
            {
                /* S0 revision of E1149R */
                *phyTypePtr = CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E;
            }
            else
            {
                /* B0, B1, C0 revisions of E1149 */
                *phyTypePtr = CPSS_VCT_PHY_1000M_MP_E;
            }
            break;
        case PRV_CPSS_DEV_E1149R:
        case PRV_CPSS_DEV_E1240:
        case PRV_CPSS_DEV_E1340:
        case PRV_CPSS_DEV_E1340_X0:
        case PRV_CPSS_DEV_E1540:
        case PRV_CPSS_DEV_E1543:
        case PRV_CPSS_DEV_E1680:
        case PRV_CPSS_DEV_E1680M:
        case PRV_CPSS_DEV_E1548P:
            *phyTypePtr = CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E;
            break;

        case PRV_CPSS_DEV_E1111:
        case PRV_CPSS_DEV_E114X:
        case PRV_CPSS_DEV_E1180:
            break;
        default:
            status = GT_NOT_SUPPORTED;
            break;
    }

    return status;
}

/*******************************************************************************
* prvGetExtendedCableStatus_Phy100M
*
* DESCRIPTION:
*       This routine perform the extended virtual cable test for the 10/100Mbps
*       phy, and returns the status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev  - device number
*       port - logical port number.
*
* OUTPUTS:
*       extendedCableStatusPtr - the port extended vct cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvGetExtendedCableStatus_Phy100M
(
    IN  GT_U8                       dev,
    IN  GT_PHYSICAL_PORT_NUM        port,
    OUT CPSS_VCT_CABLE_EXTENDED_STATUS_STC    *extendedCableStatusPtr
)
{
    GT_STATUS   status;
    GT_U16      reg17Val;


    /* read reg.17 value */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_SPEC_STATUS,
                                            &reg17Val)) != GT_OK)
        return status;

    /* check if link is on */
    if( (reg17Val & 0x0400) != 0x0400)
    {
        return GT_OK;
    }

    /* display extended statuses */
    extendedCableStatusPtr->vctExtendedCableStatus.isValid = GT_TRUE;

    /* display pair polarity, bit 1 */
    if( !(reg17Val & 0x02) )
        extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[0] =
                                                   CPSS_VCT_POSITIVE_POLARITY_E;
    else
        extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[0] =
                                                   CPSS_VCT_NEGATIVE_POLARITY_E;

    /* check if resolved bit is on */
    if( (reg17Val & 0x0F00) != 0x0F00)
    {
        return GT_OK;
    }

    /* display pair swap, bit 6 */
    if( reg17Val & 0x40 )
        extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[0] =
                                                     CPSS_VCT_CABLE_CROSSOVER_E;
    else
        extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[0] =
                                                     CPSS_VCT_CABLE_STRAIGHT_E;

    return GT_OK;
}

/*******************************************************************************
* prvGetExtendedCableStatus_Phy1000M
*
* DESCRIPTION:
*       This routine perform the extended virtual cable test for the 1000Mbps
*       phy, and returns the status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev  - device number
*       port - logical port number.
*
* OUTPUTS:
*       extendedCableStatusPtr - the port extended vct cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvGetExtendedCableStatus_Phy1000M
(
    IN  GT_U8                       dev,
    IN  GT_PHYSICAL_PORT_NUM        port,
    OUT CPSS_VCT_CABLE_EXTENDED_STATUS_STC    *extendedCableStatusPtr
)
{
    GT_STATUS   status;
    GT_U16      reg17Val;
    GT_U16      value;
    GT_U16      reg29Val;
    GT_U32      i;


    /* select page 0 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR, 0))
                                                                    != GT_OK)
        return status;

    /* read reg.17 value */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_SPEC_STATUS,
                                            &reg17Val)) != GT_OK)
        return status;

    /* check if link is on */
    if( !(reg17Val & 0x0400))
        return GT_OK;

    /* display downshift status */
    switch(reg17Val & 0x0020)
    {
        case 1:
            extendedCableStatusPtr->twoPairDownShift = CPSS_VCT_DOWNSHIFT_E;
            break;
        case 0:
            extendedCableStatusPtr->twoPairDownShift = CPSS_VCT_NO_DOWNSHIFT_E;
            break;
        default:
            ;
    }

    /* check if gigabit link is on */
    if( (reg17Val & 0xCC00) != 0x8C00)
        return GT_OK;

    /* display accurate cable length */
    reg29Val = 0x8754;
    for( i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
    {
        /* select pair */
        if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29,
                                                            reg29Val)) != GT_OK)
            return status;
        cpssOsTimerWkAfter(1);

        /* Read the accurate cable length */
        if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_REG31,
                                                             &value)) != GT_OK)
            return status;

        if( value > 97)
        {
            extendedCableStatusPtr->accurateCableLen.cableLen[i] = 135;
        }
        else
        {
            extendedCableStatusPtr->accurateCableLen.isValid[i] = GT_TRUE;
            extendedCableStatusPtr->accurateCableLen.cableLen[i] =
                                            accurateCableLenArr[value];
        }

        reg29Val += 0x1000;
    }

    /* check if valid bit on */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR,
                                            0x05)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_GE_PHY_CABLE_DIAG,
                                            &value)) != GT_OK)
            return status;

    /* if bit not valid don't do anything */
    if( !(value & 0x0040))
    {
        /* return page register to page 0 */
        status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR, 0);
        return status;
    }

    /* display extended statuses */
    extendedCableStatusPtr->vctExtendedCableStatus.isValid = GT_TRUE;

    /* read data from 28_page5 */
    /* display pair polarity, bits 0,1,2,3 */
    for( i=0; i<CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
    {
        if( !((value >> i) & 0x01) )
            extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[i] =
                                                   CPSS_VCT_POSITIVE_POLARITY_E;
        else
            extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[i] =
                                                   CPSS_VCT_NEGATIVE_POLARITY_E;
    }

    /* display pair swap, bits 4,5 */
    for( i=0; i<CPSS_VCT_CHANNEL_PAIR_NUM_CNS; i++)
    {
        if( !((value >> (i+4)) & 0x01) )
            extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[i] =
                                                     CPSS_VCT_CABLE_STRAIGHT_E;
        else
            extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[i] =
                                                     CPSS_VCT_CABLE_CROSSOVER_E;
    }

    /* read data from 28_page4 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR,
                                            0x04)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_GE_PHY_CABLE_DIAG,
                                            &value)) != GT_OK)
            return status;


    /* display pair skew */
    extendedCableStatusPtr->vctExtendedCableStatus.pairSkew.isValid = GT_TRUE;

    for( i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
        extendedCableStatusPtr->vctExtendedCableStatus.pairSkew.skew[i] =
                                                (((value >> (i*4)) & 0x0F) * 8);

    /* return page register to page 0 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR, 0))
                                                                    != GT_OK)
        return status;

    return GT_OK;
}

/*******************************************************************************
* prvGetExtendedCableStatus_Phy1000MMP_NG
*
* DESCRIPTION:
*       This routine perform the extended virtual cable test for the 1000Mbps
*       phy, E1149R, E1240 and alike,
*       and returns the status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev  - device number
*       port - logical port number.
*       phyIdRegValue - value of PHY ID register
*
* OUTPUTS:
*       extendedCableStatusPtr - the port extended vct cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvGetExtendedCableStatus_Phy1000MMP_NG
(
    IN  GT_U8                       dev,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  GT_U16                      phyIdRegValue,
    OUT CPSS_VCT_CABLE_EXTENDED_STATUS_STC    *extendedCableStatusPtr
)
{
    GT_STATUS   status;       /* return code */
    GT_U16      value;        /* temporary data */
    GT_U32      i,j,k;        /* loop iterators */
    GT_U16      minValue;     /* minimum value read from reg21 */
    GT_U16      maxValue;     /* maximum value read from reg21 */
    GT_U32      offset = 0;
    GT_U32      lengthSum;    /* sum of all lengths of same pair */
    GT_U8       startVctRegAddr; /* register's address to start
                                    retrieving of cable length */
    GT_U8       lengthRegAddr; /* register's address to get cable length */
    GT_U16      startVctValue; /* register's value to start
                                    retrieving of cable length */


    /* read reg.17 value */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x0)) != GT_OK)
            return status;

    cpssOsTimerWkAfter(1);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 17, &value)) != GT_OK)
        return status;

    /* check if link is on */
    if( !(value & 0x0400))
        return GT_OK;

    /* display downshift status */
    switch(value & 0x0020)
    {
        case 1:
            extendedCableStatusPtr->twoPairDownShift = CPSS_VCT_DOWNSHIFT_E;
            break;
        case 0:
            extendedCableStatusPtr->twoPairDownShift = CPSS_VCT_NO_DOWNSHIFT_E;
            break;
        default:
            ;
    }

    /* check if gigabit link is on */
    if( (value & 0xCC00) != 0x8C00)
        return GT_OK;

   /* get registers addresses and control value */
    switch(phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK)
    {   case PRV_CPSS_DEV_E1149R:
            startVctRegAddr = 19;
            lengthRegAddr   = 21;
            startVctValue   = 0x1018;
            break;
        case PRV_CPSS_DEV_E1240:
        case PRV_CPSS_DEV_E1340:
        case PRV_CPSS_DEV_E1340_X0:
        case PRV_CPSS_DEV_E1540:
        case PRV_CPSS_DEV_E1543:
        case PRV_CPSS_DEV_E1680:
        case PRV_CPSS_DEV_E1680M:
        case PRV_CPSS_DEV_E1548P:
            startVctRegAddr = 16;
            lengthRegAddr   = 18;
            startVctValue   = 0x1118;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* Procedure to get cable length utilising look up table */
    for(j = 0; j < CPSS_VCT_MDI_PAIR_NUM_CNS; j++)
    {
        /* reset min/max values for each pair */
        maxValue  = 0;
        minValue  = 0xFFFF;
        lengthSum = 0;

        /* Read each pair's results at least 10 times.
           As workaround to avoid deviation, highest and lowest readingns should
           be thrown */
        for(k = 0; k < 10; k++)
        {
            status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x00FF);
            if (status != GT_OK)
                 return status;

            status = PRV_PHY_REG_WRITE_MAC(dev, port,
                                           startVctRegAddr,
                                           (GT_U16)(startVctValue + j));
            if (status != GT_OK)
                 return status;

             /* keep polling reg19.15 until bit is set to 1 */
             for (i = 0; i < VCT_RETRY_COUNTER_CNS; i++)
             {
                 status = PRV_PHY_REG_READ_MAC(dev, port,
                                               startVctRegAddr, &value);
                 if(status != GT_OK)
                     return status;

             #if defined (ASIC_SIMULATION)
                      value |= 0x8000;
             #endif
                  if ((value & 0x8000))
                    break;

                  cpssOsTimerWkAfter(1);
             }
             /* read length of relevant pair */
             status = PRV_PHY_REG_READ_MAC(dev, port, lengthRegAddr, &value);
             if (status != GT_OK)
                 return status;

             lengthSum += value;

             /* save min and max to throw it and avoid deviation */
             if(value < minValue)
             {
                minValue = value;
             }

             if(value > maxValue)
             {
                maxValue = value;
             }

        }/* loop per each pair */

        /* remove deviations and get average */
        lengthSum -= (maxValue + minValue);
        lengthSum /= 8;

        /* update statuses */
        extendedCableStatusPtr->accurateCableLen.isValid[j] = GT_TRUE;

        /* convert HW value */
        if((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1149R)
        {
            /* 1149r */
            if(lengthSum > 76)
            {
                extendedCableStatusPtr->accurateCableLen.cableLen[j] = 130;
            }
            else
                extendedCableStatusPtr->accurateCableLen.cableLen[j] =
                                              accurateCableLenArr1149r[lengthSum];

        }
        else if((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240)
        {
            /* 1240 */
            if(lengthSum > 93)
            {
                extendedCableStatusPtr->accurateCableLen.cableLen[j] = 130;
            }
            else
                extendedCableStatusPtr->accurateCableLen.cableLen[j] =
                                              accurateCableLenArr1240[lengthSum];
        }
        else /* (phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340/PRV_CPSS_DEV_E154X */
        {
            /* 1543, 1545 or 1680/1685 */
            if( ((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1543) ||
                ((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1680) ||
                ((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1680M))
           {
                /* set page 0xFC */
                status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x00FC);
                if (status != GT_OK)
                    return status;

                /* Read the divider/multiplier coefficient for the DSP-based VCT */

                status = PRV_PHY_REG_READ_MAC(dev, port, 23, &value);
                if (status != GT_OK)
                    return status;

                DBG_VCT_MAC(("\n value for 1543/1545, reg 23 = 0x%x \n", value));

                switch(value & 0x3)
                {
                    case 0: /* divide result by 2.8 */
                            /* and round up/down to the nearest decimal value */
                            DBG_VCT_MAC(("\n divide result by 2.8"));
                            lengthSum = (GT_U32)((lengthSum * 10 + 14)/28);
                            break;
                    case 2:
                    case 3: /* multiple result by 1.2 */
                            /* and round up/down to the nearest decimal value */
                            DBG_VCT_MAC(("\n multiple result by 1.2"));
                            lengthSum = (GT_U32)((lengthSum * 12 + 5)/10);
                            break;
                    default: /* no change */
                             DBG_VCT_MAC(("\n no change"));
                             break;
                }

				i = 0xFFFF; /* to prevent VC10: "warning C4701: potentially 
								uninitialized local variable 'i' used" later  */
                if ((phyIdRegValue & PRV_CPSS_PHY_REV_MASK) >= 0x1) { /* phy 1680L Rev A0 */
                    DBG_VCT_MAC(("\n phy 1680L Rev A0\n"));
                    /* values for gain 0-2 from 1680L gain-based DSP VCT Table doc */
                    switch(value & 0x3) {
                    case 0:
                        offset = 110;
                        maxValue = 63;
                        i = 0;
                        break;
                    case 1:
                        offset = 81;
                        maxValue = 130;
                        i = 1;
                        break;
                    case 2:
                        offset = 69;
                        maxValue = 130;
                        i = 2;
                        break;
                    default: /* no change */
                        extendedCableStatusPtr->accurateCableLen.isValid[j] = GT_FALSE;
                        DBG_VCT_MAC(("\n isValid = FALSE"));
                        DBG_VCT_MAC(("\n \n"));
                    }
                }

                if (extendedCableStatusPtr->accurateCableLen.isValid[j] == GT_TRUE) {
                    if (lengthSum > offset) {
                        DBG_VCT_MAC(("\n offset=%d, maxValue=%d", offset, maxValue));
                        extendedCableStatusPtr->accurateCableLen.cableLen[j] = maxValue;
                        DBG_VCT_MAC(("\n cable length set by max value to %d meter", extendedCableStatusPtr->accurateCableLen.cableLen[j]));
                    }
                    else  {
                        /* if PHY revision is A0 */
                        if (((phyIdRegValue & PRV_CPSS_PHY_REV_MASK) >= 0x1) && (i != 0xFFFF)) {
                            DBG_VCT_MAC(("\n A0. length =%d meters,  table exchange on phy 1680L rev A0, cableLen = %d", lengthSum, accurateCableLenArr1680LRevA0[i][lengthSum]));
                            extendedCableStatusPtr->accurateCableLen.cableLen[j] = accurateCableLenArr1680LRevA0[i][lengthSum];
                            DBG_VCT_MAC(("\n cable length set by table to %d meter", extendedCableStatusPtr->accurateCableLen.cableLen[j]));
                        }
                    }
                }
            }
            else{
              /* 1340 */
                DBG_VCT_MAC(("\n lengthSum table exchange: %d\n", lengthSum));

                if(lengthSum > 88)
                {
                    extendedCableStatusPtr->accurateCableLen.cableLen[j] = 130;
                }
                else
                    extendedCableStatusPtr->accurateCableLen.cableLen[j] =
                    accurateCableLenArr1340[lengthSum];
            }
        }

    }/* loop on pairs */
    /* check if valid bit on */
    /* select page 5 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x05)) != GT_OK)
            return status;

    cpssOsTimerWkAfter(1);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 21, &value)) != GT_OK)
            return status;

    /* if bit not valid don't do anything */
    if( !(value & 0x0040))
    {
        /* return page register to page 0 */
        status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x0);
        cpssOsTimerWkAfter(1);
        return status;
    }

    /* display extended statuses */
    extendedCableStatusPtr->vctExtendedCableStatus.isValid = GT_TRUE;

    /* read data from 21_page5 */
    /* display pair polarity, bits 0,1,2,3 */
    for(i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
    {
        if( !((value >> i) & 0x01) )
            extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[i] =
                                                           CPSS_VCT_POSITIVE_POLARITY_E;
        else
            extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[i] =
                                                           CPSS_VCT_NEGATIVE_POLARITY_E;
    }

    /* display pair swap, bits 4,5 */
    for(i = 0; i < CPSS_VCT_CHANNEL_PAIR_NUM_CNS; i++)
    {
        if( !((value >> (i+4)) & 0x01) )
            extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[i] =
                                                            CPSS_VCT_CABLE_STRAIGHT_E;
        else
            extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[i] =
                                                            CPSS_VCT_CABLE_CROSSOVER_E;
    }

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 20, &value)) != GT_OK)
            return status;

    /* display pair skew */
    extendedCableStatusPtr->vctExtendedCableStatus.pairSkew.isValid = GT_TRUE;

    for( i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
        extendedCableStatusPtr->vctExtendedCableStatus.pairSkew.skew[i] =
                                                (((value >> (i*4)) & 0x0F) * 8);

    /* return to page 0 */
    status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x00);

    return status;
}

/*******************************************************************************
* prvGetExtendedCableStatus_Phy1000MMP
*
* DESCRIPTION:
*       This routine perform the extended virtual cable test for the 1000Mbps
*       phy, E1149, E1112 and alike,
*       and returns the status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev  - device number
*       port - logical port number.
*       phyType - phy type
*
* OUTPUTS:
*       extendedCableStatusPtr - the port extended vct cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvGetExtendedCableStatus_Phy1000MMP
(
    IN  GT_U8                       dev,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_VCT_PHY_TYPES_ENT                phyType,
    OUT CPSS_VCT_CABLE_EXTENDED_STATUS_STC    *extendedCableStatusPtr
)
{
    GT_STATUS   status;
    GT_U16      reg17Val;
    GT_U16      value;
    GT_U16      reg29Val;
    GT_U32      i;


    /* If Fiber is used, simply return. */
    if (phyType != CPSS_VCT_PHY_1000M_MP_NO_FIBER_E)
    {

        if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x1)) != GT_OK)
            return status;
        cpssOsTimerWkAfter(1);

        if ((status = PRV_PHY_REG_READ_MAC(dev, port, 17, &value)) !=
            GT_OK)
                return status;

        if(value & 0x400)
            return GT_OK;
    }

    /* read reg.17 value */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x0)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 17, &reg17Val)) != GT_OK)
        return status;

    /* check if link is on */
    if( !(reg17Val & 0x0400))
        return GT_OK;

    /* display downshift status */
    switch(reg17Val & 0x0020)
    {
        case 1:
            extendedCableStatusPtr->twoPairDownShift = CPSS_VCT_DOWNSHIFT_E;
            break;
        case 0:
            extendedCableStatusPtr->twoPairDownShift = CPSS_VCT_NO_DOWNSHIFT_E;
            break;
        default:
            ;
    }

    /* check if gigabit link is on */
    if( (reg17Val & 0xCC00) != 0x8C00)
        return GT_OK;

    /* display accurate cable length */
    reg29Val = 0x8754;
    for( i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
    {
        /* select pair */
        if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_REG29,
                                                            reg29Val)) != GT_OK)
            return status;
        cpssOsTimerWkAfter(1);

        /* Read the accurate cable length */
        if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_REG31,
                                                             &value)) != GT_OK)
            return status;

        if( value > 97)
        {
            extendedCableStatusPtr->accurateCableLen.cableLen[i] = 135;
        }
        else
        {
            extendedCableStatusPtr->accurateCableLen.isValid[i] = GT_TRUE;
            extendedCableStatusPtr->accurateCableLen.cableLen[i] =
                                                    accurateCableLenArr[value];
        }

        reg29Val += 0x1000;
    }

    /* check if valid bit on */
    /* select page 5 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x05)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 21, &value)) != GT_OK)
            return status;

    /* if bit not valid don't do anything */
    if( !(value & 0x0040))
    {
        /* return page register to page 0 */
        status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x0);
        cpssOsTimerWkAfter(1);
        return status;
    }

    /* display extended statuses */
    extendedCableStatusPtr->vctExtendedCableStatus.isValid = GT_TRUE;

    /* read data from 21_page5 */
    /* display pair polarity, bits 0,1,2,3 */
    for( i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
    {
        if( !((value >> i) & 0x01) )
            extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[i] =
                                                   CPSS_VCT_POSITIVE_POLARITY_E;
        else
            extendedCableStatusPtr->vctExtendedCableStatus.pairPolarity[i] =
                                                   CPSS_VCT_NEGATIVE_POLARITY_E;
    }

    /* display pair swap, bits 4,5 */
    for( i = 0; i < CPSS_VCT_CHANNEL_PAIR_NUM_CNS; i++)
    {
        if( !((value >> (i+4)) & 0x01) )
            extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[i] =
                                                   CPSS_VCT_CABLE_STRAIGHT_E;
        else
            extendedCableStatusPtr->vctExtendedCableStatus.pairSwap[i] =
                                                   CPSS_VCT_CABLE_CROSSOVER_E;
    }

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 20, &value)) != GT_OK)
            return status;

    /* display pair skew */
    extendedCableStatusPtr->vctExtendedCableStatus.pairSkew.isValid = GT_TRUE;

    for( i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
        extendedCableStatusPtr->vctExtendedCableStatus.pairSkew.skew[i] =
                                                (((value >> (i*4)) & 0x0F) * 8);

    /* return to page 0 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x00)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    return GT_OK;
}

/*******************************************************************************
* prvVctReady_Phy100M
*
* DESCRIPTION:
*       This routine asserts the first state of virtual cable test for the
*       10/100Mbps phy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev  - device number.
*       port - logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctReady_Phy100M
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM  port
)
{
    GT_U16      value;
    GT_STATUS   status;

    /* Save Reg 0 content. It is changed during the test and must be reset */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_CNTRL,
                                &(vctInfo[port].reg0Val))) != GT_OK)
        return status;

    /* start TDR test on FE PHY */
    if ((status = prvEnableTdrFe1WorkAround(dev, port)) != GT_OK)
        return status;

    /* set the Phy to be in 100Mb full duplex mode and reset */
    value = 0xa100;
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_CNTRL, value)) != GT_OK)
       return status;

    /* enable test - Write 1 to bit 15 in PRV_CPSS_FE_PHY_CABLE_DIAG1 */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_FE_PHY_CABLE_DIAG1, &value))
                                                != GT_OK)
        return status;

    value |= 0x8000;
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_FE_PHY_CABLE_DIAG1, value))
                                                != GT_OK)
        return status;

    return GT_OK;
}

/*******************************************************************************
* cpssVctReady_Phy100M
*
* DESCRIPTION:
*       This routine asserts the first state of virtual cable test for the
*       1000Mbps phy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev  - device number.
*       port - logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctReady_Phy1000M
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM  port
)
{
    GT_U16      value;
    GT_STATUS   status;


    /* select page 0 (cooper) */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR, 0)) != GT_OK)
        return status;

    /* save original reg. 17 value, which will be used later depending on
     * test result. */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_PHY_SPEC_STATUS,
                                    &(vctInfo[port].reg17Val))) != GT_OK)
        return status;

    /* start TDR test on GE PHY */
    if ((status = prvEnableTdrGeWorkAround(dev, port)) != GT_OK)
        return status;

    /* Set page to 0 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0)) != GT_OK)
        return status;

    cpssOsTimerWkAfter(1);

    /* enable test */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_GE_PHY_CABLE_DIAG,
                                            &value)) != GT_OK)
        return status;

    value |= 0x8000;
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_CABLE_DIAG,
                                            value)) != GT_OK)
        return status;

    return GT_OK;
}


/*******************************************************************************
* prvVctStarted_Phy100M
*
* DESCRIPTION:
*       This routine asserts the second state of virtual cable test for the
*       10/100Mbps phy. If succeded, it returns the returns the the status per
*       Rx/Tx pair. Otherwise, it restarts the test.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number.
*       port   - logical port number.
*       action - to be used by both CPSS_VCT_STARTED_E and
*                CPSS_VCT_REDO_PHASE_1_E
*
* OUTPUTS:
*       cableStatusPtr - the port copper cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctStarted_Phy100M
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  CPSS_VCT_STATE  action,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{
    GT_U8       i;
    GT_U8       dist;
    GT_U32      testRes;
    GT_U16      value;
    GT_U16      value1;
    GT_U16      value2;
    GT_BOOL     vctSuccess;
    GT_STATUS   status;


    vctSuccess = GT_FALSE;

    if((action != CPSS_VCT_STARTED_E) && (action != CPSS_VCT_REDO_PHASE_1_E))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_FE_PHY_CABLE_DIAG1,&value))
                                                                       != GT_OK)
        return status;

#if defined (ASIC_SIMULATION)
          value &= (~ 0x8000);
#endif

    /* Check if Previous test is finished. */
    if ((value & 0x8000) != 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
    }

    /* restore the original content of register 0 + software reset */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_CNTRL,
                                      (GT_U16)(vctInfo[port].reg0Val | 0x8000)))
                  != GT_OK)
       return status;

    /* read registers 26 and 27 - Check if test succeded. */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_FE_PHY_CABLE_DIAG1,
                                                &value1)) != GT_OK)
       return status;
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_FE_PHY_CABLE_DIAG2,
                                                &value2)) != GT_OK)
       return status;

    /* if the status of the both pairs is fail -> try to run TDR test again*/
    if ( (((value1 & 0x6000) >> 13) != 3) || (((value2 & 0x6000) >> 13) != 3) )
    {
        vctSuccess = GT_TRUE;
    }

    /* Disable WA. */
    if ((status = prvDisableTdrFe1WorkAround(dev, port)) != GT_OK)
        return status;

    if ((!vctSuccess) && (action == CPSS_VCT_STARTED_E))
    {
        /* switch to CPSS_VCT_REDO_PHASE_1_E */
        vctInfo[port].vctState = CPSS_VCT_REDO_PHASE_1_E;

        /* Restart the test */
        if((status = prvVctReady_Phy100M(dev,port)) != GT_OK)
            return(status);

        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
    }

    /* Test has secceded. Collect results. */
    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;

    /* collect test results for two pairs*/
    for (i = 0; i < 2; i++)
    {
        /* Read distance of the reflection and status of cable */
        if ((status = PRV_PHY_REG_READ_MAC(dev, port,
                             (GT_U8)(PRV_CPSS_FE_PHY_CABLE_DIAG1 + i), &value)) != GT_OK)
            return status;

        testRes = (value & 0x6000) >> 13;
        dist = (GT_U8)(value & 0xff);
        switch (testRes)
        {
            case 0:
                /* good cable*/
                if ((dist) == 0xFF)
                {
                    cableStatusPtr->cableStatus[i].testStatus =
                                                        CPSS_VCT_NORMAL_CABLE_E;
                    cableStatusPtr->cableStatus[i].errCableLen = 0;
                }
                /* impedance mismatch*/
                else
                {
                    cableStatusPtr->cableStatus[i].testStatus =
                                                  CPSS_VCT_IMPEDANCE_MISMATCH_E;
                    status = prvGetErrFeCabLen(dev, port, dist,
                                   &(cableStatusPtr->cableStatus[i].errCableLen));
                    if (status != GT_OK)
                    {
                        return status;
                    }
                }
                break;
            case 1:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_SHORT_CABLE_E;
                status = prvGetErrFeCabLen(dev, port, dist,
                                    &(cableStatusPtr->cableStatus[i].errCableLen));
                if (status != GT_OK)
                {
                    return status;
                }
                break;
            case 2:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_OPEN_CABLE_E;
                /*prvGetErrFeCabLen(dist,
                            &(cableStatusPtr->cableStatus[i].errCableLen));*/
                break;
            case 3:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_TEST_FAIL_E;
                cableStatusPtr->cableStatus[i].errCableLen = 0;
                break;
        }
    }

    /* run the VCT test again with 2nd WA if the status is open */
    if( (cableStatusPtr->cableStatus[0].testStatus == CPSS_VCT_OPEN_CABLE_E) ||
        (cableStatusPtr->cableStatus[1].testStatus == CPSS_VCT_OPEN_CABLE_E) )
    {
        /* switch to CPSS_VCT_REDO_PHASE_2_E */
        vctInfo[port].vctState = CPSS_VCT_REDO_PHASE_2_E;

        /* start TDR test on FE PHY */
        if ((status = prvEnableTdrFe1WorkAround(dev, port)) != GT_OK)
            return status;
        if ((status = prvEnableTdrFe2WorkAround(dev, port)) != GT_OK)
            return status;

        /* Restart the test */
        if((status = prvVctReady_Phy100M(dev,port)) != GT_OK)
            return(status);

        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
    }

    cableStatusPtr->phyType = CPSS_VCT_PHY_100M_E;

    return GT_OK;
}

/*******************************************************************************
* prvVctRedoPhase2_Phy100M
*
* DESCRIPTION:
*       This routine asserts the Phase2 state of virtual cable test for the
*       10/100Mbps phy. If succeded, it returns the returns the the status per
*       Rx/Tx pair. Otherwise, it restarts the test.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number.
*       port   - logical port number.
*
* OUTPUTS:
*       cableStatusPtr - the port copper cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctRedoPhase2_Phy100M
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM  port,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{
    GT_U8       i;
    GT_U8       dist;
    GT_U32      testRes;
    GT_U16      value;
    GT_STATUS   status;


    if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_FE_PHY_CABLE_DIAG1,&value))
                                                                       != GT_OK)
        return status;

#if defined (ASIC_SIMULATION)
      value &= (~ 0x8000) ;
#endif

    /* Check if Previous test is finished. */
    if ((value & 0x8000) != 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
    }

    /* restore the original content of register 0 + software reset */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_PHY_CNTRL,
                            (GT_U16)(vctInfo[port].reg0Val | 0x8000))) != GT_OK)
        return status;

    /* Disable WA's */
    if ((status = prvDisableTdrFe1WorkAround(dev, port)) != GT_OK)
        return status;
    if ((status = prvDisableTdrFe2WorkAround(dev, port)) != GT_OK)
        return status;

    /* Get OPEN_CABLE results. */
    for (i = 0; i < 2; i++)
    {
        /* Read distance of the reflection and status of cable */
        status = PRV_PHY_REG_READ_MAC(dev, port,
                        (GT_U8)(PRV_CPSS_FE_PHY_CABLE_DIAG1 + i), &value);

        if (status != GT_OK)
            return status;

        testRes = (value & 0x6000) >> 13;
        dist = (GT_U8)(value & 0xff);
        switch (testRes)
        {
            case 2:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_OPEN_CABLE_E;
                status = prvGetErrFeCabLen(dev, port, dist,
                                &(cableStatusPtr->cableStatus[i].errCableLen));
                if (status != GT_OK)
                {
                    return status;
                }
                break;
        }
    }

    cableStatusPtr->phyType = CPSS_VCT_PHY_100M_E;

    return GT_OK;
}



/*******************************************************************************
* prvVctStarted_Phy1000M
*
* DESCRIPTION:
*       This routine asserts the second state of virtual cable test for the
*       1000Mbps phy. If succeded, it returns the returns the the status per
*       Rx/Tx pair. Otherwise, it restarts the test.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number.
*       port   - logical port number.
*
* OUTPUTS:
*       cableStatusPtr - the port copper cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctStarted_Phy1000M
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM   port,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{
    GT_U8       i;
    GT_U8       dist;
    GT_U32      testRes;
    GT_U16      value;
    GT_STATUS   status;

    /* select page 0 (cooper) */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port,
                                 PRV_CPSS_GE_PHY_EXT_ADDR, 0)) != GT_OK)
        return status;

    /* Check if previously asserted test had been completed. */
    if((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_GE_PHY_CABLE_DIAG,&value))
                                                                       != GT_OK)
       return status;

#if defined (ASIC_SIMULATION)
    value &= (~ 0x8000) ;
#endif
    if ((value & 0x8000) != 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);

    /* Test completed. Disable WA. */
    if ((status = prvDisableTdrGeWorkAround(dev, port)) != GT_OK)
        return status;

    /* Test completed. collect results. */
    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;

    /* collect test results */
    for (i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
    {
        /* select pair */
        if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR,
                                            i)) != GT_OK)
            return status;

        cpssOsTimerWkAfter(1);

        /* Read distance of the reflection and status of cable */
        if ((status = PRV_PHY_REG_READ_MAC(dev, port, PRV_CPSS_GE_PHY_CABLE_DIAG,
                                                  &value)) != GT_OK)
            return status;

        testRes = (value & 0x6000) >> 13;
        dist = (GT_U8)(value & 0xff);

        switch (testRes)
        {
        case 0:
                /* good cable*/
                if ((dist) == 0xFF)
                {
                    cableStatusPtr->cableStatus[i].testStatus =
                                                        CPSS_VCT_NORMAL_CABLE_E;
                    cableStatusPtr->cableStatus[i].errCableLen = 0;
                }
                /* if the reflected amplitude is low it is good cable too */
                /* for registers values: 0xE23, 0xE24, 0xE25, 0xE26, 0xE27*/
                else if ((value > 0xE22) && (value < 0xE28))
                {
                    cableStatusPtr->cableStatus[i].testStatus =
                                                        CPSS_VCT_NORMAL_CABLE_E;
                    cableStatusPtr->cableStatus[i].errCableLen = 0;
                }
                /* impedance mismatch*/
                else
                {
                    cableStatusPtr->cableStatus[i].testStatus =
                                                  CPSS_VCT_IMPEDANCE_MISMATCH_E;
                    status = prvGetErrFeCabLen(dev, port, dist,
                                   &(cableStatusPtr->cableStatus[i].errCableLen));
                    if (status != GT_OK)
                    {
                        return status;
                    }
                    break;
                }

                /* To get Cable Length, Link should be ON and Speed should be
                * 100M or 1000M */
                if(!(vctInfo[port].reg17Val & 0x0400))
                {
                    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;
                    break;
                }

                if( (vctInfo[port].reg17Val & 0xC000) != 0x8000)
                {
                    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;
                    break;
                }

                switch ((vctInfo[port].reg17Val & 0x380) >> 7)
                {
                    case 0:
                        cableStatusPtr->normalCableLen = CPSS_VCT_LESS_THAN_50M_E;
                        break;
                    case 1:
                        cableStatusPtr->normalCableLen = CPSS_VCT_50M_80M_E;
                        break;
                    case 2:
                        cableStatusPtr->normalCableLen = CPSS_VCT_80M_110M_E;
                        break;
                    case 3:
                        cableStatusPtr->normalCableLen = CPSS_VCT_110M_140M_E;
                        break;
                    case 4:
                        cableStatusPtr->normalCableLen = CPSS_VCT_MORE_THAN_140_E;
                        break;
                    default:
                        cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;
                        break;
                }
                break;
            case 1:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_SHORT_CABLE_E;
                status = prvGetErrGeCabLen(dev, port, dist,
                                &(cableStatusPtr->cableStatus[i].errCableLen));
                if (status != GT_OK)
                {
                    return status;
                }
                break;
            case 2:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_OPEN_CABLE_E;
                status = prvGetErrGeCabLen(dev, port, dist,
                                &(cableStatusPtr->cableStatus[i].errCableLen));
                if (status != GT_OK)
                {
                    return status;
                }
                break;
            case 3:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_TEST_FAIL_E;
                cableStatusPtr->cableStatus[i].errCableLen = 0;
                break;
        }
    } /* for each pair */

    cableStatusPtr->phyType = CPSS_VCT_PHY_1000M_E;

    /* return the page register to page 0 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, PRV_CPSS_GE_PHY_EXT_ADDR, 0))
                                                                    != GT_OK)
        return status;

    return GT_OK;
}

/*******************************************************************************
* prvVctReady_Phy1000MMP
*
* DESCRIPTION:
*       This routine asserts the first state of virtual cable test for the
*       1000Mbps E1149, E1112 and alike phy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev  - device number.
*       port - logical port number.
*       phyType - phy type
*
* OUTPUTS:
*       cableStatusPtr - the port copper cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctReady_Phy1000MMP
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM port,
    IN  CPSS_VCT_PHY_TYPES_ENT    phyType,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{
    GT_U8       i;
    GT_U16      value;
    GT_STATUS   status;


    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;

    /* If Fiber is used, simply return with test fail. */
    if (phyType != CPSS_VCT_PHY_1000M_MP_NO_FIBER_E)
    {
        if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x1)) != GT_OK)
                return status;
        cpssOsTimerWkAfter(1);

        if ((status = PRV_PHY_REG_READ_MAC(dev, port, 17, &value)) !=
            GT_OK)
                return status;

        if(value & 0x400)
        {
                for (i=0; i<CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
                {
                        cableStatusPtr->cableStatus[i].testStatus =
                                                           CPSS_VCT_TEST_FAIL_E;
                        cableStatusPtr->cableStatus[i].errCableLen = 0;
                }

                return GT_OK;
        }
    }

    /* Save global registers. */
    /* read register 17 value */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x0)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 17, &(vctInfo[port].reg17Val)))
                                      != GT_OK)
            return status;

    /* read register 26 value */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x5)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 26, &(vctInfo[port].reg26Val)))
                                      != GT_OK)
            return status;

    /* Assert VCT test */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x5)) != GT_OK)
        return status;

    cpssOsTimerWkAfter(1);

    /* enable test */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 16, &value)) != GT_OK)
        return status;

    value |= 0x8000;
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 16, value)) != GT_OK)
        return status;

    return GT_OK;
}
/*******************************************************************************
* prvVctReady_Phy1000MMP_NG
*
* DESCRIPTION:
*       This routine asserts the first state of virtual cable test for the
*       1000Mbps E1149R/E1240.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev  - device number.
*       port - logical port number.
*
* OUTPUTS:
*       cableStatusPtr - the port copper cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctReady_Phy1000MMP_NG
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM  port,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{

    GT_U16      data;        /* temporary data */
    GT_STATUS   status;      /* return code    */

    /* select page 5*/
    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;

    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x5)) != GT_OK)
       return status;
    cpssOsTimerWkAfter(1);

    /* setting launch pair */
    if ((status = gtPrvSetTxPulseTwistedPair(dev, port, 0)) != GT_OK)
            return status;

    /* enable test */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 23, &data)) != GT_OK)
        return status;

    data |= 0x8000;

    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 23, data)) != GT_OK)
        return status;

    cableStatusPtr->phyType = CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E;

    /* save current launching pair */
    vctInfo[port].currentTxPair = 0;

    return GT_OK;
}


/*******************************************************************************
* prvVctStarted_Phy1000MMP
*
* DESCRIPTION:
*       This routine asserts the second state of virtual cable test for the
*       1000Mbps E1149, E1112 and alike phy. If succeded, it returns the status
*       per Rx/Tx pair. Otherwise, it restarts the test.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number.
*       port   - logical port number.
*
* OUTPUTS:
*       cableStatusPtr - the port copper cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctStarted_Phy1000MMP(
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM port,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{

    GT_U16      value;
    GT_U8       dist;
    GT_U32      testRes;
    GT_U8       i;
    GT_STATUS   status;


    /* Read if previously asserted test had been completed. */
    if((status = PRV_PHY_REG_READ_MAC(dev, port, 16,&value)) != GT_OK)
       return status;

#if defined (ASIC_SIMULATION)
    value &= (~ 0x8000) ;
#endif
    if ((value & 0x8000) != 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);

    /* select page 5 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x5)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    /* collect test results */
    for (i = 0; i < CPSS_VCT_MDI_PAIR_NUM_CNS; i++)
    {
        /* Read distance of the reflection and status of cable */
        if ((status = PRV_PHY_REG_READ_MAC(dev, port, (GT_U8)(16+i),
                                                    &value)) != GT_OK)
            return status;

        testRes = (value & 0x6000) >> 13;
        dist = (GT_U8)(value & 0xff);

        switch (testRes)
        {
            case 0:
                /* good cable*/
                if ((dist) == 0xFF)
                {
                    cableStatusPtr->cableStatus[i].testStatus =
                                                        CPSS_VCT_NORMAL_CABLE_E;
                    cableStatusPtr->cableStatus[i].errCableLen = 0;
                }
                /* impedance mismatch*/
                else
                {
                    cableStatusPtr->cableStatus[i].testStatus =
                                                  CPSS_VCT_IMPEDANCE_MISMATCH_E;
                    status = prvGetErrFeCabLen(dev, port, dist,
                               &(cableStatusPtr->cableStatus[i].errCableLen));
                    if (status != GT_OK)
                    {
                        return status;
                    }
                    break;
                }

                /* To get Cable Length, Link should be ON and Speed should be
                 * 100M or 1000M */

                if(!(vctInfo[port].reg17Val & 0x0400))
                {
                    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;
                    break;
                }

                if( (vctInfo[port].reg17Val & 0xC000) != 0x8000)
                {
                    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;
                    break;
                }

                switch (vctInfo[port].reg26Val & 0x7)
                {
                    case 0:
                        cableStatusPtr->normalCableLen = CPSS_VCT_LESS_THAN_50M_E;
                        break;
                    case 1:
                        cableStatusPtr->normalCableLen = CPSS_VCT_50M_80M_E;
                        break;
                    case 2:
                        cableStatusPtr->normalCableLen = CPSS_VCT_80M_110M_E;
                        break;
                    case 3:
                        cableStatusPtr->normalCableLen = CPSS_VCT_110M_140M_E;
                        break;
                    case 4:
                        cableStatusPtr->normalCableLen = CPSS_VCT_MORE_THAN_140_E;
                        break;
                    default:
                        cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;
                        break;
                }
                break;
            case 1:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_SHORT_CABLE_E;
                status = prvGetErrGeCabLen(dev, port, dist,
                                   &(cableStatusPtr->cableStatus[i].errCableLen));
                if (status != GT_OK)
                {
                    return status;
                }
                break;
            case 2:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_OPEN_CABLE_E;
                status = prvGetErrGeCabLen(dev, port, dist,
                                   &(cableStatusPtr->cableStatus[i].errCableLen));
                if (status != GT_OK)
                {
                    return status;
                }
                break;
            case 3:
                cableStatusPtr->cableStatus[i].testStatus = CPSS_VCT_TEST_FAIL_E;
                cableStatusPtr->cableStatus[i].errCableLen = 0;
                break;
        }
    } /* for each pair */

    cableStatusPtr->phyType = CPSS_VCT_PHY_1000M_MP_E;

    /* return to page 0 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x00)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    return GT_OK;
}
/*******************************************************************************
* prvVctStarted_Phy1000MMP_NG
*
* DESCRIPTION:
*       This routine asserts the second state of virtual cable test for the
*       1000Mbps E1149R/E1240. If succeded, it returns the status
*       per Rx/Tx pair. Otherwise, it restarts the test.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - device number.
*       port            - physical port number.
*       phyIdRegValue   - PHY ID register's value
*
* OUTPUTS:
*       cableStatusPtr - the port copper cable status.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvVctStarted_Phy1000MMP_NG(
    IN  GT_U8   dev,
    IN  GT_PHYSICAL_PORT_NUM   port,
    IN  GT_U16  phyIdRegValue,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
)
{
    GT_U16      data;              /* temporary data */
    GT_U32      distToFault;       /* distance to fault */
    GT_U32      amplitude;         /* accepted amplitude value */
    GT_U32      amplitudeThreshold;/* amplitude threshold to select fault type */
    GT_STATUS   status;            /* return code */
    GT_U32      polarity;          /* accepted polarity value */
    GT_U32      txPair,rxPair;     /* loop iterators */
    GT_U32      rxDbIndex;         /* index in the Rx results DB */
    GT_U8       errCableLen;
    
    cableStatusPtr->normalCableLen = CPSS_VCT_UNKNOWN_LEN_E;

    /* select page 5 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x5)) != GT_OK)
            return status;

    /* check whether TDR test is finished */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 23, &data)) != GT_OK)
            return status;
    DBG_VCT_MAC(("\n after reading register 23 data is: 0x%x\n", data));

    /* the value 1 of bit 14 of register 23 page 5 indicates that VCT completed
       and TDR results in the registers are valid */
    if(!(data & 0x4000))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
    }

    /* store accepted results */
    for (rxPair = 0; rxPair < CPSS_VCT_MDI_PAIR_NUM_CNS; rxPair++)
    {
        rxDbIndex = rxPair + (CPSS_VCT_MDI_PAIR_NUM_CNS * vctInfo[port].currentTxPair);
        if ((status = PRV_PHY_REG_READ_MAC(dev, port,
                          (GT_U8)(16 + rxPair),
                          &vctInfo[port].rxPairsRes[rxDbIndex])) != GT_OK)
                return status;
    }

    /* start analysing test results */
    if(vctInfo[port].currentTxPair == (CPSS_VCT_MDI_PAIR_NUM_CNS - 1))
    {
        /* outer loop running on all 4 pairs */
        for (txPair = 0; txPair < CPSS_VCT_MDI_PAIR_NUM_CNS; txPair++)
        {
            for (rxPair = 0; rxPair < CPSS_VCT_MDI_PAIR_NUM_CNS; rxPair++)
            {
                rxDbIndex = rxPair + (CPSS_VCT_MDI_PAIR_NUM_CNS * txPair);

                data = vctInfo[port].rxPairsRes[rxDbIndex];

                /* TDR test couldn't be completed */
                if(data == 0xFFFF)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

                /* Good Termination */
                if((data & 0x00FF)  == 0x0)
                {
                    DBG_VCT_MAC(("\n normal cable \n"));
                    /* store results fro own Rx pair and
                      continue check next Rx one */
                    if(rxPair == txPair)
                    {
                        cableStatusPtr->cableStatus[txPair].testStatus =
                                                       CPSS_VCT_NORMAL_CABLE_E;
                        cableStatusPtr->cableStatus[txPair].errCableLen = 0;
                    }
                }
                /* Polarity/Amplitude changes on the same pair */
                else
                {
                    /* calculate distance to fault */
                    distToFault = data & 0xff;
                    errCableLen = 0;
                    status = prvGetErrGeCabLenNG(dev, port, distToFault,
                                                 phyIdRegValue,
                                                 &errCableLen);
                    cableStatusPtr->cableStatus[txPair].errCableLen = errCableLen;
                    if (status != GT_OK)
                    {
                        return status;
                    }

                    if( ( ((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1543) ||
                          ((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1680) ||
                          ((phyIdRegValue & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1680M))&&
                        (cableStatusPtr->cableStatus[txPair].errCableLen >
                                                       MIN_110M_CABLE_LEN_DUE_TO_DEVIATION_CNS) )
                    {
                        amplitudeThreshold =
                            THRESHOLD_AMPLITUDE_VALUE_FOR_110M_CABLE_CNS;
                    }
                    else
                    {
                        amplitudeThreshold = THRESHOLD_AMPLITUDE_VALUE_CNS;
                    }

                    /* not same pair and cross pair short */
                    if(rxPair != txPair)
                    {
                        DBG_VCT_MAC(("\n wrong pair : %d\n",rxPair));
                        DBG_VCT_MAC(("\n active channel is : %d\n",txPair));
                        /* calculate wrong pair*/
                        cableStatusPtr->cableStatus[txPair].testStatus =
                                           CPSS_VCT_SHORT_WITH_PAIR0_E + rxPair;
                    }
                    /* not same pair */
                    else
                    {
                        amplitude = (data >> 8) & 0x7F;
                        DBG_VCT_MAC(("\n amplitude is: %d  \n", amplitude));

                        /* extract polarity - bit 15 */
                        polarity = ((data >> 15) & 0x1);

                        DBG_VCT_MAC(("\n polarity is: %d  \n", polarity));

                        if(polarity)
                        {
                            if(amplitude > amplitudeThreshold)
                            {
                                 cableStatusPtr->cableStatus[txPair].testStatus =
                                     CPSS_VCT_OPEN_CABLE_E;
                                  DBG_VCT_MAC(("\n open cable \n"));
                            }
                            else
                            {
                                 cableStatusPtr->cableStatus[txPair].testStatus =
                                                  CPSS_VCT_IMPEDANCE_MISMATCH_E;
                                  DBG_VCT_MAC(("\n case true: impedancee \n"));
                            }
                        }
                        else
                        {
                            if(amplitude > amplitudeThreshold)
                            {
                                 cableStatusPtr->cableStatus[txPair].testStatus =
                                                  CPSS_VCT_SHORT_CABLE_E;
                                 DBG_VCT_MAC(("\n short cable \n"));
                            }
                            else
                            {
                                DBG_VCT_MAC(("\n case false: impedance"));
                                cableStatusPtr->cableStatus[txPair].testStatus =
                                                  CPSS_VCT_IMPEDANCE_MISMATCH_E;
                            }
                        }/* polarity else */

                    }/* not same pair */

                    /* first failure defines final one */
                    break;

                }/* else */

            } /* for each Rx pair loop */

        }/* active pair Tx loop*/

    }
    else /* test not finished */
    {
        /* setting next launch pair */
        if ((status = gtPrvSetTxPulseTwistedPair(dev, port,
                                     ++vctInfo[port].currentTxPair)) != GT_OK)
                return status;

        /* enable test */
        if ((status = PRV_PHY_REG_READ_MAC(dev, port, 23,
                                                           &data)) != GT_OK)
            return status;

        data |= 0x8000;

        if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 23,data)) != GT_OK)
            return status;

        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
    }

    cableStatusPtr->phyType = CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E;

    /* return to page 0 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x00)) != GT_OK)
            return status;
    cpssOsTimerWkAfter(1);

    return GT_OK;



}

/*******************************************************************************
* gtPrvSetTxPulseTwistedPair
*
* DESCRIPTION:
*       Sets the pair to launch the pulse.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev        - device number.
*       port       - port number.
*       twistedPairNum    - one out of four cable pairs(values 0-3)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_NOT_SUPPORTED - on not supported PHYs
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS gtPrvSetTxPulseTwistedPair
(
    IN  GT_U8     dev,
    IN  GT_PHYSICAL_PORT_NUM     port,
    IN  GT_U16     twistedPairNum
)
{
    GT_U16    value;       /* temporary value */
    GT_STATUS status;      /* return code*/

    /* HW values start from 4 up to 7 */
    twistedPairNum += 4;

    /* read for write */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 23, &value)) != GT_OK)
    {
        return status;
    }
    /* channel is represented by bits 11-13 in reg23 */
    value &= ~(7 << 11);

    value |= (twistedPairNum << 11);

    /* update twistedPairNum */
    status = PRV_PHY_REG_WRITE_MAC(dev, port, 23, value);

    return status;

}
/*******************************************************************************
* internal_cpssVctSetCableDiagConfig
*
* DESCRIPTION:
*       This routine configures VCT diagnostic parameters per port:
*        - cable's length: greater or less then 10 meters.
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev       - device number
*       port      - port number
*       configPtr - configuration parameters
*
* OUTPUTS:
*       None
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong length configuration
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssVctSetCableDiagConfig
(
    IN  GT_U8         dev,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  CPSS_VCT_CONFIG_STC *configPtr
)
{
    CPSS_VCT_PHY_TYPES_ENT phyType;       /* phy type      */
    GT_STATUS              status;        /* return status */
    GT_U16                 data;          /* temporary data*/
    GT_U16                 page;          /* temporary page*/

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(dev) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check port and device number validity */
    PRV_CPSS_PHY_PORT_CHECK_MAC(dev, port);

    /* check configPtr is not NULL */
    CPSS_NULL_PTR_CHECK_MAC(configPtr);

    if( (configPtr->length != CPSS_VCT_CABLE_LESS_10M_E) &&
        (configPtr->length != CPSS_VCT_CABLE_GREATER_10M_E) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get PHY's type */
    if ((status = prvDoesDeviceSupportVct(dev, port, &phyType, &data)) != GT_OK)
    {
        return status;
    }
    /* check if length configuration is supported */
    if(phyType != CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* for multi-page phy disable PPU before running VCT */
    PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->genVctBindFunc.
                                 cpssPhyAutoNegotiationSet(dev, port, GT_FALSE);

    /* Read current page */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 22,
                                                &page)) != GT_OK)
    /* select page 5 */
    if ((status = PRV_PHY_REG_WRITE_MAC(dev, port, 22, 0x5)) != GT_OK)
        goto exit_cleanly;

    /* Read current pulse width value */
    if ((status = PRV_PHY_REG_READ_MAC(dev, port, 28,
                                                &data)) != GT_OK)
        goto exit_cleanly;

    /* clear relevant bits, which also sets FULL PULSE (0) */
    data &= ~(0x3 << 10);

    /* check whether pulse length should be changed to 1/4 */
    if(configPtr->length == CPSS_VCT_CABLE_LESS_10M_E)
    {
        DBG_VCT_MAC(("\n inside CPSS_VCT_CABLE_LESS_10M_E"));

        /* update width */
        data |= (0x3 << 10);
    }
    /* else reset to 0, full pulse */

    /* update value */
    status = PRV_PHY_REG_WRITE_MAC(dev, port, 28, data);

exit_cleanly:
    /* set back previous page */
    PRV_PHY_REG_WRITE_MAC(dev, port, 22, page);

    /* Enable AutoNegotiation */
    PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, port)->genVctBindFunc.
                                 cpssPhyAutoNegotiationSet(dev, port, GT_TRUE);

    return status;
}

/*******************************************************************************
* cpssVctSetCableDiagConfig
*
* DESCRIPTION:
*       This routine configures VCT diagnostic parameters per port:
*        - cable's length: greater or less then 10 meters.
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev       - device number
*       port      - port number
*       configPtr - configuration parameters
*
* OUTPUTS:
*       None
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong length configuration
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on non supported device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssVctSetCableDiagConfig
(
    IN  GT_U8         dev,
    IN  GT_PHYSICAL_PORT_NUM  port,
    IN  CPSS_VCT_CONFIG_STC *configPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssVctSetCableDiagConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, port, configPtr));

    rc = internal_cpssVctSetCableDiagConfig(dev, port, configPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, port, configPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssVctLengthOffsetSet
*
* DESCRIPTION:
*       This function sets the offset used in VCT length calcualtions per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev       - device number
*       port      - port number
*       offset    - offset in milimeters.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       According to the PHYs' release notes the tuned formula to calculate
*       length in VCT is of the form:
*       A * hwDist - B + offset
*       where:
*       A & B  - constants published in the PHYs' release notes.
*       hwDist - the distance value from HW registers to the fault.
*       offset - fine tuning required due to different "MDI traces" on
*                different boards.
*       This function should be called after cpssDxChHwPpPhase1Init() and
*       before runing the VCT.
*
*******************************************************************************/
static GT_STATUS internal_cpssVctLengthOffsetSet
(
    IN  GT_U8   dev,
    IN  GT_PHYSICAL_PORT_NUM   port,
    IN  GT_32   offset
)
{
    if(PRV_CPSS_IS_DEV_EXISTS_MAC(dev) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check port and device number validity */
    PRV_CPSS_PHY_PORT_CHECK_MAC(dev, port);

    PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].vctLengthOffset = offset;

    return GT_OK;
}

/*******************************************************************************
* cpssVctLengthOffsetSet
*
* DESCRIPTION:
*       This function sets the offset used in VCT length calcualtions per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev       - device number
*       port      - port number
*       offset    - offset in milimeters.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       According to the PHYs' release notes the tuned formula to calculate
*       length in VCT is of the form:
*       A * hwDist - B + offset
*       where:
*       A & B  - constants published in the PHYs' release notes.
*       hwDist - the distance value from HW registers to the fault.
*       offset - fine tuning required due to different "MDI traces" on
*                different boards.
*       This function should be called after cpssDxChHwPpPhase1Init() and
*       before runing the VCT.
*
*******************************************************************************/
GT_STATUS cpssVctLengthOffsetSet
(
    IN  GT_U8   dev,
    IN  GT_PHYSICAL_PORT_NUM   port,
    IN  GT_32   offset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssVctLengthOffsetSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, port, offset));

    rc = internal_cpssVctLengthOffsetSet(dev, port, offset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, port, offset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssVctLengthOffsetGet
*
* DESCRIPTION:
*       This function gets the offset used in VCT length calcualtions per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev       - device number
*       port      - port number
*
* OUTPUTS:
*       offsetPtr - (pointer to)offset in milimeters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       According to the PHYs' release notes the tuned formula to calculate
*       length in VCT is of the form:
*       A * hwDist - B + offset
*       where:
*       A & B  - constants published in the PHYs' release notes.
*       hwDist - the distance value from HW registers to the fault.
*       offset - fine tuning required due to different "MDI traces" on
*                different boards.
*
*******************************************************************************/
static GT_STATUS internal_cpssVctLengthOffsetGet
(
    IN  GT_U8   dev,
    IN  GT_PHYSICAL_PORT_NUM   port,
    OUT GT_32   *offsetPtr
)
{
    GT_U32 portMacNum;

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(dev) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check port and device number validity */
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(dev, port, portMacNum);
	
    /* check configPtr is not NULL */
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);

    *offsetPtr = PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[portMacNum].vctLengthOffset;

    return GT_OK;
}

/*******************************************************************************
* cpssVctLengthOffsetGet
*
* DESCRIPTION:
*       This function gets the offset used in VCT length calcualtions per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev       - device number
*       port      - port number
*
* OUTPUTS:
*       offsetPtr - (pointer to)offset in milimeters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       According to the PHYs' release notes the tuned formula to calculate
*       length in VCT is of the form:
*       A * hwDist - B + offset
*       where:
*       A & B  - constants published in the PHYs' release notes.
*       hwDist - the distance value from HW registers to the fault.
*       offset - fine tuning required due to different "MDI traces" on
*                different boards.
*
*******************************************************************************/
GT_STATUS cpssVctLengthOffsetGet
(
    IN  GT_U8   dev,
    IN  GT_PHYSICAL_PORT_NUM   port,
    OUT GT_32   *offsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssVctLengthOffsetGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, port, offsetPtr));

    rc = internal_cpssVctLengthOffsetGet(dev, port, offsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, port, offsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssVctDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the VCT.
*       NOTE: function 'free' the allocated memory.
*             and restore DB to 'pre-init' state
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssVctDbRelease
(
    void
)
{
    FREE_PTR_MAC(vctInfo);

    return GT_OK;
}

