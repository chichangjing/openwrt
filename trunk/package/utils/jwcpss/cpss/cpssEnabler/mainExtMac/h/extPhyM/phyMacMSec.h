/*******************************************************************************
*              (c), Copyright 2010, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* phyMacMSec.h
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#ifndef __phyMacMSech
#define __phyMacMSech

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: enum PHYMAC_SIDE_SET_ACTION_ENT
 *
 * Description: Generic Serial Management Interface numbering
 *
 * Enumerations:
 *      CPSS_PHY_SMI_INTERFACE_0_E  - SMI interface\controller 0
 *      CPSS_PHY_SMI_INTERFACE_1_E  - SMI interface\controller 1  
 *      CPSS_PHY_SMI_INTERFACE_MAX_E - to simplify validity check
 */
typedef enum
{
    PHYMAC_SWITCH_SIDE_SET_E,
    PHYMAC_PHY_SIDE_SET_E,
    PHYMAC_BOTH_SIDE_SET_E

} PHYMAC_SIDE_SET_ACTION_ENT;


/**************************************************************
*  EEE definitions
***************************************************************/

/*
 * typedef: enum MACPHY_EEE_LPI_MODE_ENT
 *
 * Description: EEE mode numbering
 *
 * Enumerations:
 *      MACPHY_EEE_LPI_DISABLE_MODE_E  - EEE disable mode
 *      MACPHY_EEE_LPI_MASTER_MODE_E  -  EEE enable master mode  
 *      MACPHY_EEE_LPI_SLAVE_MODE_E -    EEE enable slave mode  
 *      MACPHY_EEE_LPI_FORCE_MODE_E -    EEE enable force mode  
 */

typedef enum
{
    MACPHY_EEE_LPI_DISABLE_MODE_E        = 0,
    MACPHY_EEE_LPI_MASTER_MODE_E,
    MACPHY_EEE_LPI_SLAVE_MODE_E,
    MACPHY_EEE_LPI_FORCE_MODE_E
} MACPHY_EEE_LPI_MODE_ENT;

/*
 * typedef: enum MACPHY_EEE_LPI_TIMER_ENT
 *
 * Description: EEE mode numbering
 *
 * Enumerations:
 *      MACPHY_EEE_LPI_FAST_EXIT_TIME_E  - LPI exit timer when port speed is 1000 Mbps
 *      MACPHY_EEE_LPI_SLOW_EXIT_TIME_E  - LPI exit timer when port speed is 10/100 Mbps
 *      MACPHY_EEE_LPI_FAST_ENTER_TIME_E - LPI enter timer when port speed is 1000 Mbps
 *      MACPHY_EEE_LPI_SLOW_ENTER_TIME_E - LPI enter timer when port speed is 10/100 Mbps
 */
typedef enum
{
    MACPHY_EEE_LPI_FAST_EXIT_TIME_E = 0,
    MACPHY_EEE_LPI_SLOW_EXIT_TIME_E,
    MACPHY_EEE_LPI_FAST_ENTER_TIMEOUT_E,
    MACPHY_EEE_LPI_SLOW_ENTER_TIMEOUT_E
} MACPHY_EEE_LPI_TIMER_ENT;


/*******************************************************************************
* phyMacMSecSetSpeed (1)
*
* DESCRIPTION:
*       This function sets speed to PHY SIDE MAC and SWITCH SIDE MAC of 
*       1540M PHY.
*
*
* INPUTS:
*    devNum           - device number    
*    portNum          - port number
*    ssSpeed          - speed value to set in Switch Side MAC
*    psStage          - speed value to set in Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
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
*******************************************************************************/
GT_STATUS phyMacMSecSetSpeed
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_PORT_SPEED_ENT  ssSpeed,
    IN CPSS_PORT_SPEED_ENT  psSpeed,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecGetSpeed (2)
*
*       This function returns speed values from PHY SIDE MAC and SWITCH SIDE MAC of 
*       1540M PHY.
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    *ssSpeed          - speed ptr to get the value from Switch Side MAC
*    *psStage          - speed ptr to get the value from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetSpeed
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN CPSS_PORT_SPEED_ENT  *ssSpeed,
    IN CPSS_PORT_SPEED_ENT  *psSpeed,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideMacActions
);

/*******************************************************************************
* phyMacMSecSetANDuplex (3)
*
* DESCRIPTION:
*       This function sets duplex Auto Negotiation state to MacSec 
*       Switch Side & Phy Side MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - duplex state to set the value from Switch Side MAC
*    psState          - duplex state to set the value from Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
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
*******************************************************************************/
GT_STATUS phyMacMSecSetANDuplex
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssState,
    IN GT_BOOL psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecGetANDuplex (4)
*
* DESCRIPTION:
*       This function returns AN duplex values from MacSec SS & PS MACs.
*
* INPUTS:
*    devNum     - device number
*    portNum    - port number
*    sideAction - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    *ssState   - SS MAC duplex state ptr
*    *psState   - PS MAC duplex state ptr
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetANDuplex
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);


/*******************************************************************************
* phyMacMSecSetANFlowControl (5)
*
* DESCRIPTION:
*       This function sets flow control parameters to Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - flow control state to set the value to Switch Side MAC
*    ssPauseAdvertise - pause advertise (symmetric/assimetric) to set 
*                       the value to Switch Side MAC
*    psState          - flow control state to set the value to Phy Side MAC   
*    psPauseAdvertise - pause advertise (symmetric/assimetric) to set 
*                       the value to Switch Side MAC
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetANFlowControl
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssState,
    IN GT_BOOL ssPauseAdvertise,
    IN GT_BOOL psState,
    IN GT_BOOL psPauseAdvertise,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecGetANFlowControl (6)
*
* DESCRIPTION:
*       This function returns flow control parameters from Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - flow control state to set the value to Switch Side MAC
*    ssPauseAdvertise - pause advertise (symmetric/assimetric) to set 
*                       the value to Switch Side MAC
*    psState          - flow control state to set the value to Phy Side MAC   
*    psPauseAdvertise - pause advertise (symmetric/assimetric) to set 
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetANFlowControl
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *ssPauseAdvertise,
    OUT GT_BOOL *psState,
    OUT GT_BOOL *psPauseAdvertise,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);
/*******************************************************************************
* phyMacMSecSetSpeedANEnable (7)
*
* DESCRIPTION:
*       This function enables/disables speed AN on Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - flow control state to set the value to Switch Side MAC
*    psState          - flow control state to set the value to Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetSpeedANEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssState,
    IN GT_BOOL psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);
/*******************************************************************************
* phyMacMSecGetSpeedANEnable (8)
*
* DESCRIPTION:
*       This function returns speed AN values from Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - speed enable value from Switch Side MAC
*    psState          - speed enable value from Phy Side MAC   
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetSpeedANEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecSetFlowControlEnable (9)
*
* DESCRIPTION:
*       This function sets flow control mode for Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - flow control mode for Switch Side MAC
*    psState          - flow control mode for Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetFlowControlEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN CPSS_PORT_FLOW_CONTROL_ENT ssState,
    IN CPSS_PORT_FLOW_CONTROL_ENT psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecGetFlowControlEnable (10)
*
* DESCRIPTION:
*       This function returns flow control mode for Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - flow control mode for Switch Side MAC
*    psState          - flow control mode for Phy Side MAC   
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetFlowControlEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT *ssState,
    OUT CPSS_PORT_FLOW_CONTROL_ENT *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMGecSetPeriodicFlowControlEnable  (11)
*
* DESCRIPTION:
*       This function sets Periodic Xon Flow Control enable/disable for PHY SIDE MAC 
*       and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    ssPrdFlowControl   - periodic flow control value to set in Switch Side MAC
*    psPrdFlowControl   - periodic flow control value to set in Phy Side MAC   
*    sideAction         - side actions (SS or PS or BOTH) 
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
*******************************************************************************/
GT_STATUS phyMacMSecSetPeriodicFlowControlEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssPrdFlowControl,
    IN GT_BOOL psPrdFlowControl,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);
/*******************************************************************************
* phyMacMSecGetPeriodicFlowControlEnable (12)
*
* DESCRIPTION:
*       This function gets status of periodic 802.3x flow control
*       for Switch Side & Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - periodic flow control state for Switch Side MAC
*    psState          - periodic flow control state for Phy Side MAC   
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetPeriodicFlowControlEnable
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);
/*******************************************************************************
* phyMacMSecSetBackPressureEnable (13)
*
* DESCRIPTION:
*       This function Enable/Disable back pressure on port in half duplex 
*       condition for Switch Side & Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - back pressure mode for Switch Side MAC
*    psState          - back pressure mode for Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetBackPressureEnable
(
    IN GT_U8     devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL   ssState,
    IN GT_BOOL   psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);
/*******************************************************************************
* phyMacMSecGetBackPressureEnable (14)
*
* DESCRIPTION:
*       This function gets status of backpressure on port
*       for Switch Side & Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - back pressure state for Switch Side MAC
*    psState          - back pressure state for Phy Side MAC   
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetBackPressureEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);
/*******************************************************************************
* phyMacMSecGetLinkStatus (15)
*
*       This function returns link status values from PHY SIDE MAC and SWITCH SIDE MAC of 
*       1540M PHY.
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    *ssSpeed          - speed ptr to get the value from Switch Side MAC
*    *psStage          - speed ptr to get the value from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetLinkStatus
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssLinkStatus,
    OUT GT_BOOL *psLinkStatus,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction

);

/*******************************************************************************
* phyMacMSecSetDuplexMode  (16)
*
* DESCRIPTION:
*       This function sets duplex mode for PHY SIDE MAC 
*       and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    ssDuplexMode       - duplex mode value to set in Switch Side MAC
*    psDuplexMode       - duplex mode value to set in Phy Side MAC   
*    sideAction         - side actions (SS or PS or BOTH) 
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
*******************************************************************************/
GT_STATUS phyMacMSecSetDuplexMode
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssDuplexMode,
    IN GT_BOOL psDuplexMode,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);


/*******************************************************************************
* phyMacMSecGetDuplexMode  (17)
*
* DESCRIPTION:
*       This function returns duplex mode enable/disable 
*       status for PHY SIDE MAC and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    sideAction         - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssPrdFlowControl   - periodic flow control value to set in Switch Side MAC
*    psPrdFlowControl   - periodic flow control value to set in Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetDuplexMode 
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_DUPLEX_ENT *ssDuplexModePtr,
    OUT CPSS_PORT_DUPLEX_ENT *psDuplexModePtr,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);
/*******************************************************************************
* phyMacMSecSetPortEnable  (18)
*
* DESCRIPTION:
*       This function sets port enable/disable mode for PHY SIDE MAC 
*       and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    ssPortEnable       - port enable/disable value to set in Switch Side MAC
*    psPortEnable       - port enable/disable value to set in Phy Side MAC   
*    sideAction         - side actions (SS or PS or BOTH) 
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
*******************************************************************************/
GT_STATUS phyMacMSecSetPortEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssPortEnable,
    IN GT_BOOL psPortEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecGetPortEnable  (19)
*
* DESCRIPTION:
*       This function returns port enable/disable  
*       status for PHY SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    sideAction         - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssPortEnablePtr   - ptr to port enable value from Switch Side MAC
*    psPortEnablePtr   - ptr to port enable value from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetPortEnable 
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssPortEnablePtr,
    OUT GT_BOOL *psPortEnablePtr,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecSetPaddingEnable  (22)
*
* DESCRIPTION:
*       This function sets port padding enable mode 
*       for PHY SIDE MAC and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    ssPaddingEnable       - Padding enable/disable value to set in Switch Side MAC
*    psPaddingEnable       - Padding enable/disable value to set in Phy Side MAC   
*    sideAction            - side actions (SS or PS or BOTH) 
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
*******************************************************************************/
GT_STATUS phyMacMSecSetPaddingEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssPaddingEnable,
    IN GT_BOOL psPaddingEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecGetPaddingEnable  (23)
*
* DESCRIPTION:
*       This function returns port padding enable mode 
*       for PHY SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssPaddingEnable       - Padding enable/disable value to get from Switch Side MAC
*    psPaddingEnable       - Padding enable/disable value to get from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetPaddingEnable
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssPaddingEnable,
    OUT GT_BOOL *psPaddingEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);


/*******************************************************************************
* phyMacMSecSetCRCCheckEnable  (26)
*
* DESCRIPTION:
*       This function enable/disable CRC check mode 
*       for PHY SIDE MAC and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    ssCRCCheckEnable       - CRC check enable/disable value to set in Switch Side MAC
*    psCRCCheckEnable       - CRC check enable/disable value to set in Phy Side MAC   
*    sideAction            - side actions (SS or PS or BOTH) 
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
*******************************************************************************/
GT_STATUS phyMacMSecSetCRCCheckEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssCRCCheckEnable,
    IN GT_BOOL psCRCCheckEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);
/*******************************************************************************
* phyMacMSecGetCRCCheckEnable  (27)
*
* DESCRIPTION:
*       This function returns port padding enable mode 
*       for PHY SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssCRCCheckEnable       - CRC Check enable/disable value to get from Switch Side MAC
*    psCRCCheckEnable       - CRC Check enable/disable value to get from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetCRCCheckEnable
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssCRCCheckEnable,
    OUT GT_BOOL *psCRCCheckEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecSetMRU  (28)
*
* DESCRIPTION:
*       This function sets MRU size  
*       for PHY SIDE MAC and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    ssMruSize             - MruSize value to set in Switch Side MAC
*    psMruSize             - MruSize value to set in Phy Side MAC   
*    sideAction            - side actions (SS or PS or BOTH) 
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
*******************************************************************************/
GT_STATUS phyMacMSecSetMRU
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_U32  ssMruSize,
    IN GT_U32  psMruSize,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/*******************************************************************************
* phyMacMSecGetMRU  (29)
*
* DESCRIPTION:
*       This function returns port padding enable mode 
*       for PHY SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssMruSize             - Mru Size value to get from Switch Side MAC
*    psMruSize             - Mru Size value to get from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetMRU
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_U32 *ssMruSize,
    OUT GT_U32 *psMruSize,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
);

/******************************
*  EEE functions 
*******************************/


/*******************************************************************************
* phyMacEEEModeSet
*
* DESCRIPTION:
*       This function sets EEE mode   
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    mode                  - defines one of: 
*                                    master mode 
*                                    slave mode 
*                                    force mode 
*                                    EEE disable mode 
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
*******************************************************************************/
GT_STATUS phyMacEEEModeSet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN MACPHY_EEE_LPI_MODE_ENT mode 
);

/*******************************************************************************
* phyMacEEEModeGet
*
* DESCRIPTION:
*       This function returns EEE mode   
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*                                   
* OUTPUTS:
*    modePtr                  - defines one of: 
*                                    master mode 
*                                    slave mode 
*                                    force mode 
*                                    EEE disable mode 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacEEEModeGet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT MACPHY_EEE_LPI_MODE_ENT *modePtr    
);

/*******************************************************************************
* phyMacEEEInit
*
* DESCRIPTION:
*       This function:
*           - enable EEE auto-neg advertisment
*           - soft reset
*           - run EEE workaround for 1540
*
* INPUTS:
*    IN GT_U8   devNum   -  device number  
*    IN GT_PHYSICAL_PORT_NUM  portNum,  -  port number 
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacEEEInit
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __phyMacMSech */



