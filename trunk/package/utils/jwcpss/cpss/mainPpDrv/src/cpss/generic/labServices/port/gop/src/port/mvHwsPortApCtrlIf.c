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
* mvHwsPortApCtrlIf.c
*
* DESCRIPTION: AP Port Control external interface
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 42 $
******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <mvHwsPortApInitIf.h>
#include <mvHwsPortCtrlApInitIf.h>
#include <mvHwsPortCtrlApDefs.h>
#include <mvHwsIpcDefs.h>
#include <private/mvHwsPortApInitIfPrv.h>
#include <mvHwsIpcApis.h>

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
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApStartIpc(devNum, portGroup, phyPortNum, (GT_U32*)apCfg));

    return GT_OK;
}

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
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApStopIpc(devNum, portGroup, phyPortNum));

    return GT_OK;
}

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
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApSysAckIpc(devNum, portGroup, phyPortNum));

    return GT_OK;
}

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
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApCfgGetIpc(devNum, portGroup, phyPortNum, (GT_U32*)apCfg));

    return GT_OK;
}

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
    GT_U32                phyPortNum,
    MV_HWS_AP_PORT_STATUS *apStatus
)
{
    GT_STATUS rcode;

    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    apStatus->preApPortNum  = phyPortNum;
    apStatus->postApPortNum = phyPortNum;

    rcode = mvHwsPortApStatusGetIpc(devNum, portGroup, phyPortNum, (GT_U32*)apStatus);
    if (rcode == GT_OK)
    {
        switch (apStatus->hcdResult.hcdResult)
        {
        case Port_40GBase_R:
            apStatus->postApPortMode = _40GBase_KR;
            break;
        case Port_10GBase_R:
            apStatus->postApPortMode = _10GBase_KR;
            break;
        case Port_10GBase_KX4:
            apStatus->postApPortMode = _10GBase_KX4;
            break;
        case Port_1000Base_KX:
            apStatus->postApPortMode = _1000Base_X;
            break;
        }
        return GT_OK;
    }

    return rcode;
}

/*******************************************************************************
* mvHwsApPortCtrlStatsGet
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
*       apResult - AP/HCD results
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
    GT_U32               phyPortNum,
    MV_HWS_AP_PORT_STATS *apStats
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApStatsGetIpc(devNum, portGroup, phyPortNum, (GT_U32*)apStats));

    return GT_OK;
}

/*******************************************************************************
* mvHwsApPortCtrlStatsReset
*
* DESCRIPTION:
*       Reset AP port statistics information
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - AP port number
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
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApStatsResetIpc(devNum, portGroup, phyPortNum));

    return GT_OK;
}

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
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApIntropSetIpc(devNum, portGroup, phyPortNum, (GT_U32*)apIntrop));

    return GT_OK;
}

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
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApIntropGetIpc(devNum, portGroup, phyPortNum, (GT_U32*)apIntrop));

    return GT_OK;
}

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
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApDebugGetIpc(devNum, portGroup, phyPortNum, (GT_U32*)apDebug));

    return GT_OK;
}

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
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvHwsPortApAvagoGuiSetIpc(devNum, state));

    return GT_OK;
}

