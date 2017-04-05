/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChTmGlueQueueMap.h
*
* DESCRIPTION:
*       Traffic Manager Glue - Packet Queue Id mapping.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

#ifndef __cpssDxChTmGlueQueueMaph
#define __cpssDxChTmGlueQueueMaph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>

/* amount of bits in TM Queue Id */
#define CPSS_DXCH_TM_GLUE_QUEUE_ID_BIT_WIDTH_CNS 14

/*
 * typedef: CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT
 *
 * Description:
 *      Traffic Manager Queue Id Bit Select type.
 *
 * Enumerations:
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ZERO_E -
 *         set bit to zero anycase.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_EPCL_QUEUE_ID_E -
 *         take bit from Egress PCL Acion Field "Queue Id".
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_FLOW_ID_E -
 *         take bit from packet flow Id (set by TTI, IPCL, EPCL).
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_LOCAL_TARGET_PHY_PORT_E -
 *         take bit from Local Device Target Pysical Port Number.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E -
 *         take bit from Packet TM (Mapped) Traffic Class.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_PACKET_HASH_E -
 *         take bit from Packet Hash.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_DEV_E -
 *         take bit from Target Device Number.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_PHY_PORT_E -
 *         take bit from Target Device Target Pysical Port Number.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_EPORT_E -
 *         take bit from Target ePort Number.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ONE_E -
 *         set bit to one anycase.
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ZERO_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_EPCL_QUEUE_ID_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_FLOW_ID_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_LOCAL_TARGET_PHY_PORT_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_PACKET_HASH_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_DEV_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_PHY_PORT_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_EPORT_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ONE_E
} CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT;

/*
 * typedef: struct CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC
 *
 * Description:
 *      Traffic Manager Queue Id Bit Select data.
 *
 * Fields:
 *      selectType   - type of value to select bit from it.
 *      bitSelector  - index of bit to select from value.
 *                     (APPLICABLE RANGES: 0..15).
 *
 * Comment:
 */
typedef struct
{
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT selectType;
    GT_U32                                          bitSelector;
} CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC;


/*
 * typedef: struct CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC
 *
 * Description:
 *      Traffic Manager Queue Id Bit Select Table entry.
 *
 * Fields:
 *      queueIdBase    - base value to be added to queue Id combined from bits.
 *      bitSelectArr   - array of bit selectors for all 14 bits of Queue Id.
 *
 * Comment:
 *      The Queue Id calculation algorithm consists of
 *      - selection of each of 14 bits according to configuration
 *      - combining the Queue Id from these bits
 *      - adding Queue Id Base value to calculated value
 */
typedef struct
{
    GT_U32                                     queueIdBase;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_STC bitSelectArr[CPSS_DXCH_TM_GLUE_QUEUE_ID_BIT_WIDTH_CNS];
} CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC;

/*
 * typedef: CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT
 *
 * Description:
 *      TO_CPU packet mapping to queue mode.
 *
 * Enumerations:
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_REGULAR_E -
 *         Use regular index for accessing queue id bits selector index.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_USE_TO_CPU_CONFIG_E -
 *         Use TO_CPU Queue Bit Selector configuration.
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_REGULAR_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_USE_TO_CPU_CONFIG_E
} CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT;

/*
 * typedef: CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT
 *
 * Description:
 *      Multicast packet mapping to queue mode.
 *
 * Enumerations:
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_REGULAR_E -
 *         Use regular index for accessing the queue selector index.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_FORCE_EPCL_CONFIG_E -
 *         Force EPCL Queue-ID.
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_REGULAR_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_FORCE_EPCL_CONFIG_E
} CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT;

/*
 * typedef: CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT
 *
 * Description:
 *      Accessing to Queue Bit Selector mode.
 *
 * Enumerations:
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_LOCAL_TARGET_PORT_E -
 *         Use the Target Local Port[7:0] as Queue Bit Select Table index.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_TARGET_DEV_MAP_E -
 *         Use the Target Device map table output as Queue Bit Select Table index.
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_LOCAL_TARGET_PORT_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_TARGET_DEV_MAP_E
} CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT;

/*
 * typedef: CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT
 *
 * Description:
 *      Egress L1 Packet Length Offset Command.
 *      The updated length used by TM for Scaduling and Shating.
 *
 * Enumerations:
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ADD_E -
 *         Add specified value to L1 Packet Length.
 *   CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_SUBTRACT_E -
 *         Subtract specified value from L1 Packet Length.
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ADD_E,
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_SUBTRACT_E
} CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT;

/*
 * typedef: struct CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC
 *
 * Description:
 *      Traffic Manager Egress L1 Packet Length Offset Configuration.
 *
 * Fields:
 *      offsetCmd   - L1 Packet Length Offset Command.
 *      offsetValue - L1 Packet Length Bytes Offset value.
 *                     (APPLICABLE RANGES: 0..0x7F).
 *
 * Comment:
 */
typedef struct
{
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT offsetCmd;
    GT_U32                                                      offsetValue;
} CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC;

/*******************************************************************************
* cpssDxChTmGlueQueueMapBypassEnableSet
*
* DESCRIPTION:
*   The function sets enable/disable state of Bypass TM Queue Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       bypass             - GT_TRUE - bypass, GT_FALSE - mapping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
* 
* Bypassing TM Queue Mapping should not be activated on TM enabled systems.
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapBypassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   bypass
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapBypassEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable state of Bypass TM Queue Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       bypassPtr          - (pointer to)GT_TRUE - bypass, GT_FALSE - mapping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapBypassEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *bypassPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapBitSelectTableEntrySet
*
* DESCRIPTION:
*   The function sets the Queue Id Map Bit Select Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - entry index
*                        (APPLICABLE RANGES: 0..255).
*       entryPtr       - (pointer to) entry contents
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapBitSelectTableEntrySet
(
    IN  GT_U8                                              devNum,
    IN  GT_U32                                             entryIndex,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapBitSelectTableEntryGet
*
* DESCRIPTION:
*   The function gets the Queue Id Map Bit Select Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - entry index
*                        (APPLICABLE RANGES: 0..255).
*
* OUTPUTS:
*       entryPtr       - (pointer to) entry contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on unexpected HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapBitSelectTableEntryGet
(
    IN  GT_U8                                              devNum,
    IN  GT_U32                                             entryIndex,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet
*
* DESCRIPTION:
*   The function sets the Queue Id Map Bit Select Table Entry index
*   mapped to the given target device.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       targetHwDevId  - target HW device Id
*                        (APPLICABLE RANGES: 0..1023).
*       entryIndex     - index of Queue Id Map Bit Select Table Entry
*                        being mapped to the given target device.
*                        (APPLICABLE RANGES: 0..255).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet
(
    IN  GT_U8           devNum,
    IN  GT_HW_DEV_NUM   targetHwDevId,
    IN  GT_U32          entryIndex
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet
*
* DESCRIPTION:
*   The function gets the Queue Id Map Bit Select Table Entry index
*   mapped to the given target device.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       targetHwDevId  - target HW device Id
*                        (APPLICABLE RANGES: 0..1023).
*
* OUTPUTS:
*       entryIndexPtr  - (pointer to)index of Queue Id Map Bit Select Table Entry
*                        being mapped to the given target device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet
(
    IN  GT_U8           devNum,
    IN  GT_HW_DEV_NUM   targetHwDevId,
    OUT GT_U32          *entryIndexPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapCpuCodeToTcMapSet
*
* DESCRIPTION:
*   The function sets the mapped TM Traffic Class for CPU Code.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       cpuCode        - CPU code
*                        CPSS_NET_ALL_CPU_OPCODES_E - to configure all table entries.
*       tmTc           - mapped TM Traffic Class.
*                        (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapCpuCodeToTcMapSet
(
    IN  GT_U8                      devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT   cpuCode,
    IN  GT_U32                     tmTc
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapCpuCodeToTcMapGet
*
* DESCRIPTION:
*   The function gets the mapped TM Traffic Class for CPU Code.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       cpuCode        - CPU code
*
* OUTPUTS:
*       tmTcPtr        - (pointer to) mapped TM Traffic Class.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapCpuCodeToTcMapGet
(
    IN  GT_U8                      devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT   cpuCode,
    OUT GT_U32                     *tmTcPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapTcToTcMapSet
*
* DESCRIPTION:
*   The function sets mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       packetDsaCommand  - packet Outgoing DSA Tag command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*       tc                - Traffic Class
*                           (APPLICABLE RANGES: 0..7).
*       tmTc              - Traffic Manager Traffic Class
*                           (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapTcToTcMapSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand,
    IN  GT_BOOL                     isUnicast,
    IN  GT_U32                      tc,
    IN  GT_U32                      tmTc
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapTcToTcMapGet
*
* DESCRIPTION:
*   The function gets mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       packetDsaCommand  - packet Outgoing DSA Tag command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*       tc                - Traffic Class
*                           (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       tmTcPtr        - (pointer to)Traffic Manager Traffic Class
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapTcToTcMapGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand,
    IN  GT_BOOL                     isUnicast,
    IN  GT_U32                      tc,
    OUT GT_U32                      *tmTcPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapEgressPolicerForceSet
*
* DESCRIPTION:
*   The function sets force state for TM queue Id based policing.
*   If force the previous policer settings of the packet overridden.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       queueId        - Traffic Manager Queue Id
*                        (APPLICABLE RANGES: 0..0x3FFF).
*       forceMeter     - force meter,
*                        GT_TRUE - force, GT_FALSE - don't force.
*       forceCounting  - force counting
*                        GT_TRUE - force, GT_FALSE - don't force.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapEgressPolicerForceSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   queueId,
    IN  GT_BOOL  forceMeter,
    IN  GT_BOOL  forceCounting
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapEgressPolicerForceGet
*
* DESCRIPTION:
*   The function gets force state for TM queue Id based policing.
*   If force the previous policer settings of the packet overridden.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       queueId        - Traffic Manager Queue Id
*                        (APPLICABLE RANGES: 0..0x3FFF).
*
* OUTPUTS:
*       forceMeterPtr     - (pointer to)force meter
*                        GT_TRUE - force, GT_FALSE - don't force.
*       forceCountingPtr  - (pointer to)force counting
*                        GT_TRUE - force, GT_FALSE - don't force.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapEgressPolicerForceGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   queueId,
    OUT GT_BOOL  *forceMeterPtr,
    OUT GT_BOOL  *forceCountingPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapToCpuModeSet
*
* DESCRIPTION:
*   The function sets mode of index’s calculation in Queue Id Bit Select Table
*   for TO CPU packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       toCpuMode          - mode of index’s calculation in
*                            Queue Id Bit Select Table for TO CPU packets.
*       toCpuSelectorIndex - index in Queue Id Bit Select Table
*                            for TO CPU packets.
*                            Always stored to HW, but used only when
*                            toCpuMode is "Use TO_CPU Queue Bit Selector configuration".
*                            (APPLICABLE RANGES: 0..0xFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapToCpuModeSet
(
    IN  GT_U8                                         devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT   toCpuMode,
    IN  GT_U32                                        toCpuSelectorIndex
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapToCpuModeGet
*
* DESCRIPTION:
*   The function gets mode of index’s calculation in Queue Id Bit Select Table
*   for TO CPU packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       toCpuModePtr          - (pointer to)mode of index’s calculation in
*                               Queue Id Bit Select Table for TO CPU packets.
*       toCpuSelectorIndexPtr - (pointer to)index in Queue Id Bit Select Table
*                               for TO CPU packets.
*                               Always retrieved from HW, but used only when
*                               toCpuMode is "Use TO_CPU Queue Bit Selector configuration".
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapToCpuModeGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT   *toCpuModePtr,
    OUT GT_U32                                        *toCpuSelectorIndexPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapMcModeSet
*
* DESCRIPTION:
*   The function sets mode of index’s calculation in Queue Id Bit Select Table
*   for Multicast packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       mcMode          - mode of index’s calculation in
*                         Queue Id Bit Select Table for Multicast packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapMcModeSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT   mcMode
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapMcModeGet
*
* DESCRIPTION:
*   The function gets mode of index’s calculation in Queue Id Bit Select Table
*   for Multicast packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       mcModePtr       - (pointer to)mode of index’s calculation in
*                         Queue Id Bit Select Table for Multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapMcModeGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT   *mcModePtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapSelectorTableAccessModeSet
*
* DESCRIPTION:
*   The function sets Bit Selector Table Access Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       mode    - Bit Selector Table Access Mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapSelectorTableAccessModeSet
(
    IN  GT_U8                                                 devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapSelectorTableAccessModeGet
*
* DESCRIPTION:
*   The function gets Bit Selector Table Access Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to)Bit Selector Table Access Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapSelectorTableAccessModeGet
(
    IN  GT_U8                                                 devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet
*
* DESCRIPTION:
*   The function sets Egress L1 Packet Length Offset Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       physicalPort    - physical port number.
*       lengthOffsetPtr - (pointer to)Egress L1 Packet Length Offset Configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet
(
    IN  GT_U8                                                    devNum,
    IN  GT_PHYSICAL_PORT_NUM                                     physicalPort,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC  *lengthOffsetPtr
);

/*******************************************************************************
* cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet
*
* DESCRIPTION:
*   The function gets Egress L1 Packet Length Offset Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       physicalPort    - physical port number.
*
* OUTPUTS:
*       lengthOffsetPtr - (pointer to)Egress L1 Packet Length Offset Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet
(
    IN  GT_U8                                                    devNum,
    IN  GT_PHYSICAL_PORT_NUM                                     physicalPort,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC  *lengthOffsetPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenCscdh */


