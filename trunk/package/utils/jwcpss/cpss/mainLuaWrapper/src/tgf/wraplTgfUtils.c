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

#include <extUtils/trafficEngine/tgfTrafficEngine.h>
#include <extUtils/trafficEngine/tgfTrafficGenerator.h>
#include <extUtils/trafficEngine/tgfTrafficTable.h>

#define PRV_LUA_TGF_VERIFY_GT_OK(rc, msg)                                    \
    if (GT_OK != rc)                                                         \
    {                                                                        \
        lua_getglobal(L, "print");                                           \
        lua_pushfstring(L, "\n[TGF]: %s FAILED, rc = [%d]", msg, rc);        \
        lua_call(L, 1, 0);                                                   \
        lua_pushnumber(L, rc);                                               \
        return 1;                                                            \
    }
#define PRV_TGF_LOG1_MAC(msg, param) { \
    lua_getglobal(L, "print"); \
    lua_pushfstring(L, msg, param); \
    lua_call(L, 1, 0); \
}

#define PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, msg)                     \
    if (GT_OK != rc)                                                         \
    {                                                                        \
        lua_getglobal(L, "print");                                           \
        lua_pushfstring(L, "\n[TGF]: %s FAILED, rc = [%d]", msg, rc);        \
        lua_call(L, 1, 0);                                                   \
        prv_gc_TGF_PACKET_STC(&packetInfo);                                  \
        lua_pushnumber(L, rc);                                               \
        return 1;                                                            \
    }
#define PRV_LUA_TGF_TR_VERIFY_GT_OK_DEVPORT(rc,msg,dev,port)                 \
    if (GT_OK != rc)                                                         \
    {                                                                        \
        lua_getglobal(L, "print");                                           \
        lua_pushfstring(L, "\n[TGF]: %s FAILED on %d/%d, rc = [%d]",         \
                msg, dev, port, rc);                                         \
        lua_call(L, 1, 0);                                                   \
        prv_gc_TGF_PACKET_STC(&packetInfo);                                  \
        lua_pushnumber(L, rc);                                               \
        return 1;                                                            \
    }

use_prv_struct(TGF_PACKET_STC)
use_prv_struct(TGF_NET_DSA_STC)
void prv_gc_TGF_PACKET_STC(TGF_PACKET_STC *val);
use_prv_enum(TGF_PACKET_TYPE_ENT);
use_prv_enum(TGF_CAPTURE_MODE_ENT)
int mgmType_to_c_TGF_VFD_INFO_STC_ARR(lua_State *L);

extern GT_STATUS tgfStateTrgEPortInsteadPhyPort(
    IN GT_U32                trgEPort,
    IN GT_BOOL               enable
);

/*******************************************************************************
* prvLuaTgfTransmitPacketsWithCapture
*
* DESCRIPTION:
*       Transmit packets with capturing
*
* INPUTS:
*       inDevNum      - ingress device number
*       inPortNum     - ingress port number
*       packetInfoPtr - (pointer to) the packet info
*       burstCount    - number of frames
*       outDevNum     - egress device number
*       outPortNum    - egress port number
*       captureMode   - packet capture mode
*       captureOnTime - time the capture will be enabe (in milliseconds)
*       withLoopBack_senderPort  - optional . do we need loopback of sender port.
*                                  when omitted --> considered 'true'
*       withLoopBack_capturePort - optional . do we need loopback on captured port.
*                                  when omitted --> considered 'true'
*               NOTE: withLoopBack_senderPort and withLoopBack_capturePort
*                   come together or not come at all.
*       vfdArray      - optional VFD array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return code
*
* COMMENTS:
*
*******************************************************************************/
int prvLuaTgfTransmitPacketsWithCapture
(
    lua_State* L
)
{
    GT_U8                inDevNum;
    GT_U32               inPortNum;
    TGF_PACKET_STC       packetInfo;
    GT_U32               burstCount;
    GT_U8                outDevNum;
    GT_U32               outPortNum;
    TGF_CAPTURE_MODE_ENT captureMode;
    GT_U32               captureOnTime;
    GT_STATUS            rc = GT_OK;
    CPSS_INTERFACE_INFO_STC inPortInterface;
    CPSS_INTERFACE_INFO_STC outPortInterface;
    TGF_VFD_INFO_STC     *vfdArray = NULL;
    GT_U32               numVfd = 0;
    GT_BOOL              withLoopBack_senderPort,withLoopBack_capturePort;
    GT_U32              currentIndex;

    cmdOsMemSet(&packetInfo, 0,sizeof(packetInfo));
    PARAM_NUMBER(rc, inDevNum, 1, GT_U8);
    PARAM_NUMBER(rc, inPortNum, 2, GT_U32);
    PARAM_STRUCT(rc, &packetInfo, 3, TGF_PACKET_STC);
    PARAM_NUMBER(rc, burstCount, 4, GT_U32);
    PARAM_NUMBER(rc, outDevNum, 5, GT_U8);
    PARAM_NUMBER(rc, outPortNum, 6, GT_U32);
    PARAM_ENUM(rc, captureMode, 7, TGF_CAPTURE_MODE_ENT);
    PARAM_NUMBER(rc, captureOnTime, 8, GT_U32);
    currentIndex = 9;
    if (lua_isboolean(L, currentIndex))
    {
        PARAM_BOOL(rc, withLoopBack_senderPort, currentIndex);
        currentIndex++;
        PARAM_BOOL(rc, withLoopBack_capturePort, currentIndex);
        currentIndex++;
    }
    else
    {
        withLoopBack_senderPort = GT_TRUE;
        withLoopBack_capturePort = GT_TRUE;
    }
    /* gen an optional vfdArray */
    if (lua_istable(L, currentIndex))
    {
        lua_pushcfunction(L, mgmType_to_c_TGF_VFD_INFO_STC_ARR);
        lua_pushvalue(L, currentIndex);
        lua_call(L, 1, 1);
        if (lua_isuserdata(L, -1))
        {
            vfdArray = (TGF_VFD_INFO_STC*)lua_touserdata(L, -1);
            numVfd = lua_objlen(L, currentIndex);
            if (numVfd == 0)
            {
                lua_pop(L, 1);
                vfdArray = NULL;
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "Input parameters");

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "tgfTrafficTableRxPcktTblClear");

    cmdOsMemSet(&outPortInterface, 0, sizeof(outPortInterface));
    cmdOsMemSet(&inPortInterface, 0, sizeof(inPortInterface));
    /* setup receive outPortInterface for capturing */
    outPortInterface.type            = CPSS_INTERFACE_PORT_E;
    outPortInterface.devPort.hwDevNum  = outDevNum;
    outPortInterface.devPort.portNum = outPortNum;
    /* setup tx inPortInterface */
    inPortInterface.type            = CPSS_INTERFACE_PORT_E;
    inPortInterface.devPort.hwDevNum  = inDevNum;
    inPortInterface.devPort.portNum = inPortNum;

    if (withLoopBack_capturePort == GT_FALSE)
    {
        /* need to be called before calling tgfTrafficGeneratorPortTxEthCaptureSet() */
        tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet(GT_FALSE);
    }

    /* enable capture on receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(
            &outPortInterface, captureMode, GT_TRUE);
    PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "tgfTrafficGeneratorPortTxEthCaptureSet");

    if (withLoopBack_capturePort == GT_FALSE)
    {
        /* restore value in case we exit the function */
        tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet(GT_TRUE);
    }

    /* setup transmit params */
    /* enable tracing */
    /*
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, "tgfTrafficTracePacketByteSet");
    */

    /* set the CPU device to send traffic */
    rc = tgfTrafficGeneratorCpuDeviceSet(inDevNum);
    PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "tgfTrafficGeneratorCpuDeviceSet");

    /* register CB */
    rc = tgfTrafficTableNetRxPacketCbRegister(tgfTrafficTableRxPcktReceiveCb);
    if ((GT_OK != rc) && (GT_ALREADY_EXIST != rc))
    {
        PRV_TGF_LOG1_MAC("[TGF]: tgfTrafficTableNetRxPacketCbRegister FAILED, rc = [%d]", rc);
        prv_gc_TGF_PACKET_STC(&packetInfo);
        lua_pushnumber(L, rc);
        return 1;
    }


    /* start transmitting */

    if (withLoopBack_senderPort == GT_TRUE)
    {
        rc = tgfTrafficGeneratorPortLoopbackModeEnableSet(&inPortInterface, GT_TRUE);
        PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "tgfTrafficGeneratorPortLoopbackModeEnableSet enable");
    }

    /* state the TGF to not recognize the 2 bytes after the macSa as 'vlan tag' ...
        unless 'by accident' will have next value .
        NOTE: no need to restore the value since 'we always need it'
     */
    (void)tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);


    /* transmit packet */
    rc = tgfTrafficGeneratorPortTxEthTransmit(&inPortInterface,
                                              &packetInfo,
                                              burstCount,
                                              numVfd,
                                              vfdArray,
                                              0, /* sleepAfterXCount */
                                              0, /* burstSleepTime */
                                              16 /*prvTgfBurstTraceCount*/,
                                              GT_TRUE/*loopbackEnabled*/);

    if (withLoopBack_capturePort == GT_FALSE)
    {
        /* restore behavior */
        tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet(GT_TRUE);
    }

    PRV_LUA_TGF_TR_VERIFY_GT_OK_DEVPORT(rc, "tgfTrafficGeneratorPortTxEthTransmit",
            inPortInterface.devPort.hwDevNum, inPortInterface.devPort.portNum);

    if (withLoopBack_senderPort == GT_TRUE)
    {
        /* disable loopback mode on port */
        (void) tgfTrafficGeneratorPortLoopbackModeEnableSet(&inPortInterface, GT_FALSE);
    }

    cmdOsTimerWkAfter(captureOnTime);

    if (withLoopBack_capturePort == GT_FALSE)
    {
        /* need to be called before calling tgfTrafficGeneratorPortTxEthCaptureSet() */
        tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet(GT_FALSE);
    }
    /* enable capture on receive port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(
            &outPortInterface, captureMode, GT_FALSE);
    PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "tgfTrafficGeneratorPortTxEthCaptureSet");

    if (withLoopBack_capturePort == GT_FALSE)
    {
        /* restore value */
        tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet(GT_TRUE);
    }

    prv_gc_TGF_PACKET_STC(&packetInfo);
    lua_pushnumber(L, rc);
    return 1;
}

/*******************************************************************************
* prvLuaTgfTransmitPackets
*
* DESCRIPTION:
*       transmit packets
*
* INPUTS:
*       devNum        - ingress device number
*       portNum       - ingress port number
*       packetInfoPtr - (pointer to) the packet info
*       burstCount    - number of frames
*                       value 0xFFFFFFFF means 'full wirespeed'
*                       value 0 means 'stop' sending (previous WS)
*       withLoopBack  - enable loopback
*       vfdArray      - optional VFD array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return code
*
* COMMENTS:
*
*******************************************************************************/
int prvLuaTgfTransmitPackets
(
    lua_State* L
)
{
    GT_U8                devNum;
    GT_U32               portNum;
    TGF_PACKET_STC       packetInfo;
    GT_U32               burstCount;
    GT_STATUS            rc = GT_OK;
    CPSS_INTERFACE_INFO_STC portInterface;
    GT_BOOL              withLoopBack = GT_FALSE;
    TGF_VFD_INFO_STC     *vfdArray = NULL;
    GT_U32               numVfd = 0;
    GT_BOOL              isWs = GT_FALSE;
    GT_BOOL              stopTransmit = GT_FALSE;
    GT_BOOL              doTxEthTransmit = GT_TRUE;

    cmdOsMemSet(&packetInfo, 0,sizeof(packetInfo));
    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    PARAM_NUMBER(rc, portNum, 2, GT_U32);
    PARAM_STRUCT(rc, &packetInfo, 3, TGF_PACKET_STC);
    PARAM_NUMBER(rc, burstCount, 4, GT_U32);
    if (lua_isboolean(L, 5))
    {
        PARAM_BOOL(rc, withLoopBack, 5);
    }
    /* gen an optional vfdArray */
    if (lua_istable(L, 6))
    {
        lua_pushcfunction(L, mgmType_to_c_TGF_VFD_INFO_STC_ARR);
        lua_pushvalue(L, 6);
        lua_call(L, 1, 1);
        if (lua_isuserdata(L, -1))
        {
            vfdArray = (TGF_VFD_INFO_STC*)lua_touserdata(L, -1);
            numVfd = lua_objlen(L, 6);
            if (numVfd == 0)
            {
                lua_pop(L, 1);
                vfdArray = NULL;
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    if (burstCount == 0xFFFFFFFF)
    {
        isWs = GT_TRUE;
    }
    else
    if (burstCount == 0)
    {
        stopTransmit = GT_TRUE;
    }

    if(stopTransmit == GT_TRUE)/* stop WS */
    {
        /* stop send Packet from port portNum */
        /* NOTE: this function disable the mac loopback of the port */
        rc = tgfTrafficGeneratorStopWsOnPort(devNum,portNum);

        /* the loopback already disabled in luaTgfStopWsOnPort() */
        withLoopBack = GT_FALSE;
        /* we are not sending packets now */
        doTxEthTransmit = GT_FALSE;
    }
    else
    if(isWs == GT_TRUE) /* start WS */
    {
        burstCount = prvTgfWsBurst;

        rc = tgfTrafficGeneratorWsModeOnPortSet(devNum,portNum);
        PRV_LUA_TGF_TR_VERIFY_GT_OK_DEVPORT(rc, "tgfTrafficGeneratorWsModeOnPortSet",
                devNum,portNum);
    }

    if(doTxEthTransmit == GT_TRUE)
    {
        PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "Input parameters");

        cmdOsMemSet(&portInterface, 0, sizeof(portInterface));
        /* setup receive outPortInterface for capturing */
        portInterface.type            = CPSS_INTERFACE_PORT_E;
        portInterface.devPort.hwDevNum  = devNum;
        portInterface.devPort.portNum = portNum;

        /* setup transmit params */
        /* enable tracing */
        /*
        rc = tgfTrafficTracePacketByteSet(GT_TRUE);
        PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "tgfTrafficTracePacketByteSet");
        */

        /* set the CPU device to send traffic */
        rc = tgfTrafficGeneratorCpuDeviceSet(devNum);
        PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "tgfTrafficGeneratorCpuDeviceSet");

        /* start transmitting */
        if (withLoopBack == GT_TRUE)
        {
            rc = tgfTrafficGeneratorPortLoopbackModeEnableSet(&portInterface, GT_TRUE);
            PRV_LUA_TGF_TR_VERIFY_GT_OK(rc, packetInfo, "tgfTrafficGeneratorPortLoopbackModeEnableSet enable");
        }

        /* state the TGF to not recognize the 2 bytes after the macSa as 'vlan tag' ...
            unless 'by accident' will have next value .
            NOTE: no need to restore the value since 'we always need it'
         */
        (void)tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

        /* transmit packet */
        rc = tgfTrafficGeneratorPortTxEthTransmit(&portInterface,
                                                  &packetInfo,
                                                  burstCount,
                                                  numVfd,
                                                  vfdArray,
                                                  0, /* sleepAfterXCount */
                                                  0, /* burstSleepTime */
                                                  16 /*prvTgfBurstTraceCount*/,
                                                  withLoopBack);

        PRV_LUA_TGF_TR_VERIFY_GT_OK_DEVPORT(rc, "tgfTrafficGeneratorPortTxEthTransmit",
                portInterface.devPort.hwDevNum, portInterface.devPort.portNum);
    }

    if(isWs == GT_TRUE) /* start WS */
    {
        /* the loopback should not be restored when exiting the function.
           we wait until caller will call again to 'stop WS'  */
        withLoopBack = GT_FALSE;
    }

    if (withLoopBack == GT_TRUE)
    {
        /* disable loopback mode on port */
        (void) tgfTrafficGeneratorPortLoopbackModeEnableSet(&portInterface, GT_FALSE);
    }

    prv_gc_TGF_PACKET_STC(&packetInfo);
    lua_pushnumber(L, rc);
    return 1;
}

/*******************************************************************************
* prvLuaTgfRxCapturedPacket
*
* DESCRIPTION:
*       Receive captured packets
*
* INPUTS:
*       outDevNum     - egress device number. If nil, get any captured packet
*       outPortNum    - egress port number. If nil, get any captured packet
*       packetType    - (TGF_PACKET_TYPE_ENT) type
*       getFirst      - get first packet from capture table
*       vfdArray      - optional VFD array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return_code, packet, packet_hexdump, rxParam (TGF_NET_DSA_STC)
*       return_code, match_status  (if vfdArray passed)
*
* COMMENTS:
*
*******************************************************************************/
int prvLuaTgfRxCapturedPacket
(
    lua_State* L
)
{

    GT_U8                    outDevNum = 0;
    GT_U32                   outPortNum = 0;
    TGF_PACKET_TYPE_ENT      packetType;
    GT_BOOL                  getFirst           = GT_TRUE;
    GT_STATUS                rc                 = GT_OK;
    GT_U8                    devNum;
    GT_U8                    queue;
    GT_U8                    packetBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};
    GT_U32                   packetLen          = TGF_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32                   origPacketLen;
    TGF_NET_DSA_STC          rxParam;
    CPSS_INTERFACE_INFO_STC  portInterface;
    TGF_PACKET_STC          *packetInfoPtr;
    TGF_VFD_INFO_STC        *vfdArray            = NULL;
    GT_U32                   numVfd              = 0;
    GT_U32                   useAnyIface         = 0;

    useAnyIface = lua_isnil(L, 1) || lua_isnil(L, 2);

    if (!useAnyIface)
    {
        PARAM_NUMBER(rc, outDevNum, 1, GT_U8);
        PARAM_NUMBER(rc, outPortNum, 2, GT_U32);
    }

    PARAM_ENUM(rc, packetType, 3, TGF_PACKET_TYPE_ENT);
    PARAM_BOOL(rc, getFirst, 4);
    /* gen an optional vfdArray */
    if (lua_istable(L, 5))
    {
        lua_pushcfunction(L, mgmType_to_c_TGF_VFD_INFO_STC_ARR);
        lua_pushvalue(L, 5);
        lua_call(L, 1, 1);
        if (lua_isuserdata(L, -1))
        {
            vfdArray = (TGF_VFD_INFO_STC*)lua_touserdata(L, -1);
            numVfd = lua_objlen(L, 5);
            if (numVfd == 0)
            {
                lua_pop(L, 1);
                vfdArray = NULL;
            }
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    PRV_LUA_TGF_VERIFY_GT_OK(rc, "Input parameters");

    cmdOsMemSet(&portInterface, 0, sizeof(portInterface));

    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.hwDevNum  = outDevNum;
    portInterface.devPort.portNum = outPortNum;

    /* check that packet was not received at the CPU */
    rc = tgfTrafficGeneratorRxInCpuFromExpectedInterfaceGet(
            useAnyIface ? NULL : &portInterface,
            packetType,
            getFirst, GT_TRUE/*trace*/, packetBuff,
            &packetLen, &origPacketLen,
            &devNum, &queue, &rxParam);

    /* return error without printing */
    if (GT_OK != rc)
    {
        lua_pushnumber(L, rc);
        return 1;
    }

    /* use 'out' values only after checking rc == GT_OK !!!! */
    if (origPacketLen < packetLen)
        packetLen = origPacketLen;

    /* if optional vfdArray parameter given then
     * return match status instead of packet
     */
    if (numVfd != 0)
    {
        GT_BOOL match;
        rc = tgfTrafficGeneratorTxEthTriggerCheck(packetBuff, packetLen,
                numVfd, vfdArray, &match);
        if (rc != 0)
            match = GT_FALSE;
        lua_pushnumber(L, rc);
        lua_pushboolean(L, (match == GT_TRUE) ? 1 : 0);
        return 2;
    }

    rc = tgfTrafficEnginePacketParse(packetBuff, packetLen, &packetInfoPtr);
    PRV_LUA_TGF_VERIFY_GT_OK(rc, "tgfTrafficEnginePacketParse");

    lua_pushnumber(L, rc);
    prv_c_to_lua_TGF_PACKET_STC(L, packetInfoPtr);
    /* push packet as hex string */
    {
        GT_U32 i;
        char buf[64];
        size_t l = 0;

        lua_pushliteral(L, "");
        for (i = 0; i < packetLen; i++)
        {
            if (l+3 >= sizeof(buf))
            {
                lua_pushlstring(L, buf, l);
                lua_concat(L, 2);
                l = 0;
            }
            cmdOsSprintf(buf+l, "%02x", packetBuff[i]);
            l += 2;
        }
        if (l > 0)
        {
            lua_pushlstring(L, buf, l);
            lua_concat(L, 2);
        }
    }
    /* push rxParam */
    prv_c_to_lua_TGF_NET_DSA_STC(L, &rxParam);
    return 4;
}

/*******************************************************************************
* prvLuaTgfStateTrgEPortInsteadPhyPort
*
* DESCRIPTION:
*       Option to state that the cpu will send packets to trgEport that is not
*       the 'physical port' (portNum) that used by prvLuaTgfTransmitPackets(..portNum..)
*       prvLuaTgfTransmitPacketsWithCapture(..inPortNum..)
*
*       use enable = GT_TRUE to state to start using trgEPort
*       use enable = GT_FALSE to state to start NOT using trgEPort (trgEPort is ignored)
*
* INPUTS:
*       trgEPort     - egress device number
*       enable       - (GT_BOOL) do we use eport or not
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
int prvLuaTgfStateTrgEPortInsteadPhyPort
(
    lua_State* L
)
{
    GT_STATUS   rc;
    GT_U32                trgEPort;
    GT_BOOL               enable;

    PARAM_NUMBER(rc, trgEPort,  1, GT_U32);
    PARAM_BOOL  (rc, enable,    2);

    rc = tgfStateTrgEPortInsteadPhyPort(trgEPort,enable);

    lua_pushnumber(L, rc);

    return 1;
}

/*******************************************************************************
* prvLuaTgfPortForceLinkUpEnableSet
*
* DESCRIPTION:
*       enable / disable port to 'force linkup' mode
*
* INPUTS:
*       devNum  - the device number
*       portNum - the port number
*       enable  - enable / disable (force/not force)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return code
*
* COMMENTS:
*
*******************************************************************************/
int prvLuaTgfPortForceLinkUpEnableSet
(
    lua_State* L
)
{
    GT_STATUS   rc;
    CPSS_INTERFACE_INFO_STC   portInterface;
    GT_BOOL                   enable;

    PARAM_NUMBER(rc, portInterface.devPort.hwDevNum,  1, GT_U32);
    PARAM_NUMBER(rc, portInterface.devPort.portNum ,  2, GT_U32);
    PARAM_BOOL  (rc, enable,    3);

    portInterface.type = CPSS_INTERFACE_PORT_E;

    rc = tgfTrafficGeneratorPortForceLinkUpEnableSet(&portInterface,enable);

    lua_pushnumber(L, rc);

    return 1;
}

/*******************************************************************************
* prvLuaTgfPortLoopbackModeEnableSet
*
* DESCRIPTION:
*       set port loopback mode
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*       enable        - enable/disable loopback
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return code
*
* COMMENTS:
*
*******************************************************************************/
int prvLuaTgfPortLoopbackModeEnableSet
(
    lua_State* L
)
{
    GT_U8                devNum;
    GT_PORT_NUM                portNum;
    GT_BOOL              enable;
    GT_STATUS            rc = GT_OK;
    CPSS_INTERFACE_INFO_STC portInterface;
    PARAM_NUMBER(rc, devNum,  1, GT_U8);
    PARAM_NUMBER(rc, portNum, 2, GT_PORT_NUM);
    PARAM_BOOL  (rc, enable,  3);

    cmdOsMemSet(&portInterface, 0, sizeof(portInterface));
    /* setup receive outPortInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.hwDevNum  = devNum;
    portInterface.devPort.portNum = portNum;

    rc = tgfTrafficGeneratorPortLoopbackModeEnableSet(&portInterface, enable);

    lua_pushnumber(L, rc);
    return 1;
}

/*******************************************************************************
* prvLuaTgfPortInWsModeInfoGet
*
* DESCRIPTION:
*       get info of port that is in WS mode
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return code , mirror index
*
* COMMENTS:
*
*******************************************************************************/
int prvLuaTgfPortInWsModeInfoGet
(
    lua_State* L
)
{
    GT_STATUS            modeRc;
    GT_U32 mirrIndex;
    GT_U8                devNum;
    GT_U32               portNum;
    GT_STATUS            rc = GT_OK;

    PARAM_NUMBER(rc, devNum,  1, GT_U8);
    PARAM_NUMBER(rc, portNum, 2, GT_U32);

    /* get DB entry  */
    modeRc = prvTgfTxContModeEntryGet(devNum, portNum,NULL, &mirrIndex, NULL);
    if (modeRc != GT_OK)
    {
        /* port in burst mode. */
        rc = GT_NOT_FOUND;
        lua_pushnumber(L, rc);
        return 1;
    }

    rc = GT_OK;
    lua_pushnumber(L, rc);
    lua_pushnumber(L, mirrIndex);
    return 2;
}

/*******************************************************************************
* prvLuaTgfPortsRateGet
*
* DESCRIPTION:
*       get the rate of list of ports, each port will get : max rate ,rx,tx actual rates.
*
* INPUTS:
*       numOfPorts    - number of ports to get info
*       interval      - time between checks (milisec)

*       devNum1       - for port #1 : device number
*       portNum1      - for port #1 : port number
*       sizeOfPacket1 - for port #1 : size of packet (in bytes) include CRC !!!
*       devNum2       - for port #2 : device number
*       portNum2      - for port #2 : port number
*       sizeOfPacket2 - for port #2 : size of packet (in bytes) include CRC !!!

        ...

*       devNum n       - for port #n : device number
*       portNum n      - for port #n : port number
*       sizeOfPacket n - for port #n : size of packet (in bytes) include CRC !!!

*
* OUTPUTS:
*       None
*
* RETURNS:
*       1. return code ,

*       2. port#1 : max rate of the port
*       3. port#1 : rate of actual packets Rx to the port
*       4. port#1 : rate of actual packets Tx from the port

*       . port#2 : max rate of the port
*       . port#2 : rate of actual packets Rx to the port
*       . port#2 : rate of actual packets Tx from the port

        ...

*       . port#n : max rate of the port
*       . port#n : rate of actual packets Rx to the port
*       . port#n : rate of actual packets Tx from the port


*
* COMMENTS:
*
*******************************************************************************/
int prvLuaTgfPortsRateGet
(
    lua_State* L
)
{
    GT_U32               numOfPorts;
    GT_U32               interval;
    GT_STATUS            rc = GT_OK;
    GT_U8   devNumArr[GET_RATE_MAX_PORTS_CNS];
    GT_U32  portNumArr[GET_RATE_MAX_PORTS_CNS];
    GT_U32  txRateArr[GET_RATE_MAX_PORTS_CNS];
    GT_U32  rxRateArr[GET_RATE_MAX_PORTS_CNS];
    GT_U32  sizeOfPacketArr[GET_RATE_MAX_PORTS_CNS];
    GT_U32  wsRateArr[GET_RATE_MAX_PORTS_CNS];
    GT_U32  indexOnStack;
    GT_U32  ii;


    indexOnStack = 1;
    PARAM_NUMBER(rc, numOfPorts,  indexOnStack, GT_U32);
    indexOnStack++;
    PARAM_NUMBER(rc, interval,    indexOnStack, GT_U32);
    indexOnStack++;

    if(numOfPorts > GET_RATE_MAX_PORTS_CNS ||
       numOfPorts == 0)
    {
        /* Error. */
        rc = GT_BAD_SIZE;
        lua_pushnumber(L, rc);
        return 1;
    }

    for(ii = 0 ; ii  < numOfPorts ; ii++)
    {
        PARAM_NUMBER(rc, devNumArr[ii],  indexOnStack, GT_U8);
        indexOnStack++;
        PARAM_NUMBER(rc, portNumArr[ii], indexOnStack, GT_U32);
        indexOnStack++;
        PARAM_NUMBER(rc, sizeOfPacketArr[ii], indexOnStack, GT_U32);
        indexOnStack++;
    }

    /*************************************************/
    /* we are done getting parameters from the stack */
    /*************************************************/

    for(ii = 0 ; ii  < numOfPorts ; ii++)
    {
        /* get port's theoretical wire-speed rate */
        wsRateArr[ii] = prvTgfCommonDevicePortWsRateGet(devNumArr[ii],portNumArr[ii],sizeOfPacketArr[ii]);
        if (0 == wsRateArr[ii])
        {
            /* Error. */
            rc = GT_BAD_STATE;
            lua_pushnumber(L, rc);
            return 1;
        }
    }

    /* get port's actual rate */
    rc = prvTgfCommonPortsCntrRateGet_byDevPort(devNumArr,portNumArr,
        numOfPorts ,interval ,
        CPSS_GOOD_PKTS_SENT_E , txRateArr,
        CPSS_GOOD_PKTS_RCV_E  , rxRateArr);

    indexOnStack = 0;
    lua_pushnumber(L, rc);
    indexOnStack++;

    if (rc != GT_OK)
    {
        return indexOnStack;
    }

    for(ii = 0 ; ii  < numOfPorts ; ii++)
    {
        lua_pushnumber(L, wsRateArr[ii]);
        lua_pushnumber(L, rxRateArr[ii]);
        lua_pushnumber(L, txRateArr[ii]);

        indexOnStack += 3;
    }

    return indexOnStack;
}
