/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChDiagPacketGenerator.h
*
* DESCRIPTION:
*       Diag Packet Generator APIs for CPSS DxCh.
*
* Sequence for generating packets on port:
*  1. Connect the port to packet generator and set configurations
*       (cpssDxChDiagPacketGeneratorConnectSet)
*  2. Start transmission (cpssDxChDiagPacketGeneratorTransmitEnable)
*  3. If single burst transmit mode used: 
*        Check transmission done status (cpssDxChDiagPacketGeneratorBurstTransmitStatusGet)
*     If continues transmit mode used:
*        Stop transmission (cpssDxChDiagPacketGeneratorTransmitEnable)
*  4. Return the port to normal operation, disconnect the packet generator
*       (cpssDxChDiagPacketGeneratorConnectSet)
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#ifndef __cpssDxChDiagPacketGeneratorh
#define __cpssDxChDiagPacketGeneratorh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * Typedef: CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT
 *
 * Description: Enumeration of packet's payload types.
 *
 * Enumerations:
 *      CPSS_DIAG_PG_PACKET_PAYLOAD_CYCLIC_E  - The packet's payload is cyclic pattern
 *      CPSS_DIAG_PG_PACKET_PAYLOAD_RANDOM_E  - The packet's payload is random
 *      CPSS_DIAG_PG_PACKET_PAYLOAD_CONSTANT_E  - The packet's payload is constant
 */
typedef enum
{
    CPSS_DIAG_PG_PACKET_PAYLOAD_CYCLIC_E = 0,
    CPSS_DIAG_PG_PACKET_PAYLOAD_RANDOM_E,
    CPSS_DIAG_PG_PACKET_PAYLOAD_CONSTANT_E
}CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT;

/*
 * Typedef: CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT
 *
 * Description: Enumeration of packet's length types.
 *
 * Enumerations:
 *      CPSS_DIAG_PG_PACKET_LENGTH_CONSTANT_E  - The packet's length is constant
 *      CPSS_DIAG_PG_PACKET_LENGTH_RANDOM_E  - The packet's length is random
 */
typedef enum
{
    CPSS_DIAG_PG_PACKET_LENGTH_CONSTANT_E = 0,
    CPSS_DIAG_PG_PACKET_LENGTH_RANDOM_E
}CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT;

/*
 * Typedef: CPSS_DIAG_PG_TRANSMIT_MODE_ENT
 *
 * Description: Enumeration of packet generator transmit modes.
 *
 * Enumerations:
 *      CPSS_DIAG_PG_TRANSMIT_CONTINUES_E  - continues transmit
 *      CPSS_DIAG_PG_TRANSMIT_SINGLE_BURST_E  - single burst transmit
 */
typedef enum
{
    CPSS_DIAG_PG_TRANSMIT_CONTINUES_E = 0,
    CPSS_DIAG_PG_TRANSMIT_SINGLE_BURST_E

}CPSS_DIAG_PG_TRANSMIT_MODE_ENT;

/*
 * Typedef: CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT
 *
 * Description: Enumeration of packet count multiplier.
 *
 * Enumerations:
 *      CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1_E  - multiply by 1
 *      CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_256_E  - multiply  by 256
 *      CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_512_E  - multiply by 512
 *      CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1K_E - multiply by 1K (1024)
 *      CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_4K_E  - multiply by 4K (4096)
 *      CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_64K_E  - multiply by 64K (65536)
 *      CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1M_E  - multiply by 1M (1048576)
 *      CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_16M_E  - multiply by 16M (16777216)
 */
typedef enum
{
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1_E = 0,
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_256_E,
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_512_E,
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1K_E,
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_4K_E,
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_64K_E,
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1M_E,
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_16M_E
}CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT;

/*
 * Typedef: CPSS_DIAG_PG_IF_SIZE_ENT
 *
 * Description: interface size (what is the word width to the port, instead of the TxDMA).
 *
 * Enumerations:
 *      CPSS_DIAG_PG_IF_SIZE_1_BYTE_E   - 8 bits
 *      CPSS_DIAG_PG_IF_SIZE_8_BYTES_E  - 64 bits
 *      CPSS_DIAG_PG_IF_SIZE_16_BYTES_E - 128 bits
 *      CPSS_DIAG_PG_IF_SIZE_32_BYTES_E - 256 bits
 *      CPSS_DIAG_PG_IF_SIZE_64_BYTES_E - 512 bits
 *      CPSS_DIAG_PG_IF_SIZE_DEFAULT_E  - determine one of CPSS_DIAG_PG_IF_SIZE_1_BYTE_E : CPSS_DIAG_PG_IF_SIZE_64_BYTES_E 
 *                                        based on port mac type and speed
 *      CPSS_DIAG_PG_IF_SIZE_NOT_APPLICABLE_E  -- the feature is not supported at this processor (used at Get function)
 *      
 */
typedef enum 
{
     CPSS_DIAG_PG_IF_SIZE_1_BYTE_E    = 0x0
    ,CPSS_DIAG_PG_IF_SIZE_8_BYTES_E   = 0x1
    ,CPSS_DIAG_PG_IF_SIZE_16_BYTES_E  = 0x2
    ,CPSS_DIAG_PG_IF_SIZE_32_BYTES_E  = 0x3
    ,CPSS_DIAG_PG_IF_SIZE_64_BYTES_E  = 0x4
    ,CPSS_DIAG_PG_IF_SIZE_DEFAULT_E   = 0xFF
    ,CPSS_DIAG_PG_IF_SIZE_NOT_APPLICABLE_E   = 0xFFFF
}CPSS_DIAG_PG_IF_SIZE_ENT;

/*
 * typedef: struct CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC
 *
 * Description: Packet generator's configurations.
 *
 * Fields:
 *      macDa                - Ethernet Destination MAC address.
 *      macDaIncrementEnable - Enables MAC DA incremental for every transmitted packet.
 *                             GT_FLASE - MAC DA is constant for all packets
 *                             GT_TRUE - MAC DA is incremented for every packet.
 *      macSa                - Ethernet Source MAC address.
 *      vlanTagEnable        - Enables insertion of vlan tag.
 *                             GT_FLASE - vlan tag is not inserted.
 *                             GT_TRUE - vlan tag is inserted after MAC SA.
 *      vpt                  - The VPT of the VLAN tag added to packet. (APPLICABLE RANGES: 0..7)
 *                             (relevant only if vlanTagEnable == GT_TRUE)
 *      cfi                  - The CFI of the VLAN tag added to packet. (APPLICABLE RANGES: 0..1)
 *                             (relevant only if vlanTagEnable == GT_TRUE)
 *      vid                  - The VLAN ID of the VLAN tag added to packet. (APPLICABLE RANGES: 1..4095)
 *                             (relevant only if vlanTagEnable == GT_TRUE)
 *      etherType            - The etherType to be sent in the packet after the VLAN tag(if any).
 *                             (APPLICABLE RANGES: 0..0xFFFF)
 *      payloadType          - The payload type to be palced after etherType.
 *      cyclicPatternArr     - The cyclic pattern to be palced after etherType (network order).
 *                             (relevant only if payloadType == CPSS_DIAG_PG_PACKET_PAYLOAD_CYCLIC_E)
 *      packetLengthType     - The packet length type.
 *      packetLength         - The packet length without CRC. (APPLICABLE RANGES: 20..16383)
 *                             (relevant only if packetLengthType == CPSS_DIAG_PG_PACKET_LENGTH_CONSTANT_E)
 *      undersizeEnable      - Enables undersized packets transmission.
 *                             (relevant only if packetLengthType == CPSS_DIAG_PG_PACKET_LENGTH_RANDOM_E)
 *      transmitMode         - Packet generator transmit mode.
 *      packetCount          - Packet count to transmit in single burst. (APPLICABLE RANGES: 1..8191)
 *                             The total packets number to transmit is:
 *                             (packetCount) * (packetCountMultiplier value).
 *                             (relevant only if transmitMode == CPSS_DIAG_PG_TRANSMIT_SINGLE_BURST_E)
 *      packetCountMultiplier- Packet count packet count multiplier.
 *                             The total packets number to transmit is:
 *                             (packetCount) * (packetCountMultiplier value).
 *                             (relevant only if transmitMode == CPSS_DIAG_PG_TRANSMIT_SINGLE_BURST_E)
 *      ipg                  - Interpacket gap in bytes. (APPLICABLE RANGES: 0..0xFFFF)
 *      interfaceSize        - Interface size (what is the word width to the port, instead of the TxDMA, BC2 only).
 * 
 */
typedef struct {
    GT_ETHERADDR                             macDa;
    GT_BOOL                                  macDaIncrementEnable;
    GT_U32                                   macDaIncrementLimit;
    GT_ETHERADDR                             macSa;
    GT_BOOL                                  vlanTagEnable;
    GT_U8                                    vpt;
    GT_U8                                    cfi;
    GT_U16                                   vid;
    GT_U16                                   etherType;
    CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT     payloadType;
    GT_U8                                    cyclicPatternArr[8];
    CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT      packetLengthType;
    GT_U32                                   packetLength;
    GT_BOOL                                  undersizeEnable;
    CPSS_DIAG_PG_TRANSMIT_MODE_ENT           transmitMode;
    GT_U32                                   packetCount;
    CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT packetCountMultiplier;
    GT_U32                                   ipg;
    CPSS_DIAG_PG_IF_SIZE_ENT                 interfaceSize;
} CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC;


/*******************************************************************************
* cpssDxChDiagPacketGeneratorConnectSet
*
* DESCRIPTION:
*       Connect/Disconnect port to packet generator.
*       Set packet generator's configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number to connect 
*                         its MAC to packet generator.
*       connect         - GT_TRUE: connect the given port's MAC to packet generator.
*                         GT_FALSE: disconnect the given port's MAC from 
*                         packet generator; return the port's MAC
*                         to normal egress pipe.
*       configPtr       - (pointer to) packet generator configurations.
*                         Relevant only if enable==GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. In case packet generator connected to other port the function perfroms
*          the following:
*          - stops traffic.
*          - connects packet generator to new port
*          - overrides packet generator's configurations
*       2. Before enabling the packet generator, port must be initialized,
*          i.e. port interface and speed must be set.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPacketGeneratorConnectSet
(
    IN GT_U8                                 devNum,
    IN GT_PHYSICAL_PORT_NUM                  portNum,
    IN GT_BOOL                               connect,
    IN CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC *configPtr
);

/*******************************************************************************
* cpssDxChDiagPacketGeneratorConnectGet
*
* DESCRIPTION:
*       Get the connect status of specified port.
*       Get packet generator's configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number to connect 
*                         its MAC to packet generator.
*
* OUTPUTS:
*       connectPtr      - (pointer to)
*                         GT_TRUE: port connected to packet generator.
*                         GT_FALSE: port not connected to packet generator.
*       configPtr       - (pointer to) packet generator configurations.
*                         Relevant only if enable==GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPacketGeneratorConnectGet
(
    IN  GT_U8                                 devNum,
    IN  GT_PHYSICAL_PORT_NUM                  portNum,
    OUT GT_BOOL                              *connectPtr,
    OUT CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC *configPtr
);

/*******************************************************************************
* cpssDxChDiagPacketGeneratorTransmitEnable
*
* DESCRIPTION:
*       Enable/Disable(Start/Stop) transmission on specified port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       enable          - GT_TRUE: enable(start) transmission
*                         GT_FALSE: disable(stop) transmission
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_STATE             - on port not connected to packet generator
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Before calling this function the port must be connected to packet 
*          generator (cpssDxChDiagPacketGeneratorConnectSet), 
*          otherwise GT_BAD_STATE is returned.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPacketGeneratorTransmitEnable
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChDiagPacketGeneratorBurstTransmitStatusGet
*
* DESCRIPTION:
*       Get burst transmission status on specified port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*
* OUTPUTS:
*       burstTransmitDonePtr - (pointer to) burst transmit done status
*                               GT_TRUE: burst transmission done
*                               GT_FALSE: burst transmission not done
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on port not connected to packet generator
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Burst transmission status is clear on read.
*       2. Before calling this function the port must be connected to packet 
*          generator (cpssDxChDiagPacketGeneratorConnectSet), 
*          otherwise GT_BAD_STATE is returned.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPacketGeneratorBurstTransmitStatusGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL              *burstTransmitDonePtr
);


/*******************************************************************************
* prvCpssDxChMacTGDrvInit
*
* DESCRIPTION:
*       Init Mac TG driver
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChMacTGDrvInit
(
    IN    GT_U8                   devNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChDiagPacketGeneratorh */
