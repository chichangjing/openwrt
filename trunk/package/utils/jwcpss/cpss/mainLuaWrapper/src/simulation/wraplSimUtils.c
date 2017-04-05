/*************************************************************************
* wraplTgfUtils.c
*
* DESCRIPTION:
*       A lua tgf wrappers
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
**************************************************************************/
#include <string.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

#define GT_BIT  GT_U32
#include <cpss/generic/cpssTypes.h>
#include <asicSimulation/SKernel/smain/smain.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif

extern void startSimulationLog(void);
extern void stopSimulationLog(void);
extern void startSimulationLogFull(void);
extern void startSimulationLogWithFromCpu(void);
extern void setSimulationLogFullPathName(const char* fullPathName);
extern GT_STATUS skernelPortLinkStatusChange
(
    IN  GT_U8    deviceNumber,
    IN  GT_U8    portNumber,
    IN  GT_BOOL  newStatus
);
extern int osPrintf(const char* format, ...);
/*******************************************************************************
* wrlSimulationLog
*
* DESCRIPTION:
*       Option to start/stop simulation log
*
* INPUTS:
*       mode     - the simulation log mode :
            mode_stop = 0
            mode_start = 1
            mode_start_with_from_cpu = 2
            mode_start_full = 3
*
*       full_path_and_file_name - optional parameter to state the name of the
*               LOG file to open.... full path name !
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return_code
*
* COMMENTS:
*
*******************************************************************************/
int wrlSimulationLog
(
    lua_State* L
)
{
    GT_STATUS   rc = GT_OK;
    const char *fullPathName = NULL;
    size_t      len;
    GT_U32  mode;
    enum{
        mode_stop = 0,
        mode_start = 1,
        mode_start_with_from_cpu = 2,
        mode_start_full = 3
    };

    PARAM_NUMBER(rc, mode,  1, GT_U32);
    if (rc != GT_OK)
    {
        lua_pushnumber(L, rc);
        return 1;
    }

    switch (mode)
    {
        case mode_start:
        case mode_start_with_from_cpu:
        case mode_start_full:
            if (lua_type(L, 2) == LUA_TSTRING)
            {
                /* the second parameter is the FULL path */
                fullPathName = lua_tolstring(L, 2, &len);
                setSimulationLogFullPathName(fullPathName);
            }
            break;
        default:
            break;
    }

    switch(mode)
    {
        case mode_start:
            startSimulationLog();
            break;
        case mode_start_with_from_cpu:
            startSimulationLogWithFromCpu();
            break;
        case mode_start_full:
            startSimulationLogFull();
            break;
        case mode_stop:
            stopSimulationLog();
            break;
        default:
            rc = GT_NOT_SUPPORTED;
            break;
    }

    lua_pushnumber(L, rc);

    return 1;
}

#if ((defined GT_PCI) || (defined GT_PEX))
    #define SUPPORT_SIM_CONVERT
#endif/*((defined GT_PCI) || (defined GT_PEX))*/
/* convert CPSS devNum, coreId to simulation deviceId */
extern GT_STATUS prvCpssDrvDevNumToSimDevIdConvert
(
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
    OUT GT_U32      *simDeviceIdPtr
);

extern GT_STATUS   getSimDevIdFromSwDevNum
(
    IN GT_U8    swDevNum,
    IN  GT_U32  portGroupId,
    OUT GT_U32  *simDeviceIdPtr
);

/*******************************************************************************
* getSimPortNumFromCpssPortNum
*
* DESCRIPTION:
*       (simulation only) Get simulation port number from CPSS one
*
* INPUTS:
*       devNum
*       portNum
*
* OUTPUTS:
*       None
*
* RETURNS:
*       simulation port number
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 getSimPortNumFromCpssPortNum
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_PORT_MAP_STC      portMap;
    GT_STATUS                   rc;

    /* convert Physical port to MAC number */
    rc = cpssDxChPortPhysicalPortMapGet(devNum, portNum, 1, &portMap);
    if (rc == GT_OK)
    {
        if (portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E)
        {
            /* Port is SDMA one - this is not SLAN map for it */
            return portNum;
        }

        return portMap.interfaceNum;
    }
    return portNum;
#else
    return portNum;
#endif
}

static char simulationDynamicSlan_slanName[20];
static GT_STATUS    getPortSlanName
(
    IN GT_U32       swDevNum,
    IN GT_U32       simDeviceId,
    IN GT_BOOL      isFirst,
    IN  GT_PHYSICAL_PORT_NUM  startGlobalPort,
    OUT GT_PHYSICAL_PORT_NUM  *currentGlobalPortPtr
)
{
    GT_STATUS rc;
    GT_U32      ii;
    GT_U32      slanMaxLength = 20;
    GT_BOOL     portBound;

    if(isFirst == GT_TRUE)
    {
        ii = 0;
    }
    else
    {
        ii = startGlobalPort;
    }

    do
    {
        portBound = GT_FALSE;

        if(isFirst == GT_TRUE)
        {
            isFirst = GT_FALSE;
            ii--;/* compensation for the ++ that will come next step */
        }

        do
        {
            ii++;

            if(ii >= PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(swDevNum))
            {
                /* no more physical ports for this device */
                return GT_NO_MORE;
            }

            if(1 == PRV_CPSS_PHY_PORT_IS_EXIST_MAC(swDevNum,ii)
                || ii == CPSS_CPU_PORT_NUM_CNS)
            {
                /* next valid port / CPU */
                break;
            }

        }while(1);

        rc = skernelDevPortSlanGet(simDeviceId,
                              ii,
                              slanMaxLength,
                              &portBound,
                              simulationDynamicSlan_slanName);
        if(rc != GT_OK &&
           ii != CPSS_CPU_PORT_NUM_CNS)/*fix CQ#131878*/
        {
            return rc;
        }

        if(portBound == GT_TRUE)
        {
            /* this port is bound */
            break;
        }
    }
    while(1);

    *currentGlobalPortPtr = (GT_PHYSICAL_PORT_NUM)ii;

    return GT_OK;
}

static GT_STATUS wrlSimulationDynamicSlanDevDelete(IN GT_U8 swDevNum)
{
    GT_STATUS   rc;
    GT_U32  simDeviceId;
    GT_U32  portNum = 0;

    /* convert to simulation deviceId */
    rc = getSimDevIdFromSwDevNum(swDevNum,0,
                           &simDeviceId);
    if(rc != GT_OK)
    {
        return rc;
    }

    do
    {
        rc = getPortSlanName(swDevNum,
                             simDeviceId,
                             (portNum == 0) ? GT_TRUE : GT_FALSE,
                             portNum,
                             &portNum);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(portNum != CPSS_CPU_PORT_NUM_CNS)/* CPU as GMII */
        {
            /* unbind the port */
            rc = skernelUnbindDevPort2Slan(simDeviceId,portNum);
            if(rc != GT_OK)
            {
                return rc;
            }

            cmdOsTimerWkAfter(10);/* allow the interrupts due to link change to be over before we unbind next port */
        }
    }while(1);
}
/*******************************************************************************
* wrlSimulationSlanManipulations
*
* DESCRIPTION:
*       (simulation only) Do slan manipulations.
*
* INPUTS:
*       mode :
*           0 - bind
*           1 - unbind (single port in device or all ports in device , when port = 0xFF)
*           2 - unbind all (devices,ports)
*           3 - link up
*           4 - link down
*       devNum_1
*       portNum_1
*       devNum_2
*       port_2
*       slanName
*       keep_slan_connected
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return_code
*
* COMMENTS:
*
*******************************************************************************/
int wrlSimulationSlanManipulations
(
    lua_State* L
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32  currIndex = 1;
    GT_U32  devNumArr[2]={0};
    GT_U32  portNumArr[2]={0};
    GT_U32  numOfPorts = 0;
    GT_CHAR *slanName = NULL;
    GT_U32  mode;
    GT_U32  sim_deviceId;
    GT_U32  i;
    GT_U32  simPortNum;
    GT_BOOL keep_slan_connected = GT_FALSE;
    GT_BOOL unbindOtherPortsOnThisSlan = GT_TRUE;


    PARAM_NUMBER(rc, mode,  currIndex, GT_U32);
    currIndex++;

    if(mode == 2) /*unbind all */
    {
        GT_U8   device;
        for(device = 0 ; device < PRV_CPSS_MAX_PP_DEVICES_CNS; device++)
        {
            if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(device))
                continue;

            osPrintf("unbind ALL slans from device[%d] \n" , device);
            /* unbind all ports from the device */
            (void)wrlSimulationDynamicSlanDevDelete(device);
        }

        rc = GT_OK;

        lua_pushnumber(L, rc);
        return 1;
    }

    while (numOfPorts < 2)
    {
        if ((lua_type(L, currIndex) == LUA_TNUMBER) &&
            (lua_type(L, (currIndex+1)) == LUA_TNUMBER))
        {
            PARAM_NUMBER(rc, devNumArr[numOfPorts],  currIndex, GT_U32);
            PARAM_NUMBER(rc, portNumArr[numOfPorts],  currIndex+1, GT_U32);
            currIndex += 2;
            numOfPorts++;
        }
        else
        {
            break;
        }
    }
    if (lua_type(L, currIndex) == LUA_TSTRING)
    {
        slanName = (GT_CHAR*)lua_tostring(L, currIndex);
        currIndex++;
    }

    if (lua_type(L, currIndex) == LUA_TBOOLEAN)
    {
        PARAM_BOOL(rc, keep_slan_connected,  currIndex);
        currIndex++;

        unbindOtherPortsOnThisSlan = (keep_slan_connected == GT_TRUE) ?
                            GT_FALSE : GT_TRUE;
    }

    if (numOfPorts == 0)
    {
        rc = GT_BAD_PARAM;
    }
    if (rc != GT_OK)
    {
        lua_pushnumber(L, rc);
        return 1;
    }

    for (i = 0; i < numOfPorts; i++)
    {
        /* convert to simulation deviceId */
        rc = getSimDevIdFromSwDevNum(devNumArr[i],0,&sim_deviceId);

        simPortNum = getSimPortNumFromCpssPortNum(devNumArr[i], portNumArr[i]);
        if (mode == 0 && slanName) /* bind*/
        {
            skernelBindDevPort2Slan(sim_deviceId,simPortNum,
                slanName,(i == 0) ? unbindOtherPortsOnThisSlan : GT_FALSE);
        }
        else if(mode == 1) /*unbind*/
        {
            if(simPortNum == 0xFF)
            {
                /* unbind all ports from the device */
                wrlSimulationDynamicSlanDevDelete(devNumArr[i]);
            }
            else
            {
                skernelUnbindDevPort2Slan(sim_deviceId,simPortNum);
            }
        }
        else if (mode == 3) /* link up */
        {
            skernelPortLinkStatusChange(sim_deviceId,simPortNum,GT_TRUE);
        }
        else if (mode == 4) /* link down */
        {
            skernelPortLinkStatusChange(sim_deviceId,simPortNum,GT_FALSE);
        }
        else
        {
            /* not implemented */
        }
    }/* end for */

    lua_pushnumber(L, rc);

    return 1;
}

/*******************************************************************************
* wrlSimulationSlanGet
*
* DESCRIPTION:
*       (simulation only) Get SLAN name
*
* INPUTS:
*       devNum
*       portNum
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return_code
*       isConnected
*       slanName
*
* COMMENTS:
*
*******************************************************************************/
int wrlSimulationSlanGet
(
    lua_State* L
)
{
    GT_U8                   devNum;   /* CPSS device number */
    GT_PHYSICAL_PORT_NUM    portNum;  /* CPSS port number */
    GT_CHAR                 slanName[20]; /* SLAN name */
    GT_BOOL                 isConnected = GT_FALSE; /* is SLAN connected to port */
    GT_U32                  simDeviceId; /* Simulation device ID */
    GT_U32                  simPortNum;  /* Simulation port number */
    GT_STATUS               rc;          /* return code */

    rc = GT_OK; /* required because PARAM_xxx don't modify an rc if no error */
    cpssOsMemSet(slanName,0, sizeof(slanName));

    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    if(rc != GT_OK)
    {
        lua_pushnumber(L, rc);
        lua_pushboolean(L, isConnected);
        lua_pushstring(L, (char *)slanName);
        return 3;
    }

    PARAM_NUMBER(rc, portNum, 2, GT_PHYSICAL_PORT_NUM);
    if(rc != GT_OK)
    {
        lua_pushnumber(L, rc);
        lua_pushboolean(L, isConnected);
        lua_pushstring(L, (char *)slanName);
        return 3;
    }

    simPortNum = getSimPortNumFromCpssPortNum(devNum, portNum);

    /* convert to simulation deviceId */
    rc = getSimDevIdFromSwDevNum(devNum,0,&simDeviceId);
    if(rc != GT_OK)
    {
        lua_pushnumber(L, rc);
        lua_pushboolean(L, isConnected);
        lua_pushstring(L, (char *)slanName);
        return 3;
    }

    rc = skernelDevPortSlanGet(simDeviceId, simPortNum, sizeof(slanName),
                               &isConnected, slanName);
    if(rc != GT_OK)
    {
        lua_pushnumber(L, rc);
        lua_pushboolean(L, isConnected);
        lua_pushstring(L, (char *)slanName);
        return 3;
    }

    /* push the results into the stack */
    lua_pushnumber(L, GT_OK);
    lua_pushboolean(L, isConnected);
    lua_pushstring(L, (char *)slanName);
    return 3;
}


