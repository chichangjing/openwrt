/*******************************************************************************
*              (c), Copyright 2010, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* phyMacInit.c
*
* DESCRIPTION:
*   The file includes:
*     I. PHYMAC object init functions: 
*       - macDrv1540MInit
*       - macPhyDrvDataBaseInit
*   
*     II. PHYMAC object member functions:
*        - macPhySpeedSet;                     (1)
*        - macPhySpeedGet;                     (2)
*        - macPhyDuplexANSet;                  (3)    - in 1540 MACSEC only full duplex used 
*        - macPhyDuplexANGet;                  (4)    - in 1540 MACSEC only full duplex used 
*        - macPhyFlowCntlANSet;                (5) 
*        - macPhyFlowCntlANGet;                (6)
*        - macPhySpeedANSet;                   (7)    
*        - macPhySpeedANGet;                   (8)
*        - macPhyFlowControlEnableSet;         (9)
*        - macPhyFlowControlEnableGet;         (10)
*        - macPhyPeriodicFlowCntlSet;          (11)
*        - macPhyPeriodicFlowCntlGet;          (12)
*        - macPhyBackPressureSet;              (13)    - in 1540 MACSEC only full duplex used 
*        - macPhyBackPressureGet;              (14)    - in 1540 MACSEC only full duplex used 
*        - macPhyPortLinkGet;                  (15)
*        - macPhyDuplexSet;                    (16)    - in 1540 MACSEC only full duplex used 
*        - macPhyDuplexGet;                    (17)    - in 1540 MACSEC only full duplex used 
*        - macPhyPortEnableSet;                (18)
*        - macPhyPortEnableGet;                (19)
*        - macPhyExcessiveCollisionDropSet;    (20)
*        - macPhyExcessiveCollisionDropGet;    (21)
*        - macPhyPaddingEnableSet;             (22)
*        - macPhyPaddingEnableGet;             (23)
*        - macPhyPreambleLengthSet;            (24)   (??? no set field)
*        - macPhyPreambleLengthGet;            (25)   (??? no set field) 
*        - macPhyCRCCheckSet;                  (26)
*        - macPhyCRCCheckGet;                  (27)
*        - macPhyMRUSet;                       (28)
*        - macPhyMRUGet;                       (29)
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#include <cpss/generic/extMac/cpssExtMacDrv.h> 
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h> 
#include <gtOs/gtGenTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h> 
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h> 

#include <extPhyM/phyMacInit.h> 
#include <extPhyM/phyMacMSec.h> 
#include <extPhyM/phyMacRegs1540M.h> 
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>


#define PHYMAC_DBG_ERROR  osPrintf

int osPrintf(const char* format, ...);

/* creation of pointer to macPhy1540MObj object*/
CPSS_MACDRV_OBJ_STC * macPhy1540MObj = NULL;

CPSS_SMI_REGISTER_WRITE_FUNC  macPhy1540MSmiWriteRegister = NULL;
CPSS_SMI_REGISTER_READ_FUNC  macPhy1540MSmiReadRegister = NULL;

CPSS_SMI_START_TRANSACTION_FUNC  macPhy1540MSmiStartTransaction = NULL;
CPSS_SMI_FINISH_TRANSACTION_FUNC  macPhy1540MSmiFinishTransaction = NULL;



phy1540MDataBase *phy1540DataBasePtr=NULL;


/**************** phyMac static functions ********************/

/*******************************************************************************
* macPhySpeedSet (1)
*
* DESCRIPTION:
*       This function sets speed to PHY SIDE MAC and SWITCH SIDE MAC of 
*       1540M PHY.
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    speed                     - speed value to set
*    stage                     - stage to run (PRE or POST)   
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set  
*    *switchSpeedSetPtr        - the value to set on switch MAC     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhySpeedSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     speed,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT CPSS_PORT_SPEED_ENT     *switchSpeedSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    CPSS_PORT_SPEED_ENT   ssSpeed;
    CPSS_PORT_SPEED_ENT   psSpeed;

    /* speed can be defined as 10, 100 or 1000 for 1540 */
    if (speed > CPSS_PORT_SPEED_1000_E) 
    {
        return GT_BAD_PARAM;
    }
    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set speed value for SS and PS MACs */
        ssSpeed = speed;
        psSpeed = speed;

        /* set both MACs */
        rc = phyMacMSecSetSpeed(devNum,portNum,
                      ssSpeed,psSpeed, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedSet \n");
            return rc;
        }
        /* set the same value in switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchSpeedSetPtr = speed; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhySpeedGet (2)
*
* DESCRIPTION:
*       This function returns the speed of PHY SIDE MAC from 
*       1540M PHY.
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    *speed                    - speed value 
*    stage                     - stage to run (PRE or POST)   
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set  
*    *switchSpeedSetPtr        - not used     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhySpeedGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_PORT_SPEED_ENT     *speedPtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT CPSS_PORT_SPEED_ENT     *switchSpeedGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    CPSS_PORT_SPEED_ENT   ssSpeed;
    CPSS_PORT_SPEED_ENT   psSpeed;
    

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecGetSpeed(devNum,portNum,
                      &ssSpeed,&psSpeed, PHYMAC_PHY_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedGet \n");
            return rc;
        }
        /* value in switch MAC does not matter */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchSpeedGetPtr = psSpeed; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyDuplexANSet (3)
*
* DESCRIPTION:
*       This function sets duplex status (half or full) for Auto Negotiation 
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    *duplexState              - duplex value 
*    stage                     - stage to run (PRE or POST)   
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set  
*    *switchSpeedSetPtr        - not used     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyDuplexANSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 state,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL              *  doPpMacConfigPtr, 
    OUT GT_BOOL              *  switchDuplexANSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState = state;
    GT_BOOL   psState = state;


    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecSetANDuplex(devNum,portNum,
                      ssState,psState, PHYMAC_PHY_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedGet \n");
            return rc;
        }
        /* value in switch MAC does not matter */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchDuplexANSetPtr = state; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}
/*******************************************************************************
* macPhyDuplexANGet (4)
*
* DESCRIPTION:
*       This function returns Phy Side MAC duplex status (half or full) for Auto Negotiation 
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    duplexState               - speed value 
*    stage                     - stage to run (PRE or POST)   
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set  
*    *switchSpeedSetPtr        - not used     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyDuplexANGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 *statePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchDuplexANGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState;
    GT_BOOL   psState;

    /***** get duplex mode from switch ***/
    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* get duplex values from both MACs */
        rc = phyMacMSecGetANDuplex(devNum,portNum,
                      &ssState,&psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedGet rc - %d \n",rc);
            return rc;
        }
        /* no read the duplex state from switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchDuplexANGetPtr = psState; 

        /* set the return value as the phy side duplex value */
        *statePtr = psState;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}
/*******************************************************************************
* macPhyFlowCntlANSet (5)
*
* DESCRIPTION:
*       This function sets flow control status for Auto Negotiation 
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    state                     - Flow control AN enable/disable 
*    pauseAdvertise            - Advertise symmetric flow control support in
*                                Auto-Negotiation. 0 = Disable, 1 = Enable.
*    stage                     - stage to run (PRE or POST)   
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set  
*    targetState               - Flow control AN enable/disable  for switch MAC
*    targetPauseAdvertise      - Advertise symmetric flow control support in
*                                Auto-Negotiation. 0 = Disable, 1 = Enable for
*                                switch MAC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyFlowCntlANSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 state,
    IN  GT_BOOL                 pauseAdvertise,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr,
    OUT GT_BOOL                 *targetState,
    OUT GT_BOOL                 *targetPauseAdvertise
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState = state;
    GT_BOOL   ssPauseAdvertise = pauseAdvertise;
    GT_BOOL   psState = state;
    GT_BOOL   psPauseAdvertise = pauseAdvertise;

    /*  set flow control for SS & PS MACs */
    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set flow control values to both MACs */
        rc = phyMacMSecSetANFlowControl(devNum,portNum,
                      ssState,ssPauseAdvertise,
                      psState,psPauseAdvertise, 
                      PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedGet \n");
            return rc;
        }
        /*  set flow control for Switch MACs */
        *targetState = state;
        *targetPauseAdvertise = pauseAdvertise;

        /* switch MAC should be configured */
        *doPpMacConfigPtr = GT_TRUE; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyFlowCntlANGet (6)
*
* DESCRIPTION:
*       This function sets flow control status for Auto Negotiation 
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*                                Auto-Negotiation. 0 = Disable, 1 = Enable.
*    stage                     - stage to run (PRE or POST)   
*    targetState               - Flow control AN enable/disable  for switch MAC
*
* OUTPUTS:
*    state                     - Flow control AN enable/disable 
*    pauseAdvertise            - Advertise symmetric flow control support in
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set  
*    targetPauseAdvertise      - Advertise symmetric flow control support in
*                                Auto-Negotiation. 0 = Disable, 1 = Enable for
*                                switch MAC
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyFlowCntlANGet
(

    IN   GT_U8                  devNum,
    IN   GT_PHYSICAL_PORT_NUM    portNum,
    OUT  GT_BOOL                *statePtr,
    OUT  GT_BOOL                *pauseAdvertisePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT  GT_BOOL                *targetStatePtr,
    OUT  GT_BOOL                *switchFlowCntlANGetPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState;
    GT_BOOL   ssPauseAdvertisePtr;
    GT_BOOL   psState;
    GT_BOOL   psPauseAdvertisePtr;

    /*  get flow control from  PS MACs */
    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* get flow control values from PS MAC */
        rc = phyMacMSecGetANFlowControl(devNum,portNum,
                      &ssState,&ssPauseAdvertisePtr,
                      &psState,&psPauseAdvertisePtr,
                      PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyFlowCntlANGet \n");
            return rc;
        }

        /* get switch MAC values*/
        *switchFlowCntlANGetPtr = GT_TRUE;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}
/*******************************************************************************
* macPhySpeedANSet (7)
*
* DESCRIPTION:
*       This function enables/disables speed Auto Negotiation on SS and PS MACs 
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*                                Auto-Negotiation. 0 = Disable, 1 = Enable.
*    stage                     - stage to run (PRE or POST)   
*    targetState               - Flow control AN enable/disable  for switch MAC
*
* OUTPUTS:
*    state                     - Flow control AN enable/disable 
*    pauseAdvertise            - Advertise symmetric flow control support in
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set  
*    targetPauseAdvertise      - Advertise symmetric flow control support in
*                                Auto-Negotiation. 0 = Disable, 1 = Enable for
*                                switch MAC
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhySpeedANSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 state,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchSpeedANSetPtr 
)
{
    GT_STATUS rc = GT_OK;

    GT_BOOL   ssState = state;
    GT_BOOL   psState = state;

    /*  set speed AN enable/disable for SS & PS MACs */
    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* get flow control values from PS MAC */
        rc = phyMacMSecSetSpeedANEnable(devNum,portNum,
                      ssState,psState,
                      PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedANSet - rc %d \n",rc);
            return rc;
        }

        /* set speed AN enable/disable on switch MAC */
        *doPpMacConfigPtr = GT_TRUE;
        *switchSpeedANSetPtr = state;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}
/*******************************************************************************
* macPhySpeedANGet (8)
*
* DESCRIPTION:
*       This function returns status of Auto-Negotiation enable on specified port 
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    stage                     - stage to run (PRE or POST)   
*    state               - Flow control AN enable/disable  for switch MAC
*
* OUTPUTS:
*    state                     - Flow control AN enable/disable 
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set  
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhySpeedANGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT  GT_BOOL                *statePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchSpeedANGetPtr 
)
{
    GT_STATUS rc = GT_OK;

    GT_BOOL   ssState;
    GT_BOOL   psState;

    /*  get speed AN enable/disable for SS & PS MACs */
    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* get  speed AN values from PS & SS MAC */
        rc = phyMacMSecGetSpeedANEnable(devNum,portNum,
                      &ssState,&psState,
                      PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedANSet - rc %d \n",rc);
            return rc;
        }

        /* get speed AN value from  switch MAC */
        *doPpMacConfigPtr = GT_TRUE;
        *switchSpeedANGetPtr = psState;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }

    return GT_OK;
}

/*******************************************************************************
* macPhyFlowControlEnableSet (9)
*
* DESCRIPTION:
*       This function sets flow control mode on SS and PS MACs 
*       The following Flow control medes are defined:
*           typedef enum
*           {
*              CPSS_PORT_FLOW_CONTROL_DISABLE_E = GT_FALSE,
*              CPSS_PORT_FLOW_CONTROL_RX_TX_E = GT_TRUE,
*              CPSS_PORT_FLOW_CONTROL_RX_ONLY_E,  
*              CPSS_PORT_FLOW_CONTROL_TX_ONLY_E
*            }CPSS_PORT_FLOW_CONTROL_ENT;
*
*
* INPUTS:
*    devNum                            - device number    
*    portNum                           - port number
*    state                             - flow control mode
*    stage                             - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchFlowCntlSetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT CPSS_PORT_FLOW_CONTROL_ENT  *switchFlowCntlSetPtr 
)
{
    GT_STATUS rc = GT_OK;

    CPSS_PORT_FLOW_CONTROL_ENT  ssState = state;
    CPSS_PORT_FLOW_CONTROL_ENT  psState = state;

    /*  set flow control mode for SS & PS MACs */
    if (stage == CPSS_MACDRV_STAGE_PRE_E) {

        /* set flow control mode for PS & SS MAC */
        rc = phyMacMSecSetFlowControlEnable(devNum,portNum,
                      ssState,psState,
                      PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedANSet - rc %d \n",rc);
            return rc;
        }

        /* set speed AN enable/disable on switch MAC */
        *doPpMacConfigPtr = GT_TRUE;
        *switchFlowCntlSetPtr = state;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}


/*******************************************************************************
* macPhyFlowControlEnableGet (10)
*
* DESCRIPTION:
*       This function gets flow control mode from SS and PS MACs 
*
*       The following typedef defines the flow control medes:
*
*           typedef enum
*           {
*              CPSS_PORT_FLOW_CONTROL_DISABLE_E = GT_FALSE,
*              CPSS_PORT_FLOW_CONTROL_RX_TX_E = GT_TRUE,
*              CPSS_PORT_FLOW_CONTROL_RX_ONLY_E,  
*              CPSS_PORT_FLOW_CONTROL_TX_ONLY_E
*            }CPSS_PORT_FLOW_CONTROL_ENT;
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    state                     - flow control mode
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *statePtr                 - ptr to required value
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchFlowCntlGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyFlowControlEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *statePtr,
    IN CPSS_MACDRV_STAGE_ENT            stage,  
    OUT GT_BOOL                         *doPpMacConfigPtr, 
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *switchFlowCntlGetPtr 
)
{
    GT_STATUS rc = GT_OK;

    CPSS_PORT_FLOW_CONTROL_ENT  ssState;
    CPSS_PORT_FLOW_CONTROL_ENT  psState;

    /*  set flow control mode for SS & PS MACs */
    if (stage == CPSS_MACDRV_STAGE_PRE_E) {

        /* set flow control mode for PS & SS MAC */
        rc = phyMacMSecGetFlowControlEnable(devNum,portNum,
                      &ssState,&psState,
                      PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedANSet - rc %d \n",rc);
            return rc;
        }

        /* return Phy Side MAC value */
        *statePtr = psState;

        /* not need flow control mode from switch MAC */
        *doPpMacConfigPtr = GT_TRUE;
        *switchFlowCntlGetPtr = psState;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyPeriodicFlowCntlSet (11)
*
* DESCRIPTION:
*       This function gets flow control mode from SS and PS MACs 
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    state                     - flow control mode
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *statePtr                 - ptr to required value
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchFlowCntlGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPeriodicFlowCntlSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_BOOL                     state,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchPereodicFlowCntlSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState = state;
    GT_BOOL   psState = state;


    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecSetPeriodicFlowControlEnable(devNum,portNum,
                      ssState,psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyPeriodFlowCntlSet rc - %d \n",rc);
            return rc;
        }
        /* set the same value in switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchPereodicFlowCntlSetPtr = state; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyPeriodicFlowCntlGet (12)
*
* DESCRIPTION:
*       This function gets periodic flow control mode from SS and PS MACs 
*
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *enable                   - ptr to required value
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchFlowCntlGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPeriodicFlowCntlGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT  GT_BOOL                     *enable,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchPereodicFlowCntlGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState;
    GT_BOOL   psState;


    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecGetPeriodicFlowControlEnable(devNum,portNum,
                      &ssState,&psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyPeriodFlowCntlSet rc - %d \n",rc);
            return rc;
        }
        /* not need to read  value from switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchPereodicFlowCntlGetPtr = psState; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyBackPressureSet (13)
*
* DESCRIPTION:
*       This function sets Back Pressure value (in half duplex only) in SS and PS MACs 
*       This function do nothing because 1540 macsec works only in full duplex mode
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    state                     - required value
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchBPSetPtr           - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyBackPressureSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_BOOL                     state,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchBPSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState = state;
    GT_BOOL   psState = state;


    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecSetBackPressureEnable(devNum,portNum,
                      ssState,psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyPeriodFlowCntlSet rc - %d \n",rc);
            return rc;
        }
        /* set the same value in switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchBPSetPtr = state; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyBackPressureGet (14)
*
* DESCRIPTION:
*       This function gets Back Pressure value (in half duplex only) from SS and PS MACs 
*       This function do nothing because 1540 macsec works only in full duplex mode
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *state                    - ptr to required value
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchFlowCntlGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyBackPressureGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_BOOL                     *statePtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchBPGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState;
    GT_BOOL   psState;

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecGetBackPressureEnable(devNum,portNum,
                      &ssState,&psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyPeriodFlowCntlSet rc - %d \n",rc);
            return rc;
        }
        /* not need to read  value from switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchBPGetPtr = psState; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;

}

/*******************************************************************************
* macPhyPortLinkGet (15)
*
* DESCRIPTION:
*       This function returns link status value PS MACs 
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    state                     - required value
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchFlowCntlGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPortLinkGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_BOOL                    *isLinkUpPtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *switchLinkStatusGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssIsLinkUp;
    GT_BOOL   psIsLinkUp;

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecGetLinkStatus(devNum,portNum,
                      &ssIsLinkUp,&psIsLinkUp, PHYMAC_PHY_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhySpeedGet \n");
            return rc;
        }
        /* not need to read  value from switch MAC ??? */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchLinkStatusGetPtr = psIsLinkUp; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyDuplexSet (16)
*
* DESCRIPTION:
*       This function sets duplex mode in SS and PS MACs 
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    dMode                     - required value
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchDuplexSetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyDuplexSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_DUPLEX_ENT        dMode,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT CPSS_PORT_DUPLEX_ENT        *switchDuplexSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    CPSS_PORT_DUPLEX_ENT   ssState = dMode;
    CPSS_PORT_DUPLEX_ENT   psState = dMode;

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecSetDuplexMode(devNum,portNum,
                      ssState,psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyDuplexSet rc - %d \n",rc);
            return rc;
        }
        /* set the same value in switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchDuplexSetPtr = dMode; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyDuplexGet (17)
*
* DESCRIPTION:
*       This function returns duplex mode from PS MACs 
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    dModePtr                  - ptr to required value
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchDuplexGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
/* (17) Get Port Duplex Mode */
static GT_STATUS macPhyDuplexGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_PORT_DUPLEX_ENT        *dModePtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT CPSS_PORT_DUPLEX_ENT        *switchDuplexGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    CPSS_PORT_DUPLEX_ENT   ssState;
    CPSS_PORT_DUPLEX_ENT   psState;

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* get from both MACs */
        rc = phyMacMSecGetDuplexMode(devNum,portNum,
                      &ssState,&psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyDuplexGet rc - %d \n",rc);
            return rc;
        }
        /* no get from switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchDuplexGetPtr = psState; 

        /* set return value - Phy Side Mac Duplex value */
        *dModePtr = psState;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyPortEnableSet (18)
*
* DESCRIPTION:
*       This function sets port enable/disable for SS and PS MACs 
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    enable                    - required value
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr         - indicates if the switch MAC speed should be set   
*    *switchDuplexGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPortEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_BOOL                     enable,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr, 
    OUT GT_BOOL                     *targetEnableSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState = enable;
    GT_BOOL   psState = enable;


    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecSetPortEnable(devNum,portNum,
                      ssState,psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyPortEnableSet rc - %d \n",rc);
            return rc;
        }
        /* set the same value in switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *targetEnableSetPtr = psState; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}


/*******************************************************************************
* macPhyPortEnableGet (19)
*
* DESCRIPTION:
*       This function returns port enable/disable value from PS MACs 
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    statePtr                - ptr to required value
*    *doPpMacConfigPtr       - indicates if the switch MAC value should be get   
*    *switchDuplexGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPortEnableGet
(
    IN   GT_U8                  devNum,
    IN   GT_PHYSICAL_PORT_NUM   portNum,
    OUT  GT_BOOL               *statePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchPortEnableGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState;
    GT_BOOL   psState;

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecGetPortEnable(devNum,portNum,
                      &ssState,&psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyPortEnableSet rc - %d \n",rc);
            return rc;
        }
        /* no get from switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchPortEnableGetPtr = psState; 

        /* set return value */
        *statePtr = psState;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyExcessiveCollisionDropSet (20)
*
* DESCRIPTION:
*       This function sets Excessive Collision Drop enable/disable value 
*       for PS and SS MACs 
*
* INPUTS:
*    devNum                    - device number    
*    portNum                   - port number
*    enable                    - required enable/disable value
*    stage                     - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr       - indicates if the switch MAC value should be get   
*    *switchDuplexGetPtr     - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyExcessiveCollisionDropSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchExcColDropSetPtr 
)
{
    /* get value from switch MAC */
    *doPpMacConfigPtr = GT_TRUE;

    /* Nothing to do - works only in half duplex mode */
    return GT_OK;
}

/*******************************************************************************
* macPhyExcessiveCollisionDropGet (20)
*
* DESCRIPTION:
*       This function returns Excessive Collision Drop enable/disable value 
*       from PS MACs 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    stage                      - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *enable                    - required enable/disable value
*    *doPpMacConfigPtr          - indicates if the switch MAC value should be get   
*    *switchDuplexGetPtr        - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyExcessiveCollisionDropGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchExcColDropGetPtr 
)
{
    /* Nothing to do - works only in half duplex mode */
    return GT_OK;
}

/*******************************************************************************
* macPhyPaddingEnableSet (22)
*
* DESCRIPTION:
*       This function sets Padding enable/disable mode 
*       from PS and SS MACs 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    enable                     - required enable/disable value
*    stage                      - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr          - indicates if the switch MAC value should be get   
*    *switchPaddingSetPtr       - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPaddingEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT GT_BOOL                 *switchPaddingSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState = enable;
    GT_BOOL   psState = enable;


    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecSetPaddingEnable(devNum,portNum,
                      ssState,psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyPaddingEnableSet rc - %d \n",rc);
            return rc;
        }
        /* set the same value in switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchPaddingSetPtr = psState; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyPaddingEnableGet (23)
*
* DESCRIPTION:
*       This function returns Padding enable/disable value 
*       from PS MACs 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    stage                      - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *enable                    - ptr to required enable/disable value
*    *doPpMacConfigPtr          - indicates if the switch MAC value should be get   
*    *switchPaddingSetPtr       - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPaddingEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *statePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchPaddingGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState;
    GT_BOOL   psState;

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecGetPaddingEnable(devNum,portNum,
                      &ssState,&psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyPortEnableSet rc - %d \n",rc);
            return rc;
        }
        /* no get from switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchPaddingGetPtr = psState; 

        /* set return value */
        *statePtr = psState;

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyPreambleLengthSet (24)
*
* DESCRIPTION:
*       This function sets Preamble Length 
*       for PS and SS MACs 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    direction                    - port direction
*    length                        - lenght
*    stage                         - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr                  - indicates if the switch MAC value should be set  
*    *targetDirection                    - target direction
*    *switchPreambleLengthPtr       - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                 *doPpMacConfigPtr, 
    OUT CPSS_PORT_DIRECTION_ENT *targetDirection,
    OUT GT_U32                  *switchPreambleLengthPtr 
)
{
    /* get value from switch MAC */
    *doPpMacConfigPtr = GT_TRUE;

    /* ??? No such field in 1540 MAC */  
    return GT_OK;
}

/*******************************************************************************
* macPhyPreambleLengthGet (25)
*
* DESCRIPTION:
*       This function sets Preamble Length 
*       for PS and SS MACs 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    direction                    - port direction
*    stage                         - stage to run (PRE or POST)          
*
* OUTPUTS:
*    length                                    - lenght
*    *doPpMacConfigPtr                  - indicates if the switch MAC value should be get  
*    *targetDirection                    - target direction
*    *switchPreambleLengthPtr       - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyPreambleLengthGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_DIRECTION_ENT     direction,
    OUT GT_U32                      *lengthPtr,
    IN CPSS_MACDRV_STAGE_ENT        stage,  
    OUT GT_BOOL                     *doPpMacConfigPtr,
    OUT CPSS_PORT_DIRECTION_ENT     *targetDirection,
    OUT GT_U32                      *switchPreambleLengthPtr 
)
{
    /* get value from switch MAC */
    *doPpMacConfigPtr = GT_TRUE;

    /* ??? No such field in 1540 MAC */  
    return GT_OK;
}

/*******************************************************************************
* macPhyCRCCheckSet (26)
*
* DESCRIPTION:
*       This function enable/disable CRC check mode  
*       for PS and SS MACs 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    enable                     - required enable/disable value
*    stage                      - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr          - indicates if the switch MAC value should be set   
*    *switchCRCCheckSetPtr       - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyCRCCheckSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchCRCCheckSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState = enable;
    GT_BOOL   psState = enable;


    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecSetCRCCheckEnable(devNum,portNum,
                      ssState,psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyCRCCheckSet rc - %d \n",rc);
            return rc;
        }
        /* set the same value in switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchCRCCheckSetPtr = psState; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyCRCCheckGet (27)
*
* DESCRIPTION:
*       This function returns enable/disable CRC check mode  
*       status for PS MAC 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    stage                      - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *enable                     - ptr to required enable/disable value
*    *doPpMacConfigPtr           - indicates if the switch MAC value should be get   
*    *switchCRCCheckSetPtr       - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyCRCCheckGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 *enablePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_BOOL                *switchCRCCheckGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   ssState;
    GT_BOOL   psState;

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecGetCRCCheckEnable(devNum,portNum,
                      &ssState,&psState, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyCRCCheckSet rc - %d \n",rc);
            return rc;
        }
        /* don't get value from switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchCRCCheckGetPtr = psState; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyMRUSet (28)
*
* DESCRIPTION:
*       This function sets MRU for 
*       status for PS and SS MACs 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    mruSize                    - MRU size
*    stage                      - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *doPpMacConfigPtr           - indicates if the switch MAC value should be get   
*    *switchCRCCheckSetPtr       - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyMRUSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  mruSize,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_U32                 *switchMRUSetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_U32   ssMruSize = mruSize;
    GT_U32   psMruSize = mruSize;


    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecSetMRU(devNum,portNum,
                      ssMruSize,psMruSize, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyMRUSet rc - %d \n",rc);
            return rc;
        }
        /* set the same value in switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchMRUSetPtr = mruSize; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyMRUGet (29)
*
* DESCRIPTION:
*       This function returns MRU value 
*       for PS MAC 
*
* INPUTS:
*    devNum                     - device number    
*    portNum                    - port number
*    stage                      - stage to run (PRE or POST)          
*
* OUTPUTS:
*    *mruSize                    - ptr to MRU size
*    *doPpMacConfigPtr           - indicates if the switch MAC value should be get   
*    *switchCRCCheckSetPtr       - target switch MAC value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static GT_STATUS macPhyMRUGet
(
    IN   GT_U8                  devNum,
    IN   GT_PHYSICAL_PORT_NUM   portNum,
    OUT  GT_U32                 *mruSizePtr,
    IN CPSS_MACDRV_STAGE_ENT    stage,  
    OUT GT_BOOL                *doPpMacConfigPtr, 
    OUT GT_U32                 *switchMRUGetPtr 
)
{
    GT_STATUS rc = GT_OK;
    GT_U32   ssMruSize;
    GT_U32   psMruSize;

    if (stage == CPSS_MACDRV_STAGE_PRE_E) {
        /* set both MACs */
        rc = phyMacMSecGetMRU(devNum,portNum,
                      &ssMruSize,&psMruSize, PHYMAC_BOTH_SIDE_SET_E);
        if (rc != GT_OK) {
            PHYMAC_DBG_ERROR("  >>> MACPHY ERROR - macPhyCRCCheckSet rc - %d \n",rc);
            return rc;
        }
        /* don't get the value from switch MAC */
        *doPpMacConfigPtr = GT_TRUE; 
        *switchMRUGetPtr = psMruSize; 

        return GT_OK;
    }
    /* post stage - do nothing */
    if (stage == CPSS_MACDRV_STAGE_POST_E) {
        return GT_OK;
    }
    return GT_OK;
}

/*******************************************************************************
* macDrvPhyDataBaseInit
*
* DESCRIPTION:
*       - copy of the phy data base from application to phy driver
*
* INPUTS:
*       phy1540MInfo * phy1540DataBase - ptr to phy driver data base
*       phy1540MDataBase * phyMacDataBase - ptr to application data base ptr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS macPhyDrvDataBaseInit
(
    phy1540MDataBase * applPhyMacDataBasePtr
)
{
    GT_U32 cnt;
    GT_U32 dbSize = applPhyMacDataBasePtr->phyInfoDBSize;

    /* memory allocation for phy object */
    phy1540DataBasePtr = osMalloc(sizeof(phy1540MDataBase));
    if (phy1540DataBasePtr == NULL) {
        return GT_FAIL;
    }

    phy1540DataBasePtr->phyInfoPtr = osMalloc(dbSize*sizeof(phy1540MInfo));
    
    /* NULL set for allocated memory */
    osMemSet(phy1540DataBasePtr->phyInfoPtr,0,
                 sizeof(dbSize*sizeof(phy1540MInfo)));

    /* set data base size */
    phy1540DataBasePtr->phyInfoDBSize = applPhyMacDataBasePtr->phyInfoDBSize;

    /* copy data base by items */
    for (cnt = 0; cnt < dbSize; cnt++) {
        phy1540DataBasePtr->phyInfoPtr[cnt] = applPhyMacDataBasePtr->phyInfoPtr[cnt];
    }
    return GT_OK;
}

/*******************************************************************************
* macDrv1540MInit
*
* DESCRIPTION:
*       - Creation and initialization of macDrv1540M object
*       - Binds SMI read/write register functions 
*
* INPUTS:
*       smiWriteRegister  - ptr to write register function
*       smiReadRegister  - ptr to read register function 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS macDrv1540MInit
(
    IN  CPSS_SMI_REGISTER_WRITE_FUNC  smiWriteRegister,
    IN  CPSS_SMI_REGISTER_READ_FUNC   smiReadRegister,
    IN  CPSS_SMI_START_TRANSACTION_FUNC  smiStartTransaction,
    IN  CPSS_SMI_FINISH_TRANSACTION_FUNC  smiFinishTransaction,
    IN  phy1540MDataBase * phyMacDataBasePtr,
    OUT CPSS_MACDRV_OBJ_STC ** macDrvObjPtr
)
{
    GT_STATUS rc;

    /* memory allocation for phy object */
    macPhy1540MObj = osMalloc(sizeof(CPSS_MACDRV_OBJ_STC));

    *macDrvObjPtr = macPhy1540MObj; 

    /* NULL set for allocated memory */
    osMemSet(macPhy1540MObj,0,sizeof(CPSS_MACDRV_OBJ_STC));

    /* object function init */

    macPhy1540MObj->macDrvMacSpeedSetFunc = macPhySpeedSet;       /* 1 */
    macPhy1540MObj->macDrvMacSpeedGetFunc = macPhySpeedGet;       /* 2 */
    macPhy1540MObj->macDrvMacDuplexANSetFunc = macPhyDuplexANSet;    /* 3 */
    macPhy1540MObj->macDrvMacDuplexANGetFunc = macPhyDuplexANGet;    /* 4 */
    macPhy1540MObj->macDrvMacFlowCntlANSetFunc = macPhyFlowCntlANSet;  /* 5 */
    macPhy1540MObj->macDrvMacFlowCntlANGetFunc = macPhyFlowCntlANGet;  /* 6 */
    macPhy1540MObj->macDrvMacSpeedANSetFunc = macPhySpeedANSet;     /* 7 */
    macPhy1540MObj->macDrvMacSpeedANGetFunc = macPhySpeedANGet;     /* 8 */
    macPhy1540MObj->macDrvMacFlowCntlSetFunc = macPhyFlowControlEnableSet; /* 9 */
    macPhy1540MObj->macDrvMacFlowCntlGetFunc = macPhyFlowControlEnableGet; /* 10 */
    macPhy1540MObj->macDrvMacPeriodFlowCntlSetFunc = macPhyPeriodicFlowCntlSet;    /* 11 */
    macPhy1540MObj->macDrvMacPeriodFlowCntlGetFunc = macPhyPeriodicFlowCntlGet;    /* 12 */
    macPhy1540MObj->macDrvMacBackPrSetFunc = macPhyBackPressureSet;     /* 13 */
    macPhy1540MObj->macDrvMacBackPrGetFunc = macPhyBackPressureGet;     /* 14 */
    macPhy1540MObj->macDrvMacPortlinkGetFunc = macPhyPortLinkGet;     /* 15 */
    macPhy1540MObj->macDrvMacDuplexSetFunc = macPhyDuplexSet;     /* 16 */
    macPhy1540MObj->macDrvMacDuplexGetFunc = macPhyDuplexGet;     /* 17 */
    macPhy1540MObj->macDrvMacPortEnableSetFunc = macPhyPortEnableSet;     /* 18 */
    macPhy1540MObj->macDrvMacPortEnableGetFunc = macPhyPortEnableGet;     /* 19 */
    macPhy1540MObj->macDrvMacExcessiveCollisionDropSetFunc = macPhyExcessiveCollisionDropSet;  /* 20 */
    macPhy1540MObj->macDrvMacExcessiveCollisionDropGetFunc = macPhyExcessiveCollisionDropGet;  /* 21 */
    macPhy1540MObj->macDrvMacPaddingEnableSetFunc = macPhyPaddingEnableSet;     /* 22 */
    macPhy1540MObj->macDrvMacPaddingEnableGetFunc = macPhyPaddingEnableGet;     /* 23 */
    macPhy1540MObj->macDrvMacPreambleLengthSetFunc = macPhyPreambleLengthSet;     /* 24 */
    macPhy1540MObj->macDrvMacPreambleLengthGetFunc = macPhyPreambleLengthGet;     /* 25 */
    macPhy1540MObj->macDrvMacCRCCheckSetFunc = macPhyCRCCheckSet;        /* 26 */
    macPhy1540MObj->macDrvMacCRCCheckGetFunc = macPhyCRCCheckGet;        /* 27 */
    macPhy1540MObj->macDrvMacMRUSetFunc = macPhyMRUSet;        /* 28 */
    macPhy1540MObj->macDrvMacMRUGetFunc = macPhyMRUGet;        /* 29 */

    /*********** SMI read/write init **********/

    macPhy1540MSmiWriteRegister =  smiWriteRegister;
    macPhy1540MSmiReadRegister  =  smiReadRegister;

    /*********** SMI start/finish transaction **********/

    macPhy1540MSmiStartTransaction  = smiStartTransaction;
    macPhy1540MSmiFinishTransaction = smiFinishTransaction;

    /*********** PHY INFO Data Base init **********/
    rc = macPhyDrvDataBaseInit(phyMacDataBasePtr);
    if (rc != GT_OK) {
        return rc;
    }

    return GT_OK;
}

