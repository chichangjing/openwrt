/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExtMacDrv.h
*
* DESCRIPTION:
*
*       CPSS API that that will have PHY MAC extensions.
*      
*   ------------------------------------------------------------------------
*   1    cpssDxChPortSpeedSet
*   2    cpssDxChPortSpeedGet
*   3    cpssDxChPortDuplexAutoNegEnableSet    
*   4    cpssDxChPortDuplexAutoNegEnableGet
*   5    cpssDxChPortFlowCntrlAutoNegEnableSet
*   6    cpssDxChPortFlowCntrlAutoNegEnableGet
*   7    cpssDxChPortSpeedAutoNegEnableSet
*   8    cpssDxChPortSpeedAutoNegEnableGet
*   9    cpssDxChPortFlowControlEnableSet
*  10    cpssDxChPortFlowControlEnableGet
*  11    cpssDxChPortPeriodicFcEnableSet
*  12    cpssDxChPortPeriodicFcEnableGet
*  13    cpssDxChPortBackPressureEnableSet
*  14    cpssDxChPortBackPressureEnableGet
*  15    cpssDxChPortLinkStatusGet
*  16    cpssDxChPortDuplexModeSet
*  17    cpssDxChPortDuplexModeGet
*  18    cpssDxChPortEnableSet
*  19    cpssDxChPortEnableGet
*  20    cpssDxChPortExcessiveCollisionDropEnableSet
*  21    cpssDxChPortExcessiveCollisionDropEnableSet
*  22    cpssDxChPortPaddingEnableSet
*  23    cpssDxChPortPaddingEnableGet
*  24    cpssDxChPortPreambleLengthSet
*  25    cpssDxChPortPreambleLengthGet
*  26    cpssDxChPortCrcCheckEnableSet
*  27    cpssDxChPortCrcCheckEnableGet
*  28    cpssDxChPortMruSet
*  29    cpssDxChPortMruGet
*
*   ------------------------------------------------------------------------
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/
#ifndef __cpssExtMacDrvH
#define __cpssExtMacDrvH

#ifdef __cplusplus
extern "C" {
#endif

#include <cpss/generic/port/cpssPortCtrl.h> 

#define PRV_CPSS_PHY_MAC_OBJ(devNum,portNum) \
       (portNum == CPSS_CPU_PORT_NUM_CNS) ? NULL : PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portMacObjPtr

/*
 * typedef: enum CPSS_MACDRV_STAGE_ENT
 *
 * Description: Enumeration of port callback function stages 
 *
 * Enumerations:
 *    CPSS_MACDRV_STAGE_PRE_E - first position - before switch mac code run 
 *    CPSS_MACDRV_STAGE_POST_E - second position - after switch mac code run 
 */
typedef enum {
    CPSS_MACDRV_STAGE_PRE_E,
    CPSS_MACDRV_STAGE_POST_E
}CPSS_MACDRV_STAGE_ENT;


/*
 * typedef: GT_STATUS (*CPSS_MACDRV_MAC_SPEED_SET_FUNC)
 *
 * Description: defines speed set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum  - device number
 *   GT_U8                   portNum - port number
 *   CPSS_PORT_SPEED_ENT     speed   - speed value 
 *   CPSS_MACDRV_STAGE_ENT    stage  - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL                 *doPpMacConfigPtr - the parameter defines if the switch MAC 
 *                                               will be configurated  
 *   CPSS_PORT_SPEED_ENT     *switchSpeedSetPtr - speed value for switch MAC
 *   
 *  COMMENTS: (1) Speed set callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_MAC_SPEED_SET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT     speed,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT CPSS_PORT_SPEED_ENT     *switchSpeedSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_MAC_SPEED_GET_FUNC)
 *
 * Description: defines speed get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum  - device number
 *   GT_U8                   portNum - port number
 *   CPSS_MACDRV_STAGE_ENT    stage  - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   OUT CPSS_PORT_SPEED_ENT *speedPtr           - poiter to return speed value
 *   GT_BOOL                 *doPpMacConfigPtr   - the parameter defines if the switch MAC 
 *                                                  will be configurated  
 *   CPSS_PORT_SPEED_ENT     *switchSpeedSetPtr   - speed value for switch MAC
 *   
 *  COMMENTS: (2) Speed get callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_MAC_SPEED_GET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PORT_SPEED_ENT     *speedPtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT CPSS_PORT_SPEED_ENT     *switchSpeedGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_DUPLEX_AN_SET_FUNC)
 *
 * Description: defines AN duplex set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum  - device number
 *   GT_U8                   portNum - port number
 *   GT_BOOL                 state   - duplex state
 *   CPSS_MACDRV_STAGE_ENT    stage  - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL                 *doPpMacConfigPtr     - the parameter defines if the switch MAC 
 *                                                   will be configurated  
 *   CPSS_PORT_SPEED_ENT     *switchDuplexANSetPtr - duplex value for switch MAC
 *   
 *  COMMENTS: (3) Duplex AN Set callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_DUPLEX_AN_SET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                 state,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL              *  doPpMacConfigPtr, 
    OUT GT_BOOL              *  switchDuplexANSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_DUPLEX_AN_GET_FUNC)
 *
 * Description: defines AN duplex get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum  - device number
 *   GT_U8                   portNum - port number
 *   CPSS_MACDRV_STAGE_ENT    stage  - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL                 *statePtr         - pointer to return duplex value
 *   GT_BOOL                 *doPpMacConfigPtr - the parameter defines if the switch MAC 
 *                                               will be configurated  
 *   CPSS_PORT_SPEED_ENT     *switchDuplexANGetPtr  - duplex value for switch MAC
 *   
 *  COMMENTS: (4) Duplex AN Get callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_DUPLEX_AN_GET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT  GT_BOOL                 *statePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchDuplexANGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_FLOW_CNTL_AN_SET_FUNC)
 *
 * Description: defines AN flow control set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum  - device number
 *   GT_U8                   portNum - port number
 *   GT_BOOL                 state   - flow control state
 *   GT_BOOL                 pauseAdvertise  - pause advertise state
 *   CPSS_MACDRV_STAGE_ENT    stage  - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL                 *targetState           - pointer to return flow control state
 *   GT_BOOL                 *targetPauseAdvertise  - pointer to return pause advertise state
 *   GT_BOOL                 *doPpMacConfigPtr      - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *   
 *  COMMENTS: (5) Flow control AN Set
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_FLOW_CNTL_AN_SET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                 state,
    IN  GT_BOOL                 pauseAdvertise,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr,
    OUT GT_BOOL                 *targetState,
    OUT  GT_BOOL                *targetPauseAdvertise
);
/*
 *  typedef GT_STATUS (*CPSS_MACDRV_FLOW_CNTL_AN_GET_FUNC)
 *
 * Description: defines AN flow control get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum  - device number
 *   GT_U8                   portNum - port number
 *   CPSS_MACDRV_STAGE_ENT    stage  - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL                 *statePtr             - FC state pointer to return the value
 *   GT_BOOL                 *pauseAdvertisePtr    - pause advertise pointer to return the value
 *   GT_BOOL                 *targetState          - pointer to return switch MAC flow control state
 *   GT_BOOL                 *targetPauseAdvertise - pointer to return switch MAC pause advertise state
 *   GT_BOOL                 *doPpMacConfigPtr     - the parameter defines if the switch MAC 
 *                                                   will be configurated  
 *  COMMENTS: (6) Flow control AN Get
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_FLOW_CNTL_AN_GET_FUNC)
(

    IN   GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT  GT_BOOL                *statePtr,
    OUT  GT_BOOL                *pauseAdvertisePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    IN GT_BOOL                 *doPpMacConfigPtr, 
    OUT  GT_BOOL                *targetStatePtr,
    OUT  GT_BOOL                *targetPauseAdvertisePtr
);
/*
 *  typedef GT_STATUS (*CPSS_MACDRV_SPEED_AN_SET_FUNC)
 *
 * Description: defines AN speed set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum  - device number
 *   GT_U8                   portNum - port number
 *   GT_BOOL                 state   - speed AN state
 *   CPSS_MACDRV_STAGE_ENT    stage  - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL                *switchSpeedANSetPtr - AN speed pointer to value to set in switch MAC
 *   GT_BOOL                 *doPpMacConfigPtr - the parameter defines if the switch MAC 
 *                                               will be configurated  
 *
 *  COMMENTS: (7) Speed AN Set callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_SPEED_AN_SET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                 state,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchSpeedANSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_SPEED_AN_GET_FUNC)
 *
 * Description: defines AN speed get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum  - device number
 *   GT_U8                   portNum - port number
 *   GT_BOOL                 state   - speed AN state
 *   CPSS_MACDRV_STAGE_ENT    stage  - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
  *  GT_BOOL                *statePtr            - pointer to return value
 *   GT_BOOL                *switchSpeedANGetPtr - AN speed pointer to value from switch MAC
 *   GT_BOOL                 *doPpMacConfigPtr - the parameter defines if the switch MAC 
 *                                               will be configurated  
 *
 *  COMMENTS: (8) Speed AN Get callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_SPEED_AN_GET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT  GT_BOOL                *statePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchSpeedANGetPtr 
);
/*
 *  typedef GT_STATUS (*CPSS_MACDRV_FC_ENABLE_SET_FUNC)
 *
 * Description: defines AN FC set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_PORT_FLOW_CONTROL_ENT  state - AN FC state
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL                *switchFlowCntlSetPtr   - AN FC pointer to value for switch MAC
 *   GT_BOOL                *doPpMacConfigPtr       - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (9)  Flow control Enable Set callback function 
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_FC_ENABLE_SET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT CPSS_PORT_FLOW_CONTROL_ENT  *switchFlowCntlSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_FC_ENABLE_GET_FUNC)
 *
 * Description: defines AN FC get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   CPSS_PORT_FLOW_CONTROL_ENT  statePtr           - AN FC state pointer to return value
 *   GT_BOOL                *switchFlowCntlGetPtr   - AN FC pointer to value from switch MAC
 *   GT_BOOL                 *doPpMacConfigPtr      - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (10)  Flow control Enable Get callback function 
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_FC_ENABLE_GET_FUNC)
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *statePtr,
    IN CPSS_MACDRV_STAGE_ENT            stage,  
    OUT GT_BOOL                         *doPpMacConfigPtr, 
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *switchFlowCntlGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PERIODIC_FC_ENABLE_SET_FUNC)
 *
 * Description: defines periodic FC enable set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   GT_BOOL                  enable   - enable/disable value
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *switchPeriodicFlowCntlSetPtr  - periodic FC pointer to value from switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (11)  Periodic Flow control Enable Set
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PERIODIC_FC_ENABLE_SET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                     enable,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchPeriodicFlowCntlSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PERIODIC_FC_ENABLE_GET_FUNC)
 *
 * Description: defines periodic FC enable get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *enablePtr                     - enable/disable return value pointer
 *   GT_BOOL         *switchPereodicFlowCntlGetPtr  - periodic FC pointer to value of switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (12) Periodic Flow control Enable Get callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PERIODIC_FC_ENABLE_GET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                     *enablePtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchPereodicFlowCntlGetPtr 
);
/*
 *  typedef GT_STATUS (*CPSS_MACDRV_BP_ENABLE_SET_FUNC)
 *
 * Description: defines back pressure enable set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   GT_BOOL                 state     - enable/disable value
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *switchBPGetPtr                - AN speed pointer to value from switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (13)  BackPressure Enable Set callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_BP_ENABLE_SET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                     state,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchBPSetPtr 
);
/*
 *  typedef GT_STATUS (*CPSS_MACDRV_BP_ENABLE_GET_FUNC)
 *
 * Description: defines Back pressure enable/disable get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *statePtr                     - enable/disable return value pointer
 *   GT_BOOL         *switchBPGetPtr                - AN speed pointer to value from switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (14) Back pressure enable/disable Get callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_BP_ENABLE_GET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                     *statePtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchBPGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PORT_LINK_STATUS_GET_FUNC)
 *
 * Description: defines link status get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *isLinkUpPtr                   - link status return value pointer
 *   GT_BOOL         *switchLinkStatusGetPtr        - link status pointer to value of switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (15) Port Link Status Get callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PORT_LINK_STATUS_GET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL                    *isLinkUpPtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchLinkStatusGetPtr 
);
/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PORT_DUPLEX_SET_FUNC)
 *
 * Description: defines port duplex status set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_PORT_DUPLEX_ENT    dMode     - duplex mode
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *switchDuplexSetPtr            - link status pointer to value from switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (16) Set Port Duplex Mode callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PORT_DUPLEX_SET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_DUPLEX_ENT        dMode,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT CPSS_PORT_DUPLEX_ENT        *switchDuplexSetPtr 
);
/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PORT_DUPLEX_GET_FUNC)
 *
 * Description: defines port duplex status get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   CPSS_PORT_DUPLEX_ENT    *dModePtr              - pointer to duplex mode value 
 *   GT_BOOL         *switchDuplexGetPtr            - duplex mode pointer to value from switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (17) Get Port Duplex Mode callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PORT_DUPLEX_GET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PORT_DUPLEX_ENT        *dModePtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT CPSS_PORT_DUPLEX_ENT        *switchDuplexGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PORT_ENABLE_SET_FUNC)
 *
 * Description: defines port enable/disable set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   GT_BOOL                 enable    - enable/disable state
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *targetEnableSetPtr            - port status pointer to value for switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (18) Set Port enable/disable callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PORT_ENABLE_SET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                     enable,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *targetEnableSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PORT_ENABLE_GET_FUNC)
 *
 * Description: defines port enable/disable set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *statePtr                      - state pointer 
 *   GT_BOOL         *switchPortEnableGetPtr        - port status pointer to value for switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (19) Get Port enable/disable callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PORT_ENABLE_GET_FUNC)
(
    IN   GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT  GT_BOOL               *statePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchPortEnableGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_EXCL_COL_DROP_ENABLE_SET_FUNC)
 *
 * Description: defines port excessive collisions drop set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   GT_BOOL                 enable    - enable/disable state
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *switchExcColDropSetPtr        - port excessive collisions drop
 *                                                    pointer to value for switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (20) Set Port Excessive Collisions Drop Enable callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_EXCL_COL_DROP_ENABLE_SET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                 enable,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchExcColDropSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_EXCL_COL_DROP_ENABLE_GET_FUNC)
 *
 * Description: defines port excessive collisions drop get callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *enablePtr                     - pointer to return value
 *   GT_BOOL         *switchExcColDropGetPtr        - port excessive collisions drop
 *                                                    pointer to value for switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (21) Get Port Excessive Collisions Drop Enable callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_EXCL_COL_DROP_ENABLE_GET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL                 *enablePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchExcColDropGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PADDING_ENABLE_SET_FUNC)
 *
 * Description: defines padding enable/disable set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   GT_BOOL                 enable    - set value
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *switchPaddingSetPtr           - padding enable/disable pointer
 *                                                    to value for switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (22) Set Padding Enable callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PADDING_ENABLE_SET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                 enable,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchPaddingSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PADDING_ENABLE_SET_FUNC)
 *
 * Description: defines padding enable/disable set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                   devNum    - device number
 *   GT_U8                   portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         * enablePtr                    - pointer to return value
 *   GT_BOOL         *switchPaddingGetPtr           - padding enable/disable pointer
 *                                                    to value for switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (23) Get padding enable callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PADDING_ENABLE_GET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL                 *enablePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchPaddingGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PREAMBLE_LENGTH_SET_FUNC)
 *
 * Description: defines padding enable/disable set callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                    devNum    - device number
 *   GT_U8                    portNum   - port number
 *   CPSS_PORT_DIRECTION_ENT  direction - Rx or Tx or Both 
 *   GT_U32                   length    - value
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   CPSS_PORT_DIRECTION_ENT *targetDirection       - pointer to return value
 *
 *   GT_BOOL         *switchPreambleLengthPtr       - preamble length pointer
 *                                                    to value for switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (24) Set Preample length callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PREAMBLE_LENGTH_SET_FUNC)
(
    IN GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT CPSS_PORT_DIRECTION_ENT *targetDirection,
    OUT GT_U32                 *switchPreambleLengthPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_PREAMBLE_LENGTH_GET_FUNC)
 *
 * Description: defines get preamble length callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                    devNum    - device number
 *   GT_U8                    portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   CPSS_PORT_DIRECTION_ENT  *direction           - pointer to direction value 
 *   GT_U32                   *length              - pointer to preamble length value
 *
 *   CPSS_PORT_DIRECTION_ENT *targetDirection       - pointer to switch MAC value
 *
 *   GT_BOOL         *switchPreambleLengthPtr       - pointer to switch MAC preamble length
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (25) Get Preample length callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_PREAMBLE_LENGTH_GET_FUNC)
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_DIRECTION_ENT     direction,
    OUT GT_U32                      *lengthPtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr,
    OUT CPSS_PORT_DIRECTION_ENT     *targetDirection,
    OUT GT_U32                      *switchPreambleLengthPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_CRC_CHECK_ENABLE_SET_FUNC)
 *
 * Description: defines set CRC callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                    devNum    - device number
 *   GT_U8                    portNum   - port number
 *   GT_BOOL                  enable    - set value
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *switchCRCCheckSetPtr          - CRC value pointer
 *                                                    to switch MAC value
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (26) Set CRC check enable callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_CRC_CHECK_ENABLE_SET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                 enable,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchCRCCheckSetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_CRC_CHECK_ENABLE_GET_FUNC)
 *
 * Description: defines get CRC callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                    devNum    - device number
 *   GT_U8                    portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *enablePtr                     - value pointer
 *   GT_BOOL         *switchCRCCheckGetPtr          - CRC value pointer for switch MAC value
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (27) Get CRC check enable callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_CRC_CHECK_ENABLE_GET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL                 *enablePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchCRCCheckGetPtr 
);

/*
 *  typedef GT_STATUS (*CPSS_MACDRV_MRU_SET_FUNC)
 *
 * Description: defines set MRU callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                    devNum    - device number
 *   GT_U8                    portNum   - port number
 *   GT_U32                   mruSize   - MRU size
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_BOOL         *switchMRUSetPtr               - switch MAC MRU size pointer
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (28) Set MRU callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_MRU_SET_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U32                  mruSize,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_U32                 *switchMRUSetPtr 
);


/*
 *  typedef GT_STATUS (*CPSS_MACDRV_MRU_GET_FUNC)
 *
 * Description: defines get MRU size callback for MACPHY object
 *
 * INPUTS:
 *   GT_U8                    devNum    - device number
 *   GT_U8                    portNum   - port number
 *   CPSS_MACDRV_STAGE_ENT    stage    - stage for callback run (PRE or POST)
 *
 * OUTPUTS:
 *   GT_U32          *mruSizePtr                    - MRU size value pointer
 *
 *   GT_BOOL         *switchPreambleLengthPtr       - preamble length pointer
 *                                                    to value for switch MAC
 *   GT_BOOL         *doPpMacConfigPtr              - the parameter defines if the switch MAC 
 *                                                    will be configurated  
 *
 *  COMMENTS: (29) Get MRU callback function
 *
 */
typedef GT_STATUS (*CPSS_MACDRV_MRU_GET_FUNC)
(
    IN   GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT  GT_U32                 *mruSizePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_U32                 *switchMRUGetPtr 
);


/*
 *  typedef: struct CPSS_MACDRV_OBJ_STC
 *
 * Description: the structure holds the "MAC PHY driver interface" functions 
 *
 *  COMMENTS: 
 *  ----------------------------------------------------------------------------------------
 *             API                                             MACPHY function
 *------------------------------------------------------------------------------------------
 *   1    cpssDxChPortSpeedSet                               macDrvMacSpeedSetFunc
 *   2    cpssDxChPortSpeedGet                               macDrvMacSpeedGetFunc
 *   3    cpssDxChPortDuplexAutoNegEnableSet                 macDrvMacDuplexANSetFunc
 *   4    cpssDxChPortDuplexAutoNegEnableGet                 macDrvMacDuplexANGetFunc
 *   5    cpssDxChPortFlowCntrlAutoNegEnableSet              macDrvMacFlowCntlANSetFunc
 *   6    cpssDxChPortFlowCntrlAutoNegEnableGet              macDrvMacFlowCntlANGetFunc
 *   7    cpssDxChPortSpeedAutoNegEnableSet                  macDrvMacSpeedANSetFunc
 *   8    cpssDxChPortSpeedAutoNegEnableGet                  macDrvMacSpeedANGetFunc
 *   9    cpssDxChPortFlowControlEnableSet                   macDrvMacFlowCntlSetFunc
 *  10    cpssDxChPortFlowControlEnableGet                   macDrvMacFlowCntlGetFunc
 *  11    cpssDxChPortPeriodicFcEnableSet                    macDrvMacPeriodFlowCntlSetFunc
 *  12    cpssDxChPortPeriodicFcEnableGet                    macDrvMacPeriodFlowCntlGetFunc
 *  13    cpssDxChPortBackPressureEnableSet                  macDrvMacBackPrSetFunc
 *  14    cpssDxChPortBackPressureEnableGet                  macDrvMacBackPrGetFunc
 *  15    cpssDxChPortLinkStatusGet                          macDrvMacPortlinkGetFunc
 *  16    cpssDxChPortDuplexModeSet                          macDrvMacDuplexSetFunc
 *  17    cpssDxChPortDuplexModeGet                          macDrvMacDuplexGetFunc
 *  18    cpssDxChPortEnableSet                              macDrvMacPortEnableSetFunc
 *  19    cpssDxChPortEnableGet                              macDrvMacPortEnableGetFunc
 *  20    cpssDxChPortExcessiveCollisionDropEnableSet        macDrvMacExcessiveCollisionDropSetFunc
 *  21    cpssDxChPortExcessiveCollisionDropEnableSet        macDrvMacExcessiveCollisionDropGetFunc
 *  22    cpssDxChPortPaddingEnableSet                       macDrvMacPaddingEnableSetFunc
 *  23    cpssDxChPortPaddingEnableGet                       macDrvMacPaddingEnableGetFunc
 *  24    cpssDxChPortPreambleLengthSet                      macDrvMacPreambleLengthSetFunc
 *  25    cpssDxChPortPreambleLengthGet                      macDrvMacPreambleLengthGetFunc
 *  26    cpssDxChPortCrcCheckEnableSet                      macDrvMacCRCCheckSetFunc
 *  27    cpssDxChPortCrcCheckEnableGet                      macDrvMacCRCCheckGetFunc
 *  28    cpssDxChPortMruSet                                 macDrvMacMRUSetFunc
 *  29    cpssDxChPortMruGet                                 macDrvMacMRUGetFunc
 *---------------------------------------------------------------------------------------------
 */
typedef struct{
    CPSS_MACDRV_MAC_SPEED_SET_FUNC              macDrvMacSpeedSetFunc;       /* 1 */
    CPSS_MACDRV_MAC_SPEED_GET_FUNC              macDrvMacSpeedGetFunc;       /* 2 */
    CPSS_MACDRV_DUPLEX_AN_SET_FUNC              macDrvMacDuplexANSetFunc;    /* 3 */
    CPSS_MACDRV_DUPLEX_AN_GET_FUNC              macDrvMacDuplexANGetFunc;    /* 4 */
    CPSS_MACDRV_FLOW_CNTL_AN_SET_FUNC           macDrvMacFlowCntlANSetFunc;  /* 5 */
    CPSS_MACDRV_FLOW_CNTL_AN_GET_FUNC           macDrvMacFlowCntlANGetFunc;  /* 6 */
    CPSS_MACDRV_SPEED_AN_SET_FUNC               macDrvMacSpeedANSetFunc;     /* 7 */
    CPSS_MACDRV_SPEED_AN_GET_FUNC               macDrvMacSpeedANGetFunc;     /* 8 */
    CPSS_MACDRV_FC_ENABLE_SET_FUNC              macDrvMacFlowCntlSetFunc;    /* 9 */
    CPSS_MACDRV_FC_ENABLE_GET_FUNC              macDrvMacFlowCntlGetFunc;    /* 10 */
    CPSS_MACDRV_PERIODIC_FC_ENABLE_SET_FUNC     macDrvMacPeriodFlowCntlSetFunc;    /* 11 */
    CPSS_MACDRV_PERIODIC_FC_ENABLE_GET_FUNC     macDrvMacPeriodFlowCntlGetFunc;    /* 12 */
    CPSS_MACDRV_BP_ENABLE_SET_FUNC              macDrvMacBackPrSetFunc;     /* 13 */
    CPSS_MACDRV_BP_ENABLE_GET_FUNC              macDrvMacBackPrGetFunc;     /* 14 */
    CPSS_MACDRV_PORT_LINK_STATUS_GET_FUNC       macDrvMacPortlinkGetFunc;     /* 15 */
    CPSS_MACDRV_PORT_DUPLEX_SET_FUNC            macDrvMacDuplexSetFunc;     /* 16 */
    CPSS_MACDRV_PORT_DUPLEX_GET_FUNC            macDrvMacDuplexGetFunc;     /* 17 */
    CPSS_MACDRV_PORT_ENABLE_SET_FUNC            macDrvMacPortEnableSetFunc;     /* 18 */
    CPSS_MACDRV_PORT_ENABLE_GET_FUNC            macDrvMacPortEnableGetFunc;     /* 19 */
    CPSS_MACDRV_EXCL_COL_DROP_ENABLE_SET_FUNC   macDrvMacExcessiveCollisionDropSetFunc;  /* 20 */
    CPSS_MACDRV_EXCL_COL_DROP_ENABLE_GET_FUNC   macDrvMacExcessiveCollisionDropGetFunc;  /* 21 */
    CPSS_MACDRV_PADDING_ENABLE_SET_FUNC         macDrvMacPaddingEnableSetFunc;     /* 22 */
    CPSS_MACDRV_PADDING_ENABLE_GET_FUNC         macDrvMacPaddingEnableGetFunc;     /* 23 */
    CPSS_MACDRV_PREAMBLE_LENGTH_SET_FUNC        macDrvMacPreambleLengthSetFunc;     /* 24 */
    CPSS_MACDRV_PREAMBLE_LENGTH_GET_FUNC        macDrvMacPreambleLengthGetFunc;     /* 25 */
    CPSS_MACDRV_CRC_CHECK_ENABLE_SET_FUNC       macDrvMacCRCCheckSetFunc;        /* 26 */
    CPSS_MACDRV_CRC_CHECK_ENABLE_GET_FUNC       macDrvMacCRCCheckGetFunc;        /* 27 */
    CPSS_MACDRV_MRU_SET_FUNC                    macDrvMacMRUSetFunc;        /* 28 */
    CPSS_MACDRV_MRU_GET_FUNC                    macDrvMacMRUGetFunc;        /* 29 */

} CPSS_MACDRV_OBJ_STC;


#ifdef __cplusplus
}
#endif


#endif  /* __cpssExtMacDrvH */


