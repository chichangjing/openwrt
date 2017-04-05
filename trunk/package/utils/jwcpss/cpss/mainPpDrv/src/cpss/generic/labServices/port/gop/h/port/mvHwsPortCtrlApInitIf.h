/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsPortCtrlApInitIf.h
*
* DESCRIPTION:
*       API to configure and run 802.3ap Serdes AutoNeg engine
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
******************************************************************************/

#ifndef __mvHwServicesPortCtrlApIf_H
#define __mvHwServicesPortCtrlApIf_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gtOs/gtGenTypes.h>
#include <mvHwsPortInitIf.h>
#include <private/mvHwsPortTypes.h>
#include <mvHwsPortApInitIf.h>

/*******************************************************************************
* mvHwsApPortCtrlStart
*
* DESCRIPTION:
*       Init AP port capability.
*       Runs AP protocol(802.3ap Serdes AutoNeg) and configures the best port
*       mode and all it's elements accordingly.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       apCfg       - AP configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlStart
(
    GT_U8         devNum,
    GT_U32        portGroup,
    GT_U32        phyPortNum,
    MV_HWS_AP_CFG *apCfg
);

/*******************************************************************************
* mvHwsApPortCtrlStop
*
* DESCRIPTION:
*       Disable the AP engine on port and release all its resources.
*       Clears the port mode and release all its resources according to selected.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlStop
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum
);

/*******************************************************************************
* mvHwsApPortCtrlSysAck
*
* DESCRIPTION:
*       Acknowledge port resources were allocated at application level
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlSysAck
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum
);

/*******************************************************************************
* mvHwsApPortCtrlCfgGet
*
* DESCRIPTION:
*       Returns the AP port configuration.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       apCfg       - AP configuration parameters
*
* OUTPUTS:
*       apCfg       - AP configuration parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlCfgGet
(
    GT_U8         devNum,
    GT_U32        portGroup,
    GT_U32        phyPortNum,
    MV_HWS_AP_CFG *apCfg
);

/*******************************************************************************
* mvHwsApPortCtrlStatusGet
*
* DESCRIPTION:
*       Returns the AP port resolution information
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       apStatus   - AP status parameters
*
* OUTPUTS:
*       apStatus - AP/HCD results
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlStatusGet
(
    GT_U8                 devNum,
    GT_U32                portGroup,
    GT_U32                apPortNum,
    MV_HWS_AP_PORT_STATUS *apStatus
);

/*******************************************************************************
* mvHwsApPortStatsGet
*
* DESCRIPTION:
*       Returns the AP port statistics information
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       apStats - AP stats
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlStatsGet
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               apPortNum,
    MV_HWS_AP_PORT_STATS *apStats
);

/*******************************************************************************
* mvHwsApPortCtrlStatsReset
*
* DESCRIPTION:
*       Reset AP port statistics information
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlStatsReset
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               apPortNum
);

/*******************************************************************************
* mvHwsApPortCtrlIntropSet
*
* DESCRIPTION:
*       Set AP port introp information
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       apintrop    - AP introp parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlIntropSet
(
    GT_U8                 devNum,
    GT_U32                portGroup,
    GT_U32                phyPortNum,
    MV_HWS_AP_PORT_INTROP *apIntrop
);

/*******************************************************************************
* mvHwsApPortCtrlIntropGet
*
* DESCRIPTION:
*       Return AP port introp information
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       apintrop    - AP introp parameters
*
* OUTPUTS:
*       apintrop - AP introp parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlIntropGet
(
    GT_U8                 devNum,
    GT_U32                portGroup,
    GT_U32                phyPortNum,
    MV_HWS_AP_PORT_INTROP *apIntrop
);

/*******************************************************************************
* mvHwsApPortCtrlDebugGet
*
* DESCRIPTION:
*       Return AP debug information
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       apDebug    - AP debug parameters
*
* OUTPUTS:
*       apDebug - AP debug parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlDebugGet
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_AP_PORT_DEBUG *apDebug
);

/*******************************************************************************
* mvHwsApPortCtrlAvagoGuiSet
*
* DESCRIPTION:
*       Set AP state machine state when Avago GUI is enabled
*       Avago GUI access Avago Firmware as SBUS command level
*       Therefore it is required to stop the periodic behiviour of AP state
*       machine when Avago GUI is enabled
*
* INPUTS:
*       devNum - system device number
*       state  - Avago GUI state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortCtrlAvagoGuiSet
(
    GT_U8 devNum,
    GT_U8 state
);

/*******************************************************************************
* mvApPortCtrlStatsShow
*
* DESCRIPTION:
*       Print AP port statistics information stored in system
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortCtrlStatsShow
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  apPortNum
);

/*******************************************************************************
* mvApPortCtrlIntropShow
*
* DESCRIPTION:
*       Print AP port introp information stored in system
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortCtrlIntropShow
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  apPortNum
);

/*******************************************************************************
* mvApPortCtrlDebugLogShow
*
* DESCRIPTION:
*       Print AP port real-time log information stored in system
*
* INPUTS:
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortCtrlDebugInfoShow
(
    GT_U32  apPortNum
);

#ifdef __cplusplus
}
#endif

#endif /* mvHwServicesPortCtrlApIf_H */


