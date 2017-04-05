/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChRx.c
*
* DESCRIPTION:
*       Cpss DxCh RX handler
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
**************************************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <gtOs/gtOsMsgQ.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>
#include <mainLuaWrapper/wraplNetIf.h>
#include <extUtils/rxEventHandler/rxEventHandler.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>


/************* Externs *******************************************************/


/************* Locals ********************************************************/
static GT_BOOL luaDxChRxCallbackRegistered = GT_FALSE;
static GT_BOOL luaDxChRxConfigured = GT_FALSE;
static GT_MSGQ_ID luaDxChRxMsgQ = 0;
typedef GT_STATUS (*appDemoDbEntryGet_TYPE)
(
    IN  GT_CHAR *namePtr,
    OUT GT_U32  *valuePtr
);
extern appDemoDbEntryGet_TYPE appDemoDbEntryGet_func;
/*******************************************************************************
* luaDxChPktReceive
*
* DESCRIPTION:
*       Function called to handle incoming Rx packet in the CPU
*
* APPLICABLE DEVICES: ALL Devices
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_OK                    - no error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS luaDxChPktReceive
(
    IN GT_UINTPTR                cookie,
    IN RX_EV_HANDLER_EV_TYPE_ENT evType,
    IN GT_U8      devNum,
    IN GT_U8      queueIdx,
    IN GT_U32     numOfBuff,
    IN GT_U8     *packetBuffs[],
    IN GT_U32     buffLen[],
    IN GT_VOID   *rxParamsVoidPtr
)
{
    RX_DATA_STC   rxData;
    GT_U32        ii;
    CPSS_DXCH_NET_RX_PARAMS_STC *rxParamsPtr =
        (CPSS_DXCH_NET_RX_PARAMS_STC*)rxParamsVoidPtr;

    queueIdx = 0;

    /* check luaDxChRx Configured */
    if (luaDxChRxConfigured != GT_TRUE)
        return GT_OK;

    /* TBD: apply filters, resolve msgQId to send packet to */

    cmdOsMemSet(&rxData, 0, sizeof(rxData));

    /* copy packet info */
    rxData.devNum = devNum;
    rxData.vlanId = rxParamsPtr->dsaParam.commonParams.vid;
    rxData.packetIsLooped = rxParamsPtr->dsaParam.commonParams.packetIsLooped;
    switch (rxParamsPtr->dsaParam.dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            if (rxParamsPtr->dsaParam.dsaInfo.toCpu.srcIsTrunk == GT_TRUE)
            {
                rxData.srcIsTrunk = GT_TRUE;
                rxData.srcTrunkId = rxParamsPtr->dsaParam.dsaInfo.toCpu.interface.srcTrunkId;
            }
            else
            {
                GT_U32  value;
                if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) && appDemoDbEntryGet_func &&
                    (appDemoDbEntryGet_func("doublePhysicalPorts", &value) == GT_OK) &&
                    (value != 0))
                {
                    rxData.portNum = rxParamsPtr->dsaParam.dsaInfo.toCpu.interface.ePort;
                }
                else
                {
                    rxData.portNum = rxParamsPtr->dsaParam.dsaInfo.toCpu.interface.portNum;
                }
            }
            rxData.isTagged = rxParamsPtr->dsaParam.dsaInfo.toCpu.isTagged;
            rxData.hasCpuCode = GT_TRUE;
            rxData.cpuCode = rxParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode;
            rxData.wasTruncated = rxParamsPtr->dsaParam.dsaInfo.toCpu.wasTruncated;
            rxData.originByteCount = rxParamsPtr->dsaParam.dsaInfo.toCpu.originByteCount;
            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            /*TBD*/
        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            /*TBD*/
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            /*TBD*/
            break;
    }

    /* copy packet data */
    for(ii = 0; ii < numOfBuff; ii++)
    {
        if (rxData.packet.len + buffLen[ii] > PACKET_MAX_SIZE)
        {
            /* packet length is bigger than destination buffer size */
            cmdOsMemCpy(rxData.packet.data + rxData.packet.len, packetBuffs[ii], PACKET_MAX_SIZE - rxData.packet.len);
            rxData.packet.len = PACKET_MAX_SIZE;
            break;
        }
        cmdOsMemCpy(rxData.packet.data + rxData.packet.len, packetBuffs[ii], buffLen[ii]);
        rxData.packet.len += buffLen[ii];
    }

    /* now send structure to MsgQ */
    /* TBD: send packet to resolved msgQ */
    osMsgQSend(luaDxChRxMsgQ, &rxData, sizeof(rxData), OS_MSGQ_NO_WAIT);
    return GT_OK;
}


/* sample, just forward all packets */
/* RX_DATA_STC must be sent to msgQ */
/*******************************************************************************
* luaDxChRxConfigure
*
* DESCRIPTION:
*       Configure a CB function to send packet info/data to msgQ (first call only)
*       register a CB function to be called on every RX packet to CPU
*
* INPUTS:
*       msgqId   - message queue Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*
*
* COMMENTS:
*******************************************************************************/
GT_STATUS luaDxChRxConfigure
(
    IN GT_MSGQ_ID msgqId
)
{
    luaDxChRxMsgQ = msgqId;
    luaDxChRxConfigured = GT_TRUE;
    if (luaDxChRxCallbackRegistered == GT_FALSE)
    {
        rxEventHandlerAddCallback(
                RX_EV_HANDLER_DEVNUM_ALL,
                RX_EV_HANDLER_QUEUE_ALL,
                RX_EV_HANDLER_TYPE_ANY_E,
                luaDxChPktReceive,
                0);
        luaDxChRxCallbackRegistered = GT_TRUE;
    }
    return GT_OK;
}

int wrlDxChRxConfigure(lua_State *L)
{
    GT_MSGQ_ID msgqId;
    GT_STATUS rc = GT_OK;
    PARAM_NUMBER(rc, msgqId, 1, GT_MSGQ_ID);
    if (rc == GT_OK)
    {
        rc = luaDxChRxConfigure(msgqId);
    }
    lua_pushinteger(L, (lua_Integer)rc);
    return 1;
}

/*******************************************************************************
* luaDxChRxDisable
*
* DESCRIPTION:
*       Disable RX callback handler
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*
*
* COMMENTS:
*******************************************************************************/
GT_STATUS luaDxChRxDisable(void)
{
    luaDxChRxConfigured = GT_FALSE;
    return GT_OK;
}

int wrlDxChRxDisable(lua_State *L)
{
    GT_STATUS rc = GT_OK;
    rc = luaDxChRxDisable();
    lua_pushinteger(L, (lua_Integer)rc);
    return 1;
}

/*******************************************************************************
* wrlDxChNetIfCpuToDsaCode
*
* DESCRIPTION:
*       Convert CPU Code from CPDD values to HW dependend DSA tag value
*
* INPUTS:
*       CPU Code CPSS value
*
* OUTPUTS:
*       CPU Code HW DSA tag value
*
* RETURNS:
*
* COMMENTS:
*******************************************************************************/
int wrlDxChNetIfCpuToDsaCode(lua_State *L)
{
    CPSS_NET_RX_CPU_CODE_ENT               cpuCode;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;

    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)lua_tonumber(L, 1);
    prvCpssDxChNetIfCpuToDsaCode(
        cpuCode, &dsaCpuCode);
    lua_pushinteger(L, (lua_Integer)dsaCpuCode);
    return 1;
}

