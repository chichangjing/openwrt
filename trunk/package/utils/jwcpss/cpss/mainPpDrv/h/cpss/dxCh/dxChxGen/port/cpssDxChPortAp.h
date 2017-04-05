/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortAp.h
*
* DESCRIPTION:
*       CPSS DxCh API's for 802.3ap standard (defines the auto negotiation
*       for backplane Ethernet) configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#ifndef __cpssDxChPortAph
#define __cpssDxChPortAph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/* size of array of interfaces advertised by port during AP process */
#define CPSS_DXCH_PORT_AP_IF_ARRAY_SIZE_CNS 10

/*
 * typedef: enum CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT
 *
 * Description: Enumerator of AP Port FC Direction enablers.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_AP_FLOW_CONTROL_SYMMETRIC_E  - flow control in both directions
 *  CPSS_DXCH_PORT_AP_FLOW_CONTROL_ASYMMETRIC_E - in one direction
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_AP_FLOW_CONTROL_SYMMETRIC_E,
    CPSS_DXCH_PORT_AP_FLOW_CONTROL_ASYMMETRIC_E
}CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT;

/*
 * typedef: struct CPSS_DXCH_PORT_AP_PARAMS_STC
 *
 * Description: Structure for configuring AP special parameters and advertisment
 *              options on port
 *
 * Fields:
 *   fcPause  - FC pause (true/false)
 *   fcAsmDir - FC assymetric direction (Annex 28B)
 *   fecSupported  - FEC ability (true/false)
 *   fecRequired   - Request link partner to enable FEC (true/false)
 *   noneceDisable - GT_TRUE - disable ceed detection (usefull when run AP in loopback)
 *                  GT_FALSE - choose one of sides to be ceed according to protocol
 *   laneNum  - number of serdes lane of port where to run AP
 *   modesAdvertiseArr - array of pairs of (interface mode;speed) advertised by 
 *                          port for AP negotiation
 *                          last entry must be
 *                          {CPSS_PORT_INTERFACE_MODE_NA_E, CPSS_PORT_SPEED_NA_E}
 *
 */
typedef struct
{
    GT_BOOL             fcPause;
    CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT fcAsmDir;
    GT_BOOL             fecSupported;
    GT_BOOL             fecRequired;
    GT_BOOL             noneceDisable;
    GT_U32              laneNum;
    CPSS_PORT_MODE_SPEED_STC modesAdvertiseArr
                                [CPSS_DXCH_PORT_AP_IF_ARRAY_SIZE_CNS];
}CPSS_DXCH_PORT_AP_PARAMS_STC;

/*
 * typedef: struct CPSS_DXCH_PORT_AP_STATUS_STC
 *
 * Description: Structure for AP resolution result
 *
 * Fields:
 *  postApPortNum - AP resolved port number (lane swap result)
 *  portMode      - port (interface mode;speed) - result of AP auto-negotiation
 *  hcdFound      - indicating AP succeeded to find highest common denominator
 *                  with partner
 *  fecEnabled    - FEC enabled
 *  fcRxPauseEn   - enable RX flow control pause frames
 *  fcTxPauseEn   - enable TX flow control pause frames
 *
 */
typedef struct
{
    GT_U32                   postApPortNum;
    CPSS_PORT_MODE_SPEED_STC portMode;
    GT_BOOL                  hcdFound;
    GT_BOOL                  fecEnabled;
    GT_BOOL                  fcRxPauseEn;
    GT_BOOL                  fcTxPauseEn;
}CPSS_DXCH_PORT_AP_STATUS_STC;


/*
 * AP statistics information:
 *    abilityCnt        - Number of Ability detect intervals executed
 *    abilitySuccessCnt - Number of Ability detect successfull intervals executed
 *    linkFailCnt       - Number of Link check fail intervals executed
 *    linkSuccessCnt    - Number of Link check successfull intervals executed
 *    hcdResoultionTime - Time duration for HCD resolution
 *    linkUpTime        - Time duration for Link up
*/
typedef struct
{
    GT_U16 txDisCnt;          /* Number of Tx Disable intervals executed */
    GT_U16 abilityCnt;        /* Number of Ability detect intervals executed */
    GT_U16 abilitySuccessCnt; /* Number of Ability detect successfull intervals executed */
    GT_U16 linkFailCnt;       /* Number of Link check fail intervals executed */
    GT_U16 linkSuccessCnt;    /* Number of Link check successfull intervals executed */
    GT_U32 hcdResoultionTime; /* Time duration for HCD resolution */
    GT_U32 linkUpTime;        /* Time duration for Link up */

}CPSS_DXCH_PORT_AP_STATS_STC;


/*
* AP introp information:
*    attrBitMask            - Bit mask for attribute configuration:
*                             0x1 - TX Disable,....,0x80 - PD_Max Interval
*    txDisDuration          - Tx Disable duration in msec - default 60msec
*    abilityDuration        - Ability detect duration in msec - default 5 msec
*    abilityMaxInterval     - Ability detect max intervals - default 25 intervals ==> 5msec x 25 intervals = 125msec
*    abilityFailMaxInterval - Ability detect max number of failed intervals where ST_AN_GOOD_CK was detected
*                             But not resolution was found, trigger move to INIT state, instead of TX Disable in normal case
*    apLinkDuration         - AP Link check duration in msec - default 10 msec
*    apLinkMaxInterval      - AP Link check max intervals - default 50 intervals ==> 10msec x 50 intervals = 500msec
*    pdLinkDuration         - PD Link check duration in msec - default 10 msec
*    pdLinkMaxInterval      - PD Link check max intervals - default 25 intervals ==> 10msec x 25 intervals = 250msec
*/
typedef struct
{
    GT_U16 attrBitMask;
    GT_U16 txDisDuration;
    GT_U16 abilityDuration;
    GT_U16 abilityMaxInterval;
    GT_U16 abilityFailMaxInterval;
    GT_U16 apLinkDuration;
    GT_U16 apLinkMaxInterval;
    GT_U16 pdLinkDuration;
    GT_U16 pdLinkMaxInterval;
}CPSS_DXCH_PORT_AP_INTROP_STC;

/*******************************************************************************
* cpssDxChPortApEnableSet
*
* DESCRIPTION:
*       Enable/disable AP engine (loads AP code into shared memory and starts AP
*       engine).
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupsBmp   - bitmap of cores where to run AP engine
*                           (0x7FFFFFF - for ALL)
*       enable  - GT_TRUE  - enable AP on port group
*                 GT_FALSE - disbale
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Pay attention: for BC2 must be engaged before port LIB init i.e. before
*                      phase1 init!
*
*******************************************************************************/
GT_STATUS cpssDxChPortApEnableSet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_BOOL             enable
);

/*******************************************************************************
* cpssDxChPortApEnableGet
*
* DESCRIPTION:
*       Get AP engine enabled and functional on port group (local core) status.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core)
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - AP enabled and functional on port group
*                    GT_FALSE - disabled or not functional
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port group or device
*       GT_BAD_PTR               - enabledPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupNum,
    OUT GT_BOOL *enabledPtr
);

/*******************************************************************************
* cpssDxChPortApPortConfigSet
*
* DESCRIPTION:
*       Enable/disable AP process on port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*       apEnable    - AP enable/disable on port
*       apParamsPtr - (ptr to) AP parameters for port
*                               (NULL - for CPSS defaults).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApPortConfigSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         apEnable,
    IN  CPSS_DXCH_PORT_AP_PARAMS_STC    *apParamsPtr
);

/*******************************************************************************
* cpssDxChPortApPortConfigGet
*
* DESCRIPTION:
*       Get AP configuration of port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       apEnablePtr - AP enable/disable on port
*       apParamsPtr - (ptr to) AP parameters of port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apEnablePtr or apParamsPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApPortConfigGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_BOOL                         *apEnablePtr,
    OUT CPSS_DXCH_PORT_AP_PARAMS_STC    *apParamsPtr
);

/*******************************************************************************
* cpssDxChPortApPortStatusGet
*
* DESCRIPTION:
*       Get status of AP on port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       apStatusPtr - (ptr to) AP parameters for port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apStatusPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - AP engine not run
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApPortStatusGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_PORT_AP_STATUS_STC   *apStatusPtr
);

/*******************************************************************************
* cpssDxChPortApResolvedPortsBmpGet
*
* DESCRIPTION:
*       Get bitmap of ports on port group (local core) where AP process finished
*       with agreed for both sides resolution
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core)
*
* OUTPUTS:
*       apResolvedPortsBmpPtr  - 1's set for ports of local core where AP
*                                   resolution acheaved
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port group or device
*       GT_BAD_PTR               - apResolvedPortsBmpPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApResolvedPortsBmpGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupNum,
    OUT GT_U32  *apResolvedPortsBmpPtr
);

/*******************************************************************************
* cpssDxChPortApSetActiveMode
*
* DESCRIPTION:
*       Update port's AP active lanes according to new interface.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - number of physical port
*       ifMode    - interface mode
*       speed     - port data speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApSetActiveMode
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
);

/*******************************************************************************
* cpssDxChPortApLock
*
* DESCRIPTION:
*       Acquires lock so host and AP machine won't access the same 
*		resource at the same time.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       statePtr    - (ptr to) port state:
*                           GT_TRUE - locked by HOST - can be configured
*                           GT_FALSE - locked by AP processor - access forbidden
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_BAD_PTR           - statePtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApLock
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *statePtr
);

/*******************************************************************************
* cpssDxChPortApUnLock
*
* DESCRIPTION:
*       Releases the synchronization lock (between Host and AP machine).
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApUnLock
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
);

/*******************************************************************************
* cpssDxChPortApStatsGet
*
* DESCRIPTION:
*       Returns the AP port statistics information.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       apStatsPtr - (ptr to) AP statistics information
*       intropAbilityMaxIntervalPtr - (ptr to) Introp Ability Max Interval parameter - will help to represent number of failed HCD cycles 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apEnablePtr or apParamsPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortApStatsGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_PORT_AP_STATS_STC     *apStatsPtr,
    OUT GT_U16                          *intropAbilityMaxIntervalPtr
);

/*******************************************************************************
* cpssDxChPortApStatsReset
*
* DESCRIPTION:
*       Reset AP port statistics information
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                - on success - port not in use by AP processor
*       GT_BAD_PARAM         - on wrong port number or device
*       GT_NOT_INITIALIZED   - AP engine or library not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApStatsReset
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
);

/*******************************************************************************
* cpssDxChPortApIntropSet
*
* DESCRIPTION:
*       Set AP port introp information
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*       apIntropPtr - (ptr to) AP introp parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApIntropSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_DXCH_PORT_AP_INTROP_STC    *apIntropPtr
);

/*******************************************************************************
* cpssDxChPortApIntropGet
*
* DESCRIPTION:
*       Returns AP port introp information
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*
* OUTPUTS:
*       apIntropPtr - (ptr to) AP introp parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortApIntropGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT  CPSS_DXCH_PORT_AP_INTROP_STC    *apIntropPtr
);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortAph */

