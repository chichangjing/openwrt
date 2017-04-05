/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrafficGenerator.h
*
* DESCRIPTION:
*   - emulate traffic generator capabilities
*    reset counters
*    read counters
*    send traffic
*    capture received traffic
*    set trigger on received traffic
*    ...
*
* FILE REVISION NUMBER:
*       $Revision: 31 $
*
*******************************************************************************/
#ifndef __tgfTrafficGeneratorh
#define __tgfTrafficGeneratorh

#include <cpss/generic/port/cpssPortStat.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/cscd/cpssGenCscd.h>
#include <extUtils/trafficEngine/tgfTrafficEngine.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* default device number */
extern GT_U8    prvTgfDevNum; /* = 0; */
/* do we sent traffic to the CPU ? */
extern GT_U32  tgfTrafficGeneratorRxCaptureNum;
/* indication that a test set traffic to be sent to the CPU , but not relate to 'capture' */
extern GT_BOOL tgfTrafficGeneratorExpectTraficToCpuEnabled;
/* number of packets that returned 'not GT_OK' on send */
extern GT_U32  prvTgfTrafficGeneratorNumberPacketsSendFailed;
/* number of packets that returned 'GT_OK' on send */
extern GT_U32  prvTgfTrafficGeneratorNumberPacketsSendOk;
/* device that is used to send packets from CPU */
extern GT_U8   prvTgfCpuDevNum;
/* flag to use contineouse FWS */
extern GT_U32 prvTgfFwsUse;

/* max ports for API : prvTgfCommonPortsCntrRateGet_byDevPort() */
#define GET_RATE_MAX_PORTS_CNS   128

/******************************************************************************\
 *                          Enumerations definitions                          *
\******************************************************************************/

/*
 * typedef: PRV_TGF_LOOPBACK_UNIT_ENT
 *
 * Description:
 *      In which unit define loopback on port
 *
 * Fields:
 *      PRV_TGF_LOOPBACK_UNIT_MAC_E  make loopback in MAC unit
 *      PRV_TGF_LOOPBACK_UNIT_SERDES_E  make loopback in SerDes unit
 *      PRV_TGF_LOOPBACK_UNIT_MAX_E  for validity checks
 *
 * Comment:
 */

typedef enum{
    PRV_TGF_LOOPBACK_UNIT_MAC_E,
    PRV_TGF_LOOPBACK_UNIT_SERDES_E,
    PRV_TGF_LOOPBACK_UNIT_MAX_E
} PRV_TGF_LOOPBACK_UNIT_ENT;

extern PRV_TGF_LOOPBACK_UNIT_ENT prvTgfLoopbackUnit;

/*
 * typedef: enum TGF_PACKET_TYPE_ENT
 *
 * Description: Enumeration of packet types that receive in the CPU table
 *
 * Enumerations:
 *    TGF_PACKET_TYPE_REGULAR_E - the packet received in the CPU ,
 *          was trapped/mirrored/forwarded due test configuration.
 *    TGF_PACKET_TYPE_CAPTURE_E - the packet received in the CPU ,
 *          was trapped by the 'Cpture' emulation , that we supply in order to
 *          capture traffic that egress a port.
 *    TGF_PACKET_TYPE_ANY_E - any kind regular and capture
 */
typedef enum
{
    TGF_PACKET_TYPE_REGULAR_E,
    TGF_PACKET_TYPE_CAPTURE_E,
    TGF_PACKET_TYPE_ANY_E
} TGF_PACKET_TYPE_ENT;


/*
 * typedef: enum TGF_CAPTURE_MODE_ENT
 *
 * Description: Enumeration of packet capture mode
 *
 * Enumerations:
 *    TGF_CAPTURE_MODE_PCL_E - packet is captured by (ingress) PCL
 *                             (the egress port is defined 'loopback' and
 *                                  after pack looped it is send to CPU by IPCL)
 *
 *    TGF_CAPTURE_MODE_MIRRORING_E - packet is captured by (ingress) mirroring to cpu port
 *                                   (the egress port is defined 'loopback' and
 *                                  after pack looped it is ingress mirrored to the CPU)
 */
typedef enum
{
    TGF_CAPTURE_MODE_PCL_E,
    TGF_CAPTURE_MODE_MIRRORING_E
} TGF_CAPTURE_MODE_ENT;

/*
 * typedef: enum PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT
 *
 * Description:
 *    Type of access to Ingress / Egress PCL configuration Table
 *
 * Enumerators:
 *   PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - by PORT access mode
 *   PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - by VLAN access mode
 *   PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_EPORT_E - by ePORT access mode (12 LSBits)
 *
 */

typedef enum
{
    PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E,
    PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E,
    PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_EPORT_E
} PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT;

/*
 * typedef: PRV_TGF_TX_CONT_MODE_UNIT_ENT
 *
 * Description:
 *      The Tx continuouse mode units type.
 * Fields:
 *      PRV_TGF_TX_CONT_MODE_UNIT_WS_E - wirespeed traffic with port's line rate
 *      PRV_TGF_TX_CONT_MODE_UNIT_KBPS_SEC_E - kilo bits per second rate units
 *      PRV_TGF_TX_CONT_MODE_UNIT_PERCENT_E - percent of line rate units
 *
 * Comment:
 */
typedef enum{
    PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
    PRV_TGF_TX_CONT_MODE_UNIT_KBPS_SEC_E,
    PRV_TGF_TX_CONT_MODE_UNIT_PERCENT_E
}PRV_TGF_TX_CONT_MODE_UNIT_ENT;

/*
    port interface type used by :
        tgfTrafficGeneratorPortTxEthTriggerCountersGet
        tgfTrafficGeneratorPortTxEthCaptureCompare
        tgfTrafficGeneratorRxInCpuFromExpectedInterfaceGet

    PRV_TGF_EXPECTED_INTERFACE_TYPE_PHYSICAL_PORT_NUM_E - physical port number
                                                        used when 'eport' not defined
    PRV_TGF_EXPECTED_INTERFACE_TYPE_EPORT_NUM_E         - eport number
                                                        used when 'eport' defined or
                                                        IMPLICITLY when 'TO_CPU' dsa tag
                                                        not hold enough bits for 'srcPhyPort' so need to use the 'srcEport'
    PRV_TGF_EXPECTED_INTERFACE_TYPE_FORCE_PHYSICAL_PORT_NUM_E -
                                                        force use of physical port number , regardless to 'eport' logic !
*/
typedef enum{
    PRV_TGF_EXPECTED_INTERFACE_TYPE_PHYSICAL_PORT_NUM_E,
    PRV_TGF_EXPECTED_INTERFACE_TYPE_EPORT_NUM_E,

    PRV_TGF_EXPECTED_INTERFACE_TYPE_FORCE_PHYSICAL_PORT_NUM_E

}PRV_TGF_EXPECTED_INTERFACE_TYPE_ENT;




/*
 * typedef: PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC
 *
 * Description: used to describe Analyzer interface.
 *
 * Fields:
 *      interface - Analyzer interface.
 *      Only CPSS_INTERFACE_PORT_E  interface type is supported.
 *
 */
typedef struct
{
    CPSS_INTERFACE_INFO_STC  interface;

}PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC;

/*
 * typedef: struct PRV_TGF_TX_DEV_PORT_STC
 *
 * Description:
 *      continuous mode DB entry.
 *
 * Fields:
 *      devNum    - device number of Tx port
 *      portNum   - port number of Tx port
 *      unitsType - Tx units type
 *      units     - Tx units value
 *      analyzerInf - analyzer interface to restore after stop traffic
 *
 * Comment:
 *
 */
typedef struct{
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    PRV_TGF_TX_CONT_MODE_UNIT_ENT   unitsType;
    GT_U32                          units;
    PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC analyzerInf;
}PRV_TGF_TX_DEV_PORT_STC;


/*
 * typedef: PRV_TGF_TX_MODE_ENT
 *
 * Description:
 *      The Tx mode of packets engine.
 * Fields:
 *    PRV_TGF_TX_MODE_SINGLE_BURST_E - packet engine sends single burst of
 *                                   packets. The prvTgfSetTxSetupEth configures
 *                                   burst size.
 *    PRV_TGF_TX_MODE_CONTINUOUS_E    - packet engine send packets continuously.
 *                                   The prvTgfStopTransmitingEth stops transmission.
 *    PRV_TGF_TX_MODE_CONTINUOUS_FWS_E - packet engine send packets continuously.
 *                                   The prvTgfStopTransmitingEth stops transmission.
 * Comment:
 *   The continuous mode use Rx Mirroring for traffic generation. The source
 *   based mirroring is used. Up to 7 ports may be used to transmission.
 */
typedef enum{
    PRV_TGF_TX_MODE_SINGLE_BURST_E,
    PRV_TGF_TX_MODE_CONTINUOUS_E,
    PRV_TGF_TX_MODE_CONTINUOUS_FWS_E
}PRV_TGF_TX_MODE_ENT;

/* number of packets to get WS traffic. */
extern GT_U32 prvTgfWsBurst;
/* Tx ports maximal amount for continuous mode.
  The limitation is 7 Analyzer interfaces in source based mode. */
#define PRV_TGF_TX_PORTS_MAX_NUM_CNS 7

/******************************************************************************\
 *                               Struct definitions                           *
\******************************************************************************/
#define PRV_TGF_MAX_PORTS_NUM_CNS               20

/* ports array */
extern GT_U32    prvTgfPortsArray[PRV_TGF_MAX_PORTS_NUM_CNS];/* = {0, 8, 18, 23}; */

/*
    structure: PRV_TGF_DEV_PORT_STC
    for dev,port pair
*/
typedef struct{
    GT_U8           devNum;
    GT_PORT_NUM             portNum;   /* in some case the port is 'physical' on others 'ePort' */
}PRV_TGF_DEV_PORT_STC;

/*
    structure: PRV_TGF_MEMBER_FORCE_INFO_STC

    support ring ports that need to be in all vlan/mc groups.
        NOTE: in lion A those port must NOT be in vidx !
    support cascade ports that need to be in all vlan/mc groups.

    fields:
    member - {dev,port} that we refer to.
    forceToVlan - do want to force the member to vlan.
    forceToVidx - do we want to force the member to vidx.
    vlanInfo - info relate only to vlans.
        .tagged - the member tagged or untagged.
            relevant to devices without tr101 support.
        .tagCmd - tag command. the member's tag command.
            relevant to devices with tr101 support.
            hold value of one of : PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT

*/
typedef struct{
    PRV_TGF_DEV_PORT_STC    member;
    GT_BOOL     forceToVlan;
    GT_BOOL     forceToVidx;
    struct{
        GT_BOOL                                        tagged;
        GT_U32 /*PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT*/   tagCmd;
    }vlanInfo;
}PRV_TGF_MEMBER_FORCE_INFO_STC;

/* max number of ports in devices that must be added to all vlans and MC (vidx) groups */
#define PRV_TGF_FORCE_MEMBERS_COUNT_CNS     128

/* list of ports in devices that must be added to all vlans and MC (vidx) groups */
extern PRV_TGF_MEMBER_FORCE_INFO_STC  prvTgfDevPortForceArray[/*PRV_TGF_FORCE_MEMBERS_COUNT_CNS*/];
/* number of used ports/devices in arrays of :prvTgfDevPortForceArray[] */
extern GT_U32    prvTgfDevPortForceNum;



/*
 * typedef: structure TGF_NET_SDMA_TX_PARAMS_STC
 *
 * Description: structure of Tx parameters, that should be set to PP when using
 *              the SDMA channel
 *
 * fields:
 *      recalcCrc - GT_TRUE  - the PP should add CRC to the transmitted packet,
 *                  GT_FALSE - leave packet unchanged.
 *                  NOTE : The DXCH device always add 4 bytes of CRC when need
 *                         to recalcCrc = GT_TRUE
 *
 *
 *      txQueue - the queue that packet should be sent to CPU port. (0..7)
 *
 *      evReqHndl - The application handle got from cpssEventBind for
 *                  CPSS_PP_TX_BUFFER_QUEUE_E events.
 *
 *     invokeTxBufferQueueEvent - invoke Tx buffer Queue event.
 *              when the SDMA copies the buffers of the packet from the CPU ,
 *              the PP may invoke the event of CPSS_PP_TX_BUFFER_QUEUE_E.
 *              this event notify the CPU that the Tx buffers of the packet can
 *              be reused by CPU for other packets. (this in not event of
 *              CPSS_PP_TX_END_E that notify that packet left the SDMA)
 *              The invokeTxBufferQueueEvent parameter control the invoking of
 *              that event for this Packet.
 *              GT_TRUE - PP will invoke the event when buffers are copied (for
 *                        this packet's buffers).
 *              GT_FALSE - PP will NOT invoke the event when buffers are copied.
 *                        (for this packet's buffers).
 *              NOTE :
 *                  when the sent is done as "Tx synchronic" this parameter
 *                  IGNORED (the behavior will be as GT_FALSE)
 */
typedef struct
{
    GT_BOOL                         recalcCrc;
    GT_U8                           txQueue;
    GT_U32                          evReqHndl;
    GT_BOOL                         invokeTxBufferQueueEvent;
} TGF_NET_SDMA_TX_PARAMS_STC;

/*
 * typedef: structure TGF_NET_DSA_STC
 *
 * Description: structure of specific DSA tag parameters
 *
 * fields:
 *      dsaCmdIsToCpu - indicates that the DSA command is 'to CPU'
 *      cpuCode       - CPU codes that indicate the reason for sending
 *                      a packet to the CPU.
 *      srcIsTrunk    - is source Trunk
 *      hwDevNum      - the source HW device number
 *      portNum       - the source port number
 *      originByteCount - The packet's original byte count (TO_CPU packets).
 *      trunkId       - the trunkId , valid when srcIsTrunk = GT_TRUE
 *                      (supported by eDSA info)
 *      ePortNum      -  the ePort number , valid when srcIsTrunk = GT_FALSE
 *                      (supported by eDSA info)
 */
typedef struct
{
    GT_BOOL                     dsaCmdIsToCpu;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;
    GT_BOOL                     srcIsTrunk;
    GT_HW_DEV_NUM               hwDevNum;
    GT_PHYSICAL_PORT_NUM        portNum;
    GT_U32                      originByteCount;
    GT_TRUNK_ID                 trunkId;
    GT_PORT_NUM                 ePortNum;
} TGF_NET_DSA_STC;


/******************************************************************************\
 *                            Public API section                              *
\******************************************************************************/

/*******************************************************************************
* tgfTrafficGeneratorPacketChecksumUpdate
*
* DESCRIPTION:
*       Update checksums in packet built by tgfTrafficEnginePacketBuild
*
* INPUTS:
*       checksumInfoArr -       (array of) info about instance of fields that need auto checksum build
*       checksumExtraInfoPtr -  (array of) extra info for auto checksum build
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*       Input parameters are arrays returened by tgfTrafficEnginePacketBuild.
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPacketChecksumUpdate
(
    OUT TGF_AUTO_CHECKSUM_FULL_INFO_STC *checksumInfoPtr,
    OUT TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
);

/*******************************************************************************
* tgfTrafficGeneratorCpuDeviceSet
*
* DESCRIPTION:
*       Set the CPU device though it we send traffic to the 'Loop back port'
*
* INPUTS:
*       cpuDevice - the CPU device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCpuDeviceSet
(
    IN GT_U8    cpuDevice
);

/*******************************************************************************
* tgfTrafficGeneratorPortLoopbackModeEnableSet
*
* DESCRIPTION:
*       Set port in 'loopback' mode
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (loopback/no loopback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortLoopbackModeEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
);

/*******************************************************************************
* tgfTrafficGeneratorPortForceLinkUpEnableSet
*
* DESCRIPTION:
*       Set port in 'force linkup' mode
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (force/not force)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortForceLinkUpEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
);

/*******************************************************************************
* tgfTrafficGeneratorPortCountersEthReset
*
* DESCRIPTION:
*       Reset the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortCountersEthReset
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr
);

/*******************************************************************************
* tgfTrafficGeneratorPortCountersEthRead
*
* DESCRIPTION:
*       Read the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       countersPtr - (pointer to) the counters of port
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortCountersEthRead
(
    IN  CPSS_INTERFACE_INFO_STC        *portInterfacePtr,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC  *countersPtr
);


/*******************************************************************************
* tgfTrafficGeneratorCheckProcessignDone
*
* DESCRIPTION:
*       Check that all packets were processed by device.
*       The function checks Idle state of device - works on simulation only.
*
* INPUTS:
*       sleepAfterCheck - GT_TRUE - sleep some time after Idle check
*                         GT_FALSE - do not sleep after Idle check
*       loopCount       - number of 1 milisecond sleeps done before call of
*                         the function. Used only if sleepAfterCheck == GT_TRUE
* OUTPUTS:
*       None.
*
* RETURNS:
*       0 - pooling detected that system is still busy
*       other value - number of pooling loops till detection of busy.
* COMMENTS:
*
*******************************************************************************/
GT_U32 tgfTrafficGeneratorCheckProcessignDone
(
    IN GT_BOOL  sleepAfterCheck,
    IN GT_U32   loopCount
);

/*******************************************************************************
* tgfTrafficGeneratorStormingExpected
*
* DESCRIPTION:
*       state that the test doing storming on the ingress port (ingress traffic
*       will be send back to ingress port that is in 'loopback mode').
*       this indication needed for WM to know to break the loop regardless to
*       indications from skernelStatusGet()
*
* INPUTS:
*       stormingExpected - GT_TRUE - storming is expected
*                          GT_FALSE - storming is NOT expected
* OUTPUTS:
*       None.
*
* RETURNS:
*       nothing
* COMMENTS:
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorStormingExpected(IN GT_BOOL stormingExpected);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthTransmit
*
* DESCRIPTION:
*       Transmit the traffic to the port
*       NOTE:
*           1. allow to transmit with chunks of burst:
*              after x sent frames --> do sleep of y millisecond
*              see parameters
*
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       packetInfoPtr    - (pointer to) the packet info
*       burstCount       - number of frames (non-zero value)
*       numVfd           - number of VFDs
*       vfdArray         - pointer to array of VFDs (may be NULL if numVfd = 0)
*       sleepAfterXCount - do 'sleep' after X packets sent
*                          when = 0 , meaning NO SLEEP needed during the burst
*                          of 'burstCount'
*       sleepTime        - sleep time (in milliseconds) after X packets sent , see
*                          parameter sleepAfterXCount
*       traceBurstCount  - number of packets in burst count that will be printed
*       loopbackEnabled  - Check RCV counters when TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The packet will ingress to the PP after 'loopback' and will act as
*       it was received from a traffic generator connected to the port
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthTransmit
(
    IN CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN TGF_PACKET_STC                  *packetInfoPtr,
    IN GT_U32                           burstCount,
    IN GT_U32                           numVfd,
    IN TGF_VFD_INFO_STC                 vfdArray[],
    IN GT_U32                           sleepAfterXCount,
    IN GT_U32                           sleepTime,
    IN GT_U32                           traceBurstCount,
    IN GT_BOOL                          loopbackEnabled
);

/*******************************************************************************
* tgfTrafficGeneratorRxInCpuGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       packetType      - the packet type to get
*       getFirst        - get first/next entry
*       trace           - enable\disable packet tracing
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - length of the copied packet to gtBuf
*       packetLenPtr    - Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr        - Rx queue in which the packet was received.
*       rxParamsPtr     - specific device Rx info format.
*
* RETURNS:
*       GT_OK      - on success
*       GT_NO_MORE - on more entries
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       For 'captured' see API tgfTrafficGeneratorPortTxEthCaptureSet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorRxInCpuGet
(
    IN    TGF_PACKET_TYPE_ENT   packetType,
    IN    GT_BOOL               getFirst,
    IN    GT_BOOL               trace,
    OUT   GT_U8                *packetBufPtr,
    INOUT GT_U32               *packetBufLenPtr,
    OUT   GT_U32               *packetLenPtr,
    OUT   GT_U8                *devNumPtr,
    OUT   GT_U8                *queuePtr,
    OUT   TGF_NET_DSA_STC      *rxParamsPtr
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthTriggerCountersGet
*
* DESCRIPTION:
*       Get number of triggers on port/trunk
*       when 'trunk' implementation actually loops on all enabled members.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port/trunk interface
*       numVfd           - number of VFDs
*       vfdArray         - pointer to array of VFDs
*
* OUTPUTS:
*       numTriggersBmpPtr - (pointer to) bitmap of triggers
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The 'captured' (emulation of capture) must be set on this port/trunk
*       see tgfTrafficGeneratorPortTxEthCaptureSet
*
*       Triggers bitmap has the following structure:
*           N - number of captured packets on this port
*           K - number of VFDs (equal to numVfd)
*           |      Packet0     |      Packet1     |     |       PacketN    |
*           +----+----+---+----+----+----+---+----+ ... +----+----+---+----+
*           |Bit0|Bit1|...|BitK|Bit0|Bit1|...|BitK|     |Bit0|Bit1|...|BitK|
*           +----+----+---+----+----+----+---+----+     +----+----+---+----+
*       Bit[i] for Packet[j] set to 1 means that VFD[i] is matched for captured
*       packet number j.
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthTriggerCountersGet
(
    IN  CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN  GT_U32                           numVfd,
    IN  TGF_VFD_INFO_STC                 vfdArray[],
    OUT GT_U32                          *numTriggersBmpPtr
);

/*******************************************************************************
* tgfTrafficGeneratorTxEthTriggerCheck
*
* DESCRIPTION:
*       Check if the buffer triggered by the trigger parameters
*
* INPUTS:
*       bufferPtr    - (pointer to) the buffer
*       bufferLength - length of the buffer
*       numVfd       - number of VFDs
*       vfdArray     - array of VFDs
* OUTPUTS:
*       triggeredPtr - (pointer to) triggered / not triggered
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The 'captured' (emulation of capture) must be set on this port see
*       tgfTrafficGeneratorPortTxEthCaptureSet, tgfTrafficGeneratorRxInCpuGet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorTxEthTriggerCheck
(
    IN  GT_U8                           *bufferPtr,
    IN  GT_U32                           bufferLength,
    IN  GT_U32                           numVfd,
    IN  TGF_VFD_INFO_STC                 vfdArray[],
    OUT GT_BOOL                         *triggeredPtr
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port/trunk
*       when 'trunk' implementation actually loops on all enabled members.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port/trunk interface
*       mode             - packet capture mode
*       start            - start/stop capture on this port/trunk
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start capture is emulated by setting next for the port:
*           1. loopback on the port (so all traffic that egress the port will
*              ingress the same way)
*           2. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop capture is emulated by setting next for the port:
*           1. disable loopback on the port
*           2. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthCaptureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN TGF_CAPTURE_MODE_ENT      mode,
    IN GT_BOOL                   start
);

/*******************************************************************************
* tgfTrafficGeneratorCaptureLoopTimeSet
*
* DESCRIPTION:
*       Set sleeping time before disabling packet's capturing
*
* INPUTS:
*       timeInMSec - time in miliseconds (0 for restore defaults)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCaptureLoopTimeSet
(
    IN GT_U32   timeInMSec
);

/*******************************************************************************
* tgfTrafficGeneratorEtherTypeForVlanTagSet
*
* DESCRIPTION:
*       set etherType to recognize 'Tagged packes' that send from the traffic
*       generator to the PP
*
* INPUTS:
*       etherType - new etherType
*                   (was initialized as 0x8100)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorEtherTypeForVlanTagSet
(
    IN GT_U16   etherType
);

/*******************************************************************************
* tgfTrafficGeneratorLoopTimeSet
*
* DESCRIPTION:
*       Set sleeping time after sending burst of packets from the CPU
*
* INPUTS:
*       timeInSec - time in seconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorLoopTimeSet
(
    IN GT_U32   timeInSec
);

/*******************************************************************************
* tgfTrafficGeneratorGmTimeFactorSet
*
* DESCRIPTION:
*       Set time factor for GM devices , due to very slow processing of
*       packets (2 per second)
*
* INPUTS:
*       timeFactor - factor for the time
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorGmTimeFactorSet
(
    IN GT_U32   timeFactor
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCapturePclExcludeVidSet
*
* DESCRIPTION:
*       do we need to capture by PCL all packets , exclude those of specific vlan ?
*       this is relevant only when 'PCL capture' used (TGF_CAPTURE_MODE_PCL_E)
*
* INPUTS:
*       excludeEnable - GT_TRUE - exclude
*                       GT_FALSE - no excluding.
*       excludedVid - the vid to exclude .
*                     relevant only when  excludeEnable = GT_TRUE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthCapturePclExcludeVidSet
(
    IN GT_BOOL  excludeEnable,
    IN GT_U16   excludedVid
);

/*******************************************************************************
* tgfTrafficGeneratorRxInCpuNumWait
*
* DESCRIPTION:
*       wait for ALL packets that expected in the CPU, to received.
*
* INPUTS:
*       numOfExpectedPackets - number of packets
*       maxTimeToWait        - MAX time (in milliseconds) to wait for the packets.
*                              The functions will return as soon as all packets
*                              received (but not more then this max time)
*
* OUTPUTS:
*       timeActuallyWaitPtr - time in milliseconds actually waited for the packets.
*                             NOTE: this parameter can be NULL.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_NOT_FOUND  - not get all packets on time
*       GT_BAD_PARAM - wrong numOfExpectedPackets
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorRxInCpuNumWait
(
    IN GT_U32   numOfExpectedPackets,
    IN GT_U32   maxTimeToWait,
    OUT GT_U32  *timeActuallyWaitPtr
);

/*******************************************************************************
* tgfTrafficGeneratorExpectTraficToCpu
*
* DESCRIPTION:
*       state that current test expect traffic to the CPU
*       NOTE: this function should not be called for cases that test uses
*       'capture' to the CPU.
*
* INPUTS:
*       enable - GT_TRUE    - test expect traffic to the CPU.
*                GT_FALSE   - test NOT expect traffic to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorExpectTraficToCpu
(
    IN GT_BOOL  enable
);

/*******************************************************************************
* tgfTrafficGeneratorIngressCscdPortEnableSet
*
* DESCRIPTION:
*            enable/disable the port to assume that ingress traffic hold DSA tag
*            info.
*            NOTE: relevant only to devices that support detach of ingress and
*               egress cascade configurations.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - The port to be configured as cascade enable/disabled
*       enable - enable/disable the ingress DSA tag processing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this API needed to support testing of DSA tag processing by the device
*       (DSA that not processed on the ingress of the CPU_PORT)
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorIngressCscdPortEnableSet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* tgfTrafficGeneratorEgressCscdPortEnableSet
*
* DESCRIPTION:
*            enable/disable the port to assume that egress traffic hold DSA tag
*            info.
*            NOTE: relevant only to devices that support detach of ingress and
*               egress cascade configurations.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - The port to be configured as cascade enable/disabled
*       enable       - enable/disable the ingress DSA tag processing
*       cscdPortType - type of cascade port (APPLICABLE VALUES:
*                                            CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E,
*                                            CPSS_CSCD_PORT_DSA_MODE_4_WORDS_E,
*                                            CPSS_CSCD_PORT_NETWORK_E).
*                      Relevant only when enable is GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this API needed to support testing of DSA tag processing by the device
*       (DSA that not processed on the ingress of the CPU_PORT)
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorEgressCscdPortEnableSet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN GT_BOOL                      enable,
    IN CPSS_CSCD_PORT_TYPE_ENT      cscdPortType
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCaptureCompare
*
* DESCRIPTION:
*       compare captured packets with the input packet format (+VFD array)
*       NOTE: the function not care about reason why packet reached the CPU ,
*             is it due to 'capture' or other reason.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port/trunk interface
*                           NOTE: can be NULL --> meaning 'any interface'
*       packetInfoPtr   - the packet format to compare the captured packet with.
*       numOfPackets    - number of packets expected to be captured on the interface (can be 0)
*       numVfd           - number of VFDs (can be 0)
*       vfdArray         - pointer to array of VFDs (can be NULL when numVfd == 0)
*       byteNumMaskList  - pointer to array of bytes for which the comparison
*                           is prohibited (can be NULL when byteNumMaskListLen == 0)
*       byteNumMaskListLen - number of members in byteNumMaskList(can be 0)
*
* OUTPUTS:
*       actualCapturedNumOfPacketsPtr  - (pointer to) actual number of packet captured from
*                           the interface
*       onFirstPacketNumTriggersBmpPtr - (pointer to) bitmap of triggers - that
*                           relate only to first packet  (can be NULL when numVfd == 0)
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*       Triggers bitmap has the following structure:
*       K - number of VFDs (equal to numVfd)
*           |      Packet0     |
*           +----+----+---+----+
*           |Bit0|Bit1|...|BitK|
*           +----+----+---+----+
*       Bit[i] to 1 means that VFD[i] is matched for first captured packet .
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthCaptureCompare
(
    IN  CPSS_INTERFACE_INFO_STC     *portInterfacePtr,
    IN  TGF_PACKET_STC      *packetInfoPtr,
    IN  GT_U32              numOfPackets,
    IN  GT_U32              numVfd,
    IN  TGF_VFD_INFO_STC    vfdArray[],
    IN  GT_U32              byteNumMaskList[],
    IN  GT_U32              byteNumMaskListLen,
    OUT GT_U32              *actualCapturedNumOfPacketsPtr,
    OUT GT_U32              *onFirstPacketNumTriggersBmpPtr
);

/*******************************************************************************
* tgfTrafficGeneratorRxInCpuFromExpectedInterfaceGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table , for expected interface only.
*       similar to tgfTrafficGeneratorRxInCpuGet , but filtering the packets
*       from non relevant interfaces.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port/trunk interface
*                           NOTE: can be NULL --> meaning 'any interface'
*       packetType      - the packet type to get
*       getFirst        - get first/next entry
*       trace           - enable\disable packet tracing
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - length of the copied packet to gtBuf
*       packetLenPtr    - Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr        - Rx queue in which the packet was received.
*       rxParamsPtr     - specific device Rx info format.
*
* RETURNS:
*       GT_OK      - on success
*       GT_NO_MORE - on more entries
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       For 'captured' see API tgfTrafficGeneratorPortTxEthCaptureSet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorRxInCpuFromExpectedInterfaceGet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN    TGF_PACKET_TYPE_ENT   packetType,
    IN    GT_BOOL               getFirst,
    IN    GT_BOOL               trace,
    OUT   GT_U8                *packetBufPtr,
    INOUT GT_U32               *packetBufLenPtr,
    OUT   GT_U32               *packetLenPtr,
    OUT   GT_U8                *devNumPtr,
    OUT   GT_U8                *queuePtr,
    OUT   TGF_NET_DSA_STC      *rxParamsPtr
);

/*******************************************************************************
* tgfTrafficGeneratorExpectedInterfaceEport
*
* DESCRIPTION:
*       set the expected mode of 'port' to be : ePort / phyPort
*       because the CPSS_INTERFACE_INFO_STC not hold difference between eport and
*       physical port , this setting will give the functions of 'expected' packets
*       to know if expected from the eport or from the physical port.
*
*       NOTEs:
*           1. by defaults : physical port
*           2. after compares done , the default value restored automatically by the engine
* INPUTS:
*       interfaceType - the expected mode of 'port' to be : ePort / phyPort
*                   see enum for more explanations
*                  PRV_TGF_EXPECTED_INTERFACE_TYPE_EPORT_NUM_E  - 'ePort'
*                  PRV_TGF_EXPECTED_INTERFACE_TYPE_PHYSICAL_PORT_NUM_E - 'phyPort'
*                  PRV_TGF_EXPECTED_INTERFACE_TYPE_FORCE_PHYSICAL_PORT_NUM_E - force 'phyPort'
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_TRUE  - packet came from the expected interface
*       GT_FALSE - packet came from unexpected interface
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorExpectedInterfaceEport
(
    IN PRV_TGF_EXPECTED_INTERFACE_TYPE_ENT portInterfaceType
);

/*******************************************************************************
* prvTgfTrafficPrintPacketTxEnableSet
*
* DESCRIPTION:
*       Enable or Disable printing of TX Packet event
*
* INPUTS:
*       printEnable - GT_FALSE - disable
*                     GT_TRUE  - enable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       previous state
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfTrafficPrintPacketTxEnableSet
(
    GT_BOOL printEnable
);

/*******************************************************************************
* prvTgfTrafficPrintPacketTxEnableGet
*
* DESCRIPTION:
*       Returns Enable or Disable printing of TX Packet event
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_FALSE - disable
*       GT_TRUE  - enable
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfTrafficPrintPacketTxEnableGet
(
    GT_VOID
);


/*******************************************************************************
* prvTgfTrafficGeneratorNetIfSyncTxBurstSend
*
* DESCRIPTION:
*       Transmit the traffic to the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       bufferPtr        - (pointer to) the buffer to send
*       bufferLength     - buffer length (include CRC bytes)
*       packetsNumber    - number of packets to send
*       doForceDsa       - GT_FALSE - choose DSA tag automatically
*                          GT_TRUE - use DSA tag defined by forcedDsa
*       forcedDsa        - DSA tag type, used when doForceDsa == GT_TRUE
*       useMii           - use MII
* OUTPUTS:
*       timeMilliPtr      - pointer to duration of TX operation in milliseconds.
*       sendFailPtr       - number of failed TX operations
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficGeneratorNetIfSyncTxBurstSend
(
    IN CPSS_INTERFACE_INFO_STC          *portInterfacePtr,
    IN GT_U8                            *bufferPtr,
    IN GT_U32                            bufferLength,
    IN GT_U32                            packetsNumber,
    IN GT_BOOL                           doForceDsa,
    IN TGF_DSA_TYPE_ENT                  forcedDsa,
    IN GT_BOOL                           useMii,
    OUT double                          *timeMilliPtr,
    OUT GT_U32                          *sendFailPtr
);

/*******************************************************************************
* prvTgfTrafficGeneratorRxToTxInfoSet
*
* DESCRIPTION:
*       Fill in structure for Rx To Tx send packet info
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       vpt              - VPT of the packet
*       cfiBit           - CFI bit
*       vid              - VID of the packet
*       packetIsTagged   - packet is tagged flag
*       doForceDsa       - GT_FALSE - choose DSA tag automatically
*                          GT_TRUE - use DSA tag defined by forcedDsa
*       forcedDsa        - DSA tag type, used when doForceDsa == GT_TRUE
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficGeneratorRxToTxInfoSet
(
    IN CPSS_INTERFACE_INFO_STC            *portInterfacePtr,
    IN GT_U32                              vpt,
    IN GT_U32                              cfiBit,
    IN GT_U32                              vid,
    IN GT_BOOL                             packetIsTagged,
    IN GT_BOOL                             doForceDsa,
    IN TGF_DSA_TYPE_ENT                    forcedDsa
);

/*******************************************************************************
* prvTgfTrafficGeneratorRxToTxSend
*
* DESCRIPTION:
*       Transmit the traffic to predefined port by prvTgfTrafficGeneratorRxToTxInfoSet
*
* INPUTS:
*       devNum           - device number
*       numOfBufs        - number of buffers for packet
*       buffList         - list of buffers
*       buffLenList      - buffer length list (include CRC bytes)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficGeneratorRxToTxSend
(
    IN GT_U8            devNum,
    IN GT_U32           numOfBufs,
    IN GT_U8            *buffList[],
    IN GT_U32           buffLenList[]
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthBurstTransmit
*
* DESCRIPTION:
*       Transmit the traffic burst to the port.
*       Used for TX SDMA performance measurement.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       packetInfoPtr    - (pointer to) the packet info
*       burstCount       - number of frames (non-zero value)
*       doForceDsa       - GT_FALSE - choose DSA tag automatically
*                          GT_TRUE - use DSA tag defined by forcedDsa
*       forcedDsa        - DSA tag type, used when doForceDsa == GT_TRUE
* OUTPUTS:
*       timeMilliPtr      - pointer to duration of TX operation in milliseconds.
*       sendFailPtr       - number of failed TX operations
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthBurstTransmit
(
    IN CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN TGF_PACKET_STC                  *packetInfoPtr,
    IN GT_U32                           burstCount,
    IN GT_BOOL                          doForceDsa,
    IN TGF_DSA_TYPE_ENT                 forcedDsa,
    OUT double                         *timeMilliPtr,
    OUT GT_U32                         *sendFailPtr
);

/*******************************************************************************
* prvTgfCommonMemberForceInfoSet
*
* DESCRIPTION:
*       add member info to DB of 'members to force to vlan/mc groups'
*       in case that the member already exists {dev,port} the other parameters
*       are 'updated' according to last setting.
*
* INPUTS:
*       memberInfoPtr - (pointer to) member info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FULL - the DB is full and no more members can be added -->
*                 need to make DB larger.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfCommonMemberForceInfoSet(
    IN PRV_TGF_MEMBER_FORCE_INFO_STC    *memberInfoPtr
);

/*******************************************************************************
* prvTgfCommonIsDeviceForce
*
* DESCRIPTION:
*       check if the device has ports that used with force configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the device force configuration
*       GT_FALSE - the device NOT force configuration
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvTgfCommonIsDeviceForce(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvTgfPortMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a specified port on specified device.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
);

/*******************************************************************************
* prvTgfDevPortMacCountersClearOnReadSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* INPUTS:
*       devNum
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfDevPortMacCountersClearOnReadSet
(
    IN GT_U8    devNum,
    IN GT_U32    portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfDevPortMacCountersClearOnReadGet
*
* DESCRIPTION:
*       Get status (Enable or disable) of  MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr      - pointer to enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfDevPortMacCountersClearOnReadGet
(
    IN GT_U8    devNum,
    IN GT_U32    portNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* prvTgfBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent entries in the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U32                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
);

/*******************************************************************************
* prvTgfBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidBmpPtr - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U32                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32              *tpidBmpPtr
);

/*******************************************************************************
* tgfTrafficGeneratorSystemReset
*
* DESCRIPTION:
*       Preparation for system reset
*
* INPUTS:
*       None
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorSystemReset
(
    GT_VOID
);

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet
*
* DESCRIPTION:
*       indication for function tgfTrafficGeneratorPortTxEthCaptureSet(...)
*       to allow loopback on the 'captured port' or not.
*
* INPUTS:
*       None
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet
(
    IN GT_BOOL      doLoopback
);

/*******************************************************************************
* prvTgfTxContModeEntryGet
*
* DESCRIPTION:
*       Get entry from transmit continuous mode DB
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       modePtr    - (pointer to) mode entry
*       idxPtr     - (pointer to) entry index
*       freeIdxPtr - (pointer to) free entry index
*
* RETURNS:
*       GT_OK        - on success.
*       GT_NO_SUCH   - on entry not exist.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTxContModeEntryGet
(
    IN GT_U8                            devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT PRV_TGF_TX_DEV_PORT_STC         **modePtr,
    OUT GT_U32                          *idxPtr,
    OUT GT_U32                          *freeIdxPtr
);

/*******************************************************************************
* tgfTrafficGeneratorWsModeOnPortSet
*
* DESCRIPTION:
*       set WS mode on port , by setting rxMirror and setting proper tx mode
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  tgfTrafficGeneratorWsModeOnPortSet(IN GT_U8    devNum, IN GT_U32  portNum);

/*******************************************************************************
* tgfTrafficGeneratorStopWsOnPort
*
* DESCRIPTION:
*       stop WS on port , by disable rxMirror and setting proper tx mode
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  tgfTrafficGeneratorStopWsOnPort(IN GT_U8    devNum, IN GT_U32  portNum);

/*******************************************************************************
* prvTgfCommonDevicePortWsRateGet
*
* DESCRIPTION:
*       Get wire speed rate of port in packets per second
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       packetSize - packet size in bytes including CRC
*
* OUTPUTS:
*       none
*
* RETURNS:
*       wire speed rate of port in packets per second.
*       0xFFFFFFFF - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonDevicePortWsRateGet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U32 packetSize
);
/*******************************************************************************
* prvTgfCommonDiffInPercentCalc
*
* DESCRIPTION:
*       Calculate difference between input values in percent
*
* INPUTS:
*       value1    - value1 to compare
*       value2    - value2 to compare
*
* OUTPUTS:
*       none
*
* RETURNS:
*       difference between input values in percent
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonDiffInPercentCalc
(
    IN  GT_U32 value1,
    IN  GT_U32 value2
);

/*******************************************************************************
* prvTgfCommonPortsCntrRateGet_byDevPort
*
* DESCRIPTION:
*       Get rate of specific ports MAC MIB for up to 2 counters.
*
* INPUTS:
*       devNumArr - array of devNum
*       portNumArr - array of portNum
*       numOfPorts - number of ports
*       cntrName1 - name of first counter
*       timeOut - timeout in milli seconds to calculate rate
*       cntrName2 - name of second counter
*
* OUTPUTS:
*       outRateArr1 - (pointer to) array of rate of ports for cntrName1.
*       outRateArr2 - (pointer to) array of rate of ports for cntrName2.
*                       if NULL then this rate ignored.
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCommonPortsCntrRateGet_byDevPort
(
    IN  GT_U8  devNumArr[],
    IN  GT_U32 portNumArr[],
    IN  GT_U32 numOfPorts,
    IN  GT_U32 timeOut,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName1,
    OUT GT_U32 outRateArr1[],
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName2,
    OUT GT_U32 outRateArr2[]
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfTrafficGeneratorh */

