/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenPhyVct.h
*
* DESCRIPTION:
*       API definitions for CPSS Marvell Virtual Cable Tester functionality.
*
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/

#ifndef __cpssGenPhyVcth
#define __cpssGenPhyVcth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/* maximum number of pairs in cable. Mostly: (1,2) (3,6) (4,5) (7,8)   */
#define CPSS_VCT_MDI_PAIR_NUM_CNS     4

/* number of channel pairs in cable: A/B and C/D */
#define CPSS_VCT_CHANNEL_PAIR_NUM_CNS 2

/*
 * typedef: enum CPSS_VCT_TEST_STATUS_ENT
 *
 * Description: Enumeration of VCT test status
 *
 * Enumerations:
 *      CPSS_VCT_TEST_FAIL_E    - virtual cable test failed (can't run the test)
 *      CPSS_VCT_NORMAL_CABLE_E - normal cable.
 *      CPSS_VCT_OPEN_CABLE_E   - open in cable.
 *      CPSS_VCT_SHORT_CABLE_E  - short in cable.
 *      CPSS_VCT_IMPEDANCE_MISMATCH_E - impedance mismatch in cable
 *                    (two cables of different quality is connected each other).
 *      CPSS_VCT_SHORT_WITH_PAIR0_E - short between Tx pair and Rx pair 0
 *      CPSS_VCT_SHORT_WITH_PAIR1_E - short between Tx pair and Rx pair 1
 *      CPSS_VCT_SHORT_WITH_PAIR2_E - short between Tx pair and Rx pair 2
 *      CPSS_VCT_SHORT_WITH_PAIR3_E - short between Tx pair and Rx pair 3
 */
typedef enum
{
    CPSS_VCT_TEST_FAIL_E,
    CPSS_VCT_NORMAL_CABLE_E,
    CPSS_VCT_OPEN_CABLE_E,
    CPSS_VCT_SHORT_CABLE_E,
    CPSS_VCT_IMPEDANCE_MISMATCH_E,
    CPSS_VCT_SHORT_WITH_PAIR0_E,
    CPSS_VCT_SHORT_WITH_PAIR1_E,
    CPSS_VCT_SHORT_WITH_PAIR2_E,
    CPSS_VCT_SHORT_WITH_PAIR3_E

} CPSS_VCT_TEST_STATUS_ENT;


/*
 * typedef: enum CPSS_VCT_NORMAL_CABLE_LEN_ENT
 *
 * Description: Enumeration for normal cable length
 *
 * Enumerations:
 *      CPSS_VCT_LESS_THAN_50M_E - cable length less than 50 meter.
 *      CPSS_VCT_50M_80M_E       - cable length between 50 - 80 meter.
 *      CPSS_VCT_80M_110M_E      - cable length between 80 - 110 meter.
 *      CPSS_VCT_110M_140M_E     - cable length between 110 - 140 meter.
 *      CPSS_VCT_MORE_THAN_140_E - cable length more than 140 meter.
 *      CPSS_VCT_UNKNOWN_LEN_E   - unknown length.
 *
 */
typedef enum
{
    CPSS_VCT_LESS_THAN_50M_E,
    CPSS_VCT_50M_80M_E,
    CPSS_VCT_80M_110M_E,
    CPSS_VCT_110M_140M_E,
    CPSS_VCT_MORE_THAN_140_E,
    CPSS_VCT_UNKNOWN_LEN_E

} CPSS_VCT_NORMAL_CABLE_LEN_ENT;

/*
 * typedef: enum CPSS_VCT_PHY_TYPES_ENT
 *
 * Description: Enumeration of Marvel PHY types
 *
 * Enumerations:
 *  CPSS_VCT_PHY_100M_E              -  10/100M phy, E3082 or E3083
 *  CPSS_VCT_PHY_1000M_E             -  Gigabit phy, the rest phys
 *  CPSS_VCT_PHY_10000M_E            -  10 Gigabit phy, unused
 *  CPSS_VCT_PHY_1000M_B_E           -  Gigabit phy which needs work-around
 *  CPSS_VCT_PHY_1000M_MP_E          -  Multi Paged Gigabit phy, (E1112/E1149)
 *  CPSS_VCT_PHY_1000M_MP_NO_FIBER_E -  Multi Paged Gigabit phy with no fiber
 *                                      support(i.e. E1149, revision C1)
 *  CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E -  Multi Paged Gigabit phy with no fiber
 *                                      support, next generation
 *                                      (i.e. E1149R/E1240)
 */
typedef enum
{
    CPSS_VCT_PHY_100M_E = 0,
    CPSS_VCT_PHY_1000M_E,
    CPSS_VCT_PHY_10000M_E,
    CPSS_VCT_PHY_1000M_B_E,
    CPSS_VCT_PHY_1000M_MP_E,
    CPSS_VCT_PHY_1000M_MP_NO_FIBER_E,
    CPSS_VCT_PHY_1000M_MP_NO_FIBER_NG_E
} CPSS_VCT_PHY_TYPES_ENT;


/*
 * typedef: struct CPSS_VCT_STATUS_STC
 *
 * Description: Structure holding VCT results
 *
 * Fields:
 *      testStatus  - VCT test status.
 *      errCableLen - for cable failure the estimate fault distance in meters.
 *
 */
typedef struct
{
    CPSS_VCT_TEST_STATUS_ENT testStatus;
    GT_U8                    errCableLen;

} CPSS_VCT_STATUS_STC;

/*
 * typedef: struct CPSS_VCT_CABLE_STATUS_STC
 *
 * Description: virtual cable diagnostic status per MDI pair.
 *
 * Fields:
 *      cableStatus     - VCT cable status.
 *      normalCableLen  - cable lenght for normal cable.
 *      phyType         - type of phy (100M phy or Gigabit phy)
 *
 * Comments:
 *      If PHY is PHY_100M cableStatus will have only 2 pairs relevant.
 *      One is RX Pair (cableStatus[0] or cableLen[0]) and
 *      the other is TX Pair (cableStatus[1] or cableLen[1]).
 */

typedef struct
{
    CPSS_VCT_STATUS_STC           cableStatus[CPSS_VCT_MDI_PAIR_NUM_CNS];
    CPSS_VCT_NORMAL_CABLE_LEN_ENT normalCableLen;
    CPSS_VCT_PHY_TYPES_ENT        phyType;

} CPSS_VCT_CABLE_STATUS_STC;

/*
 * typedef: enum CPSS_VCT_PAIR_SWAP_ENT
 *
 * Description: Enumeration for pair swap
 *
 * Enumerations:
 *      CPSS_VCT_CABLE_STRAIGHT_E  - channel A on MDI[0] and  B on MDI[1]
 *                           or channel C on MDI[3] and D on MDI[4]
 *      CPSS_VCT_CABLE_CROSSOVER_E - channel B on MDI[0] and  A on MDI[1]
 *                           or channel D on MDI[3] and C on MDI[4]
 *      CPSS_VCT_NOT_APPLICABLE_SWAP_E - in FE just two first channels are checked
 *
 */
typedef enum
{
    CPSS_VCT_CABLE_STRAIGHT_E,
    CPSS_VCT_CABLE_CROSSOVER_E,
    CPSS_VCT_NOT_APPLICABLE_SWAP_E

} CPSS_VCT_PAIR_SWAP_ENT;

/*
 * typedef: enum CPSS_VCT_POLARITY_SWAP_ENT
 *
 * Description: Enumeration for pair polarity swap
 *
 * Enumerations:
 *      CPSS_VCT_POSITIVE_POLARITY_E         - good polarity
 *      CPSS_VCT_NEGATIVE_POLARITY_E         - reversed polarity
 *      CPSS_VCT_NOT_APPLICABLE_POLARITY_E   - in FE there just one value for
 *                              all the cable then just first pair is applicable
 *
 */
typedef enum
{
    CPSS_VCT_POSITIVE_POLARITY_E,
    CPSS_VCT_NEGATIVE_POLARITY_E,
    CPSS_VCT_NOT_APPLICABLE_POLARITY_E /* in FE just one pair is applicable  */

} CPSS_VCT_POLARITY_SWAP_ENT;

/*
 * typedef: struct CPSS_VCT_PAIR_SKEW_STC
 *
 * Description: pair skew values.
 *
 * Fields:
 *      isValid     - whether results are valid
 *                      (not valid for FE).
 *      skew        - delay between pairs in n-Seconds.
 *
 * Comments:
 *
 */
typedef struct
{
    GT_BOOL     isValid;
    GT_U32      skew[CPSS_VCT_MDI_PAIR_NUM_CNS];

} CPSS_VCT_PAIR_SKEW_STC;

/*
 * typedef: struct CPSS_VCT_EXTENDED_STATUS_STC
 *
 * Description: extended virtual cable diagnostic status per MDI pair/channel
 *              pair.
 * Fields:
 *      isValid      - if results are valid (if not, maybe there is no
 *                      gigabit link for GE or 100M link for FE).
 *      pairSwap     - determine the channel associated with the MDI pair
 *                      (cross or not for each two MDI pairs).
 *      pairPolarity - detect for each of the pairs if connected with reverse
 *                     polarity (reverse on one side between two conductors in
 *                     one pair)
 *      pairSkew     - the skew among the four pairs of the cable
 *                      (delay between pairs in n-Seconds)
 *
 * Comments:
 *      If PHY is PHY_100M cableStatus will have only 2 pairs relevant.
 *      in this case:
 *        swap - just pairSwap[0] relevant
 *        polarity - just pairPolarity[0] and pairPolarity[1] are relevant
 *        skew -  will be not relevant
 *
 */
typedef struct
{
    GT_BOOL                           isValid;
    CPSS_VCT_PAIR_SWAP_ENT            pairSwap[CPSS_VCT_CHANNEL_PAIR_NUM_CNS];
    CPSS_VCT_POLARITY_SWAP_ENT        pairPolarity[CPSS_VCT_MDI_PAIR_NUM_CNS];
    CPSS_VCT_PAIR_SKEW_STC            pairSkew;

} CPSS_VCT_EXTENDED_STATUS_STC;

/*
 * typedef: struct CPSS_VCT_ACCURATE_CABLE_LEN_STC
 *
 * Description: accurate cable length for each MDI pair.
 *
 * Fields:
 *      isValid             - if results are valid.
 *                              (for GE valid if gigabit link is on
 *                               and the length is less than 135 meters).
 *                              (for FE not valid).
 *      cableLen            - the length in meters
 *
 * Comments:
 *
 */
typedef struct
{
    GT_BOOL     isValid[CPSS_VCT_MDI_PAIR_NUM_CNS];     /* result is not valid if:
                                                       FE or length more than
                                                       135 meters */
    GT_U16      cableLen[CPSS_VCT_MDI_PAIR_NUM_CNS];    /* in meters */

} CPSS_VCT_ACCURATE_CABLE_LEN_STC;

/*
 * typedef: enum CPSS_VCT_CABLE_DOWNSHIFT_ENT
 *
 * Description: Enumeration for downshift status of the port.
 *              When the Giga link cannot be established (for example there is
 *              just two pairs is good) the downshift feature in the PHY can
 *              down the link to 100MB link, i.e. downshift in link.
 *
 * Enumerations:
 *      CPSS_VCT_NO_DOWNSHIFT_E             - no downshift
 *      CPSS_VCT_DOWNSHIFT_E                - downshift was occurs
 *      CPSS_VCT_NOT_APPLICABLE_DOWNSHIFT_E  - in FE.
 *
 */
typedef enum
{
    CPSS_VCT_NO_DOWNSHIFT_E,
    CPSS_VCT_DOWNSHIFT_E,
    CPSS_VCT_NOT_APPLICABLE_DOWNSHIFT_E

} CPSS_VCT_DOWNSHIFT_STATUS_ENT;

/*
 * typedef: struct CPSS_VCT_CABLE_EXTENDED_STATUS_STC
 *
 * Description: extended virtual cable diag. status per MDI pair/channel pair.
 *
 * Fields:
 *      vctExtendedCableStatus      - extended VCT cable status.
 *      accurateCableLen            - accurate cable lenght.
 *      twoPairDownShift            - downshift status.
 *
 * Comments:
 *      If PHY is PHY_100M cableStatus will have only 2 pairs relevant.
 *      in this case:
 *        swap - just pairSwap[0] relevant
 *        polarity - just pairPolarity[0] and pairPolarity[1] are relevant
 *        skew -  will be not relevant
 *        accurateCableLen - not relevant
 *        twoPairDownShift - not relevant
 */
typedef struct
{
    CPSS_VCT_EXTENDED_STATUS_STC       vctExtendedCableStatus;
    CPSS_VCT_ACCURATE_CABLE_LEN_STC    accurateCableLen;
    CPSS_VCT_DOWNSHIFT_STATUS_ENT      twoPairDownShift;

} CPSS_VCT_CABLE_EXTENDED_STATUS_STC;

/*
 * typedef: enum CPSS_VCT_ACTION_ENT
 *
 * Description: VCT action to be done by the vctGetCableStat function.
 *
 * Enumerations:
 *      CPSS_VCT_START_E    - Initializes a new VCT test.
 *      CPSS_VCT_GET_RES_E  - Reads the results of actually runing test.
 *      CPSS_VCT_ABORT_E    - Aborts runing test.
 *
 * Comments:
 */
typedef enum
{
    CPSS_VCT_START_E,
    CPSS_VCT_GET_RES_E,
    CPSS_VCT_ABORT_E

} CPSS_VCT_ACTION_ENT;

/*
 * typedef: enum CPSS_VCT_STATE
 *
 * Description: VCT state machine's states.
 *
 * Enumerations:
 *      CPSS_VCT_READY_E     - No test is currently running. Ready for a new test.
 *      CPSS_VCT_STARTED_E   - VCT is asserted.
 *      CPSS_VCT_REDO_PHASE_1_E - VCT is asserted. Asserting a second test.
 *      CPSS_VCT_REDO_PHASE_2_E - VCT is asserted. Asserting a third test.
 *
 * Comments:
 *          A seperate state machine would be held for each port.
 */
typedef enum
{
    CPSS_VCT_READY_E,
    CPSS_VCT_STARTED_E,
    CPSS_VCT_REDO_PHASE_1_E,
    CPSS_VCT_REDO_PHASE_2_E

} CPSS_VCT_STATE;

/*
 * typedef: enum CPSS_VCT_CABLE_LEN_ENT
 *
 * Description: Enumeration to define types of cable length.
 *
 * Enumerations:
 *      CPSS_VCT_CABLE_LESS_10M_E     - Less than 10 meters.
 *      CPSS_VCT_CABLE_GREATER_10M_E  - Greater than 10 meters.
 *
 * Comments:
 */
typedef enum{
   CPSS_VCT_CABLE_LESS_10M_E,
   CPSS_VCT_CABLE_GREATER_10M_E
}CPSS_VCT_CABLE_LEN_ENT;

/*
 * typedef: struct CPSS_VCT_CONFIG_STC
 *
 * Description: Holds cable length, configured by user.
 *
 * Fields:
 *      length   - Greater or less than 10m.
 *
 * Comments:
 */
typedef struct{
   CPSS_VCT_CABLE_LEN_ENT  length;

}CPSS_VCT_CONFIG_STC;
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
);

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
    IN  GT_U8 dev,
    IN  GT_PHYSICAL_PORT_NUM      port,
    IN  CPSS_VCT_ACTION_ENT       vctAction,
    OUT CPSS_VCT_CABLE_STATUS_STC *cableStatusPtr
);

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
    IN  GT_PHYSICAL_PORT_NUM   port,
    IN  CPSS_VCT_CONFIG_STC *configPtr
);

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
    IN  GT_32  offset
);

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
    OUT GT_32  *offsetPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenPhyVcth */

